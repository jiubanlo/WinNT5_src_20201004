// 
//
//***************************************************************************
//
//  judyp      May 1999        
//
//***************************************************************************

#include "stdafx.h"

#pragma warning (disable : 4786)
#pragma warning (disable : 4275)

#include <iostream>
#include <strstream>
#include <fstream>
#include <string>
#include <sstream>
#include <map>
#include <ctime>
#include <list>


using namespace std;


#include <tchar.h>
#include <windows.h>
#ifdef NONNT5
typedef unsigned long ULONG_PTR;
#endif
#include <wmistr.h>
#include <guiddef.h>
#include <initguid.h>
#include <evntrace.h>

#include <WTYPES.H>
#include "t_string.h"

#include "StructureWrappers.h"
#include "StructureWapperHelpers.h"
#include "ConstantMap.h" 
#include "TCOData.h"
#include "Persistor.h"
#include "Logger.h"
#include "Validator.h"
#include "Utilities.h"

#include "CollectionControl.h"
  
int QueryTraceAPI
(	
	IN LPTSTR lptstrAction,				// For logging only.
 	IN LPCTSTR lpctstrDataFile,			// For logging only.
	IN LPCTSTR lpctstrTCODetailFile,	// If valid we will log to it, can be NULL.
	IN bool bLogExpected,				// If true we log expected vs actual result.
	IN bool bUseTraceHandle,			// If true use the handle.
	IN OUT TCOData *pstructTCOData,		// TCO test data.
	OUT int *pAPIReturn					// QueryTrace API call return
)
{	
	// If bUseTracehandle is true we can check for differences in 
	// props.

	*pAPIReturn = -1;

	LPTSTR lptstrReturnedError = NULL;

	CLogger *pDetailLogger = NULL;

	int nResult = 0;

	// We only log if the test of "interest" is QueryTrace.
	if (pstructTCOData->m_ulAPITest == TCOData::QueryTraceTest)
	{
		nResult = 
			OpenLogFiles
			(	
				lpctstrTCODetailFile,
				pDetailLogger,
				&lptstrReturnedError
			);
	}

	if (FAILED(nResult))
	{
		delete pDetailLogger;
		//  Open log files sets error string plpstrReturnedError.

		LogSummaryBeforeCall
		(	
			pstructTCOData, 
			lpctstrDataFile,
			lptstrAction,
			_T("StartTrace"),
			bLogExpected
		);

		LogSummaryAfterCall
		(	
			pstructTCOData, 
			lpctstrDataFile,
			lptstrAction,
			nResult,
			lptstrReturnedError,
			bLogExpected
		);
		free (lptstrReturnedError);
		lptstrReturnedError = NULL;
		return nResult;
	}
			
	// This is our log file.
	if (pDetailLogger)
	{
		pDetailLogger->LogTCHAR(_T("\n-------------------------------------------------------\n"));
		pDetailLogger->LogTCHAR(_T("QueryTraceAPI TCO test "));
		pDetailLogger->Flush();	
	}

	if (pDetailLogger)
	{
		pDetailLogger->LogTCHAR(pstructTCOData->m_lptstrShortDesc);
		int n = pDetailLogger->LogTCHAR(_T(" started at time "));
		time_t Time;
		time(&Time);
		pDetailLogger->LogTime(Time);
		pDetailLogger->LogTCHAR(_T(".\n"));
		pDetailLogger->Flush();
	}

	BOOL bAdmin = IsAdmin();

	if (pDetailLogger)
	{
		// Log argument values before calling StopTrace.
		LogDetailBeforeCall
		(
			pDetailLogger,
			pstructTCOData,
			bAdmin
		);
	}

	CEventTraceProperties CProps (pstructTCOData->m_pProps);
	PEVENT_TRACE_PROPERTIES pProps = CProps.GetEventTracePropertiesInstance();
	free(pProps->LogFileName);
	free(pProps->LoggerName);
	pProps->LogFileName = (TCHAR *) malloc (sizeof(TCHAR) * MAX_STR);
	pProps->LoggerName = (TCHAR *) malloc (sizeof(TCHAR) * MAX_STR);

	LogSummaryBeforeCall
	(	
		pstructTCOData, 
		lpctstrDataFile,
		lptstrAction,
		_T("QueryTrace"),
		bLogExpected
	);

	*pAPIReturn = 
		QueryTrace
		( 
			bUseTraceHandle ? *pstructTCOData->m_pTraceHandle : NULL, 
			bUseTraceHandle ? NULL : pstructTCOData->m_lptstrInstanceName, 
			pProps);

	ULONG ulResult = pstructTCOData->m_ulExpectedResult == *pAPIReturn ? ERROR_SUCCESS : -1;

	if (ulResult != ERROR_SUCCESS && *pAPIReturn == ERROR_SUCCESS)
	{
		ulResult = *pAPIReturn;
	}
	else if (*pAPIReturn != ERROR_SUCCESS) 
	{
		lptstrReturnedError = DecodeStatus(*pAPIReturn);	
	}
	else if (pstructTCOData->m_ulAPITest == TCOData::QueryTraceTest &&
			 pstructTCOData->m_lptstrValidator &&
			 _tcslen(pstructTCOData->m_lptstrValidator) > 0)
	{
		CValidator Validator;
			
		bool bValid = 
			Validator.Validate
			(
				pstructTCOData->m_pTraceHandle, 
				pstructTCOData->m_lptstrInstanceName, 
				pstructTCOData->m_pProps, 
				pstructTCOData->m_lptstrValidator
			);

		if (!bValid)
		{
			ulResult = -1;
			lptstrReturnedError = NewTCHAR(_T("Validation routine failed."));
		}
	}

	bool bDiff = FALSE;
	if (bUseTraceHandle)
	{
		bDiff = 
			LogPropsDiff
			(
				NULL,
				pstructTCOData->m_pProps,
				pProps
			);
	}

	// If there were differences and 
	// *pAPIReturn == ERROR_SUCCESS == pstructTCOData->m_ulExpectedResult
	// we need to set return codes to indicate differences.  Look in detail
	// log for differences.
	if (bDiff &&
		*pAPIReturn == ERROR_SUCCESS &&
		pstructTCOData->m_ulExpectedResult == ERROR_SUCCESS)
	{
		free (lptstrReturnedError);
		lptstrReturnedError = NULL;
		*pAPIReturn = -1;
		lptstrReturnedError = 
			NewTCHAR(_T("Different values returned in EVENT_TRACE_PROPERTIES structure."));
	}

	if (pDetailLogger)
	{
		LogDetailAfterCall
		(	pDetailLogger,
			pstructTCOData,
			&pProps,
			*pAPIReturn,
			lptstrReturnedError,
			true,
			bAdmin,
			NULL,
			*pAPIReturn == ERROR_SUCCESS // Do not print out Props if not a good return.
		);	
	}

	if (bUseTraceHandle)
	{
		LogPropsDiff
			(
				pDetailLogger,
				pstructTCOData->m_pProps,
				pProps
			);
	}

	LogSummaryAfterCall
		(	
			pstructTCOData, 
			lpctstrDataFile,
			lptstrAction,
			*pAPIReturn,
			lptstrReturnedError,
			bLogExpected
		);
	
	free (lptstrReturnedError);
	lptstrReturnedError = NULL;

	delete pDetailLogger;

	if (pProps)
	{
		free(pProps->LoggerName);
		free(pProps->LogFileName);
	}
	free(pProps);	

	return ulResult;
}