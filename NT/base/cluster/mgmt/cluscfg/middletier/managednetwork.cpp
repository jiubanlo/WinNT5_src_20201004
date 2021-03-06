//////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2000-2001 Microsoft Corporation
//
//  Module Name:
//      ManagedNetwork.cpp
//
//  Description:
//      CManagedNetwork implementation.
//
//  Maintained By:
//      Galen Barbee (GalenB) 02-NOV-2000
//
//////////////////////////////////////////////////////////////////////////////

#include "Pch.h"
#include "IPAddressInfo.h"
#include "ManagedNetwork.h"

DEFINE_THISCLASS("CManagedNetwork")

#define IPADDRESS_INCREMENT 10

// ************************************************************************
//
// Constructor / Destructor
//
// ************************************************************************

//////////////////////////////////////////////////////////////////////////////
//++
//
//  HRESULT
//  CManagedNetwork::S_HrCreateInstance(
//      IUnknown ** ppunkOut
//      )
//
//--
//////////////////////////////////////////////////////////////////////////////
HRESULT
CManagedNetwork::S_HrCreateInstance(
    IUnknown ** ppunkOut
    )
{
    TraceFunc( "" );

    HRESULT             hr = S_OK;
    CManagedNetwork *   pmn = NULL;

    Assert( ppunkOut != NULL );
    if ( ppunkOut == NULL )
    {
        hr = THR( E_POINTER );
        goto Cleanup;
    }

    pmn = new CManagedNetwork;
    if ( pmn == NULL )
    {
        hr = THR( E_OUTOFMEMORY );
        goto Cleanup;
    }

    hr = THR( pmn->HrInit() );
    if ( FAILED( hr ) )
    {
        goto Cleanup;
    }

    hr = THR( pmn->TypeSafeQI( IUnknown, ppunkOut ) );
    if ( FAILED( hr ) )
    {
        goto Cleanup;
    }

Cleanup:

    if ( pmn != NULL )
    {
        pmn->Release();
    }

    HRETURN( hr );

} //*** CManagedNetwork::S_HrCreateInstance

//////////////////////////////////////////////////////////////////////////////
//++
//
//  CManagedNetwork::CManagedNetwork
//
//--
//////////////////////////////////////////////////////////////////////////////
CManagedNetwork::CManagedNetwork( void )
    : m_cRef( 1 )
{
    TraceFunc( "" );

    InterlockedIncrement( &g_cObjects );

    TraceFuncExit();

} //*** CManagedNetwork::CManagedNetwork

//////////////////////////////////////////////////////////////////////////////
//++
//
//  STDMETHODIMP
//  CManagedNetwork::HrInit
//
//--
//////////////////////////////////////////////////////////////////////////////
STDMETHODIMP
CManagedNetwork::HrInit( void )
{
    TraceFunc( "" );

    HRESULT hr = S_OK;

    // IUnknown stuff
    Assert( m_cRef == 1 );

    // Async/IClusCfgNetworkInfo
    Assert( m_bstrUID == NULL );
    Assert( m_bstrName == NULL );
    Assert( m_fHasNameChanged == FALSE );
    Assert( m_bstrDescription == NULL );
    Assert( m_fHasDescriptionChanged == FALSE );
    Assert( m_fIsPublic == FALSE );
    Assert( m_fIsPrivate == FALSE );
    Assert( m_punkPrimaryAddress == NULL );

    // IExtendObjectManager

    HRETURN( hr );

} //*** CManagedNetwork::HrInit

//////////////////////////////////////////////////////////////////////////////
//++
//
//  CManagedNetwork::~CManagedNetwork
//
//--
//////////////////////////////////////////////////////////////////////////////
CManagedNetwork::~CManagedNetwork( void )
{
    TraceFunc( "" );

    if ( m_ppunkIPs != NULL )
    {
        while ( m_cCurrentIPs != 0 )
        {
            m_cCurrentIPs --;
            Assert( m_ppunkIPs[ m_cCurrentIPs ] != NULL );
            if ( m_ppunkIPs[ m_cCurrentIPs ] != NULL )
            {
                m_ppunkIPs[ m_cCurrentIPs ]->Release();
            }
        }

        TraceFree( m_ppunkIPs );
    }

    if ( m_punkPrimaryAddress != NULL )
    {
        m_punkPrimaryAddress->Release();
    } // if:

    TraceSysFreeString( m_bstrUID );
    TraceSysFreeString( m_bstrName );
    TraceSysFreeString( m_bstrDescription );

    InterlockedDecrement( &g_cObjects );

    TraceFuncExit();

} //*** CManagedNetwork::~CManagedNetwork


// ************************************************************************
//
// IUnknown
//
// ************************************************************************


//////////////////////////////////////////////////////////////////////////////
//++
//
//  CManagedNetwork::QueryInterface
//
//  Description:
//      Query this object for the passed in interface.
//
//  Arguments:
//      riidIn
//          Id of interface requested.
//
//      ppvOut
//          Pointer to the requested interface.
//
//  Return Value:
//      S_OK
//          If the interface is available on this object.
//
//      E_NOINTERFACE
//          If the interface is not available.
//
//      E_POINTER
//          ppvOut was NULL.
//
//  Remarks:
//      None.
//
//--
//////////////////////////////////////////////////////////////////////////////
STDMETHODIMP
CManagedNetwork::QueryInterface(
    REFIID      riidIn,
    LPVOID *    ppvOut
    )
{
    TraceQIFunc( riidIn, ppvOut );

    HRESULT hr = S_OK;

    //
    // Validate arguments.
    //

    Assert( ppvOut != NULL );
    if ( ppvOut == NULL )
    {
        hr = THR( E_POINTER );
        goto Cleanup;
    }

    //
    // Handle known interfaces.
    //

    if ( IsEqualIID( riidIn, IID_IUnknown ) )
    {
        *ppvOut = static_cast< IClusCfgNetworkInfo * >( this );
    } // if: IUnknown
    else if ( IsEqualIID( riidIn, IID_IClusCfgNetworkInfo ) )
    {
        *ppvOut = TraceInterface( __THISCLASS__, IClusCfgNetworkInfo, this, 0 );
    } // else if: IClusCfgNetworkInfo
    else if ( IsEqualIID( riidIn, IID_IGatherData ) )
    {
        *ppvOut = TraceInterface( __THISCLASS__, IGatherData, this, 0 );
    } // else if: IGatherData
    else if ( IsEqualIID( riidIn, IID_IExtendObjectManager ) )
    {
        *ppvOut = TraceInterface( __THISCLASS__, IExtendObjectManager, this, 0 );
    } // else if: IExtendObjectManager
    else if ( IsEqualIID( riidIn, IID_IEnumClusCfgIPAddresses ) )
    {
        *ppvOut = TraceInterface( __THISCLASS__, IEnumClusCfgIPAddresses, this, 0 );
    } // else if: IEnumClusCfgIPAddresses
    else
    {
        *ppvOut = NULL;
        hr = E_NOINTERFACE;
    }

    //
    // Add a reference to the interface if successful.
    //

    if ( SUCCEEDED( hr ) )
    {
        ((IUnknown *) *ppvOut)->AddRef();
    } // if: success

Cleanup:

    QIRETURN_IGNORESTDMARSHALLING( hr, riidIn );

} //*** CManagedNetwork::QueryInterface

//////////////////////////////////////////////////////////////////////////////
//++
//
//  STDMETHODIMP_( ULONG )
//  CManagedNetwork::AddRef
//
//--
//////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_( ULONG )
CManagedNetwork::AddRef( void )
{
    TraceFunc( "[IUnknown]" );

    InterlockedIncrement( &m_cRef );

    CRETURN( m_cRef );

} //*** CManagedNetwork::AddRef

//////////////////////////////////////////////////////////////////////////////
//++
//
//  STDMETHODIMP_( ULONG )
//  CManagedNetwork::Release
//
//--
//////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_( ULONG )
CManagedNetwork::Release( void )
{
    TraceFunc( "[IUnknown]" );

    LONG    cRef;

    cRef = InterlockedDecrement( &m_cRef );

    if ( cRef == 0 )
    {
        TraceDo( delete this );
    }

    CRETURN( cRef );

} //*** CManagedNetwork::Release


// ************************************************************************
//
// IClusCfgNetworkInfo
//
// ************************************************************************


///////////////////////////////////////////////////////////////////////////////
//++
//
//  STDMETHODIMP
//  CManagedNetwork::GetUID(
//      BSTR * pbstrUIDOut
//      )
//
//--
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP
CManagedNetwork::GetUID(
    BSTR * pbstrUIDOut
    )
{
    TraceFunc( "[IClusCfgNetworkInfo]" );

    HRESULT hr = S_OK;

    if ( pbstrUIDOut == NULL )
    {
        hr = THR( E_POINTER );
        goto Cleanup;
    }

    if ( m_bstrUID == NULL )
    {
        hr = THR( E_POINTER );
        goto Cleanup;
    }

    *pbstrUIDOut = SysAllocString( m_bstrUID );
    if ( *pbstrUIDOut == NULL )
    {
        hr = THR( E_OUTOFMEMORY );
        goto Cleanup;
    }

Cleanup:
    HRETURN( hr );

} //*** CManagedNetwork::GetUID

///////////////////////////////////////////////////////////////////////////////
//++
//
//  STDMETHODIMP
//  CManagedNetwork::GetName(
//      BSTR * pbstrNameOut
//      )
//
//--
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP
CManagedNetwork::GetName(
    BSTR * pbstrNameOut
    )
{
    TraceFunc( "[IClusCfgNetworkInfo]" );

    HRESULT hr = S_OK;

    if ( pbstrNameOut == NULL )
    {
        hr = THR( E_POINTER );
        goto Cleanup;
    }

    if ( m_bstrName == NULL )
    {
        hr = THR( E_POINTER );
        goto Cleanup;
    }

    *pbstrNameOut = SysAllocString( m_bstrName );
    if ( *pbstrNameOut == NULL )
    {
        hr = THR( E_OUTOFMEMORY );
        goto Cleanup;
    }

Cleanup:
    HRETURN( hr );

} //*** CManagedNetwork::GetName

///////////////////////////////////////////////////////////////////////////////
//++
//
//  STDMETHODIMP
//  CManagedNetwork::SetName(
//      LPCWSTR pcszNameIn
//      )
//
//--
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP
CManagedNetwork::SetName(
    LPCWSTR pcszNameIn
    )
{
    TraceFunc1( "[IClusCfgNetworkInfo] pcszNameIn = '%ws'", ( pcszNameIn == NULL ? L"<null>" : pcszNameIn ) );

    HRESULT hr = S_OK;
    BSTR    bstrNewName;

    if ( pcszNameIn == NULL )
    {
        hr = THR( E_INVALIDARG );
        goto Cleanup;
    }

    bstrNewName = TraceSysAllocString( pcszNameIn );
    if ( bstrNewName == NULL )
    {
        hr = THR( E_OUTOFMEMORY );
        goto Cleanup;
    }

    if ( m_bstrName != NULL )
    {
        TraceSysFreeString( m_bstrName );
    }

    m_bstrName = bstrNewName;
    m_fHasNameChanged = TRUE;

Cleanup:
    HRETURN( hr );

} //*** CManagedNetwork::SetName

//////////////////////////////////////////////////////////////////////////////
//++
//
//  STDMETHODIMP
//  CManagedNetwork::GetDescription(
//      BSTR * pbstrDescriptionOut
//      )
//
//--
//////////////////////////////////////////////////////////////////////////////
STDMETHODIMP
CManagedNetwork::GetDescription(
    BSTR * pbstrDescriptionOut
    )
{
    TraceFunc( "[IClusCfgNetworkInfo]" );

    HRESULT hr;

    if ( pbstrDescriptionOut == NULL )
    {
        hr = THR( E_POINTER );
        goto Cleanup;
    }

    if ( m_bstrDescription == NULL )
    {
        hr = THR( E_POINTER );
        goto Cleanup;
    }

    *pbstrDescriptionOut = SysAllocString( m_bstrDescription );
    if ( *pbstrDescriptionOut == NULL )
    {
        hr = THR( E_OUTOFMEMORY );
        goto Cleanup;
    }

    hr = S_OK;

Cleanup:
    HRETURN( hr );

} //*** CManagedNetwork::GetDescription

//////////////////////////////////////////////////////////////////////////////
//++
//
//  STDMETHODIMP
//  CManagedNetwork::SetDescription(
//      LPCWSTR pcszDescriptionIn
//      )
//
//--
//////////////////////////////////////////////////////////////////////////////
STDMETHODIMP
CManagedNetwork::SetDescription(
    LPCWSTR pcszDescriptionIn
    )
{
    TraceFunc1( "[IClusCfgNetworkInfo] pcszNameIn = '%ws'", ( pcszDescriptionIn == NULL ? L"<null>" : pcszDescriptionIn ) );

    HRESULT hr = S_OK;
    BSTR    bstrNewDescription;

    if ( pcszDescriptionIn == NULL )
    {
        hr = THR( E_INVALIDARG );
        goto Cleanup;
    }

    bstrNewDescription = TraceSysAllocString( pcszDescriptionIn );
    if ( bstrNewDescription == NULL )
    {
        hr = THR( E_OUTOFMEMORY );
        goto Cleanup;
    }

    if ( m_bstrDescription != NULL )
    {
        TraceSysFreeString( m_bstrDescription );
    }

    m_bstrDescription = bstrNewDescription;
    m_fHasDescriptionChanged = TRUE;

Cleanup:
    HRETURN( hr );

} //*** CManagedNetwork::SetDescription

//////////////////////////////////////////////////////////////////////////////
//++
//
//  STDMETHODIMP
//  CClusCfgNetworkInfo::GetPrimaryNetworkAddress
//
//--
//////////////////////////////////////////////////////////////////////////////
STDMETHODIMP
CManagedNetwork::GetPrimaryNetworkAddress(
    IClusCfgIPAddressInfo ** ppIPAddressOut
    )
{
    TraceFunc( "[IClusCfgNetworkInfo]" );
    Assert( m_punkPrimaryAddress != NULL );

    HRESULT hr;

    if ( ppIPAddressOut == NULL )
    {
        hr = THR( E_POINTER );
    } // if:
    else
    {
        hr = THR( m_punkPrimaryAddress->TypeSafeQI( IClusCfgIPAddressInfo, ppIPAddressOut ) );
    } // else:

    HRETURN( hr );

} //*** CManagedNetwork::GetPrimaryNetworkAddress

//////////////////////////////////////////////////////////////////////////////
//++
//
//  STDMETHODIMP
//  CClusCfgNetworkInfo::SetPrimaryNetworkAddress
//
//--
//////////////////////////////////////////////////////////////////////////////
STDMETHODIMP
CManagedNetwork::SetPrimaryNetworkAddress(
    IClusCfgIPAddressInfo * pIPAddressIn
    )
{
    TraceFunc( "[IClusCfgNetworkInfo]" );

    HRESULT hr = THR( E_NOTIMPL );

    HRETURN( hr );

} //*** CManagedNetwork::SetPrimaryNetworkAddress

//////////////////////////////////////////////////////////////////////////////
//++
//
//  STDMETHODIMP
//  CManagedNetwork::IsPublic
//
//--
//////////////////////////////////////////////////////////////////////////////
STDMETHODIMP
CManagedNetwork::IsPublic( void )
{
    TraceFunc( "[IClusCfgNetworkInfo]" );

    HRESULT hr;

    if ( m_fIsPublic )
    {
        hr = S_OK;
    }
    else
    {
        hr = S_FALSE;
    }

    HRETURN( hr );

} //*** CManagedNetwork::IsPublic

//////////////////////////////////////////////////////////////////////////////
//++
//
//  STDMETHODIMP
//  CManagedNetwork::SetPublic(
//      BOOL fIsPublicIn
//      )
//
//--
//////////////////////////////////////////////////////////////////////////////
STDMETHODIMP
CManagedNetwork::SetPublic(
    BOOL fIsPublicIn
    )
{
    TraceFunc1( "[IClusCfgNetworkInfo] fIsPublic = %s", BOOLTOSTRING( fIsPublicIn ) );

    HRESULT hr = S_OK;

    m_fIsPublic = fIsPublicIn;

    HRETURN( hr );

} //*** CManagedNetwork::SetPublic

//////////////////////////////////////////////////////////////////////////////
//++
//
//  STDMETHODIMP
//  CManagedNetwork::IsPrivate
//
//--
//////////////////////////////////////////////////////////////////////////////
STDMETHODIMP
CManagedNetwork::IsPrivate( void )
{
    TraceFunc( "[IClusCfgNetworkInfo]" );

    HRESULT hr;

    if ( m_fIsPrivate )
    {
        hr = S_OK;
    }
    else
    {
        hr = S_FALSE;
    }

    HRETURN( hr );

} //*** CManagedNetwork::IsPrivate

//////////////////////////////////////////////////////////////////////////////
//++
//
//  STDMETHODIMP
//  CManagedNetwork::SetPrivate(
//      BOOL fIsPrivateIn
//      )
//
//--
//////////////////////////////////////////////////////////////////////////////
STDMETHODIMP
CManagedNetwork::SetPrivate(
    BOOL fIsPrivateIn
    )
{
    TraceFunc1( "[IClusCfgNetworkInfo] fIsPrivate = %s", BOOLTOSTRING( fIsPrivateIn ) );

    HRESULT hr = S_OK;

    m_fIsPrivate = fIsPrivateIn;

    HRETURN( hr );

} //*** CManagedNetwork::SetPrivate


//****************************************************************************
//
//  IGatherData
//
//****************************************************************************


//////////////////////////////////////////////////////////////////////////////
//++
//
//  STDMETHODIMP
//  CManagedNetwork::Gather(
//      OBJECTCOOKIE    cookieParentIn,
//      IUnknown *      punkIn
//      )
//
//--
//////////////////////////////////////////////////////////////////////////////
STDMETHODIMP
CManagedNetwork::Gather(
    OBJECTCOOKIE    cookieParentIn,
    IUnknown *      punkIn
    )
{
    TraceFunc( "[IGatherData]" );

    HRESULT hr;

    IUnknown *                  punk = NULL;
    IClusCfgNetworkInfo *       pccni = NULL;
    IEnumClusCfgIPAddresses *   peccia = NULL;
    IObjectManager *            pom = NULL;
    OBJECTCOOKIE                cookie;
    IServiceProvider *          psp = NULL;
    IGatherData *               pgd = NULL;
    IClusCfgIPAddressInfo *     piccipai = NULL;

    //
    //  Make sure we don't "gather" the same object twice.
    //

    if ( m_fGathered )
    {
        hr = THR( E_UNEXPECTED );
        goto Cleanup;   // don't cleanup the object.
    }


    //
    //  Check parameters.
    //

    if ( punkIn == NULL )
    {
        hr = THR( E_INVALIDARG );
        goto Cleanup;
    }

    //
    //  Gather the information.
    //

    hr = THR( punkIn->TypeSafeQI( IClusCfgNetworkInfo, &pccni ) );
    if ( FAILED( hr ) )
    {
        goto Cleanup;
    }

    //
    //  Gather UID
    //

    hr = THR( pccni->GetUID( &m_bstrUID ) );
    if ( FAILED( hr ) )
    {
        goto Error;
    }

    TraceMemoryAddBSTR( m_bstrUID );

    //
    //  Gather Name
    //

    hr = THR( pccni->GetName( &m_bstrName ) );
    if ( FAILED( hr ) )
    {
        goto Error;
    }

    TraceMemoryAddBSTR( m_bstrName );

    //
    //  Gather Description
    //

    hr = THR( pccni->GetDescription( &m_bstrDescription ) );
    if ( FAILED( hr ) )
    {
        goto Error;
    }

    TraceMemoryAddBSTR( m_bstrDescription );

    //
    //  Gather IsPrivate
    //

    hr = STHR( pccni->IsPrivate() );
    if ( FAILED( hr ) )
    {
        goto Error;
    }

    if ( hr == S_OK )
    {
        m_fIsPrivate = TRUE;
    }
    else
    {
        m_fIsPrivate = FALSE;
    }

    //
    //  Gather IsPublic
    //

    hr = STHR( pccni->IsPublic() );
    if ( FAILED( hr ) )
    {
        goto Error;
    }

    if ( hr == S_OK )
    {
        m_fIsPublic = TRUE;
    }
    else
    {
        m_fIsPublic = FALSE;
    }

    //
    //
    //  If the parent cookie is ZERO, then we don't grab the secondary IP
    //  address information.
    //

    if ( cookieParentIn != 0 )
    {
        //  Gather the IP Addresses
        //

        hr = THR( punkIn->TypeSafeQI( IEnumClusCfgIPAddresses, &peccia ) );
        if ( FAILED( hr ) )
        {
            goto Cleanup;
        }

        //
        //  Gather the object manager.
        //

        hr = THR( CoCreateInstance( CLSID_ServiceManager,
                                    NULL,
                                    CLSCTX_INPROC_SERVER,
                                    TypeSafeParams( IServiceProvider, &psp )
                                    ) );
        if ( FAILED( hr ) )
        {
            goto Cleanup;
        }

        hr = THR( psp->TypeSafeQS( CLSID_ObjectManager,
                                   IObjectManager,
                                   &pom
                                   ) );
        psp->Release();        // release promptly
        if ( FAILED( hr ) )
        {
            goto Cleanup;
        }

        hr = THR( pom->FindObject( CLSID_NetworkType,
                                   cookieParentIn,
                                   m_bstrUID,
                                   IID_NULL,
                                   &cookie,
                                   &punk // dummy
                                   ) );
        Assert( punk == NULL );
        if ( FAILED( hr ) )
        {
            goto Cleanup;
        }

        hr = THR( EnumChildrenAndTransferInformation( cookie, peccia ) );
        if ( FAILED( hr ) )
        {
            goto Cleanup;
        }
    }

    //
    //  Gather Primary Network Address
    //

    hr = THR( pccni->GetPrimaryNetworkAddress( &piccipai ) );
    if ( FAILED( hr ) )
    {
        goto Cleanup;
    }

    hr = THR( CIPAddressInfo::S_HrCreateInstance( &m_punkPrimaryAddress ) );
    if ( FAILED( hr ) )
    {
        goto Cleanup;
    }

    hr = THR( m_punkPrimaryAddress->TypeSafeQI( IGatherData, &pgd ) );
    if ( FAILED( hr ) )
    {
        goto Cleanup;
    }

    hr = THR( pgd->Gather( cookieParentIn, piccipai ) );
    if ( FAILED( hr ) )
    {
        goto Cleanup;
    }

    //
    //  Anything else to gather??
    //

    hr = S_OK;
    m_fGathered = TRUE;

Cleanup:
    if ( pgd != NULL )
    {
        pgd->Release();
    } // if:
    if ( piccipai != NULL )
    {
        piccipai->Release();
    } // if:
    if ( pom != NULL )
    {
        pom->Release();
    }
    if ( peccia != NULL )
    {
        peccia->Release();
    } // if:
    if ( pccni != NULL )
    {
        pccni->Release();
    }

    HRETURN( hr );

Error:
    //
    //  On error, invalidate all data.
    //
    if ( m_bstrUID != NULL )
    {
        TraceSysFreeString( m_bstrUID );
        m_bstrUID = NULL;
    }
    if ( m_bstrName != NULL )
    {
        TraceSysFreeString( m_bstrName );
        m_bstrName = NULL;
    }
    if ( m_bstrDescription != NULL )
    {
        TraceSysFreeString( m_bstrDescription );
        m_bstrDescription = NULL;
    }
    m_fIsPrivate = FALSE;
    m_fIsPublic = FALSE;
    goto Cleanup;

} //*** CManagedNetwork::Gather


// ************************************************************************
//
//  IExtendObjectManager
//
// ************************************************************************


//////////////////////////////////////////////////////////////////////////////
//++
//
// STDMETHODIMP
// CManagedNetwork::FindObject(
//        OBJECTCOOKIE  cookieIn
//      , REFCLSID      rclsidTypeIn
//      , LPCWSTR       pcszNameIn
//      , LPUNKNOWN *   punkOut
//      )
//
//--
//////////////////////////////////////////////////////////////////////////////
STDMETHODIMP
CManagedNetwork::FindObject(
      OBJECTCOOKIE  cookieIn
    , REFCLSID      rclsidTypeIn
    , LPCWSTR       pcszNameIn
    , LPUNKNOWN *   ppunkOut
    )
{
    TraceFunc( "[IExtendObjectManager]" );

    HRESULT hr = S_OK;

    //
    //  Check parameters.
    //

    //  We need a cookie.
    if ( cookieIn == NULL )
    {
        hr = THR( E_INVALIDARG );
        goto Cleanup;
    }

    //  We need to be representing a NetworkType
    if ( !IsEqualIID( rclsidTypeIn, CLSID_NetworkType ) )
    {
        hr = THR( E_INVALIDARG );
        goto Cleanup;
    }

    //  We need to have a name.
    if ( pcszNameIn == NULL )
    {
        hr = THR( E_INVALIDARG );
        goto Cleanup;
    }

    hr = THR( QueryInterface( DFGUID_NetworkResource,
                              reinterpret_cast< void ** >( ppunkOut )
                              ) );
    if ( FAILED( hr ) )
    {
        goto Cleanup;
    }

    hr = S_OK;

Cleanup:
    HRETURN( hr );

} //*** CManagedNetwork::FindObject


// ************************************************************************
//
//  Private methods.
//
// ************************************************************************


//////////////////////////////////////////////////////////////////////////////
//++
//
//  STDMETHODIMP
//  CManagedNetwork::EnumChildrenAndTransferInformation(
//      IEnumClusCfgIPAddresses * pecciaIn
//      )
//
//--
//////////////////////////////////////////////////////////////////////////////
STDMETHODIMP
CManagedNetwork::EnumChildrenAndTransferInformation(
    OBJECTCOOKIE                cookieIn,
    IEnumClusCfgIPAddresses *   pecciaIn
    )
{
    TraceFunc( "" );

    HRESULT                 hr = S_OK;
    IClusCfgIPAddressInfo * pccipai = NULL;
    ULONG                   cFetched;
    IGatherData *           pgd = NULL;
    IUnknown *              punk = NULL;
    DWORD                   cIPs = 0;

    Assert( m_ppunkIPs == NULL );
    Assert( m_cCurrentIPs == 0 );
    Assert( m_cAllocedIPs == 0 );

    hr = THR( pecciaIn->Count( &cIPs ) );
    if ( FAILED( hr ) )
    {
        goto Cleanup;
    }

    if ( cIPs > 0 )
    {
        m_ppunkIPs = (IUnknown **) TraceAlloc( HEAP_ZERO_MEMORY, cIPs * sizeof(IUnknown *) );
        if ( m_ppunkIPs == NULL )
        {
            hr = THR( E_OUTOFMEMORY );
            goto Cleanup;
        }
    }

    m_cAllocedIPs = cIPs;

    for ( m_cCurrentIPs = 0 ; m_cCurrentIPs < m_cAllocedIPs ; m_cCurrentIPs += 1 )
    {
        //
        //  Grab the next address.
        //

        hr = STHR( pecciaIn->Next( 1, &pccipai, &cFetched ) );
        if ( FAILED( hr ) )
        {
            goto Cleanup;
        }

        if ( hr == S_FALSE )
        {
            break;  // exit condition
        }

        Assert( cFetched == 1 );

        //
        //  Create a new IP Address object.
        //

        hr = THR( CIPAddressInfo::S_HrCreateInstance( &punk ) );
        if ( FAILED( hr ) )
        {
            goto Cleanup;
        }

        //
        //  Retrieve the information.
        //

        hr = THR( punk->TypeSafeQI( IGatherData, &pgd ) );
        if ( FAILED( hr ) )
        {
            goto Cleanup;
        }

        hr = THR( pgd->Gather( cookieIn, pccipai ) );
        if ( FAILED( hr ) )
        {
            goto Cleanup;
        }

        //
        //  Place it in the array.
        //

        m_ppunkIPs[ m_cCurrentIPs ] = punk;
        punk = NULL; // not released because it's now in the m_ppunkIPs array

        //
        //  Release temporary objects.
        //
        pgd->Release();
        pgd = NULL;

        pccipai->Release();
        pccipai = NULL;
    } // for:

    m_cIter = 0;

    hr = S_OK;

Cleanup:
    if ( punk != NULL )
    {
        punk->Release();
    } // if:
    if ( pgd != NULL )
    {
        pgd->Release();
    } // if:
    if ( pccipai != NULL )
    {
        pccipai->Release();
    }

    HRETURN( hr );

} //*** CManagedNetwork::EnumChildrenAndTransferInformation


//****************************************************************************
//
//  IEnumClusCfgIPAddresses
//
//****************************************************************************


//////////////////////////////////////////////////////////////////////////////
//++
//
//  STDMETHODIMP
//  CManagedNetwork::Next(
//      ULONG                       celt,
//      IClusCfgIPAddressInfo **    rgOut,
//      ULONG *                     pceltFetchedOut
//      )
//
//////////////////////////////////////////////////////////////////////////////
STDMETHODIMP
CManagedNetwork::Next(
    ULONG                       celt,
    IClusCfgIPAddressInfo **    rgOut,
    ULONG *                     pceltFetchedOut
    )
{
    TraceFunc( "[IEnumClusCfgIPAddresses]" );

    ULONG   celtFetched;

    HRESULT hr = S_OK;

    //
    //  Check parameters
    //

    if ( rgOut == NULL || celt == 0 )
    {
        hr = THR( E_POINTER );
        goto Cleanup;
    }

    //
    //  Zero the return count.
    //

    if ( pceltFetchedOut != NULL )
    {
        *pceltFetchedOut = 0;
    }

    //
    //  Clear the buffer
    //

    ZeroMemory( rgOut, celt * sizeof(rgOut[0]) );

    //
    //  Loop thru copying the interfaces.
    //

    for( celtFetched = 0
       ; celtFetched + m_cIter < m_cCurrentIPs && celtFetched < celt
       ; celtFetched ++
       )
    {
        hr = THR( m_ppunkIPs[ m_cIter + celtFetched ]->TypeSafeQI( IClusCfgIPAddressInfo, &rgOut[ celtFetched ] ) );
        if ( FAILED( hr ) )
        {
            goto CleanupList;
        }

    } // for: celtFetched

    if ( pceltFetchedOut != NULL )
    {
        *pceltFetchedOut = celtFetched;
    }

    m_cIter += celtFetched;

    if ( celtFetched != celt )
    {
        hr = S_FALSE;
    }
    else
    {
        hr = S_OK;
    }

Cleanup:
    HRETURN( hr );

CleanupList:
    for ( ; celtFetched != 0 ; )
    {
        celtFetched --;
        rgOut[ celtFetched ]->Release();
        rgOut[ celtFetched ] = NULL;
    }
    goto Cleanup;

} //*** CManagedNetwork::Next

//////////////////////////////////////////////////////////////////////////////
//++
//
//  STDMETHODIMP
//  CManagedNetwork::Skip(
//      ULONG cNumberToSkipIn
//      )
//
//--
//////////////////////////////////////////////////////////////////////////////
STDMETHODIMP
CManagedNetwork::Skip(
    ULONG cNumberToSkipIn
    )
{
    TraceFunc( "[IEnumClusCfgIPAddresses]" );

    HRESULT hr = S_OK;

    m_cIter += cNumberToSkipIn;

    if ( m_cIter >= m_cCurrentIPs )
    {
        m_cIter = m_cCurrentIPs;
        hr = S_FALSE;
    }

    HRETURN( hr );

} //*** CManagedNetwork::Skip

//////////////////////////////////////////////////////////////////////////////
//++
//
//  STDMETHODIMP
//  CManagedNetwork::Reset( void )
//
//--
//////////////////////////////////////////////////////////////////////////////
STDMETHODIMP
CManagedNetwork::Reset( void )
{
    TraceFunc( "[IEnumClusCfgIPAddresses]" );

    HRESULT hr = S_OK;

    m_cIter = 0;

    HRETURN( hr );

} //*** CManagedNetwork::Reset

//////////////////////////////////////////////////////////////////////////////
//++
//
//  STDMETHODIMP
//  CManagedNetwork::Clone(
//      IEnumClusCfgIPAddresses ** ppEnumClusCfgIPAddressesOut
//      )
//
//--
//////////////////////////////////////////////////////////////////////////////
STDMETHODIMP
CManagedNetwork::Clone(
    IEnumClusCfgIPAddresses ** ppEnumClusCfgIPAddressesOut
    )
{
    TraceFunc( "[IEnumClusCfgIPAddresses]" );

    //
    //  KB: GPease  31-JUL-2000
    //      Not going to implement this.
    //
    HRESULT hr = THR( E_NOTIMPL );

    HRETURN( hr );

} //*** CManagedNetwork::Clone


//////////////////////////////////////////////////////////////////////////////
//
//  STDMETHODIMP
//  CManagedNetwork::Count(
//      DWORD * pnCountOut
//      )
//
//////////////////////////////////////////////////////////////////////////////
STDMETHODIMP
CManagedNetwork::Count(
    DWORD * pnCountOut
    )
{
    TraceFunc( "[IEnumClusCfgIPAddresses]" );

    HRESULT hr = S_OK;

    if ( pnCountOut == NULL )
    {
        hr = THR( E_POINTER );
        goto Cleanup;
    }

    *pnCountOut = m_cCurrentIPs;

Cleanup:
    HRETURN( hr );

} //*** CManagedNetwork::Count
