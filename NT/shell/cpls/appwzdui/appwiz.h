//
//  APPWIZ.H -- App install wizard header file
//
//  Copyright (C) Microsoft, 1994,1995 All Rights Reserved.
//
//  History:
//  ral 5/23/94 - Copied from INTL CPL
//  3/20/95  [stevecat] - NT port & real clean up, unicode, etc.
//  3/14/98  [scotth] - reworked into shell 5 project
//

#ifndef _APPWIZ_H_
#define _APPWIZ_H_

#include <pif.h>        // for PROPPRG
#include <apwizhlp.h>
#include "appwizid.h"
#include <shsemip.h>    // for RestartDialog

#define CPLMODE_NORMAL            0
#define CPLMODE_AUTOMSDOS   1
#define CPLMODE_PROPWIZ     2

#define MAX_PAGES  15           // limit on number of pages we can have

#define WDFLAG_NOBROWSEPAGE 0x00000001        // Don't let user pick exe name
#define WDFLAG_APPKNOWN     0x00000002        // Finish after selecting folder
#define WDFLAG_DOSAPP       0x00000004        // Exe is a DOS program
//      -- notused          0x00000008
//      -- notused          0x00000010
#define WDFLAG_LINKHEREWIZ  0x00000020        // Create empty link (NOT IMPLEMENTED!)
#define WDFLAG_INEDITMODE   0x00000040        // Editing a folder label
#define WDFLAG_DONTOPENFLDR 0x00000080        // Don't open folder when link made
#define WDFLAG_REALMODEONLY 0x00000100        // Don't use any prot mode drivers
#define WDFLAG_COPYLINK     0x00000200        // Copy link, don't create new one.
#define WDFLAG_SETUPWIZ     0x00000400        // Setup from disk wizard
#define WDFLAG_READOPTFLAGS 0x00000800        // Value in dwDosOptGlobalFlags is valid
#define WDFLAG_NOAUTORUN    0x00001000        // Disable autorun while wizard is running
#define WDFLAG_AUTOTSINSTALLUI 0x00002000        // Automate Install Mode (For people double clicking or calling ShellExecute() on setup programs)
#define WDFLAG_EXPSZ        0x10000000        // WizData contains expanded strings

typedef struct _DOSOPT {
    HKEY        hk;
    DWORD        dwFlags;
    UINT        uOrder;
    DWORD        dwStdOpt;

} DOSOPT, FAR * LPDOSOPT;


//////////////////////////////////////////////////////////////////////////////
//
// UNINSTALL_ITEM -- more info kept in an array paralell to the listbox
//
//////////////////////////////////////////////////////////////////////////////

typedef struct
{
    TCHAR command[ MAX_PATH ];  // command to run which will nuke the app

} UNINSTALL_ITEM, FAR * LPUNINSTALL_ITEM;



typedef struct _WIZDATA {
    HWND        hwnd;
    DWORD       dwFlags;
    TCHAR       szExeName[MAX_PATH];
    TCHAR       szExpExeName[MAX_PATH];
    TCHAR       szParams[MAX_PATH];
    TCHAR       szProgDesc[MAX_PATH];
    TCHAR       szWorkingDir[MAX_PATH];
    HBITMAP     hbmpWizard;
    HIMAGELIST  himl;
    LPTSTR      lpszFolder;
    PROPPRG     PropPrg;
    LPTSTR      lpszOriginalName;       // if non-null then link exists already
    LPUNINSTALL_ITEM        lpUItem;

#ifndef NO_NEW_SHORTCUT_HOOK
    INewShortcutHook *pnshhk;
    TCHAR     szExt[MAX_PATH];
    INewShortcutHookA *pnshhkA;
#endif

    BOOL        bTermSrvAndAdmin;
    BOOL        bPrevMode;

} WIZDATA, FAR * LPWIZDATA;

//
// Private messages
//
#define WMPRIV_POKEFOCUS    WM_APP+0

//
// Wizard entry points.
//
BOOL LinkWizard(LPWIZDATA);
BOOL SetupWizard(LPWIZDATA);

//
// Main appwiz property sheet
//
BOOL_PTR CALLBACK AppListDlgProc(HWND hDlg, UINT message , WPARAM wParam, LPARAM lParam);
BOOL_PTR CALLBACK InstallUninstallDlgProc(HWND hDlg, UINT message , WPARAM wParam, LPARAM lParam);

//
//  Pushes the "OK" button on a property sheet.
//
void DismissCPL(LPWIZDATA);

//
// Setup wizard pages
//
BOOL_PTR CALLBACK SetupDlgProc(HWND hDlg, UINT message , WPARAM wParam, LPARAM lParam);
BOOL_PTR CALLBACK ChgusrDlgProc(HWND hDlg, UINT message , WPARAM wParam, LPARAM lParam);
BOOL_PTR CALLBACK ChgusrFinishDlgProc(HWND hDlg, UINT message , WPARAM wParam, LPARAM lParam);
BOOL_PTR CALLBACK ChgusrFinishPrevDlgProc(HWND hDlg, UINT message , WPARAM wParam, LPARAM lParam);

//
// Shortcut wizard pages
//
BOOL_PTR CALLBACK BrowseDlgProc(HWND hDlg, UINT message , WPARAM wParam, LPARAM lParam);
BOOL_PTR CALLBACK SetupBrowseDlgProc(HWND hDlg, UINT message , WPARAM wParam, LPARAM lParam);
BOOL_PTR CALLBACK PickFolderDlgProc(HWND hDlg, UINT message , WPARAM wParam, LPARAM lParam);
BOOL_PTR CALLBACK GetTitleDlgProc(HWND hDlg, UINT message , WPARAM wParam, LPARAM lParam);
BOOL_PTR CALLBACK PickIconDlgProc(HWND hDlg, UINT message , WPARAM wParam, LPARAM lParam);

//
// Function in Folder.C that is used for removing folders from the task bar
// property sheet.
//
BOOL RemoveItemsDialog(HWND hParent);


//
// Strips "@" characters from a string resource and replaces them with NULLs
// Returns TRUE if succeeded
//
EXTERN_C BOOL LoadAndStrip(int id, LPTSTR lpsz, int cbstr);

//
// Exec the program.  Used by Setup and AppList
//
BOOL ExecSetupProg(LPWIZDATA lpwd, BOOL ForceWx86, BOOL bMinWiz);

//
// Skip to next string in doubly null terminated string
//
LPTSTR SkipStr(LPTSTR);

//
// Used by all wizard sheets at WM_INITDIALOG
//
LPWIZDATA InitWizSheet(HWND hDlg, LPARAM lParam, DWORD dwFlags);

//
// Used by all wizard sheets at PSN_RESET
//
void CleanUpWizData(LPWIZDATA lpwd);

//
// Thunked exports for 16-bit apps/dlls
//
void InstallCPL(HWND hwnd, UINT nStartPage);

//
// Functions for links
//
BOOL CreateLink(LPWIZDATA);
BOOL GetLinkName(LPTSTR, UINT cch, LPWIZDATA);


//
// Created by the thunk scripts
//
BOOL WINAPI Pif3216_ThunkConnect32(LPCTSTR pszDll16, LPCTSTR pszDll32, HANDLE hIinst, DWORD dwReason);
BOOL WINAPI Pif1632_ThunkConnect32(LPCTSTR pszDll16, LPCTSTR pszDll32, HANDLE hIinst, DWORD dwReason);


BOOL GetSingleAppInfo(LPWIZDATA);

//
// Gets the INF name for install programs.  Returns false if none.
//
BOOL AppListGetInfName(LPWIZDATA);

//
//  Functions determine info about lpwd->szExeName
//
void DetermineExeType(LPWIZDATA);

//
//  Fills in the szProgDesc field of the wizdata structure.
//
BOOL DetermineDefaultTitle(LPWIZDATA);

//
//  Strip the extension off of a file name.
//
void StripExt(LPTSTR lpsz);

//
// Global data
//
extern int g_cxIcon;
extern int g_cyIcon;

extern TCHAR const c_szPIF[];
extern TCHAR const c_szLNK[];


#ifdef WX86
//
// from uninstal.c
//
extern BOOL bWx86Enabled;
extern BOOL bForceX86Env;
extern const WCHAR ProcArchName[];
#endif

#endif // _APPWIZ_H_
