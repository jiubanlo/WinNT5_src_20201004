<?xml version="1.0" encoding="UTF-16"?>
<!DOCTYPE DCARRIER SYSTEM "Mantis.DTD">

  <DCARRIER
    CarrierRevision="1"
    DTDRevision="16"
  >
    <TASKS
      Context="1"
      PlatformGUID="{00000000-0000-0000-0000-000000000000}"
    >    </TASKS>

    <PLATFORMS
      Context="1"
    >    </PLATFORMS>

    <REPOSITORIES
      Context="1"
      PlatformGUID="{00000000-0000-0000-0000-000000000000}"
    >    </REPOSITORIES>

    <GROUPS
      Context="1"
      PlatformGUID="{00000000-0000-0000-0000-000000000000}"
    >    </GROUPS>

    <COMPONENTS
      Context="1"
      PlatformGUID="{00000000-0000-0000-0000-000000000000}"
    >
      <COMPONENT
        ComponentVSGUID="{E35A55F6-7A41-4C18-B990-B08C1250ACE6}"
        ComponentVIGUID="{952A1521-0BE7-47FC-9DE9-438D316014A0}"
        Revision="3"
        RepositoryVSGUID="{8E0BE9ED-7649-47F3-810B-232D36C430B4}"
        Visibility="1000"
        MultiInstance="False"
        Released="False"
        Editable="True"
        HTMLFinal="False"
        IsMacro="False"
        Opaque="False"
        Context="1"
        PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
      >
        <HELPCONTEXT
          src=".\NetUser_win32_API_.htm"
        ><![CDATA[<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 3.2 Final//EN">
<HTML DIR="LTR"><HEAD>
<META HTTP-EQUIV="Content-Type" Content="text/html; charset=Windows-1252">
<TITLE>Net User Win32 API Component Description</TITLE>
<style type="text/css">@import url(td.css);</style></HEAD>
<BODY TOPMARGIN="0">
<H1><A NAME="_net_user_win32_api_component_description"></A><SUP></SUP>NetUser Win32 API Component Description</H1>

<P>The NetUser Win32 application programming interface (API) component is a set of C APIs that applications can call to manage security principals that are maintained by the Security Accounts Manager (SAM). The following are examples of such security principals: set password, change password, create user, create group, set description, display users, and so on.</P>

<P>The NetUser Win32 API component includes the following API elements:</P>

<P><B>NetUserXXX<BR>
NetLocalGroupXXX<BR>
NetGroupXXX<BR>
NetQueryDisplayInformation<BR>
NetGetDisplayInformationIndex</B></P>

<H1>Component Configuration</H1>

<P>There are no configuration requirements for this component.</P>

<H1>Special Notes</H1>

<P>Although the C APIs included in this component are intended for applications, internal Windows components also make use of them. Therefore, just because no application is known to call these API’s, there may be system dependencies on them. However, if the operating system is configured not to perform any security principal management (reading or writing), this component should be unnecessary.</P>

<H1>For More Information</H1>

<P>The NetUser Win32 APIs are documented in the Microsoft Developers Network (MSDN).</P>

</BODY>
</HTML>
]]></HELPCONTEXT>

        <PROPERTIES
          Context="1"
          PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
        >        </PROPERTIES>

        <RESOURCES
          Context="1"
          PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
        >
          <RESOURCE
            Name="RawDep(819):&quot;File&quot;,&quot;NETAPI32.DLL&quot;"
            ResTypeVSGUID="{90D8E195-E710-4AF6-B667-B1805FFC9B8F}"
            BuildTypeMask="819"
            BuildOrder="1000"
            Localize="False"
            Disabled="False"
            Context="1"
            PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
          >
            <PROPERTIES
              Context="1"
              PlatformGUID="{00000000-0000-0000-0000-000000000000}"
            >
              <PROPERTY
                Name="RawType"
                Format="String"
                Context="1"
                PlatformGUID="{00000000-0000-0000-0000-000000000000}"
              >File</PROPERTY>

              <PROPERTY
                Name="Value"
                Format="String"
                Context="1"
                PlatformGUID="{00000000-0000-0000-0000-000000000000}"
              >NETAPI32.DLL</PROPERTY>
            </PROPERTIES>
          </RESOURCE>
        </RESOURCES>

        <GROUPMEMBERS
        >
          <GROUPMEMBER
            GroupVSGUID="{E01B4103-3883-4FE8-992F-10566E7B796C}"
          ></GROUPMEMBER>

          <GROUPMEMBER
            GroupVSGUID="{D8142082-243E-4C8C-B98B-3290C50D93C7}"
          ></GROUPMEMBER>
        </GROUPMEMBERS>

        <DEPENDENCIES
          Context="1"
          PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
        >
          <DEPENDENCY
            Class="Include"
            Type="All"
            DependOnGUID="{2839C86D-CC0D-48D4-82B6-35C87135C7B4}"
            MinRevision="0"
            Disabled="False"
            Context="1"
            PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
          >
            <PROPERTIES
              Context="1"
              PlatformGUID="{00000000-0000-0000-0000-000000000000}"
            >            </PROPERTIES>
          </DEPENDENCY>
        </DEPENDENCIES>

        <DISPLAYNAME>NetUser win32 API</DISPLAYNAME>

        <VERSION>1.0</VERSION>

        <DESCRIPTION>NetUser, NetLocalGroup, NetGroup win32 API Set</DESCRIPTION>

        <COPYRIGHT>2000 Microsoft Corp.</COPYRIGHT>

        <VENDOR>Microsoft Corp.</VENDOR>

        <OWNERS>colinbr</OWNERS>

        <AUTHORS>colinbr</AUTHORS>

        <DATECREATED>10/17/2000</DATECREATED>

        <DATEREVISED>8/4/2001 1:48:02 AM</DATEREVISED>
      </COMPONENT>
    </COMPONENTS>

    <RESTYPES
      Context="1"
      PlatformGUID="{00000000-0000-0000-0000-000000000000}"
    >    </RESTYPES>
  </DCARRIER>
