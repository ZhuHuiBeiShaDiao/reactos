/*
 * PROJECT:         ReactOS Kernel
 * LICENSE:         GPL - See COPYING in the top level directory
 * FILE:            ntoskrnl/config/cmapi.c
 * PURPOSE:         Configuration Manager - Internal Registry APIs
 * PROGRAMMERS:     Alex Ionescu (alex.ionescu@reactos.org)
 *                  Eric Kohl
 */

/* INCLUDES ******************************************************************/

#include "ntoskrnl.h"
#define NDEBUG
#include "debug.h"

/* FUNCTIONS *****************************************************************/

BOOLEAN
NTAPI
CmpIsHiveAlreadyLoaded(IN HANDLE KeyHandle,
                       IN POBJECT_ATTRIBUTES SourceFile,
                       OUT PCMHIVE *CmHive)
{
    NTSTATUS Status;
    PCM_KEY_BODY KeyBody;
    PCMHIVE Hive;
    BOOLEAN Loaded = FALSE;
    PAGED_CODE();

    /* Sanity check */
    CMP_ASSERT_EXCLUSIVE_REGISTRY_LOCK();

    /* Reference the handle */
    Status = ObReferenceObjectByHandle(KeyHandle,
                                       0,
                                       CmpKeyObjectType,
                                       KernelMode,
                                       (PVOID)&KeyBody,
                                       NULL);
    if (!NT_SUCCESS(Status)) return Loaded;

    /* Don't touch deleted KCBs */
    if (KeyBody->KeyControlBlock->Delete) return Loaded;

    Hive = CONTAINING_RECORD(KeyBody->KeyControlBlock->KeyHive, CMHIVE, Hive);

    /* Must be the root key */
    if (!(KeyBody->KeyControlBlock->Flags & KEY_HIVE_ENTRY) ||
        !(Hive->FileUserName.Buffer))
    {
        /* It isn't */
        ObDereferenceObject(KeyBody);
        return Loaded;
    }

    /* Now compare the name of the file */
    if (!RtlCompareUnicodeString(&Hive->FileUserName,
                                 SourceFile->ObjectName,
                                 TRUE))
    {
        /* Same file found */
        Loaded = TRUE;
        *CmHive = Hive;
        
        /* If the hive is frozen, not sure what to do */
        if (Hive->Frozen)
        {
            /* FIXME: TODO */
            DPRINT1("ERROR: Hive is frozen\n");
            while (TRUE);
        }
     }

     /* Dereference and return result */
     ObDereferenceObject(KeyBody);
     return Loaded;
 }
 
BOOLEAN
NTAPI
CmpDoFlushAll(IN BOOLEAN ForceFlush)
{
    PLIST_ENTRY NextEntry;
    PCMHIVE Hive;
    NTSTATUS Status;
    BOOLEAN Result = TRUE;

    /* Make sure that the registry isn't read-only now */
    if (CmpNoWrite) return TRUE;

    /* Otherwise, acquire the hive list lock and disable force flush */
    CmpForceForceFlush = FALSE;
    ExAcquirePushLockShared(&CmpHiveListHeadLock);

    /* Loop the hive list */
    NextEntry = CmpHiveListHead.Flink;
    while (NextEntry != &CmpHiveListHead)
    {
        /* Get the hive */
        Hive = CONTAINING_RECORD(NextEntry, CMHIVE, HiveList);
        if (!(Hive->Hive.HiveFlags & HIVE_NOLAZYFLUSH))
        {
            /* Acquire the flusher lock */
            CmpLockHiveFlusherExclusive(Hive);
            
            /* Check for illegal state */
            if ((ForceFlush) && (Hive->UseCount))
            {
                /* Registry needs to be locked down */
                CMP_ASSERT_EXCLUSIVE_REGISTRY_LOCK();
                DPRINT1("FIXME: Hive is damaged and needs fixup\n");
                while (TRUE);
            }
            
            /* Only sync if we are forced to or if it won't cause a hive shrink */
            if ((ForceFlush) || (!HvHiveWillShrink(&Hive->Hive)))
            {
                /* Do the sync */
                Status = HvSyncHive(&Hive->Hive);

                /* If something failed - set the flag and continue looping */
                if (!NT_SUCCESS(Status)) Result = FALSE;
            }
            else
            {
                /* We won't flush if the hive might shrink */
                Result = FALSE;
                CmpForceForceFlush = TRUE;
            }

            /* Release the flusher lock */
            CmpUnlockHiveFlusher(Hive);
        }

        /* Try the next entry */
        NextEntry = NextEntry->Flink;
    }

    /* Release lock and return */
    ExReleasePushLock(&CmpHiveListHeadLock);
    return Result;
}

NTSTATUS
NTAPI
CmpSetValueKeyNew(IN PHHIVE Hive,
                  IN PCM_KEY_NODE Parent,
                  IN PUNICODE_STRING ValueName,
                  IN ULONG Index,
                  IN ULONG Type,
                  IN PVOID Data,
                  IN ULONG DataSize,
                  IN ULONG StorageType,
                  IN ULONG SmallData)
{
    PCELL_DATA CellData;
    HCELL_INDEX ValueCell;
    NTSTATUS Status;

    /* Check if we already have a value list */
    if (Parent->ValueList.Count)
    {
        /* Then make sure it's valid and dirty it */
        ASSERT(Parent->ValueList.List != HCELL_NIL);
        if (!HvMarkCellDirty(Hive, Parent->ValueList.List, FALSE))
        {
            /* Fail if we're out of space for log changes */
            return STATUS_NO_LOG_SPACE;
        }
    }

    /* Allocate a value cell */
    ValueCell = HvAllocateCell(Hive,
                               FIELD_OFFSET(CM_KEY_VALUE, Name) +
                               CmpNameSize(Hive, ValueName),
                               StorageType,
                               HCELL_NIL);
    if (ValueCell == HCELL_NIL) return STATUS_INSUFFICIENT_RESOURCES;

    /* Get the actual data for it */
    CellData = HvGetCell(Hive, ValueCell);
    if (!CellData) ASSERT(FALSE);

    /* Now we can release it, make sure it's also dirty */
    HvReleaseCell(Hive, ValueCell);
    ASSERT(HvIsCellDirty(Hive, ValueCell));

    /* Set it up and copy the name */
    CellData->u.KeyValue.Signature = CM_KEY_VALUE_SIGNATURE;
    _SEH2_TRY
    {
        /* This can crash since the name is coming from user-mode */
        CellData->u.KeyValue.NameLength = CmpCopyName(Hive,
                                                      CellData->u.KeyValue.Name,
                                                      ValueName);
    }
    _SEH2_EXCEPT(EXCEPTION_EXECUTE_HANDLER)
    {
        /* Fail */
        DPRINT1("Invalid user data!\n");
        HvFreeCell(Hive, ValueCell);
        _SEH2_YIELD(return _SEH2_GetExceptionCode());
    }
    _SEH2_END;

    /* Check for compressed name */
    if (CellData->u.KeyValue.NameLength < ValueName->Length)
    {
        /* This is a compressed name */
        CellData->u.KeyValue.Flags = VALUE_COMP_NAME;
    }
    else
    {
        /* No flags to set */
        CellData->u.KeyValue.Flags = 0;
    }

    /* Check if this is a normal key */
    if (DataSize > CM_KEY_VALUE_SMALL)
    {
        /* Build a data cell for it */
        Status = CmpSetValueDataNew(Hive,
                                    Data,
                                    DataSize,
                                    StorageType,
                                    ValueCell,
                                    &CellData->u.KeyValue.Data);
        if (!NT_SUCCESS(Status))
        {
            /* We failed, free the cell */
            HvFreeCell(Hive, ValueCell);
            return Status;
        }

        /* Otherwise, set the data length, and make sure the data is dirty */
        CellData->u.KeyValue.DataLength = DataSize;
        ASSERT(HvIsCellDirty(Hive, CellData->u.KeyValue.Data));
    }
    else
    {
        /* This is a small key, set the data directly inside */
        CellData->u.KeyValue.DataLength = DataSize + CM_KEY_VALUE_SPECIAL_SIZE;
        CellData->u.KeyValue.Data = SmallData;
    }
    
    /* Set the type now */
    CellData->u.KeyValue.Type = Type;

    /* Add this value cell to the child list */
    Status = CmpAddValueToList(Hive,
                               ValueCell,
                               Index,
                               StorageType,
                               &Parent->ValueList);

    /* If we failed, free the entire cell, including the data */
    if (!NT_SUCCESS(Status))
    {
        /* Overwrite the status with a known one */
        CmpFreeValue(Hive, ValueCell);
        Status = STATUS_INSUFFICIENT_RESOURCES;
    }

    /* Return Status */
    return Status;
}

NTSTATUS
NTAPI
CmpSetValueKeyExisting(IN PHHIVE Hive,
                       IN HCELL_INDEX OldChild,
                       IN PCM_KEY_VALUE Value,
                       IN ULONG Type,
                       IN PVOID Data,
                       IN ULONG DataSize,
                       IN ULONG StorageType,
                       IN ULONG TempData)
{
    HCELL_INDEX DataCell, NewCell;
    PCELL_DATA CellData;
    ULONG Length;
    BOOLEAN WasSmall, IsSmall;
    
    /* Registry writes must be blocked */
    CMP_ASSERT_FLUSH_LOCK(Hive);

    /* Mark the old child cell dirty */
    if (!HvMarkCellDirty(Hive, OldChild, FALSE)) return STATUS_NO_LOG_SPACE;

    /* See if this is a small or normal key */
    WasSmall = CmpIsKeyValueSmall(&Length, Value->DataLength);

    /* See if our new data can fit in a small key */
    IsSmall = (DataSize <= CM_KEY_VALUE_SMALL) ? TRUE: FALSE;

    /* Big keys are unsupported */
    ASSERT_VALUE_BIG(Hive, Length);
    ASSERT_VALUE_BIG(Hive, DataSize);

    /* Mark the old value dirty */
    if (!CmpMarkValueDataDirty(Hive, Value)) return STATUS_NO_LOG_SPACE;

    /* Check if we have a small key */
    if (IsSmall)
    {
        /* Check if we had a normal key with some data in it */
        if (!(WasSmall) && (Length > 0))
        {
            /* Free the previous data */
            CmpFreeValueData(Hive, Value->Data, Length);
        }

        /* Write our data directly */
        Value->DataLength = DataSize + CM_KEY_VALUE_SPECIAL_SIZE;
        Value->Data = TempData;
        Value->Type = Type;
        return STATUS_SUCCESS;
    }
    
    /* We have a normal key. Was the old cell also normal and had data? */
    if (!(WasSmall) && (Length > 0))
    {
        /* Get the current data cell and actual data inside it */
        DataCell = Value->Data;
        ASSERT(DataCell != HCELL_NIL);
        CellData = HvGetCell(Hive, DataCell);
        if (!CellData) return STATUS_INSUFFICIENT_RESOURCES;

        /* Immediately release the cell */
        HvReleaseCell(Hive, DataCell);

        /* Make sure that the data cell actually has a size */
        ASSERT(HvGetCellSize(Hive, CellData) > 0);

        /* Check if the previous data cell could fit our new data */
        if (DataSize <= (ULONG)(HvGetCellSize(Hive, CellData)))
        {
            /* Re-use it then */
            NewCell = DataCell;
        }
        else
        {
            /* Otherwise, re-allocate the current data cell */
            NewCell = HvReallocateCell(Hive, DataCell, DataSize);
            if (NewCell == HCELL_NIL) return STATUS_INSUFFICIENT_RESOURCES;
        }
    }
    else
    {
        /* This was a small key, or a key with no data, allocate a cell */
        NewCell = HvAllocateCell(Hive, DataSize, StorageType, HCELL_NIL);
        if (NewCell == HCELL_NIL) return STATUS_INSUFFICIENT_RESOURCES;
    }

    /* Now get the actual data for our data cell */
    CellData = HvGetCell(Hive, NewCell);
    if (!CellData) ASSERT(FALSE);

    /* Release it immediately */
    HvReleaseCell(Hive, NewCell);

    /* Copy our data into the data cell's buffer, and set up the value */
    RtlCopyMemory(CellData, Data, DataSize);
    Value->Data = NewCell;
    Value->DataLength = DataSize;
    Value->Type = Type;

    /* Return success */
    ASSERT(HvIsCellDirty(Hive, NewCell));
    return STATUS_SUCCESS;
}

NTSTATUS
NTAPI
CmpQueryKeyData(IN PHHIVE Hive,
                IN PCM_KEY_NODE Node,
                IN KEY_INFORMATION_CLASS KeyInformationClass,
                IN OUT PVOID KeyInformation,
                IN ULONG Length,
                IN OUT PULONG ResultLength)
{
    NTSTATUS Status;
    ULONG Size, SizeLeft, MinimumSize;
    PKEY_INFORMATION Info = (PKEY_INFORMATION)KeyInformation;
    USHORT NameLength;

    /* Check if the value is compressed */
    if (Node->Flags & KEY_COMP_NAME)
    {
        /* Get the compressed name size */
        NameLength = CmpCompressedNameSize(Node->Name, Node->NameLength);
    }
    else
    {
        /* Get the real size */
        NameLength = Node->NameLength;
    }

    /* Check what kind of information is being requested */
    switch (KeyInformationClass)
    {
        /* Basic information */
        case KeyBasicInformation:

            /* This is the size we need */
            Size = FIELD_OFFSET(KEY_BASIC_INFORMATION, Name) + NameLength;

            /* And this is the minimum we can work with */
            MinimumSize = FIELD_OFFSET(KEY_BASIC_INFORMATION, Name);

            /* Let the caller know and assume success */
            *ResultLength = Size;
            Status = STATUS_SUCCESS;

            /* Check if the bufer we got is too small */
            if (Length < MinimumSize)
            {
                /* Let the caller know and fail */
                Status = STATUS_BUFFER_TOO_SMALL;
                break;
            }

            /* Copy the basic information */
            Info->KeyBasicInformation.LastWriteTime = Node->LastWriteTime;
            Info->KeyBasicInformation.TitleIndex = 0;
            Info->KeyBasicInformation.NameLength = NameLength;

            /* Only the name is left */
            SizeLeft = Length - MinimumSize;
            Size = NameLength;

            /* Check if we don't have enough space for the name */
            if (SizeLeft < Size)
            {
                /* Truncate the name we'll return, and tell the caller */
                Size = SizeLeft;
                Status = STATUS_BUFFER_OVERFLOW;
            }

            /* Check if this is a compressed key */
            if (Node->Flags & KEY_COMP_NAME)
            {
                /* Copy the compressed name */
                CmpCopyCompressedName(Info->KeyBasicInformation.Name,
                                      SizeLeft,
                                      Node->Name,
                                      Node->NameLength);
            }
            else
            {
                /* Otherwise, copy the raw name */
                RtlCopyMemory(Info->KeyBasicInformation.Name,
                              Node->Name,
                              Size);
            }
            break;

        /* Node information */
        case KeyNodeInformation:

            /* Calculate the size we need */
            Size = FIELD_OFFSET(KEY_NODE_INFORMATION, Name) +
                   NameLength +
                   Node->ClassLength;

            /* And the minimum size we can support */
            MinimumSize = FIELD_OFFSET(KEY_NODE_INFORMATION, Name);

            /* Return the size to the caller and assume succes */
            *ResultLength = Size;
            Status = STATUS_SUCCESS;

            /* Check if the caller's buffer is too small */
            if (Length < MinimumSize)
            {
                /* Let them know, and fail */
                Status = STATUS_BUFFER_TOO_SMALL;
                break;
            }

            /* Copy the basic information */
            Info->KeyNodeInformation.LastWriteTime = Node->LastWriteTime;
            Info->KeyNodeInformation.TitleIndex = 0;
            Info->KeyNodeInformation.ClassLength = Node->ClassLength;
            Info->KeyNodeInformation.NameLength = NameLength;

            /* Now the name is left */
            SizeLeft = Length - MinimumSize;
            Size = NameLength;

            /* Check if the name can fit entirely */
            if (SizeLeft < Size)
            {
                /* It can't, we'll have to truncate. Tell the caller */
                Size = SizeLeft;
                Status = STATUS_BUFFER_OVERFLOW;
            }

            /* Check if the key node name is compressed */
            if (Node->Flags & KEY_COMP_NAME)
            {
                /* Copy the compressed name */
                CmpCopyCompressedName(Info->KeyNodeInformation.Name,
                                      SizeLeft,
                                      Node->Name,
                                      Node->NameLength);
            }
            else
            {
                /* It isn't, so copy the raw name */
                RtlCopyMemory(Info->KeyNodeInformation.Name,
                              Node->Name,
                              Size);
            }

            /* Check if the node has a class */
            if (Node->ClassLength > 0)
            {
                /* It does. We don't support these yet */
                ASSERTMSG("Classes not supported\n", FALSE);
            }
            else
            {
                /* It doesn't, so set offset to -1, not 0! */
                Info->KeyNodeInformation.ClassOffset = 0xFFFFFFFF;
            }
            break;

        /* Full information requsted */
        case KeyFullInformation:

            /* This is the size we need */
            Size = FIELD_OFFSET(KEY_FULL_INFORMATION, Class) +
                   Node->ClassLength;

            /* This is what we can work with */
            MinimumSize = FIELD_OFFSET(KEY_FULL_INFORMATION, Class);

            /* Return it to caller and assume success */
            *ResultLength = Size;
            Status = STATUS_SUCCESS;

            /* Check if the caller's buffer is to small */
            if (Length < MinimumSize)
            {
                /* Let them know and fail */
                Status = STATUS_BUFFER_TOO_SMALL;
                break;
            }

            /* Now copy all the basic information */
            Info->KeyFullInformation.LastWriteTime = Node->LastWriteTime;
            Info->KeyFullInformation.TitleIndex = 0;
            Info->KeyFullInformation.ClassLength = Node->ClassLength;
            Info->KeyFullInformation.SubKeys = Node->SubKeyCounts[Stable] +
                                               Node->SubKeyCounts[Volatile];
            Info->KeyFullInformation.Values = Node->ValueList.Count;
            Info->KeyFullInformation.MaxNameLen = Node->MaxNameLen;
            Info->KeyFullInformation.MaxClassLen = Node->MaxClassLen;
            Info->KeyFullInformation.MaxValueNameLen = Node->MaxValueNameLen;
            Info->KeyFullInformation.MaxValueDataLen = Node->MaxValueDataLen;

            /* Check if we have a class */
            if (Node->ClassLength > 0)
            {
                /* We do, but we currently don't support this */
                ASSERTMSG("Classes not supported\n", FALSE);
            }
            else
            {
                /* We don't have a class, so set offset to -1, not 0! */
                Info->KeyNodeInformation.ClassOffset = 0xFFFFFFFF;
            }
            break;

        /* Any other class that got sent here is invalid! */
        default:

            /* Set failure code */
            Status = STATUS_INVALID_PARAMETER;
            break;
    }

    /* Return status */
    return Status;
}

NTSTATUS
NTAPI
CmSetValueKey(IN PCM_KEY_CONTROL_BLOCK Kcb,
              IN PUNICODE_STRING ValueName,
              IN ULONG Type,
              IN PVOID Data,
              IN ULONG DataLength)
{
    PHHIVE Hive = NULL;
    PCM_KEY_NODE Parent;
    PCM_KEY_VALUE Value = NULL;
    HCELL_INDEX CurrentChild, Cell;
    NTSTATUS Status;
    BOOLEAN Found, Result;
    ULONG Count, ChildIndex, SmallData, Storage;
    VALUE_SEARCH_RETURN_TYPE SearchResult;
    BOOLEAN FirstTry = TRUE, FlusherLocked = FALSE;
    HCELL_INDEX ParentCell = HCELL_NIL, ChildCell = HCELL_NIL;

    /* Acquire hive and KCB lock */
    CmpLockRegistry();
    CmpAcquireKcbLockShared(Kcb);
    
    /* Sanity check */
    ASSERT(sizeof(ULONG) == CM_KEY_VALUE_SMALL);
    
    /* Don't touch deleted KCBs */
DoAgain:
    if (Kcb->Delete)
    {
        /* Fail */
        Status = STATUS_KEY_DELETED;
        goto Quickie;
    }
    
    /* Don't let anyone mess with symlinks */
    if ((Kcb->Flags & KEY_SYM_LINK) &&
        ((Type != REG_LINK) ||
         !(ValueName) ||
         !(RtlEqualUnicodeString(&CmSymbolicLinkValueName, ValueName, TRUE))))
    {
        /* Invalid modification of a symlink key */
        Status = STATUS_ACCESS_DENIED;
        goto Quickie;
    }

    /* Check if this is the first attempt */
    if (FirstTry)
    {
        /* Search for the value in the cache */
        SearchResult = CmpCompareNewValueDataAgainstKCBCache(Kcb,
                                                             ValueName,
                                                             Type,
                                                             Data,
                                                             DataLength);
        if (SearchResult == SearchNeedExclusiveLock)
        {
            /* Try again with the exclusive lock */
            CmpConvertKcbSharedToExclusive(Kcb);
            goto DoAgain;
        }
        else if (SearchResult == SearchSuccess)
        {
            /* We don't actually need to do anything! */
            Status = STATUS_SUCCESS;
            goto Quickie;
        }

        /* We need the exclusive KCB lock now */
        if (!(CmpIsKcbLockedExclusive(Kcb)) &&
            !(CmpTryToConvertKcbSharedToExclusive(Kcb)))
        {
            /* Acquire exclusive lock */
            CmpConvertKcbSharedToExclusive(Kcb);
        }
        
        /* Cache lookup failed, so don't try it next time */
        FirstTry = FALSE;
        
        /* Now grab the flush lock since the key will be modified */
        ASSERT(FlusherLocked == FALSE);
        CmpLockHiveFlusherShared((PCMHIVE)Kcb->KeyHive);
        FlusherLocked = TRUE;
        goto DoAgain;
    }
    else
    {
        /* Get pointer to key cell */
        Hive = Kcb->KeyHive;
        Cell = Kcb->KeyCell;

        /* Get the parent */
        Parent = (PCM_KEY_NODE)HvGetCell(Hive, Cell);
        ASSERT(Parent);
        ParentCell = Cell;
        
        /* Prepare to scan the key node */
        Count = Parent->ValueList.Count;
        Found = FALSE;
        if (Count > 0)
        {
            /* Try to find the existing name */
            Result = CmpFindNameInList(Hive,
                                       &Parent->ValueList,
                                       ValueName,
                                       &ChildIndex,
                                       &CurrentChild);
            if (!Result)
            {
                /* Fail */
                Status = STATUS_INSUFFICIENT_RESOURCES;
                goto Quickie;
            }

            /* Check if we found something */
            if (CurrentChild != HCELL_NIL)
            {
                /* Release existing child */
                if (ChildCell != HCELL_NIL)
                {
                    HvReleaseCell(Hive, ChildCell);
                    ChildCell = HCELL_NIL;
                }
                
                /* Get its value */
                Value = (PCM_KEY_VALUE)HvGetCell(Hive, CurrentChild);
                if (!Value)
                {
                    /* Fail */
                    Status = STATUS_INSUFFICIENT_RESOURCES;
                    goto Quickie;
                }

                /* Remember that we found it */
                ChildCell = CurrentChild;
                Found = TRUE;
            }
        }
        else
        {
            /* No child list, we'll need to add it */
            ChildIndex = 0;
        }
    }
    
    /* Should only get here on the second pass */
    ASSERT(FirstTry == FALSE);
    
    /* The KCB must be locked exclusive at this point */
    CMP_ASSERT_KCB_LOCK(Kcb);
    
    /* Mark the cell dirty */
    if (!HvMarkCellDirty(Hive, Cell, FALSE))
    {
        /* Not enough log space, fail */
        Status = STATUS_NO_LOG_SPACE;
        goto Quickie;
    }

    /* Get the storage type */
    Storage = HvGetCellType(Cell);

    /* Check if this is small data */
    SmallData = 0;
    if ((DataLength <= CM_KEY_VALUE_SMALL) && (DataLength > 0))
    {
        /* Need SEH because user data may be invalid */
        _SEH2_TRY
        {
            /* Copy it */
            RtlCopyMemory(&SmallData, Data, DataLength);
        }
        _SEH2_EXCEPT(EXCEPTION_EXECUTE_HANDLER)
        {
            /* Return failure code */
            Status = _SEH2_GetExceptionCode();
            _SEH2_YIELD(goto Quickie);
        }
        _SEH2_END;
    }

    /* Check if we didn't find a matching key */
    if (!Found)
    {
        /* Call the internal routine */
        Status = CmpSetValueKeyNew(Hive,
                                   Parent,
                                   ValueName,
                                   ChildIndex,
                                   Type,
                                   Data,
                                   DataLength,
                                   Storage,
                                   SmallData);
    }
    else
    {
        /* Call the internal routine */
        Status = CmpSetValueKeyExisting(Hive,
                                        CurrentChild,
                                        Value,
                                        Type,
                                        Data,
                                        DataLength,
                                        Storage,
                                        SmallData);
    }

    /* Check for success */
    if (NT_SUCCESS(Status))
    {
        /* Check if the maximum value name length changed */
        ASSERT(Parent->MaxValueNameLen == Kcb->KcbMaxValueNameLen);
        if (Parent->MaxValueNameLen < ValueName->Length)
        {
            /* Set the new values */
            Parent->MaxValueNameLen = ValueName->Length;
            Kcb->KcbMaxValueNameLen = ValueName->Length;
        }
    
        /* Check if the maximum data length changed */
        ASSERT(Parent->MaxValueDataLen == Kcb->KcbMaxValueDataLen);
        if (Parent->MaxValueDataLen < DataLength)
        {
            /* Update it */
            Parent->MaxValueDataLen = DataLength;
            Kcb->KcbMaxValueDataLen = Parent->MaxValueDataLen;
        }
        
        /* Save the write time */
        KeQuerySystemTime(&Parent->LastWriteTime);
        Kcb->KcbLastWriteTime = Parent->LastWriteTime;
        
        /* Check if the cell is cached */
        if ((Found) && (CMP_IS_CELL_CACHED(Kcb->ValueCache.ValueList)))
        {
            /* Shouldn't happen */
            ASSERT(FALSE);
        }
        else
        {
            /* Cleanup the value cache */
            CmpCleanUpKcbValueCache(Kcb);

            /* Sanity checks */
            ASSERT(!(CMP_IS_CELL_CACHED(Kcb->ValueCache.ValueList)));
            ASSERT(!(Kcb->ExtFlags & CM_KCB_SYM_LINK_FOUND));
            
            /* Set the value cache */
            Kcb->ValueCache.Count = Parent->ValueList.Count;
            Kcb->ValueCache.ValueList = Parent->ValueList.List;
        }
        
        /* Notify registered callbacks */
        CmpReportNotify(Kcb,
                        Hive,
                        Kcb->KeyCell,
                        REG_NOTIFY_CHANGE_LAST_SET);
    }
    
    /* Release the cells */
Quickie:
    if ((ParentCell != HCELL_NIL) && (Hive)) HvReleaseCell(Hive, ParentCell);
    if ((ChildCell != HCELL_NIL) && (Hive)) HvReleaseCell(Hive, ChildCell);

    /* Release the locks */
    if (FlusherLocked) CmpUnlockHiveFlusher((PCMHIVE)Hive);
    CmpReleaseKcbLock(Kcb);
    CmpUnlockRegistry();
    return Status;
}

NTSTATUS
NTAPI
CmDeleteValueKey(IN PCM_KEY_CONTROL_BLOCK Kcb,
                 IN UNICODE_STRING ValueName)
{
    NTSTATUS Status = STATUS_OBJECT_NAME_NOT_FOUND;
    PHHIVE Hive;
    PCM_KEY_NODE Parent;
    HCELL_INDEX ChildCell, Cell;
    PCHILD_LIST ChildList;
    PCM_KEY_VALUE Value = NULL;
    ULONG ChildIndex;
    BOOLEAN Result;

    /* Acquire hive lock */
    CmpLockRegistry();
    
    /* Lock KCB exclusively */
    CmpAcquireKcbLockExclusive(Kcb);
    
    /* Don't touch deleted keys */
    if (Kcb->Delete)
    {
        /* Undo everything */
        CmpReleaseKcbLock(Kcb);
        CmpUnlockRegistry();
        return STATUS_KEY_DELETED;
    }

    /* Get the hive and the cell index */
    Hive = Kcb->KeyHive;
    Cell = Kcb->KeyCell;
    
    /* Lock flushes */
    CmpLockHiveFlusherShared((PCMHIVE)Hive);

    /* Get the parent key node */
    Parent = (PCM_KEY_NODE)HvGetCell(Hive, Cell);
    ASSERT(Parent);

    /* Get the value list and check if it has any entries */
    ChildList = &Parent->ValueList;
    if (ChildList->Count)
    {
        /* Try to find this value */
        Result = CmpFindNameInList(Hive,
                                   ChildList,
                                   &ValueName,
                                   &ChildIndex,
                                   &ChildCell);
        if (!Result)
        {
            /* Fail */
            Status = STATUS_INSUFFICIENT_RESOURCES;
            goto Quickie;
        }

        /* Value not found, return error */
        if (ChildCell == HCELL_NIL) goto Quickie;

        /* We found the value, mark all relevant cells dirty */
        if (!((HvMarkCellDirty(Hive, Cell, FALSE)) &&
              (HvMarkCellDirty(Hive, Parent->ValueList.List, FALSE)) &&
              (HvMarkCellDirty(Hive, ChildCell, FALSE))))
        {
            /* Not enough log space, fail */
            Status = STATUS_NO_LOG_SPACE;
            goto Quickie;
        }

        /* Get the key value */
        Value = (PCM_KEY_VALUE)HvGetCell(Hive,ChildCell);
        ASSERT(Value);

        /* Mark it and all related data as dirty */
        if (!CmpMarkValueDataDirty(Hive, Value))
        {
            /* Not enough log space, fail */
            Status = STATUS_NO_LOG_SPACE;
            goto Quickie;
        }

        /* Ssanity checks */
        ASSERT(HvIsCellDirty(Hive, Parent->ValueList.List));
        ASSERT(HvIsCellDirty(Hive, ChildCell));

        /* Remove the value from the child list */
        Status = CmpRemoveValueFromList(Hive, ChildIndex, ChildList);
        if (!NT_SUCCESS(Status))
        {
            /* Set known error */
            Status = STATUS_INSUFFICIENT_RESOURCES;
            goto Quickie;
        }

        /* Remove the value and its data itself */
        if (!CmpFreeValue(Hive, ChildCell))
        {
            /* Failed to free the value, fail */
            Status = STATUS_INSUFFICIENT_RESOURCES;
            goto Quickie;
        }

        /* Set the last write time */
        KeQuerySystemTime(&Parent->LastWriteTime);
        Kcb->KcbLastWriteTime = Parent->LastWriteTime;

        /* Sanity check */
        ASSERT(Parent->MaxValueNameLen == Kcb->KcbMaxValueNameLen);
        ASSERT(Parent->MaxValueDataLen == Kcb->KcbMaxValueDataLen);
        ASSERT(HvIsCellDirty(Hive, Cell));

        /* Check if the value list is empty now */
        if (!Parent->ValueList.Count)
        {
            /* Then clear key node data */
            Parent->MaxValueNameLen = 0;
            Parent->MaxValueDataLen = 0;
            Kcb->KcbMaxValueNameLen = 0;
            Kcb->KcbMaxValueDataLen = 0;
        }
        
        /* Cleanup the value cache */
        CmpCleanUpKcbValueCache(Kcb);
        
        /* Sanity checks */
        ASSERT(!(CMP_IS_CELL_CACHED(Kcb->ValueCache.ValueList)));
        ASSERT(!(Kcb->ExtFlags & CM_KCB_SYM_LINK_FOUND));
        
        /* Set the value cache */
        Kcb->ValueCache.Count = ChildList->Count;
        Kcb->ValueCache.ValueList = ChildList->List;
        
        /* Notify registered callbacks */
        CmpReportNotify(Kcb, Hive, Cell, REG_NOTIFY_CHANGE_LAST_SET);

        /* Change default Status to success */
        Status = STATUS_SUCCESS;
    }

Quickie:
    /* Release the parent cell, if any */
    if (Parent) HvReleaseCell(Hive, Cell);

    /* Check if we had a value */
    if (Value)
    {
        /* Release the child cell */
        ASSERT(ChildCell != HCELL_NIL);
        HvReleaseCell(Hive, ChildCell);
    }

    /* Release locks */
    CmpUnlockHiveFlusher((PCMHIVE)Hive);
    CmpReleaseKcbLock(Kcb);
    CmpUnlockRegistry();
    return Status;
}

NTSTATUS
NTAPI
CmQueryValueKey(IN PCM_KEY_CONTROL_BLOCK Kcb,
                IN UNICODE_STRING ValueName,
                IN KEY_VALUE_INFORMATION_CLASS KeyValueInformationClass,
                IN PVOID KeyValueInformation,
                IN ULONG Length,
                IN PULONG ResultLength)
{
    NTSTATUS Status;
    PCM_KEY_VALUE ValueData;
    ULONG Index;
    BOOLEAN ValueCached = FALSE;
    PCM_CACHED_VALUE *CachedValue;
    HCELL_INDEX CellToRelease;
    VALUE_SEARCH_RETURN_TYPE Result;
    PHHIVE Hive;
    PAGED_CODE();

    /* Acquire hive lock */
    CmpLockRegistry();
    
    /* Lock the KCB shared */
    CmpAcquireKcbLockShared(Kcb);
    
    /* Don't touch deleted keys */
DoAgain:
    if (Kcb->Delete)
    {
        /* Undo everything */
        CmpReleaseKcbLock(Kcb);
        CmpUnlockRegistry();
        return STATUS_KEY_DELETED;
    }
    
    /* We don't deal with this yet */
    if (Kcb->ExtFlags & CM_KCB_SYM_LINK_FOUND)
    {
        /* Shouldn't happen */
        ASSERT(FALSE);
    }

    /* Get the hive */
    Hive = Kcb->KeyHive;

    /* Find the key value */
    Result = CmpFindValueByNameFromCache(Kcb,
                                         &ValueName,
                                         &CachedValue,
                                         &Index,
                                         &ValueData,
                                         &ValueCached,
                                         &CellToRelease);
    if (Result == SearchNeedExclusiveLock)
    {
        /* Check if we need an exclusive lock */
        ASSERT(CellToRelease == HCELL_NIL);
        ASSERT(ValueData == NULL);
        
        /* Try with exclusive KCB lock */
        CmpConvertKcbSharedToExclusive(Kcb);
        goto DoAgain;
    }
    
    if (Result == SearchSuccess)
    {
        /* Sanity check */
        ASSERT(ValueData != NULL);

        /* User data, protect against exceptions */
        _SEH2_TRY
        {
            /* Query the information requested */
            Result = CmpQueryKeyValueData(Kcb,
                                          CachedValue,
                                          ValueData,
                                          ValueCached,
                                          KeyValueInformationClass,
                                          KeyValueInformation,
                                          Length,
                                          ResultLength,
                                          &Status);
            if (Result == SearchNeedExclusiveLock)
            {            
                /* Release the value cell */
                if (CellToRelease != HCELL_NIL)
                {
                    HvReleaseCell(Hive, CellToRelease);
                    CellToRelease = HCELL_NIL;
                }
                
                /* Try with exclusive KCB lock */
                CmpConvertKcbSharedToExclusive(Kcb);
                goto DoAgain;
            }
        }
        _SEH2_EXCEPT(EXCEPTION_EXECUTE_HANDLER)
        {
            Status = _SEH2_GetExceptionCode();
        }
        _SEH2_END;
    }
    else
    {
        /* Failed to find the value */
        Status = STATUS_OBJECT_NAME_NOT_FOUND;
    }

    /* If we have a cell to release, do so */
    if (CellToRelease != HCELL_NIL) HvReleaseCell(Hive, CellToRelease);

    /* Release locks */
    CmpReleaseKcbLock(Kcb);
    CmpUnlockRegistry();
    return Status;
}

NTSTATUS
NTAPI
CmEnumerateValueKey(IN PCM_KEY_CONTROL_BLOCK Kcb,
                    IN ULONG Index,
                    IN KEY_VALUE_INFORMATION_CLASS KeyValueInformationClass,
                    IN PVOID KeyValueInformation,
                    IN ULONG Length,
                    IN PULONG ResultLength)
{
    NTSTATUS Status;
    PHHIVE Hive;
    PCM_KEY_NODE Parent;
    HCELL_INDEX CellToRelease = HCELL_NIL, CellToRelease2 = HCELL_NIL;
    VALUE_SEARCH_RETURN_TYPE Result;
    BOOLEAN IndexIsCached, ValueIsCached = FALSE;
    PCELL_DATA CellData;
    PCM_CACHED_VALUE *CachedValue;
    PCM_KEY_VALUE ValueData = NULL;
    PAGED_CODE();

    /* Acquire hive lock */
    CmpLockRegistry();
    
    /* Lock the KCB shared */
    CmpAcquireKcbLockShared(Kcb);

    /* Don't touch deleted keys */
DoAgain:
    if (Kcb->Delete)
    {
        /* Undo everything */
        CmpReleaseKcbLock(Kcb);
        CmpUnlockRegistry();
        return STATUS_KEY_DELETED;
    }

    /* Get the hive and parent */
    Hive = Kcb->KeyHive;
    Parent = (PCM_KEY_NODE)HvGetCell(Hive, Kcb->KeyCell);
    ASSERT(Parent);

    /* FIXME: Lack of cache? */
    if (Kcb->ValueCache.Count != Parent->ValueList.Count)
    {
        DPRINT1("HACK: Overriding value cache count\n");
        Kcb->ValueCache.Count = Parent->ValueList.Count;
    }

    /* Make sure the index is valid */    
    if (Index >= Kcb->ValueCache.Count)
    {
        /* Release the cell and fail */
        HvReleaseCell(Hive, Kcb->KeyCell);
        Status = STATUS_NO_MORE_ENTRIES;
        goto Quickie;
    }
    
    /* We don't deal with this yet */
    if (Kcb->ExtFlags & CM_KCB_SYM_LINK_FOUND)
    {
        /* Shouldn't happen */
        ASSERT(FALSE);
    }

    /* Find the value list */
    Result = CmpGetValueListFromCache(Kcb,
                                      &CellData,
                                      &IndexIsCached,
                                      &CellToRelease);
    if (Result == SearchNeedExclusiveLock)
    {
        /* Check if we need an exclusive lock */
        ASSERT(CellToRelease == HCELL_NIL);
        HvReleaseCell(Hive, Kcb->KeyCell);
        
        /* Try with exclusive KCB lock */
        CmpConvertKcbSharedToExclusive(Kcb);
        goto DoAgain;
    }
    else if (Result != SearchSuccess)
    {
        /* Sanity check */
        ASSERT(CellData == NULL);

        /* Release the cell and fail */
        Status = STATUS_INSUFFICIENT_RESOURCES;
        goto Quickie;
    }

    /* Now get the key value */
    Result = CmpGetValueKeyFromCache(Kcb,
                                     CellData,
                                     Index,
                                     &CachedValue,
                                     &ValueData,
                                     IndexIsCached,
                                     &ValueIsCached,
                                     &CellToRelease2);
    if (Result == SearchNeedExclusiveLock)
    {
        /* Cleanup state */
        ASSERT(CellToRelease2 == HCELL_NIL);
        if (CellToRelease)
        {
            HvReleaseCell(Hive, CellToRelease);
            CellToRelease = HCELL_NIL;
        }
        HvReleaseCell(Hive, Kcb->KeyCell);

        /* Try with exclusive KCB lock */
        CmpConvertKcbSharedToExclusive(Kcb);
        goto DoAgain;
    }
    else if (Result != SearchSuccess)
    {
        /* Sanity check */
        ASSERT(ValueData == NULL);

        /* Release the cells and fail */
        Status = STATUS_INSUFFICIENT_RESOURCES;
        goto Quickie;
    }
    
    /* User data, need SEH */
    _SEH2_TRY
    {
        /* Query the information requested */
        Result = CmpQueryKeyValueData(Kcb,
                                      CachedValue,
                                      ValueData,
                                      ValueIsCached,
                                      KeyValueInformationClass,
                                      KeyValueInformation,
                                      Length,
                                      ResultLength,
                                      &Status);
        if (Result == SearchNeedExclusiveLock)
        {
            /* Cleanup state */
            if (CellToRelease2) HvReleaseCell(Hive, CellToRelease2);
            HvReleaseCell(Hive, Kcb->KeyCell);
            if (CellToRelease) HvReleaseCell(Hive, CellToRelease);

            /* Try with exclusive KCB lock */
            CmpConvertKcbSharedToExclusive(Kcb);
            goto DoAgain;
        }
    }
    _SEH2_EXCEPT(EXCEPTION_EXECUTE_HANDLER)
    {
        /* Get exception code */
        Status = _SEH2_GetExceptionCode();
    }
    _SEH2_END;

Quickie:
    /* If we have a cell to release, do so */
    if (CellToRelease != HCELL_NIL) HvReleaseCell(Hive, CellToRelease);

    /* Release the parent cell */
    HvReleaseCell(Hive, Kcb->KeyCell);

    /* If we have a cell to release, do so */
    if (CellToRelease2 != HCELL_NIL) HvReleaseCell(Hive, CellToRelease2);

    /* Release locks */
    CmpReleaseKcbLock(Kcb);
    CmpUnlockRegistry();
    return Status;
}

NTSTATUS
NTAPI
CmQueryKey(IN PCM_KEY_CONTROL_BLOCK Kcb,
           IN KEY_INFORMATION_CLASS KeyInformationClass,
           IN PVOID KeyInformation,
           IN ULONG Length,
           IN PULONG ResultLength)
{
    NTSTATUS Status;
    PHHIVE Hive;
    PCM_KEY_NODE Parent;
    HV_TRACK_CELL_REF CellReferences = {0};

    /* Acquire hive lock */
    CmpLockRegistry();
    
    /* Lock KCB shared */
    CmpAcquireKcbLockShared(Kcb);

    /* Don't touch deleted keys */
    if (Kcb->Delete)
    {
        /* Fail */
        Status = STATUS_KEY_DELETED;
        goto Quickie;
    }

    /* Check what class we got */
    switch (KeyInformationClass)
    {
        /* Typical information */
        case KeyFullInformation:
        case KeyBasicInformation:
        case KeyNodeInformation:

            /* Get the hive and parent */
            Hive = Kcb->KeyHive;
            Parent = (PCM_KEY_NODE)HvGetCell(Hive, Kcb->KeyCell);
            ASSERT(Parent);
            
            /* Track cell references */
            if (!HvTrackCellRef(&CellReferences, Hive, Kcb->KeyCell))
            {
                /* Not enough memory to track references */
                Status = STATUS_INSUFFICIENT_RESOURCES;
            }
            else
            {
                /* Call the internal API */
                Status = CmpQueryKeyData(Hive,
                                         Parent,
                                         KeyInformationClass,
                                         KeyInformation,
                                         Length,
                                         ResultLength);
            }
            break;

        /* Unsupported classes for now */
        case KeyNameInformation:
        case KeyCachedInformation:
        case KeyFlagsInformation:

            /* Print message and fail */
            DPRINT1("Unsupported class: %d!\n", KeyInformationClass);
            Status = STATUS_NOT_IMPLEMENTED;
            break;

        /* Illegal classes */
        default:

            /* Print message and fail */
            DPRINT1("Unsupported class: %d!\n", KeyInformationClass);
            Status = STATUS_INVALID_INFO_CLASS;
            break;
    }

Quickie:
    /* Release references */
    HvReleaseFreeCellRefArray(&CellReferences);

    /* Release locks */
    CmpReleaseKcbLock(Kcb);
    CmpUnlockRegistry();
    return Status;
}

NTSTATUS
NTAPI
CmEnumerateKey(IN PCM_KEY_CONTROL_BLOCK Kcb,
               IN ULONG Index,
               IN KEY_INFORMATION_CLASS KeyInformationClass,
               IN PVOID KeyInformation,
               IN ULONG Length,
               IN PULONG ResultLength)
{
    NTSTATUS Status;
    PHHIVE Hive;
    PCM_KEY_NODE Parent, Child;
    HCELL_INDEX ChildCell;
    HV_TRACK_CELL_REF CellReferences = {0};

    /* Acquire hive lock */
    CmpLockRegistry();
    
    /* Lock the KCB shared */
    CmpAcquireKcbLockShared(Kcb);
    
    /* Don't touch deleted keys */
    if (Kcb->Delete)
    {
        /* Undo everything */
        Status = STATUS_KEY_DELETED;
        goto Quickie;
    }

    /* Get the hive and parent */
    Hive = Kcb->KeyHive;
    Parent = (PCM_KEY_NODE)HvGetCell(Hive, Kcb->KeyCell);
    ASSERT(Parent);

    /* Get the child cell */
    ChildCell = CmpFindSubKeyByNumber(Hive, Parent, Index);

    /* Release the parent cell */
    HvReleaseCell(Hive, Kcb->KeyCell);

    /* Check if we found the child */
    if (ChildCell == HCELL_NIL)
    {
        /* We didn't, fail */
        Status = STATUS_NO_MORE_ENTRIES;
        goto Quickie;
    }

    /* Now get the actual child node */
    Child = (PCM_KEY_NODE)HvGetCell(Hive, ChildCell);
    ASSERT(Child);
    
    /* Track references */
    if (!HvTrackCellRef(&CellReferences, Hive, ChildCell))
    {
        /* Can't allocate memory for tracking */
        Status = STATUS_INSUFFICIENT_RESOURCES;
        goto Quickie;
    }

    /* Data can be user-mode, use SEH */
    _SEH2_TRY
    {
        /* Query the data requested */
        Status = CmpQueryKeyData(Hive,
                                 Child,
                                 KeyInformationClass,
                                 KeyInformation,
                                 Length,
                                 ResultLength);
    }
    _SEH2_EXCEPT(EXCEPTION_EXECUTE_HANDLER)
    {
        /* Fail with exception code */
        Status = _SEH2_GetExceptionCode();
        _SEH2_YIELD(goto Quickie);
    }
    _SEH2_END;

Quickie:
    /* Release references */
    HvReleaseFreeCellRefArray(&CellReferences);

    /* Release locks */
    CmpReleaseKcbLock(Kcb);
    CmpUnlockRegistry();
    return Status;
}

NTSTATUS
NTAPI
CmDeleteKey(IN PCM_KEY_BODY KeyBody)
{
    NTSTATUS Status;
    PHHIVE Hive;
    PCM_KEY_NODE Node, Parent;
    HCELL_INDEX Cell, ParentCell;
    PCM_KEY_CONTROL_BLOCK Kcb;

    /* Acquire hive lock */
    CmpLockRegistry();
    
    /* Get the kcb */
    Kcb = KeyBody->KeyControlBlock;
    
    /* Don't allow deleting the root */
    if (!Kcb->ParentKcb)
    {
        /* Fail */
        CmpUnlockRegistry();
        return STATUS_CANNOT_DELETE;
    }
    
    /* Lock parent and child */
    CmpAcquireTwoKcbLocksExclusiveByKey(Kcb->ConvKey, Kcb->ParentKcb->ConvKey);
    
    /* Check if we're already being deleted */
    if (Kcb->Delete)
    {
        /* Don't do it twice */
        Status = STATUS_SUCCESS;
        goto Quickie2;
    }

    /* Get the hive and node */
    Hive = Kcb->KeyHive;
    Cell = Kcb->KeyCell;
    
    /* Lock flushes */
    CmpLockHiveFlusherShared((PCMHIVE)Hive);
    
    /* Get the key node */
    Node = (PCM_KEY_NODE)HvGetCell(Hive, Cell);
    ASSERT(Node);
   
    /* Sanity check */
    ASSERT(Node->Flags == Kcb->Flags);

    /* Check if we don't have any children */
    if (!(Node->SubKeyCounts[Stable] + Node->SubKeyCounts[Volatile]) &&
        !(Node->Flags & KEY_NO_DELETE))
    {
        /* Send notification to registered callbacks */
        CmpReportNotify(Kcb, Hive, Cell, REG_NOTIFY_CHANGE_NAME);
        
        /* Get the parent and free the cell */
        ParentCell = Node->Parent;
        Status = CmpFreeKeyByCell(Hive, Cell, TRUE);
        if (NT_SUCCESS(Status))
        {
            /* Flush any notifications */
            CmpFlushNotifiesOnKeyBodyList(Kcb, FALSE);
            
            /* Clean up information we have on the subkey */
            CmpCleanUpSubKeyInfo(Kcb->ParentKcb);

            /* Get the parent node */
            Parent = (PCM_KEY_NODE)HvGetCell(Hive, ParentCell);
            if (Parent)
            {
                /* Update the maximum name length */
                Kcb->ParentKcb->KcbMaxNameLen = Parent->MaxNameLen;
                
                /* Make sure we're dirty */
                ASSERT(HvIsCellDirty(Hive, ParentCell));

                /* Update the write time */
                KeQuerySystemTime(&Parent->LastWriteTime);
                Kcb->ParentKcb->KcbLastWriteTime = Parent->LastWriteTime;

                /* Release the cell */
                HvReleaseCell(Hive, ParentCell);
            }
            
            /* Set the KCB in delete mode and remove it */
            Kcb->Delete = TRUE;
            CmpRemoveKeyControlBlock(Kcb);

            /* Clear the cell */
            Kcb->KeyCell = HCELL_NIL;
        }
    }
    else
    {
        /* Fail */
        Status = STATUS_CANNOT_DELETE;
    }
    
    /* Release the cell */
    HvReleaseCell(Hive, Cell);

    /* Release flush lock */
    CmpUnlockHiveFlusher((PCMHIVE)Hive);

    /* Release the KCB locks */
Quickie2:
    CmpReleaseTwoKcbLockByKey(Kcb->ConvKey, Kcb->ParentKcb->ConvKey);

    /* Release hive lock */
    CmpUnlockRegistry();
    return Status;
}

NTSTATUS
NTAPI
CmFlushKey(IN PCM_KEY_CONTROL_BLOCK Kcb,
           IN BOOLEAN ExclusiveLock)
{
    PCMHIVE CmHive;
    NTSTATUS Status = STATUS_SUCCESS;
    PHHIVE Hive;

    /* Ignore flushes until we're ready */
    if (CmpNoWrite) return STATUS_SUCCESS;
    
    /* Get the hives */
    Hive = Kcb->KeyHive;
    CmHive = (PCMHIVE)Hive;
          
    /* Check if this is the master hive */
    if (CmHive == CmiVolatileHive)
    {
        /* Flush all the hives instead */
        CmpDoFlushAll(FALSE);
    }
    else
    {
        /* Don't touch the hive */
        CmpLockHiveFlusherExclusive(CmHive);
        ASSERT(CmHive->ViewLock);
        KeAcquireGuardedMutex(CmHive->ViewLock);
        CmHive->ViewLockOwner = KeGetCurrentThread();
        
        /* Will the hive shrink? */
        if (HvHiveWillShrink(Hive))
        {
            /* I don't believe the current Hv does shrinking */
            ASSERT(FALSE);
        }
        else
        {
            /* Now we can release views */
            ASSERT(CmHive->ViewLock);
            CMP_ASSERT_EXCLUSIVE_REGISTRY_LOCK_OR_LOADING(CmHive);
            ASSERT(KeGetCurrentThread() == CmHive->ViewLockOwner);
            KeReleaseGuardedMutex(CmHive->ViewLock);
        }
        
        /* Flush only this hive */
        if (!HvSyncHive(Hive))
        {
            /* Fail */
            Status = STATUS_REGISTRY_IO_FAILED;
        }
        
        /* Release the flush lock */
        CmpUnlockHiveFlusher((PCMHIVE)Hive);
    }

    /* Return the status */
    return Status;
}

NTSTATUS
NTAPI
CmLoadKey(IN POBJECT_ATTRIBUTES TargetKey,
          IN POBJECT_ATTRIBUTES SourceFile,
          IN ULONG Flags,
          IN PCM_KEY_BODY KeyBody)
{
    SECURITY_QUALITY_OF_SERVICE ServiceQos;
    SECURITY_CLIENT_CONTEXT ClientSecurityContext;
    HANDLE KeyHandle;
    BOOLEAN Allocate = TRUE;
    PCMHIVE CmHive, LoadedHive;
    NTSTATUS Status;
    CM_PARSE_CONTEXT ParseContext;
    
    /* Check if we have a trust key */
    if (KeyBody)
    {
        /* Fail */
        DPRINT1("Trusted classes not yet supported\n");
        return STATUS_NOT_IMPLEMENTED;
    }
    
    /* Build a service QoS for a security context */
    ServiceQos.Length = sizeof(SECURITY_QUALITY_OF_SERVICE);
    ServiceQos.ImpersonationLevel = SecurityImpersonation;
    ServiceQos.ContextTrackingMode = SECURITY_DYNAMIC_TRACKING;
    ServiceQos.EffectiveOnly = TRUE;
    Status = SeCreateClientSecurity(PsGetCurrentThread(),
                                    &ServiceQos,
                                    FALSE,
                                    &ClientSecurityContext);
    if (!NT_SUCCESS(Status))
    {
        /* Fail */
        DPRINT1("Security context failed\n");
        return Status;
    }
    
    /* Open the target key */
#if 0
    Status = ZwOpenKey(&KeyHandle, KEY_READ, TargetKey);
#else
    RtlZeroMemory(&ParseContext, sizeof(ParseContext));
    ParseContext.CreateOperation = FALSE;
    Status = ObOpenObjectByName(TargetKey,
                                CmpKeyObjectType,
                                KernelMode,
                                NULL,
                                KEY_READ,
                                &ParseContext,
                                &KeyHandle);
#endif
    if (!NT_SUCCESS(Status)) KeyHandle = NULL;

    /* Open the hive */
    Status = CmpCmdHiveOpen(SourceFile,
                            &ClientSecurityContext,
                            &Allocate,
                            &CmHive,
                            0);

    /* Get rid of the security context */
    SeDeleteClientSecurity(&ClientSecurityContext);

    /* See if we failed */
    if (!NT_SUCCESS(Status))
    {
        /* See if the target already existed */
        if (KeyHandle)
        {
            /* Lock the registry */
            CmpLockRegistryExclusive();
            
            /* Check if we are already loaded */
            if (CmpIsHiveAlreadyLoaded(KeyHandle, SourceFile, &LoadedHive))
            {
                /* That's okay then */
                ASSERT(LoadedHive);
                Status = STATUS_SUCCESS;
            }

            /* Release the registry */
            CmpUnlockRegistry();
        }
        
        /* Close the key handle if we had one */
        if (KeyHandle) ZwClose(KeyHandle);
        return Status;
    }
    
    /* Lock the registry shared */
    CmpLockRegistry();
    
    /* Lock loading */
    ExAcquirePushLockExclusive(&CmpLoadHiveLock);
    
    /* Lock the hive to this thread */
    CmHive->Hive.HiveFlags |= HIVE_IS_UNLOADING;
    CmHive->CreatorOwner = KeGetCurrentThread();
    
    /* Set flag */
    if (Flags & REG_NO_LAZY_FLUSH) CmHive->Hive.HiveFlags |= HIVE_NOLAZYFLUSH;
    
    /* Link the hive */
    Status = CmpLinkHiveToMaster(TargetKey->ObjectName,
                                 TargetKey->RootDirectory,
                                 CmHive,
                                 Allocate,
                                 TargetKey->SecurityDescriptor);
    if (NT_SUCCESS(Status))
    {
        /* Add to HiveList key */
        CmpAddToHiveFileList(CmHive);
        
        /* Sync the hive if necessary */
        if (Allocate)
        {
            /* Sync it under the flusher lock */
            CmpLockHiveFlusherExclusive(CmHive);
            HvSyncHive(&CmHive->Hive);
            CmpUnlockHiveFlusher(CmHive);
        }
        
        /* Release the hive */
        CmHive->Hive.HiveFlags &= ~HIVE_IS_UNLOADING;
        CmHive->CreatorOwner = NULL;
        
        /* Allow loads */
        ExReleasePushLock(&CmpLoadHiveLock);
    }
    else
    {
        /* FIXME: TODO */
        ASSERT(FALSE);
    }
    
    /* Is this first profile load? */
    if (!(CmpProfileLoaded) && !(CmpWasSetupBoot))
    {
        /* User is now logged on, set quotas */
        CmpProfileLoaded = TRUE;
        CmpSetGlobalQuotaAllowed();
    }
    
    /* Unlock the registry */
    CmpUnlockRegistry();
    
    /* Close handle and return */
    if (KeyHandle) ZwClose(KeyHandle);
    return Status;
}

NTSTATUS
NTAPI
CmUnloadKey(IN PCM_KEY_CONTROL_BLOCK Kcb,
            IN ULONG Flags)
{
    UNIMPLEMENTED;
    return STATUS_NOT_IMPLEMENTED;
}

ULONG
NTAPI
CmCountOpenSubKeys(IN PCM_KEY_CONTROL_BLOCK RootKcb,
                   IN BOOLEAN RemoveEmptyCacheEntries)
{
    PCM_KEY_HASH Entry;
    PCM_KEY_CONTROL_BLOCK CachedKcb;
    PCM_KEY_CONTROL_BLOCK ParentKcb;
    USHORT ParentKeyCount;
    USHORT j;
    ULONG i;
    ULONG SubKeys = 0;

    DPRINT("CmCountOpenSubKeys() called\n");

    /* The root key is the only referenced key. There are no refereced sub keys. */
    if (RootKcb->RefCount == 1)
    {
        DPRINT("open sub keys: 0\n");
        return 0;
    }

    /* Enumerate all hash lists */
    for (i = 0; i < CmpHashTableSize; i++)
    {
        /* Get the first cache entry */
        Entry = CmpCacheTable[i].Entry;

        /* Enumerate all cache entries */
        while (Entry)
        {
            /* Get the KCB of the current cache entry */
            CachedKcb = CONTAINING_RECORD(Entry, CM_KEY_CONTROL_BLOCK, KeyHash);

            /* Check keys only that are subkeys to our root key */
            if (CachedKcb->TotalLevels > RootKcb->TotalLevels)
            {
                /* Calculate the number of parent keys to the root key */
                ParentKeyCount = CachedKcb->TotalLevels - RootKcb->TotalLevels;

                /* Find a parent key that could be the root key */
                ParentKcb = CachedKcb;
                for (j = 0; j < ParentKeyCount; j++)
                {
                    ParentKcb = ParentKcb->ParentKcb;
                }

                /* Check whether the parent is the root key */
                if (ParentKcb == RootKcb)
                {
                    DPRINT("Found a sub key \n");
                    DPRINT("RefCount = %u\n", CachedKcb->RefCount);

                    if (CachedKcb->RefCount > 0)
                    {
                        /* Count the current hash entry if it is in use */
                        SubKeys++;
                    }
                    else if ((CachedKcb->RefCount == 0) && (RemoveEmptyCacheEntries == TRUE))
                    {
                        /* Remove the current key from the delayed close list */
                        CmpRemoveFromDelayedClose(CachedKcb);

                        /* Remove the current cache entry */
                        CmpCleanUpKcbCacheWithLock(CachedKcb, TRUE);

                        /* Restart, because the hash list has changed */
                        Entry = CmpCacheTable[i].Entry;
                        continue;
                    }
                }
            }

            /* Get the next cache entry */
            Entry = Entry->NextHash;
        }
    }

    DPRINT("open sub keys: %u\n", SubKeys);

    return SubKeys;
}
