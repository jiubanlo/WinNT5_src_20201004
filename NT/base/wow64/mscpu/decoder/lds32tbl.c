/*++

Copyright (c) 1995 Microsoft Corporation

Module Name:

    lds32tbl.c

Abstract:
    
    Dispatch table for 32bit instructions with the LOCK prefix.

Author:

    23-Aug-1995 Ori Gershony (t-orig)

Revision History:

--*/

#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>
#include <windows.h>
#include <stdio.h>
#include "threadst.h"
#include "instr.h"
#include "decoderp.h"

#include "bytefns.h"
#include "dwordfns.h"
#include "miscfns.h"
#include "floatfns.h"

pfnDispatchInstruction LockDispatch32[256] = {
    // 0
    LOCKadd_m_r8,
    LOCKadd_m_r32,
    LOCKadd_r_m8,
    LOCKadd_r_m32,
    bad,
    bad,
    bad,
    bad,
    // 8
    LOCKor_m_r8,
    LOCKor_m_r32,
    LOCKor_r_m8,
    LOCKor_r_m32,
    bad,
    bad,
    bad,
    LOCKdispatch232,
    // 10
    LOCKadc_m_r8,
    LOCKadc_m_r32,
    LOCKadc_r_m8,
    LOCKadc_r_m32,
    bad,
    bad,
    bad,
    bad,
    // 18
    LOCKsbb_m_r8,
    LOCKsbb_m_r32,
    LOCKsbb_r_m8,
    LOCKsbb_r_m32,
    bad,
    bad,
    bad,
    bad,
    // 20
    LOCKand_m_r8,
    LOCKand_m_r32,
    LOCKand_r_m8,
    LOCKand_r_m32,
    bad,
    bad,
    bad,
    bad,
    // 28
    LOCKsub_m_r8,
    LOCKsub_m_r32,
    LOCKsub_r_m8,
    LOCKsub_r_m32,
    bad,
    bad,
    bad,
    bad,
    // 30
    LOCKxor_m_r8,
    LOCKxor_m_r32,
    LOCKxor_r_m8,
    LOCKxor_r_m32,
    bad,
    bad,
    bad,
    bad,
    // 38
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    // 40
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    // 48
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    // 50
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    // 58
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    // 60
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    // 68
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    // 70
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    // 78
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    // 80
    LOCKGROUP_18,
    LOCKGROUP_132,
    bad,
    LOCKGROUP_1WS32,
    bad,
    bad,
    xchg_r_m8,
    xchg_r_m32,
    // 88
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    // 90
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    // 98
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    // a0
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    // a8
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    // b0
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    // b8
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    // c0
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    // c8
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    // d0
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    // d8
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    // e0
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    // e8
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    // f0
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    LOCKGROUP_38,
    LOCKGROUP_332,
    // f8
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    LOCKGROUP_532,
};


pfnDispatchInstruction LockDispatch232[256] = {
    // 0
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    // 8
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    // 10
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    // 18
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    // 20
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    // 28
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    // 30
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    // 38
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    // 40
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    // 48
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    // 50
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    // 58
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    // 60
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    // 68
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    // 70
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    // 78
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    // 80
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    // 88
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    // 90
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    // 98
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    // a0
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    // a8
    bad,
    bad,
    bad,
    LOCKbts_m_r32,
    bad,
    bad,
    bad,
    bad,
    // b0
    LOCKcmpxchg_m_r8,
    LOCKcmpxchg_m_r32,
    bad,
    LOCKbtr_m_r32,
    bad,
    bad,
    bad,
    bad,
    // b8
    bad,
    bad,
    LOCKGROUP_832,
    LOCKbtc_m_r32,
    bad,
    bad,
    bad,
    bad,
    // c0
    LOCKxadd_m_r8,
    LOCKxadd_m_r32,
    bad,
    bad,
    bad,
    bad,
    bad,
    LOCKcmpxchg8b,
    // c8
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    // d0
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    // d8
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    // e0
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    // e8
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    // f0
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    // f8
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad,
    bad
};
