//////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2000 Microsoft Corporation
//
//  Module Name:
//      pch.h
//
//  Description:
//      Precompiled header file for the TestEvictCluster EXE.
//
//  Maintained By:
//      Vij Vasu (Vvasu) 03-AUG-2000
//
//////////////////////////////////////////////////////////////////////////////

#pragma once

//////////////////////////////////////////////////////////////////////////////
//  Macro Definitions
//////////////////////////////////////////////////////////////////////////////

#if DBG==1 || defined( _DEBUG )
#define DEBUG
#endif

////////////////////////////////////////////////////////////////////////////////
// Include Files
//////////////////////////////////////////////////////////////////////////////

#include <windows.h>
// For wprintf
#include <stdio.h>

// For smart classes
#include "SmartClasses.h"

// For IClusCfgEvict
#include "ClusCfgServer.h"
