#ifndef _EXT2_H_
#define _EXT2_H_

#include <Uefi.h>

#include <Guid/FileInfo.h>
#include <Guid/FileSystemInfo.h>
#include <Guid/FileSystemVolumeLabelInfo.h>
#include <Protocol/BlockIo.h>
#include <Protocol/DiskIo.h>
#include <Protocol/SimpleFileSystem.h>
#include <Protocol/UnicodeCollation.h>

#include <Library/PcdLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>

//
#define IS_LEAP_YEAR(a)                   (((a) % 4 == 0) && (((a) % 100 != 0) || ((a) % 400 == 0)))

#define EFI_PATH_STRING_LENGTH  260
#define EFI_FILE_STRING_LENGTH  255

#define CACHE_ENABLED(a)  ((a) >= 2)
#define RAW_ACCESS(a)     ((IO_MODE)((a) & 0x1))
#define CACHE_TYPE(a)     ((CACHE_DATA_TYPE)((a) >> 2))

//
// Global Variables
//
extern EFI_DRIVER_BINDING_PROTOCOL     gExtDriverBinding;
extern EFI_COMPONENT_NAME_PROTOCOL     gExtComponentName;
extern EFI_COMPONENT_NAME2_PROTOCOL    gExtComponentName2;
extern EFI_LOCK                        ExtFsLock;
extern EFI_FILE_PROTOCOL               ExtFileInterface;

#endif
