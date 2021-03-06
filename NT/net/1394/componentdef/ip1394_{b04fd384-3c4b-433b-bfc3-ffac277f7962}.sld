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
]]></TOOLINFO><COMPONENT ComponentVSGUID="{B04FD384-3C4B-433B-BFC3-FFAC277F7962}" ComponentVIGUID="{3493B258-6EFA-4FA0-A60E-BC16F6270DC5}" PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}" RepositoryVSGUID="{8E0BE9ED-7649-47F3-810B-232D36C430B4}" Revision="4" Visibility="1000" MultiInstance="False" Released="1" Editable="True" HTMLFinal="False"><HELPCONTEXT src="\\mantisqa\ovrdaily\mantis\hlp\arvindm\ip1394_component_description.htm">&lt;!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 3.2 Final//EN"&gt;
&lt;HTML DIR="LTR"&gt;&lt;HEAD&gt;
&lt;META HTTP-EQUIV="Content-Type" Content="text/html; charset=Windows-1252"&gt;
&lt;TITLE&gt;IP1394 Component Description&lt;/TITLE&gt;
&lt;style type="text/css"&gt;@import url(td.css);&lt;/style&gt;&lt;/HEAD&gt;
&lt;BODY TOPMARGIN="0"&gt;
&lt;H1&gt;&lt;A NAME="ip1394_component_description"&gt;&lt;/A&gt;&lt;SUP&gt;&lt;/SUP&gt;IP1394 Component Description&lt;/H1&gt;

&lt;P&gt;The IP1394 component contains drivers that support networking using the IP v4 protocol over the IEEE 1394 bus specification. The drivers automatically detect the presence of other network-capable systems on a 1394 bus and sets up communication with them.&lt;/P&gt;

&lt;H1&gt;Component Configuration&lt;/H1&gt;

&lt;P&gt;No configuration is required to use this component.&lt;/P&gt;

&lt;H1&gt;For More Information&lt;/H1&gt;

&lt;P&gt;The IP1394 component conforms to IETF RFC 2734.  For more information on these topics, see the Internet Engineering Task Force Web site. &lt;/P&gt;

&lt;/BODY&gt;
&lt;/HTML&gt;
</HELPCONTEXT><DISPLAYNAME>IP1394</DISPLAYNAME><VERSION>1.0</VERSION><DESCRIPTION>Networking over 1394</DESCRIPTION><COPYRIGHT>2000 Microsoft Corp.</COPYRIGHT><VENDOR>Microsoft Corp.</VENDOR><OWNERS>arvindm</OWNERS><AUTHORS>arvindm</AUTHORS><DATECREATED>7/18/2000</DATECREATED><DATEREVISED>4/11/2001</DATEREVISED><RESOURCE Name="File:&quot;%12%&quot;,&quot;enum1394.sys&quot;" ResTypeVSGUID="{E66B49F6-4A35-4246-87E8-5C1A468315B5}" BuildTypeMask="819"><PROPERTY Name="DstPath" Format="String">%12%</PROPERTY><PROPERTY Name="DstName" Format="String">enum1394.sys</PROPERTY><PROPERTY Name="SrcFileSize" Format="Integer">0</PROPERTY><PROPERTY Name="NoExpand" Format="Boolean">0</PROPERTY><DISPLAYNAME>ENUM1394</DISPLAYNAME><DESCRIPTION>Network node enumerator driver for 1394</DESCRIPTION></RESOURCE><RESOURCE Name="File:&quot;%12%&quot;,&quot;nic1394.sys&quot;" ResTypeVSGUID="{E66B49F6-4A35-4246-87E8-5C1A468315B5}" BuildTypeMask="819"><PROPERTY Name="DstPath" Format="String">%12%</PROPERTY><PROPERTY Name="DstName" Format="String">nic1394.sys</PROPERTY><PROPERTY Name="SrcFileSize" Format="Integer">0</PROPERTY><PROPERTY Name="NoExpand" Format="Boolean">0</PROPERTY><DISPLAYNAME>NIC1394</DISPLAYNAME><DESCRIPTION>Network device driver for 1394</DESCRIPTION></RESOURCE><RESOURCE Name="File:&quot;%12%&quot;,&quot;arp1394.sys&quot;" ResTypeVSGUID="{E66B49F6-4A35-4246-87E8-5C1A468315B5}" BuildTypeMask="819"><PROPERTY Name="DstPath" Format="String">%12%</PROPERTY><PROPERTY Name="DstName" Format="String">arp1394.sys</PROPERTY><PROPERTY Name="SrcFileSize" Format="Integer">0</PROPERTY><PROPERTY Name="NoExpand" Format="Boolean">0</PROPERTY><DISPLAYNAME>ARP1394</DISPLAYNAME><DESCRIPTION>TCP/IP ARP module driver for 1394</DESCRIPTION></RESOURCE><RESOURCE Name="RawDep:&quot;RawFile&quot;,&quot;enum1394.inf&quot;" ResTypeVSGUID="{90D8E195-E710-4af6-B667-B1805FFC9B8F}" BuildTypeMask="819"><PROPERTY Name="RawType" Format="String">File</PROPERTY><PROPERTY Name="Value" Format="String">enum1394.inf</PROPERTY></RESOURCE><RESOURCE Name="RawDep:&quot;RawFile&quot;,&quot;net1394.inf&quot;" ResTypeVSGUID="{90D8E195-E710-4af6-B667-B1805FFC9B8F}" BuildTypeMask="819"><PROPERTY Name="RawType" Format="String">File</PROPERTY><PROPERTY Name="Value" Format="String">net1394.inf</PROPERTY></RESOURCE><RESOURCE Name="RawDep:&quot;RawFile&quot;,&quot;ohci1394.sys&quot;" ResTypeVSGUID="{90D8E195-E710-4af6-B667-B1805FFC9B8F}" BuildTypeMask="819"><PROPERTY Name="RawType" Format="String">File</PROPERTY><PROPERTY Name="Value" Format="String">ohci1394.sys</PROPERTY></RESOURCE><RESOURCE Name="RawDep:&quot;RawFile&quot;,&quot;1394bus.sys&quot;" ResTypeVSGUID="{90D8E195-E710-4af6-B667-B1805FFC9B8F}" BuildTypeMask="819"><PROPERTY Name="RawType" Format="String">File</PROPERTY><PROPERTY Name="Value" Format="String">1394bus.sys</PROPERTY></RESOURCE><RESOURCE Name="RawDep:&quot;RawFile&quot;,&quot;ndis.sys&quot;" ResTypeVSGUID="{90D8E195-E710-4af6-B667-B1805FFC9B8F}" BuildTypeMask="819"><PROPERTY Name="RawType" Format="String">File</PROPERTY><PROPERTY Name="Value" Format="String">ndis.sys</PROPERTY></RESOURCE><RESOURCE Name="RawDep:&quot;RawFile&quot;,&quot;tcpip.sys&quot;" ResTypeVSGUID="{90D8E195-E710-4af6-B667-B1805FFC9B8F}" BuildTypeMask="819"><PROPERTY Name="RawType" Format="String">File</PROPERTY><PROPERTY Name="Value" Format="String">tcpip.sys</PROPERTY></RESOURCE><GROUPMEMBER GroupVSGUID="{388249D2-1897-44ff-86F2-E159A27AA037}"/></COMPONENT></DCARRIER>
