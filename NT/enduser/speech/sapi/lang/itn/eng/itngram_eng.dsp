# Microsoft Developer Studio Project File - Name="itngram_eng" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) External Target" 0x0106

CFG=itngram_eng - Win32 Win64 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "itngram_eng.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "itngram_eng.mak" CFG="itngram_eng - Win32 Win64 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "itngram_eng - Win32 Release" (based on "Win32 (x86) External Target")
!MESSAGE "itngram_eng - Win32 Debug" (based on "Win32 (x86) External Target")
!MESSAGE "itngram_eng - Win32 Win64 Debug" (based on "Win32 (x86) External Target")
!MESSAGE "itngram_eng - Win32 Win64 Release" (based on "Win32 (x86) External Target")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""

!IF  "$(CFG)" == "itngram_eng - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "itngram_eng___Win32_Release"
# PROP BASE Intermediate_Dir "itngram_eng___Win32_Release"
# PROP BASE Cmd_Line "NMAKE /f itngram_eng.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "itngram_eng.exe"
# PROP BASE Bsc_Name "itngram_eng.bsc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "itngram_eng___Win32_Release"
# PROP Intermediate_Dir "itngram_eng___Win32_Release"
# PROP Cmd_Line "..\..\..\..\common\bin\spgrazzle.cmd free exec build -Z -F -I"
# PROP Rebuild_Opt "-c"
# PROP Target_File "obj\i386\itngram.dll"
# PROP Bsc_Name ""
# PROP Target_Dir ""

!ELSEIF  "$(CFG)" == "itngram_eng - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "itngram_eng___Win32_Debug"
# PROP BASE Intermediate_Dir "itngram_eng___Win32_Debug"
# PROP BASE Cmd_Line "NMAKE /f itngram_eng.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "itngram_eng.exe"
# PROP BASE Bsc_Name "itngram_eng.bsc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "itngram_eng___Win32_Debug"
# PROP Intermediate_Dir "itngram_eng___Win32_Debug"
# PROP Cmd_Line "..\..\..\..\common\bin\spgrazzle.cmd exec build -Z -F -I"
# PROP Rebuild_Opt "-c"
# PROP Target_File "objd\i386\itngram.dll"
# PROP Bsc_Name ""
# PROP Target_Dir ""

!ELSEIF  "$(CFG)" == "itngram_eng - Win32 Win64 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Win64 Debug"
# PROP BASE Intermediate_Dir "Win64 Debug"
# PROP BASE Cmd_Line "\nt\enduser\speech\common\bin\spgrazzle.cmd exec build -Z -F -I"
# PROP BASE Rebuild_Opt "-c"
# PROP BASE Target_File "itngram_eng.exe"
# PROP BASE Bsc_Name ""
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Win64 Debug"
# PROP Intermediate_Dir "Win64 Debug"
# PROP Cmd_Line "..\..\..\..\common\bin\spgrazzle.cmd win64 exec build -Z -F -I"
# PROP Rebuild_Opt "-c"
# PROP Target_File "itngram_eng.exe"
# PROP Bsc_Name ""
# PROP Target_Dir ""

!ELSEIF  "$(CFG)" == "itngram_eng - Win32 Win64 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Win64 Release"
# PROP BASE Intermediate_Dir "Win64 Release"
# PROP BASE Cmd_Line "\nt\enduser\speech\common\bin\spgrazzle.cmd free exec build -z -F -I"
# PROP BASE Rebuild_Opt "-c"
# PROP BASE Target_File "itngram_eng.exe"
# PROP BASE Bsc_Name ""
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Win64 Release"
# PROP Intermediate_Dir "Win64 Release"
# PROP Cmd_Line "..\..\..\..\common\bin\spgrazzle.cmd win64 free exec build -Z -F -I"
# PROP Rebuild_Opt "-c"
# PROP Target_File "itngram_eng.exe"
# PROP Bsc_Name ""
# PROP Target_Dir ""

!ENDIF 

# Begin Target

# Name "itngram_eng - Win32 Release"
# Name "itngram_eng - Win32 Debug"
# Name "itngram_eng - Win32 Win64 Debug"
# Name "itngram_eng - Win32 Win64 Release"

!IF  "$(CFG)" == "itngram_eng - Win32 Release"

!ELSEIF  "$(CFG)" == "itngram_eng - Win32 Debug"

!ELSEIF  "$(CFG)" == "itngram_eng - Win32 Win64 Debug"

!ELSEIF  "$(CFG)" == "itngram_eng - Win32 Win64 Release"

!ENDIF 

# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\itngram.cpp
# End Source File
# Begin Source File

SOURCE=.\itngram.rc
# End Source File
# Begin Source File

SOURCE=.\itngram_i.c
# End Source File
# Begin Source File

SOURCE=.\stdafx.cpp
# End Source File
# Begin Source File

SOURCE=.\testitn.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\itngram.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\stdafx.h
# End Source File
# Begin Source File

SOURCE=.\test.h
# End Source File
# Begin Source File

SOURCE=.\testitn.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Source File

SOURCE=.\objd\i386\test.cfg
# End Source File
# Begin Source File

SOURCE=.\test.xml
# End Source File
# End Target
# End Project
