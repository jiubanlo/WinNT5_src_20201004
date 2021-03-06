/* msiauto.hh = help context identifiers for automation help

  This file is included by msiauto.hpj, makeodl.cpp, autosrv.cpp
*/

#define MsiBase_Object                1
#define MsiBase_HasInterface          2
#define MsiBase_RefCount              3
#define MsiBase_GetInterface          4

#define Msi_Obsolete                 99

#define MsiData_Object              100
#define MsiData_StringValue         199
#define MsiData_IntegerValue        101

#define MsiString_Object            200
#define MsiString_Value             299
#define MsiString_IntegerValue      201
#define MsiString_TextSize          202
#define MsiString_CharacterCount    203
#define MsiString_IsDBCS            204
#define MsiString_Compare           205
#define MsiString_Append            206
#define MsiString_Add               207
#define MsiString_Extract           208
#define MsiString_Remove            209
#define MsiString_UpperCase         210
#define MsiString_LowerCase         211

#define MsiRecord_Object            300
#define MsiRecord_Data              399
#define MsiRecord_StringData        301     
#define MsiRecord_IntegerData       302     
#define MsiRecord_ObjectData        303     
#define MsiRecord_FieldCount        304
#define MsiRecord_IsInteger         305
#define MsiRecord_IsNull            306
#define MsiRecord_IsChanged         307
#define MsiRecord_TextSize          308
#define MsiRecord_FormatText        309
#define MsiRecord_ClearData         310
#define MsiRecord_ClearUpdate       311

#define MsiVolume_Object            400
#define MsiVolume_Path              499
#define MsiVolume_VolumeID          401
#define MsiVolume_DriveType         402
#define MsiVolume_SupportsLFN       403
#define MsiVolume_FreeSpace         404
#define MsiVolume_ClusterSize       405
#define MsiVolume_FileSystem        406
#define MsiVolume_UNCServer         407
#define MsiVolume_SerialNum         408
#define MsiVolume_DiskNotInDrive    409
#define MsiVolume_VolumeLabel       410
#define MsiVolume_TotalSpace        411
#define MsiVolume_FileSystemFlags   412

#define MsiPath_Object              500
#define MsiPath_Path                599
#define MsiPath_Volume              501
#define MsiPath_AppendPiece         502
#define MsiPath_ChopPiece           503
#define MsiPath_FileExists          504
#define MsiPath_GetFullFilePath     505
#define MsiPath_GetFileAttribute    507
#define MsiPath_SetFileAttribute    508
#define MsiPath_Exists              509
#define MsiPath_FileSize            510
#define MsiPath_FileDate            511
#define MsiPath_RemoveFile          512
#define MsiPath_EnsureExists        513
#define MsiPath_Remove              514
#define MsiPath_Writable            515
#define MsiPath_FileWritable        516
#define MsiPath_FileInUse           517
#define MsiPath_UpdateResource      518
#define MsiPath_ClusteredFileSize   519
#define MsiPath_GetFileVersionString 520
#define MsiPath_CheckFileVersion    521
#define MsiPath_GetLangIDStringFromFile 522
#define MsiPath_CheckLanguageIDs    523
#define MsiPath_Compare             524
#define MsiPath_Child               525
#define MsiPath_TempFileName        526
#define MsiPath_FindFile            527
#define MsiPath_SubFolders          528
#define MsiPath_EndSubPath          529
#define MsiPath_ReadResource        530
#define MsiPath_GetImportModulesEnum 531
#define MsiPath_SetVolume           532
#define MsiPath_ComputeFileChecksum 533
#define MsiPath_GetFileOriginalChecksum 534
#define MsiPath_BindImage           535
#define MsiPath_SupportsLFN         536
#define MsiPath_GetFullUNCFilePath  537
#define MsiPath_RelativePath        538
#define MsiPath_GetSelfRelativeSD	539

#define MsiFileCopy_Object          600
#define MsiFileCopy_CopyTo          601
#define MsiFileCopy_ChangeMedia     602

#define MsiRegKey_Object            700
#define MsiRegKey_Value             701
#define MsiRegKey_Exists            702
#define MsiRegKey_RemoveValue       703
#define MsiRegKey_RemoveSubKey      704
#define MsiRegKey_RemoveSubTree     705
#define MsiRegKey_Values            706
#define MsiRegKey_SubKeys           707
#define MsiRegKey_CreateChild       708
#define MsiRegKey_Key               709
#define MsiRegKey_ValueExists       710
#define MsiRegKey_GetSelfRelativeSD	711

#define MsiTable_Object             800
#define MsiTable_Database           801
#define MsiTable_RowCount           802
#define MsiTable_ColumnCount        803
#define MsiTable_PrimaryKeyCount    804
#define MsiTable_ReadOnly           805
#define MsiTable_ColumnName         806
#define MsiTable_ColumnType         807
#define MsiTable_GetColumnIndex     808
#define MsiTable_CreateColumn       809
#define MsiTable_CreateCursor       810
#define MsiTable_LinkTree           811

#define MsiCursor_Object            900
#define MsiCursor_Table             901
#define MsiCursor_Filter            902
#define MsiCursor_IntegerData       903
#define MsiCursor_StringData        904
#define MsiCursor_ObjectData        905
#define MsiCursor_StreamData        906
#define MsiCursor_PutNull           907
#define MsiCursor_Reset             908
#define MsiCursor_Next              909
#define MsiCursor_Update            910
#define MsiCursor_Insert            911
#define MsiCursor_InsertTemporary   912
#define MsiCursor_Assign            913
#define MsiCursor_Merge             914
#define MsiCursor_Refresh           915
#define MsiCursor_Delete            916
#define MsiCursor_Seek              917
#define MsiCursor_RowState          918
#define MsiCursor_DateData          919
#define MsiCursor_Validate          920
#define MsiCursor_Moniker           921
#define MsiCursor_Replace           922

#define MsiAuto_Object                     1000
#define MsiAuto_CreateServices             1001
#define MsiAuto_CreateEngine               1002
#define MsiAuto_CreateHandler              1003
#define MsiAuto_CreateMessageHandler       1004
#define MsiAuto_CreateConfigurationManager 1005
#define MsiAuto_OpcodeName                 1006
#define MsiAuto_ShowAsserts                1007
#define MsiAuto_SetDBCSSimulation          1008
#define MsiAuto_AssertNoObjects			   1009
#define MsiAuto_SetRefTracking			   1010
#define MsiAuto_CreateExecutor            1011

#define MsiServices_Object                    1100
#define MsiServices_GetAllocator              1101
#define MsiServices_CreateString              1102
#define MsiServices_CreateRecord              1103
#define MsiServices_SetPlatformProperties     1104
#define MsiServices_CreateLog                 1105
#define MsiServices_WriteLog                  1106
#define MsiServices_LoggingEnabled            1107
#define MsiServices_CreateDatabase            1108
#define MsiServices_CreateDatabaseFromStorage 1109
#define MsiServices_CreatePath                1110
#define MsiServices_CreateVolume              1111
#define MsiServices_CreateCopier              1112
#define MsiServices_ClearAllCaches            1113
#define MsiServices_EnumDriveType             1114
#define MsiServices_GetModuleUsage            1115
#define MsiServices_GetLocalPath              1116
#define MsiServices_CreateRegKey              1117
#define MsiServices_RegisterFont              1118
#define MsiServices_UnRegisterFont            1119
#define MsiServices_WriteIniFile              1120
#define MsiServices_ReadIniFile               1121
#define MsiServices_GetLangNamesFromLangIDString 1122
#define MsiServices_CreateStorage             1123
#define MsiServices_GetUnhandledError         1124
#define MsiServices_SupportLanguageId         1125
#define MsiServices_CreateVolumeFromLabel     1127
#define MsiServices_CreateShortcut            1128
#define MsiServices_RemoveShortcut            1129
#define MsiServices_AttachClient              1132
#define MsiServices_DetachClient              1133
#define MsiServices_ExtractFileName           1134
#define MsiServices_ValidateFileName          1135
#define MsiServices_CreateFileStream          1136
#define MsiServices_CreateMemoryStream        1137
#define MsiServices_RegisterTypeLibrary       1138
#define MsiServices_UnregisterTypeLibrary     1139
#define MsiServices_GetShellFolderPath        1140
#define MsiServices_GetUserProfilePath        1141
#define MsiServices_CreateFilePath            1142
#define MsiServices_RipFileNameFromPath       1143
#define MsiServices_CreatePatcher             1144

#define MsiView_Object             1200
#define MsiView_Execute            1201
#define MsiView_FieldCount         1202
#define MsiView_Fetch              1203
#define MsiView_GetColumnNames     1204
#define MsiView_GetColumnTypes     1205
#define MsiView_Modify             1206
#define MsiView_RowCount           1207
#define MsiView_Close              1208
#define MsiView_GetError           1209
#define MsiView_State              1210

#define MsiDatabase_Object               1300
#define MsiDatabase_UpdateState          1301
#define MsiDatabase_Storage              1302
#define MsiDatabase_OpenView             1303
#define MsiDatabase_GetPrimaryKeys       1304
#define MsiDatabase_ImportTable          1305
#define MsiDatabase_ExportTable          1306
#define MsiDatabase_DropTable            1307
#define MsiDatabase_FindTable            1308
#define MsiDatabase_LoadTable            1309
#define MsiDatabase_CreateTable          1310
#define MsiDatabase_LockTable            1311
#define MsiDatabase_GetCatalogTable      1312
#define MsiDatabase_DecodeString         1313
#define MsiDatabase_EncodeString         1314
#define MsiDatabase_CreateTempTableName  1315
#define MsiDatabase_Commit               1316
#define MsiDatabase_CreateOutputDatabase 1317
#define MsiDatabase_GenerateTransform    1318
#define MsiDatabase_SetTransform         1319
#define MsiDatabase_MergeDatabase        1320
#define MsiDatabase_TableState           1321
#define MsiDatabase_ANSICodePage         1322

#define MsiEngine_Object                 1400
#define MsiEngine_Services               1401
#define MsiEngine_ConfigurationServer    1402
#define MsiEngine_Handler                1403
#define MsiEngine_Database               1404
#define MsiEngine_Property               1405
#define MsiEngine_SelectionManager       1406
#define MsiEngine_DirectoryManager       1407
#define MsiEngine_Initialize             1408
#define MsiEngine_Terminate              1409
#define MsiEngine_DoAction               1410
#define MsiEngine_Sequence               1411
#define MsiEngine_Message                1412
#define MsiEngine_OpenView               1414
#define MsiEngine_ResolveFolderProperty  1415
#define MsiEngine_FormatText             1416
#define MsiEngine_EvaluateCondition      1417
#define MsiEngine_ExecuteRecord          1418
#define MsiEngine_ValidateProductID      1419
#define MsiEngine_GetMode                1420
#define MsiEngine_SetMode                1421


#define MsiHandler_Object           1500
#define MsiHandler_Message          1501
#define MsiHandler_DoAction         1502
#define MsiHandler_Break            1503

#define MsiDialog_Object            1600
#define MsiDialog_Visible           1601
#define MsiDialog_ControlCreate     1602
#define MsiDialog_Attribute         1603
#define MsiDialog_Control           1604
#define MsiDialog_AddControl        1605
#define MsiDialog_Execute           1606
#define MsiDialog_Reset             1607
#define MsiDialog_EventAction       1608
#define MsiDialog_RemoveControl     1609
#define MsiDialog_StringValue       1610
#define MsiDialog_IntegerValue      1611
#define MsiDialog_Handler           1612
#define MsiDialog_PropertyChanged	1613
#define MsiDialog_FinishCreate      1614
#define MsiDialog_HandleEvent       1615

#define MsiEvent_Object               1700
#define MsiEvent_PropertyChanged      1701
#define MsiEvent_ControlActivated     1702
#define MsiEvent_RegisterControlEvent 1703
#define MsiEvent_Handler              1704
#define MsiEvent_PublishEvent         1705
#define MsiEvent_Control              1706
#define MsiEvent_Attribute            1707
#define MsiEvent_EventAction          1708
#define MsiEvent_SetFocus             1709
#define MsiEvent_StringValue          1710
#define MsiEvent_IntegerValue         1711
#define MsiEvent_HandleEvent          1712
#define MsiEvent_Engine               1713
#define MsiEvent_Escape               1714

#define MsiControl_Object             1800
#define MsiControl_Attribute          1801
#define MsiControl_CanTakeFocus       1802
#define MsiControl_HandleEvent        1803
#define MsiControl_Undo               1804
#define MsiControl_SetPropertyInDatabase 1805
#define MsiControl_GetPropertyFromDatabase 1806
#define MsiControl_SetFocus           1807
#define MsiControl_Dialog             1808
#define MsiControl_WindowMessage      1809
#define MsiControl_StringValue        1810
#define MsiControl_IntegerValue       1811
#define MsiControl_GetIndirectPropertyFromDatabase 1812

#define MsiDialogHandler_Object                1900         
#define MsiDialogHandler_DialogCreate          1901 
#define MsiDialogHandler_Dialog                1902
#define MsiDialogHandler_DialogFromWindow      1903  
#define MsiDialogHandler_AddDialog             1904
#define MsiDialogHandler_RemoveDialog          1905

#define MsiStorage_Object              2000
#define MsiStorage_Class               2001
#define MsiStorage_OpenStream          2002
#define MsiStorage_OpenStorage         2003
#define MsiStorage_Streams             2004
#define MsiStorage_Storages            2005
#define MsiStorage_RemoveElement       2006
#define MsiStorage_Commit              2007
#define MsiStorage_Rollback            2008
#define MsiStorage_DeleteOnRelease     2009
#define MsiStorage_CreateSummaryInfo   2010
#define MsiStorage_CopyTo              2011
#define MsiStorage_Name                2012
#define MsiStorage_RenameElement       2013

#define MsiStream_Object           2100
#define MsiStream_Length           2101
#define MsiStream_Remaining        2102
#define MsiStream_Error            2103
#define MsiStream_GetData          2104
#define MsiStream_PutData          2105
#define MsiStream_GetInt16         2106
#define MsiStream_GetInt32         2107
#define MsiStream_PutInt16         2108
#define MsiStream_PutInt32         2109
#define MsiStream_Reset            2110
#define MsiStream_Seek             2111
#define MsiStream_Clone            2112

#define MsiSummaryInfo_Object              2200
#define MsiSummaryInfo_Property            2299
#define MsiSummaryInfo_PropertyCount       2201
#define MsiSummaryInfo_PropertyType        2202
#define MsiSummaryInfo_WritePropertyStream 2203

#define MsiMalloc_Object           2300
#define MsiMalloc_Alloc            2301
#define MsiMalloc_Free             2302
#define MsiMalloc_SetDebugFlags    2303
#define MsiMalloc_GetDebugFlags    2304
#define MsiMalloc_CheckAllBlocks   2305
#define MsiMalloc_FCheckBlock      2306
#define MsiMalloc_GetSizeOfBlock   2307

#define MsiSelectionManager_Object                2400
#define MsiSelectionManager_LoadSelectionTables   2401
#define MsiSelectionManager_FeatureTable          2402
#define MsiSelectionManager_ProcessConditionTable 2403
#define MsiSelectionManager_ComponentTable        2404
#define MsiSelectionManager_FreeSelectionTables   2405
#define MsiSelectionManager_SetFeatureHandle      2406
#define MsiSelectionManager_SetComponent          2407
#define MsiSelectionManager_SetInstallLevel       2408
#define MsiSelectionManager_GetVolumeCostTable    2409
#define MsiSelectionManager_RecostDirectory       2411
#define MsiSelectionManager_InitializeDynamicCost 2413
#define MsiSelectionManager_RegisterCostAdjuster  2414
#define MsiSelectionManager_InitializeComponents  2415
#define MsiSelectionManager_ConfigureFeature      2416
#define MsiSelectionManager_GetFeatureCost        2417
#define MsiSelectionManager_GetDescendentFeatureCost   2418
#define MsiSelectionManager_GetAncestryFeatureCost 2419
#define MsiSelectionManager_GetFeatureValidStates 2420

#define MsiDirectoryManager_Object             2500
#define MsiDirectoryManager_LoadDirectoryTable 2501
#define MsiDirectoryManager_DirectoryTable     2502
#define MsiDirectoryManager_FreeDirectoryTable 2503
#define MsiDirectoryManager_CreateTargetPaths  2504
#define MsiDirectoryManager_CreateSourcePaths  2505
#define MsiDirectoryManager_GetTargetPath      2506
#define MsiDirectoryManager_SetTargetPath      2507
#define MsiDirectoryManager_GetSourcePath      2508

#define MsiCostAdjuster_Object                 2600

#define MsiConfigurationManager_Object              2700
#define MsiConfigurationManager_RunScript           2704
#define MsiConfigurationManager_RegisterUser        2710
#define MsiConfigurationManager_ProductDatabasePath 2711
#define MsiConfigurationManager_RegisterRollbackScript   2714
#define MsiConfigurationManager_UnregisterRollbackScript 2715
#define MsiConfigurationManager_RollbackScripts          2716

#define MsiConfigurationManager_InstallFinalize              2718
#define MsiConfigurationManager_Services                     2719
#define MsiConfigurationManager_RegisterProduct              2720
#define MsiConfigurationManager_UnregisterProduct            2721 
#define MsiConfigurationManager_RegisterComponent            2722
#define MsiConfigurationManager_UnregisterComponent          2723
#define MsiConfigurationManager_RegisterFolder               2726
#define MsiConfigurationManager_UnregisterFolder             2727
#define MsiConfigurationManager_IsFolderRemovable            2728
#define MsiConfigurationManager_LockServer                   2729
#define MsiConfigurationManager_SetLastUsedSource            2731
#define MsiConfigurationManager_DoInstall                    2732
#define MsiConfigurationManager_UnlockServer                 2733

#define MsiServer_Object              2800

#define MsiMessage_Object            2900
#define MsiMessage_Message           2901

#define MsiExecute_Object            3000
#define MsiExecute_ExecuteRecord     3001
#define MsiExecute_RunScript         3003
#define MsiExecute_RemoveRollbackFiles 3004
#define MsiExecute_Rollback          3005
#define MsiExecute_RollbackFinalize  3006
#define MsiExecute_CreateScript      3007
#define MsiExecute_WriteScriptRecord 3008
#define MsiExecute_CloseScript       3009

#define MsiFilePatch_Object          3100
#define MsiFilePatch_ApplyPatch      3101
#define MsiFilePatch_ContinuePatch   3102
#define MsiFilePatch_CanPatchFile    3103
#define MsiFilePatch_CancelPatch     3104


#define Server_ScriptOpcodes       5000
// Operation_* defines added in build - based on opcodes.h
