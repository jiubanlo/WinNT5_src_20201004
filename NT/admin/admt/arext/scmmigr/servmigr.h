// ServMigr.h : Declaration of the CServMigr

#ifndef __SERVMIGR_H_
#define __SERVMIGR_H_

#include "resource.h"       // main symbols
#include "TNode.hpp"
#include "EaLen.hpp"
#include "UString.hpp"
#include "ExtSeq.h"
#include <wincrypt.h>
#include "crypt.hxx"

#define LEN_Service     200
//#import "\bin\DBManager.tlb" no_namespace, named_guids
#import "DBMgr.tlb" no_namespace, named_guids

#include "CommaLog.hpp"

class TEntryNode : public TNode
{
   WCHAR                     computer[LEN_Computer];
   WCHAR                     service[LEN_Service];
   WCHAR                     account[LEN_Account];
   CSecureString             cryptpassword;
public:
   TEntryNode(WCHAR const * c,WCHAR const * s,WCHAR const * a,WCHAR const* p)
   {
      safecopy(computer,c);
      safecopy(service,s);
      safecopy(account,a);
      cryptpassword = (p ? p: L""); // can throw exception
   }
   WCHAR const * GetComputer() { return computer; }
   WCHAR const * GetService() { return service; }
   WCHAR const * GetAccount() { return account; }
   WCHAR const * GetPassword()      // can throw exception
   {
      WCHAR* p;
      if (!cryptpassword.GetString(&p))
      {
          _com_issue_error(SEC_E_DECRYPT_FAILURE);
      }
      
      return p;
   }
   void const ReleasePassword() { cryptpassword.ReleaseString(); }
   void SetPassword(WCHAR const* p) { cryptpassword = (p ? p: L""); }   // can throw exception
};

class TEntryList : public TNodeList
{
   WCHAR                     file[LEN_Path];
public:
   TEntryList(WCHAR const * filename) { safecopy(file,filename); LoadFromFile(file);  }
   DWORD LoadFromFile(WCHAR const * filename);
   DWORD SaveToFile(WCHAR const * filename);
   ~TEntryList() { SaveToFile(file); DeleteAllListItems(TEntryNode); }
private:
   HCRYPTPROV AcquireContext(bool bContainerMustExist);
};


/////////////////////////////////////////////////////////////////////////////
// CServMigr
class ATL_NO_VTABLE CServMigr : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CServMigr, &CLSID_ServMigr>,
	public IDispatchImpl<IExtendAccountMigration, &IID_IExtendAccountMigration, &LIBID_SCMMIGRLib>,
   public ISvcMgr
{
      TEntryList        m_List;
      IIManageDBPtr     m_pDB;
      BOOL              m_bFatal;
      CPasswordLog      m_passwordLog;       //Password file
      bool              m_bTriedToOpenFile; 
      long              m_Sequence;

    _bstr_t m_strSourceDomain;
    _bstr_t m_strSourceDomainFlat;
    _bstr_t m_strTargetDomain;
    _bstr_t m_strTargetDomainFlat;
    _bstr_t m_strSourceSam;
    _bstr_t m_strTargetSam;

public:
   CServMigr() : m_List(L"SCMData.txt")
	{
      HRESULT           hr = m_pDB.CreateInstance(CLSID_IManageDB);
      
      if ( FAILED(hr) )
      {
         m_bFatal = TRUE;
      }
      else
      {
         m_bFatal = FALSE;
      }
      m_bTriedToOpenFile = FALSE;
      m_Sequence = AREXT_DEFAULT_SEQUENCE_NUMBER;
	}

DECLARE_REGISTRY_RESOURCEID(IDR_SvcMgr)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CServMigr)
	COM_INTERFACE_ENTRY(IExtendAccountMigration)
   COM_INTERFACE_ENTRY(ISvcMgr)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()

// IExtendAccountMigration
public:
   STDMETHOD(ProcessUndo)(/*[in]*/ IUnknown * pSource, /*[in]*/ IUnknown * pTarget, /*[in]*/ IUnknown * pMainSettings, /*[in, out]*/ IUnknown ** pPropToSet, /*[in,out]*/ EAMAccountStats* pStats);
   STDMETHOD(PreProcessObject)(/*[in]*/ IUnknown * pSource, /*[in]*/ IUnknown * pTarget, /*[in]*/ IUnknown * pMainSettings, /*[in,out]*/  IUnknown ** ppPropsToSet, /*[in,out]*/ EAMAccountStats* pStats);
   STDMETHOD(ProcessObject)(/*[in]*/ IUnknown * pSource, /*[in]*/ IUnknown * pTarget, /*[in]*/ IUnknown * pMainSettings, /*[in,out]*/  IUnknown ** ppPropsToSet, /*[in,out]*/ EAMAccountStats* pStats);
   STDMETHOD(get_sDesc)(/*[out, retval]*/ BSTR *pVal);
   STDMETHOD(put_sDesc)(/*[in]*/ BSTR newVal);
   STDMETHOD(get_sName)(/*[out, retval]*/ BSTR *pVal);
   STDMETHOD(put_sName)(/*[in]*/ BSTR newVal);
   STDMETHOD(get_SequenceNumber)(/*[out, retval]*/ LONG * value) { (*value) = m_Sequence; return S_OK; }
protected:
   // Helper functions
   BOOL UpdateSCMs(IUnknown * pVarSet,WCHAR const * account, WCHAR const * password,WCHAR const * strSid,IIManageDB * pDB, EAMAccountStats* pStats);
   HRESULT SaveEncryptedPassword(WCHAR const * server,WCHAR const * service,WCHAR const * account,WCHAR const * password);
   DWORD DoUpdate(WCHAR const * acount,WCHAR const * password,WCHAR const * strSid,WCHAR const * computer,WCHAR const * service,BOOL bNeedToGrantLOS, EAMAccountStats* pStats);
   BOOL GetDirectory(WCHAR* filename);
   BOOL RetrieveOriginalAccount(_bstr_t &sSrcDom, _bstr_t &sSrcSAM);
   _bstr_t GetUPNName(_bstr_t sSrcSAM);

   // ISvcMgr
public:
   STDMETHOD(TryUpdateSam)(BSTR computer,BSTR service,BSTR account);
   STDMETHOD(TryUpdateSamWithPassword)(BSTR computer,BSTR service,BSTR account,BSTR password);
};

#endif //__SERVMIGR_H_
