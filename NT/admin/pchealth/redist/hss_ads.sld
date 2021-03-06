<?xml version="1.0" encoding="UTF-16"?>
<!DOCTYPE DCARRIER SYSTEM "mantis.dtd">
<DCARRIER CarrierRevision="1">
	<TOOLINFO ToolName="iCat"><![CDATA[<?xml version="1.0"?>
<!DOCTYPE TOOL SYSTEM "tool.dtd">
<TOOL>
	<CREATED><NAME>iCat</NAME><VSGUID>{97b86ee0-259c-479f-bc46-6cea7ef4be4d}</VSGUID><VERSION>1.0.0.438</VERSION><BUILD>438</BUILD><DATE>11/30/2000</DATE></CREATED><LASTSAVED><NAME>iCat</NAME><VSGUID>{97b86ee0-259c-479f-bc46-6cea7ef4be4d}</VSGUID><VERSION>1.0.0.452</VERSION><BUILD>452</BUILD><DATE>7/19/2001</DATE></LASTSAVED></TOOL>
]]></TOOLINFO>
	<COMPONENT Revision="22" Visibility="1000" MultiInstance="0" Released="1" Editable="1" HTMLFinal="0" ComponentVSGUID="{A49DF439-D804-49F6-BB5D-C3602A20B7C3}" ComponentVIGUID="{A1142004-3731-4A87-968C-2B58810FD5A2}" PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}" RepositoryVSGUID="{8E0BE9ED-7649-47F3-810B-232D36C430B4}">
		<HELPCONTEXT src="C:\whistler\admin\pchealth\redist\_microsoft_help_and_support_center_component_description.htm">&lt;!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 3.2 Final//EN"&gt;
&lt;HTML DIR="LTR"&gt;&lt;HEAD&gt;
&lt;META HTTP-EQUIV="Content-Type" Content="text/html; charset=Windows-1252"&gt;
&lt;TITLE&gt;Microsoft Help and Support Center Component Description&lt;/TITLE&gt;
&lt;style type="text/css"&gt;@import url(td.css);&lt;/style&gt;&lt;/HEAD&gt;
&lt;BODY TOPMARGIN="0"&gt;
&lt;H1&gt;&lt;A NAME="_microsoft_help_and_support_center_component_description"&gt;&lt;/A&gt;&lt;SUP&gt;&lt;/SUP&gt;Microsoft Help and Support Center Component Description&lt;/H1&gt;

&lt;P&gt;This component provides a comprehensive resource for procedures and overviews, troubleshooters, articles, and tutorials to help users learn and use the Microsoft Windows XP operating system. In addition, it includes several other features, such as Index, Search, History, and Favorites, and access to other utilities such as My Computer Information, System Restore, Online Assisted Support, and more. Help and Support Center is the new Help implementation introduced in Windows XP. It contains information specific to Windows XP, as well as to features that are new to Windows XP, such as Remote Assistance. Other features and programs also provide Help that you can view from within them.&lt;/P&gt;

&lt;H1&gt;Component Configuration&lt;/H1&gt;

&lt;P&gt;There are no configuration requirements for this component. &lt;/P&gt;

&lt;H1&gt;For More Information&lt;/H1&gt;

&lt;P&gt;For more information about this component, see this &lt;A HREF="http://www.microsoft.com/windows/windowsxp"&gt;Microsoft Web site&lt;/A&gt;. &lt;/P&gt;

&lt;/BODY&gt;
&lt;/HTML&gt;
</HELPCONTEXT><DISPLAYNAME>Help and Support Services</DISPLAYNAME>
		<VERSION>1.0</VERSION>
		<DESCRIPTION>Help and Support Services combines an HTML-based application available via Start | Help and Support, with a comprehensive set of content and services, to better enable users to solve problems themselves and get support from others.</DESCRIPTION>
		<COPYRIGHT>2000 Microsoft Corp.</COPYRIGHT>
		<VENDOR>Microsoft Corp.</VENDOR>
		<OWNERS>gschua</OWNERS>
		<AUTHORS>gschua</AUTHORS>
		<DATECREATED>11/30/2000</DATECREATED>
		<DATEREVISED>7/19/2001</DATEREVISED>
		<RESOURCE ResTypeVSGUID="{E66B49F6-4A35-4246-87E8-5C1A468315B5}" BuildTypeMask="819" Name="File(819):&quot;%10%\PCHealth\HelpCtr\Binaries&quot;,&quot;HCAppRes.dll&quot;">
			<PROPERTY Name="DstPath" Format="String">%10%\PCHealth\HelpCtr\Binaries</PROPERTY>
			<PROPERTY Name="DstName" Format="String">HCAppRes.dll</PROPERTY>
			<PROPERTY Name="NoExpand" Format="Boolean">0</PROPERTY>
			<DISPLAYNAME>Resource file for HSS</DISPLAYNAME>
		</RESOURCE>
		<RESOURCE ResTypeVSGUID="{E66B49F6-4A35-4246-87E8-5C1A468315B5}" BuildTypeMask="819" Name="File(819):&quot;%10%\PCHealth\HelpCtr\Binaries&quot;,&quot;HelpCtr.exe&quot;">
			<PROPERTY Name="DstPath" Format="String">%10%\PCHealth\HelpCtr\Binaries</PROPERTY>
			<PROPERTY Name="DstName" Format="String">HelpCtr.exe</PROPERTY>
			<PROPERTY Name="NoExpand" Format="Boolean">0</PROPERTY>
			<DISPLAYNAME>Help Center Executable</DISPLAYNAME>
		</RESOURCE>
		<RESOURCE ResTypeVSGUID="{E66B49F6-4A35-4246-87E8-5C1A468315B5}" BuildTypeMask="819" Name="File(819):&quot;%10%\PCHealth\HelpCtr\Binaries&quot;,&quot;HelpHost.exe&quot;">
			<PROPERTY Name="DstPath" Format="String">%10%\PCHealth\HelpCtr\Binaries</PROPERTY>
			<PROPERTY Name="DstName" Format="String">HelpHost.exe</PROPERTY>
			<PROPERTY Name="NoExpand" Format="Boolean">0</PROPERTY>
			<DISPLAYNAME>Help Host</DISPLAYNAME>
		</RESOURCE>
		<RESOURCE ResTypeVSGUID="{E66B49F6-4A35-4246-87E8-5C1A468315B5}" BuildTypeMask="819" Name="File(819):&quot;%10%\PCHealth\HelpCtr\Binaries&quot;,&quot;HelpSvc.exe&quot;">
			<PROPERTY Name="DstPath" Format="String">%10%\PCHealth\HelpCtr\Binaries</PROPERTY>
			<PROPERTY Name="DstName" Format="String">HelpSvc.exe</PROPERTY>
			<PROPERTY Name="NoExpand" Format="Boolean">0</PROPERTY>
			<DISPLAYNAME>Help Service</DISPLAYNAME>
		</RESOURCE>
		<RESOURCE ResTypeVSGUID="{E66B49F6-4A35-4246-87E8-5C1A468315B5}" BuildTypeMask="819" Name="File(819):&quot;%10%\PCHealth\HelpCtr\Binaries&quot;,&quot;brpinfo.dll&quot;">
			<PROPERTY Name="DstPath" Format="String">%10%\PCHealth\HelpCtr\Binaries</PROPERTY>
			<PROPERTY Name="DstName" Format="String">brpinfo.dll</PROPERTY>
			<PROPERTY Name="NoExpand" Format="Boolean">0</PROPERTY>
			<DISPLAYNAME>Bug Report</DISPLAYNAME>
		</RESOURCE>
		<RESOURCE ResTypeVSGUID="{E66B49F6-4A35-4246-87E8-5C1A468315B5}" BuildTypeMask="819" Name="File(819):&quot;%10%\PCHealth\HelpCtr\Binaries&quot;,&quot;msinfo.dll&quot;">
			<PROPERTY Name="DstPath" Format="String">%10%\PCHealth\HelpCtr\Binaries</PROPERTY>
			<PROPERTY Name="DstName" Format="String">msinfo.dll</PROPERTY>
			<PROPERTY Name="NoExpand" Format="Boolean">0</PROPERTY>
			<DISPLAYNAME>MS Info</DISPLAYNAME>
		</RESOURCE>
		<RESOURCE ResTypeVSGUID="{E66B49F6-4A35-4246-87E8-5C1A468315B5}" BuildTypeMask="819" Name="File(819):&quot;%10%\PCHealth\HelpCtr\Binaries&quot;,&quot;pchdt_p3.cab&quot;">
			<PROPERTY Name="DstPath" Format="String">%10%\PCHealth\HelpCtr\Binaries</PROPERTY>
			<PROPERTY Name="DstName" Format="String">pchdt_e3.cab</PROPERTY>
			<PROPERTY Name="NoExpand" Format="Boolean">0</PROPERTY>
			<DISPLAYNAME>HSS Datafile</DISPLAYNAME>
		</RESOURCE>
		<RESOURCE ResTypeVSGUID="{E66B49F6-4A35-4246-87E8-5C1A468315B5}" BuildTypeMask="819" Name="File(819):&quot;%10%\PCHealth\HelpCtr\Binaries&quot;,&quot;pchshell.dll&quot;">
			<PROPERTY Name="DstPath" Format="String">%10%\PCHealth\HelpCtr\Binaries</PROPERTY>
			<PROPERTY Name="DstName" Format="String">pchshell.dll</PROPERTY>
			<PROPERTY Name="NoExpand" Format="Boolean">0</PROPERTY>
			<DISPLAYNAME>Shell UI</DISPLAYNAME>
		</RESOURCE>
		<RESOURCE ResTypeVSGUID="{E66B49F6-4A35-4246-87E8-5C1A468315B5}" BuildTypeMask="819" Name="File(819):&quot;%10%\PCHealth\HelpCtr\Binaries&quot;,&quot;pchsvc.dll&quot;">
			<PROPERTY Name="DstPath" Format="String">%10%\PCHealth\HelpCtr\Binaries</PROPERTY>
			<PROPERTY Name="DstName" Format="String">pchsvc.dll</PROPERTY>
			<PROPERTY Name="NoExpand" Format="Boolean">0</PROPERTY>
			<DISPLAYNAME>NT Service</DISPLAYNAME>
		</RESOURCE>
		<RESOURCE ResTypeVSGUID="{E66B49F6-4A35-4246-87E8-5C1A468315B5}" BuildTypeMask="819" Name="File(819):&quot;%11%&quot;,&quot;atrace.dll&quot;">
			<PROPERTY Name="DstPath" Format="String">%11%</PROPERTY>
			<PROPERTY Name="DstName" Format="String">atrace.dll</PROPERTY>
			<PROPERTY Name="NoExpand" Format="Boolean">0</PROPERTY>
			<DISPLAYNAME>HSS Trace Library</DISPLAYNAME>
		</RESOURCE>
		<RESOURCE ResTypeVSGUID="{E66B49F6-4A35-4246-87E8-5C1A468315B5}" BuildTypeMask="819" Name="File(819):&quot;%10%\PCHealth\UploadLB\Binaries&quot;,&quot;UploadM.exe&quot;">
			<PROPERTY Name="DstPath" Format="String">%10%\PCHealth\UploadLB\Binaries</PROPERTY>
			<PROPERTY Name="DstName" Format="String">UploadM.exe</PROPERTY>
			<PROPERTY Name="NoExpand" Format="Boolean">0</PROPERTY>
			<DISPLAYNAME>Upload Library Binary</DISPLAYNAME>
		</RESOURCE>
		<RESOURCE ResTypeVSGUID="{90D8E195-E710-4AF6-B667-B1805FFC9B8F}" BuildTypeMask="819" Name="RawDep(819):&quot;File&quot;,&quot;MSVCRT.dll&quot;">
			<PROPERTY Name="RawType" Format="String">File</PROPERTY>
			<PROPERTY Name="Value" Format="String">MSVCRT.dll</PROPERTY>
		</RESOURCE>
		<RESOURCE ResTypeVSGUID="{90D8E195-E710-4AF6-B667-B1805FFC9B8F}" BuildTypeMask="819" Name="RawDep(819):&quot;File&quot;,&quot;ADVAPI32.dll&quot;">
			<PROPERTY Name="RawType" Format="String">File</PROPERTY>
			<PROPERTY Name="Value" Format="String">ADVAPI32.dll</PROPERTY>
		</RESOURCE>
		<RESOURCE ResTypeVSGUID="{90D8E195-E710-4AF6-B667-B1805FFC9B8F}" BuildTypeMask="819" Name="RawDep(819):&quot;File&quot;,&quot;KERNEL32.dll&quot;">
			<PROPERTY Name="RawType" Format="String">File</PROPERTY>
			<PROPERTY Name="Value" Format="String">KERNEL32.dll</PROPERTY>
		</RESOURCE>
		<RESOURCE ResTypeVSGUID="{90D8E195-E710-4AF6-B667-B1805FFC9B8F}" BuildTypeMask="819" Name="RawDep(819):&quot;File&quot;,&quot;USER32.dll&quot;">
			<PROPERTY Name="RawType" Format="String">File</PROPERTY>
			<PROPERTY Name="Value" Format="String">USER32.dll</PROPERTY>
		</RESOURCE>
		<RESOURCE ResTypeVSGUID="{90D8E195-E710-4AF6-B667-B1805FFC9B8F}" BuildTypeMask="819" Name="RawDep(819):&quot;File&quot;,&quot;ole32.dll&quot;">
			<PROPERTY Name="RawType" Format="String">File</PROPERTY>
			<PROPERTY Name="Value" Format="String">ole32.dll</PROPERTY>
		</RESOURCE>
		<RESOURCE ResTypeVSGUID="{90D8E195-E710-4AF6-B667-B1805FFC9B8F}" BuildTypeMask="819" Name="RawDep(819):&quot;File&quot;,&quot;OLEAUT32.dll&quot;">
			<PROPERTY Name="RawType" Format="String">File</PROPERTY>
			<PROPERTY Name="Value" Format="String">OLEAUT32.dll</PROPERTY>
		</RESOURCE>
		<RESOURCE ResTypeVSGUID="{90D8E195-E710-4AF6-B667-B1805FFC9B8F}" BuildTypeMask="819" Name="RawDep(819):&quot;File&quot;,&quot;Secur32.dll&quot;">
			<PROPERTY Name="RawType" Format="String">File</PROPERTY>
			<PROPERTY Name="Value" Format="String">Secur32.dll</PROPERTY>
		</RESOURCE>
		<RESOURCE ResTypeVSGUID="{90D8E195-E710-4AF6-B667-B1805FFC9B8F}" BuildTypeMask="819" Name="RawDep(819):&quot;File&quot;,&quot;WTSAPI32.dll&quot;">
			<PROPERTY Name="RawType" Format="String">File</PROPERTY>
			<PROPERTY Name="Value" Format="String">WTSAPI32.dll</PROPERTY>
		</RESOURCE>
		<RESOURCE ResTypeVSGUID="{90D8E195-E710-4AF6-B667-B1805FFC9B8F}" BuildTypeMask="819" Name="RawDep(819):&quot;File&quot;,&quot;WINSTA.dll&quot;">
			<PROPERTY Name="RawType" Format="String">File</PROPERTY>
			<PROPERTY Name="Value" Format="String">WINSTA.dll</PROPERTY>
		</RESOURCE>
		<RESOURCE ResTypeVSGUID="{90D8E195-E710-4AF6-B667-B1805FFC9B8F}" BuildTypeMask="819" Name="RawDep(819):&quot;File&quot;,&quot;ESENT.dll&quot;">
			<PROPERTY Name="RawType" Format="String">File</PROPERTY>
			<PROPERTY Name="Value" Format="String">ESENT.dll</PROPERTY>
		</RESOURCE>
		<RESOURCE ResTypeVSGUID="{90D8E195-E710-4AF6-B667-B1805FFC9B8F}" BuildTypeMask="819" Name="RawDep(819):&quot;File&quot;,&quot;WINTRUST.dll&quot;">
			<PROPERTY Name="RawType" Format="String">File</PROPERTY>
			<PROPERTY Name="Value" Format="String">WINTRUST.dll</PROPERTY>
		</RESOURCE>
		<RESOURCE ResTypeVSGUID="{90D8E195-E710-4AF6-B667-B1805FFC9B8F}" BuildTypeMask="819" Name="RawDep(819):&quot;File&quot;,&quot;CRYPT32.dll&quot;">
			<PROPERTY Name="RawType" Format="String">File</PROPERTY>
			<PROPERTY Name="Value" Format="String">CRYPT32.dll</PROPERTY>
		</RESOURCE>
		<RESOURCE ResTypeVSGUID="{90D8E195-E710-4AF6-B667-B1805FFC9B8F}" BuildTypeMask="819" Name="RawDep(819):&quot;File&quot;,&quot;SHLWAPI.dll&quot;">
			<PROPERTY Name="RawType" Format="String">File</PROPERTY>
			<PROPERTY Name="Value" Format="String">SHLWAPI.dll</PROPERTY>
		</RESOURCE>
		<RESOURCE ResTypeVSGUID="{90D8E195-E710-4AF6-B667-B1805FFC9B8F}" BuildTypeMask="819" Name="RawDep(819):&quot;File&quot;,&quot;WININET.dll&quot;">
			<PROPERTY Name="RawType" Format="String">File</PROPERTY>
			<PROPERTY Name="Value" Format="String">WININET.dll</PROPERTY>
		</RESOURCE>
		<RESOURCE ResTypeVSGUID="{90D8E195-E710-4AF6-B667-B1805FFC9B8F}" BuildTypeMask="819" Name="RawDep(819):&quot;File&quot;,&quot;Cabinet.dll&quot;">
			<PROPERTY Name="RawType" Format="String">File</PROPERTY>
			<PROPERTY Name="Value" Format="String">Cabinet.dll</PROPERTY>
		</RESOURCE>
		<RESOURCE ResTypeVSGUID="{90D8E195-E710-4AF6-B667-B1805FFC9B8F}" BuildTypeMask="819" Name="RawDep(819):&quot;File&quot;,&quot;PSAPI.DLL&quot;">
			<PROPERTY Name="RawType" Format="String">File</PROPERTY>
			<PROPERTY Name="Value" Format="String">PSAPI.DLL</PROPERTY>
		</RESOURCE>
		<RESOURCE ResTypeVSGUID="{90D8E195-E710-4AF6-B667-B1805FFC9B8F}" BuildTypeMask="819" Name="RawDep(819):&quot;File&quot;,&quot;NETAPI32.dll&quot;">
			<PROPERTY Name="RawType" Format="String">File</PROPERTY>
			<PROPERTY Name="Value" Format="String">NETAPI32.dll</PROPERTY>
		</RESOURCE>
		<RESOURCE ResTypeVSGUID="{90D8E195-E710-4AF6-B667-B1805FFC9B8F}" BuildTypeMask="819" Name="RawDep(819):&quot;File&quot;,&quot;GDI32.dll&quot;">
			<PROPERTY Name="RawType" Format="String">File</PROPERTY>
			<PROPERTY Name="Value" Format="String">GDI32.dll</PROPERTY>
		</RESOURCE>
		<RESOURCE ResTypeVSGUID="{90D8E195-E710-4AF6-B667-B1805FFC9B8F}" BuildTypeMask="819" Name="RawDep(819):&quot;File&quot;,&quot;urlmon.dll&quot;">
			<PROPERTY Name="RawType" Format="String">File</PROPERTY>
			<PROPERTY Name="Value" Format="String">urlmon.dll</PROPERTY>
		</RESOURCE>
		<RESOURCE ResTypeVSGUID="{90D8E195-E710-4AF6-B667-B1805FFC9B8F}" BuildTypeMask="819" Name="RawDep(819):&quot;File&quot;,&quot;SHELL32.dll&quot;">
			<PROPERTY Name="RawType" Format="String">File</PROPERTY>
			<PROPERTY Name="Value" Format="String">SHELL32.dll</PROPERTY>
		</RESOURCE>
		<RESOURCE ResTypeVSGUID="{90D8E195-E710-4AF6-B667-B1805FFC9B8F}" BuildTypeMask="819" Name="RawDep(819):&quot;File&quot;,&quot;WINSPOOL.DRV&quot;">
			<PROPERTY Name="RawType" Format="String">File</PROPERTY>
			<PROPERTY Name="Value" Format="String">WINSPOOL.DRV</PROPERTY>
		</RESOURCE>
		<RESOURCE ResTypeVSGUID="{90D8E195-E710-4AF6-B667-B1805FFC9B8F}" BuildTypeMask="819" Name="RawDep(819):&quot;File&quot;,&quot;USERENV.dll&quot;">
			<PROPERTY Name="RawType" Format="String">File</PROPERTY>
			<PROPERTY Name="Value" Format="String">USERENV.dll</PROPERTY>
		</RESOURCE>
		<RESOURCE ResTypeVSGUID="{90D8E195-E710-4AF6-B667-B1805FFC9B8F}" BuildTypeMask="819" Name="RawDep(819):&quot;File&quot;,&quot;COMCTL32.dll&quot;">
			<PROPERTY Name="RawType" Format="String">File</PROPERTY>
			<PROPERTY Name="Value" Format="String">COMCTL32.dll</PROPERTY>
		</RESOURCE>
		<RESOURCE ResTypeVSGUID="{90D8E195-E710-4AF6-B667-B1805FFC9B8F}" BuildTypeMask="819" Name="RawDep(819):&quot;File&quot;,&quot;HLINK.dll&quot;">
			<PROPERTY Name="RawType" Format="String">File</PROPERTY>
			<PROPERTY Name="Value" Format="String">HLINK.dll</PROPERTY>
		</RESOURCE>
		<RESOURCE ResTypeVSGUID="{2C10DB69-39AB-48A4-A83F-9AB3ACBA7C45}" BuildTypeMask="819" Name="RegKey(819):&quot;HKEY_CURRENT_USER\Control Panel\Desktop&quot;,&quot;LameButtonEnabled&quot;" Localize="0">
			<PROPERTY Name="KeyPath" Format="String">HKEY_CURRENT_USER\Control Panel\Desktop</PROPERTY>
			<PROPERTY Name="ValueName" Format="String">LameButtonEnabled</PROPERTY>
			<PROPERTY Name="RegValue" Format="Integer">8</PROPERTY>
			<PROPERTY Name="RegType" Format="Integer">4</PROPERTY>
			<PROPERTY Name="RegOp" Format="Integer">1</PROPERTY>
			<PROPERTY Name="RegCond" Format="Integer">1</PROPERTY>
			<DISPLAYNAME>Help Comments Display Parameter</DISPLAYNAME>
			<DESCRIPTION>Registry settings to enable the Comments link</DESCRIPTION>
		</RESOURCE>
		<RESOURCE ResTypeVSGUID="{2C10DB69-39AB-48A4-A83F-9AB3ACBA7C45}" BuildTypeMask="819" Name="RegKey(819):&quot;HKEY_CURRENT_USER\Control Panel\Desktop&quot;,&quot;LameButtonText&quot;" Localize="1">
			<PROPERTY Name="KeyPath" Format="String">HKEY_CURRENT_USER\Control Panel\Desktop</PROPERTY>
			<PROPERTY Name="ValueName" Format="String">LameButtonText</PROPERTY>
			<PROPERTY Name="RegValue" Format="String">Comments?</PROPERTY>
			<PROPERTY Name="RegType" Format="Integer">1</PROPERTY>
			<PROPERTY Name="RegOp" Format="Integer">1</PROPERTY>
			<PROPERTY Name="RegCond" Format="Integer">1</PROPERTY>
			<DISPLAYNAME>Help Comments</DISPLAYNAME>
			<DESCRIPTION>Registry settings to enable the Comments link</DESCRIPTION>
		</RESOURCE>
		<RESOURCE ResTypeVSGUID="{2C10DB69-39AB-48A4-A83F-9AB3ACBA7C45}" BuildTypeMask="819" Name="RegKey(819):&quot;HKEY_LOCAL_MACHINE\Software\Microsoft\Windows NT\CurrentVersion\Svchost&quot;,&quot;PCHealth&quot;" Localize="0">
			<PROPERTY Name="KeyPath" Format="String">HKEY_LOCAL_MACHINE\Software\Microsoft\Windows NT\CurrentVersion\Svchost</PROPERTY>
			<PROPERTY Name="ValueName" Format="String">PCHealth</PROPERTY>
			<PROPERTY Name="RegValue" Format="Multi">480065006C0070005300760063000000550070006C006F00610064004D00670072000000</PROPERTY>
			<PROPERTY Name="RegType" Format="Integer">7</PROPERTY>
			<PROPERTY Name="RegOp" Format="Integer">1</PROPERTY>
			<PROPERTY Name="RegCond" Format="Integer">1</PROPERTY>
			<DISPLAYNAME>Service Host</DISPLAYNAME></RESOURCE>
		<RESOURCE ResTypeVSGUID="{2C10DB69-39AB-48A4-A83F-9AB3ACBA7C45}" BuildTypeMask="819" Name="RegKey(819):&quot;HKEY_LOCAL_MACHINE\Software\Microsoft\Windows NT\CurrentVersion\Svchost\PCHealth&quot;,&quot;CoInitializeSecurityParam&quot;" Localize="0">
			<PROPERTY Name="KeyPath" Format="String">HKEY_LOCAL_MACHINE\Software\Microsoft\Windows NT\CurrentVersion\Svchost\PCHealth</PROPERTY>
			<PROPERTY Name="ValueName" Format="String">CoInitializeSecurityParam</PROPERTY>
			<PROPERTY Name="RegValue" Format="Integer">2</PROPERTY>
			<PROPERTY Name="RegType" Format="Integer">4</PROPERTY>
			<PROPERTY Name="RegOp" Format="Integer">1</PROPERTY>
			<PROPERTY Name="RegCond" Format="Integer">3</PROPERTY>
			<DISPLAYNAME>Security Parameter</DISPLAYNAME></RESOURCE>
		<RESOURCE ResTypeVSGUID="{2C10DB69-39AB-48A4-A83F-9AB3ACBA7C45}" BuildTypeMask="819" Name="RegKey(819):&quot;HKEY_LOCAL_MACHINE\Software\Microsoft\Windows NT\CurrentVersion\Svchost\PCHealth&quot;,&quot;AuthenticationCapabilities&quot;" Localize="0">
			<PROPERTY Name="KeyPath" Format="String">HKEY_LOCAL_MACHINE\Software\Microsoft\Windows NT\CurrentVersion\Svchost\PCHealth</PROPERTY>
			<PROPERTY Name="ValueName" Format="String">AuthenticationCapabilities</PROPERTY>
			<PROPERTY Name="RegValue" Format="Integer">64</PROPERTY>
			<PROPERTY Name="RegType" Format="Integer">4</PROPERTY>
			<PROPERTY Name="RegOp" Format="Integer">1</PROPERTY>
			<PROPERTY Name="RegCond" Format="Integer">3</PROPERTY>
			<DISPLAYNAME>Authentication Capability value</DISPLAYNAME></RESOURCE>
		<RESOURCE ResTypeVSGUID="{2C10DB69-39AB-48A4-A83F-9AB3ACBA7C45}" BuildTypeMask="819" Name="RegKey(819):&quot;HKEY_LOCAL_MACHINE\Software\Microsoft\Windows\CurrentVersion\App Paths\HELPCTR.EXE&quot;,&quot;&quot;" Localize="0">
			<PROPERTY Name="KeyPath" Format="String">HKEY_LOCAL_MACHINE\Software\Microsoft\Windows\CurrentVersion\App Paths\HELPCTR.EXE</PROPERTY>
			<PROPERTY Name="ValueName" Format="String"/>
			<PROPERTY Name="RegValue" Format="String">%SystemRoot%\PCHealth\HelpCtr\Binaries\HelpCtr.exe</PROPERTY>
			<PROPERTY Name="RegType" Format="Integer">2</PROPERTY>
			<PROPERTY Name="RegOp" Format="Integer">1</PROPERTY>
			<PROPERTY Name="RegCond" Format="Integer">1</PROPERTY>
			<DISPLAYNAME>Application Path for Help Center</DISPLAYNAME></RESOURCE>
		<RESOURCE ResTypeVSGUID="{2C10DB69-39AB-48A4-A83F-9AB3ACBA7C45}" BuildTypeMask="819" Name="RegKey(819):&quot;HKEY_LOCAL_MACHINE\System\CurrentControlSet\Control\SafeBoot\Minimal\HelpSvc&quot;,&quot;&quot;" Localize="0">
			<PROPERTY Name="KeyPath" Format="String">HKEY_LOCAL_MACHINE\System\CurrentControlSet\Control\SafeBoot\Minimal\HelpSvc</PROPERTY>
			<PROPERTY Name="ValueName" Format="String"/>
			<PROPERTY Name="RegValue" Format="String">Service</PROPERTY>
			<PROPERTY Name="RegType" Format="Integer">1</PROPERTY>
			<PROPERTY Name="RegOp" Format="Integer">1</PROPERTY>
			<PROPERTY Name="RegCond" Format="Integer">1</PROPERTY>
			<DISPLAYNAME>Safe Boot for Minimal Help Service</DISPLAYNAME></RESOURCE>
		<RESOURCE ResTypeVSGUID="{2C10DB69-39AB-48A4-A83F-9AB3ACBA7C45}" BuildTypeMask="819" Name="RegKey(819):&quot;HKEY_LOCAL_MACHINE\System\CurrentControlSet\Control\SafeBoot\Network\HelpSvc&quot;,&quot;&quot;" Localize="0">
			<PROPERTY Name="KeyPath" Format="String">HKEY_LOCAL_MACHINE\System\CurrentControlSet\Control\SafeBoot\Network\HelpSvc</PROPERTY>
			<PROPERTY Name="ValueName" Format="String"/>
			<PROPERTY Name="RegValue" Format="String">Service</PROPERTY>
			<PROPERTY Name="RegType" Format="Integer">1</PROPERTY>
			<PROPERTY Name="RegOp" Format="Integer">1</PROPERTY>
			<PROPERTY Name="RegCond" Format="Integer">1</PROPERTY>
			<DISPLAYNAME>Safe Boot for Network Help Service</DISPLAYNAME></RESOURCE>
		<RESOURCE ResTypeVSGUID="{2C10DB69-39AB-48A4-A83F-9AB3ACBA7C45}" BuildTypeMask="819" Name="RegKey(819):&quot;HKEY_LOCAL_MACHINE\System\CurrentControlSet\Control\SafeBoot\Network\UploadMgr&quot;,&quot;&quot;" Localize="0">
			<PROPERTY Name="KeyPath" Format="String">HKEY_LOCAL_MACHINE\System\CurrentControlSet\Control\SafeBoot\Network\UploadMgr</PROPERTY>
			<PROPERTY Name="ValueName" Format="String"/>
			<PROPERTY Name="RegValue" Format="String">Service</PROPERTY>
			<PROPERTY Name="RegType" Format="Integer">1</PROPERTY>
			<PROPERTY Name="RegOp" Format="Integer">1</PROPERTY>
			<PROPERTY Name="RegCond" Format="Integer">1</PROPERTY>
			<DISPLAYNAME>Safe Boot for Upload Manager</DISPLAYNAME></RESOURCE>
		<RESOURCE ResTypeVSGUID="{322D2CA9-219E-4380-989B-12E8A830DFFA}" BuildTypeMask="819" Name="FBRegDLL(819):&quot;%10%\PCHealth\HelpCtr\Binaries\HelpSvc.exe&quot;" Localize="0">
			<PROPERTY Name="Arguments" Format="String">/install /svchost netsvcs /regserver</PROPERTY><PROPERTY Name="DLLEntryPoint" Format="String"></PROPERTY><PROPERTY Name="DLLInstall" Format="Boolean">False</PROPERTY><PROPERTY Name="DLLRegister" Format="Boolean">False</PROPERTY><PROPERTY Name="FilePath" Format="String">%10%\PCHealth\HelpCtr\Binaries\HelpSvc.exe</PROPERTY><PROPERTY Name="Flags" Format="Integer">0</PROPERTY><PROPERTY Name="Timeout" Format="Integer">0</PROPERTY><DISPLAYNAME>Help Service</DISPLAYNAME>
		</RESOURCE>
		<RESOURCE ResTypeVSGUID="{322D2CA9-219E-4380-989B-12E8A830DFFA}" BuildTypeMask="819" Name="FBRegDLL(819):&quot;%10%\PCHealth\UploadLB\Binaries\UploadM.exe&quot;" Localize="0">
			<PROPERTY Name="Arguments" Format="String">/svchost netsvcs /regserver</PROPERTY><PROPERTY Name="DLLEntryPoint" Format="String"></PROPERTY><PROPERTY Name="DLLInstall" Format="Boolean">False</PROPERTY><PROPERTY Name="DLLRegister" Format="Boolean">False</PROPERTY><PROPERTY Name="FilePath" Format="String">%10%\PCHealth\UploadLB\Binaries\UploadM.exe</PROPERTY><PROPERTY Name="Flags" Format="Integer">0</PROPERTY><PROPERTY Name="Timeout" Format="Integer">0</PROPERTY><DISPLAYNAME>Upload Manager</DISPLAYNAME>
		</RESOURCE>
		<RESOURCE ResTypeVSGUID="{322D2CA9-219E-4380-989B-12E8A830DFFA}" BuildTypeMask="819" Name="FBRegDLL(819):&quot;%10%\PCHealth\HelpCtr\Binaries\HelpHost.exe&quot;" Localize="0">
			<PROPERTY Name="Arguments" Format="String">/regserver</PROPERTY><PROPERTY Name="DLLEntryPoint" Format="String"></PROPERTY><PROPERTY Name="DLLInstall" Format="Boolean">False</PROPERTY><PROPERTY Name="DLLRegister" Format="Boolean">False</PROPERTY><PROPERTY Name="FilePath" Format="String">%10%\PCHealth\HelpCtr\Binaries\HelpHost.exe</PROPERTY><PROPERTY Name="Flags" Format="Integer">0</PROPERTY><PROPERTY Name="Timeout" Format="Integer">0</PROPERTY><DISPLAYNAME>Help Host</DISPLAYNAME>
		</RESOURCE>
		<RESOURCE ResTypeVSGUID="{322D2CA9-219E-4380-989B-12E8A830DFFA}" BuildTypeMask="819" Name="FBRegDLL(819):&quot;%10%\PCHealth\HelpCtr\Binaries\HelpCtr.exe&quot;" Localize="0">
			<PROPERTY Name="Arguments" Format="String">/regserver</PROPERTY><PROPERTY Name="DLLEntryPoint" Format="String"></PROPERTY><PROPERTY Name="DLLInstall" Format="Boolean">False</PROPERTY><PROPERTY Name="DLLRegister" Format="Boolean">False</PROPERTY><PROPERTY Name="FilePath" Format="String">%10%\PCHealth\HelpCtr\Binaries\HelpCtr.exe</PROPERTY><PROPERTY Name="Flags" Format="Integer">0</PROPERTY><PROPERTY Name="Timeout" Format="Integer">0</PROPERTY><DISPLAYNAME>Help Center</DISPLAYNAME>
		</RESOURCE>
		<RESOURCE ResTypeVSGUID="{322D2CA9-219E-4380-989B-12E8A830DFFA}" BuildTypeMask="819" Name="FBRegDLL(819):&quot;%10%\PCHealth\HelpCtr\Binaries\brpinfo.dll&quot;" Localize="0">
			<PROPERTY Name="Arguments" Format="String"></PROPERTY><PROPERTY Name="DLLEntryPoint" Format="String"></PROPERTY><PROPERTY Name="DLLInstall" Format="Boolean">False</PROPERTY><PROPERTY Name="DLLRegister" Format="Boolean">True</PROPERTY><PROPERTY Name="FilePath" Format="String">%10%\PCHealth\HelpCtr\Binaries\brpinfo.dll</PROPERTY><PROPERTY Name="Flags" Format="Integer">0</PROPERTY><PROPERTY Name="Timeout" Format="Integer">0</PROPERTY><DISPLAYNAME>Bug Report</DISPLAYNAME>
		</RESOURCE>
		<RESOURCE ResTypeVSGUID="{322D2CA9-219E-4380-989B-12E8A830DFFA}" BuildTypeMask="819" Name="FBRegDLL(819):&quot;%10%\PCHealth\HelpCtr\Binaries\msinfo.dll&quot;" Localize="0">
			<PROPERTY Name="Arguments" Format="String"></PROPERTY><PROPERTY Name="DLLEntryPoint" Format="String"></PROPERTY><PROPERTY Name="DLLInstall" Format="Boolean">False</PROPERTY><PROPERTY Name="DLLRegister" Format="Boolean">True</PROPERTY><PROPERTY Name="FilePath" Format="String">%10%\PCHealth\HelpCtr\Binaries\msinfo.dll</PROPERTY><PROPERTY Name="Flags" Format="Integer">0</PROPERTY><PROPERTY Name="Timeout" Format="Integer">0</PROPERTY><DISPLAYNAME>MS Info</DISPLAYNAME>
		</RESOURCE>
		<RESOURCE ResTypeVSGUID="{90D8E195-E710-4AF6-B667-B1805FFC9B8F}" BuildTypeMask="819" Name="RawDep(819):&quot;File&quot;,&quot;MSIMG32.dll&quot;"><PROPERTY Name="RawType" Format="String">File</PROPERTY><PROPERTY Name="Value" Format="String">MSIMG32.dll</PROPERTY></RESOURCE><RESOURCE ResTypeVSGUID="{90D8E195-E710-4AF6-B667-B1805FFC9B8F}" BuildTypeMask="819" Name="RawDep(819):&quot;File&quot;,&quot;RPCRT4.dll&quot;"><PROPERTY Name="RawType" Format="String">File</PROPERTY><PROPERTY Name="Value" Format="String">RPCRT4.dll</PROPERTY></RESOURCE><RESOURCE ResTypeVSGUID="{90D8E195-E710-4AF6-B667-B1805FFC9B8F}" BuildTypeMask="819" Name="RawDep(819):&quot;File&quot;,&quot;hhctrl.ocx&quot;"><PROPERTY Name="RawType" Format="String">File</PROPERTY><PROPERTY Name="Value" Format="String">hhctrl.ocx</PROPERTY></RESOURCE><RESOURCE ResTypeVSGUID="{90D8E195-E710-4AF6-B667-B1805FFC9B8F}" BuildTypeMask="819" Name="RawDep(819):&quot;File&quot;,&quot;SETUPAPI.DLL&quot;"><PROPERTY Name="RawType" Format="String">File</PROPERTY><PROPERTY Name="Value" Format="String">SETUPAPI.DLL </PROPERTY></RESOURCE><RESOURCE ResTypeVSGUID="{90D8E195-E710-4AF6-B667-B1805FFC9B8F}" BuildTypeMask="819" Name="RawDep(819):&quot;File&quot;,&quot;UxTheme.dll&quot;"><PROPERTY Name="RawType" Format="String">File</PROPERTY><PROPERTY Name="Value" Format="String">UxTheme.dll</PROPERTY></RESOURCE><RESOURCE ResTypeVSGUID="{90D8E195-E710-4AF6-B667-B1805FFC9B8F}" BuildTypeMask="819" Name="RawDep(819):&quot;File&quot;,&quot;comdlg32.dll&quot;"><PROPERTY Name="RawType" Format="String">File</PROPERTY><PROPERTY Name="Value" Format="String">comdlg32.dll</PROPERTY></RESOURCE><RESOURCE ResTypeVSGUID="{90D8E195-E710-4AF6-B667-B1805FFC9B8F}" BuildTypeMask="819" Name="RawDep(819):&quot;File&quot;,&quot;RASAPI32.dll&quot;"><PROPERTY Name="RawType" Format="String">File</PROPERTY><PROPERTY Name="Value" Format="String">RASAPI32.dll</PROPERTY></RESOURCE><RESOURCE ResTypeVSGUID="{90D8E195-E710-4AF6-B667-B1805FFC9B8F}" BuildTypeMask="819" Name="RawDep(819):&quot;File&quot;,&quot;ntdll.dll&quot;"><PROPERTY Name="RawType" Format="String">File</PROPERTY><PROPERTY Name="Value" Format="String">ntdll.dll</PROPERTY></RESOURCE><RESOURCE ResTypeVSGUID="{E66B49F6-4A35-4246-87E8-5C1A468315B5}" BuildTypeMask="819" Name="File(819):&quot;%10%\pchealth\helpctr\binaries&quot;,&quot;notiflag.exe&quot;"><PROPERTY Name="DstPath" Format="String">%10%\pchealth\helpctr\binaries</PROPERTY><PROPERTY Name="DstName" Format="String">notiflag.exe</PROPERTY><PROPERTY Name="NoExpand" Format="Boolean">0</PROPERTY><DISPLAYNAME>Notification Flag App</DISPLAYNAME></RESOURCE><RESOURCE ResTypeVSGUID="{90D8E195-E710-4AF6-B667-B1805FFC9B8F}" BuildTypeMask="819" Name="RawDep(819):&quot;File&quot;,&quot;REGSVR32.EXE&quot;"><PROPERTY Name="RawType" Format="String">File</PROPERTY><PROPERTY Name="Value" Format="String">REGSVR32.EXE</PROPERTY></RESOURCE><RESOURCE ResTypeVSGUID="{90D8E195-E710-4AF6-B667-B1805FFC9B8F}" BuildTypeMask="819" Name="RawDep(819):&quot;File&quot;,&quot;NET.EXE&quot;"><PROPERTY Name="RawType" Format="String">File</PROPERTY><PROPERTY Name="Value" Format="String">NET.EXE</PROPERTY></RESOURCE><RESOURCE ResTypeVSGUID="{90D8E195-E710-4AF6-B667-B1805FFC9B8F}" BuildTypeMask="819" Name="RawDep(819):&quot;File&quot;,&quot;MSHTML.DLL&quot;"><PROPERTY Name="RawType" Format="String">File</PROPERTY><PROPERTY Name="Value" Format="String">MSHTML.DLL</PROPERTY></RESOURCE><RESOURCE ResTypeVSGUID="{90D8E195-E710-4AF6-B667-B1805FFC9B8F}" BuildTypeMask="819" Name="RawDep(819):&quot;File&quot;,&quot;IEINFO5.OCX&quot;"><PROPERTY Name="RawType" Format="String">File</PROPERTY><PROPERTY Name="Value" Format="String">IEINFO5.OCX</PROPERTY></RESOURCE><RESOURCE ResTypeVSGUID="{90D8E195-E710-4AF6-B667-B1805FFC9B8F}" BuildTypeMask="819" Name="RawDep(819):&quot;File&quot;,&quot;OLEACC.DLL&quot;"><PROPERTY Name="RawType" Format="String">File</PROPERTY><PROPERTY Name="Value" Format="String">OLEACC.DLL</PROPERTY></RESOURCE><RESOURCE ResTypeVSGUID="{2C10DB69-39AB-48A4-A83F-9AB3ACBA7C45}" BuildTypeMask="819" Name="RegKey(819):&quot;HKEY_LOCAL_MACHINE\Software\Microsoft\Windows\CurrentVersion\Uninstall\PCHealth&quot;,&quot;QuietUninstallString&quot;" Localize="0"><PROPERTY Name="KeyPath" Format="String">HKEY_LOCAL_MACHINE\Software\Microsoft\Windows\CurrentVersion\Uninstall\PCHealth</PROPERTY><PROPERTY Name="ValueName" Format="String">QuietUninstallString</PROPERTY><PROPERTY Name="RegValue" Format="String">rundll32.exe setupapi.dll,InstallHinfSection DefaultUninstall 132 %17%\PCHealth.inf</PROPERTY><PROPERTY Name="RegType" Format="Integer">1</PROPERTY><PROPERTY Name="RegOp" Format="Integer">1</PROPERTY><PROPERTY Name="RegCond" Format="Integer">1</PROPERTY><DISPLAYNAME>Uninstall String</DISPLAYNAME><DESCRIPTION>Necessary for uninstall</DESCRIPTION></RESOURCE><RESOURCE ResTypeVSGUID="{2C10DB69-39AB-48A4-A83F-9AB3ACBA7C45}" BuildTypeMask="819" Name="RegKey(819):&quot;HKEY_LOCAL_MACHINE\Software\Microsoft\Windows\CurrentVersion\Uninstall\PCHealth&quot;,&quot;UninstallString&quot;" Localize="0"><PROPERTY Name="KeyPath" Format="String">HKEY_LOCAL_MACHINE\Software\Microsoft\Windows\CurrentVersion\Uninstall\PCHealth</PROPERTY><PROPERTY Name="ValueName" Format="String">UninstallString</PROPERTY><PROPERTY Name="RegValue" Format="String">rundll32.exe setupapi.dll,InstallHinfSection DefaultUninstall 132 %17%\PCHealth.inf</PROPERTY><PROPERTY Name="RegType" Format="Integer">1</PROPERTY><PROPERTY Name="RegOp" Format="Integer">1</PROPERTY><PROPERTY Name="RegCond" Format="Integer">1</PROPERTY><DISPLAYNAME>Uninstall String</DISPLAYNAME><DESCRIPTION>Necessary for uninstall</DESCRIPTION></RESOURCE><RESOURCE ResTypeVSGUID="{2C10DB69-39AB-48A4-A83F-9AB3ACBA7C45}" BuildTypeMask="819" Name="RegKey(819):&quot;HKEY_CLASSES_ROOT\.nfo&quot;,&quot;&quot;" Localize="0"><PROPERTY Name="KeyPath" Format="String">HKEY_CLASSES_ROOT\.nfo</PROPERTY><PROPERTY Name="ValueName" Format="String"></PROPERTY><PROPERTY Name="RegValue" Format="String">MSInfo.Document</PROPERTY><PROPERTY Name="RegType" Format="Integer">1</PROPERTY><PROPERTY Name="RegOp" Format="Integer">1</PROPERTY><PROPERTY Name="RegCond" Format="Integer">1</PROPERTY><DISPLAYNAME>MSInfo Reg key</DISPLAYNAME></RESOURCE><RESOURCE ResTypeVSGUID="{2C10DB69-39AB-48A4-A83F-9AB3ACBA7C45}" BuildTypeMask="819" Name="RegKey(819):&quot;HKEY_CLASSES_ROOT\MSInfo.Document&quot;,&quot;&quot;" Localize="0"><PROPERTY Name="KeyPath" Format="String">HKEY_CLASSES_ROOT\MSInfo.Document</PROPERTY><PROPERTY Name="ValueName" Format="String"></PROPERTY><PROPERTY Name="RegValue" Format="String">MSInfo File</PROPERTY><PROPERTY Name="RegType" Format="Integer">1</PROPERTY><PROPERTY Name="RegOp" Format="Integer">1</PROPERTY><PROPERTY Name="RegCond" Format="Integer">1</PROPERTY><DISPLAYNAME>MSInfo Reg key</DISPLAYNAME></RESOURCE><RESOURCE ResTypeVSGUID="{2C10DB69-39AB-48A4-A83F-9AB3ACBA7C45}" BuildTypeMask="819" Name="RegKey(819):&quot;HKEY_CLASSES_ROOT\MSInfo.Document\DefaultIcon&quot;,&quot;&quot;" Localize="0"><PROPERTY Name="KeyPath" Format="String">HKEY_CLASSES_ROOT\MSInfo.Document\DefaultIcon</PROPERTY><PROPERTY Name="ValueName" Format="String"></PROPERTY><PROPERTY Name="RegValue" Format="String">%16427%\Microsoft Shared\MSInfo\MSInfo32.exe,0</PROPERTY><PROPERTY Name="RegType" Format="Integer">1</PROPERTY><PROPERTY Name="RegOp" Format="Integer">1</PROPERTY><PROPERTY Name="RegCond" Format="Integer">1</PROPERTY><DISPLAYNAME>MSInfo Reg key</DISPLAYNAME></RESOURCE><RESOURCE ResTypeVSGUID="{2C10DB69-39AB-48A4-A83F-9AB3ACBA7C45}" BuildTypeMask="819" Name="RegKey(819):&quot;HKEY_CLASSES_ROOT\MSInfo.Document\Shell\Open\Command&quot;,&quot;&quot;" Localize="0"><PROPERTY Name="KeyPath" Format="String">HKEY_CLASSES_ROOT\MSInfo.Document\Shell\Open\Command</PROPERTY><PROPERTY Name="ValueName" Format="String"></PROPERTY><PROPERTY Name="RegValue" Format="String">%16427%\Microsoft Shared\MSInfo\MSInfo32.exe /msinfo_file %1</PROPERTY><PROPERTY Name="RegType" Format="Integer">1</PROPERTY><PROPERTY Name="RegOp" Format="Integer">1</PROPERTY><PROPERTY Name="RegCond" Format="Integer">1</PROPERTY><DISPLAYNAME>MSInfo Reg key</DISPLAYNAME></RESOURCE><RESOURCE ResTypeVSGUID="{2C10DB69-39AB-48A4-A83F-9AB3ACBA7C45}" BuildTypeMask="819" Name="RegKey(819):&quot;HKEY_CLASSES_ROOT\MSInfo.Document\Shell\Print\Command&quot;,&quot;&quot;" Localize="0"><PROPERTY Name="KeyPath" Format="String">HKEY_CLASSES_ROOT\MSInfo.Document\Shell\Print\Command</PROPERTY><PROPERTY Name="ValueName" Format="String"></PROPERTY><PROPERTY Name="RegValue" Format="String">%16427%\Microsoft Shared\MSInfo\MSInfo32.exe /p "%1"</PROPERTY><PROPERTY Name="RegType" Format="Integer">1</PROPERTY><PROPERTY Name="RegOp" Format="Integer">1</PROPERTY><PROPERTY Name="RegCond" Format="Integer">1</PROPERTY><DISPLAYNAME>MSInfo Reg key</DISPLAYNAME></RESOURCE><RESOURCE ResTypeVSGUID="{2C10DB69-39AB-48A4-A83F-9AB3ACBA7C45}" BuildTypeMask="819" Name="RegKey(819):&quot;HKEY_LOCAL_MACHINE\SOFTWARE\Classes\Applications\MSInfo32.exe&quot;,&quot;NoOpenWith&quot;" Localize="0"><PROPERTY Name="KeyPath" Format="String">HKEY_LOCAL_MACHINE\SOFTWARE\Classes\Applications\MSInfo32.exe</PROPERTY><PROPERTY Name="ValueName" Format="String">NoOpenWith</PROPERTY><PROPERTY Name="RegValue" Format="String"></PROPERTY><PROPERTY Name="RegType" Format="Integer">1</PROPERTY><PROPERTY Name="RegOp" Format="Integer">1</PROPERTY><PROPERTY Name="RegCond" Format="Integer">1</PROPERTY><DISPLAYNAME>MSInfo Reg key</DISPLAYNAME></RESOURCE><RESOURCE ResTypeVSGUID="{2C10DB69-39AB-48A4-A83F-9AB3ACBA7C45}" BuildTypeMask="819" Name="RegKey(819):&quot;HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Shared Tools\MSInfo&quot;,&quot;Path&quot;" Localize="0"><PROPERTY Name="KeyPath" Format="String">HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Shared Tools\MSInfo</PROPERTY><PROPERTY Name="ValueName" Format="String">Path</PROPERTY><PROPERTY Name="RegValue" Format="String">%16427%\Microsoft Shared\MSInfo\MSInfo32.exe</PROPERTY><PROPERTY Name="RegType" Format="Integer">1</PROPERTY><PROPERTY Name="RegOp" Format="Integer">1</PROPERTY><PROPERTY Name="RegCond" Format="Integer">1</PROPERTY><DISPLAYNAME>MSInfo Reg key</DISPLAYNAME></RESOURCE><RESOURCE ResTypeVSGUID="{2C10DB69-39AB-48A4-A83F-9AB3ACBA7C45}" BuildTypeMask="819" Name="RegKey(819):&quot;HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Shared Tools\MSInfo\Categories&quot;" Localize="0"><PROPERTY Name="KeyPath" Format="String">HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Shared Tools\MSInfo\Categories</PROPERTY><PROPERTY Name="RegType" Format="Integer">1</PROPERTY><PROPERTY Name="RegOp" Format="Integer">1</PROPERTY><PROPERTY Name="RegCond" Format="Integer">1</PROPERTY><DISPLAYNAME>MSInfo Reg key</DISPLAYNAME></RESOURCE><RESOURCE ResTypeVSGUID="{2C10DB69-39AB-48A4-A83F-9AB3ACBA7C45}" BuildTypeMask="819" Name="RegKey(819):&quot;HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Shared Tools\MSInfo\Templates&quot;" Localize="0"><PROPERTY Name="KeyPath" Format="String">HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Shared Tools\MSInfo\Templates</PROPERTY><PROPERTY Name="RegType" Format="Integer">1</PROPERTY><PROPERTY Name="RegOp" Format="Integer">1</PROPERTY><PROPERTY Name="RegCond" Format="Integer">1</PROPERTY><DISPLAYNAME>MSInfo Reg key</DISPLAYNAME></RESOURCE><RESOURCE ResTypeVSGUID="{2C10DB69-39AB-48A4-A83F-9AB3ACBA7C45}" BuildTypeMask="819" Name="RegKey(819):&quot;HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Shared Tools\MSInfo\Toolsets\MSInfo&quot;" Localize="0"><PROPERTY Name="KeyPath" Format="String">HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Shared Tools\MSInfo\Toolsets\MSInfo</PROPERTY><PROPERTY Name="RegType" Format="Integer">1</PROPERTY><PROPERTY Name="RegOp" Format="Integer">1</PROPERTY><PROPERTY Name="RegCond" Format="Integer">1</PROPERTY><DISPLAYNAME>MSInfo Reg key</DISPLAYNAME></RESOURCE><RESOURCE ResTypeVSGUID="{2C10DB69-39AB-48A4-A83F-9AB3ACBA7C45}" BuildTypeMask="819" Name="RegKey(819):&quot;HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\App Paths\msinfo32.exe&quot;,&quot;&quot;" Localize="0"><PROPERTY Name="KeyPath" Format="String">HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\App Paths\msinfo32.exe</PROPERTY><PROPERTY Name="ValueName" Format="String"></PROPERTY><PROPERTY Name="RegValue" Format="String">%163427%\Microsoft Shared\MSInfo\MSInfo32.exe</PROPERTY><PROPERTY Name="RegType" Format="Integer">1</PROPERTY><PROPERTY Name="RegOp" Format="Integer">1</PROPERTY><PROPERTY Name="RegCond" Format="Integer">1</PROPERTY><DISPLAYNAME>MSInfo Reg key</DISPLAYNAME></RESOURCE><RESOURCE ResTypeVSGUID="{2C10DB69-39AB-48A4-A83F-9AB3ACBA7C45}" BuildTypeMask="819" Name="RegKey(819):&quot;HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\App Paths\msinfo32.exe&quot;,&quot;Path&quot;" Localize="0"><PROPERTY Name="KeyPath" Format="String">HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\App Paths\msinfo32.exe</PROPERTY><PROPERTY Name="ValueName" Format="String">Path</PROPERTY><PROPERTY Name="RegValue" Format="String">%163427%\Microsoft Shared\MSInfo\MSInfo32.exe</PROPERTY><PROPERTY Name="RegType" Format="Integer">1</PROPERTY><PROPERTY Name="RegOp" Format="Integer">1</PROPERTY><PROPERTY Name="RegCond" Format="Integer">1</PROPERTY><DISPLAYNAME>MSInfo Reg key</DISPLAYNAME></RESOURCE><GROUPMEMBER GroupVSGUID="{E01B4103-3883-4FE8-992F-10566E7B796C}"/>
		<GROUPMEMBER GroupVSGUID="{64668FB9-9289-45F0-BEF9-23745D272E3D}"/>
	</COMPONENT>
</DCARRIER>
