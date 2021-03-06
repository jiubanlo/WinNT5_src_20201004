//+-------------------------------------------------------------------
//
//  File:        daclwrap.hxx
//
//  Contents:    class encapsulating file security.
//
//  Classes:     CDaclWrap
//
//  History:     Nov-93        Created         DaveMont
//               Oct-96        Modified        BrunoSc
//
//--------------------------------------------------------------------
#ifndef __DACLWRAP__
#define __DACLWRAP__

#include "t2.hxx"
#include "accacc.hxx"

//+-------------------------------------------------------------------
//
//  Class:      CDaclWrap
//
//  Purpose:    encapsulation of File security (ie. a wrapper around a
//              DACL)  This class provides methods to build a new acl,
//              with intent to merge it with an existing DACL on a file.
//
//--------------------------------------------------------------------
typedef struct
{
    CAccountAccess *pcaa;
    ULONG option;
} SACCOUNTANDOPTIONS;

class CDaclWrap
{
public:

    CDaclWrap();

   ~CDaclWrap();

    // building the new dacl, based on previous SetAccess inputs and
    // the input DACL

ULONG BuildAcl(ACL **pnewdacl, ACL *poldacl, WCHAR revision, BOOL fdir);

    // adding, replacing and removing aces

ULONG SetAccess(ULONG option, LPWSTR Name, LPWSTR System, ULONG access, ULONG dirmask, BOOL filespec);

private:


ULONG _GetNewAclSize(ULONG *caclsize, ACL *poldacl, BOOL fdir);
ULONG _SetDeniedAce(ACL *dacl, ACCESS_MASK mask, SID *psid, BOOL fdir);
ULONG _SetAllowedAce(ACL *dacl, ACCESS_MASK mask, SID *psid, BOOL fdir, ACCESS_MASK dirmask, BOOL filespec);
ULONG _FillNewAcl(ACL *pnewdacl, ACL *poldacl, BOOL fdir);
ULONG _AllocateNewAcl(ACL **pnewdacl, ULONG caclsize, ULONG revision);

    ULONG               _acessize            ;
    ULONG               _ccaa                ;
    SACCOUNTANDOPTIONS  _aaa[CMAXACES]       ;
public:
	PSID				_powner;
	MODE _EditMode;
};

#endif // __DACLWRAP__






