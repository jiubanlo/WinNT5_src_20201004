//+---------------------------------------------------------------------------
//
//  Microsoft Windows
//  Copyright (C) Microsoft Corporation, 1992 - 1994.
//
//  File:       ffail.cxx
//
//  Contents:   Debug functions that you don't want to step into in
//              the debugger.  This module is compiled without the /Zi flag.
//
//----------------------------------------------------------------------------

#include "headers.h"

#if _DEBUG

BOOL g_fJustFailed;

//+---------------------------------------------------------------------------
//
//  Function:   FFail
//
//  Synopsis:   Fails if count of fails is positive and evenly divides
//              interval count.
//
//----------------------------------------------------------------------------

BOOL
FFail()
{
    g_fJustFailed = (++g_cFFailCalled < 0) ? FALSE : ! (g_cFFailCalled % g_cInterval);
    return g_fJustFailed;
}



//+---------------------------------------------------------------------------
//
//  Function:   JustFailed
//
//  Synopsis:   Returns result of last call to FFail
//
//----------------------------------------------------------------------------

BOOL
JustFailed()
{
    return g_fJustFailed;
}



//+------------------------------------------------------------------------
//
//  Function:   GetFailCount
//
//  Synopsis:   Returns the number of failure points that have been
//              passed since the last failure count reset
//
//  Returns:    int
//
//-------------------------------------------------------------------------

int
GetFailCount( )
{
    Assert(g_firstFailure >= 0);
    return g_cFFailCalled + ((g_firstFailure != 0) ? g_firstFailure : INT_MIN);
}

#endif

#if DEVELOPER_DEBUG


//+---------------------------------------------------------------------------
//
//  Function:   ReturnFALSE
//
//  Synopsis:   Returns FALSE.  Used for Assert.
//
//----------------------------------------------------------------------------

BOOL
ReturnFALSE()
{
    return FALSE;
}

#endif
