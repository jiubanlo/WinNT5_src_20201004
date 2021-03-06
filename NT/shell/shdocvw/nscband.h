/**************************************************************\
    FILE: NSCBand.h

    DESCRIPTION:  the class CNscBand exists to support name 
        space control bands.  A name space control uses IShellFolder
        rooted in various namespaces including Favorites, history, 
        Shell Name Space, etc. to depict a hierarchical UI 
        representation of the given name space.  
    
    AUTHOR:  chrisny

\**************************************************************/
#include "bands.h"
#include "nsc.h"
#include "uemapp.h"

#ifndef _NSCBAND_H
#define _NSCBAND_H

// for degug trace messages.
#define DM_PERSIST      0           // trace IPS::Load, ::Save, etc.
#define DM_MENU         0           // menu code
#define DM_FOCUS        0           // focus
#define DM_FOCUS2       0           // like DM_FOCUS, but verbose

const short CSIDL_NIL = -32767;

////////////////
///  NSC band

class CNSCBand : public CToolBand
               , public IContextMenu
               , public IBandNavigate
               , public IWinEventHandler
               , public INamespaceProxy
{
public:
    // *** IUnknown ***
    virtual STDMETHODIMP QueryInterface(REFIID riid, LPVOID * ppvObj);
    STDMETHODIMP_(ULONG) AddRef(void) { return CToolBand::AddRef(); };
    STDMETHODIMP_(ULONG) Release(void) { return CToolBand::Release(); };

    // *** IOleWindow methods ***
    virtual STDMETHODIMP GetWindow(HWND * lphwnd);

    // *** IDockingWindow methods ***
    virtual STDMETHODIMP ShowDW(BOOL fShow);
    virtual STDMETHODIMP CloseDW(DWORD dw);

    // *** IDeskBand methods ***
    virtual STDMETHODIMP GetBandInfo(DWORD dwBandID, DWORD fViewMode, 
                                   DESKBANDINFO* pdbi);

    // *** IPersistStream methods ***
    // (others use base class implementation) 
    virtual STDMETHODIMP GetClassID(CLSID *pClassID);
    virtual STDMETHODIMP Load(IStream *pStm);
    virtual STDMETHODIMP Save(IStream *pStm, BOOL fClearDirty);

    // *** IWinEventHandler methods ***
    virtual STDMETHODIMP OnWinEvent(HWND hwnd, UINT dwMsg
                                    , WPARAM wParam, LPARAM lParam
                                    , LRESULT *plres);
    virtual STDMETHODIMP IsWindowOwner(HWND hwnd);

    // *** IContextMenu methods ***
    STDMETHOD(QueryContextMenu)(HMENU hmenu,
                                UINT indexMenu,
                                UINT idCmdFirst,
                                UINT idCmdLast,
                                UINT uFlags);

    STDMETHOD(InvokeCommand)(LPCMINVOKECOMMANDINFO lpici);
    STDMETHOD(GetCommandString)(UINT_PTR    idCmd,
                                UINT        uType,
                                UINT      * pwReserved,
                                LPSTR       pszName,
                                UINT        cchMax) { return E_NOTIMPL; };

    // *** IOleCommandTarget methods ***
    virtual STDMETHODIMP Exec(const GUID *pguidCmdGroup,
        DWORD nCmdID, DWORD nCmdexecopt, VARIANTARG *pvarargIn, VARIANTARG *pvarargOut);
    virtual STDMETHODIMP QueryStatus(const GUID *pguidCmdGroup, ULONG cCmds, OLECMD rgCmds[], OLECMDTEXT *pcmdtext);

    // *** IBandNavigate methods ***
    virtual STDMETHODIMP Select(LPCITEMIDLIST pidl);
    

    // *** IInputObject methods ***
    virtual STDMETHODIMP TranslateAcceleratorIO(LPMSG lpMsg);

    // *** INamespaceProxy ***
    virtual STDMETHODIMP GetNavigateTarget(LPCITEMIDLIST pidl, LPITEMIDLIST *ppidlTarget, ULONG *pulAttrib);
    virtual STDMETHODIMP Invoke(LPCITEMIDLIST pidl);
    virtual STDMETHODIMP OnSelectionChanged(LPCITEMIDLIST pidl);
    virtual STDMETHODIMP RefreshFlags(DWORD *pdwStyle, DWORD *pdwExStyle, DWORD *pdwEnum) 
        {*pdwStyle = _GetTVStyle(); *pdwExStyle = _GetTVExStyle(); *pdwEnum = _GetEnumFlags(); return S_OK; };
    virtual STDMETHODIMP CacheItem(LPCITEMIDLIST pidl) { return S_OK; };
    
protected:    
    void _SetNscMode(UINT nMode) { _pns->SetNscMode(nMode); };
    virtual DWORD _GetTVStyle();
    virtual DWORD _GetTVExStyle() { return 0; };
    virtual DWORD _GetEnumFlags() { return SHCONTF_FOLDERS | SHCONTF_NONFOLDERS; };
    
    HRESULT _Init(LPCITEMIDLIST pidl);
    virtual HRESULT _InitializeNsc();
    
    virtual ~CNSCBand();
    virtual HRESULT _OnRegisterBand(IOleCommandTarget *poctProxy) { return S_OK; } // meant to be overridden
    
    void _UnregisterBand();
    void _EnsureImageListsLoaded();

    virtual HRESULT _TranslatePidl(LPCITEMIDLIST pidl, LPITEMIDLIST *ppidlTarget, ULONG *pulAttrib);
    virtual BOOL _ShouldNavigateToPidl(LPCITEMIDLIST pidl, ULONG ulAttrib);
    virtual HRESULT _NavigateRightPane(IShellBrowser *psb, LPCITEMIDLIST pidl);
    HRESULT _QueryContextMenuSelection(IContextMenu ** ppcm);
    HRESULT _InvokeCommandOnItem(LPCTSTR pszVerb);

    friend HRESULT CHistBand_CreateInstance(IUnknown *punkOuter, IUnknown **ppunk
                                            , LPCOBJECTINFO poi);      

    LPITEMIDLIST        _pidl;
    WCHAR               _szTitle[40];
                        
    INSCTree2 *         _pns;               // name space control data.
    IWinEventHandler *  _pweh;              // name space control's OnWinEvent handler
    BITBOOL             _fInited :1;        // true if band has been inited.
    BITBOOL             _fVisible :1;       // true if band is showing
    DWORD              _dwStyle;         // Treeview style
    LPCOBJECTINFO       _poi;               // cached object info.
    HACCEL              _haccTree;

    HIMAGELIST          _himlNormal;        // shared image list
    HIMAGELIST          _himlHot;
};

#endif /* _NSCBAND_H */





