/**************************************************************************\
* 
* Copyright (c) 1999  Microsoft Corporation
*
* Module Name:
*
*   comutils.hpp
*
* Abstract:
*
*   COM utility functions and macros
*
* Revision History:
*
*   05/13/1999 davidx
*       Created it.
*
\**************************************************************************/

#ifndef _COMUTILS_HPP
#define _COMUTILS_HPP

//--------------------------------------------------------------------------
// Macros for incrementing or decrementing COM component count
//--------------------------------------------------------------------------

extern HINSTANCE DllInstance;
extern LONG ComComponentCount;

inline VOID IncrementComComponentCount()
{
    InterlockedIncrement(&ComComponentCount);
}

inline VOID DecrementComComponentCount()
{
    InterlockedDecrement(&ComComponentCount);
}


//--------------------------------------------------------------------------
// Template for implementing IUnknown interface
//
// NOTES:
// 1. We can only handle objects that has one interface other than IUnknown.
// 2. We do not support aggregation.
//--------------------------------------------------------------------------

template <class I> class IUnknownBase : public I
{
public:

    // Query interface: note that we can only handle
    // objects that one interface other than IUnknown.

    STDMETHOD(QueryInterface)(REFIID riid, VOID** ppv)
    {
        if (riid == IID_IUnknown)
            *ppv = static_cast<IUnknown*>(this);
        else if (riid == __uuidof(I))
            *ppv = static_cast<I*>(this);
        else
        {
            *ppv = NULL;
            return E_NOINTERFACE;
        }

        reinterpret_cast<IUnknown*>(*ppv)->AddRef();
        return S_OK;
    }

    // Increment reference count

    STDMETHOD_(ULONG, AddRef)(VOID)
    {
        return InterlockedIncrement(&comRefCount);
    }

    // Decrement reference count

    STDMETHOD_(ULONG, Release)(VOID)
    {
        ULONG count = InterlockedDecrement(&comRefCount);

        if (count == 0)
            delete this;

        return count;
    }

protected:

    LONG comRefCount;

    // Constructor: notice that when an object is first
    // created, its reference count is set to 1.

    IUnknownBase<I>()
    {
        comRefCount = 1;
    }

    // Declare an empty virtual destructor

    virtual ~IUnknownBase<I>() {}
};


//--------------------------------------------------------------------------
// Template for implementing IClassFactory interface
//--------------------------------------------------------------------------

template <class T> class IClassFactoryBase
    : public IUnknownBase<IClassFactory>
{
public:

    // NOTE: We don't count class factory objects in ComComponentCount.
    // This means that the existence of a running class factory is not
    // guaranteed to keep a server loaded in memory.

    // Create a new instance of the component

    STDMETHOD(CreateInstance)(
        IUnknown* outer,
        REFIID riid,
        VOID** ppv
        )
    {
        // We don't support aggregation

        if (outer != NULL)
            return CLASS_E_NOAGGREGATION;

        // Instantiate a new object

        T* obj = new T;

        if (obj == NULL)
            return E_OUTOFMEMORY;

        // Get the requested interface

        HRESULT hr = obj->QueryInterface(riid, ppv);
        obj->Release();

        return hr;
    }

    // Lock/unlock the component server DLL

    STDMETHOD(LockServer)(BOOL lock)
    {
        if (lock)
        {
            IncrementComComponentCount();
        }
        else
        {
            DecrementComComponentCount();
        }

        return S_OK;
    }
};


//--------------------------------------------------------------------------
// Helper function for registering and unregistering a component
//--------------------------------------------------------------------------

struct ComComponentRegData
{
    const CLSID* clsid;
    const WCHAR* compName;
    const WCHAR* progID;
    const WCHAR* progIDNoVer;
    const WCHAR* threading;
};

HRESULT
RegisterComComponent(
    const ComComponentRegData* regdata,
    BOOL registerIt
    );

#endif // !_COMUTILS_HPP

