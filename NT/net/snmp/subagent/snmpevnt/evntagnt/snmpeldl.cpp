/*++

Copyright (c) 1994  Microsoft Corporation

Module Name:

        SNMPELDL.CPP


Abstract:

        This module is the main extension agent DLL for the SNMP Event Log
        Extension Agent DLL.

        Standard tracing and logging routines are defined in this module.

        DLL initialization and termination routines are defined here.

        SNMP trap initialization and processing is defined here.

        Spawning of the log processing thread is done in this routine.

Author:

        Randy G. Braze (Braze Computing Services) Created 16 October 1994


Revision History:
        9 Feb 99    FlorinT: Removed warning event logs on startup (when registry parameters are not found)

        16 Dec 98   FlorinT: Added LoadPrimaryModuleParams and 'EALoad.*' files

        1 Dec 98    FlorinT: Remove psupportedView - it is freed by the SNMP master agent

        7 Feb 96    Moved tracing and logging to separate module.
                        Restructured building of varbinds to be outside of trap generation.
                        Calculated trap buffer length correctly.
                        Created varbind queue and removed event log buffer queue.

        28 Feb 96   Added code to support a performance threshold reached indicator.
                        Removed pointer references to varbindlist and enterpriseoid.

        10 Mar 96   Removed OemToChar coding and registry checking.
                        Modifications to read log file names from EventLog registry entries and not
                        from specific entries in the SNMP Extension Agent's registry entries.
                        Included SnmpMgrStrToOid as an internal function, as opposed to using the function
                        provided by MGMTAPI.DLL. SNMPTRAP.EXE will be called if MGMTAPI is called, which
                        will disable other agents from being able to receive any traps. All references
                        to MGMTAPI.DLL and MGMTAPI.H will be removed.
                        Added a ThresholdEnabled flag to the registry to indicate if the threshold values
                        were to be monitored or ignored.

        13 Mar 96   Modified StrToOid routine to append the BaseEnterpriseOID to the specified string
                        if the string passed does not start with a period. Otherwise, the string is
                        converted as normal.
                        Changed TraceFileName parameter in the registry from REG_SZ to REG_EXPAND_SZ.
                        Modified registry reading routine to accept REG_EXPAND_SZ parameters.
                        Added additional tracing.

        07 May 96   Removed SnmpUtilOidFree and use two SNMP_free. One for the OID's ids array and
                        one for the OID itself. Also added psupportedView to free memory at exit.

        26 Jun 96   Modified the StrToOid function such that strings without leading "." have the base
                        OID appended instead of strings with a leading ".".

--*/

extern "C"
{
#include <stdlib.h>
#include <errno.h>          // for ERANGE
#include <windows.h>        // windows definitions
#include <string.h>         // string declarations
#include "limits.h"         // for INT_MAX
#include <snmp.h>           // snmp definitions
#include <snmpexts.h>       // extension agent definitions
#include "snmpelea.h"       // global dll definitions
#include "snmpeldl.h"       // module specific definitions
#include "snmpelmg.h"       // message definitions
}

extern  VOID                FreeVarBind(UINT,RFC1157VarBindList     *);
#include "snmpelep.h"       // c++ variables and definitions
#include "EALoad.h"         // parameters loading functions



// MikeCure 4/3/98 hotfix for SMS Bug1 #20521
//===========================================
BOOL EnablePrivilege(VOID);


VOID
ResetGlobals(
    IN  VOID
    )

/*++

Routine Description:

        This routine is used to reset/init all globals variables.
        
        This is to handle the case that SNMP.exe calls our entry points with 
        the following sequence:
        1. Our DLL is attached to SNMP process
        2. SnmpExtensionInit
        3. SnmpExtensionClose
        4. Our DLL is not detached from SNMP process
        5. SnmpExtensionInit

Arguments:

        None


Return Value:

        None

--*/
{
    WriteTrace(0x0a,"ResetGlobals: Entering extension agent ResetGlobals routine\n");

    lpszEventLogs = (LPTSTR) NULL;          // pointer to event log name array
    phEventLogs = (PHANDLE) NULL;           // Pointer to event log handle array
    phPrimHandles = (PHMODULE) NULL;        // pointer to primary module handle array
    uNumEventLogs = 0;                      // Number of event logs present
    iLogNameSize = 0;                       // size of event log name array
    nMaxTrapSize = 0;                       // maximum trap size
    
    hkRegResult = (HKEY) NULL;              // handle to Parameters registry entry
    hWriteEvent = (HANDLE) NULL;            // handle to write log events
    hStopAll = (HANDLE) NULL;               // handle to global dll shutdown event
    hServThrd = (HANDLE) NULL;              // handle to SNMPELPT thread
    hEventNotify = (HANDLE) NULL;           // handle to notify SNMPELDL that a trap is ready to process
    hRegChanged = (HANDLE) NULL;            // handle to registry key changed event
    
    fGlobalTrim = TRUE;                     // global message trimming flag (trim msg first or insertion strings first)
    fThresholdEnabled = TRUE;               // global threshold checking enabled flag
    fTrimFlag = FALSE;                      // global trimming flag (do or don't do trimming at all)
    fTrapSent = FALSE;                      // global trap sent flag
    fRegNotify = FALSE;                     // registry notification initialization flag
    fRegOk = FALSE;                         // registry notification in effect flag
    fLogInit = FALSE;                       // indicate log file registry information not yet read
    fThreshold = FALSE;                     // global performance threshold flag
    fSendThresholdTrap = FALSE;             // indicator to send threshold reached trap
    fDoLogonEvents = TRUE;                  // do we need to send logon events

    szBaseOID[MAX_PATH] = 0;                // base enterprise OID
    szSupView[MAX_PATH] = 0;                // supported view OID

    dwTimeZero = 0;                         // time zero reference
    dwThresholdCount = THRESHOLD_COUNT;     // threshold count for performance
    dwThresholdTime = THRESHOLD_TIME;       // time in seconds for threshold for performance

    dwTrapCount = 0;                        // number of traps sent
    dwTrapStartTime = 0;                    // time when first trap in time period sent
    dwLastBootTime = 0;                     // time the last boot occurred
    dwTrapQueueSize = 0;
    
    // WinSE Bug# 15128, Windows Bug# 293698  
    // default is INFINITE ==> no polling (wait for system notification)
    g_dwEventLogPollTime = (DWORD)INFINITE; // time in millsecs to poll for event logs
    
    lpVarBindQueue = (PVarBindQueue) NULL;  // pointer to first varbind queue entry
    lpSourceHandleList = (PSourceHandleList) NULL;  // pointer to first source/handle entry

    // tracing globals
    szTraceFileName[MAX_PATH] = 0;          // file name for trace information (from registry)
    szTraceFileName[0] = 0;                 // init to null tracing filename
    fTraceFileName = FALSE;                 // flag indicating registry read for trace file name    
    fMsgModule = FALSE;                     // flag indicating registry read for message module
    hMsgModule = NULL;                      // handle to message module

    hMutex = NULL;                          // handle for mutex object

}

BOOL
StrToOid(   
        IN              PCHAR                    lpszStr,
        IN      OUT     AsnObjectIdentifier     *asnOid
    )

/*++

Routine Description:

        This routine will convert the string passed to an OID.


Arguments:

        lpszStr -       Address of a null terminated string in the form n.n...n.

        asnOid  -       Address of converted OID of the input string.


Return Value:

        TRUE    -       If the string was successfully converted.

        FALSE   -       If the string could not be converted.

--*/

{
    CHAR    tokens[] = TEXT(".");           // delimiters to scan for
    CHAR    *token;                         // token returned
    UINT    nTokens;                        // number of tokens located
    UINT    i;                              // temporary counter
    CHAR    szString[MAX_PATH*2+1];         // temporary string holder
    CHAR    szOrgString[MAX_PATH*2+1];      // temporary string holder
    CHAR    *szStopString;                  // temporary string pointer
    BOOL    fOk;                            // used by WRAP_STRCAT_A macro
    // ensures null terminated strings
    szString[MAX_PATH*2]    = 0;
    szOrgString[MAX_PATH*2] = 0;

    WriteTrace(0x0a,"StrToOid: Entering routine to convert string to OID\n");
    WriteTrace(0x00,"StrToOid: String to convert is %s\n", lpszStr);

    nTokens = 0;                                            // reset counter

    if ( strlen(lpszStr) == 0 )
    {
        WriteTrace(0x14,"StrToOid: No strings found. Exiting with FALSE\n");
        return(FALSE);
    }

    if (lpszStr[0] != '.')
    {
        WriteTrace(0x0a,"StrToOid: BaseOID will not be appended to this OID\n");
        strncpy(szString, lpszStr, MAX_PATH*2); // copy original string
    }
    else
    {
        WriteTrace(0x0a,"StrToOid: BaseOID %s will be appended to this OID\n", szBaseOID);
        strncpy(szString, szBaseOID, MAX_PATH*2);   // copy in base OID
        WRAP_STRCAT_A(szString, TEXT("."), MAX_PATH*2);        // stick in the .
        WRAP_STRCAT_A(szString, lpszStr, MAX_PATH*2);          // add in requested string
    }

    strncpy(szOrgString, szString, MAX_PATH*2);        // save this string

    token = strtok(szString, tokens);       // get first token

    while (token != NULL)
    {
        szStopString = token;                           // set pointer to string
        strtoul(token, &szStopString, 10);      // check for valid values
        if (errno == ERANGE)
        {
            errno = 0;
            WriteTrace(0x14,"StrToOid: strtoul overflow or underflow. Exiting with FALSE\n");
            return(FALSE);
        }
        if ( (token == szStopString) || (*szStopString != NULL) )
        {
            WriteTrace(0x14,"StrToOid: String contains a non-numeric value. Exiting with FALSE\n");
            WriteLog(SNMPELEA_NON_NUMERIC_OID);
            return(FALSE);
        }

        nTokens++;                                              // increment number of tokens found
        token = strtok(NULL, tokens);   // get next token
    }

    if (nTokens == 0)
    {
        WriteTrace(0x14,"StrToOid: No strings found. Exiting with FALSE\n");
        return(FALSE);
    }

    WriteTrace(0x00,"StrToOid: %lu tokens found\n", nTokens);
    WriteTrace(0x0a,"StrToOid: Allocating storage for OID\n");

    asnOid->ids = (UINT *) SNMP_malloc(nTokens * sizeof(UINT));     // allocate integer array

    if (asnOid->ids == NULL)
    {
        WriteTrace(0x14,"StrToOid: Unable to allocate integer array for OID structure. Exiting with FALSE\n");
        WriteLog(SNMPELEA_CANT_ALLOCATE_OID_ARRAY);
        return(FALSE);
    }

    WriteTrace(0x00,"StrToOid: OID integer array storage allocated at %08X\n", asnOid->ids);

    asnOid->idLength = nTokens;                     // set size of array
    strncpy(szString, szOrgString, MAX_PATH*2);     // copy original string
    token = strtok(szString, tokens);               // get first token
    i = 0;                                          // set index to 0

    while (token != NULL)
    {
        asnOid->ids[i++] = strtoul(token, &szStopString, 10);   // convert string to number
        token = strtok(NULL, tokens);                           // get next token
    }

    if (nTraceLevel == 0)
    {
        for (i = 0; i < nTokens; i++)
        {
            WriteTrace(0x00,"StrToOid: OID[%lu] is %lu\n",
                        i, asnOid->ids[i]);
        }
    }

    WriteTrace(0x0a,"StrToOid: Exiting routine with TRUE\n");
    return(TRUE);

Error:
    WriteTrace(0x14,"StrToOid: Unable to do strcat. Exiting with FALSE\n");
    return (FALSE);
}


VOID
CloseStopAll(
    IN  VOID
    )

/*++

Routine Description:

        This routine will close the event handle used to terminate the extension agent dll.


Arguments:

        None


Return Value:

        None

--*/

{
    LONG    lastError;                              // for GetLastError()

    if (hStopAll == NULL)
    {
            return;
    }

    WriteTrace(0x0a,"CloseStopAll: Closing handle to service shutdown event %08X\n",
                    hStopAll);
    if ( !CloseHandle(hStopAll) )
    {
        lastError = GetLastError(); // save status
        WriteTrace(0x14,"CloseStopAll: Error closing handle for service shutdown event %08X; code %lu\n",
                        hStopAll, lastError);
        WriteLog(SNMPELEA_ERROR_CLOSING_STOP_AGENT_HANDLE,
                    HandleToUlong(hStopAll), lastError);
    }
    
    // reset handle
    hStopAll = NULL;
}


VOID
CloseEventNotify(
    IN  VOID
    )

/*++

Routine Description:

        This routine will close the event handle used to notify SNMPELDL that a
        trap is waiting to be sent.


Arguments:

        None


Return Value:

        None

--*/

{
    LONG    lastError;                              // for GetLastError()

    if (hEventNotify == NULL)
    {
            return;
    }

    WriteTrace(0x0a,"CloseEventNotify: Closing handle to event notify event %08X\n",
            hEventNotify);
    if ( !CloseHandle(hEventNotify) )       // close log processing routine shutdown event handle
    {
            lastError = GetLastError(); // save error status
            WriteTrace(0x14,"CloseEventNotify: Error closing handle for StopLog event %08X; code %lu\n",
                    hEventNotify, lastError);
            WriteLog(SNMPELEA_ERROR_CLOSING_STOP_LOG_EVENT_HANDLE,
                    HandleToUlong(hEventNotify), lastError);
    }
    
    // reset handle
    hEventNotify = NULL;
}


VOID
CloseRegNotify(
    IN  VOID
    )

/*++

Routine Description:

        This routine will close the event handle used to notify SNMPELDL that a
        registry key value has changed.


Arguments:

        None


Return Value:

        None

--*/

{
    LONG    lastError;                              // for GetLastError()

    if (hRegChanged == NULL)
    {
            return;
    }

    WriteTrace(0x0a,"CloseRegNotify: Closing handle to registry key changed notify event %08X\n",
            hRegChanged);
    if ( !CloseHandle(hRegChanged) )        // close event handle
    {
            lastError = GetLastError(); // save error status
            WriteTrace(0x14,"CloseRegNotify: Error closing handle for registry key changed event %08X; code %lu\n",
                    hRegChanged, lastError);
            WriteLog(SNMPELEA_ERROR_CLOSING_REG_CHANGED_EVENT_HANDLE,
                    HandleToUlong(hRegChanged), lastError);
    }

    // reset handle
    hRegChanged = NULL;
}


VOID
CloseRegParmKey(
    IN  VOID
    )

/*++

Routine Description:

        This routine will close the registry key handle used to read the Parameters information
        from the registry.


Arguments:

        None


Return Value:

        None

--*/

{
    LONG    lastError;

    if (hkRegResult == NULL)
    {
            return;
    }

    WriteTrace(0x0a,"CloseRegParmKey: Closing Parameter key in registry\n");
    if ( (lastError = RegCloseKey(hkRegResult)) != ERROR_SUCCESS )  // close handle
    {
            WriteTrace(0x14,"CloseRegParmKey: Error closing handle for Parameters registry key %08X; code %lu\n",
                    hkRegResult, lastError);
            WriteLog(SNMPELEA_ERROR_CLOSING_REG_PARM_KEY,
                    HandleToUlong(hkRegResult), lastError);
    }

    // reset handle
    hkRegResult = NULL;
}

VOID
CloseLogs(
        IN      VOID
        )

/*++

Routine Description:

        This routine is called to close the currently open event logs. It is
        called when the agent is terminating normally and if an error is
        encountered during agent initialization.


Arguments:

        None


Return Value:

        None


--*/

{
    UINT   uVal;                                        // temporary counter

    WriteTrace(0x0a,"CloseLogs: Closing event logs\n");
    
    for (uVal = 0; uVal < uNumEventLogs; uVal++)
    {
        if (phEventLogs && (*(phEventLogs+uVal) != NULL))
        {
            WriteTrace(0x00,"CloseLogs: Closing event log %s, handle %lu at %08X\n",
                            lpszEventLogs+uVal*(MAX_PATH+1), uVal, *(phEventLogs+uVal));
            CloseEventLog(*(phEventLogs+uVal));
        }
        if (phPrimHandles && (*(phPrimHandles+uVal) != (HMODULE) NULL))
        {
            WriteTrace(0x00,"CloseLogs: Freeing PrimaryModule for event log %s, handle %lu at %08X\n",
                            lpszEventLogs+uVal*(MAX_PATH+1), uVal, *(phPrimHandles+uVal));
            FreeLibrary(*(phPrimHandles+uVal));
        }
    }
    
    WriteTrace(0x0a,"CloseLogs: Freeing memory for event log handles at address %08X\n",
                phEventLogs);
    SNMP_free( (LPVOID) phEventLogs );          // free event log handle array
    phEventLogs = (PHANDLE) NULL; 

    WriteTrace(0x0a,"CloseLogs: Freeing memory for PrimaryModule handles at address %08X\n",
                phPrimHandles);
    SNMP_free( (LPVOID) phPrimHandles );        // free primary module handle array
    phPrimHandles = (PHMODULE) NULL; 
    
    WriteTrace(0x0a,"CloseLogs: Freeing memory for event log names at address %08X\n",
                lpszEventLogs);
    SNMP_free( (LPVOID) lpszEventLogs );        // free log name array
    lpszEventLogs = (LPTSTR) NULL;
}


extern "C" {
BOOL
Position_to_Log_End(
        IN      HANDLE  hLog
        )

/*++

Routine Description:

        Position_to_Log_End is called during DLL initialization. After each
        event log file is successfully opened, it is necessary to position each
        event log file to the current end of file. This way, only the events
        logged after the DLL has been started are reported. This routine will
        position the requested log to the end of file.

        Positioning to the end of the event log file is done by first getting
        the number of the oldest event log record. This value is then added to
        the number of event log records minus one. The resulting value is the
        record number of the last record in the event log file. ReadEventLog is
        called, specifying the seek parameter, to position to that exact record
        number.


Arguments:

        hLog                    -       Handle of the log file to position to end of file.


Return Value:

        TRUE    -       If the log was successfully positioned to end of file.

        FALSE   -       If the log was not positioned to end of file.


--*/

{
    LONG                lastError;                      // last error code
    PEVENTLOGRECORD     lpBuffer;                       // address of data buffer
    PEVENTLOGRECORD     lpOrigBuffer;                   // address of data buffer
    DWORD               nBytesRead;                     // number of bytes read
    DWORD               nMinNumberofBytesNeeded;        // remainder if buffer too small
    DWORD               dwOldestRecord;                 // oldest record number in event log
    DWORD               dwRecords;                      // total number of records in event log
    DWORD               uRecordNumber;                  // current log position

    WriteTrace(0x0a,"Position_to_Log_End: Entering position to end of log routine\n");
    WriteTrace(0x00,"Position_to_Log_End: Handle is %08X\n",hLog);

    if (!hLog)                     // if handle is invalid
    {                              //    then we cannot position correctly
        WriteTrace(0x14,"Position_to_Log_End: Handle for end of log is invalid - %08X\n",
                        hLog);
        WriteTrace(0x14,"Position_to_Log_End: Log position to end failed\n");
        WriteLog(SNMPELEA_LOG_HANDLE_INVALID, HandleToUlong(hLog)); // log error message
        WriteLog(SNMPELEA_ERROR_LOG_END);            // log the message

        return FALSE;              // exit function
    }

    WriteTrace(0x0a,"Position_to_Log_End: Allocating log buffer\n");

    lpBuffer = (PEVENTLOGRECORD) SNMP_malloc(LOG_BUF_SIZE);  // allocate memory for buffer

    if (lpBuffer == (PEVENTLOGRECORD) NULL)              // if memory allocation failed
    {                                  //    then can't position log file
        WriteTrace(0x14,"Position_to_Log_End: Position to end of log for handle %08X failed\n",
                        hLog);
        WriteTrace(0x14,"Position_to_Log_End: Buffer memory allocation failed\n");
        WriteLog(SNMPELEA_ERROR_LOG_BUFFER_ALLOCATE, HandleToUlong(hLog));  // log error message
        WriteLog(SNMPELEA_ERROR_LOG_END);                    // log the message

        return FALSE;                  //    exit the function
    }

    WriteTrace(0x00,"Position_to_Log_End: Log buffer memory allocated at %08X\n", lpBuffer);
    WriteTrace(0x0a,"Position_to_Log_End: Positioning to last record\n");

    WriteTrace(0x0a,"Position_to_Log_End: Getting oldest event log record\n");
    if ( !GetOldestEventLogRecord(hLog, &dwOldestRecord) )
    {
        lastError = GetLastError();       // get last error code

        WriteTrace(0x0a,"Position_to_Log_End: Freeing log event record buffer %08X\n",
                        lpBuffer);
        SNMP_free(lpBuffer);                   // free up buffer memory

        WriteTrace(0x14,"Position_to_Log_End: GetOldestEventLogRecord for log handle %08X failed with code %lu\n",
            hLog, lastError);
        WriteLog(SNMPELEA_ERROR_LOG_GET_OLDEST_RECORD, HandleToUlong(hLog), lastError); // log error message
        WriteLog(SNMPELEA_ERROR_LOG_END);                                // log the message

        return FALSE;
    }

    WriteTrace(0x00,"Position_to_Log_End: Oldest event log record is %lu\n",dwOldestRecord);

    WriteTrace(0x00,"Position_to_Log_End: Getting number of event log records\n");
    if ( !GetNumberOfEventLogRecords(hLog, &dwRecords) )
    {
        lastError = GetLastError();       // get last error code

        WriteTrace(0x0a,"Position_to_Log_End: Freeing log event record buffer\n");
        SNMP_free(lpBuffer);                   // free up buffer memory

        WriteTrace(0x14,"Position_to_Log_End: GetNumberOfEventLogRecords for log handle %08X failed with code %lu\n",
            hLog, lastError);
        WriteLog(SNMPELEA_ERROR_LOG_GET_NUMBER_RECORD, HandleToUlong(hLog), lastError); // log error message
        WriteLog(SNMPELEA_ERROR_LOG_END);                                // log the message

        return FALSE;
    }

    WriteTrace(0x00,"Position_to_Log_End: Number of event log records is %lu\n",dwRecords);

    uRecordNumber = dwOldestRecord + dwRecords - 1;         // current EOF

    WriteTrace(0x00,"Position_to_Log_End: Positioning to record #%lu\n",
                    uRecordNumber);

    if ( !ReadEventLog(hLog,                    // log file handle to read
                EVENTLOG_SEEK_READ | EVENTLOG_FORWARDS_READ,    // seek forward to specific record
                uRecordNumber,                                  // record # to position to
                lpBuffer,                                       // buffer to return log record in
                LOG_BUF_SIZE,                                   // size of buffer
                &nBytesRead,                                    // return bytes read this time
                &nMinNumberofBytesNeeded))              // return bytes needed for next full record
    {
        lastError = GetLastError();             // get last error code

        WriteTrace(0x0a,"Position_to_Log_End: Freeing log event record buffer %08X\n",
                        lpBuffer);
        SNMP_free(lpBuffer);                   // free buffer memory

        if (lastError == ERROR_HANDLE_EOF)
        {
            WriteTrace(0x00,"Position_to_Log_End: Handle %08X positioned at EOF\n",hLog);
            WriteTrace(0x0a,"Position_to_Log_End: Returning from position to end of log function\n");

            return TRUE;
        }

        WriteTrace(0x14,"Position_to_Log_End: SEEK to record in event log %08X failed with code %lu\n",
            hLog, lastError);
        WriteLog(SNMPELEA_ERROR_LOG_SEEK, HandleToUlong(hLog), lastError); // log error message
        WriteLog(SNMPELEA_ERROR_LOG_END);                   // log the message

        WriteTrace(0x00,"Position_to_Log_End: BytesRead is %lu\n", nBytesRead);
        WriteTrace(0x00,"Position_to_Log_End: MinNumberofBytesNeeded is %lu\n",
                    nMinNumberofBytesNeeded);

        return FALSE;
    }

    WriteTrace(0x0a,"Position_to_Log_End: Reading any residual records\n");
        lpOrigBuffer = lpBuffer;            // save original buffer address
        nBytesRead = 0;                     // reset byte count to nothing first
        lastError = 0;                      // show no current error

    while (ReadEventLog(hLog,
        EVENTLOG_FORWARDS_READ | EVENTLOG_SEQUENTIAL_READ,
        0,
        lpBuffer,
        LOG_BUF_SIZE,
        &nBytesRead,
        &nMinNumberofBytesNeeded))
    {
                while (nBytesRead)
                {
                        WriteTrace(0x00,"Position_to_Log_End: Number of bytes read for residual read is %lu\n",
                                nBytesRead);
                        uRecordNumber = lpBuffer->RecordNumber; // save record number
                        nBytesRead -= lpBuffer->Length;         // reduce by this record count
                        lpBuffer = (PEVENTLOGRECORD) ((LPBYTE) lpBuffer +
                                lpBuffer->Length);                              // point to next record
                }
                lpBuffer = lpOrigBuffer;                // reload original address
    }

    lastError = GetLastError();                     // get last error code

    WriteTrace(0x0a,"Position_to_Log_End: Checking for EOF return\n");


    WriteTrace(0x0a,"Position_to_Log_End: Freeing event log buffer memory %08X\n",
                lpOrigBuffer);
    SNMP_free(lpOrigBuffer);                                    // free buffer memory

    if ( (lastError == ERROR_HANDLE_EOF) ||     // if at the last record now
                 (lastError == NO_ERROR) )              // if no error occured
    {
        WriteTrace(0x00,"Position_to_Log_End: Handle %08X positioned at EOF; record #%lu\n",
                        hLog, uRecordNumber);
        WriteTrace(0x0a,"Position_to_Log_End: Returning from position to end of log function\n");

        return TRUE;                   // return all okay
    }
    else                              // otherwise
    {
        WriteTrace(0x14,"Position_to_Log_End: Read for handle %08X failed with code %lu\n",
                        hLog, lastError);
        WriteTrace(0x14,"Position_to_Log_End: Log not positioned to end\n");
        WriteLog(SNMPELEA_ERROR_READ_LOG_EVENT, HandleToUlong(hLog), lastError); // log error message
        WriteLog(SNMPELEA_ERROR_LOG_END);                         // log the message

        return FALSE;                  // give bad return code
    }
}
}

extern "C" {
BOOL
Read_Registry_Parameters(
        IN VOID
        )

/*++

Routine Description:

        Read_Registry_Parameters is called during SNMP trap initialization. The
        registry information is read to determine the trace file name (TraceFileName),
        the level of tracing desired (TraceLevel), the base enterprise OID (BaseEnterpriseOID),
        the supported view (SupportedView), and the message trimming flag (TrimMessage).
        Also, the names of the event logs to monitor are read from the registry.
        If no event logs are specified, the routine will terminate, as there is no work to perform.

        If, during the course of reading the registry information, a parameter
        is encountered that is not expected, an event log record is written and
        the parameter is ignored.

        The registry layout is as follows:

        HKEY_LOCAL_MACHINE
                SOFTWARE
                        Microsoft
                                SNMP_EVENTS
                                        EventLog
                                                Parameters
                                                        TraceFileName                   (REG_EXPAND_SZ)
                                                        TraceLevel                      (REG_DWORD)
                                                        BaseEnterpriseOID               (REG_SZ)
                                                        SupportedView                   (REG_SZ)
                                                        TrimMessage                     (REG_DWORD)
                                                        MaxTrapSize                     (REG_DWORD)
                                                        TrimFlag                        (REG_DWORD)
                                                        ThresholdEnabled                (REG_DWORD)
                                                        ThresholdFlag                   (REG_DWORD)
                                                        ThresholdCount                  (REG_DWORD)
                                                        ThresholdTime                   (REG_DWORD)
                                                        LastBootTime                    (REG_DWORD)
                                                        EventLogPollTime                (REG_DWORD)
                                                        VarBindPrefixSubId              (REG_DWORD)

Arguments:

        None


Return Value:

        TRUE    -       If registry parameters were processed successfully.

        FALSE   -       If registry parameters could not be read or if there were
                                no event logs specified to monitor.


--*/

{
    LONG    lastError;                      // return code from GetLastError()
    LONG    status;                         // status of API calls
    HKEY    hkResult, hkResult2;            // handle returned from API
    DWORD   iValue;                         // temporary counter
    DWORD   dwType;                         // type of the parameter read
    TCHAR   parmName[MAX_PATH+1];           // name of the parameter read
    DWORD   nameSize;                       // length of parameter name
    TCHAR   parm[MAX_PATH+1];               // value of the parameter
    DWORD   parmSize;                       // length of the parm value
    HANDLE  hLogFile;                       // handle from log open
    UINT    uVal;                           // loop counter
    BOOL    fTrimMsg = FALSE;               // registry info found flags
    BOOL    fBaseOID = FALSE;               // registry info found flags
    BOOL    fSupView = FALSE;               // registry info found flags
    BOOL    fTrapSize = FALSE;              // registry info found flags
    BOOL    fTrimFlg = FALSE;               // registry info found flags
    BOOL    fThresholdFlg = FALSE;          // registry info found flags
    BOOL    fThresholdCountFlg = FALSE;     // registry info found flags
    BOOL    fThresholdTimeFlg = FALSE;      // registry info found flags
    BOOL    fTraceLevelFlg = FALSE;         // registry info found flags
    BOOL    fThresholdEnabledFlg = FALSE;   // registry info found flags
    BOOL    fThresholdOff = FALSE;          // temporary flag
    DWORD   nReadBytes = 0;                 // number of bytes read from profile information
    TCHAR   lpszLog[MAX_PATH+1];            // temporary registry name
    BOOL    fLastBootFlg = FALSE;           // registry info found flag
    PHANDLE phTmpEventLogs = (PHANDLE) NULL;// Pointer to event log handle array
    LPTSTR  lpszTmpEventLogs = (LPTSTR) NULL;  // pointer to event log name array
    PHMODULE phTmpPrimHandles = (PHMODULE) NULL;  // pointer to primary module handle array
    BOOL     fOk;                           // used by WRAP_STRCAT_A macro
    
    WriteTrace(0x0a,"Read_Registry_Parameters: Entering routine\n");

    parmName[MAX_PATH] = 0;
    parm[MAX_PATH]     = 0;
    lpszLog[MAX_PATH]  = 0;

    if (fSendThresholdTrap)
    {
        WriteTrace(0x0a,"Read_Registry_Parameters: Routined entered due to threshold performance parameters reached and modified.\n");

        if ( hRegChanged != NULL )
        {
            if ( (lastError = RegNotifyChangeKeyValue(
                    hkRegResult,                                            // handle of key to watch
                    TRUE,                                                                                                   // watch subkey stuff
                    REG_NOTIFY_CHANGE_NAME | REG_NOTIFY_CHANGE_LAST_SET,    // types of changes to notify on
                    hRegChanged,                                            // event to signal when change
                    TRUE)) == ERROR_SUCCESS )                               // asynchronous processing
            {
                WriteTrace(0x0a,"Read_Registry_Parameters: Notification of registry key changes was successful.\n");
                fRegOk = TRUE;                          // show registry notification is in effect
            }
            else
            {
                WriteTrace(0x14,"Read_Registry_Parameters: Notification of registry key changes failed with code of %lu\n",
                        lastError);
                WriteLog(SNMPELEA_REG_NOTIFY_CHANGE_FAILED, lastError);
                WriteTrace(0x14,"Read_Registry_Parameters: Initialization continues, but registry changes will require a restart of SNMP\n");
                CloseRegNotify();
            }
        }

        WriteTrace(0x0a,"Read_Registry_Parameters: Exiting Read_Registry_Parameters routine with TRUE.\n");
        return(TRUE);
    }

    WriteTrace(0x0a,"Read_Registry_Parameters: Opening %s\n", EXTENSION_PARM);

    if (fThreshold && fThresholdEnabled)    // if threshold checking enabled and threshold reached
    {
            fThresholdOff = TRUE;                           // indicate that we're not sending traps right now
    }
    else
    {
            fThresholdOff = FALSE;                          // otherwise, indicate we are sending traps right now
    }

    if (hkRegResult == NULL)
    {
        if ((status = RegOpenKeyEx(HKEY_LOCAL_MACHINE, EXTENSION_PARM, 0,
                KEY_READ | KEY_SET_VALUE, &hkRegResult))
                != ERROR_SUCCESS)                   // open registry information
        {
            WriteTrace(0x14,"Read_Registry_Parameters: Error in RegOpenKeyEx for Parameters = %lu \n",
                        status);
            WriteLog(SNMPELEA_NO_REGISTRY_PARAMETERS, status);   // log error message
            return(FALSE);                                  // failed -- can't continue
        }
    }

    if (!fRegNotify)
    {
        WriteTrace(0x0a,"Read_Registry_Parameters: Creating event for registry change notification\n");
        fRegNotify = TRUE;                              // set flag to show initialization complete

        if ( (hRegChanged = CreateEvent(
                (LPSECURITY_ATTRIBUTES) NULL,
                FALSE,
                FALSE,
                (LPTSTR) NULL)) == NULL)
        {
            lastError = GetLastError(); // save error status
            WriteTrace(0x14,"Read_Registry_Parameters: Error creating registry change notification event; code %lu\n",
                    lastError);
            WriteLog(SNMPELEA_ERROR_CREATING_REG_CHANGE_EVENT, lastError);

            WriteTrace(0x14,"Read_Registry_Parameters: No registry notification will be performed. Continuing with initialization.\n");
        }
        else
        {
            WriteTrace(0x00,"Read_Registry_Parameters: Registry key changed event handle is %08X\n",
                    hRegChanged);
        }
    }

    if ( hRegChanged != NULL )
    {
        if ( (lastError = RegNotifyChangeKeyValue(
                hkRegResult,                                                                                    // handle of key to watch
                TRUE,                                                                                                   // watch subkey stuff
                REG_NOTIFY_CHANGE_NAME | REG_NOTIFY_CHANGE_LAST_SET,    // types of changes to notify on
                hRegChanged,                                                                                    // event to signal when change
                TRUE)) == ERROR_SUCCESS )                                                               // asynchronous processing
        {
            WriteTrace(0x0a,"Read_Registry_Parameters: Notification of registry key changes was successful.\n");
            fRegOk = TRUE;                          // show registry notification is in effect
        }
        else
        {
            WriteTrace(0x14,"Read_Registry_Parameters: Notification of registry key changes failed with code of %lu\n",
                    lastError);
            WriteLog(SNMPELEA_REG_NOTIFY_CHANGE_FAILED, lastError);
            WriteTrace(0x14,"Read_Registry_Parameters: Initialization continues, but registry changes will require a restart of SNMP\n");
            CloseRegNotify();
        }
    }

    iValue = 0;                              // read first parameter
    nameSize = MAX_PATH;                     // can't be greater than this, size in TCHARs
    parmSize = sizeof(parm) - sizeof(TCHAR); // can't be greater than this, size in bytes
    g_dwEventLogPollTime = (DWORD) INFINITE; // default value is not to poll Event Logs
    g_dwVarBindPrefixSubId = g_dwDefaultVarBindPrefixSubId; // default prefix sub-identifier for VarBinds

    while ((status = RegEnumValue(hkRegResult, iValue, parmName, &nameSize, 0,
                &dwType, (LPBYTE)&parm, &parmSize)) != ERROR_NO_MORE_ITEMS)
    {                                     // read until no more values
        if (status != ERROR_SUCCESS)          // if error during read
        {
            WriteTrace(0x14,"Read_Registry_Parameters: Error reading registry value is %lu for index %lu (Parameters)\n",
                                status, iValue);            // show error information
            WriteLog(SNMPELEA_ERROR_REGISTRY_PARAMETER_ENUMERATE,
                                (DWORD) status, iValue);   // log error message

            fRegOk = FALSE;                                         // don't want to do notify now
            CloseRegNotify();                                       // close event handle
            CloseRegParmKey();                                      // close registry key
            return(FALSE);                                          // indicate stop
        }

        WriteTrace(0x00,"Read_Registry_Parameters: Parameter read is %s, length is %lu\n",
                        parmName, strlen(parmName));

        switch (dwType)
        {
            case REG_SZ : // if we have a string
            {
                WriteTrace(0x00,"Read_Registry_Parameters: Parameter type is REG_SZ\n");

                if ( _stricmp(parmName,EXTENSION_BASE_OID) == 0 )
                {
                    WriteTrace(0x00,"Read_Registry_Parameters: BaseEnterpriseOID parameter matched\n");
                    strncpy(szBaseOID,parm,MAX_PATH);         // save base OID
                    fBaseOID = TRUE;                        // indicate parameter read
                }
                else if ( _stricmp(parmName,EXTENSION_TRACE_FILE) == 0 )
                {
                    WriteTrace(0x00,"Read_Registry_Parameters: TraceFileName parameter matched\n");
                    strncpy(szTraceFileName,parm,MAX_PATH);   // save filename
                    fTraceFileName = TRUE;                  // indicate parameter read
                }
                else if ( _stricmp(parmName,EXTENSION_SUPPORTED_VIEW) == 0 )
                {
                    WriteTrace(0x00,"Read_Registry_Parameters: SupportedView parameter matched\n");
                    strncpy(szSupView,parm,MAX_PATH);         // save supported view OID
                    fSupView = TRUE;                        // indicate parameter read
                }
                else                          // otherwise, bad value read
                {
                    WriteTrace(0x00,"Read_Registry_Parameters: Unknown Registry value name: %s\n",parmName );
                    WriteTrace(0x00,"Read_Registry_Parameters: Unknown Registry value contents %s\n",parm );
                }
            }
            break;

            case REG_DWORD :// if double word parameter
            {
                WriteTrace(0x00,"Read_Registry_Parameters: Parameter type is REG_DWORD\n");

                if ( _stricmp(parmName,EXTENSION_TRACE_LEVEL) == 0 )
                {
                    WriteTrace(0x00,"Read_Registry_Parameters: TraceLevel parameter matched\n");
                    nTraceLevel = *((DWORD *)parm); // copy registry trace level
                    fTraceLevelFlg = TRUE;                  //indicate parameter read
                    break;
                }
                else if ( _stricmp(parmName,EXTENSION_TRIM) == 0 )
                {
                    WriteTrace(0x00,"Read_Registry_Parameters: Global TrimMessage parameter matched\n");
                    fGlobalTrim = (*((DWORD *)parm) == 1);          // set global message trim flag
                    fTrimMsg = TRUE;         // show parameter found
                    break;                                  // exit case
                }
                else if ( _stricmp(parmName,EXTENSION_MAX_TRAP_SIZE) == 0 )
                {
                    WriteTrace(0x00,"Read_Registry_Parameters: Maximum Trap Size parameter matched\n");
                    nMaxTrapSize = *((DWORD *)parm);                // get trap size
                    fTrapSize = TRUE;                               // show parameter found
                    break;
                }
                else if ( _stricmp(parmName,EXTENSION_TRIM_FLAG) == 0)
                {
                    WriteTrace(0x00,"Read_Registry_Parameters: Global trap trimming flag TrimFlag parameter matched\n");
                    fTrimFlag = (*((DWORD *)parm) == 1);    // set global trim flag
                    fTrimFlg = TRUE;                // show parameter found
                    break;                                  // exit case
                }
                else if ( _stricmp(parmName,EXTENSION_THRESHOLD_ENABLED) == 0)
                {
                    WriteTrace(0x00,"Read_Registry_Parameters: Global threshold checking flag ThresholdEnabled parameter matched\n");
                    fThresholdEnabled = (*((DWORD *)parm) == 1);    // set global threshold enabled flag
                    fThresholdEnabledFlg = TRUE;
                    break;                                  // exit case
                }
                else if ( _stricmp(parmName,EXTENSION_THRESHOLD_FLAG) == 0)
                {
                    WriteTrace(0x00,"Read_Registry_Parameters: Global preformance threshold flag Threshold parameter matched\n");
                    fThreshold = (*((DWORD *)parm) == 1);   // set global performance threshold flag
                    fThresholdFlg = TRUE;
                    break;                                  // exit case
                }
                else if ( _stricmp(parmName,EXTENSION_THRESHOLD_COUNT) == 0)
                {
                    WriteTrace(0x00,"Read_Registry_Parameters: Global preformance threshold count ThresholdCount parameter matched\n");
                    dwThresholdCount = *((DWORD *)parm);    // set global performance threshold count
                    fThresholdCountFlg = TRUE;
                    break;                                  // exit case
                }
                else if ( _stricmp(parmName,EXTENSION_THRESHOLD_TIME) == 0)
                {
                    WriteTrace(0x00,"Read_Registry_Parameters: Global preformance threshold time ThresholdTime parameter matched\n");
                    dwThresholdTime = *((DWORD *)parm);     // set global performance threshold time
                    fThresholdTimeFlg = TRUE;
                    break;                                  // exit case
                }
                else if (fDoLogonEvents && (_stricmp(parmName,EXTENSION_LASTBOOT_TIME) == 0))
                {
                    WriteTrace(0x00,"Read_Registry_Parameters: Initialization last boot time parameter matched\n");
                    dwLastBootTime = *((DWORD *)parm);  // set global last boot time
                    fLastBootFlg = TRUE;
                    break;                                  // exit case
                }
                else if ( _stricmp(parmName,EXTENSION_EVENT_LOG_POLL_TIME) == 0)
                {
                    WriteTrace(0x00,"Read_Registry_Parameters: Global Event Log poll time EventLogPollTime parameter matched\n");
                    DWORD dwEventLogPollTime = *((DWORD *)parm);     // registry poll time in seconds

                    WriteTrace(0x00,"Read_Registry_Parameters: EventLogPollTime parameter found in registry of %lu.\n",
                                    dwEventLogPollTime);
                    
                    // check for underflow and overflow values
                    if ( (dwEventLogPollTime != 0) && (dwEventLogPollTime <= ((DWORD)INFINITE/1000)) )
                    {
                        g_dwEventLogPollTime = dwEventLogPollTime * 1000; // poll time in milliseconds
                    }
                    else
                    {
                        WriteTrace(0x00,"Read_Registry_Parameters: reset EventLogPollTime parameter to %lu.\n",
                                    g_dwEventLogPollTime);
                    }
                    break;       
                }
                else if ( _stricmp(parmName,EXTENSION_VARBIND_PREFIX_SUB_ID) == 0)
                {
                    WriteTrace(0x00,"Read_Registry_Parameters: Global VarBindPrefixSubId parameter matched\n");
                    DWORD dwVarBindPrefixSubId = *((DWORD *)parm);

                    WriteTrace(0x00,"Read_Registry_Parameters: VarBindPrefixSubId parameter found in registry of %lu.\n",
                                    dwVarBindPrefixSubId);
                    
                    // 2147483647 (INT_MAX) is the largest sub-identifier for  
                    // 32-bit signed representation.
                    // 0 is an invalid sub-identifier, but we'll consider it as
                    // "no VarBind prefix sub-identifier is appended to the 
                    // BaseEnterpriseOID to form VarBind OID in a trap"
                    if ( dwVarBindPrefixSubId <= (DWORD)INT_MAX )
                    {
                        g_dwVarBindPrefixSubId = dwVarBindPrefixSubId;
                    }
                    else
                    {
                        WriteTrace(0x00,"Read_Registry_Parameters: reset VarBindPrefixSubId parameter to %lu.\n",
                                    g_dwVarBindPrefixSubId);
                    }
                    break;       
                }
                else                                                    // otherwise, bad parameter read
                {
                    WriteTrace(0x00,"Read_Registry_Parameters: Unknown Registry value name: %s\n",parmName );
                    WriteTrace(0x00,"Read_Registry_Parameters: Unknown Registry value contents: %lu\n",parm );
                }
            }
            break;

            default :   // if not above, bad value read
            {
                WriteTrace(0x00,"Read_Registry_Parameters: Unknown Registry value name: %s\n",parmName );
                WriteTrace(0x00,"Read_Registry_Parameters: Unknown Registry value contents not displayed\n" );
            }
        } // end switch

        nameSize = MAX_PATH;                  // reset maximum length, size in TCHARs
        parmSize = sizeof(parm) - sizeof(TCHAR); // reset maximum length, size in bytes
        iValue++;                             // request next parameter value

    } // end while

    if (!fRegOk)
    {
        CloseRegParmKey();                                      // close registry key
    }

    WriteTrace(0x00,"Read_Registry_Parameters: Checking BaseEnterpriseOID read from registry\n");
    
    if ( !fBaseOID )
    {
        WriteTrace(0x14,"Read_Registry_Parameters: BaseEnterpriseOID parameter not found in registry\n");
        WriteLog(SNMPELEA_NO_REGISTRY_BASEOID_PARAMETER);
        return(FALSE);                                          // exit - can't continue
    }

    WriteTrace(0x00,"Read_Registry_Parameters: Checking SupportedView read from registry\n");
    
    if ( !fSupView )
    {
        WriteTrace(0x14,"Read_Registry_Parameters: SupportedView parameter not found in registry\n");
        WriteLog(SNMPELEA_NO_REGISTRY_SUPVIEW_PARAMETER);
        return(FALSE);                                          // exit - can't continue
    }

    WriteTrace(0x00,"Read_Registry_Parameters: Checking TraceFileName read from registry\n");
    
    if ( !fTraceFileName )
    {
        WriteTrace(0x00,"Read_Registry_Parameters: TraceFileName parameter not found in registry, defaulting to %s.\n",
                        szTraceFileName);
    }
    else
    {
        WriteTrace(0x00,"Read_Registry_Parameters: TraceFileName parameter found in registry of %s.\n", szTraceFileName);
    }

    WriteTrace(0x00,"Read_Registry_Parameters: Checking TraceLevel read from registry\n");
    
    if ( !fTraceLevelFlg )
    {
        WriteTrace(0x00,"Read_Registry_Parameters: TraceLevel parameter not found in registry, defaulting to %lu.\n",
                        nTraceLevel);
    }
    else
    {
        WriteTrace(0x00,"Read_Registry_Parameters: TraceLevel parameter found in registry of %lu.\n", nTraceLevel);
    }

    WriteTrace(0x00,"Read_Registry_Parameters: Checking MaxTrapSize read from registry\n");
    
    if ( !fTrapSize )
    {
        WriteTrace(0x00,"Read_Registry_Parameters: MaxTrapSize parameter not found in registry, defaulting to %lu.\n",
                        MAX_TRAP_SIZE);
        nMaxTrapSize = MAX_TRAP_SIZE;
    }
    else
    {
        WriteTrace(0x00,"Read_Registry_Parameters: MaxTrapSize parameter found in registry of %lu.\n", nMaxTrapSize);
    }

    WriteTrace(0x00,"Read_Registry_Parameters: Checking TrimFlag read from registry\n");

    if ( !fTrimFlg )
    {
        WriteTrace(0x00,"Read_Registry_Parameters: TrimFlag parameter not found in registry, defaulting to %lu.\n",
                        fTrimFlag);
    }
    else
    {
        WriteTrace(0x00,"Read_Registry_Parameters: TrimFlag parameter found in registry of %lu.\n", fTrimFlag);
    }

    WriteTrace(0x00,"Read_Registry_Parameters: Checking TrimFlag read from registry\n");

    if ( !fTrimMsg )
    {
        WriteTrace(0x00,"Read_Registry_Parameters: TrimMessage parameter not found in registry, defaulting to %lu.\n",
                        fGlobalTrim);
    }
    else
    {
        WriteTrace(0x00,"Read_Registry_Parameters: TrimMessage parameter found in registry of %lu.\n", fGlobalTrim);
    }


    WriteTrace(0x00,"Read_Registry_Parameters: Checking ThresholdEnabled parameter read from registry\n");

    if ( !fThresholdEnabledFlg )
    {
        WriteTrace(0x00,"Read_Registry_Parameters: ThresholdEnabled parameter not found in registry, defaulting to 1.\n");
                fThresholdEnabled = TRUE;
    }
    else
    {
        WriteTrace(0x00,"Read_Registry_Parameters: ThresholdEnabled parameter found in registry of %lu.\n", fThresholdEnabled);
    }

    WriteTrace(0x00,"Read_Registry_Parameters: Checking Threshold parameter read from registry\n");
    
    if ( !fThresholdFlg )
    {
        WriteTrace(0x00,"Read_Registry_Parameters: Threshold parameter not found in registry, defaulting to 0.\n");
        fThreshold = FALSE;
    }
    else
    {
        WriteTrace(0x00,"Read_Registry_Parameters: Threshold parameter found in registry of %lu.\n", fThreshold);
    }

    WriteTrace(0x00,"Read_Registry_Parameters: Checking ThresholdCount parameter read from registry\n");
    
    if ( !fThresholdCountFlg )
    {
        WriteTrace(0x00,"Read_Registry_Parameters: ThresholdCount parameter not found in registry, defaulting to %lu.\n",
                        THRESHOLD_COUNT);
        dwThresholdCount = THRESHOLD_COUNT;
    }
    else
    {
        WriteTrace(0x00,"Read_Registry_Parameters: ThresholdCount parameter found in registry of %lu.\n",
                dwThresholdCount);

        if (dwThresholdCount < 2)
        {
            WriteTrace(0x00,"Read_Registry_Parameters: ThresholdCount is an invalid value -- a minimum of 2 is used.\n");
            dwThresholdCount = 2;
            WriteLog(SNMPELEA_REGISTRY_LOW_THRESHOLDCOUNT_PARAMETER, dwThresholdCount);
        }
    }

    WriteTrace(0x00,"Read_Registry_Parameters: Checking ThresholdTime parameter read from registry\n");
    
    if ( !fThresholdTimeFlg )
    {
        WriteTrace(0x00,"Read_Registry_Parameters: ThresholdTime parameter not found in registry, defaulting to %lu.\n",
                        THRESHOLD_TIME);
        dwThresholdTime = THRESHOLD_TIME;
    }
    else
    {
        WriteTrace(0x00,"Read_Registry_Parameters: ThresholdTime parameter found in registry of %lu.\n",
                dwThresholdTime);
        if (dwThresholdTime < 1)
        {
            WriteTrace(0x00,"Read_Registry_Parameters: ThresholdTime is an invalid value -- a minimum of 1 is used.\n");
            dwThresholdTime = 1;
            WriteLog(SNMPELEA_REGISTRY_LOW_THRESHOLDTIME_PARAMETER, dwThresholdTime);
        }
    }

    if ( (fThresholdEnabled && !fThreshold && fThresholdOff) ||
            (!fThresholdEnabled && fThresholdOff) )
    {
        WriteTrace(0x0a,"Read_Registry_Parameters: Threshold values have been reset. Trap processing resumed.\n");
        WriteLog(SNMPELEA_THRESHOLD_RESUMED);

        if (fLogInit)
        {
            for (DWORD inum = 0; inum < uNumEventLogs; inum++)
            {
                Position_to_Log_End(phEventLogs[inum]);
            }
        }
    }

    if ( fThresholdEnabled && fThreshold && !fThresholdOff )
    {
        WriteTrace(0x0a,"Read_Registry_Parameters: Threshold values have been set. Trap processing will not be done.\n");
        WriteLog(SNMPELEA_THRESHOLD_SET);
    }

    WriteTrace(0x00,"Read_Registry_Parameters: BaseEnterpriseOID is %s\n", szBaseOID);
    WriteTrace(0x00,"Read_Registry_Parameters: SupportedView is %s\n", szSupView);
    WriteTrace(0x00,"Read_Registry_Parameters: Global TrimFlag value is %lu (trim yes/no)\n", fTrimFlag);
    WriteTrace(0x00,"Read_Registry_Parameters: Global TrimMessage value is %lu (trim msg/ins str first)\n", fGlobalTrim);

    if (fLogInit)
    {
        WriteTrace(0x00,"Read_Registry_Parameters: Reread of registry parameters is complete\n");
        WriteTrace(0x0a,"Read_Registry_Parameters: Exiting Read_Registry_Parameters with TRUE\n");
        return(TRUE);
    }

    fLogInit = TRUE;                                // indicate not to read log information again

    WriteTrace(0x0a,"Read_Registry_Parameters: Opening %s\n", EVENTLOG_BASE);

    if ((status = RegOpenKeyEx(HKEY_LOCAL_MACHINE, EVENTLOG_BASE, 0,
         (KEY_QUERY_VALUE | KEY_ENUMERATE_SUB_KEYS), &hkResult))
         != ERROR_SUCCESS)                   // open for log names
    {
        WriteTrace(0x14,"Read_Registry_Parameters: Error in RegOpenKeyEx for EventLog = %lu\n",
                        status);
        WriteLog(SNMPELEA_NO_REGISTRY_LOG_NAME, status); // log error message
        return(FALSE);                        // if error, service stop
    }

    iValue = 0;             // read first parameter
    parmSize = MAX_PATH+1;  // maximum parameter size in TCHARs for RegEnumKey

    while ((status = RegEnumKey(hkResult, iValue, (char *) &parm, parmSize)) != ERROR_NO_MORE_ITEMS)
    {                                     // read until no more entries
        if (status != ERROR_SUCCESS)          // if error during read
        {
            WriteTrace(0x14,"Read_Registry_Parameters: Error reading registry value is %lu for index %lu (EventLogFiles)\n",
                                status, iValue);            // show error information
            WriteLog(SNMPELEA_ERROR_REGISTRY_LOG_NAME_ENUMERATE, status, iValue);  // log the error message
            RegCloseKey(hkResult);             // close registry
            return(FALSE);                     // indicate service stop
        }

        // MikeCure 4/3/98 hotfix for SMS Bug1 #20521
        //===========================================
        EnablePrivilege();

        hLogFile = OpenEventLog( (LPTSTR) NULL, parm);

        if (hLogFile == NULL)
        {                         // did log file open?
            lastError = GetLastError(); // save error code
            WriteTrace(0x14,"Read_Registry_Parameters: Error in EventLogOpen = %lu\n",
                        lastError);
            WriteTrace(0x14,"Read_Registry_Parameters: Log file name: %s\n",parm);

            WriteLog(SNMPELEA_ERROR_OPEN_EVENT_LOG, parm, lastError);  // log the error message
            continue;                // failed -- forget this one
        }

        if ( !Position_to_Log_End(hLogFile) )
        {
            WriteTrace(0x14,"Read_Registry_Parameters: Unable to position to end of log. DLL terminated.\n");
            WriteLog(SNMPELEA_ERROR_LOG_END);
            WriteLog(SNMPELEA_ABNORMAL_INITIALIZATION);
            RegCloseKey(hkResult);      // close registry
            CloseEventLog(hLogFile);    // close handle to event log
            return(FALSE);              // exit with error
        }

        phTmpEventLogs = (PHANDLE) SNMP_realloc( (LPVOID) phEventLogs,
                                    (uNumEventLogs+1) * sizeof(HANDLE));
                                                                 // reallocate array space
        if (phTmpEventLogs == (PHANDLE) NULL)
        {
            WriteTrace(0x14,"Read_Registry_Parameters: Unable to reallocate log event array\n");
            WriteLog(SNMPELEA_REALLOC_LOG_EVENT_ARRAY);
            RegCloseKey(hkResult);      // close registry
            CloseEventLog(hLogFile);    // close handle to event log
            return(FALSE);              // exit with error
        }

        phEventLogs = phTmpEventLogs;   // transfer value

        WriteTrace(0x00,"Read_Registry_Parameters: Event log array reallocated at %08X\n",
                phEventLogs);

        *(phEventLogs+uNumEventLogs) = hLogFile; // save handle
                               

        lpszTmpEventLogs = (LPTSTR) SNMP_realloc( (LPVOID) lpszEventLogs,
                                                iLogNameSize + MAX_PATH + 1 );

        if (lpszTmpEventLogs == (LPTSTR) NULL)
        {
            WriteTrace(0x14,"Read_Registry_Parameters: Unable to reallocate log name array\n");
            WriteLog(SNMPELEA_REALLOC_LOG_NAME_ARRAY);
            RegCloseKey(hkResult);      // close registry
            uNumEventLogs++;            // account for saved handle
            return(FALSE);              // exit with error
        }

        lpszEventLogs = lpszTmpEventLogs;   // transfer value

        WriteTrace(0x00,"Read_Registry_Parameters: Event log name array reallocated at %p\n",
                lpszEventLogs);

        iLogNameSize += MAX_PATH + 1;
        lpszEventLogs[iLogNameSize-1] = 0; // null terminated the last TCHAR
        strncpy(lpszEventLogs+uNumEventLogs*(MAX_PATH+1), parm, MAX_PATH);

        phTmpPrimHandles = (PHMODULE) SNMP_realloc( (LPVOID) phPrimHandles,
                                        (uNumEventLogs+1) * sizeof(HANDLE));
                                                                 // reallocate array space
        if (phTmpPrimHandles == (PHMODULE) NULL)
        {
            WriteTrace(0x14,"Read_Registry_Parameters: Unable to reallocate PrimaryModule handle array\n");
            WriteLog(SNMPELEA_REALLOC_PRIM_HANDLE_ARRAY);
            RegCloseKey(hkResult);      // close registry
            uNumEventLogs++;            // account for saved handle
            return(FALSE);              // exit with error
        }

        phPrimHandles = phTmpPrimHandles;   // transfer value

        WriteTrace(0x00,"Read_Registry_Parameters: PrimaryModule handle array reallocated at %08X\n",
                phPrimHandles);


        strncpy(lpszLog, EVENTLOG_BASE, MAX_PATH);  // copy base registry name
        WRAP_STRCAT_A(lpszLog, parm, MAX_PATH);     // add on the log file name read

        WriteTrace(0x0a,"Read_Registry_Parameters: Opening registry for PrimaryModule for %s\n", lpszLog);
        // e.g. open HKLM\SYSTEM\CCS\Services\EventLog\Applicaiton subkey
        if ( (status = RegOpenKeyEx(            // open the registry to read the name
                HKEY_LOCAL_MACHINE,             // of the message module DLL
                lpszLog,                        // registry key to open
                0,
                KEY_READ,
                &hkResult2) ) != ERROR_SUCCESS)
        {
            WriteTrace(0x14,"Read_Registry_Parameters: Unable to open EventLog service registry key %s; RegOpenKeyEx returned %lu\n",
                        lpszLog, status);                       // write trace event record
            WriteLog(SNMPELEA_CANT_OPEN_REGISTRY_PARM_DLL, lpszLog, status);
            WriteTrace(0x0a,"Read_Registry_Parameters: Exiting Read_Registry_Parameters with FALSE\n");
            RegCloseKey(hkResult);              // close registry
            uNumEventLogs++;                    // account for saved handle
            return(FALSE);                      // return
        }

        parmSize = sizeof(parm) - sizeof(TCHAR);// maximum parameter size in bytes for RegQueryValueEx
        if ( (status = RegQueryValueEx(         // look up module name
                hkResult2,                      // handle to registry key
                EXTENSION_PRIM_MODULE,          // key to look up
                0,                              // ignored
                &dwType,                        // address to return type value
                (LPBYTE) parm,                  // where to return message module name
                &parmSize) ) != ERROR_SUCCESS)  // size of message module name field
        {
            WriteTrace(0x14,"Read_Registry_Parameters: No PrimaryModule registry key for %s; RegQueryValueEx returned %lu\n",
                    lpszEventLogs+uNumEventLogs*(MAX_PATH+1), status);                      // write trace event record
            *(phPrimHandles+uNumEventLogs) = (HMODULE) NULL;
        }
        else
        {
            DWORD retCode;
            tPrimaryModuleParms PMParams;

            PMParams.dwParams = PMP_PARAMMSGFILE;
            retCode = LoadPrimaryModuleParams(hkResult2, parm, PMParams);
            if (retCode != ERROR_SUCCESS)
            {
                WriteTrace(0x14, "Read_Registry_Parameters: LoadPrimaryModuleParams failed with errCode = %lu\n", retCode);
                *(phPrimHandles+uNumEventLogs) = NULL;
            }
            else
                *(phPrimHandles+uNumEventLogs) = PMParams.hModule;
        }

        RegCloseKey(hkResult2);                         // close registry key

        WriteTrace(0x00,"Read_Registry_Parameters: Log file name is %s\n",
                lpszEventLogs+uNumEventLogs*(MAX_PATH+1));
        WriteTrace(0x00,"Read_Registry_Parameters: Log handle #%lu is %08X\n",
                uNumEventLogs,hLogFile);
        WriteTrace(0x00,"Read_Registry_Parameters: PrimaryModule handle #%lu is %08X\n",
                uNumEventLogs,*(phPrimHandles+uNumEventLogs));

        uNumEventLogs++;  
        parmSize = MAX_PATH+1;                // reset to maximum in TCHARs for RegEnumKey
        iValue++;                             // read next parameter

    } // end while

    RegCloseKey(hkResult);                   // close registry info

    WriteTrace(0x00,"Read_Registry_Parameters: Number of handles acquired is %lu\n",
                uNumEventLogs);
    for (uVal = 0; uVal < uNumEventLogs; uVal++)
    {
        WriteTrace(0x00,"Read_Registry_Parameters: Handle # %lu\t%08X\t%s\n", uVal,
                *(phEventLogs+uVal), lpszEventLogs+uVal*(MAX_PATH+1));
    }

    if (uNumEventLogs)                       // if we have logs opened
    {
        return(TRUE);                        // then we can say all okay
    }
    else
    {
        WriteTrace(0x14,"Read_Registry_Parameters: Registry contains no log file entries to process\n");
        //WriteLog(SNMPELEA_NO_REGISTRY_EVENT_LOGS);     // log error message
        return(FALSE);                      // if not, then not okay
    }

Error:

    WriteTrace(0x14,"Read_Registry_Parameters: Unable to do strcat to %s.\n",
               lpszLog);                       // write trace event record
    WriteTrace(0x0a,"Read_Registry_Parameters: Exiting Read_Registry_Parameters with FALSE\n");
    RegCloseKey(hkResult);              // close registry
    uNumEventLogs++;                    // account for saved handle
    return(FALSE);                      
}                                       // request stop
}

//nadir
VOID
CloseSourceHandles(VOID)
{
   PSourceHandleList    lpSource;
   UINT lastError;

   lpSource = lpSourceHandleList;

   while (lpSource != (PSourceHandleList)NULL)
   {
      if ( !FreeLibrary(lpSource->handle) )                     // free msg dll
      {
         lastError = GetLastError();                            // get error code
         WriteTrace(0x14,"CloseSourceHandles: Error freeing message dll is %lu.\n", lastError);
         WriteLog(SNMPELEA_ERROR_FREEING_MSG_DLL, lastError);
      }

      lpSourceHandleList = lpSource->Next;
      SNMP_free(lpSource);
      lpSource = lpSourceHandleList;
   }
}



//nadir




extern "C" {
BOOL
APIENTRY
DllMain(
        IN      HINSTANCE   hDll,
        IN      DWORD       dwReason,
        IN      LPVOID      lpReserved
        )

/*++

Routine Description:

        SNMPEventLogDllMain is the dll initialization and termination routine.

        Once this termination request is received, the appropriate events will be
        signaled, notifying the subordinate threads that they should terminate
        to accomodate service termination.

Arguments:

        hDll            -       Handle to the DLL. Unreferenced.

        dwReason        -       Reason this routine was entered (process/thread attach/detach).

        lpReserved      -       Reserved. Unreferenced.

Return Value:

        TRUE    -       If initialization or termination was successful.

        FALSE   -       If initialization or termination was unsuccessful.

--*/

{
    DWORD       lastError;                              // to save GetLastError() return code

    UNREFERENCED_PARAMETER(hDll);
    UNREFERENCED_PARAMETER(lpReserved);

    WriteTrace(0x0a,"SNMPEventLogDllMain: Entering SNMPEventLogDllMain routine.....\n");


    switch(dwReason)
    {
        case DLL_PROCESS_ATTACH:
            WriteTrace(0x0a,"SNMPEventLogDllMain: Reason code indicates process attach\n");
            DisableThreadLibraryCalls( hDll );

            break;

        case DLL_PROCESS_DETACH:
                WriteTrace(0x0a,"SNMPEventLogDllMain: Reason code indicates process detach\n");
                break;

        case DLL_THREAD_ATTACH:
                WriteTrace(0x0a,"SNMPEventLogDllMain: Reason code indicates thread attach\n");
                break;

        case DLL_THREAD_DETACH:
                WriteTrace(0x0a,"SNMPEventLogDllMain: Reason code indicates thread detach\n");
                break;

        default:
                WriteTrace(0x0a,"SNMPEventLogDllMain: Unknown reason code indicated in SNMPEventLogDllMain\n");
                break;

    } // end switch()

    WriteTrace(0x0a,"SNMPEventLogDllMain: Exiting SNMPEventLogDllMain routine with TRUE\n");
    return(TRUE);
}
}


extern "C" {
BOOL
APIENTRY
SnmpExtensionInit(
        IN      DWORD                           dwTimeZeroReference,
        OUT     HANDLE                          *hPollForTrapEvent,
        OUT     AsnObjectIdentifier     *supportedView
        )

/*++

Routine Description:

        SnmpExtensionInit is the extension dll initialization routine.

        This routine will create the event used to notify the manager agent that an event
        has occurred and that a trap should be generated. The TimeZeroReference will be
        saved and will be used by the trap generation routine to insert the time reference
        into the generated trap.

        The registry will be queried to determine which event logs will be used for tracking.
        These event log names are validated to insure that they are real log names. Event logs
        are opened and their handles are saved for event log processing.

        An event is created to notify the log processing thread of DLL termination. Then the
        log processing thread is spawned to handle all further event processing.

        The registry is then read to get the value for the supported view for this extension
        agent DLL. The registry layout for this routine is as follows:

        Registry
                Machine
                        SOFTWARE
                                Microsoft
                                        SNMP_EVENTS
                                                EventLog
                                                        Parameters
                                                                TraceFileName       (REG_SZ)
                                                                TraceLevel          (REG_DWORD)
                                                                BaseEnterpriseOID   (REG_SZ)
                                                                SupportedView       (REG_SZ)
                                                                TrimMessage         (REG_DWORD)
                                                                MaxTrapSize         (REG_DWORD)
                                                                TrimFlag            (REG_DWORD)
                                                                ThresholdEnabled    (REG_DWORD)
                                                                ThresholdFlag       (REG_DWORD)
                                                                ThresholdCount      (REG_DWORD)
                                                                ThresholdTime       (REG_DWORD)

Arguments:

        dwTimeZeroReference     -       Specifies a time-zero reference for the extension agent.

        hPollForTrapEvent       -       Pointer to an event handle for an event that will be asserted
                                                        when the SnmpExtensionTrap entry point should be polled by the
                                                        manager agent.

        supportedView           -       Points to an AsnObjectIdentifier specifying the MIB sub-tree
                                                        supported by this extension agent. Read from the registry.

Return Value:

        TRUE    -       If initialization or termination was successful.

        FALSE   -       If initialization or termination was unsuccessful.

--*/

{
    LONG    lastError;                      // for GetLastError()
    DWORD   dwThreadID;                     // for CreateThread()

    WriteTrace(0x0a,"SnmpExtensionInit: Entering extension agent SnmpExtensionInit routine\n");
    
    WriteTrace(0x14,"SnmpExtensionInit: SNMP Event Log Extension Agent DLL is starting\n");
    WriteLog(SNMPELEA_STARTED);

    ResetGlobals();

    if ( (hWriteEvent = RegisterEventSource(
                                            (LPTSTR) NULL,
                                            EVNTAGNT_NAME) )
                                                            == NULL)
    {
        WriteTrace(0x20,"SnmpExtensionInit: Unable to log application events; code is %lu\n",
                        GetLastError() );
        WriteTrace(0x20,"SnmpExtensionInit: SNMP Event Log Extension Agent DLL initialization abnormal termination\n");
        WriteTrace(0x0a,"SnmpExtensionInit: Exiting SnmpExtensionInit routine with FALSE\n");
        return(FALSE);                  // error initializing
    }

    WriteTrace(0x0a,"SnmpExtensionInit: Creating event for extension DLL shutdown\n");    
    if ( (hStopAll= CreateEvent(
            (LPSECURITY_ATTRIBUTES) NULL,
            TRUE, // changed to manual reset event
            FALSE,
            (LPTSTR) NULL)) == NULL)
    {
        lastError = GetLastError(); // save error status
        WriteTrace(0x14,"SnmpExtensionInit: Error creating stop extension DLL event; code %lu\n",
                        lastError);
        WriteLog(SNMPELEA_ERROR_CREATING_STOP_AGENT_EVENT, lastError);
            
        WriteTrace(0x14,"SnmpExtensionInit: SNMPELEA DLL abnormal initialization\n");
        WriteLog(SNMPELEA_ABNORMAL_INITIALIZATION);      // log error message
        WriteTrace(0x0a,"SnmpExtensionInit: Exiting SnmpExtensionInit routine with FALSE\n");

        goto Error;
    }    
    WriteTrace(0x00,"SnmpExtensionInit: Extension DLL shutdown event handle is %08X\n",
                    hStopAll);
        
    if ( !Read_Registry_Parameters() )
    {
        WriteTrace(0x14,"SnmpExtensionInit: Error during registry initialization processing\n");
        WriteLog(SNMPELEA_REGISTRY_INIT_ERROR);

        WriteTrace(0x14,"SnmpExtensionInit: SNMP Event Log Extension Agent DLL abnormal initialization\n");
        WriteLog(SNMPELEA_ABNORMAL_INITIALIZATION);

        WriteTrace(0x0a,"SnmpExtensionInit: Exiting extension agent SnmpExtensionInit routine with FALSE\n");
        goto Error;                                  // exit init routine
    }


    WriteTrace(0x0a,"SnmpExtensionInit: Creating event for manager agent trap event notification\n");

    if ( (hEventNotify = CreateEvent(
        (LPSECURITY_ATTRIBUTES) NULL,
        FALSE,
        FALSE,
        (LPTSTR) NULL)) == NULL)
    {
        lastError = GetLastError(); // save error status
        WriteTrace(0x14,"SnmpExtensionInit: Error creating EventNotify event; code %lu\n",
            lastError);
        WriteLog(SNMPELEA_ERROR_CREATING_EVENT_NOTIFY_EVENT, lastError);

        WriteTrace(0x14,"SnmpExtensionInit: SNMP Event Log Extension Agent DLL abnormal initialization\n");
        WriteLog(SNMPELEA_ABNORMAL_INITIALIZATION);      // log error message
        WriteTrace(0x0a,"SnmpExtensionInit: Exiting extension agent SnmpExtensionInit routine with FALSE\n");
        goto Error;
    }

    WriteTrace(0x00,"SnmpExtensionInit: Manager agent trap event notification handle is %08X\n",
                hEventNotify);

    WriteTrace(0x0a,"SnmpExtensionInit: Creating thread for event log processing routine\n");

    if ( (hServThrd = CreateThread(
        (LPSECURITY_ATTRIBUTES) NULL,           // security attributes
        0,                                      // initial thread stack size
        (LPTHREAD_START_ROUTINE) SnmpEvLogProc, // starting address of thread
        0,                                      // no arguments
        0,                                      // creation flags
        &dwThreadID) ) == NULL )                // returned thread id
    {
        lastError = GetLastError();                     // save error status
        WriteTrace(0x14,"SnmpExtensionInit: Error creating event log processing thread; code %lu\n",
            lastError);
        WriteLog(SNMPELEA_ERROR_CREATING_LOG_THREAD, lastError);    // log error message

        WriteTrace(0x14,"SnmpExtensionInit: SNMP Event Log Extension Agent DLL abnormal initialization\n");
        WriteLog(SNMPELEA_ABNORMAL_INITIALIZATION);      // log error message
        WriteTrace(0x0a,"SnmpExtensionInit: Exiting extension agent SnmpExtensionInit routine with FALSE\n");
        goto Error;
    }

    WriteTrace(0x00,"SnmpExtensionInit: Handle to event log processing routine thread is %08X\n",
                hServThrd);

    if (!StrToOid(szSupView, supportedView))
    {
        WriteTrace(0x14,"SnmpExtensionInit: Unable to convert supported view string to OID\n");
        WriteLog(SNMPELEA_SUPVIEW_CONVERT_ERROR);

        WriteTrace(0x14,"SnmpExtensionInit: SNMP Event Log Extension Agent DLL abnormal initialization\n");
        WriteLog(SNMPELEA_ABNORMAL_INITIALIZATION);      // log error message
        WriteTrace(0x0a,"SnmpExtensionInit: Exiting extension agent SnmpExtensionInit routine with FALSE\n");

        goto Error;
    }

    dwTimeZero = dwTimeZeroReference;                       // save time zero reference
    *hPollForTrapEvent = hEventNotify;                      // return handle to event

    WriteTrace(0x0a,"SnmpExtensionInit: Exiting extension agent SnmpExtensionInit routine with TRUE\n");
    return(TRUE);

Error:
    SnmpExtensionClose();

    return(FALSE);
}
}

extern "C" {
VOID
APIENTRY
SnmpExtensionClose()
{
    DWORD   lastError;      // to save GetLastError() return code
    DWORD   dwThreadID;
    DWORD   dwWaitResult;
    BOOL    Itworked;

    WriteTrace(0x0a,"SnmpExtensionClose: Entering extension agent SnmpExtensionClose routine.\n");
    if (hStopAll && !SetEvent(hStopAll) )
    {
        lastError = GetLastError(); // save error status
        WriteTrace(0x14,"SnmpExtensionClose: Error setting dll termination event %08X in process detach; code %lu\n",
            hStopAll, lastError);
        WriteLog(SNMPELEA_ERROR_SET_AGENT_STOP_EVENT, HandleToUlong(hStopAll), lastError);  // log error message
    }
    else
    {
        WriteTrace(0x0a,"SnmpExtensionClose: Shutdown event %08X is now complete\n",
                                hStopAll);
    }

    if (hServThrd)
    {
        WriteTrace(0x0a,"SnmpExtensionClose: Waiting for event log processing thread %08X to terminate\n", hServThrd);
        WriteTrace(0x0a,"SnmpExtensionClose: Checking for thread exit code value\n");
        Itworked = GetExitCodeThread(hServThrd, &dwThreadID);
        WriteTrace(0x0a,"SnmpExtensionClose: Thread exit code value is %lu\n",dwThreadID);

        if (!Itworked || (dwThreadID == STILL_ACTIVE))
        {
            if (!Itworked)
            {
                lastError = GetLastError();
                WriteTrace(0x14,"SnmpExtensionClose: GetExitCodeThread returned FALSE, reason code %lu\n",
                        lastError);
                WriteLog(SNMPELEA_GET_EXIT_CODE_THREAD_FAILED, lastError);
            }
            else
            {
                WriteTrace(0x0a,"SnmpExtensionClose: Thread exit code indicates still active. Will wait...\n");
            }

            // wait for the child to end
            WriteTrace(0x0a,"SnmpExtensionClose: About to wait...\n");
            // bug# 277187: removal of TerminateThread by waiting until hServThrd has gone
            dwWaitResult = WaitForSingleObject(hServThrd, INFINITE);
            
            WriteTrace(0x0a,"SnmpExtensionClose: Finished wait...\n");

            switch (dwWaitResult)
            {
            case WAIT_FAILED:
                lastError = GetLastError(); // save error status
                WriteTrace(0x14,"SnmpExtensionClose: Error on WaitForSingleObject/log processing thread %08X; code %lu\n",
                        hServThrd, lastError);
                WriteLog(SNMPELEA_ERROR_WAIT_LOG_THREAD_STOP,
                        HandleToUlong(hServThrd), lastError);    // log error message
                break;
            case WAIT_OBJECT_0 :
                WriteTrace(0x0a,"SnmpExtensionClose: Event log processing thread %08X has terminated!\n",hServThrd);
                break;
            case WAIT_TIMEOUT :
                WriteTrace(0x14,"SnmpExtensionClose: Event log processing thread %08X has not terminated within 30 seconds; terminating thread\n",
                        hServThrd);
                WriteLog(SNMPELEA_LOG_THREAD_STOP_WAIT_30,
                        HandleToUlong(hServThrd));  // log error message
                break;
            default :
                WriteTrace(0x14,"SnmpExtensionClose: Unknown result from WaitForSingleObject waiting on log processing thread %08X termination is %lu\n",
                        hServThrd, dwWaitResult );
                WriteLog(SNMPELEA_WAIT_LOG_STOP_UNKNOWN_RETURN,
                        HandleToUlong(hServThrd), dwWaitResult);  // log error message
            }
        }

        WriteTrace(0x0a,"SnmpExtensionClose: Checking for thread exit code again\n");
        Itworked = GetExitCodeThread(hServThrd, &dwThreadID);
        WriteTrace(0x0a,"SnmpExtensionClose: Thread exit code value is %lu\n",dwThreadID);

        WriteTrace(0x0a,"SnmpExtensionClose: Closing handle to log processing thread %08X\n",
                hServThrd);
        if ( !CloseHandle(hServThrd) )
        {
            lastError = GetLastError(); // save error status
            WriteTrace(0x14,"SnmpExtensionClose: Error closing handle for log processing thread %08X; code %lu\n",
                    hServThrd, lastError);
            WriteLog(SNMPELEA_ERROR_CLOSING_STOP_LOG_THREAD_HANDLE,
                    HandleToUlong(hServThrd), lastError); // log error message
        }
        hServThrd = NULL;
    }

    CloseStopAll();                     // close event handle
    CloseEventNotify();                 // close event handle
    if (fRegOk)
    {
        CloseRegNotify();               // close event handle
        CloseRegParmKey();              // close registry key
        fRegOk = FALSE;
    }
    CloseLogs();                        // close all open log files
    CloseSourceHandles();

    WriteLog(SNMPELEA_STOPPED);
    if (hWriteEvent)
    {
        DeregisterEventSource(hWriteEvent); // no longer a need for logging
        hWriteEvent = NULL;
    }
    WriteTrace(0x14,"SnmpExtensionClose: SNMPELEA Event Log Extension Agent DLL has terminated\n");
}
}


BOOL
BuildThresholdTrap(
    IN  VOID
    )

/*++

Routine Description:

        This routine will build the threshold trap.


Arguments:

        None


Return Value:

        TRUE if created varbind, FALSE if an error occurred.

--*/

{
        TCHAR   szBuf[MAX_PATH+1];      // for OID conversion
        UINT    i;                      // counter
        BOOL    fOk;                    // used by WRAP_STRCAT_A macro

        WriteTrace(0x0a,"BuildThresholdTrap: Building static variable bindings for threshold trap\n");
        WriteTrace(0x00,"BuildThresholdTrap: &thresholdVarBind is at %08X\n", &thresholdVarBind);
        WriteTrace(0x00,"BuildThresholdTrap: thresholdVarBind is %08X\n", thresholdVarBind);

        WriteTrace(0x00,"BuildThresholdTrap: BaseEnterpriseOID value read is %s\n", szBaseOID);

        if ( !StrToOid((char *) &szBaseOID, &thresholdOID) )
        {
            WriteTrace(0x14,"BuildThresholdTrap: Unable to convert OID from BaseEnterpriseOID\n");
            WriteLog(SNMPELEA_CANT_CONVERT_ENTERPRISE_OID);
            return(FALSE);
        }

        szBuf[MAX_PATH] = 0;
        strncpy(szBuf, szBaseOID, MAX_PATH);            // copy base string
        WRAP_STRCAT_A(szBuf, TEXT(".1.0"), MAX_PATH);   // tack on for varbind OID

        thresholdVarBind.list = (RFC1157VarBind *) SNMP_malloc(sizeof(RFC1157VarBind)); // allocate storage for varbind

        if (thresholdVarBind.list == NULL)
        {
             WriteTrace(0x14,"BuildThresholdTrap: Unable to allocate storage for varbind\n");
             WriteLog(SNMPELEA_ERROR_ALLOC_VAR_BIND);
             return(FALSE);
        }

        WriteTrace(0x00,"BuildThresholdTrap: Storage allocated for varbind entry at address at %08X\n",
                thresholdVarBind.list);

        thresholdVarBind.len = 1;               // set # of varbinds

        WriteTrace(0x00,"BuildThresholdTrap: Number of varbinds present set to %lu\n",
                thresholdVarBind.len);

        TCHAR * tempthreshmsg = (TCHAR *) SNMP_malloc(strlen(lpszThreshold) + 1);

        if (tempthreshmsg == NULL)
        {
            WriteTrace(0x14,"BuildThresholdTrap: Unable to allocate tempthreshmsg\n");
            SNMP_free(thresholdVarBind.list);
            thresholdVarBind.list = NULL;
            thresholdVarBind.len = 0;
            return (FALSE);
        }
        strcpy(tempthreshmsg, lpszThreshold);

        thresholdVarBind.list[0].value.asnValue.string.length = strlen(tempthreshmsg);  // get string length
        thresholdVarBind.list[0].value.asnValue.string.stream = (PUCHAR) tempthreshmsg; // point to string
        thresholdVarBind.list[0].value.asnValue.string.dynamic = TRUE;  // indicate not dynamically allocated
        thresholdVarBind.list[0].value.asnType = ASN_RFC1213_DISPSTRING;        // indicate type of object

        if ( !StrToOid((char *) &szBuf, &thresholdVarBind.list[0].name) )
        {
            WriteTrace(0x14,"BuildThresholdTrap: Unable to convert OID from BaseEnterpriseOID\n");
            WriteLog(SNMPELEA_CANT_CONVERT_ENTERPRISE_OID);
            SNMP_free(thresholdVarBind.list[0].value.asnValue.string.stream);
            SNMP_free(thresholdVarBind.list);
            thresholdVarBind.list = NULL;
            thresholdVarBind.len = 0;
            return (FALSE);
        }

        if (nTraceLevel == 0)
        {
            WriteTrace(0x00,"BuildThresholdTrap: Varbind entry length is %lu\n",
                    thresholdVarBind.list[0].value.asnValue.string.length);
            WriteTrace(0x00,"BuildThresholdTrap: Varbind entry string is %s\n",
                    thresholdVarBind.list[0].value.asnValue.string.stream);
            WriteTrace(0x00,"BuildThresholdTrap: Varbind OID length is %lu\n",
                    thresholdVarBind.list[0].name.idLength);

            for (i = 0; i < thresholdVarBind.list[0].name.idLength; i++)
            {
                    WriteTrace(0x00,"BuildThresholdTrap: Varbind OID[%lu] is %lu\n",
                            i, thresholdVarBind.list[0].name.ids[i]);
            }
        }

        WriteTrace(0x00,"BuildThresholdTrap: &thresholdOID is at %08X\n", &thresholdOID);
        WriteTrace(0x00,"BuildThresholdTrap: thresholdOID is %08X\n", thresholdOID);
        WriteTrace(0x00,"BuildThresholdTrap: &thresholdVarBind is at %08X\n", &thresholdVarBind);
        WriteTrace(0x00,"BuildThresholdTrap: thresholdVarBind is %08X\n", thresholdVarBind);
        WriteTrace(0x0a,"BuildThresholdTrap: Variable bindings for threshold trap have been built\n");

        return (TRUE);

Error:
        WriteTrace(0x14,"BuildThresholdTrap: Unable to append .1.0 to %s\n", szBuf);
        return (FALSE);
}


extern "C" {
BOOL
APIENTRY
SnmpExtensionTrap(
    IN  OUT AsnObjectIdentifier *enterprise,
        OUT AsnInteger          *genericTrap,
        OUT AsnInteger          *specificTrap,
        OUT AsnTimeticks        *timeStamp,
    IN  OUT RFC1157VarBindList  *variableBindings
        )

/*++

Routine Description:

        SnmpExtensionTrap is the extension dll trap processing routine.

        This routine will query the log processing output queue to determine if a trap has been
        generated and needs to be returned. A Mutex object is used to synchronize processing
        between this thread and the log event processing thread.

Arguments:

        enterprise              -       Points to an OID indicating the originating enterprise generating the trap.

        genericTrap             -       Points to an indication of the generic trap. Always indicates specific.

        specificTrap            -       Points to an indication of the specific trap generated. This is the
                                                        event log message number.

        timeStamp               -       Points to a variable to receive the time stamp.

        variableBindings        -       Points to a list of variable bindings.

Return Value:

        TRUE    -       Valid trap data is being returned.

        FALSE   -       No traps were on the queue to process.

--*/

{
    LONG                    lastError;              // GetLastError value
    UINT                    i,j;                    // counter
    DWORD                   status, dwTimeNow;      // status variable and temp time holder
    HANDLE                  hWaitList[2];           // wait list
    PVarBindQueue   lpNewVarBindQueue;              // temporary pointer
    DWORD                   dwOne = 1;                              // for registry setting

    WriteTrace(0x0a,"SnmpExtensionTrap: Entering SnmpExtensionTrap routine\n");
    
    hWaitList[0] = hMutex;                          // mutex handle
    hWaitList[1] = hStopAll;                        // DLL termination event handle

    WriteTrace(0x00,"SnmpExtensionTrap: Varbind list upon entry is %08X\n", variableBindings);
    WriteTrace(0x00,"SnmpExtensionTrap: Varbind queue upon entry is %08X\n", lpVarBindQueue);
    WriteTrace(0x00,"SnmpExtensionTrap: Handle to Mutex object is %08X\n", hMutex);
    WriteTrace(0x0a,"SnmpExtensionTrap: Waiting for Mutex object to become available\n");

    while (TRUE)
    {
         status = WaitForMultipleObjects(
                 2,                             // only two objects to wait on
                 (CONST PHANDLE) &hWaitList,        // address of array of event handles
                 FALSE,                          // only one event is required
                 1000);                          // only wait one second

        lastError = GetLastError();                     // save any error conditions
        WriteTrace(0x0a,"SnmpExtensionTrap: WaitForMulitpleObjects returned a value of %lu\n", status);

        switch (status)
        {
            case WAIT_FAILED:
                 WriteTrace(0x14,"SnmpExtensionTrap: Error waiting for mutex event array is %lu\n",
                         lastError);                                     // trace error message
                 WriteLog(SNMPELEA_ERROR_WAIT_ARRAY, lastError); // log error message
                 WriteTrace(0x0a,"SnmpExtensionTrap: Exiting SnmpExtensionTrap routine with FALSE\n");
                 return(FALSE);                                  // get out now
            case WAIT_TIMEOUT:
                    WriteTrace(0x0a,"SnmpExtensionTrap: Mutex object not available yet. Wait will continue.\n");
                 continue;                                               // retry the wait
            case WAIT_ABANDONED:
                    WriteTrace(0x14,"SnmpExtensionTrap: Mutex object has been abandoned.\n");
                 WriteLog(SNMPELEA_MUTEX_ABANDONED);
                 WriteTrace(0x0a,"SnmpExtensionTrap: Exiting SnmpExtensionTrap routine with FALSE\n");
                 return(FALSE);                                  // get out now
            case 1:
                 WriteTrace(0x0a,"SnmpExtensionTrap: DLL shutdown detected. Wait abandoned.\n");
                 WriteTrace(0x0a,"SnmpExtensionTrap: Exiting SnmpExtensionTrap routine with FALSE\n");
                 return(FALSE);
            case 0:
                 WriteTrace(0x0a,"SnmpExtensionTrap: Mutex object acquired.\n");
                 break;
            default:
                    WriteTrace(0x14,"SnmpExtensionTrap: Undefined error encountered in WaitForMultipleObjects. Wait abandoned.\n");
                 WriteLog(SNMPELEA_ERROR_WAIT_UNKNOWN);
                 WriteTrace(0x0a,"SnmpExtensionTrap: Exiting SnmpExtensionTrap routine with FALSE\n");
                 return(FALSE);                                  // get out now
        }   // end switch for processing WaitForMultipleObjects

        break;                  // if we get here, then we've got the Mutex object

    }   // end while true for acquiring Mutex object

    while (TRUE)
    {
        if ( lpVarBindQueue == (PVarBindQueue) NULL )
        {
            WriteTrace(0x0a,"SnmpExtensionTrap: Varbind queue pointer indicates no more data to process\n");

            WriteTrace(0x0a,"SnmpExtensionTrap: Releasing mutex object %08X\n", hMutex);
            if (!ReleaseMutex(hMutex))
            {
                lastError = GetLastError();             // get error information
                WriteTrace(0x14,"SnmpExtensionTrap: Unable to release mutex object for reason code %lu\n",
                        lastError);
                WriteLog(SNMPELEA_RELEASE_MUTEX_ERROR, lastError);
            }

            WriteTrace(0x0a,"SnmpExtensionTrap: Exiting SnmpExtensionTrap routine with FALSE\n");
            return(FALSE);                          // exit and indicate pointers are valid
        }

        if (lpVarBindQueue->fProcessed)
        {
            dwTrapQueueSize--;
            WriteTrace(0x0a,"SnmpExtensionTrap: Current queue pointer indicates processed trap\n");
            WriteTrace(0x00,"SnmpExtensionTrap: Freeing processed trap storage\n");

            WriteTrace(0x00,"SnmpExtensionTrap: Freeing enterprise OID %08X\n",
                    lpVarBindQueue->enterprise);
            SNMP_free(lpVarBindQueue->enterprise->ids);                     // free enterprise OID
            SNMP_free(lpVarBindQueue->enterprise);
            WriteTrace(0x00,"SnmpExtensionTrap: Saving forward buffer pointer %08X\n",
                    lpVarBindQueue->lpNextQueueEntry);
            lpNewVarBindQueue = lpVarBindQueue->lpNextQueueEntry;   // save forward pointer

// The following will free the storage for the VarBindQueue entry. This includes:
//              enterprise
//              dwEventTime
//              dwEventID
//              lpVariableBindings
//              fProcessed
//              lpNextQueueEntry

            WriteTrace(0x00,"SnmpExtensionTrap: Freeing varbind list pointer %08X\n",
                    lpVarBindQueue->lpVariableBindings);
            // the code here assume the trap retriever free'ed
            // individual varbinds pointed by lpVariableBindings
            SNMP_free(lpVarBindQueue->lpVariableBindings);    // free varbind list pointer
            WriteTrace(0x00,"SnmpExtensionTrap: Freeing varbind queue entry storage %08X\n",
                    lpVarBindQueue);
            SNMP_free(lpVarBindQueue);          // free remaining storage
            lpVarBindQueue = lpNewVarBindQueue; // reset current pointer
            WriteTrace(0x00,"SnmpExtensionTrap: Setting current buffer pointer to %08X\n",
                    lpVarBindQueue);
            WriteTrace(0x0a,"SnmpExtensionTrap: Reentering process loop for next buffer entry\n");

            //While cleaning up check to see if threshold trap needs to be sent

            if (fThresholdEnabled && fThreshold && fSendThresholdTrap)
            {
                WriteTrace(0x0a,"SnmpExtensionTrap: Sending trap to indicate performance threshold has been reached.\n");
                fSendThresholdTrap = FALSE;             // reset indicator

                WriteTrace(0x0a,"SnmpExtensionTrap: Delete all varbind entries\n");

                //delete all the entries in the varbind queue.
                while ( lpVarBindQueue != (PVarBindQueue) NULL )
                {
                    lpNewVarBindQueue = lpVarBindQueue->lpNextQueueEntry;   // save forward pointer
                    FreeVarBind(lpVarBindQueue->lpVariableBindings->len,
                                        lpVarBindQueue->lpVariableBindings);    // free varbind information
                    SNMP_free(lpVarBindQueue->enterprise->ids);                 // free enterprise OID field
                    SNMP_free(lpVarBindQueue->enterprise);                      // free enterprise OID field
                    SNMP_free(lpVarBindQueue->lpVariableBindings->list);        // free varbind storage
                    SNMP_free(lpVarBindQueue->lpVariableBindings);              // free varbind list storage
                    SNMP_free(lpVarBindQueue);                                  // free varbind entry
                    lpVarBindQueue = lpNewVarBindQueue;                         // reset current pointer
                }

                WriteTrace(0x0a,"SnmpExtensionTrap: Deleted all entries, releasing mutex object %08X\n", hMutex);

                if (!ReleaseMutex(hMutex))
                {
                    lastError = GetLastError();             // get error information
                    WriteTrace(0x14,"SnmpExtensionTrap: Unable to release mutex object for reason code %lu\n",
                                lastError);
                    WriteLog(SNMPELEA_RELEASE_MUTEX_ERROR, lastError);
                }

                if (!BuildThresholdTrap())
                {
                    return (FALSE);
                }

                *enterprise = *(&thresholdOID);                                 // point to enterprise OID field
                *genericTrap = SNMP_GENERICTRAP_ENTERSPECIFIC;  // indicate a specific type trap
                *timeStamp = GetCurrentTime() - dwTimeZero;             // get time reference for trap
                *specificTrap = SNMPELEA_THRESHOLD_REACHED & 0x0000ffff;        // get log message number
                *variableBindings = *(&thresholdVarBind);               // get varbind list pointer

                if (nTraceLevel == 0)
                {
                    WriteTrace(0x00,"SnmpExtensionTrap: *enterprise is %08X\n", *enterprise);
                    WriteTrace(0x00,"SnmpExtensionTrap: &thresholdOID is %08X\n", &thresholdOID);
                    WriteTrace(0x00,"SnmpExtensionTrap: *timeStamp is %08X\n", *timeStamp);
                    WriteTrace(0x00,"SnmpExtensionTrap: *variableBindings is %08X\n", *variableBindings);
                    WriteTrace(0x00,"SnmpExtensionTrap: &thresholdVarBind is %08X\n", &thresholdVarBind);
                    WriteTrace(0x00,"SnmpExtensionTrap: *specificTrap is %08X\n", *specificTrap);
                    WriteTrace(0x00,"SnmpExtensionTrap: SNMPELEA_THRESHOLD_REACHED is %08X\n", SNMPELEA_THRESHOLD_REACHED & 0x0000ffff);

                    WriteTrace(0x00,"SnmpExtensionTrap: Number of entries in enterprise OID is %lu\n",
                            enterprise->idLength);

                    for (i = 0; i < enterprise->idLength; i++)
                    {
                        WriteTrace(0x00,"SnmpExtensionTrap: Enterprise OID[%lu] is %lu\n",
                                    i, enterprise->ids[i]);
                    }

                    for (i = 0; i < variableBindings->len; i++)
                    {
                        WriteTrace(0x00,"SnmpExtensionTrap: Variable binding %lu is %s, length %lu\n",
                                i, variableBindings->list[i].value.asnValue.string.stream,
                                variableBindings->list[i].value.asnValue.string.length
                                );

                        WriteTrace(0x00,"SnmpExtensionTrap: OID for this binding is (number of %lu):\n",
                                variableBindings->list[i].name.idLength);
                        WriteTrace(0x00,"SnmpExtensionTrap: ");

                        for (j = 0; j < variableBindings->list[i].name.idLength; j++)
                        {
                                WriteTrace(MAXDWORD,"%lu.", variableBindings->list[i].name.ids[j]);
                        }
                        WriteTrace(MAXDWORD,"\n");
                    }
                }

                WriteTrace(0x0a,"SnmpExtensionTrap: Exiting SnmpExtensionTrap routine with TRUE\n");
                return(TRUE);                           // exit and indicate pointers are valid
            }

            continue;                                   // reenter loop
        }

        *enterprise = *(lpVarBindQueue->enterprise);    // point to enterprise OID field
        *genericTrap = SNMP_GENERICTRAP_ENTERSPECIFIC;  // indicate a specific type trap
        *timeStamp = lpVarBindQueue->dwEventTime;       // get time reference for trap
        *specificTrap = lpVarBindQueue->dwEventID;      // get event log message number
        *variableBindings = *(lpVarBindQueue->lpVariableBindings);      // get varbind list pointer

        if (nTraceLevel == 0)
        {
            WriteTrace(0x00,"SnmpExtensionTrap: *enterprise is %08X\n", *enterprise);
            WriteTrace(0x00,"SnmpExtensionTrap: *(lpVarBindQueue->enterprise) is %08X\n",
                    lpVarBindQueue->enterprise);
            WriteTrace(0x00,"SnmpExtensionTrap: *variableBindings is %08X\n", *variableBindings);
            WriteTrace(0x00,"SnmpExtensionTrap: *(lpVarBindQueue->VariableBindings) is %08X\n",
                    lpVarBindQueue->lpVariableBindings);

            WriteTrace(0x00,"SnmpExtensionTrap: Number of entries in enterprise OID is %lu\n",
                    enterprise->idLength);

            for (i = 0; i < enterprise->idLength; i++)
            {
                WriteTrace(0x00,"SnmpExtensionTrap: Enterprise OID[%lu] is %lu\n",
                            i, enterprise->ids[i]);
            }

            for (i = 0; i < variableBindings->len; i++)
            {
                WriteTrace(0x00,"SnmpExtensionTrap: Variable binding %lu is %s, length %lu\n",
                        i, variableBindings->list[i].value.asnValue.string.stream,
                        variableBindings->list[i].value.asnValue.string.length
                        );

                WriteTrace(0x00,"SnmpExtensionTrap: OID for this binding is (number of %lu):\n",
                        variableBindings->list[i].name.idLength);
                WriteTrace(0x00,"SnmpExtensionTrap: ");

                for (j = 0; j < variableBindings->list[i].name.idLength; j++)
                {
                    WriteTrace(MAXDWORD,"%lu.", variableBindings->list[i].name.ids[j]);
                }
                WriteTrace(MAXDWORD,"\n");
            }
        }

        lpVarBindQueue->fProcessed = TRUE;     // indicate this entry processed
        break;
    }

    WriteTrace(0x0a,"SnmpExtensionTrap: Releasing mutex object %08X\n", hMutex);
    if (!ReleaseMutex(hMutex))
    {
        lastError = GetLastError();             // get error information
        WriteTrace(0x14,"SnmpExtensionTrap: Unable to release mutex object for reason code %lu\n",
                lastError);
        WriteLog(SNMPELEA_RELEASE_MUTEX_ERROR, lastError);
    }

    if (fThresholdEnabled)
    {
        if (!fThreshold)
        {
            dwTimeNow = GetTickCount() / 1000;  // get current time information

            if (dwTrapStartTime == 0)
            {
                dwTrapCount = 1;                // indicate first trap sent
                dwTrapStartTime = dwTimeNow;    // set start time
            }
            else
            {
                if ( (dwTimeNow - dwTrapStartTime) >= dwThresholdTime )
                {
                    WriteTrace(0x0a,"SnmpExtensionTrap:     Threshold time has been exceeded. Resetting threshold values.\n");
                    dwTrapCount = 1;                // reset to 1 trap sent
                    dwTrapStartTime = dwTimeNow;    // set start time
                }
                else
                {
                    if (++dwTrapCount >= dwThresholdCount)
                    {
                        WriteTrace(0x0a,"SnmpExtensionTrap: Threshold count has been reached within defined performance parameters.\n");
                        WriteTrace(0x0a,"SnmpExtensionTrap: Further traps will not be sent without operator intervention.\n");
                        WriteLog(SNMPELEA_THRESHOLD_REACHED);

                        fThreshold = TRUE;          // indicate performance stuff active
                        fSendThresholdTrap = TRUE;  // indicate to send the threshold reached trap next time

                        dwTrapCount = 0;                // reset trap count
                        dwTrapStartTime = 0;            // indicate start time is invalid

                        if ( (lastError = RegSetValueEx(
                                hkRegResult,                // registry key opened
                                EXTENSION_THRESHOLD_FLAG,   // which key value to set
                                NULL,                       // reserved
                                REG_DWORD,                  // type of value to set
                                (const LPBYTE) &dwOne,      // address of value to set to
                                sizeof(DWORD)               // size of the data value
                                            ) != ERROR_SUCCESS)
                            )
                        {
                            WriteTrace(0x14,"SnmpExtensionTrap: Unable to set registry key for threshold reached; RegSetValueEx returned %lu\n",
                                        lastError);
                            WriteLog(SNMPELEA_SET_VALUE_FAILED, lastError);
                        }

                        WriteTrace(0x0a,"SnmpExtensionTrap: Threshold reached flag has been set in the registry\n");
                    }
                    else
                    {
                        WriteTrace(0x00,"SnmpExtensionTrap: Threshold count is %lu; time elapsed is %08X\n",
                                    dwTrapCount, dwTimeNow - dwTrapStartTime);
                    }
                }
            }
        }
    }

    WriteTrace(0x0a,"SnmpExtensionTrap: Exiting SnmpExtensionTrap routine with TRUE\n");
    return(TRUE);                                                                   // indicate that trap data is valid
}
}


extern "C" {
BOOL
APIENTRY
SnmpExtensionQuery(
    IN          BYTE                    requestType,
    IN  OUT     RFC1157VarBindList      *variableBindings,
    OUT         AsnInteger              *errorStatus,
    OUT         AsnInteger              *errorIndex
        )

/*++

Routine Description:

        SnmpExtensionQuery is the extension dll query processing routine.

        This routine is not supported and always returns an error.

Arguments:

        requestType                     -       Points to an OID indicating the originating enterprise generating the trap.

        variableBindings        -       Points to a list of variable bindings.

        errorStatus                     -       Points to a variable to receive the error status. Always ASN_ERRORSTATUS_NOSUCHNAME.

        errorIndex                      -       Points to a variable to receive the resulting error index. Always 0.

Return Value:

        Always returns TRUE.

--*/

{
    WriteTrace(0x0a,"SnmpExtensionQuery: Entering SnmpExtensionQuery routine\n");

    *errorStatus = SNMP_ERRORSTATUS_NOSUCHNAME; // indicate we don't know what they're asking for
    *errorIndex = 1;                            // indicate that it's the first varbind
                                                // show it's the first parameter

    if (requestType == MIB_ACTION_GETNEXT)
    {
        AsnObjectIdentifier oidOutOfView;

        // initialize oidOutOfView.ids, to avoid heap corruption when
        // when free-ed if StrToOid failed.
        oidOutOfView.ids = NULL;
        if (variableBindings != NULL && 
            StrToOid(szSupView, &oidOutOfView) &&
            oidOutOfView.idLength > 0)
        {
            UINT iVar;

            oidOutOfView.ids[oidOutOfView.idLength-1]++;

            for (iVar = 0; iVar < variableBindings->len; iVar++)
            {
                RFC1157VarBind *pVarBind;

                pVarBind = &(variableBindings->list[iVar]);
                SnmpUtilOidFree(&(pVarBind->name));
                if (! SnmpUtilOidCpy(&(pVarBind->name), &oidOutOfView))
                {
                    SnmpUtilOidFree(&oidOutOfView);
                    *errorStatus = SNMP_ERRORSTATUS_GENERR;
                    *errorIndex = iVar+1;       // iVar+1th varbind failed
                    return(SNMPAPI_NOERROR);    // return to caller 
                }
            }
            *errorStatus = SNMP_ERRORSTATUS_NOERROR;
            *errorIndex = 0;
        }

        SnmpUtilOidFree(&oidOutOfView);
    }


    WriteTrace(0x0a,"SnmpExtensionQuery: Exiting SnmpExtensionQuery routine\n");
    return(SNMPAPI_NOERROR);    // return to caller
}
}


// MikeCure 4/3/98 hotfix for SMS Bug1 #20521
//=============================================================================
//
//      EnablePrivileges()
//
//      Notes:  Added a new function to set the proper privileges on the
//              security log With NT 5 and NT4 SP5, the default privileges
//              no longer have access implicitely allowed. Now we've got to
//              grant explicit access.
//
//      Routine Description:
//              This routine enables all privileges in the token.
//
//      Arguments:
//              None.
//
//      Return Value:
//              BOOL.
//
//=============================================================================
BOOL EnablePrivilege(VOID)
{
    HANDLE Token;
    ULONG ReturnLength;
    PTOKEN_PRIVILEGES NewState;
    LUID Luid;
    BOOL Result;
    
    Token = NULL;
    NewState = NULL;
    
    Result = OpenProcessToken( GetCurrentProcess(),
        TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,
        &Token
        );
    if (Result) {
        ReturnLength = 4096;
        NewState = (PTOKEN_PRIVILEGES) malloc( ReturnLength );
        Result = (BOOL)(NewState != NULL);
        if (Result) {
            Result = GetTokenInformation( Token,            // TokenHandle
                TokenPrivileges,  // TokenInformationClass
                NewState,         // TokenInformation
                ReturnLength,     // TokenInformationLength
                &ReturnLength     // ReturnLength
                );
            
            if (Result) {
                //
                // Enable Security Privilege
                //
                Result = LookupPrivilegeValue(  NULL,
                    "SeSecurityPrivilege",
                    &Luid
                    );
                
                    if (Result) {
                    
                    NewState->PrivilegeCount = 1;
                    NewState->Privileges[0].Luid = Luid;
                    NewState->Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
                    
                    Result = AdjustTokenPrivileges( Token,          // TokenHandle
                        FALSE,          // DisableAllPrivileges
                        NewState,       // NewState (OPTIONAL)
                        ReturnLength,   // BufferLength
                        NULL,           // PreviousState (OPTIONAL)
                        &ReturnLength   // ReturnLength
                        );
                }       
            }
        }
                
        if (NewState != NULL) {
            free( NewState );
        }
        
        if (Token != NULL) {
            CloseHandle( Token );
        }
    }
    return( Result );
    
}
