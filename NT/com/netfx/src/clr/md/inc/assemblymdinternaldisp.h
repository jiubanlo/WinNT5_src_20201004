// ==++==
// 
//   Copyright (c) Microsoft Corporation.  All rights reserved.
// 
// ==--==
//*****************************************************************************
// AssemblyMDInternalDispenser.h
//
// Contains utility code for MD directory
//
//*****************************************************************************
#ifndef __AssemblyMDInternalDispenser__h__
#define __AssemblyMDInternalDispenser__h__



#include "..\Runtime\MDInternalRO.h"
#include "fusionpriv.h"

//*****************************************************************************
// This class can support the IMetaDataAssemblyImport and some funcationalities 
// of IMetaDataImport on the internal import interface (IMDInternalImport).
//*****************************************************************************
class AssemblyMDInternalImport :  public IMetaDataAssemblyImport, public IMetaDataImport, public IAssemblySignature
{
public:
    AssemblyMDInternalImport(IMDInternalImport *pMDInternalImport);
    ~AssemblyMDInternalImport();

    // *** IUnknown methods ***
    STDMETHODIMP    QueryInterface(REFIID riid, void** ppUnk);
    STDMETHODIMP_(ULONG) AddRef(void);
    STDMETHODIMP_(ULONG) Release(void);

    // *** IMetaDataAssemblyImport methods ***
    STDMETHODIMP GetAssemblyProps (         // S_OK or error.
        mdAssembly  mda,                    // [IN] The Assembly for which to get the properties.
        const void  **ppbPublicKey,         // [OUT] Pointer to the public key.
        ULONG       *pcbPublicKey,          // [OUT] Count of bytes in the public key.
        ULONG       *pulHashAlgId,          // [OUT] Hash Algorithm.
        LPWSTR      szName,                 // [OUT] Buffer to fill with name.
        ULONG       cchName,                // [IN] Size of buffer in wide chars.
        ULONG       *pchName,               // [OUT] Actual # of wide chars in name.
        ASSEMBLYMETADATA *pMetaData,        // [OUT] Assembly MetaData.
        DWORD       *pdwAssemblyFlags);         // [OUT] Flags.

    STDMETHODIMP GetAssemblyRefProps (      // S_OK or error.
        mdAssemblyRef mdar,                 // [IN] The AssemblyRef for which to get the properties.
        const void  **ppbPublicKeyOrToken,  // [OUT] Pointer to the public key or token.
        ULONG       *pcbPublicKeyOrToken,   // [OUT] Count of bytes in the public key or token.
        LPWSTR      szName,                 // [OUT] Buffer to fill with name.
        ULONG       cchName,                // [IN] Size of buffer in wide chars.
        ULONG       *pchName,               // [OUT] Actual # of wide chars in name.
        ASSEMBLYMETADATA *pMetaData,        // [OUT] Assembly MetaData.
        const void  **ppbHashValue,         // [OUT] Hash blob.
        ULONG       *pcbHashValue,          // [OUT] Count of bytes in the hash blob.
        DWORD       *pdwAssemblyRefFlags);      // [OUT] Flags.

    STDMETHODIMP GetFileProps (             // S_OK or error.
        mdFile      mdf,                    // [IN] The File for which to get the properties.
        LPWSTR      szName,                 // [OUT] Buffer to fill with name.
        ULONG       cchName,                // [IN] Size of buffer in wide chars.
        ULONG       *pchName,               // [OUT] Actual # of wide chars in name.
        const void  **ppbHashValue,         // [OUT] Pointer to the Hash Value Blob.
        ULONG       *pcbHashValue,          // [OUT] Count of bytes in the Hash Value Blob.
        DWORD       *pdwFileFlags);         // [OUT] Flags.

    STDMETHODIMP GetExportedTypeProps (          // S_OK or error.
        mdExportedType   mdct,                   // [IN] The ExportedType for which to get the properties.
        LPWSTR      szName,                 // [OUT] Buffer to fill with name.
        ULONG       cchName,                // [IN] Size of buffer in wide chars.
        ULONG       *pchName,               // [OUT] Actual # of wide chars in name.
        mdToken     *ptkImplementation,     // [OUT] mdFile or mdAssemblyRef or mdExportedType.
        mdTypeDef   *ptkTypeDef,            // [OUT] TypeDef token within the file.
        DWORD       *pdwExportedTypeFlags);      // [OUT] Flags.

    STDMETHODIMP GetManifestResourceProps ( // S_OK or error.
        mdManifestResource  mdmr,           // [IN] The ManifestResource for which to get the properties.
        LPWSTR      szName,                 // [OUT] Buffer to fill with name.
        ULONG       cchName,                // [IN] Size of buffer in wide chars.
        ULONG       *pchName,               // [OUT] Actual # of wide chars in name.
        mdToken     *ptkImplementation,     // [OUT] mdFile or mdAssemblyRef that provides the ManifestResource.
        DWORD       *pdwOffset,             // [OUT] Offset to the beginning of the resource within the file.
        DWORD       *pdwResourceFlags);     // [OUT] Flags.

    STDMETHODIMP EnumAssemblyRefs (         // S_OK or error
        HCORENUM    *phEnum,                // [IN|OUT] Pointer to the enum.
        mdAssemblyRef rAssemblyRefs[],      // [OUT] Put AssemblyRefs here.
        ULONG       cMax,                   // [IN] Max AssemblyRefs to put.
        ULONG       *pcTokens);             // [OUT] Put # put here.

    STDMETHODIMP EnumFiles (                // S_OK or error
        HCORENUM    *phEnum,                // [IN|OUT] Pointer to the enum.
        mdFile      rFiles[],               // [OUT] Put Files here.
        ULONG       cMax,                   // [IN] Max Files to put.
        ULONG       *pcTokens);             // [OUT] Put # put here.

    STDMETHODIMP EnumExportedTypes (        // S_OK or error
        HCORENUM    *phEnum,                // [IN|OUT] Pointer to the enum.
        mdExportedType   rExportedTypes[],  // [OUT] Put ExportedTypes here.
        ULONG       cMax,                   // [IN] Max ExportedTypes to put.
        ULONG       *pcTokens);             // [OUT] Put # put here.

    STDMETHODIMP EnumManifestResources (    // S_OK or error
        HCORENUM    *phEnum,                // [IN|OUT] Pointer to the enum.
        mdManifestResource  rManifestResources[],   // [OUT] Put ManifestResources here.
        ULONG       cMax,                   // [IN] Max Resources to put.
        ULONG       *pcTokens);             // [OUT] Put # put here.

    STDMETHODIMP GetAssemblyFromScope (     // S_OK or error
        mdAssembly  *ptkAssembly);          // [OUT] Put token here.

    STDMETHODIMP FindExportedTypeByName (   // S_OK or error
        LPCWSTR     szName,                 // [IN] Name of the ExportedType.
        mdToken     mdtExportedType,        // [IN] ExportedType for the enclosing class.
        mdExportedType   *ptkExportedType);      // [OUT] Put the ExportedType token here.

    STDMETHODIMP FindManifestResourceByName (  // S_OK or error
        LPCWSTR     szName,                 // [IN] Name of the ManifestResource.
        mdManifestResource *ptkManifestResource);       // [OUT] Put the ManifestResource token here.

    STDMETHOD_(void, CloseEnum)(
        HCORENUM hEnum);                    // Enum to be closed.

    STDMETHODIMP FindAssembliesByName (     // S_OK or error
        LPCWSTR  szAppBase,                 // [IN] optional - can be NULL
        LPCWSTR  szPrivateBin,              // [IN] optional - can be NULL
        LPCWSTR  szAssemblyName,            // [IN] required - this is the assembly you are requesting
        IUnknown *ppIUnk[],                 // [OUT] put IMetaDataAssemblyImport pointers here
        ULONG    cMax,                      // [IN] The max number to put
        ULONG    *pcAssemblies);            // [OUT] The number of assemblies returned.

    // *** IMetaDataImport methods ***
    STDMETHOD(CountEnum)(HCORENUM hEnum, ULONG *pulCount);
    STDMETHOD(ResetEnum)(HCORENUM hEnum, ULONG ulPos);     
    STDMETHOD(EnumTypeDefs)(HCORENUM *phEnum, mdTypeDef rTypeDefs[],
                            ULONG cMax, ULONG *pcTypeDefs);     
    STDMETHOD(EnumInterfaceImpls)(HCORENUM *phEnum, mdTypeDef td,
                            mdInterfaceImpl rImpls[], ULONG cMax,
                            ULONG* pcImpls);     
    STDMETHOD(EnumTypeRefs)(HCORENUM *phEnum, mdTypeRef rTypeRefs[],
                            ULONG cMax, ULONG* pcTypeRefs);     

    STDMETHOD(FindTypeDefByName)(           // S_OK or error.
        LPCWSTR     szTypeDef,              // [IN] Name of the Type.
        mdToken     tkEnclosingClass,       // [IN] TypeDef/TypeRef for Enclosing class.
        mdTypeDef   *ptd);                  // [OUT] Put the TypeDef token here.

    STDMETHOD(GetScopeProps)(               // S_OK or error.
        LPWSTR      szName,                 // [OUT] Put the name here.
        ULONG       cchName,                // [IN] Size of name buffer in wide chars.
        ULONG       *pchName,               // [OUT] Put size of name (wide chars) here.
        GUID        *pmvid);                // [OUT, OPTIONAL] Put MVID here.

    STDMETHOD(GetModuleFromScope)(          // S_OK.
        mdModule    *pmd);                  // [OUT] Put mdModule token here.

    STDMETHOD(GetTypeDefProps)(             // S_OK or error.
        mdTypeDef   td,                     // [IN] TypeDef token for inquiry.
        LPWSTR      szTypeDef,              // [OUT] Put name here.
        ULONG       cchTypeDef,             // [IN] size of name buffer in wide chars.
        ULONG       *pchTypeDef,            // [OUT] put size of name (wide chars) here.
        DWORD       *pdwTypeDefFlags,       // [OUT] Put flags here.
        mdToken     *ptkExtends);           // [OUT] Put base class TypeDef/TypeRef here.

    STDMETHOD(GetInterfaceImplProps)(       // S_OK or error.
        mdInterfaceImpl iiImpl,             // [IN] InterfaceImpl token.
        mdTypeDef   *pClass,                // [OUT] Put implementing class token here.
        mdToken     *ptkIface);             // [OUT] Put implemented interface token here.              

    STDMETHOD(GetTypeRefProps)(             // S_OK or error.
        mdTypeRef   tr,                     // [IN] TypeRef token.
        mdToken     *ptkResolutionScope,    // [OUT] Resolution scope, ModuleRef or AssemblyRef.
        LPWSTR      szName,                 // [OUT] Name of the TypeRef.
        ULONG       cchName,                // [IN] Size of buffer.
        ULONG       *pchName);              // [OUT] Size of Name.

    STDMETHOD(ResolveTypeRef)(mdTypeRef tr, REFIID riid, IUnknown **ppIScope, mdTypeDef *ptd);     

    STDMETHOD(EnumMembers)(                 // S_OK, S_FALSE, or error. 
        HCORENUM    *phEnum,                // [IN|OUT] Pointer to the enum.    
        mdTypeDef   cl,                     // [IN] TypeDef to scope the enumeration.   
        mdToken     rMembers[],             // [OUT] Put MemberDefs here.   
        ULONG       cMax,                   // [IN] Max MemberDefs to put.  
        ULONG       *pcTokens);             // [OUT] Put # put here.    

    STDMETHOD(EnumMembersWithName)(         // S_OK, S_FALSE, or error.             
        HCORENUM    *phEnum,                // [IN|OUT] Pointer to the enum.                
        mdTypeDef   cl,                     // [IN] TypeDef to scope the enumeration.   
        LPCWSTR     szName,                 // [IN] Limit results to those with this name.              
        mdToken     rMembers[],             // [OUT] Put MemberDefs here.                   
        ULONG       cMax,                   // [IN] Max MemberDefs to put.              
        ULONG       *pcTokens);             // [OUT] Put # put here.    

    STDMETHOD(EnumMethods)(                 // S_OK, S_FALSE, or error. 
        HCORENUM    *phEnum,                // [IN|OUT] Pointer to the enum.    
        mdTypeDef   cl,                     // [IN] TypeDef to scope the enumeration.   
        mdMethodDef rMethods[],             // [OUT] Put MethodDefs here.   
        ULONG       cMax,                   // [IN] Max MethodDefs to put.  
        ULONG       *pcTokens);             // [OUT] Put # put here.    

    STDMETHOD(EnumMethodsWithName)(         // S_OK, S_FALSE, or error.             
        HCORENUM    *phEnum,                // [IN|OUT] Pointer to the enum.                
        mdTypeDef   cl,                     // [IN] TypeDef to scope the enumeration.   
        LPCWSTR     szName,                 // [IN] Limit results to those with this name.              
        mdMethodDef rMethods[],             // [OU] Put MethodDefs here.    
        ULONG       cMax,                   // [IN] Max MethodDefs to put.              
        ULONG       *pcTokens);             // [OUT] Put # put here.    

    STDMETHOD(EnumFields)(                 // S_OK, S_FALSE, or error.  
        HCORENUM    *phEnum,                // [IN|OUT] Pointer to the enum.    
        mdTypeDef   cl,                     // [IN] TypeDef to scope the enumeration.   
        mdFieldDef  rFields[],              // [OUT] Put FieldDefs here.    
        ULONG       cMax,                   // [IN] Max FieldDefs to put.   
        ULONG       *pcTokens);             // [OUT] Put # put here.    

    STDMETHOD(EnumFieldsWithName)(         // S_OK, S_FALSE, or error.              
        HCORENUM    *phEnum,                // [IN|OUT] Pointer to the enum.                
        mdTypeDef   cl,                     // [IN] TypeDef to scope the enumeration.   
        LPCWSTR     szName,                 // [IN] Limit results to those with this name.              
        mdFieldDef  rFields[],              // [OUT] Put MemberDefs here.                   
        ULONG       cMax,                   // [IN] Max MemberDefs to put.              
        ULONG       *pcTokens);             // [OUT] Put # put here.    


    STDMETHOD(EnumParams)(                  // S_OK, S_FALSE, or error. 
        HCORENUM    *phEnum,                // [IN|OUT] Pointer to the enum.    
        mdMethodDef mb,                     // [IN] MethodDef to scope the enumeration. 
        mdParamDef  rParams[],              // [OUT] Put ParamDefs here.    
        ULONG       cMax,                   // [IN] Max ParamDefs to put.   
        ULONG       *pcTokens);             // [OUT] Put # put here.    

    STDMETHOD(EnumMemberRefs)(              // S_OK, S_FALSE, or error. 
        HCORENUM    *phEnum,                // [IN|OUT] Pointer to the enum.    
        mdToken     tkParent,               // [IN] Parent token to scope the enumeration.  
        mdMemberRef rMemberRefs[],          // [OUT] Put MemberRefs here.   
        ULONG       cMax,                   // [IN] Max MemberRefs to put.  
        ULONG       *pcTokens);             // [OUT] Put # put here.    

    STDMETHOD(EnumMethodImpls)(             // S_OK, S_FALSE, or error  
        HCORENUM    *phEnum,                // [IN|OUT] Pointer to the enum.    
        mdTypeDef   td,                     // [IN] TypeDef to scope the enumeration.   
        mdToken     rMethodBody[],          // [OUT] Put Method Body tokens here.   
        mdToken     rMethodDecl[],          // [OUT] Put Method Declaration tokens here.
        ULONG       cMax,                   // [IN] Max tokens to put.  
        ULONG       *pcTokens);             // [OUT] Put # put here.    

    STDMETHOD(EnumPermissionSets)(          // S_OK, S_FALSE, or error. 
        HCORENUM    *phEnum,                // [IN|OUT] Pointer to the enum.    
        mdToken     tk,                     // [IN] if !NIL, token to scope the enumeration.    
        DWORD       dwActions,              // [IN] if !0, return only these actions.   
        mdPermission rPermission[],         // [OUT] Put Permissions here.  
        ULONG       cMax,                   // [IN] Max Permissions to put. 
        ULONG       *pcTokens);             // [OUT] Put # put here.    

    STDMETHOD(FindMember)(  
        mdTypeDef   td,                     // [IN] given typedef   
        LPCWSTR     szName,                 // [IN] member name 
        PCCOR_SIGNATURE pvSigBlob,          // [IN] point to a blob value of COM+ signature 
        ULONG       cbSigBlob,              // [IN] count of bytes in the signature blob    
        mdToken     *pmb);                  // [OUT] matching memberdef 

    STDMETHOD(FindMethod)(  
        mdTypeDef   td,                     // [IN] given typedef   
        LPCWSTR     szName,                 // [IN] member name 
        PCCOR_SIGNATURE pvSigBlob,          // [IN] point to a blob value of COM+ signature 
        ULONG       cbSigBlob,              // [IN] count of bytes in the signature blob    
        mdMethodDef *pmb);                  // [OUT] matching memberdef 

    STDMETHOD(FindField)(   
        mdTypeDef   td,                     // [IN] given typedef   
        LPCWSTR     szName,                 // [IN] member name 
        PCCOR_SIGNATURE pvSigBlob,          // [IN] point to a blob value of COM+ signature 
        ULONG       cbSigBlob,              // [IN] count of bytes in the signature blob    
        mdFieldDef  *pmb);                  // [OUT] matching memberdef 

    STDMETHOD(FindMemberRef)(   
        mdTypeRef   td,                     // [IN] given typeRef   
        LPCWSTR     szName,                 // [IN] member name 
        PCCOR_SIGNATURE pvSigBlob,          // [IN] point to a blob value of COM+ signature 
        ULONG       cbSigBlob,              // [IN] count of bytes in the signature blob    
        mdMemberRef *pmr);                  // [OUT] matching memberref 

    STDMETHOD (GetMethodProps)( 
        mdMethodDef mb,                     // The method for which to get props.   
        mdTypeDef   *pClass,                // Put method's class here. 
        LPWSTR      szMethod,               // Put method's name here.  
        ULONG       cchMethod,              // Size of szMethod buffer in wide chars.   
        ULONG       *pchMethod,             // Put actual size here 
        DWORD       *pdwAttr,               // Put flags here.  
        PCCOR_SIGNATURE *ppvSigBlob,        // [OUT] point to the blob value of meta data   
        ULONG       *pcbSigBlob,            // [OUT] actual size of signature blob  
        ULONG       *pulCodeRVA,            // [OUT] codeRVA    
        DWORD       *pdwImplFlags);         // [OUT] Impl. Flags    

    STDMETHOD(GetMemberRefProps)(           // S_OK or error.   
        mdMemberRef mr,                     // [IN] given memberref 
        mdToken     *ptk,                   // [OUT] Put classref or classdef here. 
        LPWSTR      szMember,               // [OUT] buffer to fill for member's name   
        ULONG       cchMember,              // [IN] the count of char of szMember   
        ULONG       *pchMember,             // [OUT] actual count of char in member name    
        PCCOR_SIGNATURE *ppvSigBlob,        // [OUT] point to meta data blob value  
        ULONG       *pbSig);                // [OUT] actual size of signature blob  

    STDMETHOD(EnumProperties)(              // S_OK, S_FALSE, or error. 
        HCORENUM    *phEnum,                // [IN|OUT] Pointer to the enum.    
        mdTypeDef   td,                     // [IN] TypeDef to scope the enumeration.   
        mdProperty  rProperties[],          // [OUT] Put Properties here.   
        ULONG       cMax,                   // [IN] Max properties to put.  
        ULONG       *pcProperties);         // [OUT] Put # put here.    

    STDMETHOD(EnumEvents)(                  // S_OK, S_FALSE, or error. 
        HCORENUM    *phEnum,                // [IN|OUT] Pointer to the enum.    
        mdTypeDef   td,                     // [IN] TypeDef to scope the enumeration.   
        mdEvent     rEvents[],              // [OUT] Put events here.   
        ULONG       cMax,                   // [IN] Max events to put.  
        ULONG       *pcEvents);             // [OUT] Put # put here.    

    STDMETHOD(GetEventProps)(               // S_OK, S_FALSE, or error. 
        mdEvent     ev,                     // [IN] event token 
        mdTypeDef   *pClass,                // [OUT] typedef containing the event declarion.    
        LPCWSTR     szEvent,                // [OUT] Event name 
        ULONG       cchEvent,               // [IN] the count of wchar of szEvent   
        ULONG       *pchEvent,              // [OUT] actual count of wchar for event's name 
        DWORD       *pdwEventFlags,         // [OUT] Event flags.   
        mdToken     *ptkEventType,          // [OUT] EventType class    
        mdMethodDef *pmdAddOn,              // [OUT] AddOn method of the event  
        mdMethodDef *pmdRemoveOn,           // [OUT] RemoveOn method of the event   
        mdMethodDef *pmdFire,               // [OUT] Fire method of the event   
        mdMethodDef rmdOtherMethod[],       // [OUT] other method of the event  
        ULONG       cMax,                   // [IN] size of rmdOtherMethod  
        ULONG       *pcOtherMethod);        // [OUT] total number of other method of this event 

    STDMETHOD(EnumMethodSemantics)(         // S_OK, S_FALSE, or error. 
        HCORENUM    *phEnum,                // [IN|OUT] Pointer to the enum.    
        mdMethodDef mb,                     // [IN] MethodDef to scope the enumeration. 
        mdToken     rEventProp[],           // [OUT] Put Event/Property here.   
        ULONG       cMax,                   // [IN] Max properties to put.  
        ULONG       *pcEventProp);          // [OUT] Put # put here.    

    STDMETHOD(GetMethodSemantics)(          // S_OK, S_FALSE, or error. 
        mdMethodDef mb,                     // [IN] method token    
        mdToken     tkEventProp,            // [IN] event/property token.   
        DWORD       *pdwSemanticsFlags);      // [OUT] the role flags for the method/propevent pair 

    STDMETHOD(GetClassLayout) ( 
        mdTypeDef   td,                     // [IN] give typedef    
        DWORD       *pdwPackSize,           // [OUT] 1, 2, 4, 8, or 16  
        COR_FIELD_OFFSET rFieldOffset[],    // [OUT] field offset array 
        ULONG       cMax,                   // [IN] size of the array   
        ULONG       *pcFieldOffset,         // [OUT] needed array size  
        ULONG       *pulClassSize);             // [OUT] the size of the class  

    STDMETHOD(GetFieldMarshal) (    
        mdToken     tk,                     // [IN] given a field's memberdef   
        PCCOR_SIGNATURE *ppvNativeType,     // [OUT] native type of this field  
        ULONG       *pcbNativeType);        // [OUT] the count of bytes of *ppvNativeType   

    STDMETHOD(GetRVA)(                      // S_OK or error.   
        mdToken     tk,                     // Member for which to set offset   
        ULONG       *pulCodeRVA,            // The offset   
        DWORD       *pdwImplFlags);         // the implementation flags 

    STDMETHOD(GetPermissionSetProps) (  
        mdPermission pm,                    // [IN] the permission token.   
        DWORD       *pdwAction,             // [OUT] CorDeclSecurity.   
        void const  **ppvPermission,        // [OUT] permission blob.   
        ULONG       *pcbPermission);        // [OUT] count of bytes of pvPermission.    

    STDMETHOD(GetSigFromToken)(             // S_OK or error.   
        mdSignature mdSig,                  // [IN] Signature token.    
        PCCOR_SIGNATURE *ppvSig,            // [OUT] return pointer to token.   
        ULONG       *pcbSig);               // [OUT] return size of signature.  

    STDMETHOD(GetModuleRefProps)(           // S_OK or error.   
        mdModuleRef mur,                    // [IN] moduleref token.    
        LPWSTR      szName,                 // [OUT] buffer to fill with the moduleref name.    
        ULONG       cchName,                // [IN] size of szName in wide characters.  
        ULONG       *pchName);              // [OUT] actual count of characters in the name.    

    STDMETHOD(EnumModuleRefs)(              // S_OK or error.   
        HCORENUM    *phEnum,                // [IN|OUT] pointer to the enum.    
        mdModuleRef rModuleRefs[],          // [OUT] put modulerefs here.   
        ULONG       cmax,                   // [IN] max memberrefs to put.  
        ULONG       *pcModuleRefs);         // [OUT] put # put here.    

    STDMETHOD(GetTypeSpecFromToken)(        // S_OK or error.   
        mdTypeSpec typespec,                // [IN] TypeSpec token.    
        PCCOR_SIGNATURE *ppvSig,            // [OUT] return pointer to TypeSpec signature  
        ULONG       *pcbSig);               // [OUT] return size of signature.  

    STDMETHOD(GetNameFromToken)(            // Not Recommended! May be removed!
        mdToken     tk,                     // [IN] Token to get name from.  Must have a name.
        MDUTF8CSTR  *pszUtf8NamePtr);       // [OUT] Return pointer to UTF8 name in heap.

    STDMETHOD(EnumUnresolvedMethods)(       // S_OK, S_FALSE, or error. 
        HCORENUM    *phEnum,                // [IN|OUT] Pointer to the enum.    
        mdToken     rMethods[],             // [OUT] Put MemberDefs here.   
        ULONG       cMax,                   // [IN] Max MemberDefs to put.  
        ULONG       *pcTokens);             // [OUT] Put # put here.    

    STDMETHOD(GetUserString)(               // S_OK or error.
        mdString    stk,                    // [IN] String token.
        LPWSTR      szString,               // [OUT] Copy of string.
        ULONG       cchString,              // [IN] Max chars of room in szString.
        ULONG       *pchString);            // [OUT] How many chars in actual string.

    STDMETHOD(GetPinvokeMap)(               // S_OK or error.
        mdToken     tk,                     // [IN] FieldDef or MethodDef.
        DWORD       *pdwMappingFlags,       // [OUT] Flags used for mapping.
        LPWSTR      szImportName,           // [OUT] Import name.
        ULONG       cchImportName,          // [IN] Size of the name buffer.
        ULONG       *pchImportName,         // [OUT] Actual number of characters stored.
        mdModuleRef *pmrImportDLL);         // [OUT] ModuleRef token for the target DLL.

    STDMETHOD(EnumSignatures)(              // S_OK or error.
        HCORENUM    *phEnum,                // [IN|OUT] pointer to the enum.    
        mdSignature rSignatures[],          // [OUT] put signatures here.   
        ULONG       cmax,                   // [IN] max signatures to put.  
        ULONG       *pcSignatures);         // [OUT] put # put here.

    STDMETHOD(EnumTypeSpecs)(               // S_OK or error.
        HCORENUM    *phEnum,                // [IN|OUT] pointer to the enum.    
        mdTypeSpec  rTypeSpecs[],           // [OUT] put TypeSpecs here.   
        ULONG       cmax,                   // [IN] max TypeSpecs to put.  
        ULONG       *pcTypeSpecs);          // [OUT] put # put here.

    STDMETHOD(EnumUserStrings)(             // S_OK or error.
        HCORENUM    *phEnum,                // [IN/OUT] pointer to the enum.
        mdString    rStrings[],             // [OUT] put Strings here.
        ULONG       cmax,                   // [IN] max Strings to put.
        ULONG       *pcStrings);            // [OUT] put # put here.

    STDMETHOD(GetParamForMethodIndex)(      // S_OK or error.
        mdMethodDef md,                     // [IN] Method token.
        ULONG       ulParamSeq,             // [IN] Parameter sequence.
        mdParamDef  *ppd);                  // [IN] Put Param token here.

    STDMETHOD(EnumCustomAttributes)(        // S_OK or error.
        HCORENUM    *phEnum,                // [IN, OUT] COR enumerator.
        mdToken     tk,                     // [IN] Token to scope the enumeration, 0 for all.
        mdToken     tkType,                 // [IN] Type of interest, 0 for all.
        mdCustomAttribute rCustomAttributes[], // [OUT] Put custom attribute tokens here.
        ULONG       cMax,                   // [IN] Size of rCustomAttributes.
        ULONG       *pcCustomAttributes);       // [OUT, OPTIONAL] Put count of token values here.

    STDMETHOD(GetCustomAttributeProps)(     // S_OK or error.
        mdCustomAttribute cv,               // [IN] CustomAttribute token.
        mdToken     *ptkObj,                // [OUT, OPTIONAL] Put object token here.
        mdToken     *ptkType,               // [OUT, OPTIONAL] Put AttrType token here.
        void const  **ppBlob,               // [OUT, OPTIONAL] Put pointer to data here.
        ULONG       *pcbSize);              // [OUT, OPTIONAL] Put size of date here.

    STDMETHOD(FindTypeRef)(   
        mdToken     tkResolutionScope,      // [IN] ModuleRef, AssemblyRef or TypeRef.
        LPCWSTR     szName,                 // [IN] TypeRef Name.
        mdTypeRef   *ptr);                  // [OUT] matching TypeRef.

    STDMETHOD(GetMemberProps)(  
        mdToken     mb,                     // The member for which to get props.   
        mdTypeDef   *pClass,                // Put member's class here. 
        LPWSTR      szMember,               // Put member's name here.  
        ULONG       cchMember,              // Size of szMember buffer in wide chars.   
        ULONG       *pchMember,             // Put actual size here 
        DWORD       *pdwAttr,               // Put flags here.  
        PCCOR_SIGNATURE *ppvSigBlob,        // [OUT] point to the blob value of meta data   
        ULONG       *pcbSigBlob,            // [OUT] actual size of signature blob  
        ULONG       *pulCodeRVA,            // [OUT] codeRVA    
        DWORD       *pdwImplFlags,          // [OUT] Impl. Flags    
        DWORD       *pdwCPlusTypeFlag,      // [OUT] flag for value type. selected ELEMENT_TYPE_*   
        void const  **ppValue,              // [OUT] constant value 
        ULONG       *pcchValue);            // [OUT] size of constant string in chars, 0 for non-strings.

    STDMETHOD(GetFieldProps)(  
        mdFieldDef  mb,                     // The field for which to get props.    
        mdTypeDef   *pClass,                // Put field's class here.  
        LPWSTR      szField,                // Put field's name here.   
        ULONG       cchField,               // Size of szField buffer in wide chars.    
        ULONG       *pchField,              // Put actual size here 
        DWORD       *pdwAttr,               // Put flags here.  
        PCCOR_SIGNATURE *ppvSigBlob,        // [OUT] point to the blob value of meta data   
        ULONG       *pcbSigBlob,            // [OUT] actual size of signature blob  
        DWORD       *pdwCPlusTypeFlag,      // [OUT] flag for value type. selected ELEMENT_TYPE_*   
        void const  **ppValue,              // [OUT] constant value 
        ULONG       *pcchValue);            // [OUT] size of constant string in chars, 0 for non-strings.

    STDMETHOD(GetPropertyProps)(            // S_OK, S_FALSE, or error. 
        mdProperty  prop,                   // [IN] property token  
        mdTypeDef   *pClass,                // [OUT] typedef containing the property declarion. 
        LPCWSTR     szProperty,             // [OUT] Property name  
        ULONG       cchProperty,            // [IN] the count of wchar of szProperty    
        ULONG       *pchProperty,           // [OUT] actual count of wchar for property name    
        DWORD       *pdwPropFlags,          // [OUT] property flags.    
        PCCOR_SIGNATURE *ppvSig,            // [OUT] property type. pointing to meta data internal blob 
        ULONG       *pbSig,                 // [OUT] count of bytes in *ppvSig  
        DWORD       *pdwCPlusTypeFlag,      // [OUT] flag for value type. selected ELEMENT_TYPE_*   
        void const  **ppDefaultValue,       // [OUT] constant value 
        ULONG       *pcchDefaultValue,      // [OUT] size of constant string in chars, 0 for non-strings.
        mdMethodDef *pmdSetter,             // [OUT] setter method of the property  
        mdMethodDef *pmdGetter,             // [OUT] getter method of the property  
        mdMethodDef rmdOtherMethod[],       // [OUT] other method of the property   
        ULONG       cMax,                   // [IN] size of rmdOtherMethod  
        ULONG       *pcOtherMethod);        // [OUT] total number of other method of this property  

    STDMETHOD(GetParamProps)(               // S_OK or error.
        mdParamDef  tk,                     // [IN]The Parameter.
        mdMethodDef *pmd,                   // [OUT] Parent Method token.
        ULONG       *pulSequence,           // [OUT] Parameter sequence.
        LPWSTR      szName,                 // [OUT] Put name here.
        ULONG       cchName,                // [OUT] Size of name buffer.
        ULONG       *pchName,               // [OUT] Put actual size of name here.
        DWORD       *pdwAttr,               // [OUT] Put flags here.
        DWORD       *pdwCPlusTypeFlag,      // [OUT] Flag for value type. selected ELEMENT_TYPE_*.
        void const  **ppValue,              // [OUT] Constant value.
        ULONG       *pcchValue);            // [OUT] size of constant string in chars, 0 for non-strings.

    STDMETHOD(GetCustomAttributeByName)(    // S_OK or error.
        mdToken     tkObj,                  // [IN] Object with Custom Attribute.
        LPCWSTR     szName,                 // [IN] Name of desired Custom Attribute.
        const void  **ppData,               // [OUT] Put pointer to data here.
        ULONG       *pcbData);              // [OUT] Put size of data here.

    STDMETHOD_(BOOL, IsValidToken)(         // True or False.
        mdToken     tk);                    // [IN] Given token.

    STDMETHOD(GetNestedClassProps)(         // S_OK or error.
        mdTypeDef   tdNestedClass,          // [IN] NestedClass token.
        mdTypeDef   *ptdEnclosingClass);      // [OUT] EnclosingClass token.

    STDMETHOD(GetNativeCallConvFromSig)(    // S_OK or error.
        void const  *pvSig,                 // [IN] Pointer to signature.
        ULONG       cbSig,                  // [IN] Count of signature bytes.
        ULONG       *pCallConv);            // [OUT] Put calling conv here (see CorPinvokemap).                                                                                        

    STDMETHOD(IsGlobal)(                    // S_OK or error.
        mdToken     pd,                     // [IN] Type, Field, or Method token.
        int         *pbGlobal);             // [OUT] Put 1 if global, 0 otherwise.

    // *** IAssemblySignature methods ***
    STDMETHOD(GetAssemblySignature)(        // S_OK or error.
        BYTE        *pbSig,                 // [IN, OUT] Buffer to write signature
        DWORD       *pcbSig);               // [IN, OUT] Size of buffer, bytes written

    void SetHandle(HCORMODULE hHandle)
    {
        m_pHandle = hHandle;
    }

    void SetBase(LPVOID base)
    {
        m_pBase = base;
    }

private:
    ULONG                                   m_cRef;
    HCORMODULE                              m_pHandle;              // Handle to a cached PE image
    LPVOID                                  m_pBase;                // File mapping (if runtime is not init'd)
    IMDInternalImport                       *m_pMDInternalImport;
#ifdef _DEBUG
    IMetaDataAssemblyImport                 *m_pDebugMDImport;
#endif //_DEBUG
};

#endif // __AssemblyMDInternalDispenser__h__
