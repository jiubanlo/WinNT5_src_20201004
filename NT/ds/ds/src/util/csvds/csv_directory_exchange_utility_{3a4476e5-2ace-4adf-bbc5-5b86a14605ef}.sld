<?xml version="1.0" encoding="UTF-16"?>
<!DOCTYPE DCARRIER SYSTEM "mantis.dtd">
<DCARRIER CarrierRevision="1">
	<TOOLINFO ToolName="iCat"><![CDATA[<?xml version="1.0"?>
<!DOCTYPE TOOL SYSTEM "tool.dtd">
<TOOL>
	<CREATED><NAME>iCat</NAME><VSGUID>{2c9621d4-253b-4e60-adde-aef1d751c55c}</VSGUID><VERSION>1.0.0.364</VERSION><BUILD>364</BUILD><DATE>9/13/2000</DATE></CREATED><LASTSAVED><NAME>iCat</NAME><VSGUID>{97b86ee0-259c-479f-bc46-6cea7ef4be4d}</VSGUID><VERSION>1.0.0.452</VERSION><BUILD>452</BUILD><DATE>4/27/2001</DATE></LASTSAVED></TOOL>
]]></TOOLINFO><COMPONENT Revision="3" Visibility="1000" MultiInstance="0" Released="1" Editable="1" HTMLFinal="0" ComponentVSGUID="{3A4476E5-2ACE-4ADF-BBC5-5B86A14605EF}" ComponentVIGUID="{F512AA31-97B3-41A8-AC42-74EF8E2482D3}" PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}" RepositoryVSGUID="{8E0BE9ED-7649-47F3-810B-232D36C430B4}"><HELPCONTEXT src="G:\NewNTBug\ds\ds\src\util\csvds\_csv_directory_exchange_utility_component_description.htm">&lt;!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 3.2 Final//EN"&gt;
&lt;HTML DIR="LTR"&gt;&lt;HEAD&gt;
&lt;META HTTP-EQUIV="Content-Type" Content="text/html; charset=Windows-1252"&gt;
&lt;TITLE&gt;CSV Directory Exchange Utility Component Description&lt;/TITLE&gt;
&lt;style type="text/css"&gt;@import url(td.css);&lt;/style&gt;&lt;/HEAD&gt;
&lt;BODY TOPMARGIN="0"&gt;
&lt;H1&gt;&lt;A NAME="_csv_directory_exchange_utility_component_description"&gt;&lt;/A&gt;&lt;SUP&gt;&lt;/SUP&gt;CSV Directory Exchange Utility Component Description&lt;/H1&gt;

&lt;P&gt;The comma-separated value (CSV) Directory Exchange Utility component contains a command-line utility, csvde.exe. Csvde.exe supports the bulk importing and exporting of directory information to and from a Lightweight Directory Access Protocol (LDAP) directory, such as the Microsoft Active Directory. This utility reads and writes data in a CSV file format. It is useful for administrators who need to move large amounts of data from one directory to another. This utility is also useful for viewing directory information in applications, which are capable of loading comma-separated value files, such as spreadsheets or databases, but which are not capable of directly accessing LDAP directories.&lt;/P&gt;

&lt;H1&gt;Component Configuration&lt;/H1&gt;

&lt;P&gt;There are no configuration parameters for this component.&lt;/P&gt;

&lt;H1&gt;Special Notes&lt;/H1&gt;

&lt;P&gt;Besides the file format used, csvde.exe and ldifde.exe differ in the types of operations they support during import. Csvde.exe can only be used to bulk-load the directory by creating new objects. Ldifde.exe can also be used to perform operations on preexisting objects, such as deleting them, modifying their attributes, or renaming them.&lt;/P&gt;

&lt;H1&gt;For More Information&lt;/H1&gt;

&lt;P&gt;Documentation for csvde.exe is available in the Distributed Systems guide of the Server Resource Kit.&lt;/P&gt;

&lt;/BODY&gt;
&lt;/HTML&gt;
</HELPCONTEXT><DISPLAYNAME>CSV Directory Exchange Utility</DISPLAYNAME><VERSION>1.0</VERSION><DESCRIPTION>This utility performs bulk import &amp; export between comma-separated value files and a LDAP directory server</DESCRIPTION><COPYRIGHT>2000 Microsoft Corp.</COPYRIGHT><VENDOR>Microsoft Corp.</VENDOR><OWNERS>mattrim</OWNERS><AUTHORS>mattrim</AUTHORS><DATECREATED>9/13/2000</DATECREATED><DATEREVISED>4/27/2001</DATEREVISED><RESOURCE ResTypeVSGUID="{E66B49F6-4A35-4246-87E8-5C1A468315B5}" BuildTypeMask="819" Name="File:&quot;%11%&quot;,&quot;csvde.exe&quot;"><PROPERTY Name="DstPath" Format="String">%11%</PROPERTY><PROPERTY Name="DstName" Format="String">csvde.exe</PROPERTY><PROPERTY Name="NoExpand" Format="Boolean">0</PROPERTY></RESOURCE><RESOURCE ResTypeVSGUID="{90D8E195-E710-4AF6-B667-B1805FFC9B8F}" BuildTypeMask="819" Name="RawDep:&quot;RawFile&quot;,&quot;MSVCRT.dll&quot;"><PROPERTY Name="RawType" Format="String">File</PROPERTY><PROPERTY Name="Value" Format="String">MSVCRT.dll</PROPERTY></RESOURCE><RESOURCE ResTypeVSGUID="{90D8E195-E710-4AF6-B667-B1805FFC9B8F}" BuildTypeMask="819" Name="RawDep:&quot;RawFile&quot;,&quot;KERNEL32.dll&quot;"><PROPERTY Name="RawType" Format="String">File</PROPERTY><PROPERTY Name="Value" Format="String">KERNEL32.dll</PROPERTY></RESOURCE><RESOURCE ResTypeVSGUID="{90D8E195-E710-4AF6-B667-B1805FFC9B8F}" BuildTypeMask="819" Name="RawDep:&quot;RawFile&quot;,&quot;WLDAP32.dll&quot;"><PROPERTY Name="RawType" Format="String">File</PROPERTY><PROPERTY Name="Value" Format="String">WLDAP32.dll</PROPERTY></RESOURCE><RESOURCE ResTypeVSGUID="{90D8E195-E710-4AF6-B667-B1805FFC9B8F}" BuildTypeMask="819" Name="RawDep:&quot;RawFile&quot;,&quot;NETAPI32.dll&quot;"><PROPERTY Name="RawType" Format="String">File</PROPERTY><PROPERTY Name="Value" Format="String">NETAPI32.dll</PROPERTY></RESOURCE><RESOURCE ResTypeVSGUID="{90D8E195-E710-4AF6-B667-B1805FFC9B8F}" BuildTypeMask="819" Name="RawDep:&quot;RawFile&quot;,&quot;ntdll.dll&quot;"><PROPERTY Name="RawType" Format="String">File</PROPERTY><PROPERTY Name="Value" Format="String">ntdll.dll</PROPERTY></RESOURCE><GROUPMEMBER GroupVSGUID="{E01B4103-3883-4FE8-992F-10566E7B796C}"/><GROUPMEMBER GroupVSGUID="{D8142082-243E-4C8C-B98B-3290C50D93C7}"/></COMPONENT></DCARRIER>
