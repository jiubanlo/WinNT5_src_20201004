/*
****************************************************************************
|    Copyright (C) 2001  Microsoft Corporation
|
|   Module Name:
|
|       Utils.cpp
|
|   Abstract:
|		This is the core code for the IIS6 Monitor tool
|
|   Author:
|        Ivo Jeglov (ivelinj)
|
|   Revision History:
|        November 2001
|
****************************************************************************
*/


#include "stdafx.h"
#include "Utils.h"


// This is the title for all wizard pages as well as the string that is shown in Add/Remove programs
LPCWSTR	MAIN_TITLE = L"IIS 6.0 Monitor v1.2";


static LPCWSTR MON_REGKEY			= L"Software\\Microsoft\\IISMon";

static LPCTSTR MSG_ALREADYINSTALLED	= _T("IIS 6.0 Monitor is already installed. If you would like to re-install IIS 6.0 Monitor, please remove your current version using the 'Add or Remove Programs' interface in the Control Panel.");
static LPCTSTR MSG_NOTADMIN			= _T("Only members of the Administrator group can install IIS 6.0 Monitor. Please add yourself to the Administrators group, and then run IIS 6.0 Monitor installation again. If you cannot add yourself to the Administrators group, contact your network administrator.");
static LPCTSTR MSG_NOTASERVER		= _T("IIS 6.0 Monitor can be installed only on pre-release versions of the Windows Server 2003 family of products.");
static LPCTSTR MSG_NOIIS			= _T("Internet Information Services ( IIS ) is not installed. Please install IIS from Add/Remove Windows Components in Control Panel.");
static LPCTSTR MSG_IA64NOTSUPPORTED	= _T("IIS 6.0 Monitor cannot be installed on IA64 platforms.");
static LPCTSTR MSG_SCHEDULERSTOPPED	= _T("IIS 6.0 Monitor depends on Task Scheduler service to function properly. You need to enable Task Scheduler on this server by clicking Start, pointing to All Programs, then Administrative Tools, and clicking Services. From the list of services, right-click Task Scheduler, and click Start.");

static LPCTSTR ERR_COPYFAILED		= _T("IIS 6.0 Monitor installation has failed to copy the necessary files to your file system.");
static LPCTSTR ERR_REGERROR			= _T("The registry contains Windows configuration information. IIS 6.0 Monitor installation has failed to create a set of entries in the registry that are required to ensure IIS 6.0 Monitor functions properly.");
static LPCTSTR ERR_TASKERROR		= _T("IIS 6.0 Monitor installation failed while trying to use the Task Scheduler service on your server to schedule the necessary IIS 6.0 Monitor scripts to run on a periodic basis.");
static LPCTSTR ERR_DIRERROR			= _T("IIS 6.0 Monitor requires a specific set of directories to be created for proper functionality. IIS 6.0 Monitor installation has failed to create the necessary directories.");
static LPCTSTR ERR_SETACLAILED		= _T("IIS 6.0 Monitor installation has failed to setup the access rights of the required directories.");

static LPCWSTR TSK_DYN				= L"This scheduled task runs a JScript that is part of the IIS 6.0 Monitor tool.  This script is scheduled to run every two minutes to sample performance counter information, and then to generate an XML file that contains both aggregated performance counter information and entries from the Event Viewer.";
static LPCWSTR TSK_STAT				= L"This scheduled task runs a JScript that is part of the IIS 6.0 Monitor tool.  This script is scheduled to run once a week to collect system hardware information and registry settings, and then to generate an XML file containing this information.";
static LPCWSTR TSK_META				= L"This scheduled task runs a JScript that is part of the IIS 6.0 Monitor tool. This script is scheduled to run once a week to copy your XML metabase file, and then parses the copied metabase to remove sensitive information.";
static LPCWSTR TSK_UPLOAD			= L"This scheduled task runs a JScript that is part of the IIS 6.0 Monitor tool. This script uploads the XML files generated by the the following IIS 6.0 Monitor scripts to Microsoft: iismDyn.js, iismStat.js, and iismMeta.js. These scripts are located in %Systemroot%\\System32\\Inetsrv\\IISMon directory.  Files that are successfully uploaded to Microsoft will be copied to the %Systemdrive%\\IISMon directory if you have enabled the audit trail option.";



BOOL IsMonInstalled()
{
	// Check if the reg key exists. If so - the Monitor is installed
	HKEY	hKey	= NULL;
	BOOL	bRes	= FALSE;

	if ( ::RegOpenKeyExW( HKEY_LOCAL_MACHINE, MON_REGKEY, 0, KEY_READ, &hKey ) == ERROR_SUCCESS )
	{
		bRes = TRUE;
	}

	if ( hKey != NULL )
	{
		VERIFY( ::RegCloseKey( hKey ) == ERROR_SUCCESS );
	}

	return bRes;
}


// IsAdmin() - tests to see if the current user is an admin  	  
BOOL IsAdmin()
{
	// Try an Admin Privilaged API - if it works return TRUE - else FALSE
	SC_HANDLE hSC = ::OpenSCManager( NULL, NULL, GENERIC_READ | GENERIC_WRITE | GENERIC_EXECUTE );

	BOOL bAdmin = hSC != NULL;

	if ( hSC != NULL )
	{
		VERIFY( ::CloseServiceHandle( hSC ) );
	}    

	return bAdmin;
}



BOOL IsIISInstalled( void )
{
	LPCWSTR	SERVICE_NAME = L"W3SVC";

	BOOL bRes = FALSE;

	// Open the SCM on the local machine
    SC_HANDLE   schSCManager = ::OpenSCManagerW( NULL, NULL, SC_MANAGER_ALL_ACCESS );
	_ASSERT( schSCManager != NULL );	// We alredy checked that we are Admins
     
    SC_HANDLE   schService = ::OpenServiceW( schSCManager, SERVICE_NAME, SERVICE_QUERY_STATUS );
    
    if ( schService != NULL )
	{
		bRes = TRUE;
		VERIFY( ::CloseServiceHandle( schService ) );
	}

    VERIFY( ::CloseServiceHandle( schSCManager ) );
    
	return bRes;
}



BOOL IsWhistlerSrv()
{
    OSVERSIONINFOEXW osVersion = { 0 };
    osVersion.dwOSVersionInfoSize = sizeof( osVersion );

    VERIFY( ::GetVersionExW( reinterpret_cast<OSVERSIONINFOW*>( &osVersion ) ) );

    if (    ( osVersion.dwMajorVersion == 5 ) &&
            ( osVersion.dwMinorVersion == 2 ) &&
            ( ( osVersion.wProductType == VER_NT_SERVER ) || ( osVersion.wProductType == VER_NT_DOMAIN_CONTROLLER ) ) )
    {
        return TRUE;
    }

	return FALSE;
}



BOOL IsIA64()
{
	SYSTEM_INFO	Info;

	::GetSystemInfo( &Info );

	return (	( Info.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64 ) ||
				( Info.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA32_ON_WIN64 ) ||
				( Info.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 ) );
}



BOOL IsNTFS()
{
	const UINT BUFF_LEN = 32;	// Should be large enough to hold the volume and the file system type

	WCHAR wszBuffer[ BUFF_LEN ];

	// Get the system drive letter
	VERIFY( ::ExpandEnvironmentStringsW( L"%SystemDrive%", wszBuffer, BUFF_LEN ) != 0 );

	// wszBuffer containts the drive only - add the slash to make the volume string
	::wcscat( wszBuffer, L"\\" );

	DWORD dwMaxComponentLength	= 0;
	DWORD dwSystemFlags			= 0;

	WCHAR wszFileSystem[ BUFF_LEN ];
	
	VERIFY( ::GetVolumeInformationW(	wszBuffer,
										NULL,
										0,
										NULL,
										&dwMaxComponentLength,
										&dwSystemFlags,
										wszFileSystem,
										BUFF_LEN ) );

	return ::wcscmp( wszFileSystem, L"NTFS" ) == 0;
}



BOOL IsTaskSchRunning()
{
	LPCWSTR	SERVICE_NAME = L"Schedule";

	BOOL bRunning = FALSE;

	// Open the SCM on the local machine
    SC_HANDLE   schSCManager = ::OpenSCManagerW( NULL, NULL, SC_MANAGER_ALL_ACCESS );
	_ASSERT( schSCManager != NULL );	// We alredy checked that we are Admins
     
    SC_HANDLE   schService = ::OpenServiceW( schSCManager, SERVICE_NAME, SERVICE_QUERY_STATUS );
    _ASSERT( schService != NULL );	// This service is part of the OS and must exist
      
	SERVICE_STATUS ssStatus;

	VERIFY( ::QueryServiceStatus( schService, &ssStatus ) );
    
	bRunning = ( ssStatus.dwCurrentState == SERVICE_RUNNING );
    
	VERIFY( ::CloseServiceHandle( schService ) );
    VERIFY( ::CloseServiceHandle( schSCManager ) );
    
	return bRunning;
}



BOOL IsW3SVCEnabled()
{
	LPCWSTR	SERVICE_NAME = L"W3SVC";

	BOOL bSvcOK = FALSE;

	// Open the SCM on the local machine
    SC_HANDLE   schSCManager = ::OpenSCManagerW( NULL, NULL, SC_MANAGER_ALL_ACCESS );
	_ASSERT( schSCManager != NULL );	// We alredy checked that we are Admins
     
    SC_HANDLE   schService = ::OpenServiceW( schSCManager, SERVICE_NAME, SERVICE_QUERY_STATUS | SERVICE_QUERY_CONFIG );
    _ASSERT( schService != NULL );	// We already checked that this service exist ( IsIISInstalled(...) )
      
	SERVICE_STATUS			ssStatus;
	LPQUERY_SERVICE_CONFIGW	pSvcConfig = reinterpret_cast<LPQUERY_SERVICE_CONFIGW>( ::malloc( 4096 ) );

	// Do not let an out of mem condition to ruin the Setup process at this step - pretend the service is OK
	if ( NULL == pSvcConfig )
	{
		return TRUE;
	}
	
	DWORD dwNeeded = 0;

	VERIFY( ::QueryServiceStatus( schService, &ssStatus ) );
	VERIFY( ::QueryServiceConfigW( schService, pSvcConfig, 4096, &dwNeeded ) );
	_ASSERT( dwNeeded <= 4096 );
    
	bSvcOK = ( SERVICE_RUNNING == ssStatus.dwCurrentState ) && ( SERVICE_DISABLED != pSvcConfig->dwStartType );
    
	VERIFY( ::CloseServiceHandle( schService ) );
    VERIFY( ::CloseServiceHandle( schSCManager ) );
	::free( pSvcConfig );
    
	return bSvcOK;
}



LPCTSTR CanInstall()
{
	LPCTSTR szError = NULL;

	// Check all install requirements:\
	
	if ( IsMonInstalled() )
	{
		szError = MSG_ALREADYINSTALLED;
	}
	else if ( !IsAdmin() )
	{
		szError = MSG_NOTADMIN;
	}
	else if ( !IsWhistlerSrv() )
	{
		szError = MSG_NOTASERVER;
	}
	else if ( !IsIISInstalled() )
	{
		szError = MSG_NOIIS;
	}
	else if ( IsIA64() )
	{
		szError = MSG_IA64NOTSUPPORTED;
	}
	else if ( !IsTaskSchRunning() )
	{
		szError = MSG_SCHEDULERSTOPPED;
	}
		
	return szError;
}



HRESULT SetupTasks()
{
	DECLARE_HR_SUCCESS;

	ITaskSchedulerPtr		spTaskScheduler;
	TASK_TRIGGER			Trigger;

	// Get an interface to the Task Scheduler
	IF_SUCCEEDED( spTaskScheduler.CreateInstance( CLSID_CTaskScheduler ) );

	// iismDyn.js - will run every 2 minutes. Timeout 2min
	//////////////////////////////////////////////////////////////////
	::InitTrigger( /*r*/Trigger );
	Trigger.TriggerType						= TASK_TIME_TRIGGER_DAILY;
	Trigger.Type.Daily.DaysInterval			= 1;			// Every Day
	Trigger.MinutesDuration					= 24 * 60;		// The task have to be active all the day long
	Trigger.MinutesInterval					= 2;			// Run every 2 minutes
	IF_SUCCEEDED( ::AddTask( spTaskScheduler, L"DynData", L"iismDyn.js", TSK_DYN, 2 * 60 * 1000, Trigger ) );


	// iismUpload.js - will run every 2 minutes. Timeout: 12min
	//////////////////////////////////////////////////////////////////
	::InitTrigger( /*r*/Trigger );
	Trigger.TriggerType						= TASK_TIME_TRIGGER_DAILY;
	Trigger.Type.Daily.DaysInterval			= 1;			// Every Day
	Trigger.MinutesDuration					= 24 * 60;		// The task have to be active all the day long
	Trigger.MinutesInterval					= 120;			// Run every 2 hours
	IF_SUCCEEDED( ::AddTask( spTaskScheduler, L"Upload", L"iismUpld.js", TSK_UPLOAD, 12 * 60 * 1000, Trigger ) );


	// iismStat.js - will run once every week on Sunday, 3:00AM. Timeout: 2min
	//////////////////////////////////////////////////////////////////
	::InitTrigger( /*r*/Trigger );
	Trigger.TriggerType						= TASK_TIME_TRIGGER_WEEKLY;
	Trigger.Type.Weekly.WeeksInterval		= 1;	// Every week
	Trigger.Type.Weekly.rgfDaysOfTheWeek	= TASK_SUNDAY;
	Trigger.wStartHour						= 3;
	Trigger.wStartMinute					= 00;
	IF_SUCCEEDED( ::AddTask( spTaskScheduler, L"StatData", L"iismStat.js", TSK_STAT, 2 * 60 * 1000, Trigger ) );


	// iismMeta.js - will run once every week on Sunday, 3:15AM. Timeout: 5min
	//////////////////////////////////////////////////////////////////
	::InitTrigger( /*r*/Trigger );
	Trigger.TriggerType						= TASK_TIME_TRIGGER_WEEKLY;
	Trigger.Type.Weekly.WeeksInterval		= 1;	// Every week
	Trigger.Type.Weekly.rgfDaysOfTheWeek	= TASK_SUNDAY;
	Trigger.wStartHour						= 3;
	Trigger.wStartMinute					= 15;
	IF_SUCCEEDED( ::AddTask( spTaskScheduler, L"MetaData", L"iismMeta.js", TSK_META, 5 * 60 * 1000,  Trigger ) );

	return hr;
}



void DeleteTasks()
{
	ITaskSchedulerPtr		spTaskScheduler;
	
	// Get an interface to the Task Scheduler
	if ( SUCCEEDED( spTaskScheduler.CreateInstance( CLSID_CTaskScheduler ) ) )
	{
		// Try to delete the tasks. The result is for information purposes only
		VERIFY( SUCCEEDED( spTaskScheduler->Delete( L"IIS Monitor ( DynData )" ) ) );
		VERIFY( SUCCEEDED( spTaskScheduler->Delete( L"IIS Monitor ( Upload )" ) ) );
		VERIFY( SUCCEEDED( spTaskScheduler->Delete( L"IIS Monitor ( StatData )" ) ) );
		VERIFY( SUCCEEDED( spTaskScheduler->Delete( L"IIS Monitor ( MetaData )" ) ) );
	}
}



HRESULT	AddTask(	const ITaskSchedulerPtr& spTaskScheduler, 
					LPCWSTR wszSubname, 
					LPCWSTR wszFileName, 
					LPCWSTR wszComment,
					DWORD dwTimeout,
					TASK_TRIGGER& Trigger )
{
	DECLARE_HR_SUCCESS;

	static LPCWSTR TASK_NAME_FMT	= L"IIS Monitor ( %s )";
	static LPCWSTR TASK_COMMENT		= L"This task is used to collect IIS statistic info and send it back to Microsoft.";
	
	WCHAR					wszPath[ MAX_PATH + 1 ];
	WCHAR					wszName[ 512 ];
	ITaskPtr				spTask;
	IPersistFilePtr			spPersistFile;

	// Create the task name
	::swprintf( wszName, TASK_NAME_FMT, wszSubname );

	// Get the path ( for the working dir and for the executable )
	GetIISMonPath( wszPath );

	// Add the new task
	IF_SUCCEEDED( spTaskScheduler->NewWorkItem(	wszName, 
												CLSID_CTask, 
												IID_ITask, 
												reinterpret_cast<IUnknown**>( &spTask ) ) );

	// If the taks alredy exists - use it and modify it
	if ( HRESULT_FROM_WIN32( ERROR_FILE_EXISTS ) == hr )
	{
		hr = spTaskScheduler->Activate( wszName, IID_ITask, reinterpret_cast<IUnknown**>( &spTask ) );
	}

	// Setup the task
	IF_SUCCEEDED( spTask->SetWorkingDirectory( wszPath ) );	
	IF_SUCCEEDED( spTask->SetComment( wszComment ) );
	IF_SUCCEEDED( spTask->SetPriority( NORMAL_PRIORITY_CLASS ) );
	IF_SUCCEEDED( spTask->SetMaxRunTime( dwTimeout ) );	
	IF_SUCCEEDED( spTask->SetAccountInformation( L"", NULL ) );	// Use Local System account

	// Set task command line
	VERIFY( ::PathAppendW( wszPath, wszFileName ) );
	IF_SUCCEEDED( spTask->SetApplicationName( L"cscript.exe" ) );
	IF_SUCCEEDED( spTask->SetParameters( wszPath ) );

	// Set the trigger
	IScheduledWorkItemPtr	spItem;
	ITaskTriggerPtr			spTrigger;
	WORD					wUnused = 0;

	IF_SUCCEEDED( spTask.QueryInterface( IID_IScheduledWorkItem, &spItem ) );
	IF_SUCCEEDED( spItem->CreateTrigger( &wUnused, &spTrigger ) );
	IF_SUCCEEDED( spTrigger->SetTrigger( &Trigger ) );

	// Store the changes
	IF_SUCCEEDED( spTask.QueryInterface( IID_IPersistFile, &spPersistFile ) );
	IF_SUCCEEDED( spPersistFile->Save( NULL, TRUE ) );

	// Cleanup
	if ( FAILED( hr ) )
	{
		// Remove the task
		if ( spTaskScheduler != NULL )
		{
			spTaskScheduler->Delete( wszName );
		}
	}	

	return hr;
}



void InitTrigger( TASK_TRIGGER& rTrigger )
{
	::ZeroMemory( &rTrigger, sizeof( TASK_TRIGGER ) );

	rTrigger.cbTriggerSize = sizeof( TASK_TRIGGER );

	// Set the start time for something in the pas. We don't use this feature
	rTrigger.wBeginYear		= 2000;
	rTrigger.wBeginMonth	= 1;
	rTrigger.wBeginDay		= 1;
}



HRESULT	SetupRegistry( BOOL bEnableTrail, DWORD dwDaysToKeep )
{
	DECLARE_HR_SUCCESS;

	// Generete the GUID for this machine
	GUID	guid;
	DWORD	dwTrail = bEnableTrail ? 1 : 0;
	IF_SUCCEEDED( ::CoCreateGuid( &guid ) );

	// Create the unsintall string
	WCHAR	wszUninstall[ MAX_PATH + 1 ];
	GetIISMonPath( wszUninstall );
	VERIFY( ::PathAppendW( wszUninstall, L"iismoni.exe -uninstinter" ) );

	// Convert it to string
	WCHAR wszBuffer[ 64 ];	// SHould be large enough to hold a string GUID
	VERIFY( ::StringFromGUID2( guid, wszBuffer, 64 ) != 0 );

	// Store GUID
	IF_SUCCEEDED( SetIISMonRegData(	MON_REGKEY, 
									L"ServerGUID", 
									REG_SZ, 
									reinterpret_cast<BYTE*>( wszBuffer ), 
									::wcslen( wszBuffer ) * sizeof( WCHAR ) ) );

	// Store the audit trail value
	IF_SUCCEEDED( SetIISMonRegData(	MON_REGKEY, 
									L"AuditTrailEnabled", 
									REG_DWORD, 
									reinterpret_cast<BYTE*>( &dwTrail ), 
									sizeof( DWORD ) ) );

	// Set the DaysToKeep Value
	IF_SUCCEEDED( SetIISMonRegData(	MON_REGKEY, 
									L"AuditTrailTimeLimit", 
									REG_DWORD, 
									reinterpret_cast<BYTE*>( &dwDaysToKeep ), 
									sizeof( DWORD ) ) );

	// Set the uninstall string
	IF_SUCCEEDED( SetIISMonRegData(	L"Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\IISMon", 
									L"UninstallString",
									REG_SZ, 
									reinterpret_cast<BYTE*>( wszUninstall ), 
									::wcslen( wszUninstall ) * sizeof( WCHAR ) ) );

	// Set the uninstall display name
	IF_SUCCEEDED( SetIISMonRegData(	L"Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\IISMon", 
									L"DisplayName",
									REG_SZ, 
									( BYTE* )( MAIN_TITLE ), 
									::wcslen( MAIN_TITLE ) * sizeof( WCHAR ) ) );
	
	return hr;
}



HRESULT	SetIISMonRegData( LPCWSTR wszSubkey, LPCWSTR wszName, DWORD dwType, const BYTE* pbtData, DWORD dwSize )
{
	DECLARE_HR_SUCCESS;

	_ASSERT( wszName != NULL );
	_ASSERT( dwSize > 0 );
	_ASSERT( pbtData != NULL );
	_ASSERT( wszSubkey != NULL );

	HKEY	hKey = NULL;

	// If the key does not exists - create it
	if ( ::RegCreateKeyExW(	HKEY_LOCAL_MACHINE, 
							wszSubkey, 
							0, 
							NULL, 
							REG_OPTION_NON_VOLATILE, 
							KEY_SET_VALUE,
							NULL,
							&hKey,
							NULL ) != ERROR_SUCCESS )
	{
		hr = E_FAIL;
	}
	
	if ( SUCCEEDED( hr ) )
	{
		if ( ::RegSetValueExW( hKey, wszName, 0, dwType, pbtData, dwSize ) != ERROR_SUCCESS )
		{
			hr = E_FAIL;
		}
	}

	if ( hKey != NULL )
	{
		::RegCloseKey( hKey );
	}

	return hr;
}



void DelIISMonKey()
{
	VERIFY( ::SHDeleteKeyW( HKEY_LOCAL_MACHINE, MON_REGKEY ) == ERROR_SUCCESS );
	VERIFY( ::SHDeleteKeyW( HKEY_LOCAL_MACHINE, L"Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\IISMon" ) == ERROR_SUCCESS );
}



void GetIISMonPath(	LPWSTR wszPath )
{
	// wszPath should be a buffer with MAX_PATH + 1 length
	VERIFY( ::GetSystemDirectoryW( wszPath, MAX_PATH + 1 ) != 0 );
	VERIFY( ::PathAppendW( wszPath, L"Inetsrv\\IISMon" ) );
}



HRESULT SetupDirStruct()
{
	BOOL	bRes = TRUE;
	WCHAR	wszRoot[ MAX_PATH + 1 ];
	
	// Create the Log and Upload folders
	// Do not fail if they already exist
	if ( bRes )
	{
		GetIISMonPath( wszRoot );
		VERIFY( ::PathAppendW( wszRoot, L"Upload" ) );

		bRes = ::CreateDirectoryW( wszRoot, NULL );

		if ( !bRes && ( ::GetLastError() == ERROR_ALREADY_EXISTS ) )
		{
			bRes = TRUE;
		}
	}

	if ( bRes )
	{
		GetIISMonPath( wszRoot );
		VERIFY( ::PathAppendW( wszRoot, L"Log" ) );

		bRes = ::CreateDirectoryW( wszRoot, NULL );

		if ( !bRes && ( ::GetLastError() == ERROR_ALREADY_EXISTS ) )
		{
			bRes = TRUE;
		}
	}

	return bRes ? S_OK : E_FAIL;
}



HRESULT	SetupACLs( void )
{
	// ACLs are set so that only Administrators have access to IISMon folders
	// The ACLs are not inherited from parent dirs
	SECURITY_DESCRIPTOR*	pSD		= NULL;
	ACL*					pDACL	= NULL;	
	SECURITY_INFORMATION	si		= ( DACL_SECURITY_INFORMATION | PROTECTED_DACL_SECURITY_INFORMATION );

	BOOL bHaveDACL	= FALSE;
	BOOL bDefaulted	= FALSE;
	
	// This is the ACL that we will set. String ACL is used for simplicity
	// See ACE strings documentation in the MSDN ( Search for "SDDL" )
	VERIFY( ::ConvertStringSecurityDescriptorToSecurityDescriptorW(	L"D:P(A;CIOI;GA;;;BA)(A;CIOI;GA;;;SY)",
																	SDDL_REVISION_1,
																	reinterpret_cast<void**>( &pSD ),
																	NULL ) );

	VERIFY( ::GetSecurityDescriptorDacl( pSD, &bHaveDACL, &pDACL, &bDefaulted ) );

	// Set theDACL to all the folders
	LPCWSTR awszDirs[] = {	L"%systemdrive%\\IISMon",
							L"%systemroot%\\system32\\inetsrv\\IISMon" 
						};

	for ( int i = 0; i < ARRAY_SIZE( awszDirs ); ++i )
	{
		WCHAR wszPath[ MAX_PATH + 1 ];
		::wcscpy( wszPath, awszDirs[ i ] );
		VERIFY( LOWORD( ::DoEnvironmentSubstW( wszPath, MAX_PATH + 1 ) ) );

		if ( ::SetNamedSecurityInfoW(	wszPath, 
										SE_FILE_OBJECT,
										si,
										NULL,
										NULL,
										pDACL,
										NULL ) != ERROR_SUCCESS )
		{
			return E_FAIL;
		}
	}

	return S_OK;
}


void DeleteDirStruct( BOOL bRemoveTrail )
{
	WCHAR	wszRoot[ MAX_PATH + 1 ];

	::GetIISMonPath( wszRoot );
	VERIFY( ::PathAppendW( wszRoot, L"Upload\\Incomplete" ) );
	DelDirWithFiles( wszRoot );

	::GetIISMonPath( wszRoot );
	VERIFY( ::PathAppendW( wszRoot, L"Upload" ) );
	DelDirWithFiles( wszRoot );

	::GetIISMonPath( wszRoot );
	VERIFY( ::PathAppendW( wszRoot, L"Log" ) );
	DelDirWithFiles( wszRoot );

	::GetIISMonPath( wszRoot );
	VERIFY( ::PathAppendW( wszRoot, L"1033" ) );
	DelDirWithFiles( wszRoot );

	::GetIISMonPath( wszRoot );
	DelDirWithFiles( wszRoot );

	if ( bRemoveTrail )
	{
		VERIFY( ::ExpandEnvironmentStringsW( L"%SystemDrive%", wszRoot, MAX_PATH + 1 ) != 0 );
		VERIFY( ::PathAppendW( wszRoot, L"IISMon" ) );
		DelDirWithFiles( wszRoot );
	}
}



void DelDirWithFiles( LPCWSTR wszDir )
{
	WIN32_FIND_DATAW	fd;
	WCHAR				wszPath[ MAX_PATH + 1 ];

	::wcscpy( wszPath, wszDir );
	VERIFY( ::PathAppendW( wszPath, L"*.*" ) );

	HANDLE				hSearch = ::FindFirstFileW( wszPath, &fd );

	// this is not a normal case. 
	if ( INVALID_HANDLE_VALUE == hSearch ) return;

	do
	{
		::wcscpy( wszPath, wszDir );

		// Skip directories. Delete only files
		if ( 0 == ( fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) )
		{
			VERIFY( ::PathAppendW( wszPath,fd.cFileName ) );

			// If we can't delete the file right now - may be it is locked. Schedule it for deletion at next boot
			if ( !::DeleteFileW( wszPath ) )
			{
				VERIFY( ::MoveFileExW( wszPath, NULL, MOVEFILE_DELAY_UNTIL_REBOOT ) );
			}

		}
	}while( ::FindNextFileW( hSearch, &fd ) );

	VERIFY( ::FindClose( hSearch ) );

	// Remove the directory ( should be empty now )
	// Again - if it cannot be deleted right now - schedule it for next boot
	if ( !::RemoveDirectoryW( wszDir ) )
	{
		VERIFY( ::MoveFileExW( wszDir, NULL, MOVEFILE_DELAY_UNTIL_REBOOT ) );
	}
}



// Installs a section
HRESULT	InstallFromINF()
{
	DECLARE_HR_SUCCESS;

	// The INF file must be in the same dir as this EXE.Build the path to ther INF file
	WCHAR	wszPath[ _MAX_PATH + 1 ];
	WCHAR	wszDrive[ _MAX_DRIVE + 1 ];
	WCHAR	wszFolder[ _MAX_DIR + 1 ];

	VERIFY( ::GetModuleFileNameW( NULL, wszPath, MAX_PATH ) != 0 );
	
	::_wsplitpath( wszPath, wszDrive, wszFolder, NULL, NULL );

	::_wmakepath( wszPath, wszDrive, wszFolder, L"IISMon", L"inf" );

	HINF		hInf	= ::SetupOpenInfFileW( wszPath, NULL, INF_STYLE_WIN4, 0 );

	// The file MUST exist - it is installed by the IExpress tool
	_ASSERT( hInf != INVALID_HANDLE_VALUE );

	BOOL bRes = ::SetupInstallFromInfSectionW(	NULL,
												hInf,
												L"DefaultInstall",
												SPINST_FILES | SPINST_REGISTRY,
												NULL,
												NULL,
												SP_COPY_NEWER_OR_SAME,
												INFInstallCallback,
												NULL,
												NULL,
												NULL );

	::SetupCloseInfFile( hInf );

	return bRes ? S_OK : E_FAIL;
}



UINT CALLBACK INFInstallCallback( PVOID pvCtx, UINT nNotif, UINT_PTR nP1, UINT_PTR nP2 )
{
	// Abort the installation for all errors
	if (	( SPFILENOTIFY_COPYERROR == nNotif ) ||
			( SPFILENOTIFY_RENAMEERROR == nNotif ) )
	{
		return FILEOP_ABORT;
	}

	// Allow the operation to execute
	return FILEOP_DOIT;
}



LPCTSTR Install( HINSTANCE hInstance, BOOL bAuditTrailEnabled, DWORD dwDaysToKeep )
{
	DECLARE_HR_SUCCESS;

	LPCTSTR szLocalError = NULL;
	WCHAR wszPath[ MAX_PATH + 1 ];
	::swprintf( wszPath, L"wmiadap.exe /F" );

	STARTUPINFOW		si = { 0 };
	PROCESS_INFORMATION	pi = { 0 };
	si.cb = sizeof( si );

	// Execute wmiadap.exe /f to refresh perf counters on this machine
	VERIFY( ::CreateProcessW( NULL, wszPath, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi ) );
	::CloseHandle( pi.hProcess );
	::CloseHandle( pi.hThread );

	if ( SUCCEEDED( hr ) )
	{
		szLocalError = ERR_COPYFAILED;
		hr = InstallFromINF();
	}	

	// Setup ACLs
	if ( SUCCEEDED( hr ) )
	{
		szLocalError = ERR_SETACLAILED;
		hr = SetupACLs();
	}

	// Register the iismon.wsc component
	if ( SUCCEEDED( hr ) )
	{
		::swprintf( wszPath, L"regsvr32 /s \"%%systemroot%%\\system32\\inetsrv\\iismon\\iismon.wsc\"" );
		VERIFY( LOWORD( ::DoEnvironmentSubstW( wszPath, MAX_PATH + 1 ) ) );
		::ZeroMemory( &pi, sizeof( pi ) );

		if ( !::CreateProcessW( NULL, wszPath , NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi ) )
		{
			hr = E_FAIL;
		}
		else
		{
			::CloseHandle( pi.hProcess );
			::CloseHandle( pi.hThread );
		}
	}

	// Setup the registry
	if ( SUCCEEDED( hr ) )
	{
		hr = SetupRegistry( bAuditTrailEnabled, dwDaysToKeep );
		szLocalError = ERR_REGERROR;
	}

	// Use local system account for now
 	if ( SUCCEEDED( hr ) )
	{
		hr = SetupTasks();
		szLocalError = ERR_TASKERROR;
	}
	
	// Setup dir structure
	if ( SUCCEEDED( hr ) )
	{
		hr = SetupDirStruct();
		szLocalError = ERR_DIRERROR;
	}	

	// Error handling
	if ( FAILED( hr ) )
	{
		// Try to not leave side effects
		Uninstall( FALSE );
	}

	return SUCCEEDED( hr ) ? NULL : szLocalError;
}


void Uninstall( BOOL bRemoveTrail )
{
	// Unregister iismon.wsc
	WCHAR				wszPath[ MAX_PATH + 1 ];
	STARTUPINFOW		si = { 0 };
	PROCESS_INFORMATION	pi = { 0 };
	si.cb = sizeof( si );

	::swprintf( wszPath, L"regsvr32 /s /u \"%%systemroot%%\\system32\\inetsrv\\iismon\\iismon.wsc\"" );
	VERIFY( LOWORD( ::DoEnvironmentSubstW( wszPath, MAX_PATH + 1 ) ) );
	VERIFY( ::CreateProcessW( NULL, wszPath, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi ) );
	::CloseHandle( pi.hProcess );
	::CloseHandle( pi.hThread );

	// Remove the tasks
	DeleteTasks();

	// Remove the files
	DeleteDirStruct( bRemoveTrail );

	// Remove the reg key
	DelIISMonKey();
}