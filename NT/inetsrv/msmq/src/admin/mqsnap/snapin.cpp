//////////////////////////////////////////////////////////////////////////////
/*++

Copyright (c) 1997 Microsoft Corporation

Module Name:

	snapin.cpp

Abstract:

	Implementation file for the CSnapinExt snapin node class.
    This class expands the DS Snapin objects

Author:

    RaphiR


--*/
//////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "mqsnap.h"
#include "Snapin.h"
#include "rdmsg.h"
#include "globals.h"
#include "cpropmap.h"
#include "message.h"
#include "localadm.h"
#include "dsext.h"
#include "qnmsprov.h"
#include "localfld.h"
#include "privadm.h"

#import "mqtrig.tlb" no_namespace

#include "mqppage.h"
#include "rule.h"
#include "trigger.h"
#include "trigdef.h"
#include "ruledef.h"

#include "snapin.tmh"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CSnapinComponentData
static const GUID CSnapinGUID_NODETYPE = 
{ 0x74e5637c, 0xb98c, 0x11d1, { 0x9b, 0x9b, 0x0, 0xe0, 0x2c, 0x6, 0x4c, 0x39 } };
const GUID*  CSnapinData::m_NODETYPE = &CSnapinGUID_NODETYPE;
const OLECHAR* CSnapinData::m_SZNODETYPE = OLESTR("74E5637C-B98C-11D1-9B9B-00E02C064C39");
const OLECHAR* CSnapinData::m_SZDISPLAY_NAME = OLESTR("MSMQAdmin");
const CLSID* CSnapinData::m_SNAPIN_CLASSID = &CLSID_MSMQSnapin;

HRESULT CSnapinData::CreatePropertyPages(LPPROPERTYSHEETCALLBACK lpProvider,
    LONG_PTR handle, 
	IUnknown* /*pUnk*/,
	DATA_OBJECT_TYPES type)
{
	if (type == CCT_SCOPE || type == CCT_RESULT)
	{
		P<CSnapinPage> pPage = new CSnapinPage(handle, true, _T("Snapin"));
		lpProvider->AddPage(pPage->Create());
		// The second parameter  to the property page class constructor
		// should be true for only one page.

		// TODO : Add code here to add additional pages
		pPage.detach();
		return S_OK;
	}
	return E_UNEXPECTED;
}

HRESULT CSnapinData::GetScopePaneInfo(SCOPEDATAITEM *pScopeDataItem)
{
	if (pScopeDataItem->mask & SDI_STR)
		pScopeDataItem->displayname = m_bstrDisplayName;
	if (pScopeDataItem->mask & SDI_IMAGE)
		pScopeDataItem->nImage = m_scopeDataItem.nImage;
	if (pScopeDataItem->mask & SDI_OPENIMAGE)
		pScopeDataItem->nOpenImage = m_scopeDataItem.nOpenImage;
	if (pScopeDataItem->mask & SDI_PARAM)
		pScopeDataItem->lParam = m_scopeDataItem.lParam;
	if (pScopeDataItem->mask & SDI_STATE )
		pScopeDataItem->nState = m_scopeDataItem.nState;

	// TODO : Add code for SDI_CHILDREN 
	return S_OK;
}

HRESULT CSnapinData::GetResultPaneInfo(RESULTDATAITEM *pResultDataItem)
{
	if (pResultDataItem->bScopeItem)
	{
		if (pResultDataItem->mask & RDI_STR)
		{
			pResultDataItem->str = GetResultPaneColInfo(pResultDataItem->nCol);
		}
		if (pResultDataItem->mask & RDI_IMAGE)
		{
			pResultDataItem->nImage = m_scopeDataItem.nImage;
		}
		if (pResultDataItem->mask & RDI_PARAM)
		{
			pResultDataItem->lParam = m_scopeDataItem.lParam;
		}

		return S_OK;
	}

	if (pResultDataItem->mask & RDI_STR)
	{
		pResultDataItem->str = GetResultPaneColInfo(pResultDataItem->nCol);
	}
	if (pResultDataItem->mask & RDI_IMAGE)
	{
		pResultDataItem->nImage = m_resultDataItem.nImage;
	}
	if (pResultDataItem->mask & RDI_PARAM)
	{
		pResultDataItem->lParam = m_resultDataItem.lParam;
	}
	if (pResultDataItem->mask & RDI_INDEX)
	{
		pResultDataItem->nIndex = m_resultDataItem.nIndex;
	}

	return S_OK;
}

HRESULT CSnapinData::Notify( MMC_NOTIFY_TYPE event,
    LPARAM arg,
    LPARAM /*param*/,
	IComponentData* pComponentData,
	IComponent* pComponent,
	DATA_OBJECT_TYPES /*type*/)
{
	// Add code to handle the different notifications.
	// Handle MMCN_SHOW and MMCN_EXPAND to enumerate children items.
	// In response to MMCN_SHOW you have to enumerate both the scope
	// and result pane items.
	// For MMCN_EXPAND you only need to enumerate the scope items
	// Use IConsoleNameSpace::InsertItem to insert scope pane items
	// Use IResultData::InsertItem to insert result pane item.
	HRESULT hr = E_NOTIMPL;

	
	_ASSERTE(pComponentData != NULL || pComponent != NULL);

	CComPtr<IConsole> spConsole;
	CComQIPtr<IHeaderCtrl, &IID_IHeaderCtrl> spHeader;
	if (pComponentData != NULL)
		spConsole = ((CSnapin*)pComponentData)->m_spConsole;
	else
	{
		spConsole = ((CSnapinComponent*)pComponent)->m_spConsole;
		spHeader = spConsole;
	}

	switch (event)
	{
	case MMCN_SHOW:
		{
			CComQIPtr<IResultData, &IID_IResultData> spResultData(spConsole);
			// TODO : Enumerate the result pane items
			hr = S_OK;
			break;
		}
	case MMCN_EXPAND:
		{
			CComQIPtr<IConsoleNameSpace, &IID_IConsoleNameSpace> spConsoleNameSpace(spConsole);
			// TODO : Enumerate scope pane items
			hr = S_OK;
			break;
		}
	case MMCN_ADD_IMAGES:
		{
			// Add Images
			IImageList* pImageList = (IImageList*) arg;
			hr = E_FAIL;
			// Load bitmaps associated with the scope pane
			// and add them to the image list
			// Loads the default bitmaps generated by the wizard
			// Change as required
			CBitmapHandle hBitmap16 = LoadBitmap(g_hResourceMod, MAKEINTRESOURCE(IDR_MMCICONS_16x16));
			if (hBitmap16 != NULL)
			{
				CBitmapHandle hBitmap32 = LoadBitmap(g_hResourceMod, MAKEINTRESOURCE(IDR_MMCICONS_32x32));
				if (hBitmap32 != NULL)
				{
					hr = pImageList->ImageListSetStrip(
										reinterpret_cast<LONG_PTR*>((HBITMAP)hBitmap16), 
										reinterpret_cast<LONG_PTR*>((HBITMAP)hBitmap32),
										0, 
										RGB(0, 128, 128)
										);
					if (FAILED(hr))
						ATLTRACE(_T("IImageList::ImageListSetStrip failed\n"));
				}
			}
			break;
		}  
	}
	return hr;
}

LPOLESTR CSnapinData::GetResultPaneColInfo(int nCol)
{
	if (nCol == 0)
		return m_bstrDisplayName;
	// TODO : Return the text for other columns
	return OLESTR("Override GetResultPaneColInfo");
}

/******************************************************************
 *
 *                  CSnapin
 *
 ******************************************************************/
//////////////////////////////////////////////////////////////////////////////
/*++

CSnapin::GetClassID

--*/
//////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CSnapin::GetClassID(CLSID* pClassID)
{
	ATLTRACE(_T("CSnapin::GetClassID\n"));
	_ASSERTE( pClassID != NULL );

	*pClassID = CLSID_MSMQSnapin;
	return S_OK;
}



//////////////////////////////////////////////////////////////////////////////
/*++

CSnapin::IsDirty

--*/
//////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CSnapin::IsDirty()
{
	ATLTRACE(_T("CSnapin::IsDirty\n"));

	// We just return S_OK because we're always dirty.
	return S_OK;
}


//
// Version number of the file data
// To be incremented each time the data of the 
// saved file changes.
//              
static const DWORD x_dwFileVersion = 5;
//////////////////////////////////////////////////////////////////////////////
/*++

CComponentData::Load


--*/
//////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CSnapin::Load(IStream* pStream)
{
	ATLTRACE(_T("CSnapin::Load"));

	
	_ASSERTE( pStream != NULL );
    HRESULT hr;
    DWORD version;
    DWORD res;

    //
    // Load Version
    //
    hr = pStream->Read(&version, sizeof(DWORD), &res);

    //
    // res will be zero if the msc file was saved without mqsnap information
    //
    if(FAILED(hr) || res == 0)
        return(hr);


    ASSERT(res == sizeof(DWORD));

    //
    // Check version number
    //
    if(version != x_dwFileVersion)
        return(S_OK);

    //
    // Placeholder - load any information that the snapin had saved
    //

    return(hr);
}



//////////////////////////////////////////////////////////////////////////////
/*++

CSnapin::Save


--*/
//////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CSnapin::Save(IStream* pStream, BOOL  /*clearDirty*/)
{
	ATLTRACE(_T("CComponentData::Save"));


	_ASSERTE( pStream != NULL );
    HRESULT hr;
    DWORD res;

    //
    // Save Version
    //
    hr = pStream->Write(&x_dwFileVersion, sizeof(DWORD), &res);
    ASSERT(res == sizeof(DWORD));

    if(FAILED(hr))
        return(hr);

    //
    // Placeholder - any saved data should be placed here.
    //

    return(hr);


}



//////////////////////////////////////////////////////////////////////////////
/*++

CSnapin::GetSizeMax

--*/
//////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CSnapin::GetSizeMax(ULARGE_INTEGER* size)
{
	ATLTRACE(_T("CSnapin::GetSizeMax\n"));
    DWORD dwSize;

    //
    // Add version number size
    //
    dwSize = sizeof(DWORD);


	size->HighPart = 0;
	size->LowPart = dwSize;

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////////
/*++

CSnapin::Initialize

--*/
//////////////////////////////////////////////////////////////////////////////
HRESULT CSnapin::Initialize(LPUNKNOWN pUnknown)
{
	HRESULT hr = IComponentDataImpl<CSnapin, CSnapinComponent >::Initialize(pUnknown);
	if (FAILED(hr))
		return hr;

	CComPtr<IImageList> spImageList;

	if (m_spConsole->QueryScopeImageList(&spImageList) != S_OK)
	{
		ATLTRACE(_T("IConsole::QueryScopeImageList failed\n"));
		return E_UNEXPECTED;
	}

	// Load bitmaps associated with the scope pane
	// and add them to the image list
	// Loads the default bitmaps generated by the wizard
	// Change as required
	CBitmapHandle hBitmap16 = LoadBitmap(g_hResourceMod, MAKEINTRESOURCE(IDR_MMCICONS_16x16));
	if (hBitmap16 == NULL)
		return S_OK;

	CBitmapHandle hBitmap32 = LoadBitmap(g_hResourceMod, MAKEINTRESOURCE(IDR_MMCICONS_16x16));
	if (hBitmap32 == NULL)
		return S_OK;

	if (spImageList->ImageListSetStrip(
						reinterpret_cast<LONG_PTR*>((HBITMAP)hBitmap16), 
						reinterpret_cast<LONG_PTR*>((HBITMAP)hBitmap32),
						0, 
						RGB(0, 128, 128)
						) != S_OK)
	{
		ATLTRACE(_T("IImageList::ImageListSetStrip failed\n"));
		return E_UNEXPECTED;
	}

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////////
/*++

CSnapinComponent::Notify

--*/
//////////////////////////////////////////////////////////////////////////////
HRESULT 
CSnapinComponent::Notify(
	LPDATAOBJECT lpDataObject, 
	MMC_NOTIFY_TYPE event, 
	LPARAM arg, 
	LPARAM param
	)
{
    HRESULT hr = S_OK;


    if(lpDataObject != NULL && MMCN_SHOW != event)
     		return IComponentImpl<CSnapinComponent>::Notify(lpDataObject, event, arg, param);


    //
    // In this routine we handle only 
    // lpDataObject ==NULL, or we have a MMCN_SHOW event.
    //

    
    switch (event)
	{
		case MMCN_SHOW:
		{
		  //
		  // On Show event, we want to keep, or reset
		  // the node currenlty selected
		  //
		  ASSERT(lpDataObject != NULL);

		  //
		  // Retreive the pItem data type
		  //
		  CSnapInItem* pItem;
		  DATA_OBJECT_TYPES type;
		  hr = m_pComponentData->GetDataClass(lpDataObject, &pItem, &type);
  
		  if(FAILED(hr))
			  return(hr);

		  if( arg )
		  {
			 // We are being selected.   
			 m_pSelectedNode = pItem;

		  }
		  else
		  {
			 // We are being deselected.

			 // Check to make sure that our result view doesn't think
			 // this node is the currently selected one.
			 if( m_pSelectedNode == pItem)
			 {
				// We don't want to be the selected node anymore.
				m_pSelectedNode = NULL;
			 }

		  }

		  //
		  // Call SnapinItem notification routine
		  //
		  return IComponentImpl<CSnapinComponent>::Notify(lpDataObject, event, arg, param);
		}

		case MMCN_COLUMN_CLICK:
		{
			//
			// A column was clicked. We will sort the result pane accordingly
			//
            return S_OK;
		}
	}



    //
    // lpDataObject == NULL
    //

    // Currently handling only View Change (UpdateAllViews)
	ASSERT(event == MMCN_VIEW_CHANGE);

    if (param == UPDATE_REMOVE_ALL_RESULT_NODES) // delete all result items
	{
		CComQIPtr<IResultData, &IID_IResultData> spResultData(m_spConsole);

		hr = spResultData->DeleteAllRsltItems();
		return hr;
	}
     
	if( ( arg == NULL || (CSnapInItem *) arg == m_pSelectedNode ) && m_pSelectedNode != NULL )
    {

      // We basically tell MMC to simulate reselecting the
      // currently selected node, which causes it to redraw.
      // This will cause MMC to send the MMCN_SHOW notification
      // to the selected node.
      // This function requires an HSCOPEITEM.  This is the ID member
      // of the HSCOPEDATAITEM associated with this node.
      SCOPEDATAITEM *pScopeDataItem;
      m_pSelectedNode->GetScopeData( &pScopeDataItem );
      return m_spConsole->SelectScopeItem( pScopeDataItem->ID );
   }

  return(hr);	

}

//
// IResultDataCompare
//
STDMETHODIMP CSnapinComponent::Compare(LPARAM lUserParam, MMC_COOKIE cookieA, MMC_COOKIE cookieB, int* pnResult)
{
    ASSERT(cookieA != 0 && cookieB != 0);

	CSnapInItem* pItemA = (CSnapInItem*) cookieA;
	CSnapInItem* pItemB = (CSnapInItem*) cookieB;

    GUID guidTypeA, guidTypeB;

    HRESULT hr = GetSnapinItemNodeType(pItemA, &guidTypeA);
    if FAILED(hr)
    {
        return hr;
    }

    hr = GetSnapinItemNodeType(pItemB, &guidTypeB);
    if FAILED(hr)
    {
        return hr;
    }

    if (guidTypeA != guidTypeB)
    {
        //
        // Different types. Consider them equal
        //
        *pnResult = 0;
        return S_OK;
    }

    //
    // Handle different types
    //
    if (guidTypeA == *CMessage::m_NODETYPE)
    {
        return ((CMessage*)pItemA)->Compare(lUserParam, (CMessage*)pItemB, pnResult);
    }

	if (guidTypeA == *CTrigResult::m_NODETYPE)
	{
		CTrigResult::Compare(
			reinterpret_cast<CTrigResult*>(pItemA),
			reinterpret_cast<CTrigResult*>(pItemB),
			pnResult
			);
		return S_OK;
	} 

	if (guidTypeA == *CRuleResult::m_NODETYPE)
	{
		CRuleResult::Compare(
			reinterpret_cast<CRuleResult*>(pItemA),
			reinterpret_cast<CRuleResult*>(pItemB),
			pnResult
			);
		return S_OK;
	} 

    return E_NOTIMPL;
};

//////////////////////////////////////////////////////////////////////////////
/*++
ISnapinHelp interface support

CSnapin::GetHelpTopic
This routine returns the path of the msmq.chm help file

--*/
//////////////////////////////////////////////////////////////////////////////
HRESULT 
STDMETHODCALLTYPE
CSnapin::GetHelpTopic(
    LPOLESTR* lpCompiledHelpFile
    )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
    WCHAR Path[MAX_PATH];

    ASSERT( lpCompiledHelpFile != NULL );

    CString strHelpPath;
    strHelpPath.LoadString(IDS_HELPPATH);

    ExpandEnvironmentStrings(strHelpPath, Path, sizeof(Path)/sizeof(WCHAR));
    *lpCompiledHelpFile = (LPOLESTR)::CoTaskMemAlloc((wcslen(Path) + 1) * sizeof(Path[0]));
    wcscpy(*lpCompiledHelpFile, Path );

    return S_OK;
}

//
// GetSnapinItemNodeType - Get the GUID node type of a snapin item
//
HRESULT GetSnapinItemNodeType(CSnapInItem *pNode, GUID *pGuidNode)
{
    //
    // Gets the other Node type 
    //
	CGlobalPointer hGlobal(GPTR, sizeof(GUID));
    if (0 == (HGLOBAL)hGlobal)
    {
        return E_OUTOFMEMORY;
    }

	CComPtr<IStream> spStream;
	HRESULT hr = CreateStreamOnHGlobal(hGlobal, FALSE, &spStream);
	if (FAILED(hr))
    {
        return hr;
    }

    hr = pNode->FillData(CSnapInItem::m_CCF_NODETYPE, spStream);
    if FAILED(hr)
    {
        return hr;
    }

    *pGuidNode = *(GUID *)((HGLOBAL)hGlobal);

    return S_OK;
}
