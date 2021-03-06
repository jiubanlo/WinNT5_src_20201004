/***************************************************************************\
*
* File: ObjectAPI.h
*
* Description:
* This file provides a project-wide header that is included in all source 
* files specific to this project.  It is similar to a precompiled header, 
* but is designed for more rapidly changing headers.
*
* The primary purpose of this file is to determine which DirectUser 
* projects this project has direct access to instead of going through public
* API's.  It is VERY IMPORTANT that this is as minimal as possible since
* adding a new project unnecessarily reduces the benefit of project 
* partitioning.
*
*
* History:
*  1/18/2000: JStall:       Created
*
* Copyright (C) 2000 by Microsoft Corporation.  All rights reserved.
* 
\***************************************************************************/


#if !defined(OBJECTAPI__ObjectApi_h__INCLUDED)
#define OBJECTAPI__ObjectApi_h__INCLUDED
#pragma once

#include "Public.h"

#include <DUserBaseP.h>

#if DBG

#define PromptInvalid(comment) \
    do \
    { \
        if (IDebug_Prompt(GetDebug(), "Validation error:\r\n" comment, __FILE__, __LINE__, "DirectUser/ObjectAPI Notification")) \
            AutoDebugBreak(); \
    } while (0) \

#else // DBG

#define PromptInvalid(comment) ((void) 0)

#endif // DBG

#endif // OBJECTAPI__ObjectApi_h__INCLUDED
