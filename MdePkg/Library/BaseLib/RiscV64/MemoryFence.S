##------------------------------------------------------------------------------
#
# MemoryFence() for RiscV64

# Copyright (c) 2021, Hewlett Packard Enterprise Development. All rights reserved.
#
# SPDX-License-Identifier: BSD-2-Clause-Patent
#
##------------------------------------------------------------------------------

.text
.p2align 2

ASM_GLOBAL ASM_PFX(MemoryFence)


#/**
#  Used to serialize load and store operations.
#
#  All loads and stores that proceed calls to this function are guaranteed to be
#  globally visible when this function returns.
#
#**/
#VOID
#EFIAPI
#MemoryFence (
#  VOID
#  );
#
ASM_PFX(MemoryFence):
    // Fence on all memory and I/O
    fence
    ret
