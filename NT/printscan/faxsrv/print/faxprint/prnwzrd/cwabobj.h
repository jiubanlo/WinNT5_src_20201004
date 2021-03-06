/*++

Copyright (c) 1997  Microsoft Corporation

Module Name:

    cwabobj.h

Abstract:

    Class definition for CWabObj

Environment:

        Fax send wizard

Revision History:

        10/23/97 -georgeje-
                Created it.

        mm/dd/yy -author-
                description

--*/

#ifndef __CWABOBJ__H_
#define __CWABOBJ__H_

#include "abobj.h"

class CWabObj : public CCommonAbObj{
private:    
    HINSTANCE   m_hWab;
    LPWABOPEN   m_lpWabOpen;
    LPWABOBJECT m_lpWABObject;

    BOOL        m_Initialized;

    virtual eABType GetABType() { return AB_WAB; };

    HRESULT     ABAllocateBuffer(ULONG cbSize,           
                                 LPVOID FAR * lppBuffer);

public:

    BOOL isInitialized() const  {   return m_Initialized;   }

    CWabObj(HINSTANCE hInstance);
    ~CWabObj();
    
    ULONG ABFreeBuffer(LPVOID lpBuffer) ;

} ;


#endif