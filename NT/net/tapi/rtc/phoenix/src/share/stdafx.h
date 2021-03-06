// stdafx.h : include file for standard system include files,
//      or project specific include files that are used frequently,
//      but are changed infrequently

#if !defined(AFX_STDAFX_H__E456B10E_A36A_42F2_B591_3CCF7BE6868F__INCLUDED_)
#define AFX_STDAFX_H__E456B10E_A36A_42F2_B591_3CCF7BE6868F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define STRICT

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0400
#endif

#define OEMRESOURCE     // setting this gets OIC_ constants in windows.h

#define _ATL_APARTMENT_THREADED
#include <atlbase.h>

#include <commctrl.h>
#include <exdisp.h>
#include <shellapi.h>
#include <wtsapi32.h>

//You may derive a class from CComModule and use it if you want to override
//something, but do not change the name of _Module
extern CComModule _Module;

#include <atlcom.h>
#include <atlwin.h>
#include <atlhost.h>

#include <rtccore.h>
#include <rtcerr.h>

#include "rtclog.h"
#include "rtcmem.h"
#include "rtcutils.h"

#include "sessions.h"
#include "msgrua.h"
#include "mdisp.h"
#include "rtcshare.h"
#include "shareres.h"
#include "sharewin.h"
#include "shareimpl.h"

EXTERN_C const IID DIID_DMsgrSessionEvents;
EXTERN_C const IID DIID_DMsgrSessionManagerEvents;

extern const WCHAR g_cszAppShareGUID[];
extern const WCHAR g_cszWhiteboardGUID[];

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__E456B10E_A36A_42F2_B591_3CCF7BE6868F__INCLUDED)
