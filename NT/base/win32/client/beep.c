/*++

Copyright (c) 1990  Microsoft Corporation

Module Name:

    beep.c

Abstract:

    This module contains the Win32 Beep APIs

Author:

    Steve Wood (stevewo)  5-Oct-1991

Revision History:

--*/

#include "basedll.h"
#pragma hdrstop

#include <ntddbeep.h>
#include "conapi.h"

#define IsActiveConsoleSession() (BOOLEAN)(USER_SHARED_DATA->ActiveConsoleId == NtCurrentPeb()->SessionId)

/*
 * Forward declaration
 */

VOID NotifySoundSentry(VOID);

BOOL
APIENTRY
Beep(
    DWORD dwFreq,
    DWORD dwDuration
    )
{
    OBJECT_ATTRIBUTES ObjectAttributes;
    UNICODE_STRING NameString;
    NTSTATUS Status;
    IO_STATUS_BLOCK IoStatus;
    BEEP_SET_PARAMETERS BeepParameters;
    HANDLE hBeepDevice, hTSBeepDevice;

    if ( IsTerminalServer() ) {

        if ( !pWinStationBeepOpen ) {
            HMODULE hwinsta = NULL;
            /*
             *  Get handle to winsta.dll
             */
            if ( (hwinsta = LoadLibraryW( L"WINSTA" )) != NULL ) {

                PWINSTATIONBEEPOPEN BeepOpenFunction;

                BeepOpenFunction = (PWINSTATIONBEEPOPEN)
                    GetProcAddress( hwinsta, "_WinStationBeepOpen" );

                if (BeepOpenFunction
                    && ! InterlockedCompareExchangePointer((PVOID *)&pWinStationBeepOpen,
                                                           BeepOpenFunction,
                                                           NULL)) {
                    // We got a function, and we were the first to
                    // store our function to the global -- so pin the
                    // dll containing the function.
                    LdrAddRefDll(LDR_ADDREF_DLL_PIN, hwinsta);
                } else {
                    // Either we didn't get a function, or we weren't
                    // the first to store our function to the global
                    // -- so we don't need our reference to the dll.
                    FreeLibrary(hwinsta);
                }
            }
        }

        hTSBeepDevice = NULL;

        if ( pWinStationBeepOpen )
            hTSBeepDevice = (*pWinStationBeepOpen)( -1 ); //Current Session
    }

    if ( IsTerminalServer() && !IsActiveConsoleSession() ) {

        hBeepDevice = hTSBeepDevice;

        if ( hBeepDevice == NULL )
            Status = STATUS_ACCESS_DENIED;
        else
            Status = STATUS_SUCCESS;
    }
    else {

        RtlInitUnicodeString( &NameString, DD_BEEP_DEVICE_NAME_U );
        InitializeObjectAttributes( &ObjectAttributes,
                                    &NameString,
                                    0,
                                    NULL,
                                    NULL
                                  );
        Status = NtCreateFile( &hBeepDevice,
                               FILE_READ_DATA | FILE_WRITE_DATA,
                               &ObjectAttributes,
                               &IoStatus,
                               NULL,
                               0,
                               FILE_SHARE_READ | FILE_SHARE_WRITE,
                               FILE_OPEN_IF,
                               0,
                               NULL,
                               0L
                             );
    }
    if (!NT_SUCCESS( Status )) {
        if ( IsTerminalServer() && hTSBeepDevice ) {
            NtClose( hTSBeepDevice );
        }
        BaseSetLastNTError( Status );
        return( FALSE );
        }

    //
    // 0,0 is a special case used to turn off a beep.  Otherwise
    // validate the dwFreq parameter to be in range.
    //

    if ((dwFreq != 0 || dwDuration != 0) &&
        (dwFreq < (ULONG)0x25 || dwFreq > (ULONG)0x7FFF)
       ) {
        Status = STATUS_INVALID_PARAMETER;
        }
    else {
        BeepParameters.Frequency = dwFreq;
        BeepParameters.Duration = dwDuration;

        Status = NtDeviceIoControlFile( hBeepDevice,
                                        NULL,
                                        NULL,
                                        NULL,
                                        &IoStatus,
                                        IOCTL_BEEP_SET,
                                        &BeepParameters,
                                        sizeof( BeepParameters ),
                                        NULL,
                                        0
                                      );
        }

    if ( IsTerminalServer() && IsActiveConsoleSession() && hTSBeepDevice ) {

        //
        // It's the console and since we got a beep device, it's being shadowed.
        // So let's do it one more time for the shadow client.
        //
        if ( NT_SUCCESS(Status) ) {
             NtDeviceIoControlFile( hTSBeepDevice,
                                    NULL,
                                    NULL,
                                    NULL,
                                    &IoStatus,
                                    IOCTL_BEEP_SET,
                                    &BeepParameters,
                                    sizeof( BeepParameters ),
                                    NULL,
                                    0
                                  );
        }

        NtClose( hTSBeepDevice );
    }

    
    NotifySoundSentry();

    if (!NT_SUCCESS( Status )) {
        BaseSetLastNTError( Status );
        NtClose( hBeepDevice );
        return( FALSE );
        }
    else {
        //
        // Beep device is asynchronous, so sleep for duration
        // to allow this beep to complete.
        //

        if (dwDuration != (DWORD)-1 && (dwFreq != 0 || dwDuration != 0)) {
            SleepEx( dwDuration, TRUE );
            }

        NtClose( hBeepDevice );
        return( TRUE );
        }
}


VOID
NotifySoundSentry(VOID)
{

#if defined(BUILD_WOW6432)
    ULONG VideoMode;

    if (!GetConsoleDisplayMode(&VideoMode)) {
        VideoMode = 0;
    }

    //
    // SoundSentry is currently only supported for Windows mode - no
    // full screen support.
    //
     
    if (VideoMode == 0) {    
        CsrBasepSoundSentryNotification(VideoMode);
    }
#else
    BASE_API_MSG m;
    PBASE_SOUNDSENTRY_NOTIFICATION_MSG e = &m.u.SoundSentryNotification;

    if (!GetConsoleDisplayMode(&e->VideoMode)) {
        e->VideoMode = 0;
    }
    //
    // SoundSentry is currently only supported for Windows mode - no
    // full screen support.
    //
    if (e->VideoMode == 0) {
        CsrClientCallServer((PCSR_API_MSG)&m,
                            NULL,
                            CSR_MAKE_API_NUMBER( BASESRV_SERVERDLL_INDEX,
                                                 BasepSoundSentryNotification ),
                            sizeof( *e )
                            );
    }
#endif

}
