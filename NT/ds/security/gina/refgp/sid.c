//*************************************************************
//
//  SID management functions.
//
//  THESE FUNCTIONS ARE WINDOWS NT SPECIFIC!!!!!
//
//  Microsoft Confidential
//  Copyright (c) Microsoft Corporation 1995
//  All rights reserved
//
//*************************************************************

#include "refgp.h"

/***************************************************************************\
* GetSidString
*
* Allocates and returns a string representing the sid of the current user
* The returned pointer should be freed using DeleteSidString().
*
* Returns a pointer to the string or NULL on failure.
*
* History:
* 26-Aug-92 Davidc     Created
*
\***************************************************************************/
LPTSTR GetSidString(HANDLE UserToken)
{
    NTSTATUS NtStatus;
    PSID UserSid;
    UNICODE_STRING UnicodeString;
    LPTSTR lpEnd;
#ifndef UNICODE
    STRING String;
#endif

    //
    // Get the user sid
    //

    UserSid = GetUserSid(UserToken);
    if (UserSid == NULL) {
        return NULL;
    }

    //
    // Convert user SID to a string.
    //

    NtStatus = RtlConvertSidToUnicodeString(
                            &UnicodeString,
                            UserSid,
                            (BOOLEAN)TRUE // Allocate
                            );
    //
    // We're finished with the user sid
    //

    DeleteUserSid(UserSid);

    //
    // See if the conversion to a string worked
    //

    if (!NT_SUCCESS(NtStatus)) {
        return NULL;
    }

#ifdef UNICODE


    return(UnicodeString.Buffer);

#else

    //
    // Convert the string to ansi
    //

    NtStatus = RtlUnicodeStringToAnsiString(&String, &UnicodeString, TRUE);
    RtlFreeUnicodeString(&UnicodeString);
    if (!NT_SUCCESS(NtStatus)) {
        return NULL;
    }


    return(String.Buffer);

#endif

}


/***************************************************************************\
* DeleteSidString
*
* Frees up a sid string previously returned by GetSidString()
*
* Returns nothing.
*
* History:
* 26-Aug-92 Davidc     Created
*
\***************************************************************************/
VOID DeleteSidString(LPTSTR SidString)
{

#ifdef UNICODE
    UNICODE_STRING String;

    RtlInitUnicodeString(&String, SidString);

    RtlFreeUnicodeString(&String);
#else
    ANSI_STRING String;

    RtlInitAnsiString(&String, SidString);

    RtlFreeAnsiString(&String);
#endif

}



/***************************************************************************\
* GetUserSid
*
* Allocs space for the user sid, fills it in and returns a pointer. Caller
* The sid should be freed by calling DeleteUserSid.
*
* Note the sid returned is the user's real sid, not the per-logon sid.
*
* Returns pointer to sid or NULL on failure.
*
* History:
* 26-Aug-92 Davidc      Created.
\***************************************************************************/
PSID GetUserSid (HANDLE UserToken)
{
    PTOKEN_USER pUser, pTemp;
    PSID pSid;
    DWORD BytesRequired = 200;
    NTSTATUS status;


    //
    // Allocate space for the user info
    //

    pUser = (PTOKEN_USER)LocalAlloc(LMEM_FIXED, BytesRequired);


    if (pUser == NULL) {
        return NULL;
    }


    //
    // Read in the UserInfo
    //

    status = NtQueryInformationToken(
                 UserToken,                 // Handle
                 TokenUser,                 // TokenInformationClass
                 pUser,                     // TokenInformation
                 BytesRequired,             // TokenInformationLength
                 &BytesRequired             // ReturnLength
                 );

    if (status == STATUS_BUFFER_TOO_SMALL) {

        //
        // Allocate a bigger buffer and try again.
        //

        pTemp = LocalReAlloc(pUser, BytesRequired, LMEM_MOVEABLE);
        if (pTemp == NULL) {
            LocalFree (pUser);
            return NULL;
        }

        pUser = pTemp;

        status = NtQueryInformationToken(
                     UserToken,             // Handle
                     TokenUser,             // TokenInformationClass
                     pUser,                 // TokenInformation
                     BytesRequired,         // TokenInformationLength
                     &BytesRequired         // ReturnLength
                     );

    }

    if (!NT_SUCCESS(status)) {
        LocalFree(pUser);
        return NULL;
    }


    BytesRequired = RtlLengthSid(pUser->User.Sid);
    pSid = LocalAlloc(LMEM_FIXED, BytesRequired);
    if (pSid == NULL) {
        LocalFree(pUser);
        return NULL;
    }


    status = RtlCopySid(BytesRequired, pSid, pUser->User.Sid);

    LocalFree(pUser);

    if (!NT_SUCCESS(status)) {
        LocalFree(pSid);
        pSid = NULL;
    }


    return pSid;
}


/***************************************************************************\
* DeleteUserSid
*
* Deletes a user sid previously returned by GetUserSid()
*
* Returns nothing.
*
* History:
* 26-Aug-92 Davidc     Created
*
\***************************************************************************/
VOID DeleteUserSid(PSID Sid)
{
    LocalFree(Sid);
}

//+--------------------------------------------------------------------------
//
//  Function:   AllocateAndInitSidFromString
//
//  Synopsis:   given the string representation of a SID, this function
//              allocate and initializes a SID which the string represents
//              For more information on the string representation of SIDs
//              refer to ntseapi.h & ntrtl.h
//
//  Arguments:  [in] lpszSidStr : the string representation of the SID
//              [out] pSID : the actual SID structure created from the string
//
//  Returns:    STATUS_SUCCESS : if the sid structure was successfully created
//              or an error code based on errors that might occur
//
//  History:    10/6/1998  RahulTh  created
//
//---------------------------------------------------------------------------
NTSTATUS AllocateAndInitSidFromString (const WCHAR* lpszSidStr, PSID* ppSid)
{
    WCHAR *     pSidStr = 0;
    WCHAR*      pString = 0;
    NTSTATUS    Status;
    WCHAR*      pEnd = 0;
    int         count;
    BYTE        SubAuthCount;
    DWORD       SubAuths[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    ULONG       n;
    HRESULT     hr;
    SID_IDENTIFIER_AUTHORITY Auth;

    ULONG ulNoChars = lstrlen (lpszSidStr) + 1;
    pSidStr = LocalAlloc(LPTR, ulNoChars*sizeof(WCHAR));;
    if (!pSidStr)
    {
        Status = STATUS_NO_MEMORY;
        goto AllocAndInitSidFromStr_End;
    }

    hr = StringCchCopy (pSidStr, ulNoChars, lpszSidStr);
    ASSERT(SUCCEEDED(hr));

    pString = pSidStr;
    *ppSid = NULL;

    count = 0;
    do
    {
        pString = wcschr (pString, '-');
        if (NULL == pString)
            break;
        count++;
        pString++;
    } while (1);

    SubAuthCount = (BYTE)(count - 2);
    if (0 > SubAuthCount || 8 < SubAuthCount)
    {
        Status = ERROR_INVALID_SID;
        goto AllocAndInitSidFromStr_End;
    }

    pString = wcschr (pSidStr, L'-');
    pString++;
    pString = wcschr (pString, L'-'); //ignore the revision #
    pString++;
    pEnd = wcschr (pString, L'-');   //go to the beginning of subauths.
    if (NULL != pEnd) *pEnd = L'\0';

    Status = LoadSidAuthFromString (pString, &Auth);

    if (STATUS_SUCCESS != Status)
        goto AllocAndInitSidFromStr_End;

    for (count = 0; count < SubAuthCount; count++)
    {
        pString = pEnd + 1;
        pEnd = wcschr (pString, L'-');
        if (pEnd)
            *pEnd = L'\0';
        Status = GetIntFromUnicodeString (pString, 10, &n);
        if (STATUS_SUCCESS != Status)
            goto AllocAndInitSidFromStr_End;
        SubAuths[count] = n;
    }

    Status = RtlAllocateAndInitializeSid (&Auth, SubAuthCount,
                                          SubAuths[0], SubAuths[1], SubAuths[2],
                                          SubAuths[3], SubAuths[4], SubAuths[5],
                                          SubAuths[6], SubAuths[7], ppSid);

AllocAndInitSidFromStr_End:
    if (pSidStr)
        LocalFree( pSidStr );
    return Status;
}

//+--------------------------------------------------------------------------
//
//  Function:   LoadSidAuthFromString
//
//  Synopsis:   given a string representing the SID authority (as it is
//              normally represented in string format, fill the SID_AUTH..
//              structure. For more details on the format of the string
//              representation of the sid authority, refer to ntseapi.h and
//              ntrtl.h
//
//  Arguments:  [in] pString : pointer to the unicode string
//              [out] pSidAuth : pointer to the SID_IDENTIFIER_AUTH.. that is
//                              desired
//
//  Returns:    STATUS_SUCCESS if it succeeds
//              or an error code
//
//  History:    9/29/1998  RahulTh  created
//
//---------------------------------------------------------------------------
NTSTATUS LoadSidAuthFromString (const WCHAR* pString,
                                PSID_IDENTIFIER_AUTHORITY pSidAuth)
{
    size_t len;
    int i;
    NTSTATUS Status;
    const ULONG LowByteMask = 0xFF;
    ULONG n;

    len = lstrlenW (pString);

    if (len > 2 && 'x' == pString[1])
    {
        //this is in hex.
        //so we must have exactly 14 characters
        //(2 each for each of the 6 bytes) + 2 for the leading 0x
        if (14 != len)
        {
            Status = ERROR_INVALID_SID;
            goto LoadAuthEnd;
        }

        for (i=0; i < 6; i++)
        {
            pString += 2;   //we need to skip the leading 0x
            pSidAuth->Value[i] = (UCHAR)(((pString[0] - L'0') << 4) +
                                         (pString[1] - L'0'));
        }
    }
    else
    {
        //this is in decimal
        Status = GetIntFromUnicodeString (pString, 10, &n);
        if (Status != STATUS_SUCCESS)
            goto LoadAuthEnd;

        pSidAuth->Value[0] = pSidAuth->Value[1] = 0;
        for (i = 5; i >=2; i--, n>>=8)
            pSidAuth->Value[i] = (UCHAR)(n & LowByteMask);
    }

    Status = STATUS_SUCCESS;

LoadAuthEnd:
    return Status;
}

//+--------------------------------------------------------------------------
//
//  Function:   GetIntfromUnicodeString
//
//  Synopsis:   converts a unicode string into an integer
//
//  Arguments:  [in] szNum : the number represented as a unicode string
//              [in] Base : the base in which the resultant int is desired
//              [out] pValue : pointer to the integer representation of the
//                             number
//
//  Returns:    STATUS_SUCCESS if successful.
//              or some other error code
//
//  History:    9/29/1998  RahulTh  created
//
//---------------------------------------------------------------------------
NTSTATUS GetIntFromUnicodeString (const WCHAR* szNum, ULONG Base, PULONG pValue)
{
    WCHAR * pwszNumStr = 0;
    UNICODE_STRING StringW;
    size_t len;
    NTSTATUS Status;
    HRESULT hr;

    len = lstrlen (szNum);
    pwszNumStr = LocalAlloc( LPTR, (len + 1) * sizeof(WCHAR));

    if (!pwszNumStr)
    {
        Status = STATUS_NO_MEMORY;
        goto GetNumEnd;
    }

    hr = StringCchCopy (pwszNumStr, len+1, szNum);
    ASSERT(SUCCEEDED(hr));

    StringW.Length = len * sizeof(WCHAR);
    StringW.MaximumLength = StringW.Length + sizeof (WCHAR);
    StringW.Buffer = pwszNumStr;

    Status = RtlUnicodeStringToInteger (&StringW, Base, pValue);

GetNumEnd:
    if (pwszNumStr)
        LocalFree( pwszNumStr );
    return Status;
}
