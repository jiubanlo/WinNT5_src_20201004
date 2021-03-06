// Patch.cpp: implementation of the CPatch class.
//
// Copyright (c) 1997-2002 Microsoft Corporation, All Rights Reserved
//
//////////////////////////////////////////////////////////////////////

#include "precomp.h"
#include "Patch.h"

#include "ExtendString.h"
#include "ExtendQuery.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPatch::CPatch(CRequestObject *pObj, IWbemServices *pNamespace,
                                   IWbemContext *pCtx):CGenericClass(pObj, pNamespace, pCtx)
{

}

CPatch::~CPatch()
{

}

HRESULT CPatch::CreateObject(IWbemObjectSink *pHandler, ACTIONTYPE atAction)
{
    HRESULT hr = WBEM_S_NO_ERROR;
    MSIHANDLE hView = NULL, hRecord = NULL;
    int i = -1;
    WCHAR wcBuf[BUFF_SIZE];
    WCHAR wcProductCode[39];
    DWORD dwBufSize;
    bool bMatch = false;
    UINT uiStatus;
    bool bGotID = false;
    WCHAR wcTestCode[39];
    bool bGotName = false;
    WCHAR wcName[BUFF_SIZE];

    //These will change from class to class
    bool bFile, bSequence, bProduct;

    //improve getobject performance by optimizing the query
    if(atAction != ACTIONTYPE_ENUM)
	{
		// we are doing GetObject so we need to be reinitialized
		hr = WBEM_E_NOT_FOUND;

		BSTR bstrCompare;

        int iPos = -1;
        bstrCompare = SysAllocString ( L"ProductCode" );

		if ( bstrCompare )
		{
			if(FindIn(m_pRequest->m_Property, bstrCompare, &iPos))
			{
				if ( ::SysStringLen ( m_pRequest->m_Value[iPos] ) == 38 )
				{
		            //Get the product code we're looking for
					wcscpy(wcTestCode, m_pRequest->m_Value[iPos]);
					bGotID = true;
				}
				else
				{
					// we are not good to go, they have sent us longer string
					SysFreeString ( bstrCompare );
					throw hr;
				}

			}

			SysFreeString ( bstrCompare );
		}
		else
		{
			throw CHeap_Exception(CHeap_Exception::E_ALLOCATION_ERROR);
		}

        iPos = -1;
        bstrCompare = SysAllocString ( L"File" );

		if ( bstrCompare )
		{
			if(FindIn(m_pRequest->m_Property, bstrCompare, &iPos))
			{
				if ( ::SysStringLen ( m_pRequest->m_Value[iPos] ) < BUFF_SIZE )
				{
		            //Get the name we're looking for
					wcscpy(wcName, m_pRequest->m_Value[iPos]);
					bGotName = true;
				}
				else
				{
					// we are not good to go, they have sent us longer string
					SysFreeString ( bstrCompare );
					throw hr;
				}

			}

			SysFreeString ( bstrCompare );
		}
		else
		{
			throw CHeap_Exception(CHeap_Exception::E_ALLOCATION_ERROR);
		}
    }

    Query wcQuery;
    wcQuery.Append ( 1, L"select distinct `File_`, `Sequence`, `PatchSize`, `Attributes` from Patch" );

    //optimize for GetObject
    if ( bGotName )
	{
		wcQuery.Append ( 3, L" where `File_`=\'", wcName, L"\'" );
	}

	LPWSTR Buffer = NULL;
	LPWSTR dynBuffer = NULL;

	DWORD dwDynBuffer = 0L;

    while(!bMatch && m_pRequest->Package(++i) && (hr != WBEM_E_CALL_CANCELLED))
	{
		// safe operation:
		// Package ( i ) returns NULL ( tested above ) or valid WCHAR [39]

        wcscpy(wcProductCode, m_pRequest->Package(i));

        if((atAction == ACTIONTYPE_ENUM) || (bGotID && (_wcsicmp(wcTestCode, wcProductCode) == 0))){

			//Open our database
            try
			{
                if ( GetView ( &hView, wcProductCode, wcQuery, L"Patch", TRUE, FALSE ) )
				{
                    uiStatus = g_fpMsiViewFetch(hView, &hRecord);

                    while(!bMatch && (uiStatus != ERROR_NO_MORE_ITEMS) && (hr != WBEM_E_CALL_CANCELLED)){
                        CheckMSI(uiStatus);

                        if(FAILED(hr = SpawnAnInstance(&m_pObj))) throw hr;

                    //----------------------------------------------------
                        dwBufSize = BUFF_SIZE;
						GetBufferToPut ( hRecord, 1, dwBufSize, wcBuf, dwDynBuffer, dynBuffer, Buffer );

                        PutKeyProperty(m_pObj, pFile, Buffer, &bFile, m_pRequest);
                        PutProperty(m_pObj, pCaption, Buffer);
                        PutProperty(m_pObj, pDescription, Buffer);

						if ( dynBuffer && dynBuffer [ 0 ] != 0 )
						{
							dynBuffer [ 0 ] = 0;
						}

                        PutKeyProperty(m_pObj, pSequence, g_fpMsiRecordGetInteger(hRecord, 2),
                            &bSequence, m_pRequest);

                        PutKeyProperty(m_pObj, pProductCode, wcProductCode, &bProduct, m_pRequest);
                    //====================================================

                        PutProperty(m_pObj, pPatchSize, g_fpMsiRecordGetInteger(hRecord, 3));

                        PutProperty(m_pObj, pAttributes, g_fpMsiRecordGetInteger(hRecord, 4));
                    //----------------------------------------------------

                        if(bFile && bSequence && bProduct) bMatch = true;

                        if((atAction != ACTIONTYPE_GET)  || bMatch){

                            hr = pHandler->Indicate(1, &m_pObj);
                        }

                        m_pObj->Release();
                        m_pObj = NULL;

                        g_fpMsiCloseHandle(hRecord);

                        uiStatus = g_fpMsiViewFetch(hView, &hRecord);
                    }
                }
			}
			catch(...)
			{
				if ( dynBuffer )
				{
					delete [] dynBuffer;
					dynBuffer = NULL;
				}

				if (hRecord)
				g_fpMsiCloseHandle(hRecord);

				if (hView)
				{
					g_fpMsiViewClose(hView);
					g_fpMsiCloseHandle(hView);
				}

				msidata.CloseDatabase ();

				if(m_pObj)
				{
					m_pObj->Release();
					m_pObj = NULL;
				}

				throw;
			}

			if (hRecord)
			g_fpMsiCloseHandle(hRecord);

			if (hView)
			{
				g_fpMsiViewClose(hView);
				g_fpMsiCloseHandle(hView);
			}

			msidata.CloseDatabase ();
        }
    }

    if ( dynBuffer )
	{
		delete [] dynBuffer;
		dynBuffer = NULL;
	}
	
	return hr;
}