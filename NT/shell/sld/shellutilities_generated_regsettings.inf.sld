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
        ComponentVSGUID="{B6FC0C13-405F-4635-9F37-88AB7AB2D913}"
        ComponentVIGUID="{ED3A1A22-6F9A-4524-9196-4588408E7C37}"
        Revision="1"
        RepositoryVSGUID="{00000000-0000-0000-0000-000000000000}"
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
        <PROPERTIES
          Context="1"
          PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
        >
          <PROPERTY
            Name="cmiConcordanceID"
            Format="String"
            Context="1"
            PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
          >ShellUtilities_Generated_Regsettings.inf:DefaultInstall</PROPERTY>
        </PROPERTIES>

        <RESOURCES
          Context="1"
          PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
        >
          <RESOURCE
            Name="RegKey(819):&quot;HKEY_CLASSES_ROOT\.reg&quot;,&quot;&quot;"
            ResTypeVSGUID="{2C10DB69-39AB-48A4-A83F-9AB3ACBA7C45}"
            BuildTypeMask="819"
            BuildOrder="1000"
            Localize="False"
            Disabled="False"
            Context="1"
            PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
          >
            <PROPERTIES
              Context="1"
              PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
            >
              <PROPERTY
                Name="ComponentVSGUID"
                Format="GUID"
                Context="1"
                PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
              >{B6FC0C13-405F-4635-9F37-88AB7AB2D913}</PROPERTY>

              <PROPERTY
                Name="KeyPath"
                Format="String"
                Context="1"
                PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
              >HKEY_CLASSES_ROOT\.reg</PROPERTY>

              <PROPERTY
                Name="RegCond"
                Format="Integer"
                Context="1"
                PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
              >3</PROPERTY>

              <PROPERTY
                Name="RegOp"
                Format="Integer"
                Context="1"
                PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
              >1</PROPERTY>

              <PROPERTY
                Name="RegType"
                Format="Integer"
                Context="1"
                PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
              >1</PROPERTY>

              <PROPERTY
                Name="RegValue"
                Format="String"
                Context="1"
                PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
              >regfile</PROPERTY>

              <PROPERTY
                Name="ValueName"
                Format="String"
                Context="1"
                PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
              ></PROPERTY>
            </PROPERTIES>

            <DISPLAYNAME>RegKey(819):&quot;HKEY_CLASSES_ROOT\.reg&quot;,&quot;&quot;</DISPLAYNAME>

            <DESCRIPTION>RegKey(819):&quot;HKEY_CLASSES_ROOT\.reg&quot;,&quot;&quot;</DESCRIPTION>
          </RESOURCE>

          <RESOURCE
            Name="RegKey(819):&quot;HKEY_CLASSES_ROOT\regedit&quot;,&quot;&quot;"
            ResTypeVSGUID="{2C10DB69-39AB-48A4-A83F-9AB3ACBA7C45}"
            BuildTypeMask="819"
            BuildOrder="1001"
            Localize="False"
            Disabled="False"
            Context="1"
            PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
          >
            <PROPERTIES
              Context="1"
              PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
            >
              <PROPERTY
                Name="ComponentVSGUID"
                Format="GUID"
                Context="1"
                PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
              >{B6FC0C13-405F-4635-9F37-88AB7AB2D913}</PROPERTY>

              <PROPERTY
                Name="KeyPath"
                Format="String"
                Context="1"
                PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
              >HKEY_CLASSES_ROOT\regedit</PROPERTY>

              <PROPERTY
                Name="RegCond"
                Format="Integer"
                Context="1"
                PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
              >3</PROPERTY>

              <PROPERTY
                Name="RegOp"
                Format="Integer"
                Context="1"
                PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
              >1</PROPERTY>

              <PROPERTY
                Name="RegType"
                Format="Integer"
                Context="1"
                PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
              >1</PROPERTY>

              <PROPERTY
                Name="RegValue"
                Format="String"
                Context="1"
                PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
              >Registration Entries</PROPERTY>

              <PROPERTY
                Name="ValueName"
                Format="String"
                Context="1"
                PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
              ></PROPERTY>
            </PROPERTIES>

            <DISPLAYNAME>RegKey(819):&quot;HKEY_CLASSES_ROOT\regedit&quot;,&quot;&quot;</DISPLAYNAME>

            <DESCRIPTION>RegKey(819):&quot;HKEY_CLASSES_ROOT\regedit&quot;,&quot;&quot;</DESCRIPTION>
          </RESOURCE>

          <RESOURCE
            Name="RegKey(819):&quot;HKEY_CLASSES_ROOT\regedit\shell&quot;"
            ResTypeVSGUID="{2C10DB69-39AB-48A4-A83F-9AB3ACBA7C45}"
            BuildTypeMask="819"
            BuildOrder="1002"
            Localize="False"
            Disabled="False"
            Context="1"
            PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
          >
            <PROPERTIES
              Context="1"
              PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
            >
              <PROPERTY
                Name="ComponentVSGUID"
                Format="GUID"
                Context="1"
                PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
              >{B6FC0C13-405F-4635-9F37-88AB7AB2D913}</PROPERTY>

              <PROPERTY
                Name="KeyPath"
                Format="String"
                Context="1"
                PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
              >HKEY_CLASSES_ROOT\regedit\shell</PROPERTY>

              <PROPERTY
                Name="RegCond"
                Format="Integer"
                Context="1"
                PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
              >3</PROPERTY>

              <PROPERTY
                Name="RegOp"
                Format="Integer"
                Context="1"
                PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
              >1</PROPERTY>
            </PROPERTIES>

            <DISPLAYNAME>RegKey(819):&quot;HKEY_CLASSES_ROOT\regedit\shell&quot;</DISPLAYNAME>

            <DESCRIPTION>RegKey(819):&quot;HKEY_CLASSES_ROOT\regedit\shell&quot;</DESCRIPTION>
          </RESOURCE>

          <RESOURCE
            Name="RegKey(819):&quot;HKEY_CLASSES_ROOT\regedit\shell\open&quot;"
            ResTypeVSGUID="{2C10DB69-39AB-48A4-A83F-9AB3ACBA7C45}"
            BuildTypeMask="819"
            BuildOrder="1003"
            Localize="False"
            Disabled="False"
            Context="1"
            PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
          >
            <PROPERTIES
              Context="1"
              PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
            >
              <PROPERTY
                Name="ComponentVSGUID"
                Format="GUID"
                Context="1"
                PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
              >{B6FC0C13-405F-4635-9F37-88AB7AB2D913}</PROPERTY>

              <PROPERTY
                Name="KeyPath"
                Format="String"
                Context="1"
                PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
              >HKEY_CLASSES_ROOT\regedit\shell\open</PROPERTY>

              <PROPERTY
                Name="RegCond"
                Format="Integer"
                Context="1"
                PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
              >3</PROPERTY>

              <PROPERTY
                Name="RegOp"
                Format="Integer"
                Context="1"
                PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
              >1</PROPERTY>
            </PROPERTIES>

            <DISPLAYNAME>RegKey(819):&quot;HKEY_CLASSES_ROOT\regedit\shell\open&quot;</DISPLAYNAME>

            <DESCRIPTION>RegKey(819):&quot;HKEY_CLASSES_ROOT\regedit\shell\open&quot;</DESCRIPTION>
          </RESOURCE>

          <RESOURCE
            Name="RegKey(819):&quot;HKEY_CLASSES_ROOT\regedit\shell\open\command&quot;,&quot;&quot;"
            ResTypeVSGUID="{2C10DB69-39AB-48A4-A83F-9AB3ACBA7C45}"
            BuildTypeMask="819"
            BuildOrder="1004"
            Localize="False"
            Disabled="False"
            Context="1"
            PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
          >
            <PROPERTIES
              Context="1"
              PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
            >
              <PROPERTY
                Name="ComponentVSGUID"
                Format="GUID"
                Context="1"
                PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
              >{B6FC0C13-405F-4635-9F37-88AB7AB2D913}</PROPERTY>

              <PROPERTY
                Name="KeyPath"
                Format="String"
                Context="1"
                PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
              >HKEY_CLASSES_ROOT\regedit\shell\open\command</PROPERTY>

              <PROPERTY
                Name="RegCond"
                Format="Integer"
                Context="1"
                PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
              >3</PROPERTY>

              <PROPERTY
                Name="RegOp"
                Format="Integer"
                Context="1"
                PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
              >1</PROPERTY>

              <PROPERTY
                Name="RegType"
                Format="Integer"
                Context="1"
                PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
              >1</PROPERTY>

              <PROPERTY
                Name="RegValue"
                Format="String"
                Context="1"
                PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
              >regedit.exe %1</PROPERTY>

              <PROPERTY
                Name="ValueName"
                Format="String"
                Context="1"
                PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
              ></PROPERTY>
            </PROPERTIES>

            <DISPLAYNAME>RegKey(819):&quot;HKEY_CLASSES_ROOT\regedit\shell\open\command&quot;,&quot;&quot;</DISPLAYNAME>

            <DESCRIPTION>RegKey(819):&quot;HKEY_CLASSES_ROOT\regedit\shell\open\command&quot;,&quot;&quot;</DESCRIPTION>
          </RESOURCE>

          <RESOURCE
            Name="RegKey(819):&quot;HKEY_CLASSES_ROOT\regfile&quot;,&quot;&quot;"
            ResTypeVSGUID="{2C10DB69-39AB-48A4-A83F-9AB3ACBA7C45}"
            BuildTypeMask="819"
            BuildOrder="1005"
            Localize="False"
            Disabled="False"
            Context="1"
            PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
          >
            <PROPERTIES
              Context="1"
              PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
            >
              <PROPERTY
                Name="ComponentVSGUID"
                Format="GUID"
                Context="1"
                PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
              >{B6FC0C13-405F-4635-9F37-88AB7AB2D913}</PROPERTY>

              <PROPERTY
                Name="KeyPath"
                Format="String"
                Context="1"
                PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
              >HKEY_CLASSES_ROOT\regfile</PROPERTY>

              <PROPERTY
                Name="RegCond"
                Format="Integer"
                Context="1"
                PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
              >3</PROPERTY>

              <PROPERTY
                Name="RegOp"
                Format="Integer"
                Context="1"
                PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
              >1</PROPERTY>

              <PROPERTY
                Name="RegType"
                Format="Integer"
                Context="1"
                PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
              >1</PROPERTY>

              <PROPERTY
                Name="RegValue"
                Format="String"
                Context="1"
                PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
              >Registration Entries</PROPERTY>

              <PROPERTY
                Name="ValueName"
                Format="String"
                Context="1"
                PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
              ></PROPERTY>
            </PROPERTIES>

            <DISPLAYNAME>RegKey(819):&quot;HKEY_CLASSES_ROOT\regfile&quot;,&quot;&quot;</DISPLAYNAME>

            <DESCRIPTION>RegKey(819):&quot;HKEY_CLASSES_ROOT\regfile&quot;,&quot;&quot;</DESCRIPTION>
          </RESOURCE>

          <RESOURCE
            Name="RegKey(819):&quot;HKEY_CLASSES_ROOT\regfile\DefaultIcon&quot;,&quot;&quot;"
            ResTypeVSGUID="{2C10DB69-39AB-48A4-A83F-9AB3ACBA7C45}"
            BuildTypeMask="819"
            BuildOrder="1006"
            Localize="False"
            Disabled="False"
            Context="1"
            PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
          >
            <PROPERTIES
              Context="1"
              PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
            >
              <PROPERTY
                Name="ComponentVSGUID"
                Format="GUID"
                Context="1"
                PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
              >{B6FC0C13-405F-4635-9F37-88AB7AB2D913}</PROPERTY>

              <PROPERTY
                Name="KeyPath"
                Format="String"
                Context="1"
                PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
              >HKEY_CLASSES_ROOT\regfile\DefaultIcon</PROPERTY>

              <PROPERTY
                Name="RegCond"
                Format="Integer"
                Context="1"
                PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
              >3</PROPERTY>

              <PROPERTY
                Name="RegOp"
                Format="Integer"
                Context="1"
                PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
              >1</PROPERTY>

              <PROPERTY
                Name="RegType"
                Format="Integer"
                Context="1"
                PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
              >2</PROPERTY>

              <PROPERTY
                Name="RegValue"
                Format="String"
                Context="1"
                PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
              >%SystemRoot%\regedit.exe,1</PROPERTY>

              <PROPERTY
                Name="ValueName"
                Format="String"
                Context="1"
                PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
              ></PROPERTY>
            </PROPERTIES>

            <DISPLAYNAME>RegKey(819):&quot;HKEY_CLASSES_ROOT\regfile\DefaultIcon&quot;,&quot;&quot;</DISPLAYNAME>

            <DESCRIPTION>RegKey(819):&quot;HKEY_CLASSES_ROOT\regfile\DefaultIcon&quot;,&quot;&quot;</DESCRIPTION>
          </RESOURCE>

          <RESOURCE
            Name="RegKey(819):&quot;HKEY_CLASSES_ROOT\regfile\shell\open&quot;,&quot;&quot;"
            ResTypeVSGUID="{2C10DB69-39AB-48A4-A83F-9AB3ACBA7C45}"
            BuildTypeMask="819"
            BuildOrder="1007"
            Localize="False"
            Disabled="False"
            Context="1"
            PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
          >
            <PROPERTIES
              Context="1"
              PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
            >
              <PROPERTY
                Name="ComponentVSGUID"
                Format="GUID"
                Context="1"
                PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
              >{B6FC0C13-405F-4635-9F37-88AB7AB2D913}</PROPERTY>

              <PROPERTY
                Name="KeyPath"
                Format="String"
                Context="1"
                PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
              >HKEY_CLASSES_ROOT\regfile\shell\open</PROPERTY>

              <PROPERTY
                Name="RegCond"
                Format="Integer"
                Context="1"
                PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
              >3</PROPERTY>

              <PROPERTY
                Name="RegOp"
                Format="Integer"
                Context="1"
                PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
              >1</PROPERTY>

              <PROPERTY
                Name="RegType"
                Format="Integer"
                Context="1"
                PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
              >1</PROPERTY>

              <PROPERTY
                Name="RegValue"
                Format="String"
                Context="1"
                PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
              >Mer&amp;ge</PROPERTY>

              <PROPERTY
                Name="ValueName"
                Format="String"
                Context="1"
                PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
              ></PROPERTY>
            </PROPERTIES>

            <DISPLAYNAME>RegKey(819):&quot;HKEY_CLASSES_ROOT\regfile\shell\open&quot;,&quot;&quot;</DISPLAYNAME>

            <DESCRIPTION>RegKey(819):&quot;HKEY_CLASSES_ROOT\regfile\shell\open&quot;,&quot;&quot;</DESCRIPTION>
          </RESOURCE>

          <RESOURCE
            Name="RegKey(819):&quot;HKEY_CLASSES_ROOT\regfile\shell\open\command&quot;,&quot;&quot;"
            ResTypeVSGUID="{2C10DB69-39AB-48A4-A83F-9AB3ACBA7C45}"
            BuildTypeMask="819"
            BuildOrder="1008"
            Localize="False"
            Disabled="False"
            Context="1"
            PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
          >
            <PROPERTIES
              Context="1"
              PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
            >
              <PROPERTY
                Name="ComponentVSGUID"
                Format="GUID"
                Context="1"
                PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
              >{B6FC0C13-405F-4635-9F37-88AB7AB2D913}</PROPERTY>

              <PROPERTY
                Name="KeyPath"
                Format="String"
                Context="1"
                PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
              >HKEY_CLASSES_ROOT\regfile\shell\open\command</PROPERTY>

              <PROPERTY
                Name="RegCond"
                Format="Integer"
                Context="1"
                PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
              >3</PROPERTY>

              <PROPERTY
                Name="RegOp"
                Format="Integer"
                Context="1"
                PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
              >1</PROPERTY>

              <PROPERTY
                Name="RegType"
                Format="Integer"
                Context="1"
                PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
              >1</PROPERTY>

              <PROPERTY
                Name="RegValue"
                Format="String"
                Context="1"
                PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
              >regedit.exe &quot;%1&quot;</PROPERTY>

              <PROPERTY
                Name="ValueName"
                Format="String"
                Context="1"
                PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
              ></PROPERTY>
            </PROPERTIES>

            <DISPLAYNAME>RegKey(819):&quot;HKEY_CLASSES_ROOT\regfile\shell\open\command&quot;,&quot;&quot;</DISPLAYNAME>

            <DESCRIPTION>RegKey(819):&quot;HKEY_CLASSES_ROOT\regfile\shell\open\command&quot;,&quot;&quot;</DESCRIPTION>
          </RESOURCE>

          <RESOURCE
            Name="RegKey(819):&quot;HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\IniFileMapping&quot;"
            ResTypeVSGUID="{2C10DB69-39AB-48A4-A83F-9AB3ACBA7C45}"
            BuildTypeMask="819"
            BuildOrder="1009"
            Localize="False"
            Disabled="False"
            Context="1"
            PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
          >
            <PROPERTIES
              Context="1"
              PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
            >
              <PROPERTY
                Name="ComponentVSGUID"
                Format="GUID"
                Context="1"
                PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
              >{B6FC0C13-405F-4635-9F37-88AB7AB2D913}</PROPERTY>

              <PROPERTY
                Name="KeyPath"
                Format="String"
                Context="1"
                PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
              >HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\IniFileMapping</PROPERTY>

              <PROPERTY
                Name="RegCond"
                Format="Integer"
                Context="1"
                PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
              >3</PROPERTY>

              <PROPERTY
                Name="RegOp"
                Format="Integer"
                Context="1"
                PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
              >1</PROPERTY>
            </PROPERTIES>

            <DISPLAYNAME>RegKey(819):&quot;HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\IniFileMapping&quot;</DISPLAYNAME>

            <DESCRIPTION>RegKey(819):&quot;HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\IniFileMapping&quot;</DESCRIPTION>
          </RESOURCE>

          <RESOURCE
            Name="RegKey(819):&quot;HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\IniFileMapping\regedt32.ini&quot;,&quot;&quot;"
            ResTypeVSGUID="{2C10DB69-39AB-48A4-A83F-9AB3ACBA7C45}"
            BuildTypeMask="819"
            BuildOrder="1010"
            Localize="False"
            Disabled="False"
            Context="1"
            PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
          >
            <PROPERTIES
              Context="1"
              PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
            >
              <PROPERTY
                Name="ComponentVSGUID"
                Format="GUID"
                Context="1"
                PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
              >{B6FC0C13-405F-4635-9F37-88AB7AB2D913}</PROPERTY>

              <PROPERTY
                Name="KeyPath"
                Format="String"
                Context="1"
                PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
              >HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\IniFileMapping\regedt32.ini</PROPERTY>

              <PROPERTY
                Name="RegCond"
                Format="Integer"
                Context="1"
                PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
              >3</PROPERTY>

              <PROPERTY
                Name="RegOp"
                Format="Integer"
                Context="1"
                PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
              >1</PROPERTY>

              <PROPERTY
                Name="RegType"
                Format="Integer"
                Context="1"
                PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
              >1</PROPERTY>

              <PROPERTY
                Name="RegValue"
                Format="String"
                Context="1"
                PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
              >USR:Software\Microsoft\RegEdt32</PROPERTY>

              <PROPERTY
                Name="ValueName"
                Format="String"
                Context="1"
                PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
              ></PROPERTY>
            </PROPERTIES>

            <DISPLAYNAME>RegKey(819):&quot;HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\IniFileMapping\regedt32.ini&quot;,&quot;&quot;</DISPLAYNAME>

            <DESCRIPTION>RegKey(819):&quot;HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\IniFileMapping\regedt32.ini&quot;,&quot;&quot;</DESCRIPTION>
          </RESOURCE>

          <RESOURCE
            Name="RegKey(819):&quot;HKEY_CURRENT_USER\Software\Microsoft\RegEdt32&quot;"
            ResTypeVSGUID="{2C10DB69-39AB-48A4-A83F-9AB3ACBA7C45}"
            BuildTypeMask="819"
            BuildOrder="1011"
            Localize="False"
            Disabled="False"
            Context="1"
            PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
          >
            <PROPERTIES
              Context="1"
              PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
            >
              <PROPERTY
                Name="ComponentVSGUID"
                Format="GUID"
                Context="1"
                PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
              >{B6FC0C13-405F-4635-9F37-88AB7AB2D913}</PROPERTY>

              <PROPERTY
                Name="KeyPath"
                Format="String"
                Context="1"
                PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
              >HKEY_CURRENT_USER\Software\Microsoft\RegEdt32</PROPERTY>

              <PROPERTY
                Name="RegCond"
                Format="Integer"
                Context="1"
                PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
              >3</PROPERTY>

              <PROPERTY
                Name="RegOp"
                Format="Integer"
                Context="1"
                PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
              >1</PROPERTY>
            </PROPERTIES>

            <DISPLAYNAME>RegKey(819):&quot;HKEY_CURRENT_USER\Software\Microsoft\RegEdt32&quot;</DISPLAYNAME>

            <DESCRIPTION>RegKey(819):&quot;HKEY_CURRENT_USER\Software\Microsoft\RegEdt32&quot;</DESCRIPTION>
          </RESOURCE>

          <RESOURCE
            Name="RegKey(819):&quot;HKEY_CURRENT_USER\Software\Microsoft\RegEdt32\Settings&quot;,&quot;AutoRefresh&quot;"
            ResTypeVSGUID="{2C10DB69-39AB-48A4-A83F-9AB3ACBA7C45}"
            BuildTypeMask="819"
            BuildOrder="1012"
            Localize="False"
            Disabled="False"
            Context="1"
            PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
          >
            <PROPERTIES
              Context="1"
              PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
            >
              <PROPERTY
                Name="ComponentVSGUID"
                Format="GUID"
                Context="1"
                PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
              >{B6FC0C13-405F-4635-9F37-88AB7AB2D913}</PROPERTY>

              <PROPERTY
                Name="KeyPath"
                Format="String"
                Context="1"
                PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
              >HKEY_CURRENT_USER\Software\Microsoft\RegEdt32\Settings</PROPERTY>

              <PROPERTY
                Name="RegCond"
                Format="Integer"
                Context="1"
                PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
              >3</PROPERTY>

              <PROPERTY
                Name="RegOp"
                Format="Integer"
                Context="1"
                PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
              >1</PROPERTY>

              <PROPERTY
                Name="RegType"
                Format="Integer"
                Context="1"
                PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
              >1</PROPERTY>

              <PROPERTY
                Name="RegValue"
                Format="String"
                Context="1"
                PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
              >1</PROPERTY>

              <PROPERTY
                Name="ValueName"
                Format="String"
                Context="1"
                PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
              >AutoRefresh</PROPERTY>
            </PROPERTIES>

            <DISPLAYNAME>RegKey(819):&quot;HKEY_CURRENT_USER\Software\Microsoft\RegEdt32\Settings&quot;,&quot;AutoRefresh&quot;</DISPLAYNAME>

            <DESCRIPTION>RegKey(819):&quot;HKEY_CURRENT_USER\Software\Microsoft\RegEdt32\Settings&quot;,&quot;AutoRefresh&quot;</DESCRIPTION>
          </RESOURCE>

          <RESOURCE
            Name="RegKey(819):&quot;HKEY_CURRENT_USER\Software\Microsoft\RegEdt32\Settings&quot;,&quot;ConfirmOnDelete&quot;"
            ResTypeVSGUID="{2C10DB69-39AB-48A4-A83F-9AB3ACBA7C45}"
            BuildTypeMask="819"
            BuildOrder="1013"
            Localize="False"
            Disabled="False"
            Context="1"
            PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
          >
            <PROPERTIES
              Context="1"
              PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
            >
              <PROPERTY
                Name="ComponentVSGUID"
                Format="GUID"
                Context="1"
                PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
              >{B6FC0C13-405F-4635-9F37-88AB7AB2D913}</PROPERTY>

              <PROPERTY
                Name="KeyPath"
                Format="String"
                Context="1"
                PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
              >HKEY_CURRENT_USER\Software\Microsoft\RegEdt32\Settings</PROPERTY>

              <PROPERTY
                Name="RegCond"
                Format="Integer"
                Context="1"
                PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
              >3</PROPERTY>

              <PROPERTY
                Name="RegOp"
                Format="Integer"
                Context="1"
                PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
              >1</PROPERTY>

              <PROPERTY
                Name="RegType"
                Format="Integer"
                Context="1"
                PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
              >1</PROPERTY>

              <PROPERTY
                Name="RegValue"
                Format="String"
                Context="1"
                PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
              >1</PROPERTY>

              <PROPERTY
                Name="ValueName"
                Format="String"
                Context="1"
                PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
              >ConfirmOnDelete</PROPERTY>
            </PROPERTIES>

            <DISPLAYNAME>RegKey(819):&quot;HKEY_CURRENT_USER\Software\Microsoft\RegEdt32\Settings&quot;,&quot;ConfirmOnDelete&quot;</DISPLAYNAME>

            <DESCRIPTION>RegKey(819):&quot;HKEY_CURRENT_USER\Software\Microsoft\RegEdt32\Settings&quot;,&quot;ConfirmOnDelete&quot;</DESCRIPTION>
          </RESOURCE>

          <RESOURCE
            Name="RegKey(819):&quot;HKEY_CURRENT_USER\Software\Microsoft\RegEdt32\Settings&quot;,&quot;ReadOnly&quot;"
            ResTypeVSGUID="{2C10DB69-39AB-48A4-A83F-9AB3ACBA7C45}"
            BuildTypeMask="819"
            BuildOrder="1014"
            Localize="False"
            Disabled="False"
            Context="1"
            PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
          >
            <PROPERTIES
              Context="1"
              PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
            >
              <PROPERTY
                Name="ComponentVSGUID"
                Format="GUID"
                Context="1"
                PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
              >{B6FC0C13-405F-4635-9F37-88AB7AB2D913}</PROPERTY>

              <PROPERTY
                Name="KeyPath"
                Format="String"
                Context="1"
                PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
              >HKEY_CURRENT_USER\Software\Microsoft\RegEdt32\Settings</PROPERTY>

              <PROPERTY
                Name="RegCond"
                Format="Integer"
                Context="1"
                PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
              >3</PROPERTY>

              <PROPERTY
                Name="RegOp"
                Format="Integer"
                Context="1"
                PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
              >1</PROPERTY>

              <PROPERTY
                Name="RegType"
                Format="Integer"
                Context="1"
                PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
              >1</PROPERTY>

              <PROPERTY
                Name="RegValue"
                Format="String"
                Context="1"
                PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
              >0</PROPERTY>

              <PROPERTY
                Name="ValueName"
                Format="String"
                Context="1"
                PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
              >ReadOnly</PROPERTY>
            </PROPERTIES>

            <DISPLAYNAME>RegKey(819):&quot;HKEY_CURRENT_USER\Software\Microsoft\RegEdt32\Settings&quot;,&quot;ReadOnly&quot;</DISPLAYNAME>

            <DESCRIPTION>RegKey(819):&quot;HKEY_CURRENT_USER\Software\Microsoft\RegEdt32\Settings&quot;,&quot;ReadOnly&quot;</DESCRIPTION>
          </RESOURCE>

          <RESOURCE
            Name="RegKey(819):&quot;HKEY_CURRENT_USER\Software\Microsoft\RegEdt32\Settings&quot;,&quot;RemoteAccess&quot;"
            ResTypeVSGUID="{2C10DB69-39AB-48A4-A83F-9AB3ACBA7C45}"
            BuildTypeMask="819"
            BuildOrder="1015"
            Localize="False"
            Disabled="False"
            Context="1"
            PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
          >
            <PROPERTIES
              Context="1"
              PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
            >
              <PROPERTY
                Name="ComponentVSGUID"
                Format="GUID"
                Context="1"
                PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
              >{B6FC0C13-405F-4635-9F37-88AB7AB2D913}</PROPERTY>

              <PROPERTY
                Name="KeyPath"
                Format="String"
                Context="1"
                PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
              >HKEY_CURRENT_USER\Software\Microsoft\RegEdt32\Settings</PROPERTY>

              <PROPERTY
                Name="RegCond"
                Format="Integer"
                Context="1"
                PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
              >3</PROPERTY>

              <PROPERTY
                Name="RegOp"
                Format="Integer"
                Context="1"
                PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
              >1</PROPERTY>

              <PROPERTY
                Name="RegType"
                Format="Integer"
                Context="1"
                PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
              >1</PROPERTY>

              <PROPERTY
                Name="RegValue"
                Format="String"
                Context="1"
                PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
              >0</PROPERTY>

              <PROPERTY
                Name="ValueName"
                Format="String"
                Context="1"
                PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
              >RemoteAccess</PROPERTY>
            </PROPERTIES>

            <DISPLAYNAME>RegKey(819):&quot;HKEY_CURRENT_USER\Software\Microsoft\RegEdt32\Settings&quot;,&quot;RemoteAccess&quot;</DISPLAYNAME>

            <DESCRIPTION>RegKey(819):&quot;HKEY_CURRENT_USER\Software\Microsoft\RegEdt32\Settings&quot;,&quot;RemoteAccess&quot;</DESCRIPTION>
          </RESOURCE>

          <RESOURCE
            Name="RegKey(819):&quot;HKEY_CURRENT_USER\Software\Microsoft\RegEdt32\Settings&quot;,&quot;SaveSettings&quot;"
            ResTypeVSGUID="{2C10DB69-39AB-48A4-A83F-9AB3ACBA7C45}"
            BuildTypeMask="819"
            BuildOrder="1016"
            Localize="False"
            Disabled="False"
            Context="1"
            PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
          >
            <PROPERTIES
              Context="1"
              PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
            >
              <PROPERTY
                Name="ComponentVSGUID"
                Format="GUID"
                Context="1"
                PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
              >{B6FC0C13-405F-4635-9F37-88AB7AB2D913}</PROPERTY>

              <PROPERTY
                Name="KeyPath"
                Format="String"
                Context="1"
                PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
              >HKEY_CURRENT_USER\Software\Microsoft\RegEdt32\Settings</PROPERTY>

              <PROPERTY
                Name="RegCond"
                Format="Integer"
                Context="1"
                PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
              >3</PROPERTY>

              <PROPERTY
                Name="RegOp"
                Format="Integer"
                Context="1"
                PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
              >1</PROPERTY>

              <PROPERTY
                Name="RegType"
                Format="Integer"
                Context="1"
                PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
              >1</PROPERTY>

              <PROPERTY
                Name="RegValue"
                Format="String"
                Context="1"
                PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
              >1</PROPERTY>

              <PROPERTY
                Name="ValueName"
                Format="String"
                Context="1"
                PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
              >SaveSettings</PROPERTY>
            </PROPERTIES>

            <DISPLAYNAME>RegKey(819):&quot;HKEY_CURRENT_USER\Software\Microsoft\RegEdt32\Settings&quot;,&quot;SaveSettings&quot;</DISPLAYNAME>

            <DESCRIPTION>RegKey(819):&quot;HKEY_CURRENT_USER\Software\Microsoft\RegEdt32\Settings&quot;,&quot;SaveSettings&quot;</DESCRIPTION>
          </RESOURCE>
        </RESOURCES>

        <DEPENDENCIES
          Context="1"
          PlatformGUID="{B784E719-C196-4DDB-B358-D9254426C38D}"
        >        </DEPENDENCIES>

        <DISPLAYNAME>ShellUtilities_Generated_Regsettings.inf</DISPLAYNAME>

        <VERSION>Version 1.0</VERSION>

        <DESCRIPTION>ShellUtilities_Generated_Regsettings.inf</DESCRIPTION>

        <VENDOR></VENDOR>

        <OWNERS>mikesh</OWNERS>

        <AUTHORS>mikesh</AUTHORS>

        <DATECREATED>8/6/2001</DATECREATED>

        <DATEREVISED>8/6/2001</DATEREVISED>
      </COMPONENT>
    </COMPONENTS>

    <RESTYPES
      Context="1"
      PlatformGUID="{00000000-0000-0000-0000-000000000000}"
    >    </RESTYPES>
  </DCARRIER>
