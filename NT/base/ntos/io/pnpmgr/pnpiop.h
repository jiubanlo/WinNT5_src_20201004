/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

Module Name:

    pnpiop.h

Abstract:

    This module contains the plug-and-play macros and constants.

Author:

    Shie-Lin Tzong (shielint) 29-Jan-1995
    Andrew Thornton (andrewth) 5-Sept-1996

Environment:

    Kernel mode


Revision History:


--*/

//
// Pool tags
//

#define IOP_DNOD_TAG    'donD'
#define IOP_DNDT_TAG    'tdnD'
#define IOP_DPWR_TAG    'rwPD'

//
// The DEVICE_NODE is really just some extra stuff that we'd like to keep around
// for each physical device object.
// It is seperated from DEVOBJ_EXTENSION because these fields only apply to
// PDO.
//

typedef enum {

    DOCK_NOTDOCKDEVICE,
    DOCK_QUIESCENT,
    DOCK_ARRIVING,
    DOCK_DEPARTING,
    DOCK_EJECTIRP_COMPLETED

} PROFILE_STATUS;

typedef enum {

    PROFILE_IN_PNPEVENT,
    PROFILE_NOT_IN_PNPEVENT,
    PROFILE_PERHAPS_IN_PNPEVENT

} PROFILE_NOTIFICATION_TIME;

typedef struct _PENDING_SET_INTERFACE_STATE {
    LIST_ENTRY      List;
    UNICODE_STRING  LinkName;
} PENDING_SET_INTERFACE_STATE, *PPENDING_SET_INTERFACE_STATE;


typedef enum _UNLOCK_UNLINK_ACTION {
    UnlinkRemovedDeviceNodes,
    UnlinkAllDeviceNodesPendingClose,
    UnlinkOnlyChildDeviceNodesPendingClose
}   UNLOCK_UNLINK_ACTION, *PUNLOCK_UNLINK_ACTION;

typedef enum _PNP_DEVNODE_STATE {
    DeviceNodeUnspecified       = 0x300, // 768
    DeviceNodeUninitialized,             // 769
    DeviceNodeInitialized,               // 770
    DeviceNodeDriversAdded,              // 771
    DeviceNodeResourcesAssigned,         // 772 - Operational state for Added
    DeviceNodeStartPending,              // 773 - Operational state for Added
    DeviceNodeStartCompletion,           // 774 - Operational state for Added
    DeviceNodeStartPostWork,             // 775 - Operational state for Added
    DeviceNodeStarted,                   // 776
    DeviceNodeQueryStopped,              // 777
    DeviceNodeStopped,                   // 778
    DeviceNodeRestartCompletion,         // 779 - Operational state for Stopped
    DeviceNodeEnumeratePending,          // 780 - Operational state for Started
    DeviceNodeEnumerateCompletion,       // 781 - Operational state for Started
    DeviceNodeAwaitingQueuedDeletion,    // 782
    DeviceNodeAwaitingQueuedRemoval,     // 783
    DeviceNodeQueryRemoved,              // 784
    DeviceNodeRemovePendingCloses,       // 785
    DeviceNodeRemoved,                   // 786
    DeviceNodeDeletePendingCloses,       // 787
    DeviceNodeDeleted,                   // 788
    MaxDeviceNodeState                   // 788
}   PNP_DEVNODE_STATE, *PPNP_DEVNODE_STATE;

#define STATE_HISTORY_SIZE  20

typedef struct _DEVICE_NODE *PDEVICE_NODE;
typedef struct _DEVICE_NODE {

    //
    // Pointer to another DEVICE_NODE with the same parent as this one.
    //

    PDEVICE_NODE Sibling;

    //
    // Pointer to the first child of this DEVICE_NODE.
    //

    PDEVICE_NODE Child;

    //
    // Pointer to this DEVICE_NODE's parent.
    //

    PDEVICE_NODE Parent;

    //
    // Pointer to the last child of the device node
    //

    PDEVICE_NODE LastChild;

    //
    // Depth of DEVICE_NODE in the tree, root is 0
    //

    ULONG Level;

    //
    // Power notification order list entry for this device node
    //

    PPO_DEVICE_NOTIFY Notify;

    //
    // State
    //
    PNP_DEVNODE_STATE State;

    //
    // Previous State
    //
    PNP_DEVNODE_STATE PreviousState;

    //
    // Previous State
    //
    PNP_DEVNODE_STATE StateHistory[STATE_HISTORY_SIZE];

    ULONG StateHistoryEntry;

    //
    // Completion status
    //
    NTSTATUS CompletionStatus;

    //
    // Completion status
    //
    PIRP PendingIrp;

    //
    // General flags.
    //

    ULONG Flags;

    //
    // Flags used by user-mode for volatile state which should go away on a
    // reboot or when the device is removed.
    //

    ULONG UserFlags;

    //
    // Problem.  This is set if DNF_HAS_PROBLEM is set in Flags.  Indicates
    // which problem exists and uses the same values as the config manager
    // CM_PROB_*
    //

    ULONG Problem;

    //
    // Pointer to the physical device object that this DEVICE_NODE is associated
    // with.
    //

    PDEVICE_OBJECT PhysicalDeviceObject;

    //
    // Pointer to the list of resources assigned to the PhysicalDeviceObject.
    // This is the Resource list which is passed to driver's start routine.
    //

    PCM_RESOURCE_LIST ResourceList;

    PCM_RESOURCE_LIST ResourceListTranslated;

    //
    // InstancePath is the path of the instance node in the registry,
    // i.e. <EnumBus>\<DeviceId>\<uniqueid>
    //

    UNICODE_STRING InstancePath;

    //
    // ServiceName is the name of the driver who controls the device. (Not the
    // driver who enumerates/creates the PDO.)  This field is mainly for
    // convenience.
    //

    UNICODE_STRING ServiceName;

    //
    // DuplicatePDO - if the flags have DNF_DUPLICATE set then this fields indicates
    // the duplicate PDO which is enumerated by a bus driver.  N.B. It is possible
    // that DNF_DUPLICATE is set but this field is NULL.  In this case, it means that
    // we know the device is a duplicate of another device and we have not enumerated
    // the DuplicatePDO yet.
    //

    PDEVICE_OBJECT DuplicatePDO;

    //
    // ResourceRequirements
    //

    PIO_RESOURCE_REQUIREMENTS_LIST ResourceRequirements;

    //
    // Information queried from the LEGACY_BUS_INFORMATION irp.
    //

    INTERFACE_TYPE InterfaceType;
    ULONG BusNumber;

    //
    // Information queried from the BUS_INFORMATION irp.
    //

    INTERFACE_TYPE ChildInterfaceType;
    ULONG ChildBusNumber;
    USHORT ChildBusTypeIndex;

    //
    // Describes the current removal policy for the device node. This is
    // actually type DEVICE_REMOVAL_POLICY.
    //

    UCHAR RemovalPolicy;

    //
    // Similar to above, but doesn't reflect any registry overrides.
    //

    UCHAR HardwareRemovalPolicy;

    //
    // Linked list of entries that represent each driver that has registered
    // for notification on this devnode. Note: drivers (and user-mode) actually
    // register based on a FILE_OBJECT handle, which is translated into a PDO
    // by sending an IRP_MN_QUERY_DEVICE_RELATIONS for TargetDeviceRelation.
    //

    LIST_ENTRY TargetDeviceNotify;

    //
    // DeviceArbiterList - A list of arbiters registered for this physical device object
    // Note: The Arbiters must be dereferenced when the device node is going away.
    //

    LIST_ENTRY DeviceArbiterList;

    //
    // DeviceTranslatorList - A list of translator for this physical device object
    // NOTE: the Translator must be dereferenced when the devic node is going away.
    //

    LIST_ENTRY DeviceTranslatorList;

    //
    // NoTranslatorMask - the bit position corresponds to resource type
    //   if bit is set, there is no translator for the resource type in this devnode
    //

    USHORT NoTranslatorMask;

    //
    // QueryTranslatorMask - The bit position corresponds to resource type.
    //   if bit is set, the translator for the resource type is queried.
    //

    USHORT QueryTranslatorMask;

    //
    // NoArbiterMask - the bit position corresponds to resource type
    //   if bit is set, there is no arbiter for the resource type in this devnode
    //

    USHORT NoArbiterMask;

    //
    // QueryArbiterMask - The bit position corresponds to resource type.
    //   if bit is set, the arbiter for the resource type is queried.
    //

    USHORT QueryArbiterMask;

    //
    // The following fields are used to track  legacy resource allocation
    // LegacyDeviceNode - The real legacy device node.
    // NextResourceDeviceNode - link all the made-up device nodes which own part of
    //   the resources from LegacyDeviceNode.
    //

    union {
        PDEVICE_NODE LegacyDeviceNode;
        PDEVICE_RELATIONS PendingDeviceRelations;
    } OverUsed1;

    union {
        PDEVICE_NODE NextResourceDeviceNode;
    } OverUsed2;

    //
    // Remember the BootResources for the device
    //

    PCM_RESOURCE_LIST BootResources;

    //
    // When Capabilities have been queried for a device (twice, once before
    // start and once after start) the flags are stored here in the same format
    // as the query capabilities IRP - use IopDeviceNodeFlagsToCapabilities to
    // access.
    //
    ULONG CapabilityFlags;

    //
    // Maintain a list of current dock devices and their SerialNumbers
    //
    struct {
        PROFILE_STATUS  DockStatus;
        LIST_ENTRY      ListEntry;
        PWCHAR          SerialNumber;
    } DockInfo;

    //
    // Maintain a count to determine if either ourselves or any of
    // our children are stopping us from being disableable
    // count = myself (DNUF_NOT_DISABLEABLE) + 1 for each immediate
    // child that has DisableableDepends > 0
    //
    ULONG DisableableDepends;

    //
    // List of pended IoSetDeviceInterfaceState calls.
    // IoSetDeviceInterfaceState adds an entry to this list whenever it is
    // called and we haven't been started yet.  Once we do the start we'll
    // run down the list.
    //
    LIST_ENTRY PendedSetInterfaceState;

    //
    // List of device nodes with same interface type and different bus numbers.
    //
    LIST_ENTRY LegacyBusListEntry;

#if DBG_SCOPE
    ULONG FailureStatus;
    PCM_RESOURCE_LIST PreviousResourceList;
    PIO_RESOURCE_REQUIREMENTS_LIST PreviousResourceRequirements;
#endif

} DEVICE_NODE;


//
// A device Object is a PDO iff it has a non NULL device node (aka set by
// plug and play during a query device relations.
//
#define IS_PDO(d) \
    ((NULL != (d)->DeviceObjectExtension->DeviceNode) && \
    (!(((PDEVICE_NODE)(d)->DeviceObjectExtension->DeviceNode)->Flags & DNF_LEGACY_RESOURCE_DEVICENODE)))

#define ASSERT_PDO(d) \
    do { \
        if (    NULL == (d)->DeviceObjectExtension->DeviceNode || \
                (((PDEVICE_NODE)(d)->DeviceObjectExtension->DeviceNode)->Flags & DNF_LEGACY_RESOURCE_DEVICENODE))  { \
            KeBugCheckEx(PNP_DETECTED_FATAL_ERROR, PNP_ERR_INVALID_PDO, (ULONG_PTR)d, 0, 0); \
        } \
    } \
    while (0)

//
// DNF_MAKEUP - this devnode's device is created and owned by PnP manager
//

#define DNF_MADEUP                                  0x00000001

//
// DNF_DUPLICATE - this devnode's device is a duplicate of another enumerate PDO
//

#define DNF_DUPLICATE                               0x00000002

//
// DNF_HAL_NODE - a flag to indicate which device node is the root node created by
// the hal
//

#define DNF_HAL_NODE                                0x00000004

//
// DNF_REENUMERATE - needs to be reenumerated
//

#define DNF_REENUMERATE                             0x00000008

//
// DNF_ENUMERATED - used to track enumeration in IopEnumerateDevice()
//

#define DNF_ENUMERATED                              0x00000010

//
// Singal that we need to send driver query id irps
//

#define DNF_IDS_QUERIED                             0x00000020

//
// DNF_HAS_BOOT_CONFIG - the device has resource assigned by BIOS.  It is considered
//    pseudo-started and need to participate in rebalance.
//

#define DNF_HAS_BOOT_CONFIG                         0x00000040

//
// DNF_BOOT_CONFIG_RESERVED - Indicates the BOOT resources of the device are reserved.
//

#define DNF_BOOT_CONFIG_RESERVED                    0x00000080

//
// DNF_NO_RESOURCE_REQUIRED - this devnode's device does not require resource.
//

#define DNF_NO_RESOURCE_REQUIRED                    0x00000100

//
// DNF_RESOURCE_REQUIREMENTS_NEED_FILTERED - to distinguished the
//      DeviceNode->ResourceRequirements is a filtered list or not.
//

#define DNF_RESOURCE_REQUIREMENTS_NEED_FILTERED     0x00000200

//
// DNF_RESOURCE_REQUIREMENTS_CHANGED - Indicates the device's resource
//      requirements list has been changed.
//

#define DNF_RESOURCE_REQUIREMENTS_CHANGED           0x00000400

//
// DNF_NON_STOPPED_REBALANC - indicates the device can be restarted with new
//      resources without being stopped.
//

#define DNF_NON_STOPPED_REBALANCE                   0x00000800

//
// The device's controlling driver is a legacy driver
//

#define DNF_LEGACY_DRIVER                           0x00001000

//
// This corresponds to the user-mode CM_PROB_WILL_BE_REMOVED problem value and
// the DN_WILL_BE_REMOVED status flag.
//

#define DNF_HAS_PROBLEM                             0x00002000

//
// DNF_HAS_PRIVATE_PROBLEM - indicates this device reported PNP_DEVICE_FAILED
//  to a IRP_MN_QUERY_PNP_DEVICE_STATE without also reporting
//  PNP_DEVICE_RESOURCE_REQUIREMENTS_CHANGED.
//

#define DNF_HAS_PRIVATE_PROBLEM                     0x00004000

//
// DNF_HARDWARE_VERIFICATION is set on device nodes that have hardware
// verification (probably via WHQL applet).
//

#define DNF_HARDWARE_VERIFICATION                   0x00008000

//
// DNF_DEVICE_GONE is set when a pdo is no longer returned in a query bus
// relations.  It will then be processed as a surprise remove if started.
// This flag is used to better detect when a device is resurrected, and when
// processing surprise remove, to determine if the devnode should be removed
// from the tree.
//

#define DNF_DEVICE_GONE                             0x00010000

//
// DNF_LEGACY_RESOURCE_DEVICENODE is set for device nodes created for legacy
// resource allocation.
//

#define DNF_LEGACY_RESOURCE_DEVICENODE              0x00020000

//
// DNF_NEEDS_REBALANCE is set for device nodes that trigger rebalance.
//

#define DNF_NEEDS_REBALANCE                         0x00040000

//
// DNF_LOCKED_FOR_EJECT is set on device nodes that are being ejected or are
// related to a device being ejected.
//

#define DNF_LOCKED_FOR_EJECT                        0x00080000

//
// DNF_DRIVER_BLOCKED is set on device nodes that use one or more drivers that
// have been blocked from loading.
//

#define DNF_DRIVER_BLOCKED                          0x00100000

//
// DNF_CHILD_WITH_INVALID_ID is set on device nodes that has one or more children
// that have invalid id(s).
//

#define DNF_CHILD_WITH_INVALID_ID                   0x00200000

//
// This corresponds to the user-mode the DN_WILL_BE_REMOVED status flag.
//

#define DNUF_WILL_BE_REMOVED                        0x00000001

//
// This corresponds to the user-mode DN_NO_SHOW_IN_DM status flag.
//

#define DNUF_DONT_SHOW_IN_UI                        0x00000002

//
// This flag is set when user-mode lets us know that a reboot is required
// for this device.
//

#define DNUF_NEED_RESTART                           0x00000004

//
// This flag is set to let the user-mode know when a device can be disabled
// it is still possible for this to be TRUE, yet disable to fail, as it's
// a polled flag (see also PNP_DEVICE_NOT_DISABLEABLE)
//

#define DNUF_NOT_DISABLEABLE                        0x00000008

//
// Flags used during shutdown when the IO Verifier is trying to remove all
// PNP devices.
//
// DNUF_SHUTDOWN_QUERIED is set when we issue the QueryRemove to a devnode.
//
// DNUF_SHUTDOWN_SUBTREE_DONE is set once we've issued the QueryRemove to all
// a Devnodes descendants.
//
#define DNUF_SHUTDOWN_QUERIED                       0x00000010
#define DNUF_SHUTDOWN_SUBTREE_DONE                  0x00000020

//
// PNP Bugcheck Subcodes
//
#define PNP_ERR_DUPLICATE_PDO                   1
#define PNP_ERR_INVALID_PDO                     2
#define PNP_ERR_BOGUS_ID                        3
#define PNP_ERR_PDO_ENUMERATED_AFTER_DELETION   4
#define PNP_ERR_ACTIVE_PDO_FREED                5

#define PNP_ERR_DEVICE_MISSING_FROM_EJECT_LIST  6
#define PNP_ERR_UNEXPECTED_ADD_RELATION_ERR     7

#define MAX_INSTANCE_PATH_LENGTH    260

typedef NTSTATUS (*PENUM_CALLBACK)(
    IN PDEVICE_NODE DeviceNode,
    IN PVOID Context
    );

//
// Define callback routine for PipApplyFunctionToSubKeys &
// PipApplyFunctionToServiceInstances
//
typedef BOOLEAN (*PIOP_SUBKEY_CALLBACK_ROUTINE) (
    IN     HANDLE,
    IN     PUNICODE_STRING,
    IN OUT PVOID
    );

//
// Define context structures for Start and Add device services
//

#define NO_MORE_GROUP ((USHORT) -1)
#define SETUP_RESERVED_GROUP      0
#define BUS_DRIVER_GROUP          1

typedef struct _ADD_CONTEXT {
    ULONG DriverStartType;
} ADD_CONTEXT, *PADD_CONTEXT;

typedef struct _START_CONTEXT {
    BOOLEAN LoadDriver;
    BOOLEAN NewDevice;
    ADD_CONTEXT AddContext;
} START_CONTEXT, *PSTART_CONTEXT;

//
// Resource translation and allocation related structures
//

typedef enum _RESOURCE_HANDLER_TYPE {
    ResourceHandlerNull,
    ResourceTranslator,
    ResourceArbiter,
    ResourceLegacyDeviceDetection
} RESOURCE_HANDLER_TYPE;

#define PI_MAXIMUM_RESOURCE_TYPE_TRACKED 15

//
// Internal Arbiters tracking structures
// Note the first three fields of PI_RESOURCE_ARBITER_ENTRY and PI_RESOURCE_TRANSLATOR_ENTRY
// must be the same.
//

typedef struct _PI_RESOURCE_ARBITER_ENTRY {
    LIST_ENTRY          DeviceArbiterList;         // Link all the arbiters of a PDO.
    UCHAR               ResourceType;
    PARBITER_INTERFACE  ArbiterInterface;
    ULONG               Level;                     // Level of the owning device.
    LIST_ENTRY          ResourceList;
    LIST_ENTRY          BestResourceList;
    LIST_ENTRY          BestConfig;                // Link all the arbiters which produces the best logconf
    LIST_ENTRY          ActiveArbiterList;         // Link all the arbiters under testing
    UCHAR               State;
    BOOLEAN             ResourcesChanged;
} PI_RESOURCE_ARBITER_ENTRY, *PPI_RESOURCE_ARBITER_ENTRY;

//
// Define PI_RESOURCE_ARBITER_ENTRY state
//

#define PI_ARBITER_HAS_SOMETHING 1
#define PI_ARBITER_TEST_FAILED   2

//
// Internal Translator tracking structures
//

typedef struct _PI_RESOURCE_TRANSLATOR_ENTRY {
    LIST_ENTRY              DeviceTranslatorList;
    UCHAR                   ResourceType;
    PTRANSLATOR_INTERFACE   TranslatorInterface;
    PDEVICE_NODE            DeviceNode;
} PI_RESOURCE_TRANSLATOR_ENTRY, *PPI_RESOURCE_TRANSLATOR_ENTRY;

//
// IOP_RESOURCE_REQUEST
//

#define QUERY_RESOURCE_LIST                0
#define QUERY_RESOURCE_REQUIREMENTS        1

#define REGISTRY_ALLOC_CONFIG              1
#define REGISTRY_FORCED_CONFIG             2
#define REGISTRY_BOOT_CONFIG               4
#define REGISTRY_OVERRIDE_CONFIGVECTOR     1
#define REGISTRY_BASIC_CONFIGVECTOR        2

//
// An array of IOP_RESOURCE_REQUEST structures is used to anchor all the
// devices for which resource rerquirement is being attempted.
//

#define IOP_ASSIGN_RETRY              0x00000008    // Retry resource allocation later
#define IOP_ASSIGN_EXCLUDE            0x00000010    // internal IopAssign flag
#define IOP_ASSIGN_IGNORE             0x00000020    // ignore this request
#define IOP_ASSIGN_NO_REBALANCE       0x00000080    // no rebal if assign fails
#define IOP_ASSIGN_RESOURCES_RELEASED 0x00000100    // resources are released for rebalancing
#define IOP_ASSIGN_KEEP_CURRENT_CONFIG 0x00000200   // Indicate non-stopped rebalance.  We need to
                                                    //   preserved the current config.
#define IOP_ASSIGN_CLEAR_RESOURCE_REQUIREMENTS_CHANGE_FLAG \
                                      0x00000400

typedef struct _IOP_RESOURCE_REQUEST {
    PDEVICE_OBJECT                 PhysicalDevice;
    ULONG                          Flags;
    ARBITER_REQUEST_SOURCE         AllocationType;
    ULONG                          Priority;                   // 0 is highest priority
    ULONG                          Position;                   // used for sorting of entries with same priority
    PIO_RESOURCE_REQUIREMENTS_LIST ResourceRequirements;
    PVOID                          ReqList;                    // PREQ_LIST
    PCM_RESOURCE_LIST              ResourceAssignment;
    PCM_RESOURCE_LIST              TranslatedResourceAssignment;
    NTSTATUS                       Status;
} IOP_RESOURCE_REQUEST, *PIOP_RESOURCE_REQUEST;

//
// Misc
//

//
// Enumeration request type
//

typedef enum _DEVICE_REQUEST_TYPE {
    AddBootDevices,
    AssignResources,
    ClearDeviceProblem,
    ClearEjectProblem,
    HaltDevice,
    ReenumerateBootDevices,
    ReenumerateDeviceOnly,
    ReenumerateDeviceTree,
    ReenumerateRootDevices,
    RequeryDeviceState,
    ResetDevice,
    ResourceRequirementsChanged,
    RestartEnumeration,
    SetDeviceProblem,
    ShutdownPnpDevices,
    StartDevice,
    StartSystemDevices
} DEVICE_REQUEST_TYPE;

#define CmResourceTypeReserved  0xf0



//
// This macro returns the pointer to the beginning of the data
// area of KEY_VALUE_FULL_INFORMATION structure.
// In the macro, k is a pointer to KEY_VALUE_FULL_INFORMATION structure.
//

#define KEY_VALUE_DATA(k) ((PCHAR)(k) + (k)->DataOffset)

//
// Save failure status info.
//

#if DBG_SCOPE
#define SAVE_FAILURE_INFO(DeviceNode, Status) (DeviceNode)->FailureStatus = (Status)
#else
#define SAVE_FAILURE_INFO(DeviceNode, Status)
#endif

BOOLEAN
PipAreDriversLoaded(
    IN PDEVICE_NODE DeviceNode
    );

BOOLEAN
PipIsDevNodeDNStarted(
    IN PDEVICE_NODE DeviceNode
    );

VOID
PipClearDevNodeProblem(
    IN PDEVICE_NODE DeviceNode
    );

VOID
PipSetDevNodeProblem(
    IN PDEVICE_NODE DeviceNode,
    IN ULONG        Problem
    );

#define PipIsRequestPending(devnode)   FALSE

#define PipDoesDevNodeHaveResources(devnode)                        \
        ((devnode)->ResourceList != NULL || (devnode)->BootResources != NULL || \
        ((devnode)->Flags & DNF_HAS_BOOT_CONFIG) != 0)


#define PipDoesDevNodeHaveProblem(devnode)                          \
        ((devnode)->Flags & (DNF_HAS_PROBLEM | DNF_HAS_PRIVATE_PROBLEM))

#define PipIsDevNodeProblem(devnode, problem)                       \
        (((devnode)->Flags & DNF_HAS_PROBLEM) && (devnode)->Problem == (problem))

#define PipIsDevNodeDeleted(d)                                      \
    ((d)->State == DeviceNodeDeletePendingCloses ||(d)->State == DeviceNodeDeleted)

VOID
PipSetDevNodeState(
    IN  PDEVICE_NODE        DeviceNode,
    IN  PNP_DEVNODE_STATE   State,
    OUT PNP_DEVNODE_STATE  *OldState    OPTIONAL
    );

VOID
PipRestoreDevNodeState(
    IN PDEVICE_NODE DeviceNode
    );

BOOLEAN
PipIsProblemReadonly(
    IN  ULONG   Problem
    );

//++
//
// VOID
// IopRegistryDataToUnicodeString(
//     OUT PUNICODE_STRING u,
//     IN  PWCHAR p,
//     IN  ULONG l
//     )
//
//--
#define IopRegistryDataToUnicodeString(u, p, l)  \
    {                                            \
        ULONG len;                               \
                                                 \
        PiRegSzToString((p), (l), &len, NULL);   \
        (u)->Length = (USHORT)len;               \
        (u)->MaximumLength = (USHORT)(l);        \
        (u)->Buffer = (p);                       \
    }

//
// Size of scratch buffer used in this module.
//

#define PNP_SCRATCH_BUFFER_SIZE 512
#define PNP_LARGE_SCRATCH_BUFFER_SIZE (PNP_SCRATCH_BUFFER_SIZE * 8)

//
// Define Device Instance Flags (used by IoQueryDeviceConfiguration apis)
//

#define DEVINSTANCE_FLAG_HWPROFILE_DISABLED 0x1
#define DEVINSTANCE_FLAG_PNP_ENUMERATED 0x2

//
// Define Enumeration Control Flags (used by PipApplyFunctionToSubKeys)
//

#define FUNCTIONSUBKEY_FLAG_IGNORE_NON_CRITICAL_ERRORS  0x1
#define FUNCTIONSUBKEY_FLAG_DELETE_SUBKEYS              0x2

//
// The following definitions are used in IoOpenDeviceInstanceKey
//

#define PLUGPLAY_REGKEY_DEVICE  1
#define PLUGPLAY_REGKEY_DRIVER  2
#define PLUGPLAY_REGKEY_CURRENT_HWPROFILE 4

//
// Define device extension for devices reported with IoReportDetectedDevice.
//

typedef struct _IOPNP_DEVICE_EXTENSION {
    PWCHAR CompatibleIdList;
    ULONG CompatibleIdListSize;
} IOPNP_DEVICE_EXTENSION, *PIOPNP_DEVICE_EXTENSION;

//
// Reserve Boot Resources
//

typedef struct _IOP_RESERVED_RESOURCES_RECORD IOP_RESERVED_RESOURCES_RECORD, *PIOP_RESERVED_RESOURCES_RECORD;

struct _IOP_RESERVED_RESOURCES_RECORD {
    PIOP_RESERVED_RESOURCES_RECORD  Next;
    PDEVICE_OBJECT                  DeviceObject;
    PCM_RESOURCE_LIST               ReservedResources;
};

//
// External References
//

//
// Init data
//
extern PVOID IopPnpScratchBuffer1;
extern PCM_RESOURCE_LIST IopInitHalResources;
extern PDEVICE_NODE IopInitHalDeviceNode;
extern PIOP_RESERVED_RESOURCES_RECORD IopInitReservedResourceList;
extern LOGICAL PiCollectVetoedHandles;

//
// Regular data
//

//
// IopRootDeviceNode - the head of the PnP manager's device node tree.
//

extern PDEVICE_NODE IopRootDeviceNode;

//
// IopPnPDriverObject - the madeup driver object for pnp manager
//

extern PDRIVER_OBJECT IopPnPDriverObject;

//
// IopPnPSpinLock - spinlock for Pnp code.
//

extern KSPIN_LOCK IopPnPSpinLock;

//
// IopPnpEnumerationRequestList - a link list of device enumeration requests to worker thread.
//

extern LIST_ENTRY IopPnpEnumerationRequestList;

//
// PiEngineLock - Synchronizes the start/enum and remove engines.
//     (Note that this is a resource as certain acquisition paths are reentrant,
//      specifically those that call IopNotifyPnpWhenChainDereferenced.)
//

extern ERESOURCE PiEngineLock;

//
// IopDeviceTreeLock - performs syncronization on the whole device node tree.
//      IopAcquireEnumerationLock acquires this lock shared then optionally
//                                acquires an exclusive lock on a devnode.
//      IopAcquireDeviceTreeLock acquires this lock exclusive
//

extern ERESOURCE IopDeviceTreeLock;

//
// IopSurpriseRemoveListLock - synchronizes access to the surprise remove list.
//

extern ERESOURCE IopSurpriseRemoveListLock;

//
// PiEventQueueEmpty - Manual reset event which is set when the queue is empty
//

extern KEVENT PiEventQueueEmpty;

//
// PiEnumerationLock - to synchronize IoInvalidateDeviceRelations in boot phase.
//

extern KEVENT PiEnumerationLock;

//
// IopNumberDeviceNodes - Number of outstanding device nodes in the system
//

extern ULONG IopNumberDeviceNodes;

//
// PnPInitialized - A flag to indicate if PnP initialization is completed.
//

extern BOOLEAN PnPInitialized;

//
// PnPBootDriverInitialied
//

extern BOOLEAN PnPBootDriversInitialized;

//
// PnPBootDriverLoaded
//

extern BOOLEAN PnPBootDriversLoaded;

//
// IopBootConfigsReserved - Indicates whether we have reserved BOOT configs or not.
//

extern BOOLEAN IopBootConfigsReserved;

//
// PnpDefaultInterfaceTYpe - Use this if the interface type of resource list is unknown.
//

extern INTERFACE_TYPE PnpDefaultInterfaceType;

//
// IopPendingEjects - List of pending eject requests
//
extern LIST_ENTRY  IopPendingEjects;

//
// IopPendingSurpriseRemovals - List of pending surprise removal requests
//
extern LIST_ENTRY   IopPendingSurpriseRemovals;

extern KSEMAPHORE   PpRegistrySemaphore;

extern BOOLEAN      PpPnpShuttingDown;

BOOLEAN
PipIsDuplicatedDevices(
    IN PCM_RESOURCE_LIST Configuration1,
    IN PCM_RESOURCE_LIST Configuration2,
    IN PHAL_BUS_INFORMATION BusInfo1 OPTIONAL,
    IN PHAL_BUS_INFORMATION BusInfo2 OPTIONAL
    );

NTSTATUS
PipConcatenateUnicodeStrings(
    OUT PUNICODE_STRING Destination,
    IN  PUNICODE_STRING String1,
    IN  PUNICODE_STRING String2  OPTIONAL
    );

NTSTATUS
PipServiceInstanceToDeviceInstance(
    IN  HANDLE ServiceKeyHandle OPTIONAL,
    IN  PUNICODE_STRING ServiceKeyName OPTIONAL,
    IN  ULONG ServiceInstanceOrdinal,
    OUT PUNICODE_STRING DeviceInstanceRegistryPath OPTIONAL,
    OUT PHANDLE DeviceInstanceHandle OPTIONAL,
    IN  ACCESS_MASK DesiredAccess
    );

NTSTATUS
PipCreateMadeupNode(
    IN PUNICODE_STRING ServiceKeyName,
    OUT PHANDLE ReturnedHandle,
    OUT PUNICODE_STRING KeyName,
    OUT PULONG InstanceOrdinal,
    IN BOOLEAN ResourceOwned
    );

NTSTATUS
PipOpenServiceEnumKeys(
    IN PUNICODE_STRING ServiceKeyName,
    IN ACCESS_MASK DesiredAccess,
    OUT PHANDLE ServiceHandle OPTIONAL,
    OUT PHANDLE ServiceEnumHandle OPTIONAL,
    IN BOOLEAN CreateEnum
    );

NTSTATUS
IopOpenCurrentHwProfileDeviceInstanceKey(
    OUT PHANDLE Handle,
    IN  PUNICODE_STRING ServiceKeyName,
    IN  ULONG Instance,
    IN  ACCESS_MASK DesiredAccess,
    IN  BOOLEAN Create
    );

NTSTATUS
IopGetDeviceInstanceCsConfigFlags(
    IN PUNICODE_STRING DeviceInstance,
    OUT PULONG CsConfigFlags
    );

NTSTATUS
PipGetServiceInstanceCsConfigFlags(
    IN PUNICODE_STRING ServiceKeyName,
    IN ULONG Instance,
    OUT PULONG CsConfigFlags
    );

NTSTATUS
PipApplyFunctionToSubKeys(
    IN HANDLE BaseHandle OPTIONAL,
    IN PUNICODE_STRING KeyName,
    IN ACCESS_MASK DesiredAccess,
    IN ULONG Flags,
    IN PIOP_SUBKEY_CALLBACK_ROUTINE SubKeyCallbackRoutine,
    IN OUT PVOID Context
    );

NTSTATUS
PipRegMultiSzToUnicodeStrings(
    IN PKEY_VALUE_FULL_INFORMATION KeyValueInformation,
    IN PUNICODE_STRING *UnicodeStringList,
    OUT PULONG UnicodeStringCount
    );


NTSTATUS
PipApplyFunctionToServiceInstances(
    IN HANDLE ServiceKeyHandle OPTIONAL,
    IN PUNICODE_STRING ServiceKeyName OPTIONAL,
    IN ACCESS_MASK DesiredAccess,
    IN BOOLEAN IgnoreNonCriticalErrors,
    IN PIOP_SUBKEY_CALLBACK_ROUTINE DevInstCallbackRoutine,
    IN OUT PVOID Context,
    OUT PULONG ServiceInstanceOrdinal OPTIONAL
    );

VOID
PipFreeUnicodeStringList(
    IN PUNICODE_STRING UnicodeStringList,
    IN ULONG StringCount
    );

NTSTATUS
PipReadDeviceConfiguration(
    IN HANDLE Handle,
    IN ULONG Flags,
    OUT PCM_RESOURCE_LIST *CmResource,
    OUT PULONG Length
    );

#define PiInitializeEngineLock() \
    ExInitializeResourceLite(&PiEngineLock)

typedef enum {

    PPL_SIMPLE_READ,
    PPL_TREEOP_ALLOW_READS,
    PPL_TREEOP_BLOCK_READS,
    PPL_TREEOP_BLOCK_READS_FROM_ALLOW

} PNP_LOCK_LEVEL;

VOID
PpDevNodeLockTree(
    IN  PNP_LOCK_LEVEL  LockLevel
    );

VOID
PpDevNodeUnlockTree(
    IN  PNP_LOCK_LEVEL  LockLevel
    );

#if DBG
VOID
PpDevNodeAssertLockLevel(
    IN  PNP_LOCK_LEVEL  LockLevel,
    IN  PCSTR           File,
    IN  ULONG           Line
    );

#define PPDEVNODE_ASSERT_LOCK_HELD(Level) \
    PpDevNodeAssertLockLevel(Level, __FILE__, __LINE__)

#else
#define PPDEVNODE_ASSERT_LOCK_HELD(Level)
#endif

VOID
PpDevNodeInsertIntoTree(
    IN PDEVICE_NODE     ParentNode,
    IN PDEVICE_NODE     DeviceNode
    );

VOID
PpDevNodeRemoveFromTree(
    IN PDEVICE_NODE     DeviceNode
    );

NTSTATUS
PipAllocateDeviceNode(
    IN PDEVICE_OBJECT PhysicalDeviceObject,
    OUT PDEVICE_NODE *DeviceNode
    );

NTSTATUS
PipForAllDeviceNodes(
    IN PENUM_CALLBACK Callback,
    IN PVOID Context
    );

NTSTATUS
PipForDeviceNodeSubtree(
    IN PDEVICE_NODE     DeviceNode,
    IN PENUM_CALLBACK   Callback,
    IN PVOID            Context
    );

ULONG
IopDetermineResourceListSize(
    IN PCM_RESOURCE_LIST ResourceList
    );

PDEVICE_OBJECT
IopDeviceObjectFromDeviceInstance(
    IN PUNICODE_STRING  DeviceInstance
    );

NTSTATUS
IopMapDeviceObjectToDeviceInstance(
    IN PDEVICE_OBJECT   DeviceObject,
    IN PUNICODE_STRING  DeviceInstance
    );

NTSTATUS
IopDeviceObjectToDeviceInstance(
    IN PDEVICE_OBJECT DeviceObject,
    IN PHANDLE DeviceInstanceHandle,
    IN  ACCESS_MASK DesiredAccess
    );

BOOLEAN
IopIsDeviceInstanceEnabled(
    IN HANDLE DeviceInstanceHandle,
    IN PUNICODE_STRING DeviceInstance,
    IN BOOLEAN Disable
    );

BOOLEAN
IopProcessAssignResources(
   IN PDEVICE_NODE DeviceNode,
   IN BOOLEAN Reallocation,
   OUT PBOOLEAN RebalancePerformed
   );

NTSTATUS
IopStartDevice (
    IN PDEVICE_OBJECT TargetDevice
    );

NTSTATUS
IopEjectDevice(
    IN PDEVICE_OBJECT DeviceObject,
    PPENDING_RELATIONS_LIST_ENTRY PendingEntry
    );

VOID
IopCancelPendingEject(
    IN PPENDING_RELATIONS_LIST_ENTRY Entry
    );

NTSTATUS
IopRemoveDevice(
    IN PDEVICE_OBJECT TargetDevice,
    IN ULONG IrpMinorCode
    );

NTSTATUS
IopQueryDeviceRelations(
    IN DEVICE_RELATION_TYPE Relations,
    IN PDEVICE_OBJECT DeviceObject,
    IN BOOLEAN Synchronous,
    OUT PDEVICE_RELATIONS *DeviceRelations
    );

NTSTATUS
IopQueryDeviceState(
    IN PDEVICE_OBJECT DeviceObject,
    OUT PPNP_DEVICE_STATE DeviceState
    );

NTSTATUS
PipForAllChildDeviceNodes(
    IN PDEVICE_NODE Parent,
    IN PENUM_CALLBACK Callback,
    IN PVOID Context
    );

NTSTATUS
IopCleanupDeviceRegistryValues(
    IN PUNICODE_STRING InstancePath
    );

NTSTATUS
IopQueryDeviceResources(
    IN PDEVICE_OBJECT DeviceObject,
    IN ULONG ResourceType,
    OUT PVOID *Resource,
    OUT ULONG *Length
    );

NTSTATUS
IopGetDeviceResourcesFromRegistry (
    IN PDEVICE_OBJECT DeviceObject,
    IN ULONG ResourceType,
    IN ULONG Preference,
    OUT PVOID *Resource,
    OUT PULONG Length
    );

VOID
IopResourceRequirementsChanged(
    IN PDEVICE_OBJECT PhysicalDeviceObject,
    IN BOOLEAN StopRequired
    );

NTSTATUS
IopReleaseDeviceResources(
    IN PDEVICE_NODE DeviceNode,
    IN BOOLEAN  ReserveResources
    );

NTSTATUS
IopPnPAddDevice(
    IN PDRIVER_OBJECT DriverObject,
    IN PDEVICE_OBJECT DeviceObject
    );

NTSTATUS
IopPnPDispatch(
    IN PDEVICE_OBJECT DeviceObject,
    IN OUT PIRP Irp
    );

NTSTATUS
IopPowerDispatch(
    IN PDEVICE_OBJECT DeviceObject,
    IN OUT PIRP Irp
    );

VOID
IopNewDevice(
    IN PDEVICE_OBJECT DeviceObject
    );

NTSTATUS
IopFilterResourceRequirementsList (
    IN PIO_RESOURCE_REQUIREMENTS_LIST IoList,
    IN PCM_RESOURCE_LIST CmList,
    IN OUT PIO_RESOURCE_REQUIREMENTS_LIST *FilteredList,
    OUT PBOOLEAN ExactMatch
    );

NTSTATUS
IopMergeFilteredResourceRequirementsList (
    IN PIO_RESOURCE_REQUIREMENTS_LIST IoList1,
    IN PIO_RESOURCE_REQUIREMENTS_LIST IoList2,
    IN OUT PIO_RESOURCE_REQUIREMENTS_LIST *MergedList
    );

NTSTATUS
IopMergeCmResourceLists (
    IN PCM_RESOURCE_LIST List1,
    IN PCM_RESOURCE_LIST List2,
    IN OUT PCM_RESOURCE_LIST *MergedList
    );

PIO_RESOURCE_REQUIREMENTS_LIST
IopCmResourcesToIoResources (
    IN ULONG SlotNumber,
    IN PCM_RESOURCE_LIST CmResourceList,
    IN ULONG Priority
    );

NTSTATUS
IopReportResourceListToPnp(
    IN PDRIVER_OBJECT DriverObject OPTIONAL,
    IN PDEVICE_OBJECT DeviceObject OPTIONAL,
    IN PCM_RESOURCE_LIST ResourceList,
    IN ULONG ListSize,
    IN BOOLEAN Translated
    );

NTSTATUS
IopAllocateResources(
    IN PULONG DeviceCountP,
    IN OUT PIOP_RESOURCE_REQUEST *AssignTablePP,
    IN BOOLEAN Locked,
    IN BOOLEAN DoBootConfigs,
    OUT PBOOLEAN RebalancePerformed
    );

VOID
IopInitializeResourceMap (
    PLOADER_PARAMETER_BLOCK LoaderBlock
    );

VOID
IopReallocateResources(
    IN PDEVICE_NODE DeviceNode
    );

NTSTATUS
IopWriteResourceList(
    IN HANDLE ResourceMapKey,
    IN PUNICODE_STRING ClassName,
    IN PUNICODE_STRING DriverName,
    IN PUNICODE_STRING DeviceName,
    IN PCM_RESOURCE_LIST ResourceList,
    IN ULONG ResourceListSize
    );

VOID
IopRemoveResourceListFromPnp(
    IN PLIST_ENTRY ResourceList
    );

NTSTATUS
IopWriteAllocatedResourcesToRegistry (
    IN PDEVICE_NODE DeviceNode,
    IN PCM_RESOURCE_LIST ResourceList,
    IN ULONG Length
    );

USHORT
PpInitGetGroupOrderIndex(
    IN HANDLE ServiceHandle
    );

VOID
IopDeleteLegacyKey(
    IN PDRIVER_OBJECT DriverObject
    );

NTSTATUS
IopOpenDeviceParametersSubkey(
    OUT HANDLE *ParamKeyHandle,
    IN  HANDLE ParentKeyHandle,
    IN  PUNICODE_STRING SubKeyString,
    IN  ACCESS_MASK DesiredAccess
    );

NTSTATUS
IopOpenOrCreateDeviceRegistryKey(
    IN PDEVICE_OBJECT PhysicalDeviceObject,
    IN ULONG DevInstKeyType,
    IN ACCESS_MASK DesiredAccess,
    IN BOOLEAN Create,
    OUT PHANDLE DevInstRegKey
    );

NTSTATUS
PipRequestDeviceAction(
    IN PDEVICE_OBJECT DeviceObject              OPTIONAL,
    IN DEVICE_REQUEST_TYPE RequestType,
    IN BOOLEAN ReorderingBarrier,
    IN ULONG_PTR Argument,
    IN PKEVENT CompletionEvent                  OPTIONAL,
    IN PNTSTATUS CompletionStatus               OPTIONAL
    );

VOID
PipRequestDeviceRemoval(
    IN PDEVICE_NODE DeviceNode,
    IN BOOLEAN      TreeDeletion,
    IN ULONG        Problem
    );

BOOLEAN
PipIsBeingRemovedSafely(
    IN  PDEVICE_NODE    DeviceNode
    );

NTSTATUS
IopRestartDeviceNode(
    IN PDEVICE_NODE DeviceNode
    );

VOID
PpResetProblemDevices(
    IN  PDEVICE_NODE    DeviceNode,
    IN  ULONG           Problem
    );

NTSTATUS
IopDeleteKeyRecursive(
    IN HANDLE SubKeyHandle,
    IN PWCHAR SubKeyName
    );

NTSTATUS
IopQueryLegacyBusInformation (
    IN PDEVICE_OBJECT DeviceObject,
    OUT LPGUID InterfaceGuid           OPTIONAL,
    OUT INTERFACE_TYPE *InterfaceType  OPTIONAL,
    OUT ULONG *BusNumber               OPTIONAL
    );

NTSTATUS
IopBuildRemovalRelationList(
    IN  PDEVICE_OBJECT                  DeviceObject,
    IN  PLUGPLAY_DEVICE_DELETE_TYPE     OperationCode,
    OUT PNP_VETO_TYPE                  *VetoType,
    OUT PUNICODE_STRING                 VetoName,
    OUT PRELATION_LIST                 *RelationsList
    );

NTSTATUS
IopDeleteLockedDeviceNodes(
    IN  PDEVICE_OBJECT                  DeviceObject,
    IN  PRELATION_LIST                  RelationsList,
    IN  PLUGPLAY_DEVICE_DELETE_TYPE     OperationCode,
    IN  BOOLEAN                         ProcessIndirectDescendants,
    IN  ULONG                           Problem,
    OUT PNP_VETO_TYPE                  *VetoType                    OPTIONAL,
    OUT PUNICODE_STRING                 VetoName                    OPTIONAL
    );

VOID
IopUnlinkDeviceRemovalRelations(
    IN      PDEVICE_OBJECT          RemovedDeviceObject,
    IN OUT  PRELATION_LIST          RelationsList,
    IN      UNLOCK_UNLINK_ACTION    UnlinkAction
    );

NTSTATUS
IopInvalidateRelationsInList(
    IN  PRELATION_LIST              RelationsList,
    IN  PLUGPLAY_DEVICE_DELETE_TYPE OperationCode,
    IN  BOOLEAN                     OnlyIndirectDescendants,
    IN  BOOLEAN                     RestartDevNode
    );

BOOLEAN
IopQueuePendingEject(
    PPENDING_RELATIONS_LIST_ENTRY Entry
    );

VOID
IopProcessCompletedEject(
    IN PVOID Context
    );

VOID
IopQueuePendingSurpriseRemoval(
    IN PDEVICE_OBJECT DeviceObject,
    IN PRELATION_LIST List,
    IN ULONG Problem
    );

NTSTATUS
IopUnloadAttachedDriver(
    IN PDRIVER_OBJECT DriverObject
    );

BOOLEAN
IopIsAnyDeviceInstanceEnabled(
    IN PUNICODE_STRING ServiceKeyName,
    IN HANDLE ServiceHandle,
    IN BOOLEAN LegacyIncluded
    );

NTSTATUS
IopQueryResourceHandlerInterface(
    IN RESOURCE_HANDLER_TYPE HandlerType,
    IN PDEVICE_OBJECT DeviceObject,
    IN UCHAR ResourceType,
    IN OUT PVOID *Interface
    );

NTSTATUS
IopQueryReconfiguration(
    IN UCHAR Request,
    IN PDEVICE_OBJECT DeviceObject
    );

NTSTATUS
IopLegacyResourceAllocation (
    IN ARBITER_REQUEST_SOURCE AllocationType,
    IN PDRIVER_OBJECT DriverObject,
    IN PDEVICE_OBJECT DeviceObject,
    IN PIO_RESOURCE_REQUIREMENTS_LIST ResourceRequirements,
    IN OUT PCM_RESOURCE_LIST *AllocatedResources OPTIONAL
    );

NTSTATUS
IoReportResourceUsageInternal(
    IN ARBITER_REQUEST_SOURCE AllocationType,
    IN PUNICODE_STRING DriverClassName OPTIONAL,
    IN PDRIVER_OBJECT DriverObject,
    IN PCM_RESOURCE_LIST DriverList OPTIONAL,
    IN ULONG DriverListSize OPTIONAL,
    IN PDEVICE_OBJECT DeviceObject OPTIONAL,
    IN PCM_RESOURCE_LIST DeviceList OPTIONAL,
    IN ULONG DeviceListSize OPTIONAL,
    IN BOOLEAN OverrideConflict,
    OUT PBOOLEAN ConflictDetected
    );

NTSTATUS
IopDuplicateDetection (
    IN INTERFACE_TYPE LegacyBusType,
    IN ULONG BusNumber,
    IN ULONG SlotNumber,
    OUT PDEVICE_NODE *DeviceNode
    );

NTSTATUS
PipLoadBootFilterDriver(
    IN PUNICODE_STRING DriverName,
    IN ULONG GroupIndex,
    OUT PDRIVER_OBJECT *LoadedFilter
    );

NTSTATUS
IopQueryAndSaveDeviceNodeCapabilities (
    IN PDEVICE_NODE DeviceNode
    );

VOID
IopIncDisableableDepends(
    IN OUT PDEVICE_NODE DeviceNode
    );

VOID
IopDecDisableableDepends(
    IN OUT PDEVICE_NODE DeviceNode
    );

NTSTATUS
IopQueryDockRemovalInterface(
    IN      PDEVICE_OBJECT  DeviceObject,
    IN OUT  PDOCK_INTERFACE *DockInterface
    );

#ifndef FIELD_SIZE
#define FIELD_SIZE(type, field) (sizeof(((type *)0)->field))
#endif

#define IopDeviceNodeFlagsToCapabilities(DeviceNode) \
     ((PDEVICE_CAPABILITIES) (((PUCHAR) (&(DeviceNode)->CapabilityFlags)) - \
                              FIELD_OFFSET(DEVICE_CAPABILITIES, Version) - \
                              FIELD_SIZE(DEVICE_CAPABILITIES, Version)))

//
// BOOT allocation related declarations.
//

typedef
NTSTATUS
(*PIO_ALLOCATE_BOOT_RESOURCES_ROUTINE) (
    IN ARBITER_REQUEST_SOURCE   ArbiterRequestSource,
    IN PDEVICE_OBJECT           DeviceObject,
    IN PCM_RESOURCE_LIST        BootResources
    );

NTSTATUS
IopAllocateBootResources (
    IN ARBITER_REQUEST_SOURCE   ArbiterRequestSource,
    IN PDEVICE_OBJECT           DeviceObject,
    IN PCM_RESOURCE_LIST        BootResources
    );

NTSTATUS
IopReportBootResources (
    IN ARBITER_REQUEST_SOURCE   ArbiterRequestSource,
    IN PDEVICE_OBJECT           DeviceObject,
    IN PCM_RESOURCE_LIST        BootResources
    );

NTSTATUS
IopAllocateLegacyBootResources (
    IN INTERFACE_TYPE   InterfaceType,
    IN ULONG            BusNumber
    );

extern PIO_ALLOCATE_BOOT_RESOURCES_ROUTINE IopAllocateBootResourcesRoutine;

//
// Legacy Bus information related declarations.
//

extern LIST_ENTRY  IopLegacyBusInformationTable[];

VOID
IopInsertLegacyBusDeviceNode (
    IN PDEVICE_NODE BusDeviceNode,
    IN INTERFACE_TYPE InterfaceType,
    IN ULONG BusNumber
    );

#define IopRemoveLegacyBusDeviceNode(d) RemoveEntryList(&((PDEVICE_NODE)d)->LegacyBusListEntry)

//
// Conflict detection declarations
//

NTSTATUS
IopQueryConflictList(
    PDEVICE_OBJECT                  PhysicalDeviceObject,
    IN      PCM_RESOURCE_LIST               ResourceList,
    IN      ULONG                           ResourceListSize,
    OUT     PPLUGPLAY_CONTROL_CONFLICT_LIST ConflictList,
    IN      ULONG                           ConflictListSize,
    IN      ULONG                           Flags
    );

NTSTATUS
EisaBuildEisaDeviceNode(
    VOID
    );

//
// General utility macros
//

//
// This macros calculates the size in bytes of a constant string
//
//  ULONG
//  IopConstStringSize(
//      IN CONST PWSTR String
//      );
//

#define IopConstStringSize(String)          ( sizeof(String) - sizeof(UNICODE_NULL) )

//
// This macros calculates the number of characters of a constant string
//
//  ULONG
//  IopConstStringLength(
//      IN CONST PWSTR String
//      );
//

#define IopConstStringLength(String)        ( ( sizeof(String) - sizeof(UNICODE_NULL) ) / sizeof(WCHAR) )

//
// Kernel mode notification
//

//
// This macros maps a guid to a hash value based on the number of hash
// buckets we are using.  It does this by treating the  guid as an array of
// 4 ULONGs, suming them and MOD by the number of hash buckets we are using.
//
//  ULONG
//  IopHashGuid(
//      LPGUID Guid
//      );
//

#define IopHashGuid(_Guid) \
            ( ( ((PULONG)_Guid)[0] + ((PULONG)_Guid)[1] + ((PULONG)_Guid)[2] \
                + ((PULONG)_Guid)[3]) % NOTIFY_DEVICE_CLASS_HASH_BUCKETS)



//  This macros abstracts
//
//  VOID
//  IopAcquireNotifyLock(
//      PKGUARDED_MUTEX Lock
//      )

#define IopAcquireNotifyLock(Lock)     KeAcquireGuardedMutex(Lock);

/*
VOID
IopReleaseNotifyLock(
    PKGUARDED_MUTEX Lock
    )
*/
#define IopReleaseNotifyLock(Lock)     KeReleaseGuardedMutex(Lock);


//  BOOLEAN
//  IopCompareGuid(
//      IN LPGUID guid1,
//      IN LPGUID guid2
//      );

#define IopCompareGuid(g1, g2)  ( (g1) == (g2) \
                                    ? TRUE \
                                    : RtlCompareMemory( (g1), (g2), sizeof(GUID) ) == sizeof(GUID) \
                                    )

VOID
IopInitializePlugPlayNotification(
    VOID
    );

NTSTATUS
IopNotifySetupDeviceArrival(
        PDEVICE_OBJECT PhysicalDeviceObject,    // PDO of the device
        HANDLE EnumEntryKey,                    // Handle into the enum branch of the registry for this device
        BOOLEAN InstallDriver                   // Should setup attempt to install a driver
);

NTSTATUS
IopRequestHwProfileChangeNotification(
    IN   LPGUID                      EventGuid,
    IN   PROFILE_NOTIFICATION_TIME   NotificationTime,
    OUT  PPNP_VETO_TYPE              VetoType           OPTIONAL,
    OUT  PUNICODE_STRING             VetoName           OPTIONAL
    );

NTSTATUS
IopNotifyTargetDeviceChange(
    IN  LPCGUID                             EventGuid,
    IN  PDEVICE_OBJECT                      DeviceObject,
    IN  PTARGET_DEVICE_CUSTOM_NOTIFICATION  NotificationStructure   OPTIONAL,
    OUT PDRIVER_OBJECT                     *VetoingDriver
    );

NTSTATUS
IopGetRelatedTargetDevice(
    IN PFILE_OBJECT FileObject,
    OUT PDEVICE_NODE *DeviceNode
    );

NTSTATUS
IopNotifyDeviceClassChange(
    LPGUID EventGuid,
    LPGUID ClassGuid,
    PUNICODE_STRING SymbolicLinkName
    );

NTSTATUS
IopRegisterDeviceInterface(
    IN PUNICODE_STRING DeviceInstanceName,
    IN CONST GUID *InterfaceClassGuid,
    IN PUNICODE_STRING ReferenceString      OPTIONAL,
    IN BOOLEAN UserModeFormat,
    OUT PUNICODE_STRING SymbolicLinkName
    );

NTSTATUS
IopUnregisterDeviceInterface(
    IN PUNICODE_STRING SymbolicLinkName
    );

NTSTATUS
IopRemoveDeviceInterfaces(
    IN PUNICODE_STRING DeviceInstancePath
    );

NTSTATUS
IopDisableDeviceInterfaces(
    IN PUNICODE_STRING DeviceInstancePath
    );

NTSTATUS
IopGetDeviceInterfaces(
    IN CONST GUID *InterfaceClassGuid,
    IN PUNICODE_STRING DevicePath   OPTIONAL,
    IN ULONG Flags,
    IN BOOLEAN UserModeFormat,
    OUT PWSTR *SymbolicLinkList,
    OUT PULONG SymbolicLinkListSize OPTIONAL
    );

NTSTATUS
IopDoDeferredSetInterfaceState(
    IN PDEVICE_NODE DeviceNode
    );

NTSTATUS
IopProcessSetInterfaceState(
    IN PUNICODE_STRING SymbolicLinkName,
    IN BOOLEAN Enable,
    IN BOOLEAN DeferNotStarted
    );

NTSTATUS
IopReplaceSeperatorWithPound(
    OUT PUNICODE_STRING OutString,
    IN PUNICODE_STRING InString
    );

NTSTATUS
IopNotifyHwProfileChange(
    IN  LPGUID           EventGuid,
    OUT PPNP_VETO_TYPE   VetoType    OPTIONAL,
    OUT PUNICODE_STRING  VetoName    OPTIONAL
    );

VOID
IopUncacheInterfaceInformation(
    IN PDEVICE_OBJECT DeviceObject
    );

//
// Notify entry header - all notify entries have these
//

typedef struct _NOTIFY_ENTRY_HEADER {

    //
    // List Entry structure
    //

    LIST_ENTRY ListEntry;

    //
    // Notification event category for this notification entry.
    //

    IO_NOTIFICATION_EVENT_CATEGORY EventCategory;

    //
    // SessionId.
    //
    ULONG SessionId;

    //
    // Session space object to attach to for sending notification.
    //
    PVOID OpaqueSession;

    //
    // Callback routine passed in at registration
    //

    PDRIVER_NOTIFICATION_CALLBACK_ROUTINE CallbackRoutine;

    //
    // Context passed in at registration
    //

    PVOID Context;

    //
    // Driver object of the driver that registered for notifications.  Required
    // so we can dereference it when it unregisters
    //

    PDRIVER_OBJECT DriverObject;

    //
    // RefCount is the number of outstanding pointers to the node and avoids
    // deletion while another notification is taking place
    //

    USHORT RefCount;

    //
    // Unregistered is set if this notification has been unregistered but cannot
    // be removed from the list because other entities are using it
    //

    BOOLEAN Unregistered;

    //
    // Lock is a pointer to the fast mutex which is used to synchronise access
    // to the list this node is a member of and is required so that the correct
    // list can be locked during IoUnregisterPlugPlayNotification.  If no locking
    // is required it is NULL
    //

    PKGUARDED_MUTEX Lock;

} NOTIFY_ENTRY_HEADER, *PNOTIFY_ENTRY_HEADER;


//
// Data to store for each target device registration
//

typedef struct _TARGET_DEVICE_NOTIFY_ENTRY {

    //
    // Header entries
    //

    LIST_ENTRY ListEntry;
    IO_NOTIFICATION_EVENT_CATEGORY EventCategory;
    ULONG SessionId;
    PVOID OpaqueSession;
    PDRIVER_NOTIFICATION_CALLBACK_ROUTINE CallbackRoutine;
    PVOID Context;
    PDRIVER_OBJECT DriverObject;
    USHORT RefCount;
    BOOLEAN Unregistered;
    PKGUARDED_MUTEX Lock;

    //
    // FileObject - the file object of the target device we are interested in
    //

    PFILE_OBJECT FileObject;

    //
    // PhysicalDeviceObject -- the PDO upon which this notification is hooked.
    // We need to keep this here, so we can dereference it when the refcount
    // on this notification entry drops to zero.
    //

    PDEVICE_OBJECT PhysicalDeviceObject;

} TARGET_DEVICE_NOTIFY_ENTRY, *PTARGET_DEVICE_NOTIFY_ENTRY;

//
// Data to store for each device class registration
//

typedef struct _DEVICE_CLASS_NOTIFY_ENTRY {

    //
    // Header entries
    //

    LIST_ENTRY ListEntry;
    IO_NOTIFICATION_EVENT_CATEGORY EventCategory;
    ULONG SessionId;
    PVOID OpaqueSession;
    PDRIVER_NOTIFICATION_CALLBACK_ROUTINE CallbackRoutine;
    PVOID Context;
    PDRIVER_OBJECT DriverObject;
    USHORT RefCount;
    BOOLEAN Unregistered;
    PKGUARDED_MUTEX Lock;

    //
    // ClassGuid - the guid of the device class we are interested in
    //

    GUID ClassGuid;

} DEVICE_CLASS_NOTIFY_ENTRY, *PDEVICE_CLASS_NOTIFY_ENTRY;

//
// Data to store for registration of the Reserved (ie setupdd.sys) variety
//

typedef struct _SETUP_NOTIFY_DATA {

    //
    // Header entries
    //

    LIST_ENTRY ListEntry;
    IO_NOTIFICATION_EVENT_CATEGORY EventCategory;
    ULONG SessionId;
    PVOID OpaqueSession;
    PDRIVER_NOTIFICATION_CALLBACK_ROUTINE CallbackRoutine;
    PVOID Context;
    PDRIVER_OBJECT DriverObject;
    USHORT RefCount;
    BOOLEAN Unregistered;
    PKGUARDED_MUTEX Lock;

} SETUP_NOTIFY_DATA, *PSETUP_NOTIFY_DATA;


//
// Data to store for registration for HardwareProfileChange Events
//

typedef struct _HWPROFILE_NOTIFY_ENTRY {

    //
    // Header entries
    //

    LIST_ENTRY ListEntry;
    IO_NOTIFICATION_EVENT_CATEGORY EventCategory;
    ULONG SessionId;
    PVOID OpaqueSession;
    PDRIVER_NOTIFICATION_CALLBACK_ROUTINE CallbackRoutine;
    PVOID Context;
    PDRIVER_OBJECT DriverObject;
    USHORT RefCount;
    BOOLEAN Unregistered;
    PKGUARDED_MUTEX Lock;

} HWPROFILE_NOTIFY_ENTRY, *PHWPROFILE_NOTIFY_ENTRY;

#define PNP_NOTIFICATION_VERSION            1
#define NOTIFY_DEVICE_CLASS_HASH_BUCKETS    13

//
// IopMaxDeviceNodeLevel - Level number of the DeviceNode deepest in the tree
//
extern ULONG       IopMaxDeviceNodeLevel;
extern ULONG       IoDeviceNodeTreeSequence;

//
// Global notification data
//

extern KGUARDED_MUTEX IopDeviceClassNotifyLock;
extern LIST_ENTRY IopDeviceClassNotifyList[];
extern PSETUP_NOTIFY_DATA IopSetupNotifyData;
extern KGUARDED_MUTEX IopTargetDeviceNotifyLock;
extern LIST_ENTRY IopProfileNotifyList;
extern KGUARDED_MUTEX IopHwProfileNotifyLock;

VOID
IopProcessDeferredRegistrations(
    VOID
    );

//
// Generic buffer management
//

typedef struct _BUFFER_INFO {

    //
    // Buffer - pointer to the start of the buffer
    //

    PCHAR Buffer;

    //
    // Current - Pointer to the current position in the buffer
    //

    PCHAR Current;

    //
    // MaxSize - Maximum size of the buffer in bytes
    //

    ULONG MaxSize;

} BUFFER_INFO, *PBUFFER_INFO;

typedef struct _BUS_TYPE_GUID_LIST {

    //
    // Number of allocated guid slots in the table.
    //
    ULONG Count;

    //
    // Number of entries used so far.
    //
    KGUARDED_MUTEX Lock;

    //
    // Array of bus type guids
    //
    GUID Guid[1];

} BUS_TYPE_GUID_LIST, *PBUS_TYPE_GUID_LIST;

//
// List of queried bus type guids
//
extern PBUS_TYPE_GUID_LIST IopBusTypeGuidList;

//
// Arbiter entry points
//

NTSTATUS
IopPortInitialize(
    VOID
    );

NTSTATUS
IopMemInitialize(
    VOID
    );

NTSTATUS
IopIrqInitialize(
    VOID
    );

NTSTATUS
IopDmaInitialize(
    VOID
    );

NTSTATUS
IopBusNumberInitialize(
    VOID
    );

//
// Arbiter state
//

extern ARBITER_INSTANCE IopRootPortArbiter;
extern ARBITER_INSTANCE IopRootMemArbiter;
extern ARBITER_INSTANCE IopRootIrqArbiter;
extern ARBITER_INSTANCE IopRootDmaArbiter;
extern ARBITER_INSTANCE IopRootBusNumberArbiter;

//
// Buffer management routines.
//

NTSTATUS
IopAllocateBuffer(
    IN PBUFFER_INFO Info,
    IN ULONG Size
    );

NTSTATUS
IopResizeBuffer(
    IN PBUFFER_INFO Info,
    IN ULONG NewSize,
    IN BOOLEAN CopyContents
    );

VOID
IopFreeBuffer(
    IN PBUFFER_INFO Info
    );


//
// UnicodeString management routines.
//

NTSTATUS
IopAllocateUnicodeString(
    IN OUT PUNICODE_STRING String,
    IN USHORT Length
    );

//
// Misc.
//

BOOLEAN
PipFixupDeviceId(
    PWCHAR DeviceId,
    ULONG AllowedSeparators
    );

VOID
IopOrphanNotification (
    PDEVICE_NODE DeviceNode
    );

PVOID
PiAllocateCriticalMemory(
    IN  PLUGPLAY_DEVICE_DELETE_TYPE     DeleteType,
    IN  POOL_TYPE                       PoolType,
    IN  SIZE_T                          Size,
    IN  ULONG                           Tag
    );

//
// Warm eject externs and function prototypes
//
extern KEVENT IopWarmEjectLock;
extern PDEVICE_OBJECT IopWarmEjectPdo;

NTSTATUS
IopWarmEjectDevice(
    IN PDEVICE_OBJECT      DeviceToEject,
    IN SYSTEM_POWER_STATE  LightestSleepState
    );

NTSTATUS
IopSystemControlDispatch(
    IN      PDEVICE_OBJECT  DeviceObject,
    IN OUT  PIRP            Irp
    );

VOID
PiLockDeviceActionQueue(
    VOID
    );

VOID
PiUnlockDeviceActionQueue(
    VOID
    );

//
// This macro takes a value and an alignment and rounds the entry up
// appropriately. The alignment MUST be a power of two!
//
#define ALIGN_UP_ULONG(value, alignment) (((value)+(alignment)-1)&(~(alignment-1)))

#if DBG

#define PP_DEVNODESTATE_NAME(s) ((s >= DeviceNodeUnspecified && s <= MaxDeviceNodeState)? PpStateToNameTable[(s) - DeviceNodeUnspecified] : PpStateToNameTable[0])

extern char *PpStateToNameTable[];

#else

#define PP_DEVNODESTATE_NAME(s)

#endif
