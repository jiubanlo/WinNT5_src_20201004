#include "priv.h"
#include "resource.h"
#include "hlframe.h"
#include "bindcb.h"
#include "winlist.h"
#include "iface.h"
#include "shdocfl.h"
#include <optary.h>
#include <mluisupp.h>

#define DM_SHELLEXECOBJECT    0x80000000


// flags for SHDVID_DOCFAMILYCHARSET communication
#define DFC_URLCHARSET  1

#ifdef FEATURE_PICS
#include "dochost.h"    /* for IID_IsPicsBrowser */
#endif

#ifdef DEBUG
extern DWORD g_dwPerf;
#endif

#define DM_HLINKTRACE DM_TRACE

#define DM_WEBCHECKDRT          0
#define JMPMSG(psz, psz2)    TraceMsg(0, "shlf TR-CDOV::%s %s", psz, psz2)
#define JMPMSG2(psz, x)        TraceMsg(0, "shlf TR-CDOV::%s %x", psz, x)
#define DOFMSG(psz)        TraceMsg(0, "shlf TR-DOF::%s", psz)
#define DOFMSG2(psz, x)        TraceMsg(0, "shlf TR-DOF::%s %x", psz, x)
#define URLMSG(psz)        TraceMsg(0, "shlf TR-DOF::%s", psz)
#define URLMSG2(psz, x)        TraceMsg(0, "shlf TR-DOF::%s %x", psz, x)
#define URLMSG3(psz, x, y)    TraceMsg(0, "shlf TR-DOF::%s %x %x", psz, x, y)
#define BSCMSG(psz, i, j)    TraceMsg(0, "shlf TR-BSC::%s %x %x", psz, i, j)
#define BSCMSG3(psz, i, j, k)    TraceMsg(0, "shlf TR-BSC::%s %x %x %x", psz, i, j, k)
#define BSCMSGS(psz, sz)    TraceMsg(0, "shlf TR-BSC::%s %s", psz, sz)
#define OIPSMSG(psz)        TraceMsg(0, "shlf TR-OIPS::%s", psz)
#define OIPSMSG3(psz, sz, p)    TraceMsg(0, "shlf TR-OIPS::%s %s,%x", psz, sz,p)
#define REFMSG0(psz)        TraceMsg(0, "shlf TR-CDOV::%s", psz)
#define REFMSG(psz, cRef)    TraceMsg(0, "shlf TR-CDOV::%s new _cRef==%d", psz, cRef)
#define REFMSG2(psz, if, cRef)    TraceMsg(0, "shlf TR-CDOV::%s(%s) new _cRef==%d", psz, if, cRef)
#define VIEWMSG(psz)        TraceMsg(0, "shlf TR CDOV::%s", psz)
#define VIEWMSG2(psz,xx)    TraceMsg(0, "shlf TR CDOV::%s %x", psz,xx)
#define CACHEMSG(psz, d)        TraceMsg(0, "shlf TR CDocObjectCtx::%s %d", psz, d)
#define HFRMMSG(psz)        TraceMsg(TF_SHDNAVIGATE, "shlf HFRM::%s", psz)
#define HFRMMSG2(psz, x, y)    TraceMsg(TF_SHDNAVIGATE, "shlf HFRM::%s %x %x", psz, x, y)
#define MNKMSG(psz, psz2)    TraceMsg(0, "shlf MNK::%s (%s)", psz, psz2)
#define SERVMSG(psz, x, y)    TraceMsg(0, "shlf SERV::%s %x %x", psz, x, y)

#define KEY_BINDCONTEXTPARAM            _T("BIND_CONTEXT_PARAM")
#define SZ_DWNBINDINFO_OBJECTPARAM      _T("__DWNBINDINFO")

BOOL g_fHlinkDLLLoaded = FALSE;        // must be per-process

STDAPI HlinkFrameNavigate(DWORD grfHLNF, IBindCtx *pbc,
                           IBindStatusCallback *pibsc,
                           IHlink* pihlNavigate,
                           IHlinkBrowseContext *pihlbc);
STDAPI HlinkFrameNavigateNHL(DWORD grfHLNF, IBindCtx *pbc,
                           IBindStatusCallback *pibsc,
                           LPCWSTR pszTargetFrame,
                           LPCWSTR pszUrl,
                           LPCWSTR pszLocation);

// IHlinkFrame members
HRESULT CIEFrameAuto::SetBrowseContext(IHlinkBrowseContext *pihlbc)
{
    if (pihlbc)
        pihlbc->AddRef();

    if (_phlbc)
    {
        if (_dwRegHLBC) 
        {
            _phlbc->Revoke(_dwRegHLBC);
            _dwRegHLBC = 0;
        }
        _phlbc->Release();
    }

    _phlbc = pihlbc;

    return NOERROR;
}

HRESULT CIEFrameAuto::GetBrowseContext(IHlinkBrowseContext **ppihlbc)
{
    TraceMsg(0, "shlf TR ::GetBrowseContext called");
    
    *ppihlbc = _phlbc;

    if (_phlbc) 
    {
        _phlbc->AddRef();
        return S_OK;
    }
    
    return E_FAIL;
}

void CIEFrameAuto::_SetPendingNavigateContext(IBindCtx *pbc, IBindStatusCallback *pibsc)
{
    if (_pbscPending) 
    {
        _pbscPending->Release();
        _pbscPending = NULL;
    }

    if (_pbcPending) 
    {
        _pbcPending->Release();
        _pbcPending = NULL;
    }

    if (pibsc) 
    {
        _pbscPending = pibsc;
        _pbscPending->AddRef();
    }

    if (pbc) 
    {
        // as long as we are cacheing the pending BindCtx, if it specifies
        // a shortcut URL we need to cache that too. (IE:98431)
        IUnknown *          pUnk = NULL;
        IHtmlLoadOptions *  pHtmlLoadOptions  = NULL;

        _pbcPending = pbc;
        _pbcPending->AddRef();

        pbc->GetObjectParam(_T("__HTMLLOADOPTIONS"), &pUnk);
        if (pUnk)
        {
            pUnk->QueryInterface(IID_IHtmlLoadOptions, (void **) &pHtmlLoadOptions);

            if (pHtmlLoadOptions)
            {
                TCHAR    achCacheFile[MAX_PATH+1];
                ULONG    cchCacheFile = ARRAYSIZE(achCacheFile)-1;

                memset(&achCacheFile, 0, (cchCacheFile+1)*sizeof(TCHAR) );
                
                // now determine if this is a shortcut-initiated load
                pHtmlLoadOptions->QueryOption(HTMLLOADOPTION_INETSHORTCUTPATH,
                                                       &achCacheFile,
                                                       &cchCacheFile);

                if (_pwszShortcutPathPending)
                    LocalFree(_pwszShortcutPathPending);

                _pwszShortcutPathPending = StrDup(achCacheFile);

                pHtmlLoadOptions->Release();
            }

            pUnk->Release();
        }

   }
}

//
//  NavigateContext is a set of parameters passed from one CIEFrameAuto
// to another.
//
void CIEFrameAuto::_ActivatePendingNavigateContext()
{
    if (_pbsc) 
    {
        _pbsc->Release();
        _pbsc = NULL;
    }

    if (_pbc) 
    {
        _pbc->Release();
        _pbc = NULL;
    }

    if (_pwszShortcutPath)
    {
        LocalFree(_pwszShortcutPath);
        _pwszShortcutPath = NULL;
    }

    if (_pbscPending) 
    {
        _pbsc = _pbscPending;
        _pbscPending = NULL;
    }

    if (_pbcPending) 
    {
        _pbc = _pbcPending;
        _pbcPending = NULL;
    }

    if (_pwszShortcutPathPending) 
    {
        _pwszShortcutPath = _pwszShortcutPathPending;
        _pwszShortcutPathPending = NULL;
    }
        
}

//  Called to guarantee a newly created HLinkFrame's window is
//  visible after the navigate.
HRESULT ShowHlinkFrameWindow(IUnknown *pUnkTargetHlinkFrame)
{
    IWebBrowserApp* pdie;
    HRESULT hres = pUnkTargetHlinkFrame->QueryInterface(IID_PPV_ARG(IWebBrowserApp, &pdie));
    if (SUCCEEDED(hres)) 
    {
        pdie->put_Visible(TRUE);
        pdie->Release();
    }
    return hres;
}

HRESULT CIEFrameAuto::_NavigateMagnum(DWORD grfHLNF, IBindCtx *pbc, IBindStatusCallback *pibsc, LPCWSTR pszTargetName, LPCWSTR pszUrl, LPCWSTR pszLocation, IHlink *pihlNavigate, IMoniker *pmkTarget)
{
    HRESULT hres = NOERROR;
    HFRMMSG2("Navigate called", grfHLNF, pihlNavigate);
    BOOL fNavigateForReal = pszUrl || (pihlNavigate && (pihlNavigate != (IHlink*)-1));

    _fSuppressHistory = _psbProxy != _psb;  // no history for search band, etc
    _fSuppressSelect = _psbProxy != _psb;   // no need to record select pidl
    if (grfHLNF != (DWORD)-1)
    {
        if (SHHLNF_WRITENOHISTORY & grfHLNF)
        {
            _fSuppressHistory = TRUE;
        }
        if (SHHLNF_NOAUTOSELECT & grfHLNF)
        {
            _fSuppressSelect = TRUE;
        }
    }

    if (pbc == NULL && pibsc == NULL && pihlNavigate == NULL && pszUrl == NULL) 
    {
        //
        //  This is a private interface so that mshtml can do navigation
        // if it is hosted by the shell.  When IHlinkBrowseContext is implemented
        // in the shell this special code can be removed and the associated
        // code in mshtml that calls Navigate with these special parameters
        // can be removed so that it just goes through the
        // IHlinkBrowseContext->SetCurrentHlink interface.
        //
        //  We also use this private mechanism to release the navigation
        // context with grfHLNF==0.
        // 

        switch (grfHLNF&~(SHHLNF_WRITENOHISTORY|SHHLNF_NOAUTOSELECT)) 
        {
        case HLNF_NAVIGATINGBACK:
            hres = _BrowseObject(PIDL_LOCALHISTORY, SBSP_SAMEBROWSER|SBSP_NAVIGATEBACK);
            break;

        case HLNF_NAVIGATINGFORWARD:
            hres = _BrowseObject(PIDL_LOCALHISTORY, SBSP_SAMEBROWSER|SBSP_NAVIGATEFORWARD);
            break;

        case 0:
            _ActivatePendingNavigateContext();
            break;

        default:
            hres = E_INVALIDARG;
            break;
        }

        return hres;
    }

#ifdef FEATURE_PICS
    /* As part of checking ratings, the PICS code will silently download the
     * root document of a site to look for rating labels in it.  If that's a
     * frameset page, Trident will create OCXs for the subframes and try to
     * navigate them, which will invoke ratings checks for them and cause
     * infinite recursion.  So here we check to see if our top-level browser
     * is really this PICS download, and if it is, we don't do any navigation.
     */
    IUnknown *punkPics;
    if (SUCCEEDED(QueryService(SID_STopLevelBrowser, IID_IsPicsBrowser, (void **)&punkPics)))
    {
        punkPics->Release();
        return S_OK;
    }
#endif

    //
    // If we've got this call while we are busy (EnableModeless is FALSE),
    // we should just bail here (instead of doing somthing and let _JumpTo
    // call fail. 
    //
    // This can happen if someone has a window.location="foobar.htm" in their unload
    // event handler.  
    if (fNavigateForReal && !(grfHLNF & HLNF_OPENINNEWWINDOW)) 
    {
        // If _pbs is NULL, it is bad news; we can't navigate.
        // An allowable reason for this condition is that someone has called CIEFrameAuto::Quit()
        // and we are in the process of shutting down.
        //
        if (_pbs == NULL)
        {
            if (_fQuitInProgress)
            {
                TraceMsg(TF_WARNING, "CIEFrameAuto::_NavigateMagnum quitting due to browser closing.");
                return S_OK;
            }
            TraceMsg(TF_WARNING, "CIEFrameAuto::_NavigateMagnum _pbs is NULL, but we are not shutting down.");
            return E_FAIL;
        }

        // If we have a _pbs but the browser says that it can't navigate now, then return S_FALSE.
        //
        else if (_pbs->CanNavigateNow() != S_OK) 
        {
            TraceMsg(TF_WARNING, "CIEFrameAuto::Navigate CanNavigateNow returns non S_OK, bail out.");
            return S_FALSE;
        }
    }

    //
    // This Navigate method is not re-entrant (because of _SetPendingNavigateContext)
    //
    if (_fBusy) 
    {
        TraceMsg(DM_WARNING, "CIEA::Navigate re-entered. Returning E_FAIL");
        return E_FAIL;
    }
    _fBusy = TRUE;

    //
    // HACK: To let Webcheck DRT go.
    //
    if (fNavigateForReal  && !(grfHLNF & HLNF_OPENINNEWWINDOW)) 
    {
        TraceMsg(DM_WEBCHECKDRT, "CIFA::Navigate calling _CancelPendingNavigation");
        VARIANT var = { 0 };
        var.vt = VT_I4;
        var.lVal = TRUE;    // synchronous

        _CancelPendingNavigation(&var);
    }

    if (pszUrl && SHRestricted2(REST_NOFILEURL, NULL, 0) && PathIsFilePath(pszUrl))
    {
        TCHAR szPath[MAX_URL_STRING];
        SHUnicodeToTChar(pszUrl, szPath, ARRAYSIZE(szPath));
        MLShellMessageBox(NULL, MAKEINTRESOURCE(IDS_SHURL_ERR_PARSE_NOTALLOWED),
                        szPath, MB_OK | MB_ICONERROR, szPath);
    
        _fBusy = FALSE;
        return E_ACCESSDENIED;
    }


    _SetPendingNavigateContext(pbc, pibsc);

#ifdef DEBUG
    g_dwPerf = GetCurrentTime();

#endif

    if (pihlNavigate == (IHlink*)-1) 
    {
        //
        // HACK: -1 means "release the navigation state".
        // CDocObjectHost::_CancelPendingNavigation is the only caller.
        // It Exec's SBCMDID_CANCELNAVIGATION which will asynchronously
        // cancel the pending navigation. Therefore, we no longer need
        // to call _CancelPendingNavigation here. (SatoNa)
        // 
        // _CancelPendingNavigation();
    }
    else if (pihlNavigate || pszUrl) 
    {
        hres = S_OK;

        if (SUCCEEDED(hres))
        {
            if ((grfHLNF & HLNF_EXTERNALNAVIGATE) && (grfHLNF & HLNF_NAVIGATINGBACK))
                GoBack();
            else if ((grfHLNF & HLNF_EXTERNALNAVIGATE) && (grfHLNF & HLNF_NAVIGATINGFORWARD))
                GoForward();
            else 
            {
                hres = _JumpTo(pbc,(LPWSTR) pszLocation, grfHLNF, pibsc, pihlNavigate, pszTargetName, pszUrl);
                if (FAILED(hres)) 
                {
                    TraceMsg(DM_ERROR, "IEAuto::Navigate _JumpTo failed %x", hres);
                }
            }
            if (pihlNavigate)
            {
                //
                //  Hopefully, we'll come up with a clean solution to
                //  solve this problem nicely. I made a proposal to NatBro/SriniK
                //  that CreateHlink will CoCreateInstance IHlink so that OLE
                //  LoadLibrary it and maintains it as an InProc server. (SatoNa)
                //
                // HACK: If we AddRef to IHlink, we need to make it sure that
                //  HLINK.DLL is stay loaded even though the DocObject InProc
                //  server (that implicitly links to HLINK.DLL) is unloaded.
                //
                if (!g_fHlinkDLLLoaded) 
                {
                    LoadLibrary(TEXT("hlink.dll"));
                    g_fHlinkDLLLoaded = TRUE;
                }
            }
        }
        else
        {
            TraceMsg(DM_ERROR, "CIEFA::Nav phl->GetMonRef failed %x", hres);
        }
    }

    _fBusy = FALSE;

    HFRMMSG2("Navigate returning", hres, 0);
    
    if (SUCCEEDED(hres) && (pihlNavigate != (IHlink*)-1)) 
    {
        if (grfHLNF & HLNF_EXTERNALNAVIGATE) 
        {
            HWND hwndFrame;
            _psb->GetWindow(&hwndFrame);
            
            if (_phlbc) 
            {
                // if we have a browse context, then we're navigating from it and
                // we should size our window to match it.
                HLBWINFO hlbwi;
                
                hlbwi.cbSize = SIZEOF(hlbwi);
                if (SUCCEEDED(_phlbc->GetBrowseWindowInfo(&hlbwi)) &&
                    (hlbwi.grfHLBWIF & HLBWIF_HASFRAMEWNDINFO)) 
                {
                    WINDOWPLACEMENT wp;
                    
                    wp.length = sizeof(WINDOWPLACEMENT);
                    GetWindowPlacement(hwndFrame, &wp);
                    wp.rcNormalPosition = hlbwi.rcFramePos;
                    wp.showCmd = (hlbwi.grfHLBWIF & HLBWIF_FRAMEWNDMAXIMIZED) 
                                    ? SW_SHOWMAXIMIZED : SW_SHOWNORMAL;

                    // This is not broken in AOL because this
                    // is an external navigate (word has cocreateinstance()d
                    // Internet.Explorer and navigated it.
                    //
                    SetWindowPlacement(hwndFrame, &wp);
                }

                // Register the hlinkframe interface with the browse context, if it has not already
                // been registered
                if (_dwRegHLBC == 0)
                    _phlbc->Register(0, (IHlinkFrame *) this, pmkTarget, &_dwRegHLBC); 

                // add the link to browse context and
                // REVIEW: need to pass the proper friendly name
                _phlbc->OnNavigateHlink(grfHLNF, pmkTarget, pszLocation, NULL, NULL);
            }

            put_Visible(TRUE);
            
            SetForegroundWindow(hwndFrame);
        }

        //
        // According to SriniK, we need to call IHlinkSite::OnNavigationComplete
        // before returning from IHlinkFrame::Navigate with S_OK. (SatoNa)
        //
        if (pihlNavigate) 
        {
            BOOL fExternal = FALSE;
            if (_phlbc && _pbs) 
            {
                ITravelLog* ptl;
                if (SUCCEEDED(_pbs->GetTravelLog(&ptl))) 
                {
                    if (FAILED(ptl->GetTravelEntry(_pbs, 0, NULL))) 
                    {
                        TraceMsg(DM_HLINKTRACE, "CIEFA::_NavMag this is external nav. Don't call OnNavigationComplete");
                        fExternal = TRUE;
                    }
                    else if (SUCCEEDED(ptl->GetTravelEntry(_pbs, TLOG_BACKEXTERNAL, NULL))) 
                    {
                        TraceMsg(DM_HLINKTRACE, "CIEFA::_NavMag this is external for. Don't call OnNavigationComplete");
                        fExternal = TRUE;
                    }
                    ptl->Release();
                }
            }

            //
            // Don't call OnNavigationComplete if this is an external navigation.
            //
            if (!fExternal) 
            {
                IHlinkSite* pihlSite = NULL;
                DWORD dwSiteData;
                HRESULT hresT = pihlNavigate->GetHlinkSite(&pihlSite, &dwSiteData);
                if (SUCCEEDED(hresT) && pihlSite) 
                {
                    TraceMsg(DM_HLINKTRACE, "CIEFA::_NavMag calling OnNavigationComplete");
                    hresT = pihlSite->OnNavigationComplete(dwSiteData, 0, S_OK, L"");
                    if (FAILED(hresT)) 
                    {
                        TraceMsg(DM_ERROR, "CIEFA::Navigat OnNavComplete failed %x", hresT);
                    }
                    pihlSite->Release();
                }
            }
        }
    }

    return hres;
}

//
//  HACK - what we really want is a good private marshalled interface - zekel 8-AUG-97
//  to the Browser.  but for now we will overload the NavigateHack method, 
//  because it is simple and quick for ship.
//
#define HLNF_REFERRERHACK       0x40000000
HRESULT CIEFrameAuto::_ReferrerHack(LPCWSTR pszUrl)
{
    if (_pbs == NULL)    //Make sure we have a IBrowserService.
        return S_FALSE;

    LPITEMIDLIST pidl;

    if (SUCCEEDED(_pbs->IEParseDisplayName(CP_ACP, pszUrl, &pidl)))
    {
        ASSERT(pidl);
        _pbs->SetReferrer(pidl);
        ILFree(pidl);
    }

    return S_OK;
}

HRESULT CIEFrameAuto::NavigateHack(DWORD grfHLNF, IBindCtx *pbc, IBindStatusCallback *pibsc, LPCWSTR pszTargetName, LPCWSTR pszUrl, LPCWSTR pszLocation)
{
    HRESULT     hres = E_FAIL;
    IBindCtx *  pBindCtx = pbc;
    IUnknown *  pNotify = NULL;
    IUnknown *  pBindCtxParam = NULL;
    BOOL        fAsyncCalled = FALSE;
       
    // Check if we are actually a native frame build... 
    if (pbc)
    {
        hres = pbc->GetObjectParam(KEY_BINDCONTEXTPARAM, &pBindCtxParam);
    }
    
    if (SUCCEEDED(hres) && pBindCtxParam)
    {
        // NavigateHack can be called multiple times, and we only want to create the 
        // new bind context the first time. Since the ITargetNotify pointer is removed
        // after the first use, we can check that to make sure.
        // get and transfer the target notify pointer.
        hres = pbc->GetObjectParam(TARGET_NOTIFY_OBJECT_NAME, &pNotify);
        if (SUCCEEDED(hres) && pNotify)
        {
            // The call is coming from a native frame build of MSHTML. 
            // We can not use their bind context, create a new one and transfer
            // parameters.
            // The old bind context is going to be released by the creator, so do not
            // make a release call on it.
            hres = CreateAsyncBindCtxEx(NULL, 0, NULL, NULL, &pBindCtx, 0);
            if(FAILED(hres))
                goto Exit;

            fAsyncCalled = TRUE;

            // carry over the ITargetNotify2 pointer.
            hres = pBindCtx->RegisterObjectParam( TARGET_NOTIFY_OBJECT_NAME, pNotify );
            if (FAILED(hres))
                goto Exit;

            pNotify->Release();
            pNotify = NULL;

            // carry over the bind context parameter.
            hres = pBindCtx->RegisterObjectParam( KEY_BINDCONTEXTPARAM, pBindCtxParam );
            if (FAILED(hres))
                goto Exit;

            {
                IUnknown * pDwnBindInfo = NULL;

                if (SUCCEEDED(pbc->GetObjectParam(SZ_DWNBINDINFO_OBJECTPARAM, &pDwnBindInfo)) && pDwnBindInfo)
                {
                    pBindCtx->RegisterObjectParam(SZ_DWNBINDINFO_OBJECTPARAM, pDwnBindInfo);
                    pDwnBindInfo->Release();
                }
            }
        }

        pBindCtxParam->Release();
        pBindCtxParam = NULL;
    }
    
    if (IsFlagSet(grfHLNF, HLNF_REFERRERHACK))
        hres =  _ReferrerHack(pszUrl);
    else
        hres = _NavigateMagnum(grfHLNF, pBindCtx, pibsc, pszTargetName, pszUrl, pszLocation, NULL, NULL);

Exit:
    SAFERELEASE(pNotify); 
    SAFERELEASE(pBindCtxParam);

    // If the call failed anywhere, we can not be sure the new document
    // will free the object parameter that is in the bind context 
    // we have created in this function.
    if (FAILED(hres) && pBindCtx)
    {
        // we don't want to change the return code here.
        pBindCtx->RevokeObjectParam(KEY_BINDCONTEXTPARAM);
        pBindCtx->RevokeObjectParam(TARGET_NOTIFY_OBJECT_NAME);
    }
    
    if (fAsyncCalled)
        pBindCtx->Release();

    return hres;
}

// passing NULL pibsc and pbc will make be like "ReleaseNavigationState"
// passing -1 for pihlNavigate will cancel pending navigation

HRESULT CIEFrameAuto::Navigate(DWORD grfHLNF, IBindCtx *pbc,
     IBindStatusCallback *pibsc, IHlink *pihlNavigate)
{
    IMoniker* pmkTarget = NULL;
    LPOLESTR pwszDisplayName = NULL;
    LPOLESTR pwszLocation = NULL;
    LPOLESTR pwszFrameName = NULL;
    HRESULT hres = S_OK;

    if (pihlNavigate && ((IHlink *)-1) != pihlNavigate)
    {
        pihlNavigate->GetTargetFrameName(&pwszFrameName);

        //
        // Note that we are discarding "relative" portion.
        //
        hres = pihlNavigate->GetMonikerReference(HLINKGETREF_ABSOLUTE, &pmkTarget, &pwszLocation);

        HFRMMSG2("Navigate pihl->GetMonRef returned", hres, pmkTarget);

        if (SUCCEEDED(hres))
        {
            IBindCtx* pbcLocal;
    
            if (pbc) 
            {
                pbcLocal = pbc;
                pbcLocal->AddRef();
            }
            else 
            {
                hres = CreateBindCtx(0, &pbcLocal);
            }

            if (SUCCEEDED(hres))
            {
                hres = pmkTarget->GetDisplayName(pbcLocal, NULL, &pwszDisplayName);
                pbcLocal->Release();
            }
        }
    }

    if (SUCCEEDED(hres))
    {
        hres = _NavigateMagnum(grfHLNF, pbc, pibsc, pwszFrameName, pwszDisplayName, pwszLocation, pihlNavigate, pmkTarget);
    }
    if (pwszFrameName)
    {
        OleFree(pwszFrameName);
    }
    if (pwszDisplayName)
    {
        OleFree(pwszDisplayName);
    }
    if (pwszLocation)
    {
        OleFree(pwszLocation);
    }
    if (pmkTarget)
    {
        pmkTarget->Release();
    }
    return hres;
}

HRESULT CIEFrameAuto::OnNavigate(DWORD grfHLNF,
            /* [unique][in] */ IMoniker *pimkTarget,
            /* [unique][in] */ LPCWSTR pwzLocation,
            /* [unique][in] */ LPCWSTR pwzFriendlyName,
            /* [in] */ DWORD dwreserved)
{
    TraceMsg(0, "shlf TR ::OnNavigate called");
    return S_OK;
}

void CIEFrameAuto::_CancelPendingNavigation(VARIANTARG* pvar)
{
    TraceMsg(0, "shd TR _CancelPendingNavigation called");
    if (_pmsc) 
    {
        TraceMsg(0, "shd TR _CancelPendingNavigation calling _pmsc->Exec");
        _pmsc->Exec(&CGID_Explorer, SBCMDID_CANCELNAVIGATION, 0, pvar, NULL);
    }
}

// *** ITargetNotify ***

void 
CIEFrameAuto::_HandleOpenOptions( IUnknown * pUnkDestination, ITargetNotify * ptgnNotify)
{
    HRESULT             hres = S_OK;
    ITargetNotify2 *    ptgnNotify2 = NULL; 

    if (!pUnkDestination || !ptgnNotify)
        return;

    if (SUCCEEDED(ptgnNotify->QueryInterface( IID_ITargetNotify2, (void **)&ptgnNotify2)))
    {
        BSTR    bstrOptions = NULL;

        ASSERT(ptgnNotify2);

        // Apply the options only if the initator of the navigation 
        // asks for it.

        if (S_OK == ptgnNotify2->GetOptionString(&bstrOptions))
        {
            _omwin._OpenOptions.ReInitialize();

            if (bstrOptions)
            {
                _omwin._ParseOptionString(bstrOptions, ptgnNotify2);

                // We are done with the options string, release it
                SysFreeString(bstrOptions);
            }

            // Apply the options now.
            //
            IWebBrowser2 * pNewIE;

            if (SUCCEEDED(pUnkDestination->QueryInterface(IID_PPV_ARG(IWebBrowser2, &pNewIE))))
            {
                _omwin._ApplyOpenOptions(pNewIE);
                pNewIE->Release();
            }
        }

        ptgnNotify2->Release();
    
    }
}

HRESULT CIEFrameAuto::OnCreate(IUnknown *pUnkDestination, ULONG cbCookie)
{
    HRESULT             hres = S_OK;

    if (cbCookie == (ULONG)_cbCookie && _ptgnNotify)
    {
        _HandleOpenOptions( pUnkDestination, _ptgnNotify);
    
        hres = _ptgnNotify->OnCreate(pUnkDestination, cbCookie);
        SAFERELEASE(_ptgnNotify);
    }
    return hres;
}

HRESULT CIEFrameAuto::OnReuse(IUnknown *pUnkDestination)
{
    return S_OK;
}

#define NOTIFY_WAIT_TIMEOUT (60000)
//  chrisfra 10/10/96: do we need EnableModeless(FALSE)/(TRUE) around 
//  our little loop, or is the busy flag (which is set) sufficient?

HRESULT CIEFrameAuto::_WaitForNotify()
{
    if (_ptgnNotify && IsInternetExplorerApp())
    {
        DWORD dwObject, msWait, msStart = GetTickCount();

        goto DOPEEK;

        while (_ptgnNotify)
        {
            // NB We need to let the run dialog become active so we have to half handle sent
            // messages but we don't want to handle any input events or we'll swallow the
            // type-ahead.
            msWait = GetTickCount();
            if (msWait - msStart > NOTIFY_WAIT_TIMEOUT) 
                break;

            msWait = NOTIFY_WAIT_TIMEOUT - (msWait - msStart);
            dwObject = MsgWaitForMultipleObjects(0, NULL, FALSE, msWait, QS_ALLINPUT);
            // Are we done waiting?
            switch (dwObject) 
            {
            case WAIT_FAILED:
                break;
                
            case WAIT_OBJECT_0:
DOPEEK:
                // got a message, dispatch it and wait again
                MSG msg;
                while (PeekMessage(&msg, NULL,0, 0, PM_REMOVE)) 
                {
                    DispatchMessage(&msg);
                    if (_ptgnNotify == NULL || 
                        ((GetTickCount() - msStart) > NOTIFY_WAIT_TIMEOUT)) 
                        break;
                    
                }
                break;
            }
        }
    }
    return S_OK;
}

HRESULT CIEFrameAuto::_RegisterCallback(TCHAR *szFrameName, ITargetNotify *ptgnNotify)
{
    HRESULT hr = S_OK;

    SAFERELEASE(_ptgnNotify);

    _fRegistered = 0;

    if (ptgnNotify)
    {
        IDispatch *pid;
        hr = QueryInterface(IID_PPV_ARG(IDispatch, &pid));
        if (SUCCEEDED(hr))
        {
            hr = E_FAIL;
            IShellWindows *psw = WinList_GetShellWindows(TRUE);
            if (psw != NULL)
            {
                long cbCookie;
                hr = psw->Register(pid, NULL, SWC_CALLBACK, &cbCookie);
                if (SUCCEEDED(hr))
                {
                    TCHAR szCookie[25];   // big enough for "_[cbCookie]"
                    int slenCookie;
                    int slenName;
                    int slenMin;

                    _cbCookie = cbCookie;
                    _fRegistered = 1;
                    _ptgnNotify = ptgnNotify;
                    _ptgnNotify->AddRef();

                    //  prepend unique id to target -- tells created WebBrowserOC to
                    //  register the remainder (if any) as frame name and to perform
                    //  callbacks on all registered callbacks
                    wnsprintf(szCookie, ARRAYSIZE(szCookie), TEXT("_[%ld]"), cbCookie);
                    slenCookie = lstrlen(szCookie);
                    slenName = lstrlen(szFrameName);
                    slenMin =  min((int)MAX_URL_STRING-slenCookie,slenName);
                    MoveMemory(&szFrameName[slenCookie], szFrameName, CbFromCch(slenMin));
                    szFrameName[slenCookie+slenMin] = 0;
                    CopyMemory(szFrameName, szCookie, CbFromCch(slenCookie));
                }
                psw->Release();
            }
            pid->Release();
        }
    }

    return hr;
}


HRESULT CIEFrameAuto::_RevokeCallback()
{
    HRESULT hr = S_OK;

    if (_fRegistered)
    {
        IShellWindows *psw = WinList_GetShellWindows(TRUE);
        if (psw != NULL)
        {
            hr = psw->Revoke(_cbCookie);
            psw->Release();
        }
    }
    SAFERELEASE(_ptgnNotify);
    _fRegistered = 0;
    return hr;
}


//
//  HACK - what we really want is a good private marshalled interface - zekel 8-AUG-97
//  to the Browser.  but for now we will overload the NavigateHack method, 
//  because it is simple and quick for ship.
//
void CIEFrameAuto::_SetReferrer(ITargetFramePriv *ptgfp)
{
    LPITEMIDLIST pidl;
    WCHAR szUrl[MAX_URL_STRING];

    ASSERT(ptgfp);

    //Make sure we have a IBrowserService.
    if (_psb && SUCCEEDED(_pbs->GetPidl(&pidl)))
    {
        if (SUCCEEDED(_pbs->IEGetDisplayName(pidl, szUrl, SHGDN_FORPARSING)))
            ptgfp->NavigateHack(HLNF_REFERRERHACK, NULL, NULL, NULL, szUrl, NULL);

        ILFree(pidl);
    }
}

BOOL _ShouldInvokeDefaultBrowserOnNewWindow(IUnknown *punk)
{
    BOOL fResult = FALSE;

    IOleCommandTarget *poct;

    if (SUCCEEDED(IUnknown_QueryService(punk, SID_STopLevelBrowser, IID_PPV_ARG(IOleCommandTarget, &poct))))
    {
        VARIANT var;

        var.vt = VT_EMPTY;

        if (SUCCEEDED(poct->Exec(&CGID_InternetExplorer, 
                                 IECMDID_GET_INVOKE_DEFAULT_BROWSER_ON_NEW_WINDOW,
                                 0,
                                 NULL,
                                 &var)))
        {
            fResult = var.boolVal ? TRUE : FALSE;
        }

        poct->Release();
    }
    
    return fResult;
}

HRESULT CIEFrameAuto::_JumpTo(IBindCtx *pbc, LPWSTR pszLocation, DWORD grfHLNF, IBindStatusCallback *pibsc, IHlink *pihlNavigate, LPCWSTR pszFrameName, LPCWSTR pszUrl)
{
    LPITEMIDLIST pidl = NULL;
    HRESULT hres;
    ITargetNotify *ptgnNotify = NULL;
    IUnknown *punkNotify = NULL;
    IUnknown *punkThis = NULL;
    UINT uiCP = CP_ACP;

    // Get the current document codepage from Trident and use it for url string conversion if necessary.
    if (!(grfHLNF & HLNF_ALLOW_AUTONAVIGATE) && _pmsc)
    {
        VARIANT varOut = { 0 };
        VARIANT varIn = { 0 };

        varIn.vt = VT_I4;
        varIn.lVal = DFC_URLCHARSET; // we want the doc's url charset

        if (SUCCEEDED(_pmsc->Exec(&CGID_ShellDocView, SHDVID_DOCFAMILYCHARSET, 0, &varIn, &varOut)))
            uiCP = (UINT)varOut.lVal;
    }

    //  Note that we are simply passing the pidl to ISB::BrowseObject,
    // assuming that new shell32.dll allows us to bind to DocObject
    // documents.
    //

    DWORD flags = (grfHLNF & HLNF_OPENINNEWWINDOW) ?
        (SBSP_NEWBROWSER | SBSP_ABSOLUTE | SBSP_INITIATEDBYHLINKFRAME) :
        (SBSP_SAMEBROWSER | SBSP_ABSOLUTE | SBSP_INITIATEDBYHLINKFRAME);


    flags |= ((grfHLNF & HLNF_ALLOW_AUTONAVIGATE) ? (SBSP_ALLOW_AUTONAVIGATE) : 0);
    flags |= ((grfHLNF & SHHLNF_WRITENOHISTORY) ? (SBSP_WRITENOHISTORY) : 0);
    flags |= ((grfHLNF & SHHLNF_NOAUTOSELECT) ? (SBSP_NOAUTOSELECT) : 0);

    if (pbc && SUCCEEDED(pbc->GetObjectParam(TARGET_NOTIFY_OBJECT_NAME, &punkNotify)))
    {
        if (FAILED(punkNotify->QueryInterface(IID_PPV_ARG(ITargetNotify, &ptgnNotify))))
            ptgnNotify = NULL;
        
        punkNotify->Release();
        QueryInterface(IID_PPV_ARG(IUnknown, &punkThis));
    }

    if (grfHLNF & HLNF_CREATENOHISTORY)
        flags |= SBSP_REDIRECT;

    if (flags & SBSP_NEWBROWSER)
    {
        TCHAR *pszHeaders = NULL;
        BYTE *pPostData = NULL;
        DWORD cbPostData = 0;
        TCHAR szFrameName[MAX_URL_STRING+1];
        STGMEDIUM stgPostData = { TYMED_NULL, NULL, NULL };

        //Qfe:1478 If restricted to open in new window, return failure.
        if ((grfHLNF & HLNF_OPENINNEWWINDOW) 
            && SHIsRestricted2W(_hwnd, REST_NoOpeninNewWnd, NULL, 0))
        {
            SAFERELEASE(punkThis);
            return E_ACCESSDENIED;
        }

        if ((_ShouldInvokeDefaultBrowserOnNewWindow(_psb) || IsDesktopFrame(_psb)) && !ShouldNavigateInIE(pszUrl))
        {
            //  IE is not the default browser so we'll ShellExecute the Url since someone
            //  has told us that's the behaviour they prefer.
            HINSTANCE hinstRet = ShellExecuteW(NULL, NULL, pszUrl, NULL, NULL, SW_SHOWNORMAL);
            
            hres = ((UINT_PTR)hinstRet) <= 32 ? E_FAIL : S_OK;
        }
        else
        {

            szFrameName[0] = 0;

            //  Here is where if we are doing a new window we must
            //  extract frame, post etc and append to pidl.  These must
            //  be done in the following order (to match extraction code):
            //      URLID_FRAMENAME,URLID_POSTDATA,URLID_HEADERS

            if (pszFrameName)
            {
                SHUnicodeToTChar(pszFrameName, szFrameName, ARRAYSIZE(szFrameName));
            }


            if (pibsc)
            {
                GetHeadersAndPostData(pibsc,&pszHeaders,&stgPostData,&cbPostData, NULL);

                if (stgPostData.tymed == TYMED_HGLOBAL) 
                {
                    pPostData = (LPBYTE) stgPostData.hGlobal;
                }
            }

            hres = _PidlFromUrlEtc(uiCP, pszUrl, pszLocation, &pidl);

            HFRMMSG2("_JumpTo _PidlFromUrlEtc returned", hres, pidl);

            if (SUCCEEDED(hres))
            {
                IUnknown* punkNewWindow = NULL;
                BOOL fCancel = FALSE;

                // The NewWindow2 event may return the window for us.
                FireEvent_NewWindow2(_GetOuter(), &punkNewWindow, &fCancel);
                if (!fCancel)
                {
                    BOOL fProcessed = FALSE;

                    // We might need the old NewWindow event...
                    if (!punkNewWindow)
                    {
                        _RegisterCallback(szFrameName, ptgnNotify);
            
                        // fire an event to indicate a new window needs to be created
                        // to allow a container to handle it itself if it wants
                        // since we may be aggregated, QI our parent

                        // Yet another Compuserve workaround (IE 60688):
                        // If the target frame name is "_blank", Compuserve will pass that name
                        // in to the Navigate call of the new window.  We would then create a new window
                        // (which would fire this event) causing a loop.  Break the recursion by sending
                        // an empty string for the frame name.
                        HWND hwnd = _GetHWND();
                        
                        if (hwnd)
                        {
                            FireEvent_NewWindow(_GetOuter(), hwnd, pidl,pszLocation,0,
                                StrCmpI(szFrameName, TEXT("_blank")) ? szFrameName : TEXT(""),  // Target frame name
                                pPostData,cbPostData,pszHeaders,&fProcessed);
                        }
                    }
        
                    if (!fProcessed)
                    {
                        if (!punkNewWindow)
                        {
    #ifdef INCLUDE_BUSTED_OC_QI
                            IUnknown* pdvb = NULL;
    #endif
                            _RevokeCallback();

    #ifdef INCLUDE_BUSTED_OC_QI
                            // For some unidentifiable reason the old code did NOT
                            // create a new window if we were hosted in the WebBrowserOC.
                            // mikesh/cheechew/jeremys/chrisfra don't know why this happens.
                            // Who knows what app will break if we change this...
                            // (Note: IDefViewBrowser is a CWebBrowseSB only interface)
                            //
                            // NOTE: chrisfra 3/11/97, this code breaks open a
                            // new window for a non-existent target, when in
                            // desktop component or browser band
                            fCancel = !(_psbTop && FAILED(_psbTop->QueryInterface(IID_PPV_ARG(IDefViewBrowser, &pdvb))));
                            if (pdvb)
                                pdvb->Release();
    #endif
                        }
        
                        // what we really want to do is just hand this off to
                        // _psbTop->BrowseObject and let it (CWebBrowserSB or CShellBrowser)
                        // decide whether to use HlinkFrameNavigate or not, but if we
                        // do that, then we lose the grfHLNF and pihlNavigate.
                        // So put that logic here...
                        //
                        if (!fCancel)
                        {
                            hres = CreateTargetFrame(pszFrameName, &punkNewWindow);
                            if (SUCCEEDED(hres))
                            {
                                //  Notify ptgnNotify, then release and remove from bindctx
                                if (ptgnNotify)
                                {
                                    _HandleOpenOptions( punkNewWindow, ptgnNotify);

                                    ptgnNotify->OnCreate(punkNewWindow, GetTickCount());

                                    ptgnNotify->Release();
                                    ptgnNotify = NULL;

                                    pbc->RevokeObjectParam(TARGET_NOTIFY_OBJECT_NAME);
                                }

                                LPHLINKFRAME phf;

                                hres = punkNewWindow->QueryInterface(IID_PPV_ARG(IHlinkFrame, &phf));
                                if (SUCCEEDED(hres))
                                {
                                    ITargetFramePriv * ptgfp;

                                    if (NULL == pihlNavigate)
                                    {
                                        hres = punkNewWindow->QueryInterface(IID_PPV_ARG(ITargetFramePriv, &ptgfp));
                                    }

                                    if (SUCCEEDED(hres))
                                    {
                                        if (pihlNavigate)
                                        {
                                            hres = phf->Navigate(grfHLNF & ~HLNF_OPENINNEWWINDOW, 
                                                                 pbc, 
                                                                 pibsc, 
                                                                 pihlNavigate);
                                        }
                                        else
                                        {
                                            // HACK - see this methods comments 
                                            _SetReferrer(ptgfp);

                                            hres = ptgfp->NavigateHack(grfHLNF & ~HLNF_OPENINNEWWINDOW, 
                                                                 pbc, 
                                                                 pibsc,
                                                                 NULL,
                                                                 pszUrl,
                                                                 pszLocation);
                                        }
        
                                        if (FAILED(hres)) 
                                        {
                                            TraceMsg(DM_ERROR, "CIEFA::_JumpTo marshalled IHlinkFrame::Navigate failed %x", hres);
                                        }

                                        ShowHlinkFrameWindow(punkNewWindow);
                                        if (NULL == pihlNavigate)
                                        {
                                            ptgfp->Release();
                                        }

                                        if(SUCCEEDED(hres) && pibsc)
                                        {
                                            _SetPendingNavigateContext(NULL, NULL);
                                        }

                                    }
                                    phf->Release();
                                }
                            }
                        }
                        else 
                        {
                            //
                            //  If NEWBROWSER is specified when there is no top level
                            // browser, we should ask IE/Shell to do browsing.
                            // We don't pass HLNF_OPENINNEWWINDOW in this case.
                            //
                            
                            //  Notify object doing navigation that we are the object implementing IWebBrowserApp
                            if (ptgnNotify) ptgnNotify->OnReuse(punkThis);
        
                            if (pihlNavigate)
                            {
                                hres = HlinkFrameNavigate(grfHLNF & ~HLNF_OPENINNEWWINDOW,
                                                            NULL, NULL, pihlNavigate, NULL);
                            }
                            else
                            {
                                hres = HlinkFrameNavigateNHL(grfHLNF & ~HLNF_OPENINNEWWINDOW,
                                           NULL, NULL, NULL, pszUrl, pszLocation);
                            }
                        }
                    }
                    else
                    {
                        //  Oldstyle AOL or other 3rd Party, wait for registration of 
                        //  WebBrowserOC, which calls us back on _ptgnNotify
                        _WaitForNotify();
                        //  We timed out the window create, notify caller
                        if (_ptgnNotify) 
                            _ptgnNotify->OnCreate(NULL, 0);
                        _RevokeCallback();
                    }
                }

                if (punkNewWindow)
                    punkNewWindow->Release();

            }
            else
            {
                TraceMsg(DM_ERROR, "IEAuto::_JumpTo _PidlFromUrlEtc (1) failed %x", hres);
            }
        }
        if (pszHeaders) 
        {
            LocalFree(pszHeaders);
            pszHeaders = NULL;
        }

        if (stgPostData.tymed != TYMED_NULL)
        {
            ReleaseStgMedium(&stgPostData);
        }

    }
    else
    {
        //  Notify object doing navigation that we are the object implementing IWebBrowserApp
        if (ptgnNotify) ptgnNotify->OnReuse(punkThis);

        hres = _PidlFromUrlEtc(uiCP, pszUrl, pszLocation, &pidl);
        if (SUCCEEDED(hres))
        {
            hres = _psb->BrowseObject(pidl, flags);
        }
        else 
        {
            TraceMsg(DM_ERROR, "IEAuto::_JumpTo _PidlFromUrlEtc (2) failed %x", hres);
        }
    }

    if (pidl)
    {
        HFRMMSG2("_JumpTo _psb->BrowseObject returned", hres, 0);
        ILFree(pidl);
    }
    
    if (ptgnNotify)
    {
        ptgnNotify->Release();
        pbc->RevokeObjectParam(TARGET_NOTIFY_OBJECT_NAME);
    }

    SAFERELEASE(punkThis);

    return hres;
}


HRESULT CIEFrameAuto::QueryService(REFGUID guidService, REFIID riid, void ** ppvObj)
{
    *ppvObj = NULL;

    // WARNING: Note that we are not following the strict semantics of
    //  ISP::QueryService. It is, however, OK because this (the fact that
    //  IHlinkFrame support IServiceProvider) is not public.

    if (IsEqualIID(guidService, SID_SOmWindow)) 
    {
        return _omwin.QueryInterface(riid, ppvObj);
    }
    else if (IsEqualIID(guidService, IID_IHlinkFrame)) 
    {
        SERVMSG("QueryService called", _pbc, _pbsc);

        if (IsEqualIID(riid, IID_IBindCtx) && _pbc) 
        {
            *ppvObj = _pbc;
            _pbc->AddRef();
        }
        else if (IsEqualIID(riid, IID_IBindStatusCallback) && _pbsc)
        {
            *ppvObj = _pbsc;
            _pbsc->AddRef();
        }         
        else
        {
            return QueryInterface(riid, ppvObj);
        }
    }
    else if (IsEqualIID(guidService, SID_PendingBindStatusCallback)) 
    {
        if (IsEqualIID(riid, IID_IBindStatusCallback) && _pbscPending)
        {
            *ppvObj = _pbscPending;
            _pbscPending->AddRef();
        }
    } 
    else if (_psp) 
    {
        return _psp->QueryService(guidService, riid, ppvObj);
    }

    return *ppvObj ? S_OK : E_FAIL;
}


HRESULT CIEFrameAuto::Exec(
    /* [unique][in] */ const GUID *pguidCmdGroup,
    /* [in] */ DWORD nCmdID,
    /* [in] */ DWORD nCmdexecopt,
    /* [unique][in] */ VARIANTARG *pvarargIn,
    /* [unique][out][in] */ VARIANTARG *pvarargOut)
{
    HRESULT hres = S_OK;

    if (pguidCmdGroup)
    {
        if (IsEqualGUID(CGID_Explorer, *pguidCmdGroup))
        {
            switch(nCmdID)
            {
            case SBCMDID_CANCELNAVIGATION:
                _CancelPendingNavigation(NULL);
                break;

            case SBCMDID_SELECTHISTPIDL:
            case SBCMDID_HISTSFOLDER:
                if (_poctFrameTop)
                    hres = _poctFrameTop->Exec(pguidCmdGroup, nCmdID, nCmdexecopt, pvarargIn, pvarargOut);
                else
                    hres = S_OK;
                break;

            case SBCMDID_IESHORTCUT:
#ifdef BROWSENEWPROCESS_STRICT // "Nav in new process" has become "Launch in new process", so this is no longer needed
                // If this is an IE shortcut and browse in a new process is turned on
                // and we are explorer.exe - we should pass on the request to navigate to
                // this shortcut. The caller is expected to create a new window/process to
                // launch this shortcut
                if (IsBrowseNewProcessAndExplorer())  
                    hres = E_FAIL;
                else
#endif
                    hres = _NavIEShortcut(pvarargIn,pvarargOut);
                
                break;

            case SBCMDID_GETSHORTCUTPATH:
               if (_pwszShortcutPath && pvarargOut)
               {
                    pvarargOut->bstrVal = SysAllocString(_pwszShortcutPath);
                    if (pvarargOut->bstrVal)
                        pvarargOut->vt = VT_BSTR;        //no need to set hres=S_OK since it is inited already
                    else 
                        hres = E_OUTOFMEMORY;
               }
               else 
               {
                    if (pvarargOut)
                        pvarargOut->vt = VT_EMPTY;

                    hres = E_FAIL;
               }
               
               break;    
            default:
                hres = OLECMDERR_E_NOTSUPPORTED; 
            }
        }
        else if (IsEqualGUID(CGID_ShortCut, *pguidCmdGroup))
        {
            if (_poctFrameTop) // we must check!
                hres = _poctFrameTop->Exec(&CGID_ShortCut, nCmdID, nCmdexecopt, pvarargIn, pvarargOut);
            else
                hres = OLECMDERR_E_NOTSUPPORTED;
        } 
        else if (IsEqualGUID(CGID_ShellDocView, *pguidCmdGroup))
        {
            switch (nCmdID)
            {
                case SHDVID_DELEGATEWINDOWOM:
                    _omwin.SetDelegationPolicy(V_BOOL(pvarargIn));
                    break;
                default:
                    hres = OLECMDERR_E_NOTSUPPORTED;
            }
        }
        else if (IsEqualGUID(CGID_InternetExplorer, *pguidCmdGroup))
        // CGID_InternetExplorer are public defined in msiehost.h
        {
            switch (nCmdID)
            {
                case IECMDID_CLEAR_AUTOCOMPLETE_FOR_FORMS:
                {
                    if (pvarargIn->vt == VT_I4)
                    {
                        hres = ClearAutoSuggestForForms(V_I4(pvarargIn));
                    }
                    else
                        hres = E_INVALIDARG;
                }
                break;

                case IECMDID_SETID_AUTOCOMPLETE_FOR_FORMS:
                {
                    if ((pvarargIn->vt == VT_UI8) ||
                        (pvarargIn->vt == VT_I8))
                    {
                        hres = SetIdAutoSuggestForForms(((GUID *)(&pvarargIn->ullVal)), _omwin.IntelliForms());
                    }
                    else
                        hres = E_INVALIDARG;
                }
                break;

                default:
                    hres = OLECMDERR_E_NOTSUPPORTED;
            }
        }
        else
        {
            hres = OLECMDERR_E_UNKNOWNGROUP;
        }
    }
    else
    {
        hres = OLECMDERR_E_UNKNOWNGROUP;
    }

    return hres;
}


BOOL CIEFrameAuto::_fNavigationPending()
{
    // unfortunately, the hyperlink frame doesn't REALLY know when there's
    // a navigation pending or not because people might not call OnReleaseNavigation.
    // only the real browser knows.

    if (_pmsc) 
    {
        MSOCMD rgCmd;
        rgCmd.cmdID = SBCMDID_CANCELNAVIGATION;
        rgCmd.cmdf = 0;

        _pmsc->QueryStatus(&CGID_Explorer, 1, &rgCmd, NULL);
        return (rgCmd.cmdf & MSOCMDF_ENABLED);
    }
    return FALSE;
}

HRESULT CIEFrameAuto::QueryStatus(const GUID *pguidCmdGroup,
    ULONG cCmds, OLECMD rgCmds[], OLECMDTEXT *pcmdtext)
{
    if (pguidCmdGroup && IsEqualGUID(CGID_Explorer, *pguidCmdGroup)) 
    {
        for (ULONG i = 0; i < cCmds; i++)
        {
            switch (rgCmds[i].cmdID)
            {
            case SBCMDID_CANCELNAVIGATION:
                rgCmds[i].cmdf = _fNavigationPending() ? MSOCMDF_ENABLED : 0;
                break;

            case SBCMDID_WRITEHIST:
                rgCmds[i].cmdf = _fSuppressHistory ? 0:MSOCMDF_ENABLED;
                break;
            
            case SBCMDID_SELECTHISTPIDL:
                rgCmds[i].cmdf = _fSuppressSelect || !_poctFrameTop ? 0:MSOCMDF_ENABLED;
                break;

            default:
                rgCmds[i].cmdf = 0;
                break;
            }
        }
    }
    else
    {
        return OLECMDERR_E_UNKNOWNGROUP;
    }

    if (pcmdtext)
    {
        pcmdtext->cmdtextf = MSOCMDTEXTF_NONE;
        pcmdtext->cwActual = 0;
    }

    return NOERROR;
}

HRESULT CIEFrameAuto::_PidlFromUrlEtc(UINT uiCP, LPCWSTR pszUrl, LPWSTR pszLocation, LPITEMIDLIST* ppidl)
{
    *ppidl = NULL;      // assumes error

    // ALGORITHM:
    //  - First, we call IEParseDisplayName to generate the pidl
    //    to the specified URL or file name.
    //  - if we have fragment (pszLocation) specified,
    //    we call IEILAppendFragment() to add the hidden fragment id
    if (_pbs == NULL)  //Make sure we have a IBrowserService.
        return (S_FALSE);
        
    HRESULT hr = _pbs->IEParseDisplayName(uiCP, pszUrl, ppidl);

    // This is ugly, if it's a file path that failed to parse because
    // it doesn't exist, we want to create a SimpleIDList so we display
    // a res: navigation failed IFrame instead of the err dlg displayed
    // in DisplayParseError() below.
    if (FAILED(hr)) 
    {
        TCHAR szPath[MAX_PATH];
        DWORD cchBuf = ARRAYSIZE(szPath);

        // If it's a FILE URL, convert it to a path.
        if (IsFileUrlW(pszUrl) && SUCCEEDED(PathCreateFromUrl(pszUrl, szPath, &cchBuf, 0)))
        {
            // That worked, we are done because our buffer is now full.
        }
        else        
        {
            // We now need to copy to the buffer and we assume it's a path.
            StrCpyN(szPath, pszUrl, ARRAYSIZE(szPath));
        }

        *ppidl = SHSimpleIDListFromPath(szPath);
        if (*ppidl)
            hr = S_OK;
    }

    if (SUCCEEDED(hr))
    {
        if (pszLocation && *pszLocation)
        {
            *ppidl = IEILAppendFragment(*ppidl, pszLocation);
            hr = *ppidl ? S_OK : E_OUTOFMEMORY;
        }
    } 
    else 
    {
        //
        // NOTES: This behavior is new in IE4.0. We are adding
        //  this message box based on the request (bug-report)
        //  from Office guys. (SatoNa)
        //
        hr = _pbs->DisplayParseError(hr, pszUrl);
    }
    return hr;
}

HRESULT CIEFrameAuto::_NavIEShortcut(VARIANT *pvarIn, VARIANT *pvarargOut)
{
    //  need to validate verb and clsid
    HRESULT hr = E_ACCESSDENIED;
    READYSTATE ready;
    BOOL fForceNavigate = pvarargOut ? ((VT_BOOL == pvarargOut->vt ) && (pvarargOut->boolVal)) : FALSE;
    
    get_ReadyState(&ready);

    ASSERT(pvarIn);
    ASSERT(pvarIn->vt == VT_BSTR);
    //
    //  we dont want to allow the exec to go through if this window
    //  is busy with something else.  we should probably allow
    //  READYSTATE_COMPLETE and READYSTATE_UNINITIALIZED.
    //  if we use READYSTATE_UNINITIALIZED, we need to init the browser
    //  and make it visible and stuff like that.  something to the 
    //  check that IPersisteHistory->LoadHistory() does in shvocx.cpp.
    //  right now we will only allow COMPLETE.
    //
    TraceMsgW(DM_SHELLEXECOBJECT, "[%X] IEAuto_NavIEShortcut entered '%s' ready = %d", this, pvarIn->bstrVal, ready);

    
    if (((ready == READYSTATE_COMPLETE || ready == READYSTATE_UNINITIALIZED) || (fForceNavigate))
        && S_OK == IUnknown_Exec(_psbTop, &CGID_Explorer, SBCMDID_ISIEMODEBROWSER, 0, NULL, NULL))
        
    {
        IPersistFile *ppf;
        if (SUCCEEDED(CoCreateInstance(CLSID_InternetShortcut, NULL, CLSCTX_ALL, IID_PPV_ARG(IPersistFile, &ppf))))
        {
            if (SUCCEEDED(ppf->Load(pvarIn->bstrVal, STGM_READ)))
            {
                LPWSTR pszUrl = NULL;
                TraceMsg(DM_SHELLEXECOBJECT, "[%X] IEAuto_NavIEShortcut shortcut inited with file", this);

                IUniformResourceLocatorW *purl;
                if (SUCCEEDED(ppf->QueryInterface(IID_PPV_ARG(IUniformResourceLocatorW, &purl))))
                {
                    purl->GetURL(&pszUrl);
                    purl->Release();
                }
                
                if (pszUrl)
                {
                    TraceMsgW(DM_SHELLEXECOBJECT, "[%X] IEAuto_NavIEShortcut found %s", this, pszUrl);
                    
                    LPITEMIDLIST pidl;
                    IEParseDisplayNameW(CP_ACP, pszUrl, &pidl);
                    if (pidl)
                    {
                        ASSERT(NULL == _pwszShortcutPathPending);
                        if (_pwszShortcutPathPending)
                            LocalFree(_pwszShortcutPathPending);

                        _pwszShortcutPathPending = StrDupW(pvarIn->bstrVal);

                        hr = _BrowseObject(pidl, SBSP_SAMEBROWSER);

                        if (SUCCEEDED(hr))
                        {
                            if (ready == READYSTATE_UNINITIALIZED)
                                put_Visible(VARIANT_TRUE);
                            HWND hwnd = _GetHWND();
                            if (hwnd)
                            {
                                if (IsIconic(hwnd))
                                    ShowWindow(hwnd, SW_RESTORE);
                                else
                                    SetForegroundWindow(hwnd);
                            }
                        }
                        ILFree(pidl);
                    }
                    SHFree(pszUrl);
                }
            }
            ppf->Release();
        }
    }
    TraceMsg(DM_SHELLEXECOBJECT, "IEAuto_NavIEShortcut returns 0x%X", hr);

    return hr;
}

