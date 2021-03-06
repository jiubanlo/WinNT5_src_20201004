// ==++==
// 
//   Copyright (c) Microsoft Corporation.  All rights reserved.
// 
// ==--==
//*****************************************************************************
// File: imptlb.h
// 
// TypeLib importer.
//*****************************************************************************
#ifndef __imptlb_h__
#define __imptlb_h__

extern "C"
HRESULT __stdcall ImportTypeLib(
    LPCWSTR     szLibrary,              // Name of library being imported.
    ITypeLib    *pitlb,                 // The type library to import from.
    REFIID      riid,                   // Interface to return.
    void        **ppObj);               // Return pointer to object here.


//#define TLB_STATS

#define MAX_TLB_VT                  VT_LPWSTR + 1
#define MAX_INIT_SIG                3
#define MAX_COM_GUID_SIG            6
#define MAX_COM_ADDLISTENER_SIG     8
#define MAX_COM_REMOVELISTENER_SIG  8
#define CB_MAX_ELEMENT_TYPE         4

// Forward declarations.
struct ITypeLibImporterNotifySink;
class Assembly;
class Module;
class CImportTlb;

//*****************************************************************************
// Class to perform memory management.  Memory is not moved as the heap is 
//  expanded, and all of the allocations are cleaned up in the destructor.
//*****************************************************************************
class CWCHARPool : public StgPool
{
public:
    CWCHARPool() : StgPool() { InitNew(); }

    // Allocate some bytes from the pool.
    WCHAR * Alloc(ULONG nChars)
    {   
        BYTE *pRslt;
        // Convert from characters to bytes.
        nChars *= sizeof(WCHAR);
        if (nChars > GetCbSegAvailable())
            if (!Grow(nChars))
                return 0;
        pRslt = GetNextLocation();
        SegAllocate(nChars);
        return (WCHAR*)pRslt;
    }
}; // class CDescPool : public StgPool


//*****************************************************************************
// This helper method is used to track an url to typeref token.  This makes
// defining new typerefs faster.
//*****************************************************************************
class CImpTlbTypeRef
{
public:
    CImpTlbTypeRef() { }
    ~CImpTlbTypeRef() { m_Map.Clear(); }

    //*****************************************************************************
    // Look for an existing typeref in the map and return if found.  If not found,
    // then create a new one and add it to the map for later.
    //*****************************************************************************
    HRESULT DefineTypeRef(                  // S_OK or error.
        IMetaDataEmit *pEmit,               // Emit interface.
        mdAssemblyRef ar,                   // Containing assembly.
        const LPCWSTR szURL,                // URL of the TypeDef, wide chars.
        mdTypeRef   *ptr);                  // Put mdTypeRef here

    class TokenOfTypeRefHashKey
    {
    public:
        mdToken     tkResolutionScope;      // TypeRef's resolution scope.
        LPCWSTR     szName;                 // TypeRef's name.
        mdTypeRef   tr;                     // The TypeRef's token.
    };

private:

    class CTokenOfTypeRefHash : public CClosedHash<class TokenOfTypeRefHashKey>
    {
    public:
        typedef CClosedHash<class TokenOfTypeRefHashKey> Super;
        typedef TokenOfTypeRefHashKey T;

        CTokenOfTypeRefHash() : CClosedHash<class TokenOfTypeRefHashKey>(101) {}
        ~CTokenOfTypeRefHash() { Clear(); }

        virtual void Clear();
        
        unsigned long Hash(const void *pData) {return Hash((const T*)pData);}
        unsigned long Hash(const T *pData);

        unsigned long Compare(const void *p1, BYTE *p2) {return Compare((const T*)p1, (T*)p2);}
        unsigned long Compare(const T *p1, T *p2);

        ELEMENTSTATUS Status(BYTE *p) {return Status((T*)p);}
        ELEMENTSTATUS Status(T *p);

        void SetStatus(BYTE *p, ELEMENTSTATUS s) {SetStatus((T*)p, s);}
        void SetStatus(T *p, ELEMENTSTATUS s);

        void* GetKey(BYTE *p) {return GetKey((T*)p);}
        void *GetKey(T *p);
        
        T* Add(const T *pData);
        
        CWCHARPool          m_Names;        // Heap of names.
    };

    CTokenOfTypeRefHash m_Map;          // Map of namespace to token.
};


//*****************************************************************************
// This helper class is used to track source interface ITypeInfo*'s to event 
// information.
//*****************************************************************************
class ImpTlbEventInfo
{
public:
    LPCWSTR     szSrcItfName;           // The source interface name (the key).
    mdTypeRef   trEventItf;             // The event interface typedef.
    LPCWSTR     szEventItfName;         // The event interface name.
    LPCWSTR     szEventProviderName;    // The event provider name.
    Assembly*   SrcItfAssembly;         // The assembly where source interface resides.
};

class CImpTlbEventInfoMap : protected CClosedHash<class ImpTlbEventInfo>
{
public:
    typedef CClosedHash<class ImpTlbEventInfo> Super;
    typedef ImpTlbEventInfo T;

    CImpTlbEventInfoMap() : CClosedHash<class ImpTlbEventInfo>(101) {}
    ~CImpTlbEventInfoMap() { Clear(); }

    HRESULT AddEventInfo(LPCWSTR szSrcItfName, mdTypeRef trEventItf, LPCWSTR szEventItfName, LPCWSTR szEventProviderName, Assembly* SrcItfAssembly);
    ImpTlbEventInfo *FindEventInfo(LPCWSTR szSrcItfName);

    HRESULT GetEventInfoList(CQuickArray<ImpTlbEventInfo*> &qbEvInfoList);

private:
    unsigned long Hash(const void *pData) {return Hash((const T*)pData);}
    unsigned long Hash(const T *pData);

    unsigned long Compare(const void *p1, BYTE *p2) {return Compare((const T*)p1, (T*)p2);}
    unsigned long Compare(const T *p1, T *p2);

    ELEMENTSTATUS Status(BYTE *p) {return Status((T*)p);}
    ELEMENTSTATUS Status(T *p);

    void SetStatus(BYTE *p, ELEMENTSTATUS s) {SetStatus((T*)p, s);}
    void SetStatus(T *p, ELEMENTSTATUS s);

    void* GetKey(BYTE *p) {return GetKey((T*)p);}
    void *GetKey(T *p);
    
    T* Add(const T *pData);
    
    CWCHARPool          m_Names;        // Heap of names.
};

class CImpTlbReservedNames
{
public:
    CImpTlbReservedNames() {}
    ~CImpTlbReservedNames() {/*m_StringMap.Clear();*/}

    HRESULT Init() {return m_StringMap.NewInit();}

    void AddReservedName(LPCWSTR szName) {BOOL flag = TRUE; m_StringMap.AddItem(szName, flag);}
    BOOL IsReservedName(LPCWSTR szName) {return m_StringMap.GetItem(szName) != 0;}

private:
    TStringMap<BOOL>    m_StringMap;
};


//*****************************************************************************
// Helper class to keep track of the mappings from default interfaces to 
// class interfaces.
//*****************************************************************************
class ImpTlbClassItfInfo
{
public:
    IID         ItfIID;                 // The IID of the interface.
    LPCWSTR     szClassItfName;         // The class interface name.
};

class CImpTlbDefItfToClassItfMap : protected CClosedHash<class ImpTlbClassItfInfo>
{
public:
    typedef CClosedHash<class ImpTlbClassItfInfo> Super;
    typedef ImpTlbClassItfInfo T;

    CImpTlbDefItfToClassItfMap();
    ~CImpTlbDefItfToClassItfMap();

    HRESULT Init(ITypeLib *pTlb, BSTR bstrNameSpace);

    LPCWSTR GetClassItfName(IID &rItfIID);

private:
    HRESULT AddCoClassInterfaces(ITypeInfo *pCoClassITI, TYPEATTR *pCoClassTypeAttr);

    unsigned long Hash(const void *pData) {return Hash((const T*)pData);}
    unsigned long Hash(const T *pData);

    unsigned long Compare(const void *p1, BYTE *p2) {return Compare((const T*)p1, (T*)p2);}
    unsigned long Compare(const T *p1, T *p2);

    ELEMENTSTATUS Status(BYTE *p) {return Status((T*)p);}
    ELEMENTSTATUS Status(T *p);

    void SetStatus(BYTE *p, ELEMENTSTATUS s) {SetStatus((T*)p, s);}
    void SetStatus(T *p, ELEMENTSTATUS s);

    void* GetKey(BYTE *p) {return GetKey((T*)p);}
    void *GetKey(T *p);
    
    T* Add(const T *pData);
    
    CWCHARPool          m_Names;            // Heap of names.
    BSTR                m_bstrNameSpace;    // Namespace of the typelib.
};


//*****************************************************************************
// Helper class to keep track of imported typelibs.  Typically, a typelib
//  imports only 2 or 3 other typelibs, so a simple array is used.
//*****************************************************************************
struct CTlbRef
{
    GUID            guid;               // GUID of referenced typelib.
    mdAssemblyRef   ar;                 // AssemblyRef for the module containing reference.
    BSTR            szNameSpace;        // The namespace of the types contained in the assembly.
    BSTR            szAsmName;          // The assembly name.
    Assembly*       Asm;                // The assembly;
    CImpTlbDefItfToClassItfMap *pDefItfToClassItfMap; // The default interface to class interface map.

    ~CTlbRef() 
    {
        SysFreeString(szNameSpace); 
        SysFreeString(szAsmName);
        delete pDefItfToClassItfMap;
    }
};

class CImpTlbLibRef : public CQuickArray<CTlbRef>
{
    typedef CQuickArray<CTlbRef> base;
public:
    CImpTlbLibRef() {base::ReSize(0);}
    ~CImpTlbLibRef();

    CImpTlbDefItfToClassItfMap *Add(ITypeLib *pITLB, CImportTlb  *pImporter, mdAssemblyRef ar, BSTR wzNamespace, BSTR wzAsmName, Assembly* assm);
    int Find(ITypeLib *pITLB, mdAssemblyRef *par, BSTR *pwzNamespace, BSTR *pwzAsmName, Assembly** assm, CImpTlbDefItfToClassItfMap **ppDefItfToClassItfMap);
};


class CImportTlb
{
public:
    static CImportTlb* CreateImporter(LPCWSTR szLibrary, ITypeLib *pitlb, BOOL bGenerateTCEAdapters, BOOL bUnsafeInterfaces, BOOL bSafeArrayAsSystemArray, BOOL bTransformDispRetVals);
    
    CImportTlb();
    CImportTlb(LPCWSTR szLibrary, ITypeLib *pitlb, BOOL bGenerateTCEAdapters, BOOL bUnsafeInterfaces, BOOL bSafeArrayAsSystemArray, BOOL bTransformDispRetVals);
    ~CImportTlb();

    HRESULT Import();
    HRESULT ImportTypeLib(ITypeLib *pITLB);
    HRESULT ImportTypeInfo(ITypeInfo *pITI, mdTypeDef *pCl);
    HRESULT GetInterface(REFIID riid, void ** pp);
    HRESULT SetNamespace(WCHAR const *pNamespace);
    WCHAR *GetNamespace() {return m_wzNamespace;}
    HRESULT SetNotification(ITypeLibImporterNotifySink *pINotify);
    HRESULT SetMetaData(IUnknown *pIUnk);
    void    SetAssembly(Assembly *pAssembly) {m_pAssembly = pAssembly;}
    void    SetModule(Module *pModule) {m_pModule = pModule;}
    HRESULT GetNamespaceOfRefTlb(ITypeLib *pITLB, BSTR *pwzNamespace, CImpTlbDefItfToClassItfMap **ppDefItfToClassItfMap);
    HRESULT GetEventInfoList(CQuickArray<ImpTlbEventInfo*> &qbEvInfoList) {return m_EventInfoMap.GetEventInfoList(qbEvInfoList);}

    static HRESULT GetDefaultInterface(ITypeInfo *pCoClassTI, ITypeInfo **pDefaultItfTI);

protected:
    
    struct MemberInfo
    {
        union 
        {
            FUNCDESC    *m_psFunc;      // Pointer to FuncDesc.
            VARDESC     *m_psVar;       // Pointer to VarDesc.
        };
        LPWSTR      m_pName;            // Function/Prop's name, possibly decorated.
        int         m_iMember;          // The index of the member in the ITypeInfo.
        union
        {
            LPWSTR      m_pName2;       // Prop's second name, if any.
            mdToken     m_mdFunc;       // Function's token & semantics, if not property.
            USHORT      m_msSemantics;  // Semantics only.
        };
        void SetFuncInfo(mdMethodDef mdFunc, USHORT msSemantics) {m_mdFunc = RidFromToken(mdFunc) | (msSemantics<<24);}
        void GetFuncInfo(mdMethodDef &mdFunc, USHORT &msSemantics) {mdFunc = m_mdFunc&0xffffff | mdtMethodDef; msSemantics = m_mdFunc>>24;}
    };

    
    HRESULT ConvertTypeLib();
    HRESULT ConvertTypeInfo();

    HRESULT ExplicitlyImplementsIEnumerable(ITypeInfo *pITI, TYPEATTR *psAttr, BOOL fLookupPartner = TRUE);

    HRESULT _NewLibraryObject();
    HRESULT ConvCoclass(ITypeInfo *pITI, TYPEATTR *psAttr);
    HRESULT ConvEnum(ITypeInfo *pITI, TYPEATTR *psAttr);
    HRESULT ConvRecord(ITypeInfo *pITI, TYPEATTR *psAttr, BOOL bUnion);
    HRESULT ConvIface(ITypeInfo *pITI, TYPEATTR *psAttr, BOOL bVtblGaps=true);
    HRESULT ConvDispatch(ITypeInfo *pITI, TYPEATTR *psAttr, BOOL bVtblGaps=true);
    HRESULT ConvModule(ITypeInfo *pITI, TYPEATTR *psAttr);

    HRESULT IsIUnknownDerived(ITypeInfo *pITI, TYPEATTR *psAttr);
    HRESULT IsIDispatchDerived(ITypeInfo *pITI, TYPEATTR *psAttr);
    HRESULT HasNewEnumMember(ITypeInfo *pItfTI);
    HRESULT FuncIsNewEnum(ITypeInfo *pITI, FUNCDESC *pFuncDesc, DWORD index);
    HRESULT PropertyIsNewEnum(ITypeInfo *pITI, VARDESC *pVarDesc, DWORD index);

    HRESULT HasObjectFields(ITypeInfo *pITI, TYPEATTR *psAttr);
    HRESULT IsObjectType(ITypeInfo *pITI, const TYPEDESC *pType);
    HRESULT CompareSigsIgnoringRetType(PCCOR_SIGNATURE pbSig1, ULONG cbSig1, PCCOR_SIGNATURE pbSig2, ULONG cbSig2);

    HRESULT FindMethod(mdTypeDef td, LPCWSTR szName, PCCOR_SIGNATURE pbSig, ULONG cbSig, mdMethodDef *pmb);
    HRESULT FindProperty(mdTypeDef td, LPCWSTR szName, PCCOR_SIGNATURE pSig, ULONG cbSig, mdProperty *pPr);
    HRESULT FindEvent(mdTypeDef td, LPCWSTR szName, mdProperty *pEv);   

    HRESULT ReportEvent(int ev, int hr, ...);
    
    HRESULT _DefineSysRefs();
    HRESULT _GetNamespaceName(ITypeLib *pITLB, BSTR *pwzNamespace);
    HRESULT _GetTokenForTypeInfo(ITypeInfo *pITI, BOOL bConvDefItfToClassItf, mdToken *pToken, LPWSTR pszTypeRef=0, int chTypeRef=0, int *pchTypeRef=0, BOOL bAsmQualifiedName = FALSE);

    HRESULT _FindFirstUserMethod(ITypeInfo *pITI, TYPEATTR *psAttr, int *pIx);
    HRESULT _ResolveTypeDescAliasTypeKind(ITypeInfo *pITIAlias, TYPEDESC *ptdesc, TYPEKIND *ptkind);
    HRESULT _ResolveTypeDescAlias(ITypeInfo *pITIAlias, const TYPEDESC *ptdesc, ITypeInfo **ppTIResolved, TYPEATTR **ppsAttrResolved, GUID *pGuid=0);

    HRESULT _SetHiddenCA(mdTypeDef token);
    HRESULT _ForceIEnumerableCVExists(ITypeInfo* pITI, BOOL* CVExists);
    HRESULT _SetDispIDCA(ITypeInfo* pITI, int iMember, long lDispId, mdToken func, BOOL fAlwaysAdd, long* lDispSet, BOOL bFunc);
    HRESULT _GetDispIDCA(ITypeInfo* pITI, int iMember, long* lDispSet, BOOL bFunc);
    HRESULT _CheckForPropertyCustomAttributes(ITypeInfo* pITI, int index, INVOKEKIND* ikind);

    HRESULT _ConvIfaceMembers(ITypeInfo *pITI, TYPEATTR *psAttr, BOOL bVtblGaps, BOOL bAddDispIds, BOOL bInheritsIEnum);
    HRESULT _ConvSrcIfaceMembers(ITypeInfo   *pITI, TYPEATTR* psAttr, BOOL fInheritsIEnum);
    HRESULT _ConvDispatchMembers(ITypeInfo *pITI, TYPEATTR *psAttr, BOOL fInheritsIEnum);
    HRESULT _GetFunctionPropertyInfo(FUNCDESC *psFunc, USHORT *pSemantics, FUNCDESC **ppSig, TYPEDESC **ppProperty, BOOL *pbRetval);
    HRESULT _ConvFunction(ITypeInfo *pITI, MemberInfo *pMember, int bVtblGapFuncs, BOOL bAddDispIds, BOOL bDelegateInvokeMeth, BOOL* bAllowIEnum);
    HRESULT _GenerateEvent(ITypeInfo *pITI, MemberInfo  *pMember, BOOL fInheritsIEnum);
    HRESULT _GenerateEventDelegate(ITypeInfo *pITI, MemberInfo  *pMember, mdTypeDef *ptd, BOOL fInheritsIEnum);
    HRESULT _AddSrcItfMembersToClass(mdTypeRef trSrcItf);

    HRESULT _ConvPropertiesForFunctions(ITypeInfo *pITI, TYPEATTR *psAttr);
    enum ParamOpts{ParamNormal=0, ParamOptional, ParamVarArg};
    HRESULT _ConvParam(ITypeInfo *pITI, mdMethodDef mbFunc, int iSequence, const ELEMDESC *pdesc, ParamOpts paramOpts, const WCHAR *pszName, BYTE *pbNative, ULONG cbNative);
    HRESULT _ConvConstant(ITypeInfo *pITI, VARDESC *psVar, BOOL bEnumMember=false);
    HRESULT _ConvField(ITypeInfo *pITI, VARDESC *psVar, mdFieldDef *pmdField, BOOL bUnion);
    HRESULT _ConvProperty(ITypeInfo *pITI, MemberInfo *pMember);
    HRESULT _ConvNewEnumProperty(ITypeInfo *pITI, VARDESC *psVar, MemberInfo *pMember);

    HRESULT _HandleAliasInfo(ITypeInfo *pITI, TYPEDESC *pTypeDesc, mdToken tk);

    HRESULT _AddTlbRef(ITypeLib *pITLB, mdAssemblyRef *par, BSTR *pwzNamespace, BSTR *pwzAsmName, CImpTlbDefItfToClassItfMap **ppDefItfToClassItfMap);

    HRESULT _AddGuidCa(mdToken tkObj, REFGUID guid);
    HRESULT _AddDefaultMemberCa(mdToken tkObj, LPCWSTR szName);

    HRESULT _AddStringCa(int attr, mdToken tk, LPCWSTR wzString);

    HRESULT GetKnownTypeToken(VARTYPE vt, mdTypeRef *ptr);

    HRESULT _GetTokenForEventItf(ITypeInfo *pSrcItfITI, mdTypeDef *ptr);
    HRESULT _CreateClassInterface(ITypeInfo *pCoClassITI, ITypeInfo *pDefItfITI, mdTypeRef trDefItf, mdTypeRef rtDefEvItf, mdToken *ptr);

    HRESULT GetManagedNameForCoClass(ITypeInfo *pITI, CQuickArray<WCHAR> &qbClassName);
    HRESULT GenerateUniqueTypeName(CQuickArray<WCHAR> &qbTypeName);
    HRESULT GenerateUniqueMemberName(CQuickArray<WCHAR> &qbMemberName, PCCOR_SIGNATURE pSig, ULONG SigSize, LPCWSTR szPrefix, mdToken type);
    HRESULT _IsAlias(ITypeInfo *pITI, TYPEDESC *pTypeDesc);

    HRESULT GetDefMemberName(ITypeInfo *pITI, BOOL bItfQualified, CQuickArray<WCHAR> &qbDefMemberName);

    enum SigFlags {
        // These match the typelib values
        SIG_IN          = 0x0001,           // Input param.
        SIG_OUT         = 0x0002,           // Output param.
        SIG_RET         = 0x0008,           // Retval.  Currently unused.
        SIG_OPT         = 0x0010,           // Optional param.  Currently unused.
        SIG_FLAGS_MASK  = 0x001b,           // Mask of flags from TypeLib PARAMFLAGs

        SIG_FUNC        = 0x0100,           // Convert signature for function.
        SIG_FIELD       = 0x0200,           // Convert signature for field.
        SIG_ELEM        = 0x0300,           // Convert signature for sub element (eg, array of X).
        SIG_TYPE_MASK   = 0x0300,

        SIG_USE_BYREF   = 0x1000,           // If set convert one ptr as E_T_BYREF.
        SIG_VARARG      = 0x4000,           // If set, this is a paramarray type.  Use szArray, not System.Array.

        SIG_FLAGS_NONE  = 0                 // '0' of this enum type.
    };    

    #define IsSigIn(flags)              ((flags & SIG_IN) == SIG_IN)
    #define IsSigOut(flags)             ((flags & SIG_OUT) == SIG_OUT)
    #define IsSigRet(flags)             ((flags & SIG_RET) == SIG_RET)
    #define IsSigOpt(flags)             ((flags & SIG_OPT) == SIG_OPT)
    #define IsSigOutRet(flags)          ((flags & (SIG_OUT|SIG_RET)) == (SIG_OUT|SIG_RET))

    #define IsSigFunc(flags)            ((flags & SIG_TYPE_MASK) == SIG_FUNC)
    #define IsSigField(flags)           ((flags & SIG_TYPE_MASK) == SIG_FIELD)
    #define IsSigElem(flags)            ((flags & SIG_TYPE_MASK) == SIG_ELEM)
    
    #define IsSigUseByref(flags)        ((flags & SIG_USE_BYREF) == SIG_USE_BYREF)
    #define IsSigVarArg(flags)          ((flags & SIG_VARARG) == SIG_VARARG)

    HRESULT _ConvSignature(ITypeInfo *pITI, const TYPEDESC *pType, ULONG Flags, CQuickBytes &qbSigBuf, ULONG cbSig, ULONG *pcbSig, CQuickArray<BYTE> &qbNativeTypeBuf, ULONG cbNativeType, ULONG *pcbNativeType, BOOL bNewEnumMember, int iByRef=0);

    // For handling out-of-order vtables.
    CQuickArray<MemberInfo> m_MemberList;
    CWCHARPool              *m_pMemberNames;
    int                     m_cMemberProps;       // Count of props in memberlist.
    HRESULT BuildMemberList(ITypeInfo *pITI, int iStart, int iEnd, BOOL bInheritsIEnum);
    HRESULT FreeMemberList(ITypeInfo *pITI);

    // List of predefined token types for custom attributes.
#define INTEROP_ATTRIBUTES()                            \
        INTEROP_ATTRIBUTE(DISPID)                       \
        INTEROP_ATTRIBUTE(CLASSINTERFACE)               \
        INTEROP_ATTRIBUTE(INTERFACETYPE)                \
        INTEROP_ATTRIBUTE(TYPELIBTYPE)                  \
        INTEROP_ATTRIBUTE(TYPELIBVAR)                   \
        INTEROP_ATTRIBUTE(TYPELIBFUNC)                  \
        INTEROP_ATTRIBUTE(COMSOURCEINTERFACES)          \
        INTEROP_ATTRIBUTE(COMCONVERSIONLOSS)            \
        INTEROP_ATTRIBUTE(GUID)                         \
        INTEROP_ATTRIBUTE(DEFAULTMEMBER)                \
        INTEROP_ATTRIBUTE(COMALIASNAME)                 \
        INTEROP_ATTRIBUTE(PARAMARRAY)                   \
        INTEROP_ATTRIBUTE(LCIDCONVERSION)               \
        INTEROP_ATTRIBUTE(DECIMALVALUE)                 \
        INTEROP_ATTRIBUTE(DATETIMEVALUE)                \
        INTEROP_ATTRIBUTE(IUNKNOWNVALUE)                \
        INTEROP_ATTRIBUTE(IDISPATCHVALUE)               \
        INTEROP_ATTRIBUTE(COMVISIBLE)                   \
        INTEROP_ATTRIBUTE_SPECIAL(COMEVENTINTERFACE)    \
        INTEROP_ATTRIBUTE_SPECIAL(COCLASS)              \

#define INTEROP_ATTRIBUTE(x) ATTR_##x,
#define INTEROP_ATTRIBUTE_SPECIAL(x) ATTR_##x,
    enum {INTEROP_ATTRIBUTES()
          // Last value gives array size.
          ATTR_COUNT};
#undef INTEROP_ATTRIBUTE
#undef INTEROP_ATTRIBUTE_SPECIAL

    mdToken             m_tkAttr[ATTR_COUNT];
    HRESULT GetAttrType(int attr, mdToken *ptk);

    // look up table for known type
    mdTypeRef           m_tkKnownTypes[MAX_TLB_VT];

    LPCWSTR             m_szLibrary;    // Name of typelib being imported.
    BOOL                m_bGenerateTCEAdapters;     // A flag indicating if the TCE adapters are being generated or not.
    BOOL                m_bUnsafeInterfaces;        // A flag indicating whether runtime security checks should be disabled on an interface
    BOOL                m_bSafeArrayAsSystemArray;  // A flag indicating whether to import SAFEARRAY's as System.Array's.
    BOOL                m_bTransformDispRetVals;     // A flag indicating if we should do [out,retval] transformation on disp only itfs.
    mdMemberRef         m_tkSuppressCheckAttr;      // Cached ctor for security check custom attribute
    ITypeLib            *m_pITLB;       // Typelib being imported.
    IMetaDataEmit       *m_pEmit;       // Emit API Interface pointer.
    IMetaDataImport     *m_pImport;     // Import API Interface pointer.

    BSTR                m_wzNamespace;  // Namespace of the created TypeDefs.
    mdTypeRef           m_trObject;     // Token of System.Object.
    mdTypeRef           m_trValueType;  // Token of System.ValueType.
    mdTypeRef           m_trEnum;       // Token of System.Enum.
    mdAssemblyRef       m_arSystem;     // AssemblyRef for classlib.

    ITypeInfo           *m_pITI;        // "Current" ITypeInfo being converted.
    TYPEATTR            *m_psAttr;      // "TYPEATTR" of current ITypeInfo.
    BSTR                m_szName;       // Name of current ITypeInfo.
    BSTR                m_szMember;     // Name of current Member (method or field).
    LPWSTR              m_szMngName;    // Full name of the managed type.
    
    ULONG               m_Slot;         // "Current" vtbl index within an interface.

    void                *m_psClass;     // "Current" class record. 
    mdTypeDef           m_tdTypeDef;    // Current TypeDef.
    mdTypeDef           m_tdHasDefault; // Most recent TypeDef with a default.
    enum {eImplIfaceNone, eImplIfaceDefault, eImplIface} m_ImplIface;
    mdToken             m_tkInterface;  // Interface being added to a coclass.
    BSTR                m_szInterface;  // Interface name for decoration.

    CImpTlbTypeRef      m_TRMap;        // Typeref map.
    CImpTlbLibRef       m_LibRefs;      // Referenced typelibs.
    CImpTlbDefItfToClassItfMap m_DefItfToClassItfMap; // The default interface to class interface map.
    
    CImpTlbReservedNames m_ReservedNames;    // Reserved names.
    CImpTlbEventInfoMap  m_EventInfoMap;     // Map of event info's.

    ITypeLibImporterNotifySink *m_Notify;    // Notification object.
    Assembly            *m_pAssembly;   // Containing assembly.
    Module              *m_pModule;     // Module we are emiting into.
    
#if defined(TLB_STATS)
    LARGE_INTEGER       m_freqVal;      // Frequency of perf counter.
    BOOL                m_bStats;       // If true, collect timings.
#endif // TLB_STATS
};



#endif

//-eof-************************************************************************
