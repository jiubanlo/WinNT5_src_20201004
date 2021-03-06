/////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 1996-2002 Microsoft Corporation
//
//  Module Name:
//      DDxDDv.cpp
//
//  Abstract:
//      Implementation of custom dialog data exchange/dialog data validation
//      routines.
//
//  Author:
//      David Potter (davidp)   September 5, 1996
//
//  Revision History:
//
//  Notes:
//
/////////////////////////////////////////////////////////////////////////////

#include <afxwin.h>
#include <resapi.h>
#include <StrSafe.h>
#include "DDxDDv.h"
#include "AdmCommonRes.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Static Function Prototypes
/////////////////////////////////////////////////////////////////////////////
void CleanupLabel(LPTSTR psz);

/////////////////////////////////////////////////////////////////////////////
//++
//
//  DDX_Number
//
//  Routine Description:
//      Do data exchange between the dialog and the class.
//
//  Arguments:
//      pDX         [IN OUT] Data exchange object 
//      nIDC        [IN] Control ID.
//      dwValue     [IN OUT] Value to set or get.
//      dwMin       [IN] Minimum value.
//      dwMax       [IN] Maximum value.
//      bSigned     [IN] TRUE = value is signed, FALSE = value is unsigned
//
//  Return Value:
//      None.
//
//--
/////////////////////////////////////////////////////////////////////////////
void AFXAPI DDX_Number(
    IN OUT CDataExchange *  pDX,
    IN int                  nIDC,
    IN OUT DWORD &          rdwValue,
    IN DWORD                dwMin,
    IN DWORD                dwMax,
    IN BOOL                 bSigned
    )
{
    HWND    hwndCtrl;
    DWORD   dwValue;

    ASSERT(pDX != NULL);
#ifdef _DEBUG
    if (bSigned)
    {
        ASSERT((LONG) dwMin < (LONG) dwMax);
    }
    else
    {
        ASSERT(dwMin < dwMax);
    }
#endif // _DEBUG

    // Get the control window handle.
    hwndCtrl = pDX->PrepareEditCtrl(nIDC);

    if (pDX->m_bSaveAndValidate)
    {
        BOOL    bTranslated;

        // Get the number from the control.
        dwValue = GetDlgItemInt(pDX->m_pDlgWnd->m_hWnd, nIDC, &bTranslated, bSigned);

        // If the retrival failed, it is a signed number, and the minimum
        // value is the smallest negative value possible, check the string itself.
        if (!bTranslated && bSigned && (dwMin == 0x80000000))
        {
            UINT    cch;
            TCHAR   szNumber[20];

            // See if it is the smallest negative number.
            cch = GetDlgItemText(pDX->m_pDlgWnd->m_hWnd, nIDC, szNumber, sizeof(szNumber) / sizeof(TCHAR));
            if ((cch != 0) && (_tcsncmp( szNumber, _T("-2147483648"), RTL_NUMBER_OF( szNumber ) ) == 0))
            {
                dwValue = 0x80000000;
                bTranslated = TRUE;
            }  // if:  text retrieved successfully and is highest negative number
        }  // if:  error translating number and getting signed number

        // If the retrieval failed or the specified number is
        // out of range, display an error.
        if (   !bTranslated
            || (bSigned && (((LONG) dwValue < (LONG) dwMin) || ((LONG) dwValue > (LONG) dwMax)))
            || (!bSigned && ((dwValue < dwMin) || (dwValue > dwMax)))
            )
        {
            TCHAR   szMin[32];
            TCHAR   szMax[32];
            CString strPrompt;
            HRESULT hr;

            if (bSigned)
            {
                hr = StringCchPrintf(szMin, RTL_NUMBER_OF( szMin ), _T("%d%"), dwMin);
                ASSERT( SUCCEEDED( hr ) );
                hr = StringCchPrintf(szMax, RTL_NUMBER_OF( szMax ), _T("%d%"), dwMax);
                ASSERT( SUCCEEDED( hr ) );
            }  // if:  signed number
            else
            {
                hr = StringCchPrintf(szMin, RTL_NUMBER_OF( szMin ), _T("%u%"), dwMin);
                ASSERT( SUCCEEDED( hr ) );
                hr = StringCchPrintf(szMax, RTL_NUMBER_OF( szMax ), _T("%u%"), dwMax);
                ASSERT( SUCCEEDED( hr ) );
            }  // else:  unsigned number
            AfxFormatString2(strPrompt, AFX_IDP_PARSE_INT_RANGE, szMin, szMax);
            AfxMessageBox(strPrompt, MB_ICONEXCLAMATION, AFX_IDP_PARSE_INT_RANGE);
            strPrompt.Empty(); // exception prep
            pDX->Fail();
        }  // if:  invalid string
        else
            rdwValue = dwValue;
    }  // if:  saving data
    else
    {
        CString     strMinValue;
        CString     strMaxValue;
        UINT        cchMax;

        // Set the maximum number of characters that can be entered.
        if (bSigned)
        {
            strMinValue.Format(_T("%d"), dwMin);
            strMaxValue.Format(_T("%d"), dwMax);
        }  // if:  signed value
        else
        {
            strMinValue.Format(_T("%u"), dwMin);
            strMaxValue.Format(_T("%u"), dwMax);
        }  // else:  unsigned value
        cchMax = max(strMinValue.GetLength(), strMaxValue.GetLength());
        SendMessage(hwndCtrl, EM_LIMITTEXT, cchMax, 0);

        // Set the value into the control.
        if (bSigned)
        {
            LONG lValue = (LONG) rdwValue;
            DDX_Text(pDX, nIDC, lValue);
        }  // if:  signed value
        else
            DDX_Text(pDX, nIDC, rdwValue);
    }  // else:  setting data onto the dialog

}  //*** DDX_Number()

/////////////////////////////////////////////////////////////////////////////
//++
//
//  DDV_RequiredText
//
//  Routine Description:
//      Validate that the dialog string is present.
//
//  Arguments:
//      pDX         [IN OUT] Data exchange object 
//      nIDC        [IN] Control ID.
//      nIDCLabel   [IN] Label control ID.
//      rstrValue   [IN] Value to set or get.
//
//  Return Value:
//      None.
//
//--
/////////////////////////////////////////////////////////////////////////////
void AFXAPI DDV_RequiredText(
    IN OUT CDataExchange *  pDX,
    IN int                  nIDC,
    IN int                  nIDCLabel,
    IN const CString &      rstrValue
    )
{
    ASSERT(pDX != NULL);

    if (pDX->m_bSaveAndValidate)
    {
        if (rstrValue.GetLength() == 0)
        {
            HWND        hwndLabel;
            TCHAR       szLabel[1024];
            CString     strPrompt;

            // Get the label window handle
            hwndLabel = pDX->PrepareEditCtrl(nIDCLabel);

            // Get the text of the label.
            GetWindowText(hwndLabel, szLabel, sizeof(szLabel) / sizeof(TCHAR));

            // Remove ampersands (&) and colons (:).
            CleanupLabel(szLabel);

            // Format and display a message.
            strPrompt.FormatMessage(ADMC_IDS_REQUIRED_FIELD_EMPTY, szLabel);
            AfxMessageBox(strPrompt, MB_ICONEXCLAMATION);

            // Do this so that the control receives focus.
            (void) pDX->PrepareEditCtrl(nIDC);

            // Fail the call.
            strPrompt.Empty();  // exception prep
            pDX->Fail();
        }  // if:  field not specified
    }  // if:  saving data

}  //*** DDV_RequiredText()

/////////////////////////////////////////////////////////////////////////////
//++
//
//  DDV_Path
//
//  Routine Description:
//      Validate that the path string contains valid characters.
//
//  Arguments:
//      pDX         [IN OUT] Data exchange object 
//      nIDC        [IN] Control ID.
//      nIDCLabel   [IN] Label control ID.
//      rstrValue   [IN] Path to validate.
//
//  Return Value:
//      None.
//
//--
/////////////////////////////////////////////////////////////////////////////
void AFXAPI DDV_Path(
    IN OUT CDataExchange *  pDX,
    IN int                  nIDC,
    IN int                  nIDCLabel,
    IN const CString &      rstrValue
    )
{
    ASSERT(pDX != NULL);

    if (pDX->m_bSaveAndValidate)
    {
        if (!ResUtilIsPathValid(rstrValue))
        {
            HWND        hwndLabel;
            TCHAR       szLabel[1024];
            CString     strPrompt;

            // Get the label window handle
            hwndLabel = pDX->PrepareEditCtrl(nIDCLabel);

            // Get the text of the label.
            GetWindowText(hwndLabel, szLabel, sizeof(szLabel) / sizeof(TCHAR));

            // Remove ampersands (&) and colons (:).
            CleanupLabel(szLabel);

            // Format and display a message.
            strPrompt.FormatMessage(ADMC_IDS_PATH_IS_INVALID, szLabel);
            AfxMessageBox(strPrompt, MB_ICONEXCLAMATION);

            // Do this so that the control receives focus.
            (void) pDX->PrepareEditCtrl(nIDC);

            // Fail the call.
            strPrompt.Empty();  // exception prep
            pDX->Fail();
        }  // if:  path is invalid
    }  // if:  saving data

}  //*** DDV_Path()

/////////////////////////////////////////////////////////////////////////////
//++
//
//  CleanupLabel
//
//  Routine Description:
//      Prepare a label read from a dialog to be used as a string in a
//      message by removing ampersands (&) and colons (:).
//
//  Arguments:
//      pszLabel    [IN OUT] Label to be cleaned up.
//
//  Return Value:
//      None.
//
//--
/////////////////////////////////////////////////////////////////////////////
void CleanupLabel(LPTSTR pszLabel)
{
    LPTSTR  pIn, pOut;
    LANGID  langid;
    WORD    primarylangid;
    BOOL    bFELanguage;

    // Get the language ID.
    langid = GetUserDefaultLangID();
    primarylangid = (WORD) PRIMARYLANGID(langid);
    bFELanguage = ((primarylangid == LANG_JAPANESE)
                    || (primarylangid == LANG_CHINESE)
                    || (primarylangid == LANG_KOREAN));

    //
    // copy the name sans '&' and ':' chars
    //

    pIn = pOut = pszLabel;
    do
    {
        //
        // strip FE accelerators with parentheses. e.g. "foo(&F)" -> "foo"
        //
        if (   bFELanguage
            && (pIn[0] == _T('('))
            && (pIn[1] == _T('&'))
            && (pIn[2] != _T('\0'))
            && (pIn[3] == _T(')')))
        {
            pIn += 3;
        }
        else if ((*pIn != _T('&')) && (*pIn != _T(':')))
            *pOut++ = *pIn;
    } while (*pIn++ != _T('\0')) ;

}  //*** CleanupLabel()
