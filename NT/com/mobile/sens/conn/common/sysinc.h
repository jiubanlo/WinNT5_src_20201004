/*++

Copyright (C) Microsoft Corporation, 1997 - 1999

Module Name:

    sysinc.h

Abstract:

    This contains all the platform-independent stuff for the SENS project.

Author:

    Gopal Parupudi    <GopalP>

[Notes:]

    optional-notes

Revision History:

    GopalP          3/6/1998         Start.

--*/


#ifndef __SYSINC_H__
#define __SYSINC_H__

//
// Global defines
//

#ifndef SENS_CHICAGO

#define SENS_NT
#define UNICODE
#define _UNICODE

#else // SENS_CHICAGO

#undef UNICODE
#undef _UNICODE

#endif // SENS_CHICAGO



//
// Includes
//
#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>
#include <tchar.h>
#include <stdio.h>
#include <windows.h>
#include <rpc.h>
#include <strsafe.h>



//
// Typedefs
//
typedef WCHAR               SENS_CHAR;
typedef SHORT               SENS_SCHAR;

typedef WCHAR               *PSENS_CHAR;
typedef SHORT               *PSENS_SCHAR;

//
// Defines
//
#define SENS_STRING(string)     L##string
#define SENS_BSTR(bstr)         L##bstr

//
// Function Mappings
//

//
// Threadpool Apis
//

#define SENS_TIMER_CALLBACK_RETURN          VOID NTAPI
#define SENS_LONG_ITEM                      (WT_EXECUTELONGFUNCTION)
#define SENS_TIMER_CREATE_FAILED(bStatus, hTimer) \
        (FALSE == bStatus)

// Use KERNEL32's Win32 functions
#define SensQueueUserWorkItem               QueueUserWorkItem
#define SensRegisterWaitForSingleObject     RegisterWaitForSingleObject
#define SensUnregisterWait                  UnregisterWait
#define SensCreateTimerQueue                CreateTimerQueue
#define SensDeleteTimerQueue                DeleteTimerQueue
#define SensCancelTimerQueueTimer(TimerQueue, Timer, Event) \
        DeleteTimerQueueTimer(TimerQueue, Timer, Event)      
#define SensSetTimerQueueTimer(bStatus, hTimer, hQueue, pfnCallback, pContext, dwDueTime, dwPeriod, dwFlags) \
        bStatus = CreateTimerQueueTimer(&hTimer, hQueue, pfnCallback, pContext, dwDueTime, dwPeriod, SENS_LONG_ITEM)        

//
// Output Macros and functions
//
#ifdef DBG

//
// Currently these macros get preprocesed as some some variants of printf.
// Eventually, these will be replaced by a function more comprehensive than
// printf.
//
// Notes:
//
// o SensDbgPrintW works like ntdll!DbgPrint() except that it can handle wide
//   strings.
//
#define SensPrint(_LEVEL_, _X_)             SensDbgPrintW _X_
#define SensPrintA(_LEVEL_, _X_)            SensDbgPrintA _X_
#define SensPrintW(_LEVEL_, _X_)            SensDbgPrintW _X_
#define SensPrintToDebugger(_LEVEL_, _X_)   DbgPrint      _X_
#define SensBreakPoint()                    DebugBreak()

#else // RETAIL

//
// The following functions do nothing and they should be optimized and no
// code should be generated by the compiler.
//
#define SensPrint(_LEVEL_, _X_)             // Nothing
#define SensPrintA(_LEVEL_, _X_)            // Nothing
#define SensPrintW(_LEVEL_, _X_)            // Nothing
#define SensPrintToDebugger(_LEVEL_, _X_)   // Nothing
#define SensBreakPoint()                    // Nothing

#endif // DBG


#endif // __SYSINC_H__
