/** @file
  Random number generator services that uses seed CSR access
  to provide high-quality random numbers.

Copyright (c) 2021, NUVIA Inc. All rights reserved.<BR>
Copyright (c) 2015, Intel Corporation. All rights reserved.<BR>
Copyright (c) 2021, Hewlett Packard Enterprise Development LP. All rights reserved.<BR>

SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>

#include "BaseRngLibInternals.h"


STATIC BOOLEAN mSeedSupported;

/**
  The constructor function checks whether or not RDRAND instruction is supported
  by the host hardware.

  The constructor function checks whether or not RDRAND instruction is supported.
  It will ASSERT() if RDRAND instruction is not supported.
  It will always return EFI_SUCCESS.

  @retval EFI_SUCCESS   The constructor always returns EFI_SUCCESS.

**/
EFI_STATUS
EFIAPI
BaseRngLibConstructor (
  VOID
  )
{
  // TODO: Determine if seed CSR is supported
  mSeedSupported = FALSE;

  return EFI_SUCCESS;
}

/**
  Generates a 16-bit random number.

  @param[out] Rand     Buffer pointer to store the 16-bit random value.

  @retval TRUE         Random number generated successfully.
  @retval FALSE        Failed to generate the random number.

**/
BOOLEAN
EFIAPI
ArchGetRandomNumber16 (
  OUT     UINT16                    *Rand
  )
{
  return FALSE;
}

/**
  Generates a 32-bit random number.

  @param[out] Rand     Buffer pointer to store the 32-bit random value.

  @retval TRUE         Random number generated successfully.
  @retval FALSE        Failed to generate the random number.

**/
BOOLEAN
EFIAPI
ArchGetRandomNumber32 (
  OUT     UINT32                    *Rand
  )
{
  return FALSE;
}

/**
  Generates a 64-bit random number.

  @param[out] Rand     Buffer pointer to store the 64-bit random value.

  @retval TRUE         Random number generated successfully.
  @retval FALSE        Failed to generate the random number.

**/
BOOLEAN
EFIAPI
ArchGetRandomNumber64 (
  OUT     UINT64                    *Rand
  )
{
  return FALSE;
}

/**
  Checks whether RDRAND is supported.

  @retval TRUE         RDRAND is supported.
  @retval FALSE        RDRAND is not supported.

**/
BOOLEAN
EFIAPI
ArchIsRngSupported (
  VOID
  )
{
  /*
     Existing software depends on this always returning TRUE, so for
     now hard-code it.

     return mRdRandSupported;
  */
  return mSeedSupported;
}
