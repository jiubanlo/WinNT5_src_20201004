//+---------------------------------------------------------------------------
//
//  Microsoft Windows
//  Copyright (C) Microsoft Corporation, 1992 - 1995.
//
//  File:       cbinding.cxx
//
//  Contents:
//
//  Classes:
//
//  Functions:
//
//  History:    11-11-95   JohannP (Johann Posch)   Created
//
//----------------------------------------------------------------------------

#include <trans.h>

PerfDbgTag(tagCBSC, "Urlmon", "Log CBSC", DEB_BINDING);

CBSC::CBSC(Medium medium)
{
    DEBUG_ENTER((DBG_TRANS,
                None,
                "CBSC::CBSC",
                "this=%#x, %#x",
                this, medium
                ));
                
    PerfDbgLog(tagCBSC, this, "+CBSC::CBSC");
    _cRef   = 1;
    _pBdg = NULL;
    _fBindToObject = (medium == Medium_Unknown) ? TRUE : FALSE;
    _fGotStopBinding = FALSE;
    _Medium = medium;
    _pStm = NULL;
    _pStg = NULL;
    _pUnk = NULL;
    _hrResult = NOERROR;
    PerfDbgLog(tagCBSC, this, "-CBSC::CBSC");

    DEBUG_LEAVE(0);
}

CBSC::~CBSC()
{
    DEBUG_ENTER((DBG_TRANS,
                None,
                "CBSC::~CBSC",
                "this=%#x",
                this
                ));
                
    PerfDbgLog(tagCBSC, this, "+CBSC::~CBSC");
    if (_pStm)
    {
        _pStm->Release();
    }
    if (_pUnk)
    {
        _pUnk->Release();
    }
    PerfDbgLog(tagCBSC, this, "-CBSC::~CBSC");

    DEBUG_LEAVE(0);
}

HRESULT CBSC::GetRequestedObject(IBindCtx *pbc, void **ppvObj)
{
    DEBUG_ENTER((DBG_TRANS,
                Hresult,
                "CBSC::GetRequestedObject",
                "this=%#x, %#x, %#x",
                this, pbc, ppvObj
                ));
                
    HRESULT hr = NOERROR;
    PerfDbgLog1(tagCBSC, this, "+CBSC::GetRequestedObject (_wzPath:%ws)", _wzPath);

    if (_Medium == Medium_Stream)
    {
        *ppvObj = _pStm;
        //TransAssert((_pStm));
        if (_pStm)
        {
            _pStm->AddRef();
        }
        else if (_hrResult != NOERROR)
        {
            hr = _hrResult;
        }
        else
        {
            hr = E_FAIL;
        }
    }
    else if (_Medium == Medium_Storage)
    {
        BIND_OPTS bindopts;
        bindopts.cbStruct = sizeof(BIND_OPTS);

        hr = pbc->GetBindOptions(&bindopts);
        if (hr == NOERROR)
        {
            hr = StgOpenStorage(_wzPath, NULL, bindopts.grfMode, NULL, 0, (LPSTORAGE FAR*)ppvObj );
            if (hr != NOERROR)
            {
                hr = StgOpenStorage(_wzPath, NULL, STGM_READWRITE | STGM_SHARE_DENY_NONE, NULL, 0, (LPSTORAGE FAR*)ppvObj );
            }
            if (hr != NOERROR)
            {
                hr = StgOpenStorage(_wzPath, NULL, STGM_READ | STGM_SHARE_DENY_WRITE, NULL, 0, (LPSTORAGE FAR*)ppvObj );
            }
        }
    }
    else if (_Medium == Medium_Unknown)
    {
        *ppvObj = _pUnk;
        TransAssert((_pUnk));
        if (_pUnk)
        {
            _pUnk->AddRef();
        }
        else
        {
            hr = E_FAIL;
        }
    }

    PerfDbgLog1(tagCBSC, this, "-CBSC::GetRequestedObject hr:%lx", hr);

    DEBUG_LEAVE(hr);
    return hr;
}

STDMETHODIMP CBSC::QueryInterface(REFIID riid, void **ppvObj)
{
    DEBUG_ENTER((DBG_TRANS,
                Hresult,
                "CBSC::IUnknown::QueryInterface",
                "this=%#x, %#x, %#x",
                this, &riid, ppvObj
                ));
                
    HRESULT hr = NOERROR;
    PerfDbgLog(tagCBSC, this, "+CBSC::QueryInterface");

    if (   IsEqualIID(riid, IID_IBindStatusCallback)
        || IsEqualIID(riid, IID_IUnknown))
    {
         _cRef++;   // A pointer to this object is returned
         *ppvObj = (void *)(IBindStatusCallback *)this;
    }
    else
    {
         *ppvObj = NULL;
         hr = E_NOINTERFACE;
    }

    PerfDbgLog1(tagCBSC, this, "-CBSC::QueryInterface hr:%lx", hr);

    DEBUG_LEAVE(hr);
    return hr;
}
STDMETHODIMP_(ULONG) CBSC::AddRef(void)
{
    DEBUG_ENTER((DBG_TRANS,
                Dword,
                "CBSC::IUnknown::AddRef",
                "this=%#x",
                this
                ));
                
    ULONG cRef = ++_cRef;
    PerfDbgLog1(tagCBSC, this, "CBSC::AddRef (cRef:%ld)", cRef);

    DEBUG_LEAVE(cRef);
    return cRef;
}
STDMETHODIMP_(ULONG) CBSC::Release(void)
{
    DEBUG_ENTER((DBG_TRANS,
                Dword,
                "CBSC::IUnknown::Release",
                "this=%#x",
                this
                ));
                
    ULONG cRef = 0;
    PerfDbgLog(tagCBSC, this, "+CBSC::Release");

    cRef = --_cRef;
    if (cRef == 0)
    {
        delete this;
    }

    PerfDbgLog1(tagCBSC, this, "-CBSC::Release cref:%ld", cRef);

    DEBUG_LEAVE(cRef);
    return cRef;
}


STDMETHODIMP  CBSC::GetBindInfo(
    /* [out] */ DWORD  *grfBINDINFOF,
    /* [out] */ BINDINFO  *pbindinfo)
{
    DEBUG_ENTER((DBG_TRANS,
                Hresult,
                "CBSC::IBindStatusCallback::GetBindInfo",
                "this=%#x, %#x, %#x",
                this, grfBINDINFOF, pbindinfo
                ));
                
    HRESULT hr = NOERROR;
    PerfDbgLog(tagCBSC, this, "+CBSC::GetBindInfo");

    if (   (grfBINDINFOF == NULL)
        || (pbindinfo == NULL)
        || (pbindinfo->cbSize == 0)
       )

    {
        DEBUG_LEAVE(E_INVALIDARG);
        return E_INVALIDARG;
    }

    //  Nuke BINDINFO preserving cbSize;
    DWORD cbSize = pbindinfo->cbSize;
    memset(pbindinfo, 0, cbSize);
    pbindinfo->cbSize = cbSize;

    if (_Medium == Medium_Storage)
    {
        *grfBINDINFOF = 0;

    }
    else
    {
        *grfBINDINFOF = 0;
        *grfBINDINFOF |= BINDF_ASYNCSTORAGE;
        *grfBINDINFOF |= BINDF_PULLDATA;
        *grfBINDINFOF |= BINDF_NEEDFILE;

    }


    PerfDbgLog1(tagCBSC, this, "-CBSC::GetBindInfo hr:%lx", hr);

    DEBUG_LEAVE(hr);
    return hr;
}

STDMETHODIMP  CBSC::OnStartBinding(DWORD grfBINDINFOF, IBinding  *pib)
{
    DEBUG_ENTER((DBG_TRANS,
                Hresult,
                "CBSC::IBindStatusCallback::OnStartBinding",
                "this=%#x, %#x, %#x",
                this, grfBINDINFOF, pib
                ));
                
    HRESULT hr = NOERROR;
    PerfDbgLog(tagCBSC, this, "+CBSC::OnStartBinding");
    if (pib)
    {
        _pBdg = pib;
        _pBdg->AddRef();
    }

    PerfDbgLog1(tagCBSC, this, "-CBSC::OnStartBinding hr:%lx", hr);

    DEBUG_LEAVE(hr);
    return hr;
}

STDMETHODIMP  CBSC::GetPriority(
    /* [out] */ LONG  *pnPriority)
{
    DEBUG_ENTER((DBG_TRANS,
                Hresult,
                "CBSC::IBindStatusCallback::GetPriority",
                "this=%#x, %#x",
                this, pnPriority
                ));
                
    HRESULT hr = NOERROR;
    PerfDbgLog(tagCBSC, this, "+CBSC::GetPriority");

    if (pnPriority == NULL)
    {
        hr = E_INVALIDARG;
    }
    else
    {
        *pnPriority = THREAD_PRIORITY_NORMAL;
    }

    PerfDbgLog1(tagCBSC, this, "-CBSC::GetPriority hr:%lx", hr);

    DEBUG_LEAVE(hr);
    return hr;
}

STDMETHODIMP  CBSC::OnProgress(
    /* [in] */ ULONG ulProgress,
    /* [in] */ ULONG ulProgressMax,
    /* [in] */ ULONG ulStatusCode,
    /* [in] */ LPCWSTR szStatusText)
{
    DEBUG_ENTER((DBG_TRANS,
                Hresult,
                "CBSC::IBindStatusCallback::OnProgress",
                "this=%#x, %#x, %#x, %#x, %.80wq",
                this, ulProgress, ulProgressMax, ulStatusCode, szStatusText
                ));
                
    HRESULT hr = NOERROR;
    PerfDbgLog4(tagCBSC, this, "+CBSC::OnProgress (StatusCode:%ld/%lx, Progress:%ld ProgressMax:%ld))",
        ulStatusCode, ulStatusCode, ulProgress, ulProgressMax);

    PerfDbgLog1(tagCBSC, this, "-CBSC::OnProgress hr:%lx", hr);

    DEBUG_LEAVE(hr);
    return hr;
}

STDMETHODIMP  CBSC::OnDataAvailable(
    /* [in] */ DWORD grfBSC,
    /* [in] */ DWORD dwSize,
    /* [in] */ FORMATETC  *pFmtetc,
    /* [in] */ STGMEDIUM  *pstgmed)
{
    DEBUG_ENTER((DBG_TRANS,
                Hresult,
                "CBSC::IBindStatusCallback::OnDataAvailable",
                "this=%#x, %#x, %#x, %#x, %#x",
                this, grfBSC, dwSize, pFmtetc, pstgmed
                ));
                
    HRESULT hr = NOERROR;
    HRESULT hr1 = NOERROR;
    PerfDbgLog(tagCBSC, this, "+CBSC::OnDataAvailable");

    UrlMkAssert((dwSize > 0));
    UrlMkAssert((pFmtetc != NULL && pstgmed != NULL));

    // if this is the first notification then make sure that the proper formatetc
    // is passed in.
    if (grfBSC & BSCF_FIRSTDATANOTIFICATION)
    {
        if (pFmtetc->tymed & TYMED_ISTREAM ||
            pstgmed->tymed == TYMED_ISTREAM )
        {
            STATSTG statstg;

            PerfDbgLog1(tagCBSC, this, "CBSC::OnDataAvailable - received IStream %lx", pstgmed->pstm);
            _pStm = pstgmed->pstm;
            _pStm->AddRef();

            if (pstgmed->pUnkForRelease)
            {
                _pUnk = pstgmed->pUnkForRelease;
                _pUnk->AddRef();
            }
        }
    }

    if (grfBSC & BSCF_LASTDATANOTIFICATION)
    {
        // if this is the final notification then get the data and display it

        if (_pStm)
        {
            UrlMkAssert((pFmtetc->tymed & TYMED_ISTREAM));
            //_pStm->Release();
            //_pStm = 0;
        }

        // if this is the final notification then get the data and display it

        if (pFmtetc->tymed & TYMED_FILE)
        {
            PerfDbgLog1(tagCBSC, this, "CBSC::OnDataAvailable - received File:%ws", pstgmed->lpszFileName);
            wcscpy(_wzPath, pstgmed->lpszFileName);
        }

        UrlMkAssert((_wzPath ));
    }

    PerfDbgLog1(tagCBSC, this, "-CBSC::OnDataAvailable hr:%lx", hr);

    DEBUG_LEAVE(hr);
    return hr;
}

STDMETHODIMP  CBSC::OnLowResource(
    /* [in] */ DWORD reserved)
{
    DEBUG_ENTER((DBG_TRANS,
                Hresult,
                "CBSC::IBindStatusCallback::OnLowResource",
                "this=%#x, %#x",
                this, reserved
                ));
                
    HRESULT hr = NOERROR;
    PerfDbgLog(tagCBSC, this, "+CBSC::OnLowResource");
    PerfDbgLog1(tagCBSC, this, "-CBSC::OnLowResource hr:%lx", hr);

    DEBUG_LEAVE(hr);
    return hr;
}

STDMETHODIMP CBSC::OnStopBinding(HRESULT hresult, LPCWSTR szError)
{
    DEBUG_ENTER((DBG_TRANS,
                Hresult,
                "CBSC::IBindStatusCallback::OnStopBinding",
                "this=%#x, %#x, %.80wq",
                this, hresult, szError
                ));
                
    HRESULT hr = NOERROR;
    PerfDbgLog1(tagCBSC, this, "+CBSC::OnStopBinding (hresult:%lx)", hresult);

    _fGotStopBinding = TRUE;
    _hrResult = hresult;
    if (_pBdg)
    {
        _pBdg->Release();
        _pBdg = NULL;
    }

    PerfDbgLog2(tagCBSC, this, "-CBSC::OnStopBinding (hresult:%lx) hr:%lx", hresult, hr);

    DEBUG_LEAVE(hr);
    return hr;
}

STDMETHODIMP  CBSC::OnObjectAvailable(REFIID riid, IUnknown  *punk)
{
    DEBUG_ENTER((DBG_TRANS,
                Hresult,
                "CBSC::IBindStatusCallback::OnObjectAvailable",
                "this=%#x, %#x, %#x",
                this, &riid, punk
                ));
                
    HRESULT hr = NOERROR;
    PerfDbgLog(tagCBSC, this, "+CBSC::OnObjectAvailable");

    UrlMkAssert((_Medium == Medium_Unknown));
    UrlMkAssert((punk));

    punk->AddRef();
    _pUnk = punk;

    PerfDbgLog1(tagCBSC, this, "-CBSC::OnObjectAvailable hr:%lx", hr);

    DEBUG_LEAVE(hr);
    return hr;
}
