; Simple install script for Quadra
; Based on example2.nsi

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

; optional section (can be disabled by the user)
Section "Start Menu Shortcuts"

  CreateDirectory "$SMPROGRAMS\Quadra"
  CreateShortCut "$SMPROGRAMS\Quadra\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
  CreateShortCut "$SMPROGRAMS\Quadra\Quadra.lnk" "$INSTDIR\quadra.exe" "" "$INSTDIR\quadra.exe" 0
  
SectionEnd

;--------------------------------

; Uninstaller

UninstallText "This will uninstall Quadra. Hit next to continue or cancel if you've come to your senses."

; Uninstall section

Section "Uninstall"
  
  ; remove registry keys
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Quadra"
  DeleteRegKey HKLM SOFTWARE\Quadra

  ; remove files and uninstaller
  Delete $INSTDIR\readme-win32.txt
  Delete $INSTDIR\quadra.exe
  Delete $INSTDIR\quadra.res
  Delete $INSTDIR\uninstall.exe

  ; remove shortcuts, if any
  Delete "$SMPROGRAMS\Quadra\*.*"

  ; remove directories used
  RMDir "$SMPROGRAMS\Quadra"
  RMDir "$INSTDIR"

SectionEnd
