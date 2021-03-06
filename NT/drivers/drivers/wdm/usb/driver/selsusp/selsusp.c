/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    selSusp.c

Abstract:

    This module contains code for a generic client driver that can be loaded 
    for all USB devices/child interfaces.

Author:

Environment:

    kernel mode only

Notes:

    Copyright (c) 2000 Microsoft Corporation.  
    All Rights Reserved.

--*/

#include "selSusp.h"
#include "sSPnP.h"
#include "sSPwr.h"
#include "sSUsr.h"
#include "sSDevCtr.h"
#include "sSWmi.h"

//
// Globals
//

GLOBALS Globals;
ULONG   DebugLevel = 3;

NTSTATUS
DriverEntry(
    IN PDRIVER_OBJECT  DriverObject,
    IN PUNICODE_STRING UniRegistryPath
    );

VOID
SS_DriverUnload(
    IN PDRIVER_OBJECT DriverObject
    );

NTSTATUS
SS_AddDevice(
    IN PDRIVER_OBJECT DriverObject,
    IN PDEVICE_OBJECT PhysicalDeviceObject
    );

#ifdef PAGE_CODE
#ifdef ALLOC_PRAGMA
#pragma alloc_text(INIT, DriverEntry)
#pragma alloc_text(PAGE, SS_DriverUnload)
#pragma alloc_text(PAGE, SS_DispatchCreate)
#pragma alloc_text(PAGE, SS_DispatchClose)
#endif
#endif

NTSTATUS
DriverEntry(
    IN PDRIVER_OBJECT  DriverObject,
    IN PUNICODE_STRING UniRegistryPath
    )
/*++ 

Routine Description:

    Installable driver initialization entry point.
    This entry point is called directly by the I/O system.    

Arguments:
    
    DriverObject - pointer to driver object 

    RegistryPath - pointer to a unicode string representing the path to driver 
                   specific key in the registry.

Return Values:
    
--*/
{

    NTSTATUS        ntStatus;
    PUNICODE_STRING registryPath;
    
    //
    // initialization of variables
    //

    registryPath = &Globals.SSRegistryPath;

    //
    // Allocate pool to hold a null-terminated copy of the path.
    // Safe in paged pool since all registry routines execute at
    // PASSIVE_LEVEL.
    //

    registryPath->MaximumLength = UniRegistryPath->Length + sizeof(UNICODE_NULL);
    registryPath->Length        = UniRegistryPath->Length;
    registryPath->Buffer        = ExAllocatePool(PagedPool,
                                                 registryPath->MaximumLength);

    if (!registryPath->Buffer) {

        SSDbgPrint(1, ("Failed to allocate memory for registryPath\n"));
        ntStatus = STATUS_INSUFFICIENT_RESOURCES;
        goto DriverEntry_Exit;
    } 


    RtlZeroMemory (registryPath->Buffer, 
                   registryPath->MaximumLength);
    RtlMoveMemory (registryPath->Buffer, 
                   UniRegistryPath->Buffer, 
                   UniRegistryPath->Length);

    ntStatus = STATUS_SUCCESS;

    //
    // Initialize the driver object with this driver's entry points.
    //

    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = SS_DispatchDevCtrl;
    DriverObject->MajorFunction[IRP_MJ_POWER]          = SS_DispatchPower;
    DriverObject->MajorFunction[IRP_MJ_PNP]            = SS_DispatchPnP;
    DriverObject->MajorFunction[IRP_MJ_CREATE]         = SS_DispatchCreate;
    DriverObject->MajorFunction[IRP_MJ_CLOSE]          = SS_DispatchClose;
    DriverObject->MajorFunction[IRP_MJ_CLEANUP]        = SS_DispatchClean;
    DriverObject->MajorFunction[IRP_MJ_SYSTEM_CONTROL] = SS_DispatchSysCtrl;
    DriverObject->DriverUnload                         = SS_DriverUnload;
    DriverObject->DriverExtension->AddDevice           = (PDRIVER_ADD_DEVICE)
                                                         SS_AddDevice;
DriverEntry_Exit:

    return ntStatus;
}

VOID
SS_DriverUnload(
    IN PDRIVER_OBJECT DriverObject
    )
/*++

Description:

    This function will clean up all resources we allocated.

Arguments:

Return:
	
    None

--*/
{
    PUNICODE_STRING registryPath;

    SSDbgPrint(3, ("SS_DriverUnload - begins\n"));

    registryPath = &Globals.SSRegistryPath;

    if(registryPath->Buffer) {

        ExFreePool(registryPath->Buffer);
        registryPath->Buffer = NULL;
    }

    SSDbgPrint(3, ("SS_DriverUnload - ends\n"));

    return;
}

NTSTATUS
SS_AddDevice(
    IN PDRIVER_OBJECT DriverObject,
    IN PDEVICE_OBJECT PhysicalDeviceObject
    )
/*++

Description:

Arguments:

    DriverObject - Store the pointer to the object representing us.

    PhysicalDeviceObject - Pointer to the device object created by the
                           undelying bus driver.

Return:
	
    STATUS_SUCCESS - if successful STATUS_UNSUCCESSFUL - otherwise

--*/
{
    NTSTATUS          ntStatus;
    PDEVICE_OBJECT    deviceObject;
    PDEVICE_EXTENSION deviceExtension;
    POWER_STATE       state;
    KIRQL             oldIrql;

    SSDbgPrint(3, ("SS_AddDevice - begins\n"));

    deviceObject = NULL;

    ntStatus = IoCreateDevice(
                    DriverObject,                   // our driver object
                    sizeof(DEVICE_EXTENSION),       // extension size for us
                    NULL,                           // name for this device
                    FILE_DEVICE_UNKNOWN,
                    FILE_AUTOGENERATED_DEVICE_NAME, // device characteristics
                    FALSE,                          // Not exclusive
                    &deviceObject);                 // Our device object

    if(!NT_SUCCESS(ntStatus)) {
        
        SSDbgPrint(1, ("Failed to create device object\n"));
        return ntStatus;
    }

    //
    // Initialize the device extension
    //

    deviceExtension = (PDEVICE_EXTENSION) deviceObject->DeviceExtension;
    deviceExtension->FunctionalDeviceObject = deviceObject;
    deviceExtension->PhysicalDeviceObject = PhysicalDeviceObject;
    deviceObject->Flags |= DO_BUFFERED_IO;

    //
    // initialize the device state lock and set the device state
    //

    KeInitializeSpinLock(&deviceExtension->DevStateLock);
    INITIALIZE_PNP_STATE(deviceExtension);

    //
    //initialize OpenHandleCount
    //
    deviceExtension->OpenHandleCount = 0;

    //
    // Initialize the selective suspend variables
    //
    KeInitializeSpinLock(&deviceExtension->IdleReqStateLock);
    deviceExtension->IdleReqPend = 0;
    deviceExtension->PendingIdleIrp = NULL;

    //
    // Hold requests until the device is started
    //

    deviceExtension->QueueState = HoldRequests;

    //
    // Initialize the queue and the queue spin lock
    //

    InitializeListHead(&deviceExtension->NewRequestsQueue);
    KeInitializeSpinLock(&deviceExtension->QueueLock);

    //
    // Initialize the remove event to not-signaled.
    //

    KeInitializeEvent(&deviceExtension->RemoveEvent, 
                      SynchronizationEvent, 
                      FALSE);

    //
    // Initialize the stop event to signaled.
    // This event is signaled when the OutstandingIO becomes 1
    //

    KeInitializeEvent(&deviceExtension->StopEvent, 
                      SynchronizationEvent, 
                      TRUE);

    //
    // OutstandingIo count biased to 1.
    // Transition to 0 during remove device means IO is finished.
    // Transition to 1 means the device can be stopped
    //

    deviceExtension->OutStandingIO = 1;
    KeInitializeSpinLock(&deviceExtension->IOCountLock);

    //
    // Delegating to WMILIB
    //
    ntStatus = SSWmiRegistration(deviceExtension);

    if(!NT_SUCCESS(ntStatus)) {

        SSDbgPrint(1, ("SSWmiRegistration failed with %X\n", ntStatus));
        IoDeleteDevice(deviceObject);
        return ntStatus;
    }

    //
    // set the flags as underlying PDO
    //

    if(PhysicalDeviceObject->Flags & DO_POWER_PAGABLE) {

        deviceObject->Flags |= DO_POWER_PAGABLE;
    }

    //
    // set the power state of the device
    //

    deviceExtension->DevPower = PowerDeviceD0;
    deviceExtension->SysPower = PowerSystemWorking;

    state.DeviceState = PowerDeviceD0;
    PoSetPowerState(deviceObject, DevicePowerState, state);

    //
    // attach our driver to device stack
    //

    deviceExtension->TopOfStackDeviceObject = 
                IoAttachDeviceToDeviceStack(deviceObject,
                                            PhysicalDeviceObject);

    if(NULL == deviceExtension->TopOfStackDeviceObject) {

        SSWmiDeRegistration(deviceExtension);
        IoDeleteDevice(deviceObject);
        return STATUS_NO_SUCH_DEVICE;
    }
        
    //
    // Register device interfaces
    //

    ntStatus = IoRegisterDeviceInterface(deviceExtension->PhysicalDeviceObject, 
                                         &GUID_GENERIC_SELECTIVE_SUSPEND, 
                                         NULL, 
                                         &deviceExtension->InterfaceName);

    if(!NT_SUCCESS(ntStatus)) {

        SSWmiDeRegistration(deviceExtension);
        IoDetachDevice(deviceExtension->TopOfStackDeviceObject);
        IoDeleteDevice(deviceObject);
        return ntStatus;
    }

    //
    // initialize DPC
    //
    KeInitializeDpc(&deviceExtension->DeferredProcCall, 
                    DpcRoutine, 
                    deviceObject);

    //
    // initialize timer
    //
    KeInitializeTimerEx(&deviceExtension->Timer,
                        NotificationTimer);

    //
    // Clear the DO_DEVICE_INITIALIZING flag.
    //

    deviceObject->Flags &= ~DO_DEVICE_INITIALIZING;

    SSDbgPrint(3, ("SS_AddDevice - ends\n"));

    return ntStatus;
}

