; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CManualAnswer
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "ConfigTest.h"

ClassCount=21
Class1=CConfigTestApp
Class2=CConfigTestDlg
Class3=CAboutDlg

ResourceCount=21
Resource1=IDD_DLGDEVICES
Resource2=IDR_MAINFRAME
Resource3=IDD_QUEUESTATE
Class4=CQueueState
Resource4=IDD_MSG_DLG
Class5=CSMTPDlg
Resource5=IDD_REMOVE_R_EXT
Class6=CDlgVersion
Resource6=IDD_ARCHIVE_ACCESS
Class7=COutboxDlg
Resource7=IDD_DLG_ENUM_FSP
Class8=CArchiveDlg
Resource8=IDD_DLGDEVICE
Class9=CDlgActivityLogging
Resource9=IDD_ARCHIVEDLG
Class10=CDlgProviders
Resource10=IDD_DLGEXTENSION
Class11=CDlgDevices
Resource11=IDD_SMTP
Class12=CDlgDevice
Resource12=IDD_DLGVERSION
Class13=CDlgExtensionData
Resource13=IDD_ABOUTBOX
Class14=CAddGroupDlg
Resource14=IDD_REMOVEFSP_DLG
Class15=CAddFSPDlg
Resource15=IDD_ADDFSP_DLG
Class16=CRemoveFSPDlg
Resource16=IDD_ADDGROUP_DLG
Class17=CArchiveAccessDlg
Resource17=IDD_CONFIGTEST_DIALOG
Class18=CArchiveMsgDlg
Resource18=IDD_DLGOUTBOX
Class19=CDlgTIFF
Resource19=IDD_ACTIVITYLOGGING_DLG
Class20=CRemoveRtExt
Resource20=IDD_TIFF_DLG
Class21=CManualAnswer
Resource21=IDD_MANUAL_ANSWER

[CLS:CConfigTestApp]
Type=0
HeaderFile=ConfigTest.h
ImplementationFile=ConfigTest.cpp
Filter=N

[CLS:CConfigTestDlg]
Type=0
HeaderFile=ConfigTestDlg.h
ImplementationFile=ConfigTestDlg.cpp
Filter=D
LastObject=IDC_ARCHIVEACCESS
BaseClass=CDialog
VirtualFilter=dWC

[CLS:CAboutDlg]
Type=0
HeaderFile=ConfigTestDlg.h
ImplementationFile=ConfigTestDlg.cpp
Filter=D

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[DLG:IDD_CONFIGTEST_DIALOG]
Type=1
Class=CConfigTestDlg
ControlCount=21
Control1=IDC_EDIT1,edit,1350631552
Control2=IDC_CONNECT,button,1342242817
Control3=IDC_QUEUESTATE,button,1342243072
Control4=IDC_SMTP,button,1342243072
Control5=IDC_VERSION,button,1342243072
Control6=IDC_OUTBOX,button,1342243072
Control7=IDC_SENTITEMS,button,1342243072
Control8=IDC_INBOX,button,1342243072
Control9=IDC_ACTIVITY,button,1342243072
Control10=IDC_FSPS,button,1342243072
Control11=IDC_DEVICES,button,1342243072
Control12=IDC_EXTENSION,button,1342243072
Control13=IDC_ADDGROUP,button,1342243072
Control14=IDC_ADDFSP,button,1342243072
Control15=IDCANCEL,button,1342242817
Control16=IDC_STATIC,button,1342177287
Control17=IDC_REMOVEFSP,button,1342243072
Control18=IDC_ARCHIVEACCESS,button,1342243072
Control19=IDC_TIFF,button,1342243072
Control20=IDC_REMOVERR,button,1342243072
Control21=IDC_MANUAL_ANSWER,button,1342243072

[DLG:IDD_QUEUESTATE]
Type=1
Class=CQueueState
ControlCount=6
Control1=IDCANCEL,button,1342242816
Control2=IDC_INCOMING_BLOCKED,button,1342242819
Control3=IDC_OUTBOX_BLOCKED,button,1342242819
Control4=IDC_OUTBOX_PAUSED,button,1342242819
Control5=IDC_WRITE,button,1342242816
Control6=IDC_READ,button,1342242816

[CLS:CQueueState]
Type=0
HeaderFile=QueueState.h
ImplementationFile=QueueState.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=CQueueState

[DLG:IDD_SMTP]
Type=1
Class=CSMTPDlg
ControlCount=19
Control1=IDC_REC_OPTIONS,edit,1350639746
Control2=IDC_EDIT_MAPI_PROFILE,edit,1350631552
Control3=IDC_EDIT_SERVER_NAME,edit,1350631552
Control4=IDC_EDIT_SERVER_PORT,edit,1350639746
Control5=IDC_SMTP_AUTH,edit,1350639746
Control6=IDC_EDIT_SENDER,edit,1350631552
Control7=IDC_EDIT_USER_NAME,edit,1350631552
Control8=IDC_EDIT_PASSWORD,edit,1350631552
Control9=IDC_READ,button,1342242816
Control10=IDC_WRITE,button,1342242816
Control11=IDCANCEL,button,1342242816
Control12=IDC_STATIC,button,1342177287
Control13=IDC_STATIC,button,1342177287
Control14=IDC_STATIC,button,1342177287
Control15=IDC_STATIC,button,1342177287
Control16=IDC_STATIC,static,1342308352
Control17=IDC_STATIC,button,1342177287
Control18=IDC_STATIC,static,1342308352
Control19=IDC_STATIC,button,1342177287

[CLS:CSMTPDlg]
Type=0
HeaderFile=SMTPDlg.h
ImplementationFile=SMTPDlg.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=CSMTPDlg

[DLG:IDD_DLGVERSION]
Type=1
Class=CDlgVersion
ControlCount=3
Control1=IDOK,button,1342242817
Control2=IDC_STATIC,button,1342177287
Control3=IDC_SERVERVERSION,static,1342308352

[CLS:CDlgVersion]
Type=0
HeaderFile=DlgVersion.h
ImplementationFile=DlgVersion.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=CDlgVersion

[DLG:IDD_DLGOUTBOX]
Type=1
Class=COutboxDlg
ControlCount=20
Control1=IDC_PERSONALCP,button,1342242819
Control2=IDC_USERDEVICETSID,button,1342242819
Control3=IDC_RETRIES,edit,1350639746
Control4=IDC_RETRYDELAY,edit,1350639746
Control5=IDC_STARTH,edit,1350639746
Control6=IDC_STARTM,edit,1350639746
Control7=IDC_ENDH,edit,1350639746
Control8=IDC_ENDM,edit,1350639746
Control9=IDC_AGELIMIT,edit,1350639746
Control10=IDC_BRANDING,button,1342242819
Control11=IDC_READ,button,1342242816
Control12=IDC_WRITE,button,1342242816
Control13=IDCANCEL,button,1342242816
Control14=IDC_STATIC,static,1342308352
Control15=IDC_STATIC,static,1342308352
Control16=IDC_STATIC,static,1342308352
Control17=IDC_STATIC,static,1342308352
Control18=IDC_STATIC,static,1342308352
Control19=IDC_STATIC,static,1342308352
Control20=IDC_STATIC,static,1342308352

[CLS:COutboxDlg]
Type=0
HeaderFile=OutboxDlg.h
ImplementationFile=OutboxDlg.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=COutboxDlg

[DLG:IDD_ARCHIVEDLG]
Type=1
Class=CArchiveDlg
ControlCount=13
Control1=IDC_USE,button,1342242819
Control2=IDC_FOLDER,edit,1350631552
Control3=IDC_WARN,button,1342242819
Control4=IDC_HIGH_WM,edit,1350639746
Control5=IDC_LOW_WM,edit,1350639746
Control6=IDC_AGE_LIMIT,edit,1350639746
Control7=IDC_READ,button,1342242816
Control8=IDC_WRITE,button,1342242816
Control9=IDCANCEL,button,1342242816
Control10=IDC_STATIC,static,1342308352
Control11=IDC_STATIC,static,1342308352
Control12=IDC_STATIC,static,1342308352
Control13=IDC_STATIC,static,1342308352

[CLS:CArchiveDlg]
Type=0
HeaderFile=ArchiveDlg.h
ImplementationFile=ArchiveDlg.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=CArchiveDlg

[DLG:IDD_ACTIVITYLOGGING_DLG]
Type=1
Class=CDlgActivityLogging
ControlCount=7
Control1=IDC_CHK_OUT,button,1342242819
Control2=IDC_CHK_IN,button,1342242819
Control3=IDC_DBFILE,edit,1350631552
Control4=IDC_READ,button,1342242816
Control5=IDC_WRITE,button,1342242816
Control6=IDCANCEL,button,1342242816
Control7=IDC_STATIC,static,1342308352

[CLS:CDlgActivityLogging]
Type=0
HeaderFile=DlgActivityLogging.h
ImplementationFile=DlgActivityLogging.cpp
BaseClass=CDialog
Filter=D
LastObject=IDC_CHK_OUT
VirtualFilter=dWC

[DLG:IDD_DLG_ENUM_FSP]
Type=1
Class=CDlgProviders
ControlCount=5
Control1=IDC_FSPS,SysListView32,1350631433
Control2=IDC_REFRESH,button,1342242816
Control3=IDCANCEL,button,1342242816
Control4=IDC_NUMFSP,static,1342308353
Control5=IDC_STATIC,static,1342308352

[CLS:CDlgProviders]
Type=0
HeaderFile=DlgProviders.h
ImplementationFile=DlgProviders.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=CDlgProviders

[DLG:IDD_DLGDEVICES]
Type=1
Class=CDlgDevices
ControlCount=5
Control1=IDC_DEVS,SysListView32,1350631425
Control2=IDCANCEL,button,1342242816
Control3=IDC_REFRESH,button,1342242816
Control4=IDC_STATIC,static,1342308352
Control5=IDC_NUMDEVS,static,1342308353

[CLS:CDlgDevices]
Type=0
HeaderFile=DlgDevices.h
ImplementationFile=DlgDevices.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=CDlgDevices

[DLG:IDD_DLGDEVICE]
Type=1
Class=CDlgDevice
ControlCount=21
Control1=IDC_DESCRIPTION,edit,1350631552
Control2=IDC_SEND,button,1342242819
Control3=IDC_RECEIVE,button,1342242819
Control4=IDC_RINGS,edit,1350639746
Control5=IDC_CSID,edit,1350631552
Control6=IDC_TSID,edit,1350631552
Control7=IDC_REFRESH,button,1342242816
Control8=IDC_WRITE,button,1342242816
Control9=IDCANCEL,button,1342242816
Control10=IDC_STATIC,static,1342308352
Control11=IDC_DEVID,static,1342308354
Control12=IDC_STATIC,static,1342308352
Control13=IDC_DEVNAME,static,1342308352
Control14=IDC_STATIC,static,1342308352
Control15=IDC_STATIC,static,1342308352
Control16=IDC_PROVNAME,static,1342308352
Control17=IDC_STATIC,static,1342308352
Control18=IDC_PROVGUID,static,1342308352
Control19=IDC_STATIC,static,1342308352
Control20=IDC_STATIC,static,1342308352
Control21=IDC_STATIC,static,1342308352

[CLS:CDlgDevice]
Type=0
HeaderFile=DlgDevice.h
ImplementationFile=DlgDevice.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=CDlgDevice

[DLG:IDD_DLGEXTENSION]
Type=1
Class=CDlgExtensionData
ControlCount=9
Control1=IDC_CMDDEVICES,combobox,1344340035
Control2=IDC_GUID,edit,1350631552
Control3=IDC_DATA,edit,1350631620
Control4=IDC_READ,button,1342242816
Control5=IDC_WRITE,button,1342242816
Control6=IDCANCEL,button,1342242816
Control7=IDC_STATIC,static,1342308352
Control8=IDC_STATIC,static,1342308352
Control9=IDC_STATIC,static,1342308352

[CLS:CDlgExtensionData]
Type=0
HeaderFile=DlgExtensionData.h
ImplementationFile=DlgExtensionData.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=CDlgExtensionData

[DLG:IDD_ADDGROUP_DLG]
Type=1
Class=CAddGroupDlg
ControlCount=4
Control1=ID_ADD,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_GROUP_NAME,edit,1350631552
Control4=IDC_STATIC,button,1342177287

[CLS:CAddGroupDlg]
Type=0
HeaderFile=AddGroupDlg.h
ImplementationFile=AddGroupDlg.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=CAddGroupDlg

[DLG:IDD_ADDFSP_DLG]
Type=1
Class=CAddFSPDlg
ControlCount=21
Control1=IDC_GUID,edit,1350631552
Control2=IDC_FRIENDLY_NAME,edit,1350631552
Control3=IDC_IMAGENAME,edit,1350631552
Control4=IDC_TSPNAME,edit,1350631552
Control5=IDC_VERSION1,button,1342308361
Control6=IDC_VERSION2,button,1342177289
Control7=IDC_FSPI_CAP_BROADCAST,button,1342242819
Control8=IDC_FSPI_CAP_MULTISEND,button,1342242819
Control9=IDC_FSPI_CAP_SCHEDULING,button,1342242819
Control10=IDC_FSPI_CAP_ABORT_RECIPIENT,button,1342242819
Control11=IDC_FSPI_CAP_ABORT_PARENT,button,1342242819
Control12=IDC_FSPI_CAP_AUTO_RETRY,button,1342242819
Control13=IDC_FSPI_CAP_SIMULTANEOUS_SEND_RECEIVE,button,1342242819
Control14=IDADD,button,1342242817
Control15=IDCANCEL,button,1342242816
Control16=IDC_STATIC,static,1342308352
Control17=IDC_STATIC,static,1342308352
Control18=IDC_STATIC,static,1342308352
Control19=IDC_STATIC,static,1342308352
Control20=IDC_STATIC,button,1342177287
Control21=IDC_STATIC,button,1342177287

[CLS:CAddFSPDlg]
Type=0
HeaderFile=AddFSPDlg.h
ImplementationFile=AddFSPDlg.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=CAddFSPDlg

[DLG:IDD_REMOVEFSP_DLG]
Type=1
Class=CRemoveFSPDlg
ControlCount=6
Control1=IDC_GUID,edit,1350631552
Control2=IDC_COMBO,combobox,1344340227
Control3=IDREMOVE,button,1342242817
Control4=IDCANCEL,button,1342242816
Control5=IDC_STATIC,button,1342177287
Control6=IDC_STATIC,button,1342177287

[CLS:CRemoveFSPDlg]
Type=0
HeaderFile=RemoveFSPDlg.h
ImplementationFile=RemoveFSPDlg.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=CRemoveFSPDlg

[DLG:IDD_ARCHIVE_ACCESS]
Type=1
Class=CArchiveAccessDlg
ControlCount=11
Control1=IDCANCEL,button,1342242816
Control2=IDC_REFRESH,button,1342242816
Control3=IDC_STATIC,button,1342177287
Control4=IDC_INBOX,button,1342308361
Control5=IDC_SENDITEMS,button,1342177289
Control6=IDC_STATIC,static,1342308352
Control7=IDC_NUMSGS,static,1342308354
Control8=IDC_LIST,SysListView32,1350631429
Control9=IDC_STATIC,button,1342177287
Control10=IDC_MSGSPERCALL,edit,1350639746
Control11=IDC_SPIN,msctls_updown32,1342177334

[CLS:CArchiveAccessDlg]
Type=0
HeaderFile=ArchiveAccessDlg.h
ImplementationFile=ArchiveAccessDlg.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=CArchiveAccessDlg

[DLG:IDD_MSG_DLG]
Type=1
Class=CArchiveMsgDlg
ControlCount=50
Control1=IDREMOVE,button,1342242816
Control2=IDCANCEL,button,1342242817
Control3=IDC_STATIC,static,1342308352
Control4=IDC_FOLDER,static,1342308352
Control5=IDC_STATIC,static,1342308352
Control6=IDC_ID,static,1342308352
Control7=IDC_STATIC,static,1342308352
Control8=IDC_TYPE,static,1342308352
Control9=IDC_STATIC,static,1342308352
Control10=IDS_SIZE,static,1342308352
Control11=IDC_STATIC,static,1342308352
Control12=IDC_PAGES,static,1342308352
Control13=IDC_STATIC,static,1342308352
Control14=IDC_R_NUMBER,static,1342308352
Control15=IDC_STATIC,static,1342308352
Control16=IDC_R_NAME,static,1342308352
Control17=IDC_STATIC,static,1342308352
Control18=IDC_S_NUMBER,static,1342308352
Control19=IDC_STATIC,static,1342308352
Control20=IDC_S_NAME,static,1342308352
Control21=IDC_STATIC,static,1342308352
Control22=IDC_TSID_VAL,static,1342308352
Control23=IDC_STATIC,static,1342308352
Control24=IDC_CSID_VAL,static,1342308352
Control25=IDC_STATIC,static,1342308352
Control26=IDC_SEND_USER,static,1342308352
Control27=IDC_STATIC,static,1342308352
Control28=IDC_BILLING,static,1342308352
Control29=IDC_STATIC,static,1342308352
Control30=IDC_ORIGTIME,static,1342308352
Control31=IDC_STATIC,static,1342308352
Control32=IDC_SUBMITTIME,static,1342308352
Control33=IDC_STATIC,static,1342308352
Control34=IDC_START_TIME,static,1342308352
Control35=IDC_STATIC,static,1342308352
Control36=IDC_END_TIME,static,1342308352
Control37=IDC_STATIC,static,1342308352
Control38=IDC_DEVICE,static,1342308352
Control39=IDC_STATIC,static,1342308352
Control40=IDC_PRIORITY,static,1342308352
Control41=IDC_STATIC,static,1342308352
Control42=IDC_RETRIES,static,1342308352
Control43=IDC_STATIC,static,1342308352
Control44=IDC_DOCUMENT,static,1342308352
Control45=IDC_STATIC,static,1342308352
Control46=IDC_SUBJECT,static,1342308352
Control47=IDC_STATIC,static,1342308352
Control48=IDC_CALLERID,static,1342308352
Control49=IDC_STATIC,static,1342308352
Control50=IDC_ROUTINGINFO,static,1342308352

[CLS:CArchiveMsgDlg]
Type=0
HeaderFile=ArchiveMsgDlg.h
ImplementationFile=ArchiveMsgDlg.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=CArchiveMsgDlg

[DLG:IDD_TIFF_DLG]
Type=1
Class=CDlgTIFF
ControlCount=10
Control1=IDC_DESTFILE,edit,1350631552
Control2=IDC_INBOX,button,1342308361
Control3=IDC_SENTITEMS,button,1342177289
Control4=IDC_QUEUE,button,1342177289
Control5=IDC_MGSID,edit,1350639746
Control6=IDC_COPY,button,1342242816
Control7=IDCANCEL,button,1342242816
Control8=IDC_STATIC,static,1342308352
Control9=IDC_STATIC,button,1342177287
Control10=IDC_STATIC,static,1342308352

[CLS:CDlgTIFF]
Type=0
HeaderFile=DlgTIFF.h
ImplementationFile=DlgTIFF.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=CDlgTIFF

[DLG:IDD_REMOVE_R_EXT]
Type=1
Class=CRemoveRtExt
ControlCount=4
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC,button,1342177287
Control4=IDC_EXTNAME,edit,1350631552

[CLS:CRemoveRtExt]
Type=0
HeaderFile=RemoveRtExt.h
ImplementationFile=RemoveRtExt.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=IDOK

[CLS:CManualAnswer]
Type=0
HeaderFile=ManualAnswer.h
ImplementationFile=ManualAnswer.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=CManualAnswer

[DLG:IDD_MANUAL_ANSWER]
Type=1
Class=CManualAnswer
ControlCount=5
Control1=IDC_MAN_ANSWER_DEV_ID,edit,1350639746
Control2=IDC_READ,button,1342242816
Control3=IDC_WRITE,button,1342242816
Control4=IDCANCEL,button,1342242816
Control5=IDC_STATIC,static,1342308352

