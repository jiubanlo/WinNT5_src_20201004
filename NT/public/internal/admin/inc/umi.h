
#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0357 */
/* Compiler settings for umi.idl:
    Oicf, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__


#ifndef __umi_h__
#define __umi_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IUmiPropList_FWD_DEFINED__
#define __IUmiPropList_FWD_DEFINED__
typedef interface IUmiPropList IUmiPropList;
#endif 	/* __IUmiPropList_FWD_DEFINED__ */


#ifndef __IUmiBaseObject_FWD_DEFINED__
#define __IUmiBaseObject_FWD_DEFINED__
typedef interface IUmiBaseObject IUmiBaseObject;
#endif 	/* __IUmiBaseObject_FWD_DEFINED__ */


#ifndef __IUmiObject_FWD_DEFINED__
#define __IUmiObject_FWD_DEFINED__
typedef interface IUmiObject IUmiObject;
#endif 	/* __IUmiObject_FWD_DEFINED__ */


#ifndef __IUmiConnection_FWD_DEFINED__
#define __IUmiConnection_FWD_DEFINED__
typedef interface IUmiConnection IUmiConnection;
#endif 	/* __IUmiConnection_FWD_DEFINED__ */


#ifndef __IUmiContainer_FWD_DEFINED__
#define __IUmiContainer_FWD_DEFINED__
typedef interface IUmiContainer IUmiContainer;
#endif 	/* __IUmiContainer_FWD_DEFINED__ */


#ifndef __IUmiCursor_FWD_DEFINED__
#define __IUmiCursor_FWD_DEFINED__
typedef interface IUmiCursor IUmiCursor;
#endif 	/* __IUmiCursor_FWD_DEFINED__ */


#ifndef __IUmiObjectSink_FWD_DEFINED__
#define __IUmiObjectSink_FWD_DEFINED__
typedef interface IUmiObjectSink IUmiObjectSink;
#endif 	/* __IUmiObjectSink_FWD_DEFINED__ */


#ifndef __IUmiURLKeyList_FWD_DEFINED__
#define __IUmiURLKeyList_FWD_DEFINED__
typedef interface IUmiURLKeyList IUmiURLKeyList;
#endif 	/* __IUmiURLKeyList_FWD_DEFINED__ */


#ifndef __IUmiURL_FWD_DEFINED__
#define __IUmiURL_FWD_DEFINED__
typedef interface IUmiURL IUmiURL;
#endif 	/* __IUmiURL_FWD_DEFINED__ */


#ifndef __IUmiQuery_FWD_DEFINED__
#define __IUmiQuery_FWD_DEFINED__
typedef interface IUmiQuery IUmiQuery;
#endif 	/* __IUmiQuery_FWD_DEFINED__ */


#ifndef __IUmiCustomInterfaceFactory_FWD_DEFINED__
#define __IUmiCustomInterfaceFactory_FWD_DEFINED__
typedef interface IUmiCustomInterfaceFactory IUmiCustomInterfaceFactory;
#endif 	/* __IUmiCustomInterfaceFactory_FWD_DEFINED__ */


#ifndef __UmiDefURL_FWD_DEFINED__
#define __UmiDefURL_FWD_DEFINED__

#ifdef __cplusplus
typedef class UmiDefURL UmiDefURL;
#else
typedef struct UmiDefURL UmiDefURL;
#endif /* __cplusplus */

#endif 	/* __UmiDefURL_FWD_DEFINED__ */


#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 


#ifndef __UMI_V6_LIBRARY_DEFINED__
#define __UMI_V6_LIBRARY_DEFINED__

/* library UMI_V6 */
/* [uuid] */ 












typedef 
enum tag_UMI_TYPE_ENUMERATION
    {	UMI_TYPE_NULL	= 0,
	UMI_TYPE_I1	= 1,
	UMI_TYPE_I2	= 2,
	UMI_TYPE_I4	= 3,
	UMI_TYPE_I8	= 4,
	UMI_TYPE_UI1	= 5,
	UMI_TYPE_UI2	= 6,
	UMI_TYPE_UI4	= 7,
	UMI_TYPE_UI8	= 8,
	UMI_TYPE_R4	= 9,
	UMI_TYPE_R8	= 10,
	UMI_TYPE_FILETIME	= 12,
	UMI_TYPE_SYSTEMTIME	= 13,
	UMI_TYPE_BOOL	= 14,
	UMI_TYPE_IDISPATCH	= 15,
	UMI_TYPE_IUNKNOWN	= 16,
	UMI_TYPE_VARIANT	= 17,
	UMI_TYPE_LPWSTR	= 20,
	UMI_TYPE_OCTETSTRING	= 21,
	UMI_TYPE_UMIARRAY	= 22,
	UMI_TYPE_DISCOVERY	= 23,
	UMI_TYPE_UNDEFINED	= 24,
	UMI_TYPE_DEFAULT	= 25,
	UMI_TYPE_ARRAY_FLAG	= 0x2000
    } 	UMI_TYPE_ENUMERATION;

typedef ULONG UMI_TYPE;

typedef /* [public] */ 
enum __MIDL___MIDL_itf_umi_0000_0001
    {	UMI_GENUS_CLASS	= 1,
	UMI_GENUS_INSTANCE	= 2
    } 	UMI_GENUS_TYPE;

typedef /* [public] */ 
enum __MIDL___MIDL_itf_umi_0000_0002
    {	UMI_DONT_COMMIT_SECURITY_DESCRIPTOR	= 0x10
    } 	UMI_COMMIT_FLAGS;

typedef /* [public] */ 
enum __MIDL___MIDL_itf_umi_0000_0003
    {	UMI_FLAG_GETPROPS_ALL	= 1,
	UMI_FLAG_GETPROPS_SCHEMA	= 0x2,
	UMI_MASK_GETPROPS_PROP	= 0xff,
	UMI_FLAG_GETPROPS_NAMES	= 0x100,
	UMI_MASK_GETPROPS_EXT	= 0x100
    } 	UMI_GETPROPS_FLAGS;

typedef /* [public] */ 
enum __MIDL___MIDL_itf_umi_0000_0004
    {	UMI_FLAG_OWNER_SECURITY_INFORMATION	= 0x1,
	UMI_FLAG_GROUP_SECURITY_INFORMATION	= 0x2,
	UMI_FLAG_DACL_SECURITY_INFORMATION	= 0x4,
	UMI_FLAG_SACL_SECURITY_INFORMATION	= 0x8,
	UMI_SECURITY_MASK	= 0xf,
	UMI_FLAG_PROVIDER_CACHE	= 0x10,
	UMI_FLAG_PROPERTY_ORIGIN	= 0x20
    } 	UMI_GET_FLAGS;

typedef /* [public] */ 
enum __MIDL___MIDL_itf_umi_0000_0005
    {	UMI_FLAG_REFRESH_ALL	= 0,
	UMI_FLAG_REFRESH_PARTIAL	= 1
    } 	UMI_REFRESH_FLAGS;

typedef /* [public] */ 
enum __MIDL___MIDL_itf_umi_0000_0006
    {	UMI_OPERATION_NONE	= 0,
	UMI_OPERATION_APPEND	= 1,
	UMI_OPERATION_UPDATE	= 2,
	UMI_OPERATION_EMPTY	= 3,
	UMI_OPERATION_INSERT_AT	= 4,
	UMI_OPERATION_REMOVE_AT	= 5,
	UMI_OPERATION_DELETE_AT	= 6,
	UMI_OPERATION_DELETE_FIRST_MATCH	= 7,
	UMI_OPERATION_DELETE_ALL_MATCHES	= 8,
	UMI_OPERATION_RESTORE_DEFAULT	= 9
    } 	UMI_PROP_INSTRUCTION;

typedef /* [public] */ 
enum __MIDL___MIDL_itf_umi_0000_0007
    {	UMI_OPERATION_INSTANCE	= 0x1000,
	UMI_OPERATION_CLASS	= 0x2000
    } 	UMI_OPERATION_PATH;

typedef /* [public] */ 
enum __MIDL___MIDL_itf_umi_0000_0008
    {	UMIPATH_CREATE_AS_NATIVE	= 0x8000,
	UMIPATH_CREATE_AS_EITHER	= 0x4000,
	UMIPATH_CREATE_ACCEPT_RELATIVE	= 0x4
    } 	tag_UMI_PATH_CREATE_FLAG;

typedef 
enum tag_WMI_PATH_STATUS_FLAG
    {	UMIPATH_INFO_NATIVE_STRING	= 0x1,
	UMIPATH_INFO_RELATIVE_PATH	= 0x2,
	UMIPATH_INFO_INSTANCE_PATH	= 0x4,
	UMIPATH_INFO_CLASS_PATH	= 0x8,
	UMIPATH_INFO_SINGLETON_PATH	= 0x10
    } 	tag_UMI_PATH_STATUS_FLAG;

typedef /* [public] */ 
enum __MIDL___MIDL_itf_umi_0000_0009
    {	UMI_S_NO_ERROR	= 0,
	UMI_E_CONNECTION_FAILURE	= 0x80041001,
	UMI_E_TIMED_OUT	= 0x80041002,
	UMI_E_TYPE_MISMATCH	= 0x80041003,
	UMI_E_NOT_FOUND	= 0x80041004,
	UMI_E_INVALID_FLAGS	= 0x80041005,
	UMI_E_UNSUPPORTED_FLAGS	= 0x80041006,
	UMI_E_SYNCHRONIZATION_REQUIRED	= 0x80041007,
	UMI_E_UNSUPPORTED_OPERATION	= 0x80041008,
	UMI_E_TRANSACTION_FAILURE	= 0x80041009,
	UMI_E_UNBOUND_OBJECT	= 0x8004100a
    } 	UMI_STATUS;

typedef struct tag_UMI_OCTET_STRING
    {
    ULONG uLength;
    byte *lpValue;
    } 	UMI_OCTET_STRING;

typedef struct tag_UMI_OCTET_STRING *PUMI_OCTET_STRING;

typedef struct tag_UMI_COM_OBJECT
    {
    IID *priid;
    LPVOID pInterface;
    } 	UMI_COM_OBJECT;

typedef struct tag_UMI_COM_OBJECT *PUMI_COM_OBJECT;

typedef /* [public][public][public][public][public][public][public][public][public][public] */ union __MIDL___MIDL_itf_umi_0000_0010
    {
    CHAR cValue[ 1 ];
    UCHAR ucValue[ 1 ];
    WCHAR wcValue[ 1 ];
    WORD wValue[ 1 ];
    DWORD dwValue[ 1 ];
    LONG lValue[ 1 ];
    ULONG uValue[ 1 ];
    BYTE byteValue[ 1 ];
    BOOL bValue[ 1 ];
    LPWSTR pszStrValue[ 1 ];
    FILETIME fileTimeValue[ 1 ];
    SYSTEMTIME sysTimeValue[ 1 ];
    double dblValue[ 1 ];
    unsigned __int64 uValue64[ 1 ];
    __int64 nValue64[ 1 ];
    UMI_OCTET_STRING octetStr[ 1 ];
    UMI_COM_OBJECT comObject[ 1 ];
    } 	UMI_VALUE;

typedef union __MIDL___MIDL_itf_umi_0000_0010 *PUMI_VALUE;

typedef /* [public][public][public][public][public][public][public][public] */ struct __MIDL___MIDL_itf_umi_0000_0011
    {
    UMI_TYPE uType;
    ULONG uCount;
    ULONG uOperationType;
    LPWSTR pszPropertyName;
    UMI_VALUE *pUMIValue;
    } 	UMI_PROPERTY;

typedef struct __MIDL___MIDL_itf_umi_0000_0011 *PUMI_PROPERTY;

typedef /* [public][public][public][public][public][public] */ struct __MIDL___MIDL_itf_umi_0000_0012
    {
    ULONG uCount;
    UMI_PROPERTY *pPropArray;
    } 	UMI_PROPERTY_VALUES;

typedef struct __MIDL___MIDL_itf_umi_0000_0012 *PUMI_PROPERTY_VALUES;


EXTERN_C const IID LIBID_UMI_V6;

#ifndef __IUmiPropList_INTERFACE_DEFINED__
#define __IUmiPropList_INTERFACE_DEFINED__

/* interface IUmiPropList */
/* [uuid][local][object] */ 


EXTERN_C const IID IID_IUmiPropList;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("12575a7b-d9db-11d3-a11f-00105a1f515a")
    IUmiPropList : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Put( 
            /* [in] */ LPCWSTR pszName,
            /* [in] */ ULONG uFlags,
            /* [in] */ UMI_PROPERTY_VALUES *pProp) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Get( 
            /* [in] */ LPCWSTR pszName,
            /* [in] */ ULONG uFlags,
            /* [out] */ UMI_PROPERTY_VALUES **pProp) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetAt( 
            /* [in] */ LPCWSTR pszName,
            /* [in] */ ULONG uFlags,
            /* [in] */ ULONG uBufferLength,
            /* [out] */ LPVOID pExistingMem) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetAs( 
            /* [in] */ LPCWSTR pszName,
            /* [in] */ ULONG uFlags,
            /* [in] */ ULONG uCoercionType,
            /* [out] */ UMI_PROPERTY_VALUES **pProp) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE FreeMemory( 
            ULONG uReserved,
            LPVOID pMem) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Delete( 
            /* [in] */ LPCWSTR pszName,
            /* [in] */ ULONG uFlags) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetProps( 
            /* [in] */ LPCWSTR *pszNames,
            /* [in] */ ULONG uNameCount,
            /* [in] */ ULONG uFlags,
            /* [out] */ UMI_PROPERTY_VALUES **pProps) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE PutProps( 
            /* [in] */ LPCWSTR *pszNames,
            /* [in] */ ULONG uNameCount,
            /* [in] */ ULONG uFlags,
            /* [in] */ UMI_PROPERTY_VALUES *pProps) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE PutFrom( 
            /* [in] */ LPCWSTR pszName,
            /* [in] */ ULONG uFlags,
            /* [in] */ ULONG uBufferLength,
            /* [in] */ LPVOID pExistingMem) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IUmiPropListVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUmiPropList * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUmiPropList * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUmiPropList * This);
        
        HRESULT ( STDMETHODCALLTYPE *Put )( 
            IUmiPropList * This,
            /* [in] */ LPCWSTR pszName,
            /* [in] */ ULONG uFlags,
            /* [in] */ UMI_PROPERTY_VALUES *pProp);
        
        HRESULT ( STDMETHODCALLTYPE *Get )( 
            IUmiPropList * This,
            /* [in] */ LPCWSTR pszName,
            /* [in] */ ULONG uFlags,
            /* [out] */ UMI_PROPERTY_VALUES **pProp);
        
        HRESULT ( STDMETHODCALLTYPE *GetAt )( 
            IUmiPropList * This,
            /* [in] */ LPCWSTR pszName,
            /* [in] */ ULONG uFlags,
            /* [in] */ ULONG uBufferLength,
            /* [out] */ LPVOID pExistingMem);
        
        HRESULT ( STDMETHODCALLTYPE *GetAs )( 
            IUmiPropList * This,
            /* [in] */ LPCWSTR pszName,
            /* [in] */ ULONG uFlags,
            /* [in] */ ULONG uCoercionType,
            /* [out] */ UMI_PROPERTY_VALUES **pProp);
        
        HRESULT ( STDMETHODCALLTYPE *FreeMemory )( 
            IUmiPropList * This,
            ULONG uReserved,
            LPVOID pMem);
        
        HRESULT ( STDMETHODCALLTYPE *Delete )( 
            IUmiPropList * This,
            /* [in] */ LPCWSTR pszName,
            /* [in] */ ULONG uFlags);
        
        HRESULT ( STDMETHODCALLTYPE *GetProps )( 
            IUmiPropList * This,
            /* [in] */ LPCWSTR *pszNames,
            /* [in] */ ULONG uNameCount,
            /* [in] */ ULONG uFlags,
            /* [out] */ UMI_PROPERTY_VALUES **pProps);
        
        HRESULT ( STDMETHODCALLTYPE *PutProps )( 
            IUmiPropList * This,
            /* [in] */ LPCWSTR *pszNames,
            /* [in] */ ULONG uNameCount,
            /* [in] */ ULONG uFlags,
            /* [in] */ UMI_PROPERTY_VALUES *pProps);
        
        HRESULT ( STDMETHODCALLTYPE *PutFrom )( 
            IUmiPropList * This,
            /* [in] */ LPCWSTR pszName,
            /* [in] */ ULONG uFlags,
            /* [in] */ ULONG uBufferLength,
            /* [in] */ LPVOID pExistingMem);
        
        END_INTERFACE
    } IUmiPropListVtbl;

    interface IUmiPropList
    {
        CONST_VTBL struct IUmiPropListVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUmiPropList_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IUmiPropList_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IUmiPropList_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IUmiPropList_Put(This,pszName,uFlags,pProp)	\
    (This)->lpVtbl -> Put(This,pszName,uFlags,pProp)

#define IUmiPropList_Get(This,pszName,uFlags,pProp)	\
    (This)->lpVtbl -> Get(This,pszName,uFlags,pProp)

#define IUmiPropList_GetAt(This,pszName,uFlags,uBufferLength,pExistingMem)	\
    (This)->lpVtbl -> GetAt(This,pszName,uFlags,uBufferLength,pExistingMem)

#define IUmiPropList_GetAs(This,pszName,uFlags,uCoercionType,pProp)	\
    (This)->lpVtbl -> GetAs(This,pszName,uFlags,uCoercionType,pProp)

#define IUmiPropList_FreeMemory(This,uReserved,pMem)	\
    (This)->lpVtbl -> FreeMemory(This,uReserved,pMem)

#define IUmiPropList_Delete(This,pszName,uFlags)	\
    (This)->lpVtbl -> Delete(This,pszName,uFlags)

#define IUmiPropList_GetProps(This,pszNames,uNameCount,uFlags,pProps)	\
    (This)->lpVtbl -> GetProps(This,pszNames,uNameCount,uFlags,pProps)

#define IUmiPropList_PutProps(This,pszNames,uNameCount,uFlags,pProps)	\
    (This)->lpVtbl -> PutProps(This,pszNames,uNameCount,uFlags,pProps)

#define IUmiPropList_PutFrom(This,pszName,uFlags,uBufferLength,pExistingMem)	\
    (This)->lpVtbl -> PutFrom(This,pszName,uFlags,uBufferLength,pExistingMem)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IUmiPropList_Put_Proxy( 
    IUmiPropList * This,
    /* [in] */ LPCWSTR pszName,
    /* [in] */ ULONG uFlags,
    /* [in] */ UMI_PROPERTY_VALUES *pProp);


void __RPC_STUB IUmiPropList_Put_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IUmiPropList_Get_Proxy( 
    IUmiPropList * This,
    /* [in] */ LPCWSTR pszName,
    /* [in] */ ULONG uFlags,
    /* [out] */ UMI_PROPERTY_VALUES **pProp);


void __RPC_STUB IUmiPropList_Get_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IUmiPropList_GetAt_Proxy( 
    IUmiPropList * This,
    /* [in] */ LPCWSTR pszName,
    /* [in] */ ULONG uFlags,
    /* [in] */ ULONG uBufferLength,
    /* [out] */ LPVOID pExistingMem);


void __RPC_STUB IUmiPropList_GetAt_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IUmiPropList_GetAs_Proxy( 
    IUmiPropList * This,
    /* [in] */ LPCWSTR pszName,
    /* [in] */ ULONG uFlags,
    /* [in] */ ULONG uCoercionType,
    /* [out] */ UMI_PROPERTY_VALUES **pProp);


void __RPC_STUB IUmiPropList_GetAs_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IUmiPropList_FreeMemory_Proxy( 
    IUmiPropList * This,
    ULONG uReserved,
    LPVOID pMem);


void __RPC_STUB IUmiPropList_FreeMemory_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IUmiPropList_Delete_Proxy( 
    IUmiPropList * This,
    /* [in] */ LPCWSTR pszName,
    /* [in] */ ULONG uFlags);


void __RPC_STUB IUmiPropList_Delete_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IUmiPropList_GetProps_Proxy( 
    IUmiPropList * This,
    /* [in] */ LPCWSTR *pszNames,
    /* [in] */ ULONG uNameCount,
    /* [in] */ ULONG uFlags,
    /* [out] */ UMI_PROPERTY_VALUES **pProps);


void __RPC_STUB IUmiPropList_GetProps_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IUmiPropList_PutProps_Proxy( 
    IUmiPropList * This,
    /* [in] */ LPCWSTR *pszNames,
    /* [in] */ ULONG uNameCount,
    /* [in] */ ULONG uFlags,
    /* [in] */ UMI_PROPERTY_VALUES *pProps);


void __RPC_STUB IUmiPropList_PutProps_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IUmiPropList_PutFrom_Proxy( 
    IUmiPropList * This,
    /* [in] */ LPCWSTR pszName,
    /* [in] */ ULONG uFlags,
    /* [in] */ ULONG uBufferLength,
    /* [in] */ LPVOID pExistingMem);


void __RPC_STUB IUmiPropList_PutFrom_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IUmiPropList_INTERFACE_DEFINED__ */


#ifndef __IUmiBaseObject_INTERFACE_DEFINED__
#define __IUmiBaseObject_INTERFACE_DEFINED__

/* interface IUmiBaseObject */
/* [uuid][local][object] */ 


EXTERN_C const IID IID_IUmiBaseObject;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("12575a7c-d9db-11d3-a11f-00105a1f515a")
    IUmiBaseObject : public IUmiPropList
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetLastStatus( 
            /* [in] */ ULONG uFlags,
            /* [out] */ ULONG *puSpecificStatus,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ LPVOID *pStatusObj) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetInterfacePropList( 
            /* [in] */ ULONG uFlags,
            /* [out] */ IUmiPropList **pPropList) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IUmiBaseObjectVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUmiBaseObject * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUmiBaseObject * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUmiBaseObject * This);
        
        HRESULT ( STDMETHODCALLTYPE *Put )( 
            IUmiBaseObject * This,
            /* [in] */ LPCWSTR pszName,
            /* [in] */ ULONG uFlags,
            /* [in] */ UMI_PROPERTY_VALUES *pProp);
        
        HRESULT ( STDMETHODCALLTYPE *Get )( 
            IUmiBaseObject * This,
            /* [in] */ LPCWSTR pszName,
            /* [in] */ ULONG uFlags,
            /* [out] */ UMI_PROPERTY_VALUES **pProp);
        
        HRESULT ( STDMETHODCALLTYPE *GetAt )( 
            IUmiBaseObject * This,
            /* [in] */ LPCWSTR pszName,
            /* [in] */ ULONG uFlags,
            /* [in] */ ULONG uBufferLength,
            /* [out] */ LPVOID pExistingMem);
        
        HRESULT ( STDMETHODCALLTYPE *GetAs )( 
            IUmiBaseObject * This,
            /* [in] */ LPCWSTR pszName,
            /* [in] */ ULONG uFlags,
            /* [in] */ ULONG uCoercionType,
            /* [out] */ UMI_PROPERTY_VALUES **pProp);
        
        HRESULT ( STDMETHODCALLTYPE *FreeMemory )( 
            IUmiBaseObject * This,
            ULONG uReserved,
            LPVOID pMem);
        
        HRESULT ( STDMETHODCALLTYPE *Delete )( 
            IUmiBaseObject * This,
            /* [in] */ LPCWSTR pszName,
            /* [in] */ ULONG uFlags);
        
        HRESULT ( STDMETHODCALLTYPE *GetProps )( 
            IUmiBaseObject * This,
            /* [in] */ LPCWSTR *pszNames,
            /* [in] */ ULONG uNameCount,
            /* [in] */ ULONG uFlags,
            /* [out] */ UMI_PROPERTY_VALUES **pProps);
        
        HRESULT ( STDMETHODCALLTYPE *PutProps )( 
            IUmiBaseObject * This,
            /* [in] */ LPCWSTR *pszNames,
            /* [in] */ ULONG uNameCount,
            /* [in] */ ULONG uFlags,
            /* [in] */ UMI_PROPERTY_VALUES *pProps);
        
        HRESULT ( STDMETHODCALLTYPE *PutFrom )( 
            IUmiBaseObject * This,
            /* [in] */ LPCWSTR pszName,
            /* [in] */ ULONG uFlags,
            /* [in] */ ULONG uBufferLength,
            /* [in] */ LPVOID pExistingMem);
        
        HRESULT ( STDMETHODCALLTYPE *GetLastStatus )( 
            IUmiBaseObject * This,
            /* [in] */ ULONG uFlags,
            /* [out] */ ULONG *puSpecificStatus,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ LPVOID *pStatusObj);
        
        HRESULT ( STDMETHODCALLTYPE *GetInterfacePropList )( 
            IUmiBaseObject * This,
            /* [in] */ ULONG uFlags,
            /* [out] */ IUmiPropList **pPropList);
        
        END_INTERFACE
    } IUmiBaseObjectVtbl;

    interface IUmiBaseObject
    {
        CONST_VTBL struct IUmiBaseObjectVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUmiBaseObject_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IUmiBaseObject_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IUmiBaseObject_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IUmiBaseObject_Put(This,pszName,uFlags,pProp)	\
    (This)->lpVtbl -> Put(This,pszName,uFlags,pProp)

#define IUmiBaseObject_Get(This,pszName,uFlags,pProp)	\
    (This)->lpVtbl -> Get(This,pszName,uFlags,pProp)

#define IUmiBaseObject_GetAt(This,pszName,uFlags,uBufferLength,pExistingMem)	\
    (This)->lpVtbl -> GetAt(This,pszName,uFlags,uBufferLength,pExistingMem)

#define IUmiBaseObject_GetAs(This,pszName,uFlags,uCoercionType,pProp)	\
    (This)->lpVtbl -> GetAs(This,pszName,uFlags,uCoercionType,pProp)

#define IUmiBaseObject_FreeMemory(This,uReserved,pMem)	\
    (This)->lpVtbl -> FreeMemory(This,uReserved,pMem)

#define IUmiBaseObject_Delete(This,pszName,uFlags)	\
    (This)->lpVtbl -> Delete(This,pszName,uFlags)

#define IUmiBaseObject_GetProps(This,pszNames,uNameCount,uFlags,pProps)	\
    (This)->lpVtbl -> GetProps(This,pszNames,uNameCount,uFlags,pProps)

#define IUmiBaseObject_PutProps(This,pszNames,uNameCount,uFlags,pProps)	\
    (This)->lpVtbl -> PutProps(This,pszNames,uNameCount,uFlags,pProps)

#define IUmiBaseObject_PutFrom(This,pszName,uFlags,uBufferLength,pExistingMem)	\
    (This)->lpVtbl -> PutFrom(This,pszName,uFlags,uBufferLength,pExistingMem)


#define IUmiBaseObject_GetLastStatus(This,uFlags,puSpecificStatus,riid,pStatusObj)	\
    (This)->lpVtbl -> GetLastStatus(This,uFlags,puSpecificStatus,riid,pStatusObj)

#define IUmiBaseObject_GetInterfacePropList(This,uFlags,pPropList)	\
    (This)->lpVtbl -> GetInterfacePropList(This,uFlags,pPropList)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IUmiBaseObject_GetLastStatus_Proxy( 
    IUmiBaseObject * This,
    /* [in] */ ULONG uFlags,
    /* [out] */ ULONG *puSpecificStatus,
    /* [in] */ REFIID riid,
    /* [iid_is][out] */ LPVOID *pStatusObj);


void __RPC_STUB IUmiBaseObject_GetLastStatus_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IUmiBaseObject_GetInterfacePropList_Proxy( 
    IUmiBaseObject * This,
    /* [in] */ ULONG uFlags,
    /* [out] */ IUmiPropList **pPropList);


void __RPC_STUB IUmiBaseObject_GetInterfacePropList_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IUmiBaseObject_INTERFACE_DEFINED__ */


#ifndef __IUmiObject_INTERFACE_DEFINED__
#define __IUmiObject_INTERFACE_DEFINED__

/* interface IUmiObject */
/* [uuid][local][object] */ 


EXTERN_C const IID IID_IUmiObject;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("5ed7ee23-64a4-11d3-a0da-00105a1f515a")
    IUmiObject : public IUmiBaseObject
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Clone( 
            /* [in] */ ULONG uFlags,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ LPVOID *pCopy) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CopyTo( 
            /* [in] */ ULONG uFlags,
            /* [in] */ IUmiURL *pURL,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ LPVOID *pCopy) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Refresh( 
            /* [in] */ ULONG uFlags,
            /* [in] */ ULONG uNameCount,
            /* [in] */ LPWSTR *pszNames) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Commit( 
            /* [in] */ ULONG uFlags) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IUmiObjectVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUmiObject * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUmiObject * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUmiObject * This);
        
        HRESULT ( STDMETHODCALLTYPE *Put )( 
            IUmiObject * This,
            /* [in] */ LPCWSTR pszName,
            /* [in] */ ULONG uFlags,
            /* [in] */ UMI_PROPERTY_VALUES *pProp);
        
        HRESULT ( STDMETHODCALLTYPE *Get )( 
            IUmiObject * This,
            /* [in] */ LPCWSTR pszName,
            /* [in] */ ULONG uFlags,
            /* [out] */ UMI_PROPERTY_VALUES **pProp);
        
        HRESULT ( STDMETHODCALLTYPE *GetAt )( 
            IUmiObject * This,
            /* [in] */ LPCWSTR pszName,
            /* [in] */ ULONG uFlags,
            /* [in] */ ULONG uBufferLength,
            /* [out] */ LPVOID pExistingMem);
        
        HRESULT ( STDMETHODCALLTYPE *GetAs )( 
            IUmiObject * This,
            /* [in] */ LPCWSTR pszName,
            /* [in] */ ULONG uFlags,
            /* [in] */ ULONG uCoercionType,
            /* [out] */ UMI_PROPERTY_VALUES **pProp);
        
        HRESULT ( STDMETHODCALLTYPE *FreeMemory )( 
            IUmiObject * This,
            ULONG uReserved,
            LPVOID pMem);
        
        HRESULT ( STDMETHODCALLTYPE *Delete )( 
            IUmiObject * This,
            /* [in] */ LPCWSTR pszName,
            /* [in] */ ULONG uFlags);
        
        HRESULT ( STDMETHODCALLTYPE *GetProps )( 
            IUmiObject * This,
            /* [in] */ LPCWSTR *pszNames,
            /* [in] */ ULONG uNameCount,
            /* [in] */ ULONG uFlags,
            /* [out] */ UMI_PROPERTY_VALUES **pProps);
        
        HRESULT ( STDMETHODCALLTYPE *PutProps )( 
            IUmiObject * This,
            /* [in] */ LPCWSTR *pszNames,
            /* [in] */ ULONG uNameCount,
            /* [in] */ ULONG uFlags,
            /* [in] */ UMI_PROPERTY_VALUES *pProps);
        
        HRESULT ( STDMETHODCALLTYPE *PutFrom )( 
            IUmiObject * This,
            /* [in] */ LPCWSTR pszName,
            /* [in] */ ULONG uFlags,
            /* [in] */ ULONG uBufferLength,
            /* [in] */ LPVOID pExistingMem);
        
        HRESULT ( STDMETHODCALLTYPE *GetLastStatus )( 
            IUmiObject * This,
            /* [in] */ ULONG uFlags,
            /* [out] */ ULONG *puSpecificStatus,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ LPVOID *pStatusObj);
        
        HRESULT ( STDMETHODCALLTYPE *GetInterfacePropList )( 
            IUmiObject * This,
            /* [in] */ ULONG uFlags,
            /* [out] */ IUmiPropList **pPropList);
        
        HRESULT ( STDMETHODCALLTYPE *Clone )( 
            IUmiObject * This,
            /* [in] */ ULONG uFlags,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ LPVOID *pCopy);
        
        HRESULT ( STDMETHODCALLTYPE *CopyTo )( 
            IUmiObject * This,
            /* [in] */ ULONG uFlags,
            /* [in] */ IUmiURL *pURL,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ LPVOID *pCopy);
        
        HRESULT ( STDMETHODCALLTYPE *Refresh )( 
            IUmiObject * This,
            /* [in] */ ULONG uFlags,
            /* [in] */ ULONG uNameCount,
            /* [in] */ LPWSTR *pszNames);
        
        HRESULT ( STDMETHODCALLTYPE *Commit )( 
            IUmiObject * This,
            /* [in] */ ULONG uFlags);
        
        END_INTERFACE
    } IUmiObjectVtbl;

    interface IUmiObject
    {
        CONST_VTBL struct IUmiObjectVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUmiObject_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IUmiObject_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IUmiObject_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IUmiObject_Put(This,pszName,uFlags,pProp)	\
    (This)->lpVtbl -> Put(This,pszName,uFlags,pProp)

#define IUmiObject_Get(This,pszName,uFlags,pProp)	\
    (This)->lpVtbl -> Get(This,pszName,uFlags,pProp)

#define IUmiObject_GetAt(This,pszName,uFlags,uBufferLength,pExistingMem)	\
    (This)->lpVtbl -> GetAt(This,pszName,uFlags,uBufferLength,pExistingMem)

#define IUmiObject_GetAs(This,pszName,uFlags,uCoercionType,pProp)	\
    (This)->lpVtbl -> GetAs(This,pszName,uFlags,uCoercionType,pProp)

#define IUmiObject_FreeMemory(This,uReserved,pMem)	\
    (This)->lpVtbl -> FreeMemory(This,uReserved,pMem)

#define IUmiObject_Delete(This,pszName,uFlags)	\
    (This)->lpVtbl -> Delete(This,pszName,uFlags)

#define IUmiObject_GetProps(This,pszNames,uNameCount,uFlags,pProps)	\
    (This)->lpVtbl -> GetProps(This,pszNames,uNameCount,uFlags,pProps)

#define IUmiObject_PutProps(This,pszNames,uNameCount,uFlags,pProps)	\
    (This)->lpVtbl -> PutProps(This,pszNames,uNameCount,uFlags,pProps)

#define IUmiObject_PutFrom(This,pszName,uFlags,uBufferLength,pExistingMem)	\
    (This)->lpVtbl -> PutFrom(This,pszName,uFlags,uBufferLength,pExistingMem)


#define IUmiObject_GetLastStatus(This,uFlags,puSpecificStatus,riid,pStatusObj)	\
    (This)->lpVtbl -> GetLastStatus(This,uFlags,puSpecificStatus,riid,pStatusObj)

#define IUmiObject_GetInterfacePropList(This,uFlags,pPropList)	\
    (This)->lpVtbl -> GetInterfacePropList(This,uFlags,pPropList)


#define IUmiObject_Clone(This,uFlags,riid,pCopy)	\
    (This)->lpVtbl -> Clone(This,uFlags,riid,pCopy)

#define IUmiObject_CopyTo(This,uFlags,pURL,riid,pCopy)	\
    (This)->lpVtbl -> CopyTo(This,uFlags,pURL,riid,pCopy)

#define IUmiObject_Refresh(This,uFlags,uNameCount,pszNames)	\
    (This)->lpVtbl -> Refresh(This,uFlags,uNameCount,pszNames)

#define IUmiObject_Commit(This,uFlags)	\
    (This)->lpVtbl -> Commit(This,uFlags)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IUmiObject_Clone_Proxy( 
    IUmiObject * This,
    /* [in] */ ULONG uFlags,
    /* [in] */ REFIID riid,
    /* [iid_is][out] */ LPVOID *pCopy);


void __RPC_STUB IUmiObject_Clone_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IUmiObject_CopyTo_Proxy( 
    IUmiObject * This,
    /* [in] */ ULONG uFlags,
    /* [in] */ IUmiURL *pURL,
    /* [in] */ REFIID riid,
    /* [iid_is][out] */ LPVOID *pCopy);


void __RPC_STUB IUmiObject_CopyTo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IUmiObject_Refresh_Proxy( 
    IUmiObject * This,
    /* [in] */ ULONG uFlags,
    /* [in] */ ULONG uNameCount,
    /* [in] */ LPWSTR *pszNames);


void __RPC_STUB IUmiObject_Refresh_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IUmiObject_Commit_Proxy( 
    IUmiObject * This,
    /* [in] */ ULONG uFlags);


void __RPC_STUB IUmiObject_Commit_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IUmiObject_INTERFACE_DEFINED__ */


#ifndef __IUmiConnection_INTERFACE_DEFINED__
#define __IUmiConnection_INTERFACE_DEFINED__

/* interface IUmiConnection */
/* [uuid][local][object] */ 


EXTERN_C const IID IID_IUmiConnection;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("5ed7ee20-64a4-11d3-a0da-00105a1f515a")
    IUmiConnection : public IUmiBaseObject
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Open( 
            /* [in] */ IUmiURL *pURL,
            /* [in] */ ULONG uFlags,
            /* [in] */ REFIID TargetIID,
            /* [iid_is][out] */ void **ppvRes) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IUmiConnectionVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUmiConnection * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUmiConnection * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUmiConnection * This);
        
        HRESULT ( STDMETHODCALLTYPE *Put )( 
            IUmiConnection * This,
            /* [in] */ LPCWSTR pszName,
            /* [in] */ ULONG uFlags,
            /* [in] */ UMI_PROPERTY_VALUES *pProp);
        
        HRESULT ( STDMETHODCALLTYPE *Get )( 
            IUmiConnection * This,
            /* [in] */ LPCWSTR pszName,
            /* [in] */ ULONG uFlags,
            /* [out] */ UMI_PROPERTY_VALUES **pProp);
        
        HRESULT ( STDMETHODCALLTYPE *GetAt )( 
            IUmiConnection * This,
            /* [in] */ LPCWSTR pszName,
            /* [in] */ ULONG uFlags,
            /* [in] */ ULONG uBufferLength,
            /* [out] */ LPVOID pExistingMem);
        
        HRESULT ( STDMETHODCALLTYPE *GetAs )( 
            IUmiConnection * This,
            /* [in] */ LPCWSTR pszName,
            /* [in] */ ULONG uFlags,
            /* [in] */ ULONG uCoercionType,
            /* [out] */ UMI_PROPERTY_VALUES **pProp);
        
        HRESULT ( STDMETHODCALLTYPE *FreeMemory )( 
            IUmiConnection * This,
            ULONG uReserved,
            LPVOID pMem);
        
        HRESULT ( STDMETHODCALLTYPE *Delete )( 
            IUmiConnection * This,
            /* [in] */ LPCWSTR pszName,
            /* [in] */ ULONG uFlags);
        
        HRESULT ( STDMETHODCALLTYPE *GetProps )( 
            IUmiConnection * This,
            /* [in] */ LPCWSTR *pszNames,
            /* [in] */ ULONG uNameCount,
            /* [in] */ ULONG uFlags,
            /* [out] */ UMI_PROPERTY_VALUES **pProps);
        
        HRESULT ( STDMETHODCALLTYPE *PutProps )( 
            IUmiConnection * This,
            /* [in] */ LPCWSTR *pszNames,
            /* [in] */ ULONG uNameCount,
            /* [in] */ ULONG uFlags,
            /* [in] */ UMI_PROPERTY_VALUES *pProps);
        
        HRESULT ( STDMETHODCALLTYPE *PutFrom )( 
            IUmiConnection * This,
            /* [in] */ LPCWSTR pszName,
            /* [in] */ ULONG uFlags,
            /* [in] */ ULONG uBufferLength,
            /* [in] */ LPVOID pExistingMem);
        
        HRESULT ( STDMETHODCALLTYPE *GetLastStatus )( 
            IUmiConnection * This,
            /* [in] */ ULONG uFlags,
            /* [out] */ ULONG *puSpecificStatus,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ LPVOID *pStatusObj);
        
        HRESULT ( STDMETHODCALLTYPE *GetInterfacePropList )( 
            IUmiConnection * This,
            /* [in] */ ULONG uFlags,
            /* [out] */ IUmiPropList **pPropList);
        
        HRESULT ( STDMETHODCALLTYPE *Open )( 
            IUmiConnection * This,
            /* [in] */ IUmiURL *pURL,
            /* [in] */ ULONG uFlags,
            /* [in] */ REFIID TargetIID,
            /* [iid_is][out] */ void **ppvRes);
        
        END_INTERFACE
    } IUmiConnectionVtbl;

    interface IUmiConnection
    {
        CONST_VTBL struct IUmiConnectionVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUmiConnection_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IUmiConnection_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IUmiConnection_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IUmiConnection_Put(This,pszName,uFlags,pProp)	\
    (This)->lpVtbl -> Put(This,pszName,uFlags,pProp)

#define IUmiConnection_Get(This,pszName,uFlags,pProp)	\
    (This)->lpVtbl -> Get(This,pszName,uFlags,pProp)

#define IUmiConnection_GetAt(This,pszName,uFlags,uBufferLength,pExistingMem)	\
    (This)->lpVtbl -> GetAt(This,pszName,uFlags,uBufferLength,pExistingMem)

#define IUmiConnection_GetAs(This,pszName,uFlags,uCoercionType,pProp)	\
    (This)->lpVtbl -> GetAs(This,pszName,uFlags,uCoercionType,pProp)

#define IUmiConnection_FreeMemory(This,uReserved,pMem)	\
    (This)->lpVtbl -> FreeMemory(This,uReserved,pMem)

#define IUmiConnection_Delete(This,pszName,uFlags)	\
    (This)->lpVtbl -> Delete(This,pszName,uFlags)

#define IUmiConnection_GetProps(This,pszNames,uNameCount,uFlags,pProps)	\
    (This)->lpVtbl -> GetProps(This,pszNames,uNameCount,uFlags,pProps)

#define IUmiConnection_PutProps(This,pszNames,uNameCount,uFlags,pProps)	\
    (This)->lpVtbl -> PutProps(This,pszNames,uNameCount,uFlags,pProps)

#define IUmiConnection_PutFrom(This,pszName,uFlags,uBufferLength,pExistingMem)	\
    (This)->lpVtbl -> PutFrom(This,pszName,uFlags,uBufferLength,pExistingMem)


#define IUmiConnection_GetLastStatus(This,uFlags,puSpecificStatus,riid,pStatusObj)	\
    (This)->lpVtbl -> GetLastStatus(This,uFlags,puSpecificStatus,riid,pStatusObj)

#define IUmiConnection_GetInterfacePropList(This,uFlags,pPropList)	\
    (This)->lpVtbl -> GetInterfacePropList(This,uFlags,pPropList)


#define IUmiConnection_Open(This,pURL,uFlags,TargetIID,ppvRes)	\
    (This)->lpVtbl -> Open(This,pURL,uFlags,TargetIID,ppvRes)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IUmiConnection_Open_Proxy( 
    IUmiConnection * This,
    /* [in] */ IUmiURL *pURL,
    /* [in] */ ULONG uFlags,
    /* [in] */ REFIID TargetIID,
    /* [iid_is][out] */ void **ppvRes);


void __RPC_STUB IUmiConnection_Open_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IUmiConnection_INTERFACE_DEFINED__ */


#ifndef __IUmiContainer_INTERFACE_DEFINED__
#define __IUmiContainer_INTERFACE_DEFINED__

/* interface IUmiContainer */
/* [uuid][local][object] */ 


EXTERN_C const IID IID_IUmiContainer;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("5ed7ee21-64a4-11d3-a0da-00105a1f515a")
    IUmiContainer : public IUmiObject
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Open( 
            /* [in] */ IUmiURL *pURL,
            /* [in] */ ULONG uFlags,
            /* [in] */ REFIID TargetIID,
            /* [iid_is][out] */ void **ppvRes) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE PutObject( 
            /* [in] */ ULONG uFlags,
            /* [in] */ REFIID TargetIID,
            /* [iid_is][out][in] */ void *pObj) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE DeleteObject( 
            /* [in] */ IUmiURL *pURL,
            /* [optional][in] */ ULONG uFlags) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Create( 
            /* [in] */ IUmiURL *pURL,
            /* [in] */ ULONG uFlags,
            /* [out] */ IUmiObject **pNewObj) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Move( 
            /* [in] */ ULONG uFlags,
            /* [in] */ IUmiURL *pOldURL,
            /* [in] */ IUmiURL *pNewURL) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CreateEnum( 
            /* [in] */ IUmiURL *pszEnumContext,
            /* [in] */ ULONG uFlags,
            /* [in] */ REFIID TargetIID,
            /* [iid_is][out] */ void **ppvEnum) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ExecQuery( 
            /* [in] */ IUmiQuery *pQuery,
            /* [in] */ ULONG uFlags,
            /* [in] */ REFIID TargetIID,
            /* [iid_is][out] */ void **ppResult) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IUmiContainerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUmiContainer * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUmiContainer * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUmiContainer * This);
        
        HRESULT ( STDMETHODCALLTYPE *Put )( 
            IUmiContainer * This,
            /* [in] */ LPCWSTR pszName,
            /* [in] */ ULONG uFlags,
            /* [in] */ UMI_PROPERTY_VALUES *pProp);
        
        HRESULT ( STDMETHODCALLTYPE *Get )( 
            IUmiContainer * This,
            /* [in] */ LPCWSTR pszName,
            /* [in] */ ULONG uFlags,
            /* [out] */ UMI_PROPERTY_VALUES **pProp);
        
        HRESULT ( STDMETHODCALLTYPE *GetAt )( 
            IUmiContainer * This,
            /* [in] */ LPCWSTR pszName,
            /* [in] */ ULONG uFlags,
            /* [in] */ ULONG uBufferLength,
            /* [out] */ LPVOID pExistingMem);
        
        HRESULT ( STDMETHODCALLTYPE *GetAs )( 
            IUmiContainer * This,
            /* [in] */ LPCWSTR pszName,
            /* [in] */ ULONG uFlags,
            /* [in] */ ULONG uCoercionType,
            /* [out] */ UMI_PROPERTY_VALUES **pProp);
        
        HRESULT ( STDMETHODCALLTYPE *FreeMemory )( 
            IUmiContainer * This,
            ULONG uReserved,
            LPVOID pMem);
        
        HRESULT ( STDMETHODCALLTYPE *Delete )( 
            IUmiContainer * This,
            /* [in] */ LPCWSTR pszName,
            /* [in] */ ULONG uFlags);
        
        HRESULT ( STDMETHODCALLTYPE *GetProps )( 
            IUmiContainer * This,
            /* [in] */ LPCWSTR *pszNames,
            /* [in] */ ULONG uNameCount,
            /* [in] */ ULONG uFlags,
            /* [out] */ UMI_PROPERTY_VALUES **pProps);
        
        HRESULT ( STDMETHODCALLTYPE *PutProps )( 
            IUmiContainer * This,
            /* [in] */ LPCWSTR *pszNames,
            /* [in] */ ULONG uNameCount,
            /* [in] */ ULONG uFlags,
            /* [in] */ UMI_PROPERTY_VALUES *pProps);
        
        HRESULT ( STDMETHODCALLTYPE *PutFrom )( 
            IUmiContainer * This,
            /* [in] */ LPCWSTR pszName,
            /* [in] */ ULONG uFlags,
            /* [in] */ ULONG uBufferLength,
            /* [in] */ LPVOID pExistingMem);
        
        HRESULT ( STDMETHODCALLTYPE *GetLastStatus )( 
            IUmiContainer * This,
            /* [in] */ ULONG uFlags,
            /* [out] */ ULONG *puSpecificStatus,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ LPVOID *pStatusObj);
        
        HRESULT ( STDMETHODCALLTYPE *GetInterfacePropList )( 
            IUmiContainer * This,
            /* [in] */ ULONG uFlags,
            /* [out] */ IUmiPropList **pPropList);
        
        HRESULT ( STDMETHODCALLTYPE *Clone )( 
            IUmiContainer * This,
            /* [in] */ ULONG uFlags,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ LPVOID *pCopy);
        
        HRESULT ( STDMETHODCALLTYPE *CopyTo )( 
            IUmiContainer * This,
            /* [in] */ ULONG uFlags,
            /* [in] */ IUmiURL *pURL,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ LPVOID *pCopy);
        
        HRESULT ( STDMETHODCALLTYPE *Refresh )( 
            IUmiContainer * This,
            /* [in] */ ULONG uFlags,
            /* [in] */ ULONG uNameCount,
            /* [in] */ LPWSTR *pszNames);
        
        HRESULT ( STDMETHODCALLTYPE *Commit )( 
            IUmiContainer * This,
            /* [in] */ ULONG uFlags);
        
        HRESULT ( STDMETHODCALLTYPE *Open )( 
            IUmiContainer * This,
            /* [in] */ IUmiURL *pURL,
            /* [in] */ ULONG uFlags,
            /* [in] */ REFIID TargetIID,
            /* [iid_is][out] */ void **ppvRes);
        
        HRESULT ( STDMETHODCALLTYPE *PutObject )( 
            IUmiContainer * This,
            /* [in] */ ULONG uFlags,
            /* [in] */ REFIID TargetIID,
            /* [iid_is][out][in] */ void *pObj);
        
        HRESULT ( STDMETHODCALLTYPE *DeleteObject )( 
            IUmiContainer * This,
            /* [in] */ IUmiURL *pURL,
            /* [optional][in] */ ULONG uFlags);
        
        HRESULT ( STDMETHODCALLTYPE *Create )( 
            IUmiContainer * This,
            /* [in] */ IUmiURL *pURL,
            /* [in] */ ULONG uFlags,
            /* [out] */ IUmiObject **pNewObj);
        
        HRESULT ( STDMETHODCALLTYPE *Move )( 
            IUmiContainer * This,
            /* [in] */ ULONG uFlags,
            /* [in] */ IUmiURL *pOldURL,
            /* [in] */ IUmiURL *pNewURL);
        
        HRESULT ( STDMETHODCALLTYPE *CreateEnum )( 
            IUmiContainer * This,
            /* [in] */ IUmiURL *pszEnumContext,
            /* [in] */ ULONG uFlags,
            /* [in] */ REFIID TargetIID,
            /* [iid_is][out] */ void **ppvEnum);
        
        HRESULT ( STDMETHODCALLTYPE *ExecQuery )( 
            IUmiContainer * This,
            /* [in] */ IUmiQuery *pQuery,
            /* [in] */ ULONG uFlags,
            /* [in] */ REFIID TargetIID,
            /* [iid_is][out] */ void **ppResult);
        
        END_INTERFACE
    } IUmiContainerVtbl;

    interface IUmiContainer
    {
        CONST_VTBL struct IUmiContainerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUmiContainer_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IUmiContainer_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IUmiContainer_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IUmiContainer_Put(This,pszName,uFlags,pProp)	\
    (This)->lpVtbl -> Put(This,pszName,uFlags,pProp)

#define IUmiContainer_Get(This,pszName,uFlags,pProp)	\
    (This)->lpVtbl -> Get(This,pszName,uFlags,pProp)

#define IUmiContainer_GetAt(This,pszName,uFlags,uBufferLength,pExistingMem)	\
    (This)->lpVtbl -> GetAt(This,pszName,uFlags,uBufferLength,pExistingMem)

#define IUmiContainer_GetAs(This,pszName,uFlags,uCoercionType,pProp)	\
    (This)->lpVtbl -> GetAs(This,pszName,uFlags,uCoercionType,pProp)

#define IUmiContainer_FreeMemory(This,uReserved,pMem)	\
    (This)->lpVtbl -> FreeMemory(This,uReserved,pMem)

#define IUmiContainer_Delete(This,pszName,uFlags)	\
    (This)->lpVtbl -> Delete(This,pszName,uFlags)

#define IUmiContainer_GetProps(This,pszNames,uNameCount,uFlags,pProps)	\
    (This)->lpVtbl -> GetProps(This,pszNames,uNameCount,uFlags,pProps)

#define IUmiContainer_PutProps(This,pszNames,uNameCount,uFlags,pProps)	\
    (This)->lpVtbl -> PutProps(This,pszNames,uNameCount,uFlags,pProps)

#define IUmiContainer_PutFrom(This,pszName,uFlags,uBufferLength,pExistingMem)	\
    (This)->lpVtbl -> PutFrom(This,pszName,uFlags,uBufferLength,pExistingMem)


#define IUmiContainer_GetLastStatus(This,uFlags,puSpecificStatus,riid,pStatusObj)	\
    (This)->lpVtbl -> GetLastStatus(This,uFlags,puSpecificStatus,riid,pStatusObj)

#define IUmiContainer_GetInterfacePropList(This,uFlags,pPropList)	\
    (This)->lpVtbl -> GetInterfacePropList(This,uFlags,pPropList)


#define IUmiContainer_Clone(This,uFlags,riid,pCopy)	\
    (This)->lpVtbl -> Clone(This,uFlags,riid,pCopy)

#define IUmiContainer_CopyTo(This,uFlags,pURL,riid,pCopy)	\
    (This)->lpVtbl -> CopyTo(This,uFlags,pURL,riid,pCopy)

#define IUmiContainer_Refresh(This,uFlags,uNameCount,pszNames)	\
    (This)->lpVtbl -> Refresh(This,uFlags,uNameCount,pszNames)

#define IUmiContainer_Commit(This,uFlags)	\
    (This)->lpVtbl -> Commit(This,uFlags)


#define IUmiContainer_Open(This,pURL,uFlags,TargetIID,ppvRes)	\
    (This)->lpVtbl -> Open(This,pURL,uFlags,TargetIID,ppvRes)

#define IUmiContainer_PutObject(This,uFlags,TargetIID,pObj)	\
    (This)->lpVtbl -> PutObject(This,uFlags,TargetIID,pObj)

#define IUmiContainer_DeleteObject(This,pURL,uFlags)	\
    (This)->lpVtbl -> DeleteObject(This,pURL,uFlags)

#define IUmiContainer_Create(This,pURL,uFlags,pNewObj)	\
    (This)->lpVtbl -> Create(This,pURL,uFlags,pNewObj)

#define IUmiContainer_Move(This,uFlags,pOldURL,pNewURL)	\
    (This)->lpVtbl -> Move(This,uFlags,pOldURL,pNewURL)

#define IUmiContainer_CreateEnum(This,pszEnumContext,uFlags,TargetIID,ppvEnum)	\
    (This)->lpVtbl -> CreateEnum(This,pszEnumContext,uFlags,TargetIID,ppvEnum)

#define IUmiContainer_ExecQuery(This,pQuery,uFlags,TargetIID,ppResult)	\
    (This)->lpVtbl -> ExecQuery(This,pQuery,uFlags,TargetIID,ppResult)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IUmiContainer_Open_Proxy( 
    IUmiContainer * This,
    /* [in] */ IUmiURL *pURL,
    /* [in] */ ULONG uFlags,
    /* [in] */ REFIID TargetIID,
    /* [iid_is][out] */ void **ppvRes);


void __RPC_STUB IUmiContainer_Open_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IUmiContainer_PutObject_Proxy( 
    IUmiContainer * This,
    /* [in] */ ULONG uFlags,
    /* [in] */ REFIID TargetIID,
    /* [iid_is][out][in] */ void *pObj);


void __RPC_STUB IUmiContainer_PutObject_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IUmiContainer_DeleteObject_Proxy( 
    IUmiContainer * This,
    /* [in] */ IUmiURL *pURL,
    /* [optional][in] */ ULONG uFlags);


void __RPC_STUB IUmiContainer_DeleteObject_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IUmiContainer_Create_Proxy( 
    IUmiContainer * This,
    /* [in] */ IUmiURL *pURL,
    /* [in] */ ULONG uFlags,
    /* [out] */ IUmiObject **pNewObj);


void __RPC_STUB IUmiContainer_Create_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IUmiContainer_Move_Proxy( 
    IUmiContainer * This,
    /* [in] */ ULONG uFlags,
    /* [in] */ IUmiURL *pOldURL,
    /* [in] */ IUmiURL *pNewURL);


void __RPC_STUB IUmiContainer_Move_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IUmiContainer_CreateEnum_Proxy( 
    IUmiContainer * This,
    /* [in] */ IUmiURL *pszEnumContext,
    /* [in] */ ULONG uFlags,
    /* [in] */ REFIID TargetIID,
    /* [iid_is][out] */ void **ppvEnum);


void __RPC_STUB IUmiContainer_CreateEnum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IUmiContainer_ExecQuery_Proxy( 
    IUmiContainer * This,
    /* [in] */ IUmiQuery *pQuery,
    /* [in] */ ULONG uFlags,
    /* [in] */ REFIID TargetIID,
    /* [iid_is][out] */ void **ppResult);


void __RPC_STUB IUmiContainer_ExecQuery_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IUmiContainer_INTERFACE_DEFINED__ */


#ifndef __IUmiCursor_INTERFACE_DEFINED__
#define __IUmiCursor_INTERFACE_DEFINED__

/* interface IUmiCursor */
/* [uuid][local][object] */ 


EXTERN_C const IID IID_IUmiCursor;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("5ed7ee26-64a4-11d3-a0da-00105a1f515a")
    IUmiCursor : public IUmiBaseObject
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE SetIID( 
            /* [in] */ REFIID riid) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Reset( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Next( 
            /* [in] */ ULONG uNumRequested,
            /* [out] */ ULONG *puNumReturned,
            /* [length_is][size_is][out] */ LPVOID *pObjects) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Count( 
            /* [out] */ ULONG *puNumObjects) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Previous( 
            /* [in] */ ULONG uFlags,
            /* [out] */ LPVOID *pObj) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IUmiCursorVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUmiCursor * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUmiCursor * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUmiCursor * This);
        
        HRESULT ( STDMETHODCALLTYPE *Put )( 
            IUmiCursor * This,
            /* [in] */ LPCWSTR pszName,
            /* [in] */ ULONG uFlags,
            /* [in] */ UMI_PROPERTY_VALUES *pProp);
        
        HRESULT ( STDMETHODCALLTYPE *Get )( 
            IUmiCursor * This,
            /* [in] */ LPCWSTR pszName,
            /* [in] */ ULONG uFlags,
            /* [out] */ UMI_PROPERTY_VALUES **pProp);
        
        HRESULT ( STDMETHODCALLTYPE *GetAt )( 
            IUmiCursor * This,
            /* [in] */ LPCWSTR pszName,
            /* [in] */ ULONG uFlags,
            /* [in] */ ULONG uBufferLength,
            /* [out] */ LPVOID pExistingMem);
        
        HRESULT ( STDMETHODCALLTYPE *GetAs )( 
            IUmiCursor * This,
            /* [in] */ LPCWSTR pszName,
            /* [in] */ ULONG uFlags,
            /* [in] */ ULONG uCoercionType,
            /* [out] */ UMI_PROPERTY_VALUES **pProp);
        
        HRESULT ( STDMETHODCALLTYPE *FreeMemory )( 
            IUmiCursor * This,
            ULONG uReserved,
            LPVOID pMem);
        
        HRESULT ( STDMETHODCALLTYPE *Delete )( 
            IUmiCursor * This,
            /* [in] */ LPCWSTR pszName,
            /* [in] */ ULONG uFlags);
        
        HRESULT ( STDMETHODCALLTYPE *GetProps )( 
            IUmiCursor * This,
            /* [in] */ LPCWSTR *pszNames,
            /* [in] */ ULONG uNameCount,
            /* [in] */ ULONG uFlags,
            /* [out] */ UMI_PROPERTY_VALUES **pProps);
        
        HRESULT ( STDMETHODCALLTYPE *PutProps )( 
            IUmiCursor * This,
            /* [in] */ LPCWSTR *pszNames,
            /* [in] */ ULONG uNameCount,
            /* [in] */ ULONG uFlags,
            /* [in] */ UMI_PROPERTY_VALUES *pProps);
        
        HRESULT ( STDMETHODCALLTYPE *PutFrom )( 
            IUmiCursor * This,
            /* [in] */ LPCWSTR pszName,
            /* [in] */ ULONG uFlags,
            /* [in] */ ULONG uBufferLength,
            /* [in] */ LPVOID pExistingMem);
        
        HRESULT ( STDMETHODCALLTYPE *GetLastStatus )( 
            IUmiCursor * This,
            /* [in] */ ULONG uFlags,
            /* [out] */ ULONG *puSpecificStatus,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ LPVOID *pStatusObj);
        
        HRESULT ( STDMETHODCALLTYPE *GetInterfacePropList )( 
            IUmiCursor * This,
            /* [in] */ ULONG uFlags,
            /* [out] */ IUmiPropList **pPropList);
        
        HRESULT ( STDMETHODCALLTYPE *SetIID )( 
            IUmiCursor * This,
            /* [in] */ REFIID riid);
        
        HRESULT ( STDMETHODCALLTYPE *Reset )( 
            IUmiCursor * This);
        
        HRESULT ( STDMETHODCALLTYPE *Next )( 
            IUmiCursor * This,
            /* [in] */ ULONG uNumRequested,
            /* [out] */ ULONG *puNumReturned,
            /* [length_is][size_is][out] */ LPVOID *pObjects);
        
        HRESULT ( STDMETHODCALLTYPE *Count )( 
            IUmiCursor * This,
            /* [out] */ ULONG *puNumObjects);
        
        HRESULT ( STDMETHODCALLTYPE *Previous )( 
            IUmiCursor * This,
            /* [in] */ ULONG uFlags,
            /* [out] */ LPVOID *pObj);
        
        END_INTERFACE
    } IUmiCursorVtbl;

    interface IUmiCursor
    {
        CONST_VTBL struct IUmiCursorVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUmiCursor_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IUmiCursor_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IUmiCursor_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IUmiCursor_Put(This,pszName,uFlags,pProp)	\
    (This)->lpVtbl -> Put(This,pszName,uFlags,pProp)

#define IUmiCursor_Get(This,pszName,uFlags,pProp)	\
    (This)->lpVtbl -> Get(This,pszName,uFlags,pProp)

#define IUmiCursor_GetAt(This,pszName,uFlags,uBufferLength,pExistingMem)	\
    (This)->lpVtbl -> GetAt(This,pszName,uFlags,uBufferLength,pExistingMem)

#define IUmiCursor_GetAs(This,pszName,uFlags,uCoercionType,pProp)	\
    (This)->lpVtbl -> GetAs(This,pszName,uFlags,uCoercionType,pProp)

#define IUmiCursor_FreeMemory(This,uReserved,pMem)	\
    (This)->lpVtbl -> FreeMemory(This,uReserved,pMem)

#define IUmiCursor_Delete(This,pszName,uFlags)	\
    (This)->lpVtbl -> Delete(This,pszName,uFlags)

#define IUmiCursor_GetProps(This,pszNames,uNameCount,uFlags,pProps)	\
    (This)->lpVtbl -> GetProps(This,pszNames,uNameCount,uFlags,pProps)

#define IUmiCursor_PutProps(This,pszNames,uNameCount,uFlags,pProps)	\
    (This)->lpVtbl -> PutProps(This,pszNames,uNameCount,uFlags,pProps)

#define IUmiCursor_PutFrom(This,pszName,uFlags,uBufferLength,pExistingMem)	\
    (This)->lpVtbl -> PutFrom(This,pszName,uFlags,uBufferLength,pExistingMem)


#define IUmiCursor_GetLastStatus(This,uFlags,puSpecificStatus,riid,pStatusObj)	\
    (This)->lpVtbl -> GetLastStatus(This,uFlags,puSpecificStatus,riid,pStatusObj)

#define IUmiCursor_GetInterfacePropList(This,uFlags,pPropList)	\
    (This)->lpVtbl -> GetInterfacePropList(This,uFlags,pPropList)


#define IUmiCursor_SetIID(This,riid)	\
    (This)->lpVtbl -> SetIID(This,riid)

#define IUmiCursor_Reset(This)	\
    (This)->lpVtbl -> Reset(This)

#define IUmiCursor_Next(This,uNumRequested,puNumReturned,pObjects)	\
    (This)->lpVtbl -> Next(This,uNumRequested,puNumReturned,pObjects)

#define IUmiCursor_Count(This,puNumObjects)	\
    (This)->lpVtbl -> Count(This,puNumObjects)

#define IUmiCursor_Previous(This,uFlags,pObj)	\
    (This)->lpVtbl -> Previous(This,uFlags,pObj)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IUmiCursor_SetIID_Proxy( 
    IUmiCursor * This,
    /* [in] */ REFIID riid);


void __RPC_STUB IUmiCursor_SetIID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IUmiCursor_Reset_Proxy( 
    IUmiCursor * This);


void __RPC_STUB IUmiCursor_Reset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IUmiCursor_Next_Proxy( 
    IUmiCursor * This,
    /* [in] */ ULONG uNumRequested,
    /* [out] */ ULONG *puNumReturned,
    /* [length_is][size_is][out] */ LPVOID *pObjects);


void __RPC_STUB IUmiCursor_Next_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IUmiCursor_Count_Proxy( 
    IUmiCursor * This,
    /* [out] */ ULONG *puNumObjects);


void __RPC_STUB IUmiCursor_Count_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IUmiCursor_Previous_Proxy( 
    IUmiCursor * This,
    /* [in] */ ULONG uFlags,
    /* [out] */ LPVOID *pObj);


void __RPC_STUB IUmiCursor_Previous_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IUmiCursor_INTERFACE_DEFINED__ */


#ifndef __IUmiObjectSink_INTERFACE_DEFINED__
#define __IUmiObjectSink_INTERFACE_DEFINED__

/* interface IUmiObjectSink */
/* [uuid][local][object] */ 


EXTERN_C const IID IID_IUmiObjectSink;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("5ed7ee24-64a4-11d3-a0da-00105a1f515a")
    IUmiObjectSink : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Put( 
            /* [in] */ LONG lNumObjects,
            /* [size_is][in] */ IUmiObject **ppObjects) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetResult( 
            /* [in] */ HRESULT hResStatus,
            /* [in] */ ULONG uFlags,
            /* [in] */ IUnknown *pObject) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IUmiObjectSinkVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUmiObjectSink * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUmiObjectSink * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUmiObjectSink * This);
        
        HRESULT ( STDMETHODCALLTYPE *Put )( 
            IUmiObjectSink * This,
            /* [in] */ LONG lNumObjects,
            /* [size_is][in] */ IUmiObject **ppObjects);
        
        HRESULT ( STDMETHODCALLTYPE *SetResult )( 
            IUmiObjectSink * This,
            /* [in] */ HRESULT hResStatus,
            /* [in] */ ULONG uFlags,
            /* [in] */ IUnknown *pObject);
        
        END_INTERFACE
    } IUmiObjectSinkVtbl;

    interface IUmiObjectSink
    {
        CONST_VTBL struct IUmiObjectSinkVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUmiObjectSink_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IUmiObjectSink_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IUmiObjectSink_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IUmiObjectSink_Put(This,lNumObjects,ppObjects)	\
    (This)->lpVtbl -> Put(This,lNumObjects,ppObjects)

#define IUmiObjectSink_SetResult(This,hResStatus,uFlags,pObject)	\
    (This)->lpVtbl -> SetResult(This,hResStatus,uFlags,pObject)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IUmiObjectSink_Put_Proxy( 
    IUmiObjectSink * This,
    /* [in] */ LONG lNumObjects,
    /* [size_is][in] */ IUmiObject **ppObjects);


void __RPC_STUB IUmiObjectSink_Put_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IUmiObjectSink_SetResult_Proxy( 
    IUmiObjectSink * This,
    /* [in] */ HRESULT hResStatus,
    /* [in] */ ULONG uFlags,
    /* [in] */ IUnknown *pObject);


void __RPC_STUB IUmiObjectSink_SetResult_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IUmiObjectSink_INTERFACE_DEFINED__ */


#ifndef __IUmiURLKeyList_INTERFACE_DEFINED__
#define __IUmiURLKeyList_INTERFACE_DEFINED__

/* interface IUmiURLKeyList */
/* [uuid][object][local] */ 


EXTERN_C const IID IID_IUmiURLKeyList;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("cf779c98-4739-4fd4-a415-da937a599f2f")
    IUmiURLKeyList : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetCount( 
            /* [out] */ ULONG *puKeyCount) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetKey( 
            /* [string][in] */ LPCWSTR pszName,
            /* [string][in] */ LPCWSTR pszValue) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetKey( 
            /* [in] */ ULONG uKeyIx,
            /* [in] */ ULONG uFlags,
            /* [out][in] */ ULONG *puKeyNameBufSize,
            /* [in] */ LPWSTR pszKeyName,
            /* [out][in] */ ULONG *puValueBufSize,
            /* [in] */ LPWSTR pszValue) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RemoveKey( 
            /* [string][in] */ LPCWSTR pszName,
            /* [in] */ ULONG uFlags) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RemoveAllKeys( 
            /* [in] */ ULONG uFlags) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetKeysInfo( 
            /* [in] */ ULONG uRequestedInfo,
            /* [out] */ ULONGLONG *puResponse) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IUmiURLKeyListVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUmiURLKeyList * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUmiURLKeyList * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUmiURLKeyList * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetCount )( 
            IUmiURLKeyList * This,
            /* [out] */ ULONG *puKeyCount);
        
        HRESULT ( STDMETHODCALLTYPE *SetKey )( 
            IUmiURLKeyList * This,
            /* [string][in] */ LPCWSTR pszName,
            /* [string][in] */ LPCWSTR pszValue);
        
        HRESULT ( STDMETHODCALLTYPE *GetKey )( 
            IUmiURLKeyList * This,
            /* [in] */ ULONG uKeyIx,
            /* [in] */ ULONG uFlags,
            /* [out][in] */ ULONG *puKeyNameBufSize,
            /* [in] */ LPWSTR pszKeyName,
            /* [out][in] */ ULONG *puValueBufSize,
            /* [in] */ LPWSTR pszValue);
        
        HRESULT ( STDMETHODCALLTYPE *RemoveKey )( 
            IUmiURLKeyList * This,
            /* [string][in] */ LPCWSTR pszName,
            /* [in] */ ULONG uFlags);
        
        HRESULT ( STDMETHODCALLTYPE *RemoveAllKeys )( 
            IUmiURLKeyList * This,
            /* [in] */ ULONG uFlags);
        
        HRESULT ( STDMETHODCALLTYPE *GetKeysInfo )( 
            IUmiURLKeyList * This,
            /* [in] */ ULONG uRequestedInfo,
            /* [out] */ ULONGLONG *puResponse);
        
        END_INTERFACE
    } IUmiURLKeyListVtbl;

    interface IUmiURLKeyList
    {
        CONST_VTBL struct IUmiURLKeyListVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUmiURLKeyList_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IUmiURLKeyList_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IUmiURLKeyList_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IUmiURLKeyList_GetCount(This,puKeyCount)	\
    (This)->lpVtbl -> GetCount(This,puKeyCount)

#define IUmiURLKeyList_SetKey(This,pszName,pszValue)	\
    (This)->lpVtbl -> SetKey(This,pszName,pszValue)

#define IUmiURLKeyList_GetKey(This,uKeyIx,uFlags,puKeyNameBufSize,pszKeyName,puValueBufSize,pszValue)	\
    (This)->lpVtbl -> GetKey(This,uKeyIx,uFlags,puKeyNameBufSize,pszKeyName,puValueBufSize,pszValue)

#define IUmiURLKeyList_RemoveKey(This,pszName,uFlags)	\
    (This)->lpVtbl -> RemoveKey(This,pszName,uFlags)

#define IUmiURLKeyList_RemoveAllKeys(This,uFlags)	\
    (This)->lpVtbl -> RemoveAllKeys(This,uFlags)

#define IUmiURLKeyList_GetKeysInfo(This,uRequestedInfo,puResponse)	\
    (This)->lpVtbl -> GetKeysInfo(This,uRequestedInfo,puResponse)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IUmiURLKeyList_GetCount_Proxy( 
    IUmiURLKeyList * This,
    /* [out] */ ULONG *puKeyCount);


void __RPC_STUB IUmiURLKeyList_GetCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IUmiURLKeyList_SetKey_Proxy( 
    IUmiURLKeyList * This,
    /* [string][in] */ LPCWSTR pszName,
    /* [string][in] */ LPCWSTR pszValue);


void __RPC_STUB IUmiURLKeyList_SetKey_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IUmiURLKeyList_GetKey_Proxy( 
    IUmiURLKeyList * This,
    /* [in] */ ULONG uKeyIx,
    /* [in] */ ULONG uFlags,
    /* [out][in] */ ULONG *puKeyNameBufSize,
    /* [in] */ LPWSTR pszKeyName,
    /* [out][in] */ ULONG *puValueBufSize,
    /* [in] */ LPWSTR pszValue);


void __RPC_STUB IUmiURLKeyList_GetKey_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IUmiURLKeyList_RemoveKey_Proxy( 
    IUmiURLKeyList * This,
    /* [string][in] */ LPCWSTR pszName,
    /* [in] */ ULONG uFlags);


void __RPC_STUB IUmiURLKeyList_RemoveKey_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IUmiURLKeyList_RemoveAllKeys_Proxy( 
    IUmiURLKeyList * This,
    /* [in] */ ULONG uFlags);


void __RPC_STUB IUmiURLKeyList_RemoveAllKeys_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IUmiURLKeyList_GetKeysInfo_Proxy( 
    IUmiURLKeyList * This,
    /* [in] */ ULONG uRequestedInfo,
    /* [out] */ ULONGLONG *puResponse);


void __RPC_STUB IUmiURLKeyList_GetKeysInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IUmiURLKeyList_INTERFACE_DEFINED__ */


#ifndef __IUmiURL_INTERFACE_DEFINED__
#define __IUmiURL_INTERFACE_DEFINED__

/* interface IUmiURL */
/* [uuid][local][object] */ 


EXTERN_C const IID IID_IUmiURL;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("12575a7d-d9db-11d3-a11f-00105a1f515a")
    IUmiURL : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Set( 
            /* [in] */ long lFlags,
            /* [in] */ LPCWSTR pszText) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Get( 
            /* [in] */ long lFlags,
            /* [out][in] */ ULONG *puBufSize,
            /* [string][in] */ LPWSTR pszDest) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetPathInfo( 
            /* [in] */ ULONG uRequestedInfo,
            /* [out] */ ULONGLONG *puResponse) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetLocator( 
            /* [string][in] */ LPCWSTR Name) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetLocator( 
            /* [out][in] */ ULONG *puNameBufLength,
            /* [string][in] */ LPWSTR pName) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetRootNamespace( 
            /* [string][in] */ LPCWSTR Name) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetRootNamespace( 
            /* [out][in] */ ULONG *puNameBufLength,
            /* [string][out][in] */ LPWSTR pName) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetComponentCount( 
            /* [out] */ ULONG *puCount) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetComponent( 
            /* [in] */ ULONG uIndex,
            /* [in] */ LPWSTR pszClass) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetComponentFromText( 
            /* [in] */ ULONG uIndex,
            /* [in] */ LPWSTR pszText) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetComponent( 
            /* [in] */ ULONG uIndex,
            /* [out][in] */ ULONG *puClassNameBufSize,
            /* [out][in] */ LPWSTR pszClass,
            /* [out] */ IUmiURLKeyList **pKeyList) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetComponentAsText( 
            /* [in] */ ULONG uIndex,
            /* [out][in] */ ULONG *puTextBufSize,
            /* [out][in] */ LPWSTR pszText) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RemoveComponent( 
            /* [in] */ ULONG uIndex) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RemoveAllComponents( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetLeafName( 
            /* [string][in] */ LPCWSTR Name) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetLeafName( 
            /* [out][in] */ ULONG *puBuffLength,
            /* [string][out][in] */ LPWSTR pszName) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetKeyList( 
            /* [out] */ IUmiURLKeyList **pOut) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CreateLeafPart( 
            /* [in] */ long lFlags,
            /* [string][in] */ LPCWSTR Name) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE DeleteLeafPart( 
            /* [in] */ long lFlags) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IUmiURLVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUmiURL * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUmiURL * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUmiURL * This);
        
        HRESULT ( STDMETHODCALLTYPE *Set )( 
            IUmiURL * This,
            /* [in] */ long lFlags,
            /* [in] */ LPCWSTR pszText);
        
        HRESULT ( STDMETHODCALLTYPE *Get )( 
            IUmiURL * This,
            /* [in] */ long lFlags,
            /* [out][in] */ ULONG *puBufSize,
            /* [string][in] */ LPWSTR pszDest);
        
        HRESULT ( STDMETHODCALLTYPE *GetPathInfo )( 
            IUmiURL * This,
            /* [in] */ ULONG uRequestedInfo,
            /* [out] */ ULONGLONG *puResponse);
        
        HRESULT ( STDMETHODCALLTYPE *SetLocator )( 
            IUmiURL * This,
            /* [string][in] */ LPCWSTR Name);
        
        HRESULT ( STDMETHODCALLTYPE *GetLocator )( 
            IUmiURL * This,
            /* [out][in] */ ULONG *puNameBufLength,
            /* [string][in] */ LPWSTR pName);
        
        HRESULT ( STDMETHODCALLTYPE *SetRootNamespace )( 
            IUmiURL * This,
            /* [string][in] */ LPCWSTR Name);
        
        HRESULT ( STDMETHODCALLTYPE *GetRootNamespace )( 
            IUmiURL * This,
            /* [out][in] */ ULONG *puNameBufLength,
            /* [string][out][in] */ LPWSTR pName);
        
        HRESULT ( STDMETHODCALLTYPE *GetComponentCount )( 
            IUmiURL * This,
            /* [out] */ ULONG *puCount);
        
        HRESULT ( STDMETHODCALLTYPE *SetComponent )( 
            IUmiURL * This,
            /* [in] */ ULONG uIndex,
            /* [in] */ LPWSTR pszClass);
        
        HRESULT ( STDMETHODCALLTYPE *SetComponentFromText )( 
            IUmiURL * This,
            /* [in] */ ULONG uIndex,
            /* [in] */ LPWSTR pszText);
        
        HRESULT ( STDMETHODCALLTYPE *GetComponent )( 
            IUmiURL * This,
            /* [in] */ ULONG uIndex,
            /* [out][in] */ ULONG *puClassNameBufSize,
            /* [out][in] */ LPWSTR pszClass,
            /* [out] */ IUmiURLKeyList **pKeyList);
        
        HRESULT ( STDMETHODCALLTYPE *GetComponentAsText )( 
            IUmiURL * This,
            /* [in] */ ULONG uIndex,
            /* [out][in] */ ULONG *puTextBufSize,
            /* [out][in] */ LPWSTR pszText);
        
        HRESULT ( STDMETHODCALLTYPE *RemoveComponent )( 
            IUmiURL * This,
            /* [in] */ ULONG uIndex);
        
        HRESULT ( STDMETHODCALLTYPE *RemoveAllComponents )( 
            IUmiURL * This);
        
        HRESULT ( STDMETHODCALLTYPE *SetLeafName )( 
            IUmiURL * This,
            /* [string][in] */ LPCWSTR Name);
        
        HRESULT ( STDMETHODCALLTYPE *GetLeafName )( 
            IUmiURL * This,
            /* [out][in] */ ULONG *puBuffLength,
            /* [string][out][in] */ LPWSTR pszName);
        
        HRESULT ( STDMETHODCALLTYPE *GetKeyList )( 
            IUmiURL * This,
            /* [out] */ IUmiURLKeyList **pOut);
        
        HRESULT ( STDMETHODCALLTYPE *CreateLeafPart )( 
            IUmiURL * This,
            /* [in] */ long lFlags,
            /* [string][in] */ LPCWSTR Name);
        
        HRESULT ( STDMETHODCALLTYPE *DeleteLeafPart )( 
            IUmiURL * This,
            /* [in] */ long lFlags);
        
        END_INTERFACE
    } IUmiURLVtbl;

    interface IUmiURL
    {
        CONST_VTBL struct IUmiURLVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUmiURL_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IUmiURL_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IUmiURL_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IUmiURL_Set(This,lFlags,pszText)	\
    (This)->lpVtbl -> Set(This,lFlags,pszText)

#define IUmiURL_Get(This,lFlags,puBufSize,pszDest)	\
    (This)->lpVtbl -> Get(This,lFlags,puBufSize,pszDest)

#define IUmiURL_GetPathInfo(This,uRequestedInfo,puResponse)	\
    (This)->lpVtbl -> GetPathInfo(This,uRequestedInfo,puResponse)

#define IUmiURL_SetLocator(This,Name)	\
    (This)->lpVtbl -> SetLocator(This,Name)

#define IUmiURL_GetLocator(This,puNameBufLength,pName)	\
    (This)->lpVtbl -> GetLocator(This,puNameBufLength,pName)

#define IUmiURL_SetRootNamespace(This,Name)	\
    (This)->lpVtbl -> SetRootNamespace(This,Name)

#define IUmiURL_GetRootNamespace(This,puNameBufLength,pName)	\
    (This)->lpVtbl -> GetRootNamespace(This,puNameBufLength,pName)

#define IUmiURL_GetComponentCount(This,puCount)	\
    (This)->lpVtbl -> GetComponentCount(This,puCount)

#define IUmiURL_SetComponent(This,uIndex,pszClass)	\
    (This)->lpVtbl -> SetComponent(This,uIndex,pszClass)

#define IUmiURL_SetComponentFromText(This,uIndex,pszText)	\
    (This)->lpVtbl -> SetComponentFromText(This,uIndex,pszText)

#define IUmiURL_GetComponent(This,uIndex,puClassNameBufSize,pszClass,pKeyList)	\
    (This)->lpVtbl -> GetComponent(This,uIndex,puClassNameBufSize,pszClass,pKeyList)

#define IUmiURL_GetComponentAsText(This,uIndex,puTextBufSize,pszText)	\
    (This)->lpVtbl -> GetComponentAsText(This,uIndex,puTextBufSize,pszText)

#define IUmiURL_RemoveComponent(This,uIndex)	\
    (This)->lpVtbl -> RemoveComponent(This,uIndex)

#define IUmiURL_RemoveAllComponents(This)	\
    (This)->lpVtbl -> RemoveAllComponents(This)

#define IUmiURL_SetLeafName(This,Name)	\
    (This)->lpVtbl -> SetLeafName(This,Name)

#define IUmiURL_GetLeafName(This,puBuffLength,pszName)	\
    (This)->lpVtbl -> GetLeafName(This,puBuffLength,pszName)

#define IUmiURL_GetKeyList(This,pOut)	\
    (This)->lpVtbl -> GetKeyList(This,pOut)

#define IUmiURL_CreateLeafPart(This,lFlags,Name)	\
    (This)->lpVtbl -> CreateLeafPart(This,lFlags,Name)

#define IUmiURL_DeleteLeafPart(This,lFlags)	\
    (This)->lpVtbl -> DeleteLeafPart(This,lFlags)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IUmiURL_Set_Proxy( 
    IUmiURL * This,
    /* [in] */ long lFlags,
    /* [in] */ LPCWSTR pszText);


void __RPC_STUB IUmiURL_Set_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IUmiURL_Get_Proxy( 
    IUmiURL * This,
    /* [in] */ long lFlags,
    /* [out][in] */ ULONG *puBufSize,
    /* [string][in] */ LPWSTR pszDest);


void __RPC_STUB IUmiURL_Get_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IUmiURL_GetPathInfo_Proxy( 
    IUmiURL * This,
    /* [in] */ ULONG uRequestedInfo,
    /* [out] */ ULONGLONG *puResponse);


void __RPC_STUB IUmiURL_GetPathInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IUmiURL_SetLocator_Proxy( 
    IUmiURL * This,
    /* [string][in] */ LPCWSTR Name);


void __RPC_STUB IUmiURL_SetLocator_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IUmiURL_GetLocator_Proxy( 
    IUmiURL * This,
    /* [out][in] */ ULONG *puNameBufLength,
    /* [string][in] */ LPWSTR pName);


void __RPC_STUB IUmiURL_GetLocator_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IUmiURL_SetRootNamespace_Proxy( 
    IUmiURL * This,
    /* [string][in] */ LPCWSTR Name);


void __RPC_STUB IUmiURL_SetRootNamespace_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IUmiURL_GetRootNamespace_Proxy( 
    IUmiURL * This,
    /* [out][in] */ ULONG *puNameBufLength,
    /* [string][out][in] */ LPWSTR pName);


void __RPC_STUB IUmiURL_GetRootNamespace_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IUmiURL_GetComponentCount_Proxy( 
    IUmiURL * This,
    /* [out] */ ULONG *puCount);


void __RPC_STUB IUmiURL_GetComponentCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IUmiURL_SetComponent_Proxy( 
    IUmiURL * This,
    /* [in] */ ULONG uIndex,
    /* [in] */ LPWSTR pszClass);


void __RPC_STUB IUmiURL_SetComponent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IUmiURL_SetComponentFromText_Proxy( 
    IUmiURL * This,
    /* [in] */ ULONG uIndex,
    /* [in] */ LPWSTR pszText);


void __RPC_STUB IUmiURL_SetComponentFromText_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IUmiURL_GetComponent_Proxy( 
    IUmiURL * This,
    /* [in] */ ULONG uIndex,
    /* [out][in] */ ULONG *puClassNameBufSize,
    /* [out][in] */ LPWSTR pszClass,
    /* [out] */ IUmiURLKeyList **pKeyList);


void __RPC_STUB IUmiURL_GetComponent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IUmiURL_GetComponentAsText_Proxy( 
    IUmiURL * This,
    /* [in] */ ULONG uIndex,
    /* [out][in] */ ULONG *puTextBufSize,
    /* [out][in] */ LPWSTR pszText);


void __RPC_STUB IUmiURL_GetComponentAsText_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IUmiURL_RemoveComponent_Proxy( 
    IUmiURL * This,
    /* [in] */ ULONG uIndex);


void __RPC_STUB IUmiURL_RemoveComponent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IUmiURL_RemoveAllComponents_Proxy( 
    IUmiURL * This);


void __RPC_STUB IUmiURL_RemoveAllComponents_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IUmiURL_SetLeafName_Proxy( 
    IUmiURL * This,
    /* [string][in] */ LPCWSTR Name);


void __RPC_STUB IUmiURL_SetLeafName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IUmiURL_GetLeafName_Proxy( 
    IUmiURL * This,
    /* [out][in] */ ULONG *puBuffLength,
    /* [string][out][in] */ LPWSTR pszName);


void __RPC_STUB IUmiURL_GetLeafName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IUmiURL_GetKeyList_Proxy( 
    IUmiURL * This,
    /* [out] */ IUmiURLKeyList **pOut);


void __RPC_STUB IUmiURL_GetKeyList_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IUmiURL_CreateLeafPart_Proxy( 
    IUmiURL * This,
    /* [in] */ long lFlags,
    /* [string][in] */ LPCWSTR Name);


void __RPC_STUB IUmiURL_CreateLeafPart_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IUmiURL_DeleteLeafPart_Proxy( 
    IUmiURL * This,
    /* [in] */ long lFlags);


void __RPC_STUB IUmiURL_DeleteLeafPart_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IUmiURL_INTERFACE_DEFINED__ */


#ifndef __IUmiQuery_INTERFACE_DEFINED__
#define __IUmiQuery_INTERFACE_DEFINED__

/* interface IUmiQuery */
/* [uuid][local][object] */ 


EXTERN_C const IID IID_IUmiQuery;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("12575a7e-d9db-11d3-a11f-00105a1f515a")
    IUmiQuery : public IUmiBaseObject
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Set( 
            /* [in] */ LPCWSTR pszLanguage,
            /* [in] */ ULONG uFlags,
            /* [in] */ LPCWSTR pszText) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetQuery( 
            /* [out][in] */ ULONG *puLangBufSize,
            /* [out][in] */ LPWSTR pszLangBuf,
            /* [out][in] */ ULONG *puQueryTextBufSize,
            /* [out][in] */ LPWSTR pszQueryTextBuf) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IUmiQueryVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUmiQuery * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUmiQuery * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUmiQuery * This);
        
        HRESULT ( STDMETHODCALLTYPE *Put )( 
            IUmiQuery * This,
            /* [in] */ LPCWSTR pszName,
            /* [in] */ ULONG uFlags,
            /* [in] */ UMI_PROPERTY_VALUES *pProp);
        
        HRESULT ( STDMETHODCALLTYPE *Get )( 
            IUmiQuery * This,
            /* [in] */ LPCWSTR pszName,
            /* [in] */ ULONG uFlags,
            /* [out] */ UMI_PROPERTY_VALUES **pProp);
        
        HRESULT ( STDMETHODCALLTYPE *GetAt )( 
            IUmiQuery * This,
            /* [in] */ LPCWSTR pszName,
            /* [in] */ ULONG uFlags,
            /* [in] */ ULONG uBufferLength,
            /* [out] */ LPVOID pExistingMem);
        
        HRESULT ( STDMETHODCALLTYPE *GetAs )( 
            IUmiQuery * This,
            /* [in] */ LPCWSTR pszName,
            /* [in] */ ULONG uFlags,
            /* [in] */ ULONG uCoercionType,
            /* [out] */ UMI_PROPERTY_VALUES **pProp);
        
        HRESULT ( STDMETHODCALLTYPE *FreeMemory )( 
            IUmiQuery * This,
            ULONG uReserved,
            LPVOID pMem);
        
        HRESULT ( STDMETHODCALLTYPE *Delete )( 
            IUmiQuery * This,
            /* [in] */ LPCWSTR pszName,
            /* [in] */ ULONG uFlags);
        
        HRESULT ( STDMETHODCALLTYPE *GetProps )( 
            IUmiQuery * This,
            /* [in] */ LPCWSTR *pszNames,
            /* [in] */ ULONG uNameCount,
            /* [in] */ ULONG uFlags,
            /* [out] */ UMI_PROPERTY_VALUES **pProps);
        
        HRESULT ( STDMETHODCALLTYPE *PutProps )( 
            IUmiQuery * This,
            /* [in] */ LPCWSTR *pszNames,
            /* [in] */ ULONG uNameCount,
            /* [in] */ ULONG uFlags,
            /* [in] */ UMI_PROPERTY_VALUES *pProps);
        
        HRESULT ( STDMETHODCALLTYPE *PutFrom )( 
            IUmiQuery * This,
            /* [in] */ LPCWSTR pszName,
            /* [in] */ ULONG uFlags,
            /* [in] */ ULONG uBufferLength,
            /* [in] */ LPVOID pExistingMem);
        
        HRESULT ( STDMETHODCALLTYPE *GetLastStatus )( 
            IUmiQuery * This,
            /* [in] */ ULONG uFlags,
            /* [out] */ ULONG *puSpecificStatus,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ LPVOID *pStatusObj);
        
        HRESULT ( STDMETHODCALLTYPE *GetInterfacePropList )( 
            IUmiQuery * This,
            /* [in] */ ULONG uFlags,
            /* [out] */ IUmiPropList **pPropList);
        
        HRESULT ( STDMETHODCALLTYPE *Set )( 
            IUmiQuery * This,
            /* [in] */ LPCWSTR pszLanguage,
            /* [in] */ ULONG uFlags,
            /* [in] */ LPCWSTR pszText);
        
        HRESULT ( STDMETHODCALLTYPE *GetQuery )( 
            IUmiQuery * This,
            /* [out][in] */ ULONG *puLangBufSize,
            /* [out][in] */ LPWSTR pszLangBuf,
            /* [out][in] */ ULONG *puQueryTextBufSize,
            /* [out][in] */ LPWSTR pszQueryTextBuf);
        
        END_INTERFACE
    } IUmiQueryVtbl;

    interface IUmiQuery
    {
        CONST_VTBL struct IUmiQueryVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUmiQuery_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IUmiQuery_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IUmiQuery_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IUmiQuery_Put(This,pszName,uFlags,pProp)	\
    (This)->lpVtbl -> Put(This,pszName,uFlags,pProp)

#define IUmiQuery_Get(This,pszName,uFlags,pProp)	\
    (This)->lpVtbl -> Get(This,pszName,uFlags,pProp)

#define IUmiQuery_GetAt(This,pszName,uFlags,uBufferLength,pExistingMem)	\
    (This)->lpVtbl -> GetAt(This,pszName,uFlags,uBufferLength,pExistingMem)

#define IUmiQuery_GetAs(This,pszName,uFlags,uCoercionType,pProp)	\
    (This)->lpVtbl -> GetAs(This,pszName,uFlags,uCoercionType,pProp)

#define IUmiQuery_FreeMemory(This,uReserved,pMem)	\
    (This)->lpVtbl -> FreeMemory(This,uReserved,pMem)

#define IUmiQuery_Delete(This,pszName,uFlags)	\
    (This)->lpVtbl -> Delete(This,pszName,uFlags)

#define IUmiQuery_GetProps(This,pszNames,uNameCount,uFlags,pProps)	\
    (This)->lpVtbl -> GetProps(This,pszNames,uNameCount,uFlags,pProps)

#define IUmiQuery_PutProps(This,pszNames,uNameCount,uFlags,pProps)	\
    (This)->lpVtbl -> PutProps(This,pszNames,uNameCount,uFlags,pProps)

#define IUmiQuery_PutFrom(This,pszName,uFlags,uBufferLength,pExistingMem)	\
    (This)->lpVtbl -> PutFrom(This,pszName,uFlags,uBufferLength,pExistingMem)


#define IUmiQuery_GetLastStatus(This,uFlags,puSpecificStatus,riid,pStatusObj)	\
    (This)->lpVtbl -> GetLastStatus(This,uFlags,puSpecificStatus,riid,pStatusObj)

#define IUmiQuery_GetInterfacePropList(This,uFlags,pPropList)	\
    (This)->lpVtbl -> GetInterfacePropList(This,uFlags,pPropList)


#define IUmiQuery_Set(This,pszLanguage,uFlags,pszText)	\
    (This)->lpVtbl -> Set(This,pszLanguage,uFlags,pszText)

#define IUmiQuery_GetQuery(This,puLangBufSize,pszLangBuf,puQueryTextBufSize,pszQueryTextBuf)	\
    (This)->lpVtbl -> GetQuery(This,puLangBufSize,pszLangBuf,puQueryTextBufSize,pszQueryTextBuf)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IUmiQuery_Set_Proxy( 
    IUmiQuery * This,
    /* [in] */ LPCWSTR pszLanguage,
    /* [in] */ ULONG uFlags,
    /* [in] */ LPCWSTR pszText);


void __RPC_STUB IUmiQuery_Set_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IUmiQuery_GetQuery_Proxy( 
    IUmiQuery * This,
    /* [out][in] */ ULONG *puLangBufSize,
    /* [out][in] */ LPWSTR pszLangBuf,
    /* [out][in] */ ULONG *puQueryTextBufSize,
    /* [out][in] */ LPWSTR pszQueryTextBuf);


void __RPC_STUB IUmiQuery_GetQuery_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IUmiQuery_INTERFACE_DEFINED__ */


#ifndef __IUmiCustomInterfaceFactory_INTERFACE_DEFINED__
#define __IUmiCustomInterfaceFactory_INTERFACE_DEFINED__

/* interface IUmiCustomInterfaceFactory */
/* [uuid][local][object] */ 


EXTERN_C const IID IID_IUmiCustomInterfaceFactory;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("14CD599E-2BE7-4c6f-B95B-B150DCD93585")
    IUmiCustomInterfaceFactory : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetCLSIDForIID( 
            /* [in] */ REFIID riid,
            /* [in] */ long lFlags,
            /* [out][in] */ CLSID *pCLSID) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetObjectByCLSID( 
            /* [in] */ CLSID clsid,
            /* [in] */ IUnknown *pUnkOuter,
            /* [in] */ DWORD dwClsContext,
            /* [in] */ REFIID riid,
            /* [in] */ long lFlags,
            /* [iid_is][out] */ void **ppInterface) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetCLSIDForNames( 
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId,
            /* [in] */ long lFlags,
            /* [out][in] */ CLSID *pCLSID) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IUmiCustomInterfaceFactoryVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUmiCustomInterfaceFactory * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUmiCustomInterfaceFactory * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUmiCustomInterfaceFactory * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetCLSIDForIID )( 
            IUmiCustomInterfaceFactory * This,
            /* [in] */ REFIID riid,
            /* [in] */ long lFlags,
            /* [out][in] */ CLSID *pCLSID);
        
        HRESULT ( STDMETHODCALLTYPE *GetObjectByCLSID )( 
            IUmiCustomInterfaceFactory * This,
            /* [in] */ CLSID clsid,
            /* [in] */ IUnknown *pUnkOuter,
            /* [in] */ DWORD dwClsContext,
            /* [in] */ REFIID riid,
            /* [in] */ long lFlags,
            /* [iid_is][out] */ void **ppInterface);
        
        HRESULT ( STDMETHODCALLTYPE *GetCLSIDForNames )( 
            IUmiCustomInterfaceFactory * This,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId,
            /* [in] */ long lFlags,
            /* [out][in] */ CLSID *pCLSID);
        
        END_INTERFACE
    } IUmiCustomInterfaceFactoryVtbl;

    interface IUmiCustomInterfaceFactory
    {
        CONST_VTBL struct IUmiCustomInterfaceFactoryVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUmiCustomInterfaceFactory_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IUmiCustomInterfaceFactory_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IUmiCustomInterfaceFactory_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IUmiCustomInterfaceFactory_GetCLSIDForIID(This,riid,lFlags,pCLSID)	\
    (This)->lpVtbl -> GetCLSIDForIID(This,riid,lFlags,pCLSID)

#define IUmiCustomInterfaceFactory_GetObjectByCLSID(This,clsid,pUnkOuter,dwClsContext,riid,lFlags,ppInterface)	\
    (This)->lpVtbl -> GetObjectByCLSID(This,clsid,pUnkOuter,dwClsContext,riid,lFlags,ppInterface)

#define IUmiCustomInterfaceFactory_GetCLSIDForNames(This,rgszNames,cNames,lcid,rgDispId,lFlags,pCLSID)	\
    (This)->lpVtbl -> GetCLSIDForNames(This,rgszNames,cNames,lcid,rgDispId,lFlags,pCLSID)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IUmiCustomInterfaceFactory_GetCLSIDForIID_Proxy( 
    IUmiCustomInterfaceFactory * This,
    /* [in] */ REFIID riid,
    /* [in] */ long lFlags,
    /* [out][in] */ CLSID *pCLSID);


void __RPC_STUB IUmiCustomInterfaceFactory_GetCLSIDForIID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IUmiCustomInterfaceFactory_GetObjectByCLSID_Proxy( 
    IUmiCustomInterfaceFactory * This,
    /* [in] */ CLSID clsid,
    /* [in] */ IUnknown *pUnkOuter,
    /* [in] */ DWORD dwClsContext,
    /* [in] */ REFIID riid,
    /* [in] */ long lFlags,
    /* [iid_is][out] */ void **ppInterface);


void __RPC_STUB IUmiCustomInterfaceFactory_GetObjectByCLSID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IUmiCustomInterfaceFactory_GetCLSIDForNames_Proxy( 
    IUmiCustomInterfaceFactory * This,
    /* [size_is][in] */ LPOLESTR *rgszNames,
    /* [in] */ UINT cNames,
    /* [in] */ LCID lcid,
    /* [size_is][out] */ DISPID *rgDispId,
    /* [in] */ long lFlags,
    /* [out][in] */ CLSID *pCLSID);


void __RPC_STUB IUmiCustomInterfaceFactory_GetCLSIDForNames_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IUmiCustomInterfaceFactory_INTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_UmiDefURL;

#ifdef __cplusplus

class DECLSPEC_UUID("d4b21cc2-f2a5-453e-8459-b27f362cb0e0")
UmiDefURL;
#endif
#endif /* __UMI_V6_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


