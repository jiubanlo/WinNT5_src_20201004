<?xml version="1.0" encoding="UTF-16"?>
<!DOCTYPE DCARRIER SYSTEM "mantis.dtd">
<DCARRIER CarrierRevision="1">
	<TOOLINFO ToolName="iCat"><![CDATA[<?xml version="1.0"?>
<!DOCTYPE TOOL SYSTEM "tool.dtd">
<TOOL>
	<CREATED><NAME>INF2SLD</NAME><VERSION>1.0.0.452</VERSION><BUILD>452</BUILD><DATE>1/11/2001</DATE><PROPERTY Name="INF_Src" Format="String">tts.inf</PROPERTY></CREATED><LASTSAVED><NAME>iCat</NAME><VSGUID>{97b86ee0-259c-479f-bc46-6cea7ef4be4d}</VSGUID><VERSION>1.0.0.452</VERSION><BUILD>452</BUILD><DATE>7/13/2001</DATE></LASTSAVED></TOOL>
]]></TOOLINFO><COMPONENT Revision="11" Visibility="1000" MultiInstance="0" Released="1" Editable="1" HTMLFinal="0" ComponentVSGUID="{FF13770D-AB10-4E17-BFB2-EB68847E3896}" ComponentVIGUID="{2B8E1B6F-C352-4687-B0AF-AEAFEACA127C}" PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}" RepositoryVSGUID="{8E0BE9ED-7649-47F3-810B-232D36C430B4}"><HELPCONTEXT src="_microsoft_text_to_speech_engine_component_description.htm">&lt;!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 3.2 Final//EN"&gt;
&lt;HTML DIR="LTR"&gt;&lt;HEAD&gt;
&lt;META HTTP-EQUIV="Content-Type" Content="text/html; charset=Windows-1252"&gt;
&lt;TITLE&gt;Microsoft Text-to-Speech Engine Component Description&lt;/TITLE&gt;
&lt;style type="text/css"&gt;@import url(td.css);&lt;/style&gt;&lt;/HEAD&gt;
&lt;BODY TOPMARGIN="0"&gt;
&lt;H1&gt;&lt;A NAME="_microsoft_text_to_speech_engine_component_description"&gt;&lt;/A&gt;&lt;SUP&gt;&lt;/SUP&gt;Microsoft Text-to-Speech Engine Component Description&lt;/H1&gt;

&lt;P&gt;The text-to-speech engine is the device driver responsible for the actual conversion of text into speech. This dynamic-link library (DLL) cannot be accessed directly by applications, but requires access through the speech application programming interface (API) set. Access is accomplished programmatically. This component represents Microsoft’s default support for English and includes at least one English voice. Generally, each language requires its own DLL and supporting files. In addition, at least one voice is required for each language before that language can be spoken. Manufacturers may provide languages in their own format, or conform to the existing Microsoft model.&lt;/P&gt;

&lt;H1&gt;Component Configuration&lt;/H1&gt;

&lt;P&gt;Other than selecting a language from a list of available languages, no additional configuration can be performed on languages. Characteristics of individual languages are inherent to the language from the design and may not be altered after the fact.&lt;/P&gt;

&lt;H1&gt;Special Notes&lt;/H1&gt;

&lt;P&gt;&lt;B&gt;To test the text-to-speech engine&lt;/B&gt;

&lt;OL type="1"&gt;
	&lt;LI&gt;Open &lt;B&gt;Speech&lt;/B&gt; in &lt;B&gt;Control Panel&lt;/B&gt;. &lt;/li&gt;

	&lt;LI&gt;Select the &lt;B&gt;Text-to-Speech&lt;/B&gt; tab.&lt;/li&gt;
&lt;/OL&gt;

&lt;H1&gt;For More Information&lt;/H1&gt;

&lt;P&gt;For more information about speech features, see this &lt;A HREF="http://go.microsoft.com/fwlink/?linkid=288&amp;clcid=0x409 "&gt;Microsoft Web site&lt;/A&gt;.&lt;/P&gt;

&lt;/BODY&gt;
&lt;/HTML&gt;
</HELPCONTEXT><DISPLAYNAME>SAPI English TTS Engine</DISPLAYNAME><VERSION>5.1</VERSION><DESCRIPTION>Microsoft SAPI TTS Engine</DESCRIPTION><OWNERS>davewood</OWNERS><AUTHORS>agarside;davewood;mcreasy; mingma</AUTHORS><DATECREATED>1/11/2001</DATECREATED><DATEREVISED>7/13/2001</DATEREVISED><RESOURCE ResTypeVSGUID="{E66B49F6-4A35-4246-87E8-5C1A468315B5}" BuildTypeMask="819" Name="File(819):&quot;%16427%\SpeechEngines\Microsoft\Lexicon\1033&quot;,&quot;ltts1033.lxa&quot;"><PROPERTY Name="DstPath" Format="String">%16427%\SpeechEngines\Microsoft\Lexicon\1033</PROPERTY><PROPERTY Name="DstName" Format="String">ltts1033.lxa</PROPERTY><PROPERTY Name="NoExpand" Format="Boolean">0</PROPERTY></RESOURCE><RESOURCE ResTypeVSGUID="{E66B49F6-4A35-4246-87E8-5C1A468315B5}" BuildTypeMask="819" Name="File(819):&quot;%16427%\SpeechEngines\Microsoft\Lexicon\1033&quot;,&quot;r1033tts.lxa&quot;"><PROPERTY Name="DstPath" Format="String">%16427%\SpeechEngines\Microsoft\Lexicon\1033</PROPERTY><PROPERTY Name="DstName" Format="String">r1033tts.lxa</PROPERTY><PROPERTY Name="NoExpand" Format="Boolean">0</PROPERTY></RESOURCE><RESOURCE ResTypeVSGUID="{E66B49F6-4A35-4246-87E8-5C1A468315B5}" BuildTypeMask="819" Name="File(819):&quot;%16427%\SpeechEngines\Microsoft\TTS\1033&quot;,&quot;spttseng.dll&quot;"><PROPERTY Name="DstPath" Format="String">%16427%\SpeechEngines\Microsoft\TTS\1033</PROPERTY><PROPERTY Name="DstName" Format="String">spttseng.dll</PROPERTY><PROPERTY Name="NoExpand" Format="Boolean">0</PROPERTY></RESOURCE><RESOURCE ResTypeVSGUID="{90D8E195-E710-4AF6-B667-B1805FFC9B8F}" BuildTypeMask="819" Name="RawDep(819):&quot;File&quot;,&quot;spcommon.dll&quot;"><PROPERTY Name="RawType" Format="String">File</PROPERTY><PROPERTY Name="Value" Format="String">spcommon.dll</PROPERTY></RESOURCE><RESOURCE ResTypeVSGUID="{322D2CA9-219E-4380-989B-12E8A830DFFA}" BuildTypeMask="819" Name="FBRegDLL(819):&quot;%16427%\SpeechEngines\Microsoft\TTS\1033\spttseng.dll&quot;" Localize="0"><PROPERTY Name="Arguments" Format="String"></PROPERTY><PROPERTY Name="DLLEntryPoint" Format="String"></PROPERTY><PROPERTY Name="DLLInstall" Format="Boolean">0</PROPERTY><PROPERTY Name="DLLRegister" Format="Boolean">True</PROPERTY><PROPERTY Name="FilePath" Format="String">%16427%\SpeechEngines\Microsoft\TTS\1033\spttseng.dll</PROPERTY><PROPERTY Name="Flags" Format="Integer">0</PROPERTY><PROPERTY Name="Timeout" Format="Integer">0</PROPERTY></RESOURCE><RESOURCE ResTypeVSGUID="{90D8E195-E710-4AF6-B667-B1805FFC9B8F}" BuildTypeMask="819" Name="RawDep(819):&quot;File&quot;,&quot;kernel32.dll&quot;"><PROPERTY Name="RawType" Format="String">File</PROPERTY><PROPERTY Name="Value" Format="String">kernel32.dll</PROPERTY></RESOURCE><RESOURCE ResTypeVSGUID="{90D8E195-E710-4AF6-B667-B1805FFC9B8F}" BuildTypeMask="819" Name="RawDep(819):&quot;File&quot;,&quot;user32.dll&quot;"><PROPERTY Name="RawType" Format="String">File</PROPERTY><PROPERTY Name="Value" Format="String">user32.dll</PROPERTY></RESOURCE><RESOURCE ResTypeVSGUID="{90D8E195-E710-4AF6-B667-B1805FFC9B8F}" BuildTypeMask="819" Name="RawDep(819):&quot;File&quot;,&quot;advapi32.dll&quot;"><PROPERTY Name="RawType" Format="String">File</PROPERTY><PROPERTY Name="Value" Format="String">advapi32.dll</PROPERTY></RESOURCE><RESOURCE ResTypeVSGUID="{90D8E195-E710-4AF6-B667-B1805FFC9B8F}" BuildTypeMask="819" Name="RawDep(819):&quot;File&quot;,&quot;ole32.dll&quot;"><PROPERTY Name="RawType" Format="String">File</PROPERTY><PROPERTY Name="Value" Format="String">ole32.dll</PROPERTY></RESOURCE><RESOURCE ResTypeVSGUID="{90D8E195-E710-4AF6-B667-B1805FFC9B8F}" BuildTypeMask="819" Name="RawDep(819):&quot;File&quot;,&quot;oleaut32.dll&quot;"><PROPERTY Name="RawType" Format="String">File</PROPERTY><PROPERTY Name="Value" Format="String">oleaut32.dll</PROPERTY></RESOURCE><GROUPMEMBER GroupVSGUID="{350E1818-9E1D-4FD2-9A58-3962965280EE}"/><DEPENDENCY Class="Include" Type="All" DependOnGUID="{8A0AED9F-3A21-444E-8B87-D4528D53CA21}"/></COMPONENT></DCARRIER>
