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
	<CREATED><NAME>iCat</NAME><VSGUID>{f802f157-4623-426a-8b39-ac2eb4332c74}</VSGUID><VERSION>1.0.0.244</VERSION><BUILD>244</BUILD><DATE>7/25/2000</DATE></CREATED><LASTSAVED><NAME>iCat</NAME><VSGUID>{97b86ee0-259c-479f-bc46-6cea7ef4be4d}</VSGUID><VERSION>1.0.0.452</VERSION><BUILD>452</BUILD><DATE>4/17/2001</DATE></LASTSAVED></TOOL>
]]></TOOLINFO><COMPONENT ComponentVSGUID="{BAA4CC2D-0E00-4CB4-B145-7DB9E19AA303}" ComponentVIGUID="{A1E1E395-2CB8-47AC-BA3F-385BBA6E41B8}" PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}" RepositoryVSGUID="{8E0BE9ED-7649-47F3-810B-232D36C430B4}" Revision="5" Visibility="1000" MultiInstance="False" Released="1" Editable="True"><HELPCONTEXT src="G:\nt.A\net\ias\misc\sld\ias_netshell_component_description.htm">&lt;!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 3.2 Final//EN"&gt;
&lt;HTML DIR="LTR"&gt;&lt;HEAD&gt;
&lt;META HTTP-EQUIV="Content-Type" Content="text/html; charset=Windows-1252"&gt;
&lt;TITLE&gt;IAS NetShell Component Description&lt;/TITLE&gt;
&lt;style type="text/css"&gt;@import url(td.css);&lt;/style&gt;&lt;/HEAD&gt;
&lt;BODY TOPMARGIN="0"&gt;
&lt;H1&gt;&lt;A NAME="ias_netshell_component_description"&gt;&lt;/A&gt;&lt;SUP&gt;&lt;/SUP&gt;IAS NetShell Component Description&lt;/H1&gt;

&lt;P&gt;The Internet Authentication Service (IAS) NetShell component contains the command-line interface for IAS. . &lt;/P&gt;

&lt;H1&gt;Component Configuration&lt;/H1&gt;

&lt;P&gt;There are no configuration requirements for this component.&lt;/P&gt;

&lt;/BODY&gt;
&lt;/HTML&gt;
</HELPCONTEXT><DISPLAYNAME>Internet Authentication Service (IAS) Net Shell Context</DISPLAYNAME><VERSION>1.0</VERSION><DESCRIPTION>IAS RAP Netshell Context</DESCRIPTION><COPYRIGHT>2000 Microsoft Corp.</COPYRIGHT><VENDOR>Microsoft Corp.</VENDOR><OWNERS>tperraut;sbens</OWNERS><AUTHORS>tperraut</AUTHORS><DATECREATED>7/25/2000</DATECREATED><DATEREVISED>4/17/2001</DATEREVISED><RESOURCE Name="File:&quot;%11%&quot;,&quot;aaaamon.dll&quot;" ResTypeVSGUID="{E66B49F6-4A35-4246-87E8-5C1A468315B5}" BuildTypeMask="819"><PROPERTY Name="DstPath" Format="String">%11%</PROPERTY><PROPERTY Name="DstName" Format="String">aaaamon.dll</PROPERTY><PROPERTY Name="SrcFileSize" Format="Integer">0</PROPERTY><PROPERTY Name="NoExpand" Format="Boolean">0</PROPERTY><DISPLAYNAME>aaaamon.dll</DISPLAYNAME></RESOURCE><RESOURCE Name="RegKey:HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\NetSh\aaaamon" ResTypeVSGUID="{2C10DB69-39AB-48a4-A83F-9AB3ACBA7C45}" BuildTypeMask="819" Localize="0"><PROPERTY Name="KeyPath" Format="String">HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\NetSh</PROPERTY><PROPERTY Name="ValueName" Format="String">aaaamon</PROPERTY><PROPERTY Name="RegValue" Format="String">aaaamon.dll</PROPERTY><PROPERTY Name="RegType" Format="Integer">1</PROPERTY><PROPERTY Name="RegOp" Format="Integer">1</PROPERTY><PROPERTY Name="RegCond" Format="Integer">3</PROPERTY></RESOURCE><RESOURCE Name="RawDep:&quot;RawFile&quot;,&quot;netsh.exe&quot;" ResTypeVSGUID="{90D8E195-E710-4af6-B667-B1805FFC9B8F}" BuildTypeMask="819"><PROPERTY Name="RawType" Format="String">File</PROPERTY><PROPERTY Name="Value" Format="String">netsh.exe</PROPERTY></RESOURCE><RESOURCE Name="RawDep:&quot;RawFile&quot;,&quot;iasrecst.dll&quot;" ResTypeVSGUID="{90D8E195-E710-4af6-B667-B1805FFC9B8F}" BuildTypeMask="819"><PROPERTY Name="RawType" Format="String">File</PROPERTY><PROPERTY Name="Value" Format="String">iasrecst.dll</PROPERTY></RESOURCE><RESOURCE Name="RawDep:&quot;RawCLSID&quot;,&quot;{6BC09693-0CE6-11D1-BAAE-00C04FC2E20D}&quot;" ResTypeVSGUID="{90D8E195-E710-4af6-B667-B1805FFC9B8F}" BuildTypeMask="819"><PROPERTY Name="RawType" Format="String">CLSID</PROPERTY><PROPERTY Name="Value" Format="String">{6BC09693-0CE6-11D1-BAAE-00C04FC2E20D}</PROPERTY></RESOURCE><RESOURCE ResTypeVSGUID="{90D8E195-E710-4AF6-B667-B1805FFC9B8F}" BuildTypeMask="819" Name="RawDep(819):&quot;File&quot;,&quot;MSVCRT.dll&quot;"><PROPERTY Name="RawType" Format="String">File</PROPERTY><PROPERTY Name="Value" Format="String">MSVCRT.dll</PROPERTY></RESOURCE><RESOURCE ResTypeVSGUID="{90D8E195-E710-4AF6-B667-B1805FFC9B8F}" BuildTypeMask="819" Name="RawDep(819):&quot;File&quot;,&quot;MSVCP60.dll&quot;"><PROPERTY Name="RawType" Format="String">File</PROPERTY><PROPERTY Name="Value" Format="String">MSVCP60.dll</PROPERTY></RESOURCE><RESOURCE ResTypeVSGUID="{90D8E195-E710-4AF6-B667-B1805FFC9B8F}" BuildTypeMask="819" Name="RawDep(819):&quot;File&quot;,&quot;ntdll.dll&quot;"><PROPERTY Name="RawType" Format="String">File</PROPERTY><PROPERTY Name="Value" Format="String">ntdll.dll</PROPERTY></RESOURCE><RESOURCE ResTypeVSGUID="{90D8E195-E710-4AF6-B667-B1805FFC9B8F}" BuildTypeMask="819" Name="RawDep(819):&quot;File&quot;,&quot;ADVAPI32.dll&quot;"><PROPERTY Name="RawType" Format="String">File</PROPERTY><PROPERTY Name="Value" Format="String">ADVAPI32.dll</PROPERTY></RESOURCE><RESOURCE ResTypeVSGUID="{90D8E195-E710-4AF6-B667-B1805FFC9B8F}" BuildTypeMask="819" Name="RawDep(819):&quot;File&quot;,&quot;KERNEL32.dll&quot;"><PROPERTY Name="RawType" Format="String">File</PROPERTY><PROPERTY Name="Value" Format="String">KERNEL32.dll</PROPERTY></RESOURCE><RESOURCE ResTypeVSGUID="{90D8E195-E710-4AF6-B667-B1805FFC9B8F}" BuildTypeMask="819" Name="RawDep(819):&quot;File&quot;,&quot;NETSH.EXE&quot;"><PROPERTY Name="RawType" Format="String">File</PROPERTY><PROPERTY Name="Value" Format="String">NETSH.EXE</PROPERTY></RESOURCE><RESOURCE ResTypeVSGUID="{90D8E195-E710-4AF6-B667-B1805FFC9B8F}" BuildTypeMask="819" Name="RawDep(819):&quot;File&quot;,&quot;ole32.dll&quot;"><PROPERTY Name="RawType" Format="String">File</PROPERTY><PROPERTY Name="Value" Format="String">ole32.dll</PROPERTY></RESOURCE><RESOURCE ResTypeVSGUID="{90D8E195-E710-4AF6-B667-B1805FFC9B8F}" BuildTypeMask="819" Name="RawDep(819):&quot;File&quot;,&quot;OLEAUT32.dll&quot;"><PROPERTY Name="RawType" Format="String">File</PROPERTY><PROPERTY Name="Value" Format="String">OLEAUT32.dll</PROPERTY></RESOURCE><RESOURCE ResTypeVSGUID="{90D8E195-E710-4AF6-B667-B1805FFC9B8F}" BuildTypeMask="819" Name="RawDep(819):&quot;File&quot;,&quot;SHLWAPI.dll&quot;"><PROPERTY Name="RawType" Format="String">File</PROPERTY><PROPERTY Name="Value" Format="String">SHLWAPI.dll</PROPERTY></RESOURCE><RESOURCE ResTypeVSGUID="{90D8E195-E710-4AF6-B667-B1805FFC9B8F}" BuildTypeMask="819" Name="RawDep(819):&quot;File&quot;,&quot;USERENV.dll&quot;"><PROPERTY Name="RawType" Format="String">File</PROPERTY><PROPERTY Name="Value" Format="String">USERENV.dll</PROPERTY></RESOURCE><GROUPMEMBER GroupVSGUID="{388249D2-1897-44ff-86F2-E159A27AA037}"/><GROUPMEMBER GroupVSGUID="{64668FB9-9289-45f0-BEF9-23745D272E3D}"/></COMPONENT></DCARRIER>
