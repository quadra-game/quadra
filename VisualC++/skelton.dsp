# Microsoft Developer Studio Project File - Name="Skelton" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=Skelton - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Skelton.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Skelton.mak" CFG="Skelton - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Skelton - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Skelton - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Skelton - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /W3 /WX /GX /O2 /Ob2 /I "include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "UGS_DIRECTX" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0xc0c
# ADD RSC /l 0xc0c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "Skelton - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /W3 /Gi /GX /Zi /Od /Ob1 /Gf /I "include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "UGS_DIRECTX" /D "_CRTDBG_MAP_ALLOC" /Fr /YX /FD /c
# SUBTRACT CPP /Gy
# ADD BASE RSC /l 0xc0c
# ADD RSC /l 0xc0c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "Skelton - Win32 Release"
# Name "Skelton - Win32 Debug"
# Begin Group "Source files"

# PROP Default_Filter "cpp"
# Begin Source File

SOURCE=.\common\bitmap.cpp
# End Source File
# Begin Source File

SOURCE=.\common\buf.cpp
# End Source File
# Begin Source File

SOURCE=.\common\clipable.cpp
# End Source File
# Begin Source File

SOURCE=.\common\clock.cpp
# End Source File
# Begin Source File

SOURCE=.\common\command.cpp
# End Source File
# Begin Source File

SOURCE=.\common\crypt.cpp
# End Source File
# Begin Source File

SOURCE=.\common\cursor.cpp
# End Source File
# Begin Source File

SOURCE=.\common\cursor_self.cpp
# End Source File
# Begin Source File

SOURCE=.\common\debug.cpp
# End Source File
# Begin Source File

SOURCE=.\common\dict.cpp
# End Source File
# Begin Source File

SOURCE=.\directx\error.cpp
# End Source File
# Begin Source File

SOURCE=.\directx\find_file.cpp
# End Source File
# Begin Source File

SOURCE=.\common\http_post.cpp
# End Source File
# Begin Source File

SOURCE=.\common\http_request.cpp
# End Source File
# Begin Source File

SOURCE=.\common\id.cpp
# End Source File
# Begin Source File

SOURCE=.\directx\input.cpp
# End Source File
# Begin Source File

SOURCE=.\common\input_dumb.cpp
# End Source File
# Begin Source File

SOURCE=.\common\inter.cpp
# End Source File
# Begin Source File

SOURCE=.\common\listbox.cpp
# End Source File
# Begin Source File

SOURCE=.\directx\main.cpp
# End Source File
# Begin Source File

SOURCE=.\directx\music.cpp
# End Source File
# Begin Source File

SOURCE=.\common\net.cpp
# End Source File
# Begin Source File

SOURCE=.\common\notify.cpp
# End Source File
# Begin Source File

SOURCE=.\common\overmind.cpp
# End Source File
# Begin Source File

SOURCE=.\common\packet.cpp
# End Source File
# Begin Source File

SOURCE=.\directx\palette.cpp
# End Source File
# Begin Source File

SOURCE=.\common\pcx.cpp
# End Source File
# Begin Source File

SOURCE=.\common\pcx24.cpp
# End Source File
# Begin Source File

SOURCE=.\common\random.cpp
# End Source File
# Begin Source File

SOURCE=.\common\raw.cpp
# End Source File
# Begin Source File

SOURCE=.\directx\res.cpp
# End Source File
# Begin Source File

SOURCE=.\common\res_compress.cpp
# End Source File
# Begin Source File

SOURCE=.\common\resfile.cpp
# End Source File
# Begin Source File

SOURCE=.\common\resmanager.cpp
# End Source File
# Begin Source File

SOURCE=.\common\reswriter.cpp
# End Source File
# Begin Source File

SOURCE=.\directx\sound.cpp
# End Source File
# Begin Source File

SOURCE=.\directx\spawn.cpp
# End Source File
# Begin Source File

SOURCE=.\common\sprite.cpp
# End Source File
# Begin Source File

SOURCE=.\common\stringtable.cpp
# End Source File
# Begin Source File

SOURCE=.\common\unicode.cpp
# End Source File
# Begin Source File

SOURCE=.\common\url.cpp
# End Source File
# Begin Source File

SOURCE=.\directx\video.cpp
# End Source File
# Begin Source File

SOURCE=.\common\video_dumb.cpp
# End Source File
# Begin Source File

SOURCE=.\directx\video_dx16.cpp
# End Source File
# Begin Source File

SOURCE=.\directx\video_new.cpp
# End Source File
# Begin Source File

SOURCE=.\common\zone_text_clock.cpp
# End Source File
# End Group
# Begin Group "Header files"

# PROP Default_Filter "h"
# Begin Source File

SOURCE=.\include\array.h
# End Source File
# Begin Source File

SOURCE=.\include\bitmap.h
# End Source File
# Begin Source File

SOURCE=.\include\buf.h
# End Source File
# Begin Source File

SOURCE=.\include\clipable.h
# End Source File
# Begin Source File

SOURCE=.\include\clock.h
# End Source File
# Begin Source File

SOURCE=.\include\command.h
# End Source File
# Begin Source File

SOURCE=.\include\crypt.h
# End Source File
# Begin Source File

SOURCE=.\include\cursor.h
# End Source File
# Begin Source File

SOURCE=.\include\cursor_self.h
# End Source File
# Begin Source File

SOURCE=.\include\debug.h
# End Source File
# Begin Source File

SOURCE=.\include\dict.h
# End Source File
# Begin Source File

SOURCE=.\include\dynlib.h
# End Source File
# Begin Source File

SOURCE=.\include\error.h
# End Source File
# Begin Source File

SOURCE=.\include\find_file.h
# End Source File
# Begin Source File

SOURCE=.\include\http_post.h
# End Source File
# Begin Source File

SOURCE=.\include\http_request.h
# End Source File
# Begin Source File

SOURCE=.\include\id.h
# End Source File
# Begin Source File

SOURCE=.\include\image.h
# End Source File
# Begin Source File

SOURCE=.\include\input.h
# End Source File
# Begin Source File

SOURCE=.\include\input_dx.h
# End Source File
# Begin Source File

SOURCE=.\include\input_keys.h
# End Source File
# Begin Source File

SOURCE=.\include\inter.h
# End Source File
# Begin Source File

SOURCE=.\include\listbox.h
# End Source File
# Begin Source File

SOURCE=.\include\main.h
# End Source File
# Begin Source File

SOURCE=.\include\music.h
# End Source File
# Begin Source File

SOURCE=.\include\net.h
# End Source File
# Begin Source File

SOURCE=.\include\net_buf.h
# End Source File
# Begin Source File

SOURCE=.\include\net_call.h
# End Source File
# Begin Source File

SOURCE=.\include\notify.h
# End Source File
# Begin Source File

SOURCE=.\include\overmind.h
# End Source File
# Begin Source File

SOURCE=.\include\packet.h
# End Source File
# Begin Source File

SOURCE=.\include\palette.h
# End Source File
# Begin Source File

SOURCE=.\include\pcx.h
# End Source File
# Begin Source File

SOURCE=.\include\pcx24.h
# End Source File
# Begin Source File

SOURCE=.\include\random.h
# End Source File
# Begin Source File

SOURCE=.\include\raw.h
# End Source File
# Begin Source File

SOURCE=.\include\res.h
# End Source File
# Begin Source File

SOURCE=.\include\res_compress.h
# End Source File
# Begin Source File

SOURCE=.\include\resfile.h
# End Source File
# Begin Source File

SOURCE=.\include\resmanager.h
# End Source File
# Begin Source File

SOURCE=.\include\sound.h
# End Source File
# Begin Source File

SOURCE=.\include\spawn.h
# End Source File
# Begin Source File

SOURCE=.\include\sprite.h
# End Source File
# Begin Source File

SOURCE=.\include\stringtable.h
# End Source File
# Begin Source File

SOURCE=.\include\surface.h
# End Source File
# Begin Source File

SOURCE=.\include\svgakeys.h
# End Source File
# Begin Source File

SOURCE=.\include\track.h
# End Source File
# Begin Source File

SOURCE=.\include\types.h
# End Source File
# Begin Source File

SOURCE=.\include\unicode.h
# End Source File
# Begin Source File

SOURCE=.\include\url.h
# End Source File
# Begin Source File

SOURCE=.\include\utils.h
# End Source File
# Begin Source File

SOURCE=.\include\video.h
# End Source File
# Begin Source File

SOURCE=.\include\video_dx.h
# End Source File
# Begin Source File

SOURCE=.\include\video_dx16.h
# End Source File
# Begin Source File

SOURCE=.\include\zone_text_clock.h
# End Source File
# End Group
# End Target
# End Project
