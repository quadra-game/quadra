# Microsoft Developer Studio Project File - Name="Quadra" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=Quadra - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Quadra.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Quadra.mak" CFG="Quadra - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Quadra - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "Quadra - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Quadra - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /WX /GX /O2 /Ob2 /I "..\include" /I "..\skelton\include" /D "NDEBUG" /D "DEMO_VERSION" /D "WIN32" /D "UGS_DIRECTX" /FD /c
# SUBTRACT CPP /Fr /YX
# ADD BASE RSC /l 0xc0c /d "NDEBUG"
# ADD RSC /l 0x1009 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 gdi32.lib winspool.lib comdlg32.lib ole32.lib oleaut32.lib uuid.lib winmm.lib ddraw.lib dxguid.lib dinput.lib dsound.lib kernel32.lib user32.lib shell32.lib advapi32.lib skelton.lib wsock32.lib zlib.lib libpng.lib /nologo /subsystem:windows /machine:I386 /out:"..\quadra.exe" /libpath:"release" /libpath:"../../zlib/zlib/release"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "Quadra - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /Zi /Od /Ob1 /I "..\include" /I "..\skelton\include" /D "_DEBUG" /D "_CRTDBG_MAP_ALLOC" /D "WIN32" /D "UGS_DIRECTX" /Fr /YX /FD /c
# SUBTRACT CPP /Gf
# ADD BASE RSC /l 0xc0c /d "_DEBUG"
# ADD RSC /l 0xc0c /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 winmm.lib ddraw.lib dxguid.lib dinput.lib dsound.lib kernel32.lib user32.lib shell32.lib advapi32.lib skelton.lib wsock32.lib zlib.lib libpng.lib /nologo /subsystem:windows /map /debug /machine:I386 /out:"..\quadra.exe" /pdbtype:sept /libpath:"debug" /libpath:"../../zlib/zlib/debug"
# SUBTRACT LINK32 /verbose /profile /pdb:none /incremental:no /force

!ENDIF 

# Begin Target

# Name "Quadra - Win32 Release"
# Name "Quadra - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp"
# Begin Source File

SOURCE=..\source\bloc.cpp
# End Source File
# Begin Source File

SOURCE=..\source\canvas.cpp
# End Source File
# Begin Source File

SOURCE=..\source\chat_text.cpp
# End Source File
# Begin Source File

SOURCE=..\source\color.cpp
# End Source File
# Begin Source File

SOURCE=..\source\config.cpp
# End Source File
# Begin Source File

SOURCE=..\source\fonts.cpp
# End Source File
# Begin Source File

SOURCE=..\source\game.cpp
# End Source File
# Begin Source File

SOURCE=..\source\game_menu.cpp
# End Source File
# Begin Source File

SOURCE=..\source\global.cpp
# End Source File
# Begin Source File

SOURCE=..\source\highscores.cpp
# End Source File
# Begin Source File

SOURCE=..\source\menu.cpp
# End Source File
# Begin Source File

SOURCE=..\source\menu_base.cpp
# End Source File
# Begin Source File

SOURCE=..\source\menu_demo_central.cpp
# End Source File
# Begin Source File

SOURCE=..\source\misc.cpp
# End Source File
# Begin Source File

SOURCE=..\source\multi_player.cpp
# End Source File
# Begin Source File

SOURCE=..\source\net_list.cpp
# End Source File
# Begin Source File

SOURCE=..\source\net_server.cpp
# End Source File
# Begin Source File

SOURCE=..\source\net_stuff.cpp
# End Source File
# Begin Source File

SOURCE=..\source\nglog.cpp
# End Source File
# Begin Source File

SOURCE=..\source\packets.cpp
# End Source File
# Begin Source File

SOURCE=..\source\pane.cpp
# End Source File
# Begin Source File

SOURCE=..\source\qserv.cpp
# End Source File
# Begin Source File

SOURCE=..\source\quadra.cpp
# End Source File
# Begin Source File

SOURCE=..\source\recording.cpp
# End Source File
# Begin Source File

SOURCE=..\source\score.cpp
# End Source File
# Begin Source File

SOURCE=..\source\sons.cpp
# End Source File
# Begin Source File

SOURCE=..\source\stats.cpp
# End Source File
# Begin Source File

SOURCE=..\source\zone.cpp
# End Source File
# Begin Source File

SOURCE=..\source\zone_list.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h"
# Begin Source File

SOURCE=..\include\bloc.h
# End Source File
# Begin Source File

SOURCE=..\include\canvas.h
# End Source File
# Begin Source File

SOURCE=..\include\chat_text.h
# End Source File
# Begin Source File

SOURCE=..\include\color.h
# End Source File
# Begin Source File

SOURCE=..\include\config.h
# End Source File
# Begin Source File

SOURCE=..\include\fonts.h
# End Source File
# Begin Source File

SOURCE=..\include\game.h
# End Source File
# Begin Source File

SOURCE=..\include\game_menu.h
# End Source File
# Begin Source File

SOURCE=..\include\global.h
# End Source File
# Begin Source File

SOURCE=..\include\highscores.h
# End Source File
# Begin Source File

SOURCE=..\include\menu.h
# End Source File
# Begin Source File

SOURCE=..\include\menu_base.h
# End Source File
# Begin Source File

SOURCE=..\include\menu_demo_central.h
# End Source File
# Begin Source File

SOURCE=..\include\misc.h
# End Source File
# Begin Source File

SOURCE=..\include\multi_player.h
# End Source File
# Begin Source File

SOURCE=..\include\net_list.h
# End Source File
# Begin Source File

SOURCE=..\include\net_server.h
# End Source File
# Begin Source File

SOURCE=..\include\net_stuff.h
# End Source File
# Begin Source File

SOURCE=..\include\nglog.h
# End Source File
# Begin Source File

SOURCE=..\include\packets.h
# End Source File
# Begin Source File

SOURCE=..\include\pane.h
# End Source File
# Begin Source File

SOURCE=..\include\qserv.h
# End Source File
# Begin Source File

SOURCE=..\include\quadra.h
# End Source File
# Begin Source File

SOURCE=..\include\recording.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=..\include\sons.h
# End Source File
# Begin Source File

SOURCE=..\include\stats.h
# End Source File
# Begin Source File

SOURCE=..\include\texte.h
# End Source File
# Begin Source File

SOURCE=..\include\zone.h
# End Source File
# Begin Source File

SOURCE=..\include\zone_list.h
# End Source File
# End Group
# Begin Group "Resource files"

# PROP Default_Filter "wav;spr;raw;pcx;pcc;ico;idx;txt;rec"
# Begin Source File

SOURCE=.\images\window.ico
# End Source File
# End Group
# Begin Source File

SOURCE=.\Quadra.rc
# End Source File
# End Target
# End Project
