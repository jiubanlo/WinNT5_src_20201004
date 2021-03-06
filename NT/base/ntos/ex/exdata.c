/*++

Copyright (c) 1989  Microsoft Corporation

Module Name:

    exdata.c

Abstract:

    This module contains the global read/write data for the I/O system.

Author:

    Ken Reneris (kenr)

Revision History:


--*/

#include "exp.h"

//
// Executive callbacks.
//

PCALLBACK_OBJECT ExCbSetSystemTime;
PCALLBACK_OBJECT ExCbSetSystemState;
PCALLBACK_OBJECT ExCbPowerState;

#ifdef _PNP_POWER_

//
// Work Item to scan SystemInformation levels
//

WORK_QUEUE_ITEM ExpCheckSystemInfoWorkItem;

#endif


//
// Pageable data
//

#ifdef ALLOC_DATA_PRAGMA
#pragma const_seg("PAGECONST")
#endif

#ifdef _PNP_POWER_

const WCHAR ExpWstrSystemInformation[] = L"Control\\System Information";
const WCHAR ExpWstrSystemInformationValue[] = L"Value";

#endif

//
// Initialization time data
//

#ifdef ALLOC_DATA_PRAGMA
#pragma const_seg("INITCONST")
#endif

const WCHAR ExpWstrCallback[] = L"\\Callback";

const EXP_INITIALIZE_GLOBAL_CALLBACKS  ExpInitializeCallback[] = {
    &ExCbSetSystemTime,             L"\\Callback\\SetSystemTime",
    &ExCbSetSystemState,            L"\\Callback\\SetSystemState",
    &ExCbPowerState,                L"\\Callback\\PowerState",
    NULL,                           NULL
};

#ifdef ALLOC_DATA_PRAGMA
#pragma data_seg("PAGEDATA")
#endif

#ifdef _PNP_POWER_

LONG ExpCheckSystemInfoBusy = 0;

#endif

#ifdef ALLOC_DATA_PRAGMA
#pragma data_seg()
#pragma const_seg()
#endif

