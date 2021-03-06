// ==++==
// 
//   Copyright (c) Microsoft Corporation.  All rights reserved.
// 
// ==--==
/*
* 
* EXEMPT: copyright change only, no build required
* 
*/
#ifndef _UNKNOWN_HXX
#define _UNKNOWN_HXX
#pragma once

#include "core.h"
//===========================================================================
// This template implements the IUnknown portion of a given COM interface.

template <class I, const IID* I_IID> class _unknown : public I
{
private:    long _refcount;

public:        
        _unknown() 
        { 
            _refcount = 0;
        }

        virtual ~_unknown()
        {
        }

        virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void ** ppvObject)
        {
            if (riid == IID_IUnknown)
            {
                *ppvObject = static_cast<IUnknown*>(this);
            }
            else if (riid == *I_IID)
            {
                *ppvObject = static_cast<I*>(this);
            }
            else
                return E_NOINTERFACE;
            
            reinterpret_cast<IUnknown*>(*ppvObject)->AddRef();
            return S_OK;
        }
    
        virtual ULONG STDMETHODCALLTYPE AddRef( void)
        {
            return InterlockedIncrement(&_refcount);
        }
    
        virtual ULONG STDMETHODCALLTYPE Release( void)
        {
            if (InterlockedDecrement(&_refcount) == 0)
            {
                delete this;
                return 0;
            }
            return _refcount;
        }
};    

#endif _UNKNOWN_HXX
