//////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2000-2002 Microsoft Corporation
//
//  Module Name:
//      Exceptions.h
//
//  Description:
//      This file contains the declarations of many exception classes.
//
//  Implementation File:
//      None.
//
//  Maintained By:
//      Ozan Ozhan (OzanO) 19-JAN-2002
//      Vij Vasu   (Vvasu) 03-MAR-2000
//
//////////////////////////////////////////////////////////////////////////////


// Make sure that this file is included only once per compile path.
#pragma once


//////////////////////////////////////////////////////////////////////////////
// Include Files
//////////////////////////////////////////////////////////////////////////////

// For HRESULT, WCHAR, etc.
#include <windef.h>

// For the base class of all exceptions
#include "CException.h"

// For the CStr class
#include "CStr.h"


//////////////////////////////////////////////////////////////////////
// Macro Definitions
//////////////////////////////////////////////////////////////////////////////

//
// Shorthand for throwing different exceptions.
//

#define THROW_ASSERT( _hrErrorCode, _pszMessage ) \
    throw CAssert( _hrErrorCode, TEXT( __FILE__ ), __LINE__, TEXT( _pszMessage ) )

#define THROW_RUNTIME_ERROR( _hrErrorCode, _stringId ) \
    throw CRuntimeError( _hrErrorCode, TEXT( __FILE__ ), __LINE__, _stringId )

#define THROW_RUNTIME_ERROR_REF( _hrErrorCode, _stringId, _stringRefId ) \
    throw CRuntimeError( _hrErrorCode, TEXT( __FILE__ ), __LINE__, _stringId, _stringRefId )

#define THROW_CONFIG_ERROR( _hrErrorCode, _stringId ) \
    throw CConfigError( _hrErrorCode, TEXT( __FILE__ ), __LINE__, _stringId )

#define THROW_ABORT( _hrErrorCode, _stringId ) \
    throw CAbortException( _hrErrorCode, TEXT( __FILE__ ), __LINE__, _stringId )


//////////////////////////////////////////////////////////////////////
// External variable declarations
//////////////////////////////////////////////////////////////////////////////

// Handle to the instance of this DLL
extern HINSTANCE g_hInstance;



//////////////////////////////////////////////////////////////////////////////
//++
//
//  class CExceptionWithString
//
//  Description:
//      The class is a CException with an additional message string.
//
//--
//////////////////////////////////////////////////////////////////////////////
class CExceptionWithString : public CException
{
public:
    //////////////////////////////////////////////////////////////////////////
    // Public type definitions
    //////////////////////////////////////////////////////////////////////////
    typedef CException BaseClass;


    //////////////////////////////////////////////////////////////////////////
    // Public constructors and destructors
    //////////////////////////////////////////////////////////////////////////

    // Constructor ( string id overload )
    CExceptionWithString( 
          HRESULT       hrErrorCodeIn
        , const WCHAR * pcszFileNameIn
        , UINT          uiLineNumberIn
        , UINT          uiErrorStringIdIn
        ) throw()
        : BaseClass( hrErrorCodeIn, pcszFileNameIn, uiLineNumberIn )
        , m_fHasUserBeenNotified( false )
    {
        AssignString( uiErrorStringIdIn );
        m_strErrorRefString = NULL;
    }

    // Constructor ( character string overload )
    CExceptionWithString( 
          HRESULT       hrErrorCodeIn
        , const WCHAR * pcszFileNameIn
        , UINT          uiLineNumberIn
        , const WCHAR * pcszErrorStringIn
        ) throw()
        : BaseClass( hrErrorCodeIn, pcszFileNameIn, uiLineNumberIn )
        , m_fHasUserBeenNotified( false )
    {
        AssignString( pcszErrorStringIn );
        m_strErrorRefString = NULL;
    }

    // Constructor ( string id & ref string id overload )
    CExceptionWithString( 
          HRESULT       hrErrorCodeIn
        , const WCHAR * pcszFileNameIn
        , UINT          uiLineNumberIn
        , UINT          uiErrorStringIdIn
        , UINT          uiErrorRefStringIdIn
        ) throw()
        : BaseClass( hrErrorCodeIn, pcszFileNameIn, uiLineNumberIn )
        , m_fHasUserBeenNotified( false )
    {
        AssignString( uiErrorStringIdIn );
        AssignRefString( uiErrorRefStringIdIn );
    }

    // Constructor ( character string & ref character string overload )
    CExceptionWithString( 
          HRESULT       hrErrorCodeIn
        , const WCHAR * pcszFileNameIn
        , UINT          uiLineNumberIn
        , const WCHAR * pcszErrorStringIn
        , const WCHAR * pcszErrorRefStringIn
        ) throw()
        : BaseClass( hrErrorCodeIn, pcszFileNameIn, uiLineNumberIn )
        , m_fHasUserBeenNotified( false )
    {
        AssignString( pcszErrorStringIn );
        AssignRefString( pcszErrorRefStringIn );
    }

    // Copy constructor.
    CExceptionWithString( const CExceptionWithString & cesuSrcIn )  throw()
        : BaseClass( cesuSrcIn )
        , m_fHasUserBeenNotified( cesuSrcIn.m_fHasUserBeenNotified )
    {
        AssignString( cesuSrcIn.m_strErrorString );
        AssignRefString( cesuSrcIn.m_strErrorRefString );
    }

    // Default destructor.
    ~CExceptionWithString() throw() {}



    //////////////////////////////////////////////////////////////////////////
    // Public methods
    //////////////////////////////////////////////////////////////////////////

    // Assignment operator.
    const CExceptionWithString & 
        operator =( const CExceptionWithString & cesuSrcIn ) throw()
    {
        *( static_cast< BaseClass * >( this ) ) = cesuSrcIn;
        AssignString( cesuSrcIn.m_strErrorString );
        AssignRefString( cesuSrcIn.m_strErrorRefString );
        m_fHasUserBeenNotified = cesuSrcIn.m_fHasUserBeenNotified;
        return *this;
    }

    //
    // Accessor methods.
    //
    const CStr &
        StrGetErrorString() const throw() { return m_strErrorString; }

    void
        SetErrorString( UINT uiErrorStringIdIn ) throw()
    {
        AssignString( uiErrorStringIdIn );
    }

    void
        SetErrorString( const WCHAR * pcszSrcIn ) throw()
    {
        AssignString( pcszSrcIn );
    }

    const CStr &
        StrGetErrorRefString() const throw() { return m_strErrorRefString; }

    void
        SetErrorRefString( UINT uiErrorRefStringIdIn ) throw()
    {
        AssignRefString( uiErrorRefStringIdIn );
    }

    void
        SetErrorRefString( const WCHAR * pcszSrcIn ) throw()
    {
        AssignRefString( pcszSrcIn );
    }

    bool
        FHasUserBeenNotified() const throw() { return m_fHasUserBeenNotified; }

    void
        SetUserNotified( bool fNotifiedIn = true ) throw() { m_fHasUserBeenNotified = fNotifiedIn; }

private:
    //////////////////////////////////////////////////////////////////////////
    // Private member functions
    //////////////////////////////////////////////////////////////////////////

    // Function to set the member string ( string id overload ).
    void AssignString( UINT uiStringIdIn ) throw()
    {
        try
        {
            m_strErrorString.Empty();
            m_strErrorString.LoadString( g_hInstance, uiStringIdIn );
        }
        catch( ... )
        {
            // If an error has occurred, nothing can be done - we are most probably in a stack unwind anyway.
            THR( E_UNEXPECTED );
        } // catch all: cannot let an exception propagate out of any of the methods of this class
    }

    // Function to set the member string ( character string overload ).
    void AssignString( const WCHAR * pcszSrcIn ) throw()
    {
        try
        {
            m_strErrorString.Empty();
            m_strErrorString.Assign( pcszSrcIn );
        }
        catch( ... )
        {
            // If an error has occurred, nothing can be done - we are most probably in a stack unwind anyway.
            THR( E_UNEXPECTED );
        } // catch all: cannot let an exception propagate out of any of the methods of this class
    }


    // Function to set the member string ( CStr overload ).
    void AssignString( const CStr & rcstrSrcIn ) throw()
    {
        try
        {
            m_strErrorString.Empty();
            m_strErrorString.Assign( rcstrSrcIn );
        }
        catch( ... )
        {
            // If an error has occurred, nothing can be done - we are most probably in a stack unwind anyway.
            THR( E_UNEXPECTED );
        } // catch all: cannot let an exception propagate out of any of the methods of this class
    }

    // Function to set the member REF string ( REF string id overload ).
    void AssignRefString( UINT uiRefStringIdIn ) throw()
    {
        try
        {
            m_strErrorRefString.Empty();
            m_strErrorRefString.LoadString( g_hInstance, uiRefStringIdIn );
        }
        catch( ... )
        {
            // If an error has occurred, nothing can be done - we are most probably in a stack unwind anyway.
            THR( E_UNEXPECTED );
        } // catch all: cannot let an exception propagate out of any of the methods of this class
    }

    // Function to set the member REF string ( REF character string overload ).
    void AssignRefString( const WCHAR * pcszRefSrcIn ) throw()
    {
        try
        {
            m_strErrorRefString.Empty();
            m_strErrorRefString.Assign( pcszRefSrcIn );
        }
        catch( ... )
        {
            // If an error has occurred, nothing can be done - we are most probably in a stack unwind anyway.
            THR( E_UNEXPECTED );
        } // catch all: cannot let an exception propagate out of any of the methods of this class
    }


    // Function to set the member REF string ( CStr overload ).
    void AssignRefString( const CStr & rcstrRefSrcIn ) throw()
    {
        try
        {
            m_strErrorRefString.Empty();
            m_strErrorRefString.Assign( rcstrRefSrcIn );
        }
        catch( ... )
        {
            // If an error has occurred, nothing can be done - we are most probably in a stack unwind anyway.
            THR( E_UNEXPECTED );
        } // catch all: cannot let an exception propagate out of any of the methods of this class
    }

    //////////////////////////////////////////////////////////////////////////
    // Private data
    //////////////////////////////////////////////////////////////////////////
    // Error message string
    CStr            m_strErrorString;

    // Error reference message string
    CStr            m_strErrorRefString;

    // Indicates if the user has been notified about this exception or not.
    bool            m_fHasUserBeenNotified;

}; //*** class CExceptionWithString


//////////////////////////////////////////////////////////////////////////////
//++
//
//  class CAssert
//
//  Description:
//      This class of exceptions is used to represent programming errors or 
//      invalid assumptions.
//
//      The accompanying message is not expected to be localized.
//
//--
//////////////////////////////////////////////////////////////////////////////
class CAssert : public CExceptionWithString
{
public:
    //////////////////////////////////////////////////////////////////////////
    // Public type definitions
    //////////////////////////////////////////////////////////////////////////
    typedef CExceptionWithString BaseClass;


    //////////////////////////////////////////////////////////////////////////
    // Public constructors and destructors
    //////////////////////////////////////////////////////////////////////////

    // CAssert ( string id overload ).
    CAssert( 
          HRESULT       hrErrorCodeIn
        , const WCHAR * pcszFileNameIn
        , UINT          uiLineNumberIn
        , UINT          uiErrorStringIdIn
        ) throw()
        : BaseClass( hrErrorCodeIn, pcszFileNameIn, uiLineNumberIn, uiErrorStringIdIn )
    {
    }

    // Constructor ( character string overload )
    CAssert( 
          HRESULT       hrErrorCodeIn
        , const WCHAR * pcszFileNameIn
        , UINT          uiLineNumberIn
        , const WCHAR * pcszErrorStringIn
        ) throw()
        : BaseClass( hrErrorCodeIn, pcszFileNameIn, uiLineNumberIn, pcszErrorStringIn )
    {
    }


}; //*** class CAssert


//////////////////////////////////////////////////////////////////////////////
//++
//
//  class CRuntimeError
//
//  Description:
//      This class of exceptions is used to signal runtime errors such as memory
//      exhaustion, failure of Win32 API calls, etc.
//
//      The accompanying message may be shown to the user and should therefore
//      be localized.
//
//--
//////////////////////////////////////////////////////////////////////////////
class CRuntimeError : public CExceptionWithString
{
public:
    //////////////////////////////////////////////////////////////////////////
    // Public type definitions
    //////////////////////////////////////////////////////////////////////////
    typedef CExceptionWithString BaseClass;


    //////////////////////////////////////////////////////////////////////////
    // Public constructors and destructors
    //////////////////////////////////////////////////////////////////////////

    // Constructor ( string id overload ).
    CRuntimeError( 
          HRESULT       hrErrorCodeIn
        , const WCHAR * pcszFileNameIn
        , UINT          uiLineNumberIn
        , UINT          uiErrorStringIdIn
        ) throw()
        : BaseClass( hrErrorCodeIn, pcszFileNameIn, uiLineNumberIn, uiErrorStringIdIn )
    {
    }

    // Constructor ( character string overload )
    CRuntimeError( 
          HRESULT       hrErrorCodeIn
        , const WCHAR * pcszFileNameIn
        , UINT          uiLineNumberIn
        , const WCHAR * pcszErrorStringIn
        ) throw()
        : BaseClass( hrErrorCodeIn, pcszFileNameIn, uiLineNumberIn, pcszErrorStringIn )
    {
    }

    // Constructor ( string id & ref string id overload ).
    CRuntimeError( 
          HRESULT       hrErrorCodeIn
        , const WCHAR * pcszFileNameIn
        , UINT          uiLineNumberIn
        , UINT          uiErrorStringIdIn
        , UINT          uiErrorRefStringIdIn
        ) throw()
        : BaseClass( hrErrorCodeIn, pcszFileNameIn, uiLineNumberIn, uiErrorStringIdIn, uiErrorRefStringIdIn )
    {
    }

    // Constructor ( character string & character ref string overload )
    CRuntimeError( 
          HRESULT       hrErrorCodeIn
        , const WCHAR * pcszFileNameIn
        , UINT          uiLineNumberIn
        , const WCHAR * pcszErrorStringIn
        , const WCHAR * pcszErrorRefStringIn
        ) throw()
        : BaseClass( hrErrorCodeIn, pcszFileNameIn, uiLineNumberIn, pcszErrorStringIn, pcszErrorRefStringIn )
    {
    }

}; //*** class CRuntimeError


//////////////////////////////////////////////////////////////////////////////
//++
//
//  class CAbortException
//
//  Description:
//      This exception is thrown to indicate that the configuration operation
//      was aborted.
//
//      The accompanying message may be shown to the user and should therefore
//      be localized.
//
//--
//////////////////////////////////////////////////////////////////////////////
class CAbortException : public CExceptionWithString
{
public:
    //////////////////////////////////////////////////////////////////////////
    // Public type definitions
    //////////////////////////////////////////////////////////////////////////
    typedef CExceptionWithString BaseClass;


    //////////////////////////////////////////////////////////////////////////
    // Public constructors and destructors
    //////////////////////////////////////////////////////////////////////////

    // Constructor ( string id overload ).
    CAbortException( 
          HRESULT       hrErrorCodeIn
        , const WCHAR * pcszFileNameIn
        , UINT          uiLineNumberIn
        , UINT          uiErrorStringIdIn
        ) throw()
        : BaseClass( hrErrorCodeIn, pcszFileNameIn, uiLineNumberIn, uiErrorStringIdIn )
    {
    }

    // Constructor ( character string overload )
    CAbortException( 
          HRESULT       hrErrorCodeIn
        , const WCHAR * pcszFileNameIn
        , UINT          uiLineNumberIn
        , const WCHAR * pcszErrorStringIn
        ) throw()
        : BaseClass( hrErrorCodeIn, pcszFileNameIn, uiLineNumberIn, pcszErrorStringIn )
    {
    }

}; //*** class CAbortException


//////////////////////////////////////////////////////////////////////////////
//++
//
//  class CConfigError
//
//  Description:
//      This class of exceptions is used to signal errors related to cluster
//      configuration. For example, an object of this class is thrown if the
//      OS version of the computer cannot support the requested configuration
//      step.
//
//      The accompanying message may be shown to the user and should therefore
//      be localized.
//
//--
//////////////////////////////////////////////////////////////////////////////
class CConfigError : public CExceptionWithString
{
public:
    //////////////////////////////////////////////////////////////////////////
    // Public type definitions
    //////////////////////////////////////////////////////////////////////////
    typedef CExceptionWithString BaseClass;


    //////////////////////////////////////////////////////////////////////////
    // Public constructors and destructors
    //////////////////////////////////////////////////////////////////////////

    // Constructor ( string id overload )
    CConfigError( 
          HRESULT       hrErrorCodeIn
        , const WCHAR * pcszFileNameIn
        , UINT          uiLineNumberIn
        , UINT          uiErrorStringIdIn
        ) throw()
        : BaseClass( hrErrorCodeIn, pcszFileNameIn, uiLineNumberIn, uiErrorStringIdIn )
    {
    }

    // Constructor ( character string overload )
    CConfigError( 
          HRESULT       hrErrorCodeIn
        , const WCHAR * pcszFileNameIn
        , UINT          uiLineNumberIn
        , const WCHAR * pcszErrorStringIn
        ) throw()
        : BaseClass( hrErrorCodeIn, pcszFileNameIn, uiLineNumberIn, pcszErrorStringIn )
    {
    }

}; //*** class CConfigError
