/*++

Copyright (c) 1995 Microsoft Corporation

Module Name:

    initunlo.c

Abstract:

    This module contains the code that is very specific to initialization
    and unload operations in the irenum driver

Author:

    Brian Lieuallen, 7-13-2000

Environment:

    Kernel mode

Revision History :

--*/

#include "internal.h"




NTSTATUS
IrCommAddDevice(
    IN PDRIVER_OBJECT DriverObject,
    IN PDEVICE_OBJECT Pdo
    )

{
    NTSTATUS       Status;

    ULONG          Incoming;

    PDEVICE_OBJECT Fdo = NULL;

    PDEVICE_OBJECT LowerDevice=NULL;

    //
    // Pointer to the device extension created for this
    // device
    //
    PFDO_DEVICE_EXTENSION DeviceExtension = NULL;

    D_PNP(DbgPrint("IrComm: AddDevice\n");)


    //
    // Create the device object for this device.
    //

    Status = IoCreateDevice(
                 DriverObject,
                 sizeof(FDO_DEVICE_EXTENSION),
                 NULL,
                 FILE_DEVICE_NULL,
                 FILE_AUTOGENERATED_DEVICE_NAME,
                 FALSE,
                 &Fdo
                 );

    if (!NT_SUCCESS(Status)) {

        return Status;
    }

    DeviceExtension=Fdo->DeviceExtension;

    LowerDevice=IoAttachDeviceToDeviceStack(
        Fdo,
        Pdo
        );

    if (LowerDevice == NULL) {

        D_ERROR(DbgPrint("IRCOMM: Could not attach to PDO\n");)

        Status=STATUS_INSUFFICIENT_RESOURCES;

        goto CleanUp;
    }

    Fdo->Flags |= DO_BUFFERED_IO | DO_POWER_PAGABLE;
    Fdo->Flags &= ~DO_DEVICE_INITIALIZING;

    Fdo->StackSize=LowerDevice->StackSize+1;



    RtlZeroMemory(DeviceExtension,sizeof(*DeviceExtension));

    D_ERROR(DbgPrint("IRCOMM: Device Extension %p\n",DeviceExtension);)

    DeviceExtension->DeviceObject = Fdo;
    DeviceExtension->Pdo=Pdo;
    DeviceExtension->LowerDevice=LowerDevice;

    KeInitializeTimer(
        &DeviceExtension->Read.IntervalTimer
        );

    KeInitializeDpc(
        &DeviceExtension->Read.IntervalTimerDpc,
        IntervalTimeProc,
        DeviceExtension
        );

    KeInitializeTimer(
        &DeviceExtension->Read.TotalTimer
        );

    KeInitializeDpc(
        &DeviceExtension->Read.TotalTimerDpc,
        TotalTimerProc,
        DeviceExtension
        );


    KeInitializeSpinLock(
        &DeviceExtension->SpinLock
        );



    KeInitializeSpinLock(
        &DeviceExtension->Read.ReadLock
        );


    KeInitializeSpinLock(
        &DeviceExtension->Mask.Lock
        );



    IrCommHandleSymbolicLink(
        Pdo,
        &DeviceExtension->InterfaceName,
        TRUE
        );


    //
    //  see if this is going to be instance of the driver is going to accept
    //  incoming connection instead outgoing connections.
    //
    //  default of outgoing if the key can't be read
    //
    DeviceExtension->OutgoingConnection=TRUE;

    Status=GetRegistryKeyValue(
        Pdo,
        PLUGPLAY_REGKEY_DRIVER,
        L"ListenForIncommingConnections",
        REG_DWORD,
        &Incoming,
        sizeof(Incoming)
        );

    if (NT_SUCCESS(Status)) {

        DeviceExtension->OutgoingConnection= (Incoming == 0) ? TRUE : FALSE;
    }


    //
    //  open the file handles to the irda stack here in the system process context.
    //  The handles will only be used to close the object during remove.
    //
    DeviceExtension->TdiObjects=OpenTdiObjects(
        "IrDA:IrCOMM",
        DeviceExtension->OutgoingConnection
        );

    if (DeviceExtension->TdiObjects == NULL) {

        D_ERROR(DbgPrint("IRCOMM: Could not open tdi objects\n");)

        Status=STATUS_INSUFFICIENT_RESOURCES;
        goto CleanUp;
    }


    InitializePacketQueue(
        &DeviceExtension->Write.Queue,
        DeviceExtension,
        WriteStartRoutine
        );

    InitializePacketQueue(
        &DeviceExtension->Read.Queue,
        DeviceExtension,
        ReadStartRoutine
        );

    InitializePacketQueue(
        &DeviceExtension->Mask.Queue,
        DeviceExtension,
        MaskStartRoutine
        );

    InitializePacketQueue(
        &DeviceExtension->Uart.Queue,
        DeviceExtension,
        UartStartRoutine
        );

#if 0
    DeviceExtension->LineControl.StopBits=1;
    DeviceExtension->LineControl.Parity=0;
    DeviceExtension->LineControl.WordLength=8;

    DeviceExtension->BaudRate=115200;
#endif
    DeviceExtension->Read.BytesInBuffer=0;
    DeviceExtension->Read.NextFilledByte=&DeviceExtension->Read.InputBuffer[0];
    DeviceExtension->Read.NextEmptyByte=&DeviceExtension->Read.InputBuffer[0];

    return STATUS_SUCCESS;

CleanUp:

     if (DeviceExtension->InterfaceName.Buffer != NULL) {

         IrCommHandleSymbolicLink(
             DeviceExtension->Pdo,
             &DeviceExtension->InterfaceName,
             FALSE
             );
     }


    if (DeviceExtension->LowerDevice != NULL) {

        IoDetachDevice(DeviceExtension->LowerDevice);
    }

    IoDeleteDevice(Fdo);

    return Status;

}

NTSTATUS
IrCommPnP(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )

{

    PFDO_DEVICE_EXTENSION   DeviceExtension = DeviceObject->DeviceExtension;
    PIO_STACK_LOCATION      irpSp = IoGetCurrentIrpStackLocation(Irp);
    NTSTATUS                status;
    ULONG                   i;



    switch (irpSp->MinorFunction) {

        case IRP_MN_START_DEVICE:

            D_PNP(DbgPrint("IRCOMM: IRP_MN_START_DEVICE\n");)

            Irp->IoStatus.Status = STATUS_SUCCESS;

            return ForwardIrp(DeviceExtension->LowerDevice, Irp);


        case IRP_MN_QUERY_STOP_DEVICE:

            D_PNP(DbgPrint("IRCOMM: IRP_MN_QUERY_STOP_DEVICE\n");)

            Irp->IoStatus.Status = STATUS_SUCCESS;

            return ForwardIrp(DeviceExtension->LowerDevice, Irp);


        case IRP_MN_CANCEL_STOP_DEVICE:

            D_PNP(DbgPrint("IRCOMM: IRP_MN_CANCEL_STOP_DEVICE\n");)

            Irp->IoStatus.Status = STATUS_SUCCESS;

            return ForwardIrp(DeviceExtension->LowerDevice, Irp);


        case IRP_MN_STOP_DEVICE:

            D_PNP(DbgPrint("IRCOMM: IRP_MN_STOP_DEVICE\n");)

            Irp->IoStatus.Status = STATUS_SUCCESS;

            return ForwardIrp(DeviceExtension->LowerDevice, Irp);


        case IRP_MN_QUERY_REMOVE_DEVICE:

            D_PNP(DbgPrint("IrComm: IRP_MN_QUERY_REMOVE_DEVICE\n");)

            Irp->IoStatus.Status = STATUS_SUCCESS;

            return ForwardIrp(DeviceExtension->LowerDevice, Irp);


        case IRP_MN_CANCEL_REMOVE_DEVICE:

            D_PNP(DbgPrint("IrComm: IRP_MN_CANCEL_REMOVE_DEVICE\n");)

            Irp->IoStatus.Status = STATUS_SUCCESS;

            return ForwardIrp(DeviceExtension->LowerDevice, Irp);

        case IRP_MN_SURPRISE_REMOVAL: {

            D_PNP(DbgPrint("IrComm: IRP_MN_SURPRISE_REMOVAL\n");)

            DeviceExtension->Removing=TRUE;

            //
            //  now that new io is blocked, flush out all pended irps
            //
            CleanupIoRequests(DeviceExtension);

            Irp->IoStatus.Status = STATUS_SUCCESS;

            return ForwardIrp(DeviceExtension->LowerDevice, Irp);

        }
        break;


        case IRP_MN_REMOVE_DEVICE: {

            ULONG    NewReferenceCount;

            D_PNP(DbgPrint("IrComm: IRP_MN_REMOVE_DEVICE\n");)
            //
            //  removing now for sure
            //
            DeviceExtension->Removing=TRUE;
            DeviceExtension->Removed=TRUE;


            if (DeviceExtension->InterfaceName.Buffer != NULL) {

                IrCommHandleSymbolicLink(
                    DeviceExtension->Pdo,
                    &DeviceExtension->InterfaceName,
                    FALSE
                    );
            }

            if (DeviceExtension->TdiObjects != NULL) {
                //
                //  close the tdi objects that we opened during AddDevice(). This has to
                //  be done in the same process as they were opened which is the system process
                //
                CloseTdiObjects(DeviceExtension->TdiObjects);
            }

            IoCopyCurrentIrpStackLocationToNext(Irp);

            status=IoCallDriver(DeviceExtension->LowerDevice, Irp);

            //
            //  detach from the driver below
            //
            IoDetachDevice(DeviceExtension->LowerDevice);

            //
            //  delete our device object
            //
            IoDeleteDevice(DeviceObject);

            D_PNP(DbgPrint("IrComm: IRP_MN_REMOVE_DEVICE exit, %08lx\n",status);)

            return status;

        }


        default:
            D_PNP(DbgPrint("IrComm: Sending to PDO PnP IRP, MN func=%d\n",irpSp->MinorFunction);)

            return ForwardIrp(DeviceExtension->LowerDevice, Irp);
    }



    IoCompleteRequest(Irp,IO_NO_INCREMENT);

    return STATUS_SUCCESS;

}

NTSTATUS
IrCommPower(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )

{
    PFDO_DEVICE_EXTENSION   DeviceExtension = DeviceObject->DeviceExtension;

    PoStartNextPowerIrp(Irp);

    IoSkipCurrentIrpStackLocation(Irp);

    return PoCallDriver(DeviceExtension->LowerDevice, Irp);

}

NTSTATUS
IrCommWmi(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )

{

    IoCompleteRequest(Irp,IO_NO_INCREMENT);

    return STATUS_SUCCESS;

}
