#include <guiddef.h>

// Reserved GUIDS for our use
//
// BA126AD1-2166-11D1-B1D0-00805FC1270E     CLSID_ConnectionManager
// BA126AD2-2166-11D1-B1D0-00805FC1270E     CLSID_ConnectionManagerEnumConnection
// BA126AD3-2166-11D1-B1D0-00805FC1270E     CLSID_LanConnectionManager
// BA126AD4-2166-11D1-B1D0-00805FC1270E     CLSID_LanConnectionManagerEnumConnection
// BA126AD5-2166-11D1-B1D0-00805FC1270E     CLSID_WanConnectionManager
// BA126AD6-2166-11D1-B1D0-00805FC1270E     CLSID_WanConnectionManagerEnumConnection
// BA126AD7-2166-11D1-B1D0-00805FC1270E     CLSID_DialUpConnection
// BA126AD8-2166-11D1-B1D0-00805FC1270E     CLSID_NetGroupPolicies
// BA126AD9-2166-11D1-B1D0-00805FC1270E     CLSID_InboundConnection
// BA126ADA-2166-11D1-B1D0-00805FC1270E     (free) CLSID_InternetConnection
// BA126ADB-2166-11D1-B1D0-00805FC1270E     CLSID_LanConnection
// BA126ADC-2166-11D1-B1D0-00805FC1270E     (free) CLSID_VpnConnection
// BA126ADD-2166-11D1-B1D0-00805FC1270E     CLSID_InboundConnectionManager
// BA126ADE-2166-11D1-B1D0-00805FC1270E     CLSID_InboundConnectionManagerEnumConnection
// BA126ADF-2166-11D1-B1D0-00805FC1270E     CLSID_InstallQueue
// BA126AE0-2166-11D1-B1D0-00805FC1270E     CLSID_SharedAccessConnectionManager
// BA126AE1-2166-11D1-B1D0-00805FC1270E     CLSID_SharedAccessConnectionManagerEnumConnection
// BA126AE2-2166-11D1-B1D0-00805FC1270E     CLSID_SharedAccessConnection
// BA126AE3-2166-11D1-B1D0-00805FC1270E     CLSID_NetConnectionHNetUtil
// BA126AE4-2166-11D1-B1D0-00805FC1270E     CLSID_EAPOLManager
// BA126AE5-2166-11D1-B1D0-00805FC1270E     CLSID_ConnectionManager2
// ...
// BA126B1D-2166-11D1-B1D0-00805FC1270E
// BA126B1E-2166-11D1-B1D0-00805FC1270E
// BA126B1F-2166-11D1-B1D0-00805FC1270E

// The following CLSIDs are defined in uuid.lib because the public uses them.
//
EXTERN_C const CLSID CLSID_ConnectionManager;
EXTERN_C const CLSID CLSID_LanConnectionManager;
EXTERN_C const CLSID CLSID_NetConnectionHNetUtil;
EXTERN_C const CLSID CLSID_EAPOLManager;

DEFINE_GUID(CLSID_ConnectionManagerEnumConnection,          0xBA126AD2,0x2166,0x11D1,0xB1,0xD0,0x00,0x80,0x5F,0xC1,0x27,0x0E);
DEFINE_GUID(CLSID_LanConnectionManagerEnumConnection,       0xBA126AD4,0x2166,0x11D1,0xB1,0xD0,0x00,0x80,0x5F,0xC1,0x27,0x0E);
DEFINE_GUID(CLSID_WanConnectionManager,                     0xBA126AD5,0x2166,0x11D1,0xB1,0xD0,0x00,0x80,0x5F,0xC1,0x27,0x0E);
DEFINE_GUID(CLSID_WanConnectionManagerEnumConnection,       0xBA126AD6,0x2166,0x11D1,0xB1,0xD0,0x00,0x80,0x5F,0xC1,0x27,0x0E);
DEFINE_GUID(CLSID_DialupConnection,                         0xBA126AD7,0x2166,0x11D1,0xB1,0xD0,0x00,0x80,0x5F,0xC1,0x27,0x0E);
DEFINE_GUID(CLSID_NetGroupPolicies,                         0xBA126AD8,0x2166,0x11D1,0xB1,0xD0,0x00,0x80,0x5F,0xC1,0x27,0x0E);
DEFINE_GUID(CLSID_InboundConnection,                        0xBA126AD9,0x2166,0x11D1,0xB1,0xD0,0x00,0x80,0x5F,0xC1,0x27,0x0E);
DEFINE_GUID(CLSID_LanConnection,                            0xBA126ADB,0x2166,0x11D1,0xB1,0xD0,0x00,0x80,0x5F,0xC1,0x27,0x0E);
DEFINE_GUID(CLSID_InboundConnectionManager,                 0xBA126ADD,0x2166,0x11D1,0xB1,0xD0,0x00,0x80,0x5F,0xC1,0x27,0x0E);
DEFINE_GUID(CLSID_InboundConnectionManagerEnumConnection,   0xBA126ADE,0x2166,0x11D1,0xB1,0xD0,0x00,0x80,0x5F,0xC1,0x27,0x0E);
DEFINE_GUID(CLSID_SharedAccessConnectionManager,            0xBA126AE0,0x2166,0x11D1,0xB1,0xD0,0x00,0x80,0x5F,0xC1,0x27,0x0E);
DEFINE_GUID(CLSID_SharedAccessConnectionManagerEnumConnection, 0xBA126AE1,0x2166,0x11D1,0xB1,0xD0,0x00,0x80,0x5F,0xC1,0x27,0x0E);
DEFINE_GUID(CLSID_SharedAccessConnection,                   0xBA126AE2,0x2166,0x11D1,0xB1,0xD0,0x00,0x80,0x5F,0xC1,0x27,0x0E);

DEFINE_GUID(CLSID_InstallQueue,                             0xBA126ADF,0x2166,0x11D1,0xB1,0xD0,0x00,0x80,0x5F,0xC1,0x27,0x0E);

