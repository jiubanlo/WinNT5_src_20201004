<?xml version="1.0" encoding="UTF-16"?>
<!DOCTYPE DCARRIER SYSTEM "mantis.dtd" [
<!-- RegKey registry types (map to REG_SZ etc) -->
<!ENTITY RegTypeNone                        "0" >
<!ENTITY RegTypeSz                          "1" >
<!ENTITY RegTypeExpandSz                    "2" >
<!ENTITY RegTypeBinary                      "3" >
<!ENTITY RegTypeDword                       "4" >
<!ENTITY RegTypeDwordBigEndian              "5" >
<!ENTITY RegTypeLink                        "6" >
<!ENTITY RegTypeMultiSz                     "7" >
<!ENTITY RegTypeResourceList                "8" >
<!ENTITY RegTypeFullResourceDescriptor      "9" >
<!ENTITY RegTypeResourceRequirementsList    "10" >
<!ENTITY RegTypeQword                       "11" >

<!-- RegKey registry operations -->
<!ENTITY RegOpWrite               "1" >
<!ENTITY RegOpDelete              "2" >
<!ENTITY RegOpEdit                "3" >

<!-- RegKey registry conditionals -->
<!ENTITY RegCondAlways            "1" >
<!ENTITY RegCondIfExists          "2" >
<!ENTITY RegCondIfNotExists       "3" >

<!-- RawDep dependency types -->
<!ENTITY RawDepNone               "None" >
<!ENTITY RawDepCLSID              "CLSID" >
<!ENTITY RawDepFile               "File" >
<!ENTITY RawDepRegKey             "RegKey" >
<!ENTITY RawDepRegValue           "RegValue" >
<!ENTITY RawDepRegPath            "RegPath" >
]>
<DCARRIER CarrierRevision="1">
	<TOOLINFO ToolName="iCat"><![CDATA[<?xml version="1.0"?>
<!DOCTYPE ICAT SYSTEM "file://mess/icat/icat.dtd">
<ICAT>
	<VSGUID>{b1d2b402-607c-42c7-b618-fa902d01afc7}</VSGUID><VERSION>1.0.0.216</VERSION><BUILD>216</BUILD></ICAT>
]]></TOOLINFO><COMPONENT ComponentVSGUID="{A9CF983F-38B5-4A92-9DC4-2BD5894CA341}" ComponentVIGUID="{269911BF-8CF1-434D-A6F0-23CBACC6EDB4}" PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}" RepositoryVSGUID="{484C9D34-846E-40E2-A2E6-FF2771A303D5}" Revision="1" Visibility="1000" MultiInstance="False" Released="True" Editable="True"><DISPLAYNAME>Shell common controls</DISPLAYNAME><VERSION>5.80</VERSION><DESCRIPTION>Listview, Treeview, and other controls</DESCRIPTION><COPYRIGHT>2000 Microsoft Corp.</COPYRIGHT><VENDOR>Microsoft Corp.</VENDOR><OWNERS>lamadio</OWNERS><AUTHORS>raymondc</AUTHORS><DATECREATED>3/24/2000</DATECREATED><RESOURCE Name="File:&quot;%11%&quot;,&quot;comctl32.dll&quot;" ResTypeVSGUID="{E66B49F6-4A35-4246-87E8-5C1A468315B5}" BuildTypeMask="819"><PROPERTY Name="DstPath" Format="String">%11%</PROPERTY><PROPERTY Name="DstName" Format="String">comctl32.dll</PROPERTY><PROPERTY Name="SrcFileSize" Format="Integer">0</PROPERTY><PROPERTY Name="NoExpand" Format="Boolean">0</PROPERTY><DISPLAYNAME>Shell common controls</DISPLAYNAME><DESCRIPTION>Listview, Treeview, and other controls</DESCRIPTION></RESOURCE><GROUPMEMBER GroupVSGUID="{350E1818-9E1D-4fd2-9A58-3962965280EE}"/></COMPONENT></DCARRIER>
