/* $Id: iface.c,v 1.58 2001/10/10 22:18:58 hbirr Exp $
 *
 * COPYRIGHT:        See COPYING in the top level directory
 * PROJECT:          ReactOS kernel
 * FILE:             services/fs/vfat/iface.c
 * PURPOSE:          VFAT Filesystem
 * PROGRAMMER:       Jason Filby (jasonfilby@yahoo.com)
 * UPDATE HISTORY:
 *    ??           Created
 *   24-10-1998   Fixed bugs in long filename support
 *                Fixed a bug that prevented unsuccessful file open requests
 *                being reported
 *                Now works with long filenames that span over a sector
 *                boundary
 *   28-10-1998   Reads entire FAT into memory
 *                VFatReadSector modified to read in more than one sector at a
 *                time
 *   7-11-1998    Fixed bug that assumed that directory data could be
 *                fragmented
 *   8-12-1998    Added FAT32 support
 *                Added initial writability functions
 *                WARNING: DO NOT ATTEMPT TO TEST WRITABILITY FUNCTIONS!!!
 *   12-12-1998   Added basic support for FILE_STANDARD_INFORMATION request
 *
 */

/* INCLUDES *****************************************************************/

#include <ddk/ntddk.h>

#define NDEBUG
#include <debug.h>

#include "vfat.h"

/* GLOBALS *****************************************************************/

#define  CACHEPAGESIZE(pDeviceExt) ((pDeviceExt)->BytesPerCluster > PAGESIZE ? \
		   (pDeviceExt)->BytesPerCluster : PAGESIZE)

static PDRIVER_OBJECT VfatDriverObject;

/* FUNCTIONS ****************************************************************/

static NTSTATUS
VfatHasFileSystem(PDEVICE_OBJECT DeviceToMount,
		  PBOOLEAN RecognizedFS)
/*
 * FUNCTION: Tests if the device contains a filesystem that can be mounted
 *           by this fsd
 */
{
   BootSector *Boot;
   NTSTATUS Status;

   Boot = ExAllocatePool(NonPagedPool, 512);

   Status = VfatReadSectors(DeviceToMount, 0, 1, (UCHAR *) Boot);
   if (!NT_SUCCESS(Status))
     {
	return(Status);
     }

   DPRINT("Boot->SysType %.5s\n", Boot->SysType);
   if (strncmp(Boot->SysType, "FAT12", 5) == 0 ||
       strncmp(Boot->SysType, "FAT16", 5) == 0 ||
       strncmp(((struct _BootSector32 *) (Boot))->SysType, "FAT32", 5) == 0)
     {
	*RecognizedFS = TRUE;
     }
   else
     {
	*RecognizedFS = FALSE;
     }

   ExFreePool(Boot);

   return(STATUS_SUCCESS);
}


static NTSTATUS
VfatMountDevice(PDEVICE_EXTENSION DeviceExt,
		PDEVICE_OBJECT DeviceToMount)
/*
 * FUNCTION: Mounts the device
 */
{
   NTSTATUS Status;

   DPRINT("Mounting VFAT device...");
   DPRINT("DeviceExt %x\n", DeviceExt);

   DeviceExt->Boot = ExAllocatePool(NonPagedPool, 512);

   Status = VfatReadSectors(DeviceToMount, 0, 1, (UCHAR *) DeviceExt->Boot);
   if (!NT_SUCCESS(Status))
     {
	return(Status);
     }

   DeviceExt->FATStart = DeviceExt->Boot->ReservedSectors;
   DeviceExt->rootDirectorySectors =
     (DeviceExt->Boot->RootEntries * 32) / DeviceExt->Boot->BytesPerSector;
   DeviceExt->rootStart =
     DeviceExt->FATStart +
     DeviceExt->Boot->FATCount * DeviceExt->Boot->FATSectors;
   DeviceExt->dataStart =
     DeviceExt->rootStart + DeviceExt->rootDirectorySectors;
   DeviceExt->BytesPerSector = DeviceExt->Boot->BytesPerSector;
   DeviceExt->FATEntriesPerSector = DeviceExt->Boot->BytesPerSector / 32;
   DeviceExt->BytesPerCluster = DeviceExt->Boot->SectorsPerCluster *
     DeviceExt->Boot->BytesPerSector;

   if (DeviceExt->BytesPerCluster >= PAGESIZE && 
       (DeviceExt->BytesPerCluster % PAGESIZE) != 0)
     {
	DbgPrint("Invalid cluster size\n");
	KeBugCheck(0);
     }
   else if (DeviceExt->BytesPerCluster < PAGESIZE &&
	    (PAGESIZE % DeviceExt->BytesPerCluster) != 0)
     {
	DbgPrint("Invalid cluster size2\n");
	KeBugCheck(0);
     }

   if (strncmp (DeviceExt->Boot->SysType, "FAT12", 5) == 0)
     {
	DbgPrint("FAT12\n");
	DeviceExt->FatType = FAT12;
     }
   else if (strncmp
	    (((struct _BootSector32 *) (DeviceExt->Boot))->SysType, "FAT32",
	    5) == 0)
     {
	DbgPrint("FAT32\n");
	DeviceExt->FatType = FAT32;
	DeviceExt->rootDirectorySectors = DeviceExt->Boot->SectorsPerCluster;
	DeviceExt->dataStart = DeviceExt->FATStart + DeviceExt->Boot->FATCount
	  * ((struct _BootSector32 *) (DeviceExt->Boot))->FATSectors32;
	DeviceExt->rootStart = ClusterToSector (DeviceExt,
	  ((struct _BootSector32 *)(DeviceExt->Boot))->RootCluster);
     }
   else
     {
	DbgPrint("FAT16\n");
	DeviceExt->FatType = FAT16;
     }

   return(STATUS_SUCCESS);
}


static NTSTATUS
VfatMount (PDEVICE_OBJECT DeviceToMount)
/*
 * FUNCTION: Mount the filesystem
 */
{
  PDEVICE_OBJECT DeviceObject;
  PDEVICE_EXTENSION DeviceExt;
  BOOLEAN RecognizedFS;
  NTSTATUS Status;
  PVFATFCB Fcb;
  PVFATCCB Ccb;

  Status = VfatHasFileSystem (DeviceToMount, &RecognizedFS);
  if (!NT_SUCCESS(Status))
    {
      return(Status);
    }

  if (RecognizedFS == FALSE)
    {
      DPRINT("VFAT: Unrecognized Volume\n");
      return(STATUS_UNRECOGNIZED_VOLUME);
    }

  DPRINT("VFAT: Recognized volume\n");

  Status = IoCreateDevice(VfatDriverObject,
			  sizeof (DEVICE_EXTENSION),
			  NULL,
			  FILE_DEVICE_FILE_SYSTEM,
			  0,
			  FALSE,
			  &DeviceObject);
  if (!NT_SUCCESS(Status))
    {
      return(Status);
    }

  DeviceObject->Flags = DeviceObject->Flags | DO_DIRECT_IO;
  DeviceExt = (PVOID) DeviceObject->DeviceExtension;
  /* use same vpb as device disk */
  DeviceObject->Vpb = DeviceToMount->Vpb;
  Status = VfatMountDevice(DeviceExt,
			   DeviceToMount);
  if (!NT_SUCCESS(Status))
    {
      /* FIXME: delete device object */
      return(Status);
    }

#if 1
  DbgPrint("BytesPerSector:     %d\n", DeviceExt->Boot->BytesPerSector);
  DbgPrint("SectorsPerCluster:  %d\n", DeviceExt->Boot->SectorsPerCluster);
  DbgPrint("ReservedSectors:    %d\n", DeviceExt->Boot->ReservedSectors);
  DbgPrint("FATCount:           %d\n", DeviceExt->Boot->FATCount);
  DbgPrint("RootEntries:        %d\n", DeviceExt->Boot->RootEntries);
  DbgPrint("Sectors:            %d\n", DeviceExt->Boot->Sectors);
  DbgPrint("FATSectors:         %d\n", DeviceExt->Boot->FATSectors);
  DbgPrint("SectorsPerTrack:    %d\n", DeviceExt->Boot->SectorsPerTrack);
  DbgPrint("Heads:              %d\n", DeviceExt->Boot->Heads);
  DbgPrint("HiddenSectors:      %d\n", DeviceExt->Boot->HiddenSectors);
  DbgPrint("SectorsHuge:        %d\n", DeviceExt->Boot->SectorsHuge);
  DbgPrint("RootStart:          %d\n", DeviceExt->rootStart);
  DbgPrint("DataStart:          %d\n", DeviceExt->dataStart);
  if (DeviceExt->FatType == FAT32)
    {
      DbgPrint("FATSectors32:       %d\n",
	       ((struct _BootSector32*)(DeviceExt->Boot))->FATSectors32);
      DbgPrint("RootCluster:        %d\n",
	       ((struct _BootSector32*)(DeviceExt->Boot))->RootCluster);
      DbgPrint("FSInfoSector:       %d\n",
	       ((struct _BootSector32*)(DeviceExt->Boot))->FSInfoSector);
      DbgPrint("BootBackup:         %d\n",
	       ((struct _BootSector32*)(DeviceExt->Boot))->BootBackup);
    }
#endif
  DeviceObject->Vpb->Flags |= VPB_MOUNTED;
  DeviceExt->StorageDevice = IoAttachDeviceToDeviceStack(DeviceObject,
							 DeviceToMount);

  DeviceExt->FATFileObject = IoCreateStreamFileObject(NULL, DeviceExt->StorageDevice);
  Fcb = vfatNewFCB(NULL);
  if (Fcb == NULL)
  {
    return STATUS_INSUFFICIENT_RESOURCES;
  }
  Ccb = ExAllocatePoolWithTag (NonPagedPool, sizeof (VFATCCB), TAG_CCB);
  if (Ccb == NULL)
  {
    return STATUS_INSUFFICIENT_RESOURCES;
  }
  memset(Ccb, 0, sizeof (VFATCCB));
  DeviceExt->FATFileObject->Flags = DeviceExt->FATFileObject->Flags | FO_FCB_IS_VALID | FO_DIRECT_CACHE_PAGING_READ;
  DeviceExt->FATFileObject->FsContext = (PVOID) &Fcb->RFCB;
  DeviceExt->FATFileObject->FsContext2 = Ccb;
  Ccb->pFcb = Fcb;
  Ccb->PtrFileObject = DeviceExt->FATFileObject;
  Fcb->FileObject = DeviceExt->FATFileObject;
  Fcb->pDevExt = (PDEVICE_EXTENSION)DeviceExt->StorageDevice;

  Fcb->Flags = FCB_IS_FAT;

  if (DeviceExt->FatType == FAT32)
  {
    Fcb->RFCB.FileSize.QuadPart = ((struct _BootSector32 *)DeviceExt->Boot)->FATSectors32 * BLOCKSIZE;
    Fcb->RFCB.ValidDataLength.QuadPart = ((struct _BootSector32 *)DeviceExt->Boot)->FATSectors32 * BLOCKSIZE;
    Fcb->RFCB.AllocationSize.QuadPart = ROUND_UP(((struct _BootSector32 *)DeviceExt->Boot)->FATSectors32 * BLOCKSIZE, CACHEPAGESIZE(DeviceExt));
    Status = CcRosInitializeFileCache(DeviceExt->FATFileObject, &Fcb->RFCB.Bcb, CACHEPAGESIZE(DeviceExt));
  }
  else
  {
    if (DeviceExt->FatType == FAT16)
    {
      Fcb->RFCB.FileSize.QuadPart = DeviceExt->Boot->FATSectors * BLOCKSIZE;
      Fcb->RFCB.ValidDataLength.QuadPart = DeviceExt->Boot->FATSectors * BLOCKSIZE;
      Fcb->RFCB.AllocationSize.QuadPart = ROUND_UP(DeviceExt->Boot->FATSectors * BLOCKSIZE, CACHEPAGESIZE(DeviceExt));
      Status = CcRosInitializeFileCache(DeviceExt->FATFileObject, &Fcb->RFCB.Bcb, CACHEPAGESIZE(DeviceExt));
    }
    else
    {
      Fcb->RFCB.FileSize.QuadPart = DeviceExt->Boot->FATSectors * BLOCKSIZE;
      Fcb->RFCB.ValidDataLength.QuadPart = DeviceExt->Boot->FATSectors * BLOCKSIZE;
      Fcb->RFCB.AllocationSize.QuadPart = 2 * PAGESIZE;
      Status = CcRosInitializeFileCache(DeviceExt->FATFileObject, &Fcb->RFCB.Bcb, 2 * PAGESIZE);
    }
  }
  if (!NT_SUCCESS (Status))
  {
     DbgPrint ("CcRosInitializeFileCache failed\n");
//     KeBugCheck (0);
     // FIXME: delete device object
     return(Status);
  }


  ExInitializeResourceLite(&DeviceExt->DirResource);
  ExInitializeResourceLite(&DeviceExt->FatResource);

  KeInitializeSpinLock(&DeviceExt->FcbListLock);
  InitializeListHead(&DeviceExt->FcbListHead);

  /* read serial number */
  if (DeviceExt->FatType == FAT12 || DeviceExt->FatType == FAT16)
    DeviceObject->Vpb->SerialNumber =
      ((struct _BootSector *) (DeviceExt->Boot))->VolumeID;
  else if (DeviceExt->FatType == FAT32)
    DeviceObject->Vpb->SerialNumber =
      ((struct _BootSector32 *) (DeviceExt->Boot))->VolumeID;

  /* read volume label */
  ReadVolumeLabel(DeviceExt,  DeviceObject->Vpb);

  return(STATUS_SUCCESS);
}


NTSTATUS STDCALL
VfatFileSystemControl(PDEVICE_OBJECT DeviceObject,
		      PIRP Irp)
/*
 * FUNCTION: File system control
 */
{
   PIO_STACK_LOCATION Stack = IoGetCurrentIrpStackLocation (Irp);
   NTSTATUS Status;

   switch (Stack->MinorFunction)
     {
	case IRP_MN_USER_FS_REQUEST:
	   DPRINT1("VFAT FSC: IRP_MN_USER_FS_REQUEST\n");
	   Status = STATUS_INVALID_DEVICE_REQUEST;
	   break;

	case IRP_MN_MOUNT_VOLUME:
	   Status = VfatMount(Stack->Parameters.Mount.DeviceObject);
	   break;

	case IRP_MN_VERIFY_VOLUME:
	   DPRINT1("VFAT FSC: IRP_MN_VERIFY_VOLUME\n");
	   Status = STATUS_INVALID_DEVICE_REQUEST;
	   break;

	default:
	   DPRINT1("VFAT FSC: MinorFunction %d\n", Stack->MinorFunction);
	   Status = STATUS_INVALID_DEVICE_REQUEST;
	   break;
     }

   Irp->IoStatus.Status = Status;
   Irp->IoStatus.Information = 0;

   IoCompleteRequest (Irp, IO_NO_INCREMENT);
   return (Status);
}


NTSTATUS STDCALL
DriverEntry(PDRIVER_OBJECT _DriverObject,
	    PUNICODE_STRING RegistryPath)
/*
 * FUNCTION: Called by the system to initalize the driver
 * ARGUMENTS:
 *           DriverObject = object describing this driver
 *           RegistryPath = path to our configuration entries
 * RETURNS: Success or failure
 */
{
   PDEVICE_OBJECT DeviceObject;
   UNICODE_STRING DeviceName;
   NTSTATUS Status;

   DbgPrint("VFAT 0.0.6\n");

   VfatDriverObject = _DriverObject;

   RtlInitUnicodeString(&DeviceName,
			L"\\Device\\Vfat");
   Status = IoCreateDevice(VfatDriverObject,
			   0,
			   &DeviceName,
			   FILE_DEVICE_FILE_SYSTEM,
			   0,
			   FALSE,
			   &DeviceObject);
   if (!NT_SUCCESS(Status))
     {
	return (Status);
     }

   DeviceObject->Flags = DO_DIRECT_IO;
   VfatDriverObject->MajorFunction[IRP_MJ_CLOSE] = VfatClose;
   VfatDriverObject->MajorFunction[IRP_MJ_CREATE] = VfatCreate;
   VfatDriverObject->MajorFunction[IRP_MJ_READ] = VfatRead;
   VfatDriverObject->MajorFunction[IRP_MJ_WRITE] = VfatWrite;
   VfatDriverObject->MajorFunction[IRP_MJ_FILE_SYSTEM_CONTROL] =
     VfatFileSystemControl;
   VfatDriverObject->MajorFunction[IRP_MJ_QUERY_INFORMATION] =
     VfatQueryInformation;
   VfatDriverObject->MajorFunction[IRP_MJ_SET_INFORMATION] =
     VfatSetInformation;
   VfatDriverObject->MajorFunction[IRP_MJ_DIRECTORY_CONTROL] =
     VfatDirectoryControl;
   VfatDriverObject->MajorFunction[IRP_MJ_QUERY_VOLUME_INFORMATION] =
     VfatQueryVolumeInformation;
   VfatDriverObject->MajorFunction[IRP_MJ_SET_VOLUME_INFORMATION] =
     VfatSetVolumeInformation;
   VfatDriverObject->MajorFunction[IRP_MJ_SHUTDOWN] = VfatShutdown;
   VfatDriverObject->MajorFunction[IRP_MJ_CLEANUP] = VfatCleanup;

   VfatDriverObject->DriverUnload = NULL;

   IoRegisterFileSystem(DeviceObject);

   return STATUS_SUCCESS;
}

/* EOF */
