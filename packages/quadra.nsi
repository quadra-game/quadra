; Simple install script for Quadra
; Based on example2.nsi

; for SF_SELECTED
!include Sections.nsh

; The name of the installer
Name "Quadra"

; The file to write
OutFile "..\quadra-1.1.9.exe"

Icon "winzip_icone.ico"
UninstallIcon "winzip_icone.ico"

ShowInstDetails show

; The default installation directory
InstallDir $PROGRAMFILES\Quadra

; Registry key to check for directory (so if you install again, it will 
; overwrite the old one automatically)
InstallDirRegKey HKLM SOFTWARE\Quadra "InstallDir"

LicenseText "Installation instructions:" "Continue"
LicenseData "readme-win32.txt"

; The text to prompt the user to enter a directory
ComponentText "This will install Quadra on your computer. Select which optional things you want installed."

; The text to prompt the user to enter a directory
DirText "Choose a directory to install in to:"

;--------------------------------

; The stuff to install
Section "Quadra (required)"
  SectionIn RO
  
  ; Set output path to the installation directory.
  SetOutPath $INSTDIR
  
  ; Put file there
  File "readme-win32.txt"
  File "..\quadra.exe"
  File "..\quadra.res"
  
  ; Write the installation path into the registry
  WriteRegStr HKLM SOFTWARE\Quadra "InstallDir" "$INSTDIR"
  
  ; Write the uninstall keys for Windows
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Quadra" "DisplayName" "Quadra (remove only)"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Quadra" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteUninstaller "uninstall.exe"
SectionEnd

Section "QSnoop (recommanded)" QSnoopSectionIndex
	File "QSnoop.exe"
	File "QS.dll"
	File "QSEn.dll"
	File "QSFr.dll"
SectionEnd	

Section "Start Menu Shortcuts (recommanded)"
  CreateDirectory "$SMPROGRAMS\Quadra"
  CreateShortCut "$SMPROGRAMS\Quadra\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
  CreateShortCut "$SMPROGRAMS\Quadra\Quadra.lnk" "$INSTDIR\quadra.exe" "" "$INSTDIR\quadra.exe" 0

  ; install QSnoop shortcut only when the QSnoop section is selected
  SectionGetFlags ${QSnoopSectionIndex} $R0
  IntOp $R0 $R0 & ${SF_SELECTED}
  IntCmp $R0 ${SF_SELECTED} 0 skip
    CreateShortCut "$SMPROGRAMS\Quadra\QSnoop.lnk" "$INSTDIR\QSnoop.exe" "" "$INSTDIR\QSnoop.exe" 0
  skip:
SectionEnd

;--------------------------------

; Uninstaller
UninstallText "This will uninstall Quadra. Hit next to continue or cancel if you've come to your senses."

; Uninstall section
Section "Uninstall"
  ; remove files
  Delete $INSTDIR\readme-win32.txt
  Delete $INSTDIR\quadra.exe
  Delete $INSTDIR\quadra.res
  Delete $INSTDIR\QSnoop.exe
  Delete $INSTDIR\QS.dll
  Delete $INSTDIR\QSEn.dll
  Delete $INSTDIR\QSFr.dll

  ; remove shortcuts, if they exist
  Delete "$SMPROGRAMS\Quadra\Quadra.lnk"
  Delete "$SMPROGRAMS\Quadra\QSnoop.lnk"
  Delete "$SMPROGRAMS\Quadra\Uninstall.lnk"
  RMDir "$SMPROGRAMS\Quadra"

  Delete $INSTDIR\uninstall.exe

  ; remove uninstall registry key for Windows
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Quadra"

  ; remove install directory
  ; TODO: move Quadra and QSnoop configuration to the registry and demos to "Application data" to make this work
  RMDir "$INSTDIR"
SectionEnd
