/*

// Copyright (c) 2000-2002 Microsoft Corporation, All Rights Reserved
 *
 *	Created:	4/21/2000, Kevin Hughes
 */


#pragma once

class CSidAndAttribute
{
public:
    CSidAndAttribute() {}
    CSidAndAttribute(
        CSid& csidIn,
        DWORD dwAttribIn)
    {
        m_sid = csidIn;
        m_dwAttributes = dwAttribIn;
    }

    virtual ~CSidAndAttribute() {}
    CSid m_sid;
    DWORD m_dwAttributes; 
};


class Privilege
{
public:

    Privilege() : dwAttributes(0) {}
    virtual ~Privilege() {}
    Privilege(
        CHString& strIn,
        DWORD attribsIn)
      : dwAttributes(attribsIn)
    {
        chstrName = strIn;
    }

    CHString chstrName;
	DWORD dwAttributes;
};


typedef std::vector<CSidAndAttribute> SANDATTRIBUTE_VECTOR;
typedef std::vector<Privilege> PRIVILEGE_VECTOR;

//
// forwarding
//
class CSecurityDescriptor;

class CToken
{
public:
    CToken();
    CToken(const CToken& rTok);
    virtual ~CToken();

	void CleanToken () ;

    BOOL Duplicate	(
						const CToken& rTok,
						BOOL bReInit = TRUE,
						DWORD dwDesiredAccess = TOKEN_IMPERSONATE | TOKEN_DUPLICATE | TOKEN_QUERY,
						SECURITY_IMPERSONATION_LEVEL ImpersonationLevel = SecurityImpersonation,
						TOKEN_TYPE type = TokenImpersonation

					) ;

	BOOL GetTokenType ( TOKEN_TYPE& type ) const;
	BOOL IsValidToken ()
	{
		return m_fIsValid ;
	}

    long GetPrivCount() const;
    long GetGroupCount() const;

    bool GetPrivilege(
        Privilege* privOut,
        long lPos) const;

    bool GetGroup(
        CSid* sidOut,
        long lPos) const;

    // Deletes a member from the access token's
    // member list, and applies the change.  
    bool DeleteGroup(
        CSid& sidToDelete);

    // Adds a member to the specified group to
    // the list of token groups.
    bool AddGroup(
        CSid& sidToAdd, 
        DWORD dwAttributes);

    CToken& operator=(const CToken& rv);

    HANDLE GetTokenHandle() const;

    bool GetTokenOwner(
        CSid* sidOwner) const;

    // NOTE: hands back internal descriptor.
    bool GetDefaultSD(
        CSecurityDescriptor** ppsdDefault);

    DWORD SetDefaultSD(
        CSecurityDescriptor& SourceSD);

    DWORD EnablePrivilege(
        CHString& strPrivilegeName);

    DWORD DisablePrivilege(
        CHString& chstrPrivilegeName);

    void Dump(WCHAR* pszFileName);


protected:
    
    DWORD ReinitializeAll();    
    HANDLE m_hToken;
    DWORD m_dwLastError;
    bool m_fIsValid;

private:

    
	DWORD ReinitializeOwnerSid();
	DWORD ReinitializeDefaultSD();
	DWORD RebuildGroupList();
	DWORD RebuildPrivilegeList();
    DWORD GTI(
        TOKEN_INFORMATION_CLASS TokenInformationClass,
        PVOID* ppvBuff);
    bool ApplyTokenGroups();

   
    CSid m_sidTokenOwner;					
	CSecurityDescriptor* m_psdDefault;				// Default security info
	SANDATTRIBUTE_VECTOR m_vecGroupsAndAttributes;	// List of groups and their attributes
	PRIVILEGE_VECTOR m_vecPrivileges;				// List of privileges

protected:

	bool m_fClose;

};



class CProcessToken : public CToken
{
public:
    CProcessToken	(
						HANDLE hProcess = INVALID_HANDLE_VALUE,
						bool fGetHandleOnly = true,
						DWORD dwDesiredAccess = MAXIMUM_ALLOWED
					);
    
    virtual ~CProcessToken() {}

private:

};


class CThreadToken : public CToken
{
public:
    //CThreadToken();

    CThreadToken	(
						HANDLE hThread = INVALID_HANDLE_VALUE,
						bool fGetHandleOnly = true,
						bool fAccessCheckProcess = false, 
						DWORD dwDesiredAccess = MAXIMUM_ALLOWED
			        );

    virtual ~CThreadToken() {}

private:

};
