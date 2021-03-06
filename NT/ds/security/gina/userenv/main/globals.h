//*************************************************************
//
//  Global Variable Extern's
//
//  Microsoft Confidential
//  Copyright (c) Microsoft Corporation 1995
//  All rights reserved
//
//*************************************************************


#define WINLOGON_KEY                 TEXT("Software\\Microsoft\\Windows NT\\CurrentVersion\\Winlogon")
#define WINDOWS_POLICIES_KEY         TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Policies")
#define ROOT_POLICIES_KEY            TEXT("Software\\Policies")
#define SYSTEM_POLICIES_KEY          TEXT("Software\\Policies\\Microsoft\\Windows\\System")
#define DIAGNOSTICS_KEY              TEXT("Software\\Microsoft\\Windows NT\\CurrentVersion\\Diagnostics")
#define DIAGNOSTICS_GLOBAL_VALUE     TEXT("RunDiagnosticLoggingGlobal")
#define DIAGNOSTICS_POLICY_VALUE     TEXT("RunDiagnosticLoggingGroupPolicy")
#define LOCAL_GPO_DIRECTORY          TEXT("%SystemRoot%\\System32\\GroupPolicy")
#define LONG_UNC_PATH_PREFIX         TEXT("\\\\?\\UNC")

//ds
//
// When the time to change the profiles directory to the root,
// these things need to be done / checked:
//
// 1)  Search for //ds everywhere in this directory
// 2)  Change the default profiles directory in
//     windows\setup\inf\win4\inf\usa\hivesft.txt
//     note:  the new name is "Documents and Settings"
// 3)  In hivedef.inx, remove the TEMP and TMP environment variable entries
// 4)  Remove all of the special folder entries
// 5)  In usa\hivedef.txt, remove TEMP, TMP, and special folder entries
// 6)  Review MoveUserProfile() function to make sure it will handle
//     name collision (dual boot) case for All Users and Default User
//

#define NT4_PROFILES_DIRECTORY       TEXT("%SystemRoot%\\Profiles")
#define DEFAULT_USER                 TEXT("Default User")
#define DEFAULT_USER_NETWORK         TEXT("Default User (Network)")
#define ALL_USERS                    TEXT("All Users")

#define GUIMODE_SETUP_MUTEX          TEXT("Global\\userenv: GUI mode setup running")
#define USER_POLICY_MUTEX            TEXT("userenv: user policy mutex")
#define MACHINE_POLICY_MUTEX         TEXT("Global\\userenv: machine policy mutex")
#define USER_POLICY_REFRESH_EVENT    TEXT("userenv: user policy refresh event")
#define MACHINE_POLICY_REFRESH_EVENT TEXT("Global\\userenv: machine policy refresh event")
#define USER_POLICY_APPLIED_EVENT    TEXT("userenv: User Group Policy has been applied")
#define MACHINE_POLICY_APPLIED_EVENT TEXT("Global\\userenv: Machine Group Policy has been applied")
#define USER_POLICY_DONE_EVENT       TEXT("userenv: User Group Policy Processing is done")
#define MACHINE_POLICY_DONE_EVENT    TEXT("Global\\userenv: Machine Group Policy Processing is done")

#define MACH_POLICY_FOREGROUND_DONE_EVENT   TEXT("Global\\userenv: Machine Policy Foreground Done Event")
#define USER_POLICY_FOREGROUND_DONE_EVENT   TEXT("userenv: User Policy Foreground Done Event")

#define USER_REGISTRY_EXT_MUTEX      TEXT("userenv: User Registry policy mutex")
#define MACH_REGISTRY_EXT_MUTEX      TEXT("Global\\userenv: Machine Registry policy mutex")


//
// This event needs to be set for user or machine forced refresh
//

#define USER_POLICY_FORCE_REFRESH_EVENT  TEXT("userenv: user policy force refresh event")
#define MACHINE_POLICY_FORCE_REFRESH_EVENT  TEXT("Global\\userenv: machine policy force refresh event")


//
// This event is set by the core if a reboot is needed for the forced refresh to complete.
//

#define USER_POLICY_REFRESH_NEEDFG_EVENT    TEXT("userenv: User Group Policy ForcedRefresh Needs Foreground Processing")
#define MACHINE_POLICY_REFRESH_NEEDFG_EVENT TEXT("Global\\userenv: Machine Group Policy ForcedRefresh Needs Foreground Processing")


#define USER_SHELL_FOLDERS           TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\User Shell Folders")
#if defined(_WIN64)
#define USER_SHELL_FOLDERS32         TEXT("Software\\Wow6432Node\\Microsoft\\Windows\\CurrentVersion\\Explorer\\User Shell Folders")
#endif
#define SHELL_FOLDERS                TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders")
#define PROFILE_LIST_PATH            TEXT("Software\\Microsoft\\Windows NT\\CurrentVersion\\ProfileList")
#define PROFILE_GUID_PATH            TEXT("Software\\Microsoft\\Windows NT\\CurrentVersion\\ProfileGuid")
#define PROFILES_DIRECTORY           TEXT("ProfilesDirectory")
#define ALL_USERS_PROFILE            TEXT("AllUsersProfile")
#define DEFAULT_USER_PROFILE         TEXT("DefaultUserProfile")
#define PROFILE_FLAGS                TEXT("Flags")
#define PROFILE_GUID                 TEXT("Guid")
#define PROFILE_STATE                TEXT("State")
#define PROFILE_IMAGE_VALUE_NAME     TEXT("ProfileImagePath")
#define PROFILE_CENTRAL_PROFILE      TEXT("CentralProfile")
#define PROFILE_REF_COUNT            TEXT("RefCount")
#define PREFERENCE_KEYNAME           TEXT("Preference")
#define USER_PREFERENCE              TEXT("UserPreference")
#define PROFILE_LOAD_TIME_LOW        TEXT("ProfileLoadTimeLow")
#define PROFILE_LOAD_TIME_HIGH       TEXT("ProfileLoadTimeHigh")
#define PROFILE_UNLOAD_TIME_LOW      TEXT("ProfileUnloadTimeLow")
#define PROFILE_UNLOAD_TIME_HIGH     TEXT("ProfileUnloadTimeHigh")
#define PROFILE_GENERAL_SECTION      TEXT("General")
#define PROFILE_EXCLUSION_LIST       TEXT("ExclusionList")
#define PROFILE_LOAD_TYPE            TEXT("ProfileLoadType")
#define PROFILE_LAST_UPLOAD_STATE    TEXT("LastUploadState")
#define COMPLETE_PROFILE             TEXT("Complete")
#define PARTIAL_PROFILE              TEXT("Partial")
#define PROFILE_BUILD_NUMBER         TEXT("BuildNumber")
#define PROFILE_SID_STRING           TEXT("SidString")
#define TEMP_PROFILE_NAME_BASE       TEXT("TEMP")
#define PROFILE_UNLOAD_TIMEOUT       TEXT("ProfileUnloadTimeout")
#define DISABLE_PROFILE_UNLOAD_MSG   TEXT("DisableProfileUnloadMsg")
#define DELETE_ROAMING_CACHE         TEXT("DeleteRoamingCache")
#define ADD_ADMIN_GROUP_TO_RUP       TEXT("AddAdminGroupToRUP")
#define READONLY_RUP                 TEXT("ReadOnlyProfile")
#define PROFILE_LOCALONLY            TEXT("LocalProfile")
#define USER_PROFILE_SETUP_EVENT     TEXT("Global\\userenv:  User Profile setup event")
#define USER_PROFILE_MUTEX           TEXT("Global\\userenv:  User Profile Mutex for ")

#define SYSTEM_PROFILE_LOCATION      TEXT("%systemroot%\\system32\\config\\systemprofile")


//
// Appmgmt stuff to nuke
//

#define APPMGMT_DIR_ROOT             TEXT("%systemroot%\\system32\\appmgmt")
#define APPMGMT_REG_MANAGED          TEXT("Software\\Microsoft\\Windows\\Currentversion\\Installer\\Managed")

//
// IE cache key
//

#define IE4_CACHE_KEY                TEXT("Software\\Microsoft\\Windows\\Currentversion\\Internet Settings\\Cache\\Extensible Cache")
#define IE5_CACHE_KEY                TEXT("Software\\Microsoft\\Windows\\Currentversion\\Internet Settings\\5.0\\Cache\\Extensible Cache")
#define IE_CACHEKEY_PREFIX           TEXT("MsHist")

#ifdef  __cplusplus
extern "C" {
#endif

extern HINSTANCE    g_hDllInstance;
extern TCHAR        c_szRegistryExtName[];
extern const TCHAR  c_szRegistryPol[];


//
// Group policy handles
//

extern HANDLE           g_hPolicyCritMutexMach;
extern HANDLE           g_hPolicyCritMutexUser;

extern HANDLE           g_hRegistryPolicyCritMutexMach;
extern HANDLE           g_hRegistryPolicyCritMutexUser;

extern HANDLE           g_hPolicyNotifyEventMach;
extern HANDLE           g_hPolicyNotifyEventUser;

extern HANDLE           g_hPolicyNeedFGEventMach;
extern HANDLE           g_hPolicyNeedFGEventUser;

extern HANDLE           g_hPolicyDoneEventMach;
extern HANDLE           g_hPolicyDoneEventUser;

extern DWORD            g_dwBuildNumber;
extern HANDLE           g_hProfileSetup;

extern HANDLE           g_hPolicyForegroundDoneEventMach;
extern HANDLE           g_hPolicyForegroundDoneEventUser;

extern const TCHAR c_szStarDotStar[];
extern const TCHAR c_szSlash[];
extern const TCHAR c_szDot[];
extern const TCHAR c_szDotDot[];
extern const TCHAR c_szMAN[];
extern const TCHAR c_szUSR[];
extern const TCHAR c_szLog[];
extern const TCHAR c_szPDS[];
extern const TCHAR c_szPDM[];
extern const TCHAR c_szLNK[];
extern const TCHAR c_szBAK[];
extern const TCHAR c_szNTUserTmp[];
extern const TCHAR c_szNTUserMan[];
extern const TCHAR c_szNTUserDat[];
extern const TCHAR c_szNTUserIni[];
extern const TCHAR c_szNTUserStar[];
extern const TCHAR c_szUserStar[];
extern const TCHAR c_szSpace[];
extern const TCHAR c_szDotPif[];
extern const TCHAR c_szNULL[];
extern const TCHAR c_szCommonGroupsLocation[];
#if defined(__cplusplus)
}
#endif

//
// Timeouts
//

#define SLOW_LINK_TIMEOUT        120  // ticks
#define SLOW_LINK_TRANSFER_RATE  500  // Kbps
#define PROFILE_DLG_TIMEOUT       30  // seconds

//
// Folder sizes
//

#define MAX_FOLDER_SIZE                80
#define MAX_COMMON_LEN                 30
#define MAX_DLL_NAME_LEN               13

//
// Personal / common profile folders
//

#if defined(__cplusplus)
extern "C" {
#endif
extern DWORD g_dwNumShellFolders;
extern DWORD g_dwNumCommonShellFolders;
#if defined(__cplusplus)
}
#endif


typedef struct _FOLDER_INFO {
    BOOL   bHidden;
    BOOL   bLocal;
    BOOL   bAddCSIDL;
    BOOL   bNewNT5;
    BOOL   bLocalSettings;
    INT    iFolderID;
    LPTSTR lpFolderName;
    TCHAR  szFolderLocation[MAX_FOLDER_SIZE]; // must be at end of structure
    LPTSTR lpFolderResourceDLL;
    INT    iFolderResourceID;
} FOLDER_INFO;

#if defined(__cplusplus)
extern "C" {
#endif
extern FOLDER_INFO c_ShellFolders[];
extern FOLDER_INFO c_CommonShellFolders[];
#if defined(__cplusplus)
}
#endif


//
// Product type
//

typedef enum {
   PT_WORKSTATION           = 0x0001,   // Workstation
   PT_SERVER                = 0x0002,   // Server
   PT_DC                    = 0x0004,   // Domain controller
   PT_WINDOWS               = 0x0008    // Windows
} NTPRODUCTTYPE;

#if defined(__cplusplus)
extern "C" {
#endif
extern NTPRODUCTTYPE g_ProductType;
#if defined(__cplusplus)
}
#endif


//
// Function proto-types
//

#if defined(__cplusplus)
extern "C" {
#endif
void InitializeGlobals (HINSTANCE hInstance);
void PatchLocalAppData(HANDLE hToken);
void InitializeSnapProv();
#if defined(__cplusplus)
}
#endif

//
// Type of message
//

#define DLGTYPE_ERROR        0
#define DLGTYPE_SLOWLINK     1

