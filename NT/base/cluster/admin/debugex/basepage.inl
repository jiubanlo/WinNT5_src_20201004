/////////////////////////////////////////////////////////////////////////////
//
//	Copyright (c) 1997-2000 Microsoft Corporation
//
//	Module Name:
//		BasePage.inl
//
//	Description:
//		Implementation of inline methods of the CBasePropertyPage class.
//
//	Maintained By:
//		David Potter (DavidP) Mmmm DD, 1997
//
//	Revision History:
//
//	Notes:
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _BASEPAGE_INL_
#define _BASEPAGE_INL_

/////////////////////////////////////////////////////////////////////////////
// Include Files
/////////////////////////////////////////////////////////////////////////////

#ifndef _BASEPAGE_H_
#include "BasePage.h"
#endif

/////////////////////////////////////////////////////////////////////////////

BOOL CBasePropertyPage::BWizard(void) const
{
	ASSERT(Peo() != NULL);
	return Peo()->BWizard();
}

HCLUSTER CBasePropertyPage::Hcluster(void) const
{
	ASSERT(Peo() != NULL);
	return Peo()->Hcluster();
}

CLUADMEX_OBJECT_TYPE CBasePropertyPage::Cot(void) const
{
	ASSERT(Peo() != NULL);
	return Peo()->Cot();
}

/////////////////////////////////////////////////////////////////////////////

#endif // _BASEPAGE_INL_
