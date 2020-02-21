/** @file
  Main file for embeddedramdisk shell Debug1 function.

  (C) Copyright 2020 Hewlett-Packard Development Company, L.P.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "UefiShellDebug1CommandsLib.h"
#include <Uefi/UefiBaseType.h>
#include <Uefi/UefiSpec.h>
#include <PiDxe.h>
#include <Library/BaseLib.h>
#include <Library/ShellLib.h>
#include <Library/IoLib.h>
#include <Protocol/DeviceIo.h>
#include <Library/PeCoffLib.h>
#include <Library/DxeServicesLib.h>
#include <Library/DxeServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/CacheMaintenanceLib.h>
#include <Guid/MemoryProfile.h>
#include <Protocol/RamDisk.h>

STATIC CONST SHELL_PARAM_ITEM ParamList[] = {
  {L"-g", TypeValue},
  {NULL, TypeMax}
  };

typedef enum  {
  ImageTypeVirtualCd,
  ImageTypeVirtualDisk,
} EMBEDDED_RAMDISK_TYPE_TYPE;

// Taken from NetworkPkg/HttpBootDxe/HttpBootSupport.c
/**
  This function register the RAM disk info to the system.

  @param[in]       BufferSize      The size of Buffer in bytes.
  @param[in]       Buffer          The base address of the RAM disk.
  @param[in]       ImageType       The image type of the file in Buffer.

  @retval EFI_SUCCESS              The RAM disk has been registered.
  @retval EFI_NOT_FOUND            No RAM disk protocol instances were found.
  @retval EFI_UNSUPPORTED          The ImageType is not supported.
  @retval Others                   Unexpected error happened.

**/
EFI_STATUS
HttpBootRegisterRamDisk (
  IN  UINTN                        BufferSize,
  IN  VOID                        *Buffer,
  IN  EMBEDDED_RAMDISK_TYPE_TYPE   ImageType
  )
{
  EFI_RAM_DISK_PROTOCOL      *RamDisk;
  EFI_STATUS                 Status;
  EFI_DEVICE_PATH_PROTOCOL   *DevicePath;
  EFI_GUID                   *RamDiskType;

  ASSERT (Buffer != NULL);
  ASSERT (BufferSize != 0);

  Status = gBS->LocateProtocol (&gEfiRamDiskProtocolGuid, NULL, (VOID**) &RamDisk);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "HTTP Boot: Couldn't find the RAM Disk protocol - %r\n", Status));
    return Status;
  }

  if (ImageType == ImageTypeVirtualCd) {
    RamDiskType = &gEfiVirtualCdGuid;
  } else if (ImageType == ImageTypeVirtualDisk) {
    RamDiskType = &gEfiVirtualDiskGuid;
  } else {
    return EFI_UNSUPPORTED;
  }

  Status = RamDisk->Register (
             (UINTN)Buffer,
             (UINT64)BufferSize,
             RamDiskType,
             NULL,
             &DevicePath
             );
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "HTTP Boot: Failed to register RAM Disk - %r\n", Status));
  }

  return Status;
}

// Mostly taken from MdeModulePkg/Universal/Acpi/BootScriptExecutorDxe/ScriptExecute.c
VOID
EFIAPI
MountRamdisk (
  EFI_GUID Guid
  )
{
  EFI_STATUS                                    Status;
  UINT8                                         *Buffer;
  UINTN                                         BufferSize;
  EFI_HANDLE                                    NewImageHandle;

  //
  // A workaround: Here we install a dummy handle
  //
  NewImageHandle = NULL;
  Status = gBS->InstallProtocolInterface (
                  &NewImageHandle,
                  &Guid,
                  EFI_NATIVE_INTERFACE,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);

  //
  // Load BootScriptExecutor image itself to RESERVED mem
  //
  Status = GetSectionFromAnyFv  (
             &Guid,
             EFI_SECTION_RAW,
             0,
             (VOID **) &Buffer,
             &BufferSize
             );
  if (EFI_ERROR (Status)) {
    //DEBUG ((EFI_D_ERROR, "Loading image of size %d into ramdisk\n", BufferSize));
    ASSERT_EFI_ERROR (Status);
    return;
  }

  DEBUG ((EFI_D_ERROR, "Loading image of size %d into ramdisk\n", BufferSize));

  Status = HttpBootRegisterRamDisk(BufferSize, Buffer, ImageTypeVirtualCd);
  ASSERT_EFI_ERROR (Status);
}

/**
  Function for 'execembedded' command.

  @param[in] ImageHandle  Handle to the Image (NULL if Internal).
  @param[in] SystemTable  Pointer to the System Table (NULL if Internal).
**/
SHELL_STATUS
EFIAPI
ShellCommandRunEmbeddedRamdisk (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS                            Status;
  UINTN                                 Size;
  LIST_ENTRY                           *Package;
  CHAR16                               *ProblemParam;
  SHELL_STATUS                          ShellStatus;
  CONST CHAR16                         *GuidStr;
  EFI_GUID                              Guid;

  ShellStatus = SHELL_SUCCESS;
  Size = 1;

  //
  // initialize the shell lib (we must be in non-auto-init...)
  //
  Status = ShellInitialize();
  ASSERT_EFI_ERROR(Status);

  //
  // Parse arguments
  //
  Status = ShellCommandLineParse (ParamList, &Package, &ProblemParam, TRUE);
  if (EFI_ERROR (Status)) {
    if (Status == EFI_VOLUME_CORRUPTED && ProblemParam != NULL) {
      ShellPrintHiiEx (-1, -1, NULL, STRING_TOKEN (STR_GEN_PROBLEM), gShellDebug1HiiHandle, L"execembedded", ProblemParam);
      FreePool (ProblemParam);
      ShellStatus = SHELL_INVALID_PARAMETER;
      goto Done;
    } else {
      ASSERT (FALSE);
    }
  } else {
    if (ShellCommandLineGetCount(Package) > 2) {
      ShellPrintHiiEx (-1, -1, NULL, STRING_TOKEN (STR_GEN_NO_VALUE), gShellDebug1HiiHandle, L"execembedded");
      ShellStatus = SHELL_INVALID_PARAMETER;
      goto Done;
    }

    GuidStr = ShellCommandLineGetRawValue(Package, 1);
    if (GuidStr == NULL) {
      ShellPrintHiiEx (-1, -1, NULL, STRING_TOKEN (STR_GEN_PARAM_INV), gShellDebug1HiiHandle, L"execembedded");
      ShellStatus = SHELL_INVALID_PARAMETER;
      goto Done;
    }

    ShellPrintEx (-1, -1, L"GuidStr: %s\r\n", GuidStr);

    StrToGuid (GuidStr, &Guid);
    //Guid = mUefiShellHelloWorldGuid;

    ShellPrintEx (-1, -1, L"Mounting selected section as ramdisk...\r\n");
    MountRamdisk (Guid);
    //ShellPrintEx (-1, -1, L"Hello World2!\r\n");

  }
  ASSERT (ShellStatus == SHELL_SUCCESS);

Done:
  //if (GuidStr != NULL) {
  //  FreePool (GuidStr);
  //}
  if (Package != NULL) {
    ShellCommandLineFreeVarList (Package);
  }
  return ShellStatus;
}
