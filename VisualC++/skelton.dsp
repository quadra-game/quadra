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
# ADD CPP /nologo /W3 /WX /GX /O2 /Ob2 /I "..\skelton\include" /I "..\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "UGS_DIRECTX" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0xc0c
# ADD RSC /l 0xc0c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:".\Release\skelton.lib"

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
# ADD CPP /nologo /W3 /Gi /GX /Zi /Od /Ob1 /Gf /I "..\skelton\include" /I "..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "UGS_DIRECTX" /D "_CRTDBG_MAP_ALLOC" /Fr /YX /FD /c
# SUBTRACT CPP /Gy
# ADD BASE RSC /l 0xc0c
# ADD RSC /l 0xc0c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:".\Debug\skelton.lib"

!ENDIF 

# Begin Target

# Name "Skelton - Win32 Release"
# Name "Skelton - Win32 Debug"
# Begin Group "Source files"

# PROP Default_Filter "cpp"
# Begin Source File

SOURCE=..\skelton\common\bitmap.cpp
# End Source File
# Begin Source File

SOURCE=..\skelton\common\buf.cpp
# End Source File
# Begin Source File

SOURCE=..\skelton\common\clipable.cpp
# End Source File
# Begin Source File

SOURCE=..\skelton\common\clock.cpp
# End Source File
# Begin Source File

SOURCE=..\skelton\common\command.cpp
# End Source File
# Begin Source File

SOURCE=..\skelton\common\crypt.cpp
# End Source File
# Begin Source File

SOURCE=..\skelton\common\cursor.cpp
# End Source File
# Begin Source File

SOURCE=..\skelton\common\cursor_self.cpp
# End Source File
# Begin Source File

SOURCE=..\skelton\common\debug.cpp
# End Source File
# Begin Source File

SOURCE=..\skelton\common\dict.cpp
# End Source File
# Begin Source File

SOURCE=..\skelton\directx\error.cpp
# End Source File
# Begin Source File

SOURCE=..\skelton\directx\find_file.cpp
# End Source File
# Begin Source File

SOURCE=..\skelton\common\http_post.cpp
# End Source File
# Begin Source File

SOURCE=..\skelton\common\http_request.cpp
# End Source File
# Begin Source File

SOURCE=..\skelton\common\id.cpp
# End Source File
# Begin Source File

SOURCE=..\skelton\common\image_png.cpp
# End Source File
# Begin Source File

SOURCE=..\skelton\directx\input.cpp
# End Source File
# Begin Source File

SOURCE=..\skelton\common\input_dumb.cpp
# End Source File
# Begin Source File

SOURCE=..\skelton\common\inter.cpp
# End Source File
# Begin Source File

SOURCE=..\skelton\common\listbox.cpp
# End Source File
# Begin Source File

SOURCE=..\skelton\directx\main.cpp
# End Source File
# Begin Source File

SOURCE=..\skelton\directx\music.cpp
# End Source File
# Begin Source File

SOURCE=..\skelton\common\net.cpp
# End Source File
# Begin Source File

SOURCE=..\skelton\common\notify.cpp
# End Source File
# Begin Source File

SOURCE=..\skelton\common\overmind.cpp
# End Source File
# Begin Source File

SOURCE=..\skelton\common\packet.cpp
# End Source File
# Begin Source File

SOURCE=..\skelton\directx\palette.cpp
# End Source File
# Begin Source File

SOURCE=..\skelton\common\pcx.cpp
# End Source File
# Begin Source File

SOURCE=..\skelton\common\random.cpp
# End Source File
# Begin Source File

SOURCE=..\skelton\common\raw.cpp
# End Source File
# Begin Source File

SOURCE=..\skelton\directx\res.cpp
# End Source File
# Begin Source File

SOURCE=..\skelton\common\res_compress.cpp
# End Source File
# Begin Source File

SOURCE=..\skelton\common\resfile.cpp
# End Source File
# Begin Source File

SOURCE=..\skelton\common\resmanager.cpp
# End Source File
# Begin Source File

SOURCE=..\skelton\common\reswriter.cpp
# End Source File
# Begin Source File

SOURCE=..\skelton\directx\sound.cpp
# End Source File
# Begin Source File

SOURCE=..\skelton\directx\spawn.cpp
# End Source File
# Begin Source File

SOURCE=..\skelton\common\sprite.cpp
# End Source File
# Begin Source File

SOURCE=..\skelton\common\stringtable.cpp
# End Source File
# Begin Source File

SOURCE=..\skelton\common\unicode.cpp
# End Source File
# Begin Source File

SOURCE=..\skelton\common\url.cpp
# End Source File
# Begin Source File

SOURCE=..\skelton\directx\video.cpp
# End Source File
# Begin Source File

SOURCE=..\skelton\common\video_dumb.cpp
# End Source File
# Begin Source File

SOURCE=..\skelton\directx\video_new.cpp
# End Source File
# Begin Source File

SOURCE=..\skelton\common\zone_text_clock.cpp
# End Source File
# End Group
# Begin Group "Header files"

# PROP Default_Filter "h"
# Begin Source File

SOURCE=..\skelton\include\array.h
# End Source File
# Begin Source File

SOURCE=..\skelton\include\bitmap.h
# End Source File
# Begin Source File

SOURCE=..\skelton\include\buf.h
# End Source File
# Begin Source File

SOURCE=..\skelton\include\clipable.h
# End Source File
# Begin Source File

SOURCE=..\skelton\include\clock.h
# End Source File
# Begin Source File

SOURCE=..\skelton\include\command.h
# End Source File
# Begin Source File

SOURCE=..\skelton\include\crypt.h
# End Source File
# Begin Source File

SOURCE=..\skelton\include\cursor.h
# End Source File
# Begin Source File

SOURCE=..\skelton\include\cursor_self.h
# End Source File
# Begin Source File

SOURCE=..\skelton\include\debug.h
# End Source File
# Begin Source File

SOURCE=..\skelton\include\dict.h
# End Source File
# Begin Source File

SOURCE=..\skelton\include\dynlib.h
# End Source File
# Begin Source File

SOURCE=..\skelton\include\error.h
# End Source File
# Begin Source File

SOURCE=..\skelton\include\find_file.h
# End Source File
# Begin Source File

SOURCE=..\skelton\include\http_post.h
# End Source File
# Begin Source File

SOURCE=..\skelton\include\http_request.h
# End Source File
# Begin Source File

SOURCE=..\skelton\include\id.h
# End Source File
# Begin Source File

SOURCE=..\skelton\include\image.h
# End Source File
# Begin Source File

SOURCE=..\skelton\include\image_png.h
# End Source File
# Begin Source File

SOURCE=..\skelton\include\input.h
# End Source File
# Begin Source File

SOURCE=..\skelton\include\input_dx.h
# End Source File
# Begin Source File

SOURCE=..\skelton\include\input_keys.h
# End Source File
# Begin Source File

SOURCE=..\skelton\include\inter.h
# End Source File
# Begin Source File

SOURCE=..\skelton\include\listbox.h
# End Source File
# Begin Source File

SOURCE=..\skelton\include\main.h
# End Source File
# Begin Source File

SOURCE=..\skelton\include\music.h
# End Source File
# Begin Source File

SOURCE=..\skelton\include\net.h
# End Source File
# Begin Source File

SOURCE=..\skelton\include\net_buf.h
# End Source File
# Begin Source File

SOURCE=..\skelton\include\net_call.h
# End Source File
# Begin Source File

SOURCE=..\skelton\include\notify.h
# End Source File
# Begin Source File

SOURCE=..\skelton\include\overmind.h
# End Source File
# Begin Source File

SOURCE=..\skelton\include\packet.h
# End Source File
# Begin Source File

SOURCE=..\skelton\include\palette.h
# End Source File
# Begin Source File

SOURCE=..\skelton\include\pcx.h
# End Source File
# Begin Source File

SOURCE=..\skelton\include\random.h
# End Source File
# Begin Source File

SOURCE=..\skelton\include\raw.h
# End Source File
# Begin Source File

SOURCE=..\skelton\include\res.h
# End Source File
# Begin Source File

SOURCE=..\skelton\include\res_compress.h
# End Source File
# Begin Source File

SOURCE=..\skelton\include\resfile.h
# End Source File
# Begin Source File

SOURCE=..\skelton\include\resmanager.h
# End Source File
# Begin Source File

SOURCE=..\skelton\include\sound.h
# End Source File
# Begin Source File

SOURCE=..\skelton\include\spawn.h
# End Source File
# Begin Source File

SOURCE=..\skelton\include\sprite.h
# End Source File
# Begin Source File

SOURCE=..\skelton\include\stringtable.h
# End Source File
# Begin Source File

SOURCE=..\skelton\include\surface.h
# End Source File
# Begin Source File

SOURCE=..\skelton\include\svgakeys.h
# End Source File
# Begin Source File

SOURCE=..\skelton\include\track.h
# End Source File
# Begin Source File

SOURCE=..\skelton\include\types.h
# End Source File
# Begin Source File

SOURCE=..\skelton\include\unicode.h
# End Source File
# Begin Source File

SOURCE=..\skelton\include\url.h
# End Source File
# Begin Source File

SOURCE=..\skelton\include\utils.h
# End Source File
# Begin Source File

SOURCE=..\skelton\include\video.h
# End Source File
# Begin Source File

SOURCE=..\skelton\include\video_dx.h
# End Source File
# Begin Source File

SOURCE=..\skelton\include\zone_text_clock.h
# End Source File
# End Group
# End Target
# End Project
