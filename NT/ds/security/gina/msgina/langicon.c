#include "msgina.h"
#include <shellapi.h>

typedef struct
{
    HKL dwHkl;
    HICON hIcon;

} LAYOUTINFO, *PLAYOUTINFO;

typedef struct
{
    HKL hklLast;
    PLAYOUTINFO pLayoutInfo;
    UINT uNumLayouts;
} USERLAYOUTINFO, *PUSERLAYOUTINFO;

HICON
CreateLangIdIcon(
    WORD LangId);
int
CreateIconList(
    PLAYOUTINFO pLayoutInfo,
    HKL hkl,
    UINT uLangs); 

HICON
GetIconFromHkl(
    PLAYOUTINFO pLayoutInfo,
    HKL hkl,
    UINT uLangs); 


USERLAYOUTINFO UserLayoutInfo[2];

typedef BOOL  (WINAPI *LPFNIMMGETIMEFILENAME)(HKL, LPTSTR, UINT);
LPFNIMMGETIMEFILENAME pfnImmGetImeFileName = NULL;
TCHAR szImm32DLL[] = TEXT("imm32.dll");

typedef UINT (WINAPI *PFNEXTRACTICONEXW)(LPCWSTR lpszFile, int nIconIndex, HICON FAR *phiconLarge, HICON FAR *phiconSmall, UINT nIcons);
 



/***************************************************************************\
* FUNCTION: CreateLangIdIcon
*
* PURPOSE:  Create an icon that displays the first two letters of the 
*           supplied language ID.
*
* RETURNS:  Icon that shows displays Language ID.
*
* HISTORY:
*
*   04-17-98  ShanXu       Borrowed from internat.exe
*
\***************************************************************************/

HICON
CreateLangIdIcon(
    WORD langID
    )
{
    HBITMAP hbmColour = NULL;
    HBITMAP hbmMono;
    HBITMAP hbmOld;
    HICON hicon = NULL;
    ICONINFO ii;
    RECT rc;
    DWORD rgbText;
    DWORD rgbBk = 0;
    HDC hdc = NULL;
    HDC hdcScreen;
    LOGFONT lf;
    HFONT hfont;
    HFONT hfontOld;
    TCHAR szData[20];
    UINT cxSmIcon, cySmIcon;
    

    cxSmIcon =  GetSystemMetrics(SM_CXSMICON);
    cySmIcon =  GetSystemMetrics(SM_CYSMICON);
    //
    //  Get the indicator by using the first 2 characters of the
    //  abbreviated language name.
    //
    if (GetLocaleInfo( MAKELCID(langID, SORT_DEFAULT),
                       LOCALE_SABBREVLANGNAME | LOCALE_NOUSEROVERRIDE,
                       szData,
                       sizeof(szData) / sizeof(szData[0]) ))
    {
        //
        //  Only use the first two characters.
        //
        szData[2] = TEXT('\0');
    }
    else
    {
        //
        //  Id wasn't found.  Use question marks.
        //
        szData[0] = TEXT('?');
        szData[1] = TEXT('?');
        szData[2] = TEXT('\0');
    }

    if (SystemParametersInfo(SPI_GETICONTITLELOGFONT, sizeof(LOGFONT), &lf, 0))
    {
#if (1) //DSIE: Bug 351507
        if (hdcScreen = GetDC(NULL))
        {         
            double dScaleY = GetDeviceCaps(hdcScreen, LOGPIXELSY) / 96.0f;
            lf.lfHeight = (int) (lf.lfHeight * dScaleY); // Scale the height based on the system DPI.
            ReleaseDC(NULL, hdcScreen);
        }
#endif
        if ((hfont = CreateFontIndirect(&lf)))
        {
            hdcScreen = GetDC(NULL);
            if ( hdcScreen )
            {
                hdc = CreateCompatibleDC(hdcScreen);
                hbmColour = CreateCompatibleBitmap(hdcScreen, cxSmIcon, cySmIcon);
                ReleaseDC(NULL, hdcScreen);
            }

            if (hbmColour && hdc)
            {
                hbmMono = CreateBitmap(cxSmIcon, cySmIcon, 1, 1, NULL);
                if (hbmMono)
                {
                    hbmOld    = SelectObject(hdc, hbmColour);
                    rc.left   = 0;
                    rc.top    = 0;
                    rc.right  = cxSmIcon;
                    rc.bottom = cySmIcon;

                    rgbBk = SetBkColor(hdc, GetSysColor(COLOR_HIGHLIGHT));
                    rgbText = SetTextColor(hdc, GetSysColor(COLOR_HIGHLIGHTTEXT));

                    ExtTextOut( hdc,
                                rc.left,
                                rc.top,
                                ETO_OPAQUE,
                                &rc,
                                TEXT(""),
                                0,
                                NULL );
                    SelectObject(hdc, GetStockObject(DEFAULT_GUI_FONT));
                    hfontOld = SelectObject(hdc, hfont);
                    DrawText( hdc,
                              szData,
                              2,
                              &rc,
                              DT_CENTER | DT_VCENTER | DT_SINGLELINE );
#ifdef USE_MIRRORING
                    {
                        DWORD dwLayout;

                        GetProcessDefaultLayout(&dwLayout);
                        if (dwLayout & LAYOUT_RTL)
                        {
                            MirrorBitmapInDC(hdc, hbmColour);
                        }
                    }
#endif
                    SelectObject(hdc, hbmMono);
                    PatBlt(hdc, 0, 0, cxSmIcon, cySmIcon, BLACKNESS);
                    SelectObject(hdc, hbmOld);

                    ii.fIcon    = TRUE;
                    ii.xHotspot = 0;
                    ii.yHotspot = 0;
                    ii.hbmColor = hbmColour;
                    ii.hbmMask  = hbmMono;
                    hicon       = CreateIconIndirect(&ii);

                    DeleteObject(hbmMono);
                    SelectObject(hdc, hfontOld);
                }
                DeleteObject(hbmColour);
                DeleteDC(hdc);
            }
            DeleteObject(hfont);
        }
    }

    return (hicon);
}

/***************************************************************************\
* FUNCTION: CreateIconList
*
* PURPOSE:  Create the table that contains the relationship between an hkl
*           and an icon.
*
* RETURNS:  Index of the current hkl in the table.
*
* HISTORY:
*
*   04-17-98  ShanXu      Created
*
\***************************************************************************/
int
CreateIconList(
    PLAYOUTINFO pLayoutInfo,
    HKL hklCur,
    UINT uLangs)
{
    HKL *pLanguages;
    UINT uCount;
    int nCurIndex = -1;

    pLanguages = (HKL *)LocalAlloc(LPTR, uLangs * sizeof(HKL));
    if (!pLanguages)
    {
        return -1;
    }
    GetKeyboardLayoutList(uLangs, (HKL *)pLanguages);


    for (uCount = 0; uCount < uLangs; uCount++)
    {
        pLayoutInfo[uCount].dwHkl = pLanguages[uCount];
        if (pLanguages[uCount] == hklCur)
        {
            nCurIndex = uCount;
        }
        if ((HIWORD(pLanguages[uCount]) & 0xf000) == 0xe000)
        {
            WCHAR szIMEFile[32];   // assume long filename up to 32 byte

            if (!pfnImmGetImeFileName)
            {
                HMODULE hMod;
                hMod = GetModuleHandle(szImm32DLL);
                if (hMod)
                {
                    pfnImmGetImeFileName = (LPFNIMMGETIMEFILENAME) 
                                            GetProcAddress(
                                                hMod, 
                                                "ImmGetIMEFileNameW");
                }
            }
            if (pfnImmGetImeFileName &&
                (*pfnImmGetImeFileName) (pLanguages[uCount],
                                         szIMEFile,
                                         sizeof(szIMEFile) ))
            {
                HINSTANCE hInstShell32;
                PFNEXTRACTICONEXW pfnExtractIconExW;

                hInstShell32 = LoadLibrary (TEXT("shell32.dll"));

                if (hInstShell32)
                {
                    pfnExtractIconExW = (PFNEXTRACTICONEXW) GetProcAddress (hInstShell32,
                                        "ExtractIconExW");

                    if (pfnExtractIconExW)
                    {

                        //
                        //  First one of the file.
                        //
                        pfnExtractIconExW(
                                szIMEFile,
                                0,
                                NULL,
                                &pLayoutInfo[uCount].hIcon,
                                1);
                    }

                    FreeLibrary (hInstShell32);
                }
                continue;
            }
        }

        //
        // for non-ime layout
        //
        pLayoutInfo[uCount].hIcon = CreateLangIdIcon(LOWORD(pLanguages[uCount]));
        
    }

    LocalFree(pLanguages);

    return nCurIndex;
}

/***************************************************************************\
* FUNCTION: GetIconFromHkl
*
* PURPOSE:  Find the icon in our table that has a matching hkl
*           with the supplied hkl. Create the table if it does not
*           exist.
*
* RETURNS:  Icon of the macthing hkl.
*
* HISTORY:
*
*   04-17-98  ShanXu      Created 
*
\***************************************************************************/
HICON
GetIconFromHkl(
    PLAYOUTINFO pLayoutInfo,
    HKL hkl,
    UINT uLangs)
{
    UINT uCount;
    int nIndex = -1;

    if (pLayoutInfo[0].dwHkl == 0)
    {
        //
        //  Icon/hkl list no exsists yet.  Create it.
        //
        nIndex = CreateIconList(pLayoutInfo, hkl, uLangs);
    }
    else
    {
        //  
        //  Find the icon with a matching hkl
        //
        for (uCount = 0; uCount < uLangs; uCount++)
        {
            if (pLayoutInfo[uCount].dwHkl == hkl)
            {
                nIndex = uCount;
                break;
            }
        }
    }

    if (nIndex == -1)
    {
        return NULL;
    }
    

    return ( pLayoutInfo[nIndex].hIcon);
}

/***************************************************************************\
* FUNCTION: DisplayLanguageIcon
*
* PURPOSE:  Displays the icon of the currently selected hkl in the
*           dlg window.
*
* RETURNS:  TRUE - The icon is displayed.
*           FALSE - No icon displayed.
*
* HISTORY:
*
*   04-17-98  ShanXu      Created
*
\***************************************************************************/
BOOL
DisplayLanguageIcon(
    HWND hwnd,
    LAYOUT_USER LayoutUser,
    HKL  hkl)

{
    HICON hIconLayout;
    UINT uLangs;
    PLAYOUTINFO pLayout;
    
    uLangs = GetKeyboardLayoutList(0, NULL);
    if (uLangs < 2)
    {
        return FALSE;
    }

    pLayout = UserLayoutInfo[LayoutUser].pLayoutInfo;

    if (!pLayout)
    {
        pLayout = (PLAYOUTINFO)LocalAlloc(LPTR, uLangs * sizeof(LAYOUTINFO));

        if (!pLayout)
        {
            return FALSE;
        }

        UserLayoutInfo[LayoutUser].pLayoutInfo = pLayout;
        UserLayoutInfo[LayoutUser].uNumLayouts = uLangs;
    }
        

    hIconLayout = GetIconFromHkl(
                        pLayout, 
                        hkl,    
                        uLangs);

    if (!hIconLayout)
    {
        return FALSE;
    }

    SendMessage( 
        GetDlgItem(hwnd, IDD_KBLAYOUT_ICON),
        STM_SETICON,
        (WPARAM)hIconLayout,
        0 );

    UserLayoutInfo[LayoutUser].hklLast = hkl;

    SetTimer(hwnd, TIMER_MYLANGUAGECHECK, 500, NULL);
    return TRUE;
    
}

/***************************************************************************\
* FUNCTION: FreeLayoutInfo
*
* PURPOSE:  Delete the icon/hkl table and destroy all icons.
*
* RETURNS:  -
*
* HISTORY:
*
*   04-17-98  ShanXu      Created
*
\***************************************************************************/
void
FreeLayoutInfo(
    LAYOUT_USER LayoutUser)
{
    UINT uLangs;
    UINT uCount;

    PLAYOUTINFO pLayoutInfo;

    pLayoutInfo = UserLayoutInfo[LayoutUser].pLayoutInfo;

    if (!pLayoutInfo)
    {
        return;
    }

    uLangs = UserLayoutInfo[LayoutUser].uNumLayouts;
    for (uCount = 0; uCount < uLangs; uCount++)
    {
        DestroyIcon (pLayoutInfo[uCount].hIcon);
    }

    LocalFree(pLayoutInfo);
    UserLayoutInfo[LayoutUser].pLayoutInfo = NULL;
    UserLayoutInfo[LayoutUser].uNumLayouts = 0;

    return;
}

/***************************************************************************\
* FUNCTION: LayoutCheckHandler
*
* PURPOSE:  Handle layout check.  Set appropriate icon if there is
*           a change in keyboard layout.
*
* RETURNS:  -
*
* HISTORY:
*
*   04-22-98  ShanXu      Created
*
\***************************************************************************/
void
LayoutCheckHandler(
    HWND hwnd,
    LAYOUT_USER LayoutUser)
{
    HKL hklCurrent;

    KillTimer(hwnd, TIMER_MYLANGUAGECHECK);

    hklCurrent = GetKeyboardLayout(0);

    if (hklCurrent != UserLayoutInfo[LayoutUser].hklLast)
    {
        DisplayLanguageIcon(
            hwnd,
            LayoutUser,
            hklCurrent);

    }else{
        SetTimer(hwnd, TIMER_MYLANGUAGECHECK, 500, NULL);
    }
}
