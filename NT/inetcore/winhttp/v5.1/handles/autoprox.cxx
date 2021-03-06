/*++

Copyright (c) 1996  Microsoft Corporation

Module Name:

    autoprox.cxx

Abstract:

    Contains class implementation for auto-proxy DLLs which can extent WININET's
        abilities (logic) for deciding what proxies to use.

    How auto-proxy works:
        By offloading requests to a specialized Win32 Thread which picks
        up queued up message requests for Queries, Shutdown, and Initialization


    Contents:
        AUTO_PROXY_ASYNC_MSG

Author:

    Arthur L Bierer (arthurbi) 17-Dec-1996

Revision History:

    17-Dec-1996 arthurbi
        Created

--*/

#include <wininetp.h>



AUTO_PROXY_ASYNC_MSG::AUTO_PROXY_ASYNC_MSG(
    IN INTERNET_SCHEME isUrlScheme,
    IN LPSTR lpszUrl,
    IN LPSTR lpszUrlHostName,
    IN DWORD dwUrlHostNameLength
    )
{
    URL_COMPONENTSA urlComponents;

    Initalize();

    if ( lpszUrl )
    {
        _lpszUrl      = lpszUrl;
        _dwUrlLength  = lstrlen(lpszUrl);
        _tUrlProtocol = isUrlScheme;
        _pmProxyQuery = PROXY_MSG_GET_PROXY_INFO;
        _pmaAllocMode = MSG_ALLOC_STACK_ONLY;

        memset(&urlComponents, 0, sizeof(urlComponents));
        urlComponents.dwStructSize = sizeof(urlComponents);
        urlComponents.lpszHostName = lpszUrlHostName;
        urlComponents.dwHostNameLength = dwUrlHostNameLength;

        //
        // parse out the host name and port. The host name will be decoded; the
        // original URL will not be modified
        //

        if (WinHttpCrackUrlA(lpszUrl, 0, ICU_DECODE, &urlComponents))
        {
           _nUrlPort            = urlComponents.nPort;
           _lpszUrlHostName     = urlComponents.lpszHostName;
           _dwUrlHostNameLength = urlComponents.dwHostNameLength;

           if ( _tUrlProtocol == INTERNET_SCHEME_UNKNOWN )
           {
               _tUrlProtocol = urlComponents.nScheme;
           }

        }
        else
        {
            _Error = GetLastError();
        }
    }
    else
    {
        _Error = ERROR_NOT_ENOUGH_MEMORY;
    }
}

AUTO_PROXY_ASYNC_MSG::AUTO_PROXY_ASYNC_MSG(
    IN INTERNET_SCHEME isUrlScheme,
    IN LPSTR lpszUrlHostName,
    IN DWORD dwUrlHostNameLength
    )
{

    Initalize();

    _tUrlProtocol = isUrlScheme;
    _pmProxyQuery = PROXY_MSG_GET_PROXY_INFO;
    _pmaAllocMode = MSG_ALLOC_STACK_ONLY;
    _lpszUrlHostName     = lpszUrlHostName;
    _dwUrlHostNameLength = dwUrlHostNameLength;
}

AUTO_PROXY_ASYNC_MSG::AUTO_PROXY_ASYNC_MSG(
    IN INTERNET_SCHEME isUrlScheme,
    IN LPSTR lpszUrl,
    IN DWORD dwUrlLength,
    IN LPSTR lpszUrlHostName,
    IN DWORD dwUrlHostNameLength,
    IN INTERNET_PORT nUrlPort
    )
{
    Initalize();

    _tUrlProtocol        = isUrlScheme;
    _pmProxyQuery        = PROXY_MSG_GET_PROXY_INFO;
    _pmaAllocMode        = MSG_ALLOC_STACK_ONLY;
    _nUrlPort            = nUrlPort;
    _lpszUrlHostName     = lpszUrlHostName;
    _dwUrlHostNameLength = dwUrlHostNameLength;
    _lpszUrl             = lpszUrl;
    _dwUrlLength         = dwUrlLength;

}

VOID
AUTO_PROXY_ASYNC_MSG::SetProxyMsg(
    IN INTERNET_SCHEME isUrlScheme,
    IN LPSTR lpszUrl,
    IN DWORD dwUrlLength,
    IN LPSTR lpszUrlHostName,
    IN DWORD dwUrlHostNameLength,
    IN INTERNET_PORT nUrlPort
    )
{
    _tUrlProtocol        = isUrlScheme;
    _pmProxyQuery        = PROXY_MSG_GET_PROXY_INFO;
    _pmaAllocMode        = MSG_ALLOC_STACK_ONLY;
    _nUrlPort            = nUrlPort;
    _lpszUrlHostName     = lpszUrlHostName;
    _dwUrlHostNameLength = dwUrlHostNameLength;
    _lpszUrl             = lpszUrl;
    _dwUrlLength         = dwUrlLength;
}


AUTO_PROXY_ASYNC_MSG::AUTO_PROXY_ASYNC_MSG(
    IN PROXY_MESSAGE_TYPE pmProxyQuery
    )
{
    Initalize();

    _pmaAllocMode = MSG_ALLOC_HEAP_MSG_OBJ_OWNS;
    _pmProxyQuery = pmProxyQuery;
}

AUTO_PROXY_ASYNC_MSG::AUTO_PROXY_ASYNC_MSG(
    IN AUTO_PROXY_ASYNC_MSG *pStaticAutoProxy
    )
{
    Initalize();

    _tUrlProtocol          = pStaticAutoProxy->_tUrlProtocol;
    _lpszUrl               = (pStaticAutoProxy->_lpszUrl) ? NewString(pStaticAutoProxy->_lpszUrl) : NULL;
    _dwUrlLength           = pStaticAutoProxy->_dwUrlLength;
    _lpszUrlHostName       =
                (pStaticAutoProxy->_lpszUrlHostName ) ?
                NewString(pStaticAutoProxy->_lpszUrlHostName, pStaticAutoProxy->_dwUrlHostNameLength) :
                NULL;
    _dwUrlHostNameLength   = pStaticAutoProxy->_dwUrlHostNameLength;
    _nUrlPort              = pStaticAutoProxy->_nUrlPort;
    _tProxyScheme          = pStaticAutoProxy->_tProxyScheme;

    //
    // ProxyHostName is something that is generated by the request,
    //   therefore it should not be copied OR freed.
    //

    INET_ASSERT( pStaticAutoProxy->_lpszProxyHostName == NULL );
    //_lpszProxyHostName     = (pStaticAutoProxy->_lpszProxyHostName ) ? NewString(pStaticAutoProxy->_lpszProxyHostName) : NULL;


    _dwProxyHostNameLength = pStaticAutoProxy->_dwProxyHostNameLength;
    _nProxyHostPort        = pStaticAutoProxy->_nProxyHostPort;
    _pmProxyQuery          = pStaticAutoProxy->_pmProxyQuery;
    _pmaAllocMode          = MSG_ALLOC_HEAP_MSG_OBJ_OWNS;
    _pProxyState           = pStaticAutoProxy->_pProxyState;

    INET_ASSERT(_pProxyState == NULL);

    _dwQueryResult         = pStaticAutoProxy->_dwQueryResult;
    _Error                 = pStaticAutoProxy->_Error;
    _MessageFlags.Dword    = pStaticAutoProxy->_MessageFlags.Dword;
    _dwProxyVersion        = pStaticAutoProxy->_dwProxyVersion;
}

AUTO_PROXY_ASYNC_MSG::~AUTO_PROXY_ASYNC_MSG(
    VOID
    )
{
    DEBUG_ENTER((DBG_OBJECTS,
                None,
                "~AUTO_PROXY_ASYNC_MSG",
                NULL
                ));

    if ( IsAlloced() )
    {
        DEBUG_PRINT(OBJECTS,
                    INFO,
                    ("Freeing Allocated MSG ptr=%x\n",
                    this
                    ));


        if ( _lpszUrl )
        {
            //DEBUG_PRINT(OBJECTS,
            //            INFO,
            //            ("Url ptr=%x, %q\n",
            //            _lpszUrl,
            //            _lpszUrl
            //            ));

            FREE_MEMORY(_lpszUrl);
        }

        if ( _lpszUrlHostName )
        {
            FREE_MEMORY(_lpszUrlHostName);
        }


        if ( _pProxyState )
        {
            delete _pProxyState;
        }
    }
    if (_bFreeProxyHostName && (_lpszProxyHostName != NULL)) {
        FREE_MEMORY(_lpszProxyHostName);
    }

    DEBUG_LEAVE(0);
}


