/*
 * PROJECT:         ReactOS Kernel
 * LICENSE:         GPL - See COPYING in the top level directory
 * FILE:            ntoskrnl/config/cmapi.c
 * PURPOSE:         Configuration Manager - Internal Registry APIs
 * PROGRAMMERS:     Alex Ionescu (alex.ionescu@reactos.org)
 */

/* INCLUDES ******************************************************************/

#include "ntoskrnl.h"
#include "cm.h"
#define NDEBUG
#include "debug.h"

/* FUNCTIONS *****************************************************************/

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
        HvMarkCellDirty(Hive, Parent->ValueList.List);
    }

    /* Allocate  avalue cell */
    ValueCell = HvAllocateCell(Hive,
                               FIELD_OFFSET(CM_KEY_VALUE, Name) +
                               CmpNameSize(Hive, ValueName),
                               StorageType);
    if (ValueCell == HCELL_NIL) return STATUS_INSUFFICIENT_RESOURCES;

    /* Get the actual data for it */
    CellData = HvGetCell(Hive, ValueCell);
    if (!CellData) ASSERT(FALSE);

    /* Now we can release it, make sure it's also dirty */
    HvReleaseCell(Hive, ValueCell);
    ASSERT(HvIsCellDirty(Hive, ValueCell));

    /* Set it up and copy the name */
    CellData->u.KeyValue.Signature = CM_KEY_VALUE_SIGNATURE;
    CellData->u.KeyValue.Flags = 0;
    CellData->u.KeyValue.Type = Type;
    CellData->u.KeyValue.NameLength = CmpCopyName(Hive,
                                                  CellData->u.KeyValue.Name,
                                                  ValueName);
    if (CellData->u.KeyValue.NameLength < ValueName->Length)
    {
        /* This is a compressed name */
        CellData->u.KeyValue.Flags = VALUE_COMP_NAME;
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

    /* Add this value cell to the child list */
    Status = CmpAddValueToList(Hive,
                               ValueCell,
                               Index,
                               StorageType,
                               &Parent->ValueList);

    /* If we failed, free the entire cell, including the data */
    if (!NT_SUCCESS(Status)) CmpFreeValue(Hive, ValueCell);

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

    /* Mark the old child cell dirty */
    HvMarkCellDirty(Hive, OldChild);

    /* See if this is a small or normal key */
    WasSmall = CmpIsKeyValueSmall(&Length, Value->DataLength);

    /* See if our new data can fit in a small key */
    IsSmall = (DataSize <= CM_KEY_VALUE_SMALL) ? TRUE: FALSE;

    /* Big keys are unsupported */
    ASSERT_VALUE_BIG(Hive, Length);
    ASSERT_VALUE_BIG(Hive, DataSize);

    /* Mark the old value dirty */
    CmpMarkValueDataDirty(Hive, Value);

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
    else
    {
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
            NewCell = HvAllocateCell(Hive, DataSize, StorageType);
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
}

NTSTATUS
NTAPI
CmSetValueKey(IN PKEY_OBJECT KeyObject,
              IN PUNICODE_STRING ValueName,
              IN ULONG Type,
              IN PVOID Data,
              IN ULONG DataLength)
{
    PHHIVE Hive;
    PCM_KEY_NODE Parent;
    PCM_KEY_VALUE Value = NULL;
    HCELL_INDEX CurrentChild, Cell;
    NTSTATUS Status;
    BOOLEAN Found, Result;
    ULONG Count, ChildIndex, SmallData, Storage;

    /* Acquire hive lock exclusively */
    KeEnterCriticalRegion();
    ExAcquireResourceExclusiveLite(&CmpRegistryLock, TRUE);

    /* Get pointer to key cell */
    Parent = KeyObject->KeyCell;
    Hive = &KeyObject->RegistryHive->Hive;
    Cell = KeyObject->KeyCellOffset;

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
            /* Get its value */
            Value = (PCM_KEY_VALUE)HvGetCell(Hive, CurrentChild);
            if (!Value)
            {
                /* Fail */
                Status = STATUS_INSUFFICIENT_RESOURCES;
                goto Quickie;
            }

            /* Remember that we found it */
            Found = TRUE;
        }
    }
    else
    {
        /* No child list, we'll need to add it */
        ChildIndex = 0;
    }

    /* Mark the cell dirty */
    HvMarkCellDirty(Hive, Cell);

    /* Get the storage type */
    Storage = HvGetCellType(Cell);

    /* Check if this is small data */
    SmallData = 0;
    if ((DataLength <= CM_KEY_VALUE_SMALL) && (DataLength > 0))
    {
        /* Copy it */
        RtlCopyMemory(&SmallData, Data, DataLength);
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

    /* Mark link key */
    if ((Type == REG_LINK) &&
        (_wcsicmp(ValueName->Buffer, L"SymbolicLinkValue") == 0))
    {
        Parent->Flags |= KEY_SYM_LINK;
    }

    /* Check for success */
Quickie:
    if (NT_SUCCESS(Status))
    {
        /* Save the write time */
        KeQuerySystemTime(&Parent->LastWriteTime);
    }

    /* Release the lock */
    ExReleaseResourceLite(&CmpRegistryLock);
    KeLeaveCriticalRegion();
    return Status;
}

NTSTATUS
NTAPI
CmDeleteValueKey(IN PKEY_OBJECT KeyObject,
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
    KeEnterCriticalRegion();
    ExAcquireResourceExclusiveLite(&CmpRegistryLock, TRUE);

    /* Get the hive and the cell index */
    Hive = &KeyObject->RegistryHive->Hive;
    Cell = KeyObject->KeyCellOffset;

    /* Get the parent key node */
    Parent = (PCM_KEY_NODE)HvGetCell(Hive, Cell);
    if (!Parent)
    {
        /* Fail */
        Status = STATUS_INSUFFICIENT_RESOURCES;
        goto Quickie;
    }

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
        HvMarkCellDirty(Hive, Cell);
        HvMarkCellDirty(Hive, Parent->ValueList.List);
        HvMarkCellDirty(Hive, ChildCell);

        /* Get the key value */
        Value = (PCM_KEY_VALUE)HvGetCell(Hive,ChildCell);
        if (!Value) ASSERT(FALSE);

        /* Mark it and all related data as dirty */
        CmpMarkValueDataDirty(Hive, Value);

        /* Ssanity checks */
        ASSERT(HvIsCellDirty(Hive, Parent->ValueList.List));
        ASSERT(HvIsCellDirty(Hive, ChildCell));

        /* Remove the value from the child list */
        Status = CmpRemoveValueFromList(Hive, ChildIndex, ChildList);
        if(!NT_SUCCESS(Status)) goto Quickie;

        /* Remove the value and its data itself */
        if (!CmpFreeValue(Hive, ChildCell))
        {
            /* Failed to free the value, fail */
            Status = STATUS_INSUFFICIENT_RESOURCES;
            goto Quickie;
        }

        /* Set the last write time */
        KeQuerySystemTime(&Parent->LastWriteTime);

        /* Sanity check */
        ASSERT(HvIsCellDirty(Hive, Cell));

        /* Check if the value list is empty now */
        if (!Parent->ValueList.Count)
        {
            /* Then clear key node data */
            Parent->MaxValueNameLen = 0;
            Parent->MaxValueDataLen = 0;
        }

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

    /* Release hive lock */
    ExReleaseResourceLite(&CmpRegistryLock);
    KeLeaveCriticalRegion();
    return Status;
}

NTSTATUS
NTAPI
CmQueryValueKey(IN PKEY_OBJECT KeyObject,
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
    KeEnterCriticalRegion();
    ExAcquireResourceExclusiveLite(&CmpRegistryLock, TRUE);

    /* Get the hive */
    Hive = &KeyObject->RegistryHive->Hive;

    /* Find the key value */
    Result = CmpFindValueByNameFromCache(KeyObject,
                                         &ValueName,
                                         &CachedValue,
                                         &Index,
                                         &ValueData,
                                         &ValueCached,
                                         &CellToRelease);
    if (Result == SearchSuccess)
    {
        /* Sanity check */
        ASSERT(ValueData != NULL);

        /* Query the information requested */
        Result = CmpQueryKeyValueData(KeyObject,
                                      CachedValue,
                                      ValueData,
                                      ValueCached,
                                      KeyValueInformationClass,
                                      KeyValueInformation,
                                      Length,
                                      ResultLength,
                                      &Status);
    }
    else
    {
        /* Failed to find the value */
        Status = STATUS_OBJECT_NAME_NOT_FOUND;
    }

    /* If we have a cell to release, do so */
    if (CellToRelease != HCELL_NIL) HvReleaseCell(Hive, CellToRelease);

    /* Release hive lock */
    ExReleaseResourceLite(&CmpRegistryLock);
    KeLeaveCriticalRegion();
    return Status;
}
