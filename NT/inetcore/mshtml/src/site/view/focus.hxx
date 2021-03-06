//+----------------------------------------------------------------------------
//
// File:        Adorner.HXX
//
// Contents:    CAdorner class
//
//  An Adorner provides the addition of  'non-content-based' nodes in the display tree
// 
// Copyright (c) 1998 Microsoft Corporation. All rights reserved.
//
// History
//
//  07-18-98 - marka - created
//-----------------------------------------------------------------------------


#ifndef I_ADORNER_HXX_
#define I_ADORNER_HXX_
#pragma INCMSG("--- Beg 'view.hxx'")

#ifndef X_DISPCLIENT_HXX_
#define X_DISPCLIENT_HXX_
#include "dispclient.hxx"
#endif

class CDispNode;

MtExtern(CElementAdorner)
MtExtern(CFocusAdorner)
MtExtern(CPainterBehavior)
MtExtern(CFocusBehavior)
MtExtern(CAdornerBehavior)

#ifdef USE_OLD_ADORNERS
//+---------------------------------------------------------------------------
//
//  Enums
//
//----------------------------------------------------------------------------

enum ADORNERLAYER
{
    ADL_ALWAYSONTOP = 1,    // The adorner display node is on top of all other content
    ADL_ONELEMENT   = 2,    // The adorner display node is on top of the associated element
    ADL_TOPOFFLOW   = 3,    // The adorner display node is on top of all flow content

    ADL_MAX
};


//+----------------------------------------------------------------------------
//
//  Class:  CAdorner
//
//-----------------------------------------------------------------------------

class CAdorner: public CDispClient
{
    friend CView;

public:
    CAdorner( CView * pView, CElement * pElement = NULL );
    virtual ~CAdorner();

    DWORD       AddRef()
                {
                    Assert( _cRefs >= 0 );
                    return ++_cRefs;
                }
    DWORD       Release()
                {
                    Assert( _cRefs > 0 );
                    _cRefs--;
                    if ( !_cRefs )
                    {
                        delete this;
                        return 0;
                    }
                    return _cRefs;
                }

    void        Destroy()
                {
                    DestroyDispNode();
                    Release();
                }

    void        GetBounds( CRect * prc ) const;
    void        GetBounds( RECT * prc ) const
                {
                    CRect & rc = (CRect &)(*prc);
                    GetBounds( & rc );
                }
    CDispNode * GetDispNode()
                {
                    return _pDispNode;
                }
    CElement *  GetElement() const
                {
                    return _pElement;
                }
    void        GetRange( long * pcpStart, long * pcpEnd ) const;
    CView *     GetView() const
                {
                    return _pView;
                }

    void        Invalidate(const CRect & rc = (const CRect &)g_Zero.rc) const
                {
                    if (_pDispNode)
                    {
                        if (rc.IsEmpty())
                        {
                            _pDispNode->Invalidate();
                        }
                        else
                        {
                            _pDispNode->Invalidate(rc, COORDSYS_FLOWCONTENT);
                        }
                    }
                }

    virtual void        PositionChanged(const CSize * psize = NULL) = 0;
    virtual void        ShapeChanged() = 0;

    // IDispClient overrides
    void  GetOwner(
                CDispNode *pDispNode,
                void ** ppv);

    void  DrawClient(
                const RECT *   prcBounds,
                const RECT *   prcRedraw,
                CDispSurface * pDispSurface,
                CDispNode *    pDispNode,
                void *         cookie,
                void *         pClientData,
                DWORD          dwFlags);

    void  DrawClientBackground(
                const RECT *   prcBounds,
                const RECT *   prcRedraw,
                CDispSurface * pDispSurface,
                CDispNode *    pDispNode,
                void *         pClientData,
                DWORD          dwFlags);

    void  DrawClientBorder(
                const RECT *   prcBounds,
                const RECT *   prcRedraw,
                CDispSurface * pDispSurface,
                CDispNode *    pDispNode,
                void *         pClientData,
                DWORD          dwFlags);

    void  DrawClientScrollbar(
                int whichScrollbar,
                const RECT* prcBounds,
                const RECT* prcRedraw,
                LONG contentSize,
                LONG containerSize,
                LONG scrollAmount,
                CDispSurface *pSurface,
                CDispNode *pDispNode,
                void *pClientData,
                DWORD dwFlags);
                            
    void  DrawClientScrollbarFiller(
                const RECT *   prcBounds,
                const RECT *   prcRedraw,
                CDispSurface * pDispSurface,
                CDispNode *    pDispNode,
                void *         pClientData,
                DWORD          dwFlags);

    BOOL  HitTestContent(
                const POINT *pptHit,
                CDispNode *pDispNode,
                void *pClientData,
                BOOL fDeclinedByPeer);

    BOOL  HitTestFuzzy(
                const POINT *pptHitInParentCoords,
                CDispNode *pDispNode,
                void *pClientData);

    BOOL  HitTestScrollbar(
                int whichScrollbar,
                const POINT *pptHit,
                CDispNode *pDispNode,
                void *pClientData) ;

    BOOL  HitTestScrollbarFiller(
                const POINT *pptHit,
                CDispNode *pDispNode,
                void *pClientData) ;

    BOOL  HitTestBorder(
                const POINT *pptHit,
                CDispNode *pDispNode,
                void *pClientData) ;

    BOOL  ProcessDisplayTreeTraversal(
                void *          pClientData);

    LONG  GetZOrderForSelf(CDispNode *pDispNode);

    LONG  CompareZOrder(
                CDispNode * pDispNode1,
                CDispNode * pDispNode2);
    
    void  HandleViewChange(
                DWORD flags,
                const RECT* prcClient,  // global coordinates
                const RECT* prcClip,    // global coordinates
                CDispNode* pDispNode) ;
                            
    void  NotifyScrollEvent(
                RECT *  prcScroll,
                SIZE *  psizeScrollDelta);

    DWORD GetClientPainterInfo( CDispNode *pDispNodeFor,
                                CAryDispClientInfo *pAryClientInfo);

    void  DrawClientLayers(
                const RECT* prcBounds,
                const RECT* prcRedraw,
                CDispSurface *pSurface,
                CDispNode *pDispNode,
                void *cookie,
                void *pClientData,
                DWORD dwFlags);

#if DBG==1
    void  DumpDebugInfo(
                HANDLE hFile,
                long level,
                long childNumber,
                CDispNode *pDispNode,
                void* cookie);
#endif
                                
protected:
    DWORD           _cRefs;
    CView *         _pView;
    CElement *      _pElement;
    CDispNode *     _pDispNode;

    virtual void            EnsureDispNode();
    void                    DestroyDispNode();

    virtual void *          GetDispCookie() const = 0;
    virtual DISPNODELAYER   GetDispLayer() const = 0;

#if DBG==1
    void                    ShowRectangle(CFormDrawInfo * pDI);
#endif
};


//+----------------------------------------------------------------------------
//
//  Class:  CElementAdorner
//
//-----------------------------------------------------------------------------

class CElementAdorner : public CAdorner 
{
    typedef CAdorner super;

    friend CView;

public:

    DECLARE_MEMCLEAR_NEW_DELETE(Mt(CElementAdorner));
    
    CElementAdorner( CView * pView, CElement* pElement );
    ~CElementAdorner();

    void    SetSite( IElementAdorner * pIElementAdorner )
            {
                if ( _pIElementAdorner )
                {
                    _pIElementAdorner->Release();
                }

                _pIElementAdorner = pIElementAdorner;

                if ( _pIElementAdorner )
                {
                    _pIElementAdorner->AddRef();
                }
            }

    void    PositionChanged(const CSize * psize = NULL);
    void    ShapeChanged();

    void    UpdateAdorner();

    BOOL    ScrollIntoView();
    
    void    DrawClient(
                const RECT *   prcBounds,
                const RECT *   prcRedraw,
                CDispSurface * pDispSurface,
                CDispNode *    pDispNode,
                void *         cookie,
                void *         pClientData,
                DWORD          dwFlags);

    BOOL    HitTestContent(
                const POINT *  pptHit,
                CDispNode *    pDispNode,
                void *         pClientData,
                BOOL           fDeclinedByPeer);   

    LONG    GetZOrderForSelf(CDispNode *pDispNode);

protected:
    IElementAdorner *   _pIElementAdorner;

    void *          GetDispCookie() const
                    {
                        return (void *)(ADL_ALWAYSONTOP);
                    }
    DISPNODELAYER   GetDispLayer() const
                    {
                        return DISPNODELAYER_POSITIVEZ;
                    }
};


//+----------------------------------------------------------------------------
//
//  Class:  CFocusAdorner
//
//-----------------------------------------------------------------------------

class CFocusAdorner : public CAdorner 
{
    typedef CAdorner super;

    friend CView;

public:

    DECLARE_MEMCLEAR_NEW_DELETE(Mt(CFocusAdorner));
    
    CFocusAdorner( CView * pView );
    ~CFocusAdorner();

    void SetElement( CElement * pElement, long iDivision );

    long    GetDivision() const
            {
                return _iDivision;
            }

    void    PositionChanged(const CSize * psize = NULL);
    void    ShapeChanged();
    
    void    DrawClient(
                const RECT *   prcBounds,
                const RECT *   prcRedraw,
                CDispSurface * pDispSurface,
                CDispNode *    pDispNode,
                void *         cookie,
                void *         pClientData,
                DWORD          dwFlags);

    LONG    GetZOrderForSelf(CDispNode *pDispNode);

protected:
    long            _iDivision;
    CShape *        _pShape;
    ADORNERLAYER    _adl;
    DISPNODELAYER   _dnl;

    void            EnsureFocus();
    void            EnsureDispNode();

    void *          GetDispCookie() const
                    {
                        return (void *)_adl;
                    }
    DISPNODELAYER   GetDispLayer() const
                    {
                        return _dnl;
                    }
};

#endif

class CFocusBehavior : public IElementBehavior , public IHTMLPainter
{
public:
    DECLARE_MEMCLEAR_NEW_DELETE(Mt(CFocusBehavior));

    CFocusBehavior(CView *pView) : _ulRefs(1) , _pView(pView) , _fDirtyShape(TRUE)
    {
    }
    ~CFocusBehavior();

    STDMETHOD(QueryInterface)(REFIID iid, LPVOID *ppv);

    STDMETHOD_(ULONG, AddRef)()
    {
        return ++_ulRefs;
    }

    STDMETHOD_(ULONG, Release)()
    {
        if (--_ulRefs == 0)
        {
            _ulRefs = ULREF_IN_DESTRUCTOR;
            delete this;
            return 0;
        }
        return _ulRefs;
    }

    STDMETHOD(Init)(IElementBehaviorSite *pBehaviorSite)
    {
        ClearInterface(&_pBehaviorSite);
        _pBehaviorSite = pBehaviorSite;

        if (_pBehaviorSite)
        {
            _pBehaviorSite->AddRef();

            IGNORE_HR(_pBehaviorSite->QueryInterface(IID_IHTMLPaintSite, (LPVOID *)&_pPaintSite));
        }

        RRETURN(S_OK);
    }

    STDMETHOD(Detach)()
    {
        Assert(_pBehaviorSite);

        ClearInterface(&_pBehaviorSite);
        ClearInterface(&_pPaintSite);

        RRETURN(S_OK);
    }

    STDMETHOD(Notify)(long lEvent, VARIANT *pVar)
    {
        RRETURN(S_OK);
    }


    void SetElement(CElement *pElement, long iDivision);

    void ShapeChanged()
    {
        if (!_fDirtyShape)
        {
            if (_pPaintSite)
                _pPaintSite->InvalidateRect(NULL);
            _fDirtyShape = TRUE;
        }
    }

    void UpdateShape();

    // IHTMLPainter methods
    STDMETHOD(GetPainterInfo)(HTML_PAINTER_INFO *pInfo);
    STDMETHOD(Draw)(RECT rcBounds, RECT rcUpdate, LONG lDrawFlags, HDC hdc, LPVOID pvDrawObject);
    STDMETHOD(HitTestPoint)(POINT pt, BOOL *pbHit, LONG *plPartID)
    {
        if (pbHit)
            *pbHit = FALSE;

        if (plPartID)
            *plPartID = 0;

        RRETURN(S_OK);
    }

    STDMETHOD(OnResize)(SIZE size)
    {
        return S_OK;
    }

private:
    ULONG _ulRefs;
    IElementBehaviorSite *_pBehaviorSite;
    IHTMLPaintSite *_pPaintSite;
    CElement *_pElement;
    LONG _iDivision;
    CView *_pView;
    CShape *_pShape;
    BOOL _fDirtyShape;
    LONG _lCookie;
    CRect _rcBounds;
};

#endif
