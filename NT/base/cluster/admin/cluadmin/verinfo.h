/////////////////////////////////////////////////////////////////////////////
//
//	Copyright (c) 1996 Microsoft Corporation
//
//	Module Name:
//		VerInfo.h
//
//	Abstract:
//		Definition of the CVersionInfo class.
//
//	Implementation File:
//		VerInfo.cpp
//
//	Author:
//		David Potter (davidp)	October 11, 1996
//
//	Revision History:
//
//	Notes:
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _VERINFO_H_
#define _VERINFO_H_

/////////////////////////////////////////////////////////////////////////////
// Include Files
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// Forward Class Declarations
/////////////////////////////////////////////////////////////////////////////

class CVersionInfo;

/////////////////////////////////////////////////////////////////////////////
// External Class Declarations
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CVersionInfo:
/////////////////////////////////////////////////////////////////////////////

class CVersionInfo
{
public:
	// Construction
	CVersionInfo(void);
	~CVersionInfo(void);

	// Secondary construction.
	void		Init(void);

// Operations
public:
	LPCTSTR		PszQueryValue(IN LPCTSTR pszValueName);
	BOOL		BQueryValue(
					IN LPCTSTR	pszValueName,
					OUT DWORD &	rdwValue
					);
	const VS_FIXEDFILEINFO *	PffiQueryValue(void);
	void		QueryFileVersionDisplayString(OUT CString & rstrValue);

// Implementation
protected:
	LPBYTE		m_pbVerInfo;

	LPBYTE		PbVerInfo(void)		{ return m_pbVerInfo; }

};  //*** class CVersionInfo

/////////////////////////////////////////////////////////////////////////////

#endif // _VERINFO_H_
