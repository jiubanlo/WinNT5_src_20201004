/*++

Copyright (C) 1997-2001 Microsoft Corporation

Module Name:

    PERFPROV.H

Abstract:

	Defines the guids for performance monitor provider.

History:

	a-davj  04-Mar-97   Created.

--*/

#ifndef _PERFPROV_H_
#define _PERFPROV_H_

DEFINE_GUID(LIBID_PERFPROV,0xF00B4403L,0xF8F1,0x11CE,0xA5,0xB6,0x00,0xAA,0x00,0x68,0x0C,0x3F);

DEFINE_GUID(CLSID_PerfProvider,0xF00B4404L,0xF8F1,0x11CE,0xA5,0xB6,0x00,0xAA,0x00,0x68,0x0C,0x3F);

// {72967903-68EC-11d0-B729-00AA0062CBB7}
DEFINE_GUID(CLSID_PerfPropProv, 
0x72967903, 0x68ec, 0x11d0, 0xb7, 0x29, 0x0, 0xaa, 0x0, 0x62, 0xcb, 0xb7);

#ifdef __cplusplus
class PerfProvider;
#endif

#endif
