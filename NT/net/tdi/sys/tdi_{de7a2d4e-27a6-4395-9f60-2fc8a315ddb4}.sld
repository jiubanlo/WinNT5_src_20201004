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
<!DOCTYPE TOOL SYSTEM "file://mess/icat/tool.dtd">
<TOOL>
	<CREATED><NAME>iCat</NAME><VSGUID>{f802f157-4623-426a-8b39-ac2eb4332c74}</VSGUID><VERSION>1.0.0.244</VERSION><BUILD>244</BUILD><DATE>7/18/2000</DATE></CREATED><LASTSAVED><NAME>iCat</NAME><VSGUID>{97b86ee0-259c-479f-bc46-6cea7ef4be4d}</VSGUID><VERSION>1.0.0.452</VERSION><BUILD>452</BUILD><DATE>4/11/2001</DATE></LASTSAVED></TOOL>
]]></TOOLINFO><COMPONENT ComponentVSGUID="{DE7A2D4E-27A6-4395-9F60-2FC8A315DDB4}" ComponentVIGUID="{1FB16490-54E2-4731-B8D4-091A2C12A631}" PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}" RepositoryVSGUID="{8E0BE9ED-7649-47F3-810B-232D36C430B4}" Revision="5" Visibility="200" MultiInstance="0" Released="1" Editable="0" HTMLFinal="0" IsMacro="0" HTMLTitle="" PrototypeVIGUID=""><HELPCONTEXT src="\\mantisqa\ovrdaily\mantis\hlp\arvindm\tdi_component_description.htm">&lt;!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 3.2 Final//EN"&gt;
&lt;HTML DIR="LTR"&gt;&lt;HEAD&gt;
&lt;META HTTP-EQUIV="Content-Type" Content="text/html; charset=Windows-1252"&gt;
&lt;TITLE&gt;TDI Component Description&lt;/TITLE&gt;
&lt;style type="text/css"&gt;@import url(td.css);&lt;/style&gt;&lt;/HEAD&gt;
&lt;BODY TOPMARGIN="0"&gt;
&lt;H1&gt;&lt;A NAME="tdi_component_description"&gt;&lt;/A&gt;&lt;SUP&gt;&lt;/SUP&gt;TDI Component Description&lt;/H1&gt;

&lt;P&gt;The Transport Drive Interface (TDI) component implements the TDI library that is used by network protocol components such as TCP/IP, and by kernel-mode Winsock components. &lt;/P&gt;

&lt;H1&gt;Component Configuration&lt;/H1&gt;

&lt;P&gt;There are no configuration requirements for this component.&lt;/P&gt;

&lt;H1&gt;For More Information&lt;/H1&gt;

&lt;P&gt;For more information about TDI, see this &lt;A HREF="http://www.microsoft.com/hwdev"&gt;Microsoft Web site&lt;/A&gt;.&lt;/P&gt;

&lt;/BODY&gt;
&lt;/HTML&gt;
</HELPCONTEXT><DISPLAYNAME>TDI</DISPLAYNAME><VERSION>1.0</VERSION><DESCRIPTION>Transport Driver Interface wrapper driver</DESCRIPTION><COPYRIGHT>2000 Microsoft Corp.</COPYRIGHT><VENDOR>Microsoft Corp.</VENDOR><OWNERS>arvindm</OWNERS><AUTHORS>arvindm</AUTHORS><DATECREATED>7/18/2000</DATECREATED><DATEREVISED>4/11/2001</DATEREVISED><RESOURCE Name="File:&quot;%12%&quot;,&quot;tdi.sys&quot;" ResTypeVSGUID="{E66B49F6-4A35-4246-87E8-5C1A468315B5}" BuildTypeMask="819"><PROPERTY Name="DstPath" Format="String">%12%</PROPERTY><PROPERTY Name="DstName" Format="String">tdi.sys</PROPERTY><PROPERTY Name="SrcFileSize" Format="Integer">0</PROPERTY><PROPERTY Name="NoExpand" Format="Boolean">0</PROPERTY><DISPLAYNAME>TDI Wrapper</DISPLAYNAME><DESCRIPTION>Transport Driver Interface system driver</DESCRIPTION></RESOURCE><RESOURCE Name="RawDep:&quot;RawFile&quot;,&quot;ndis.sys&quot;" ResTypeVSGUID="{90D8E195-E710-4af6-B667-B1805FFC9B8F}" BuildTypeMask="819"><PROPERTY Name="RawType" Format="String">File</PROPERTY><PROPERTY Name="Value" Format="String">ndis.sys</PROPERTY></RESOURCE><GROUPMEMBER GroupVSGUID="{388249D2-1897-44ff-86F2-E159A27AA037}"/><GROUPMEMBER GroupVSGUID="{4E7B2F7A-A581-11D4-8E94-00B0D03D27C6}"/></COMPONENT></DCARRIER>
