
Name "Hashish"
OutFile "Hashish-1.1b-setup.exe"

LicenseText "You must agree to this license before installing."
LicenseData "gpl.txt"

InstallDir "$PROGRAMFILES\Hashish"
InstallDirRegKey HKEY_LOCAL_MACHINE "SOFTWARE\ASLS\Hashish" ""

DirText "Select the directory to install Hashish in:"

Section "" ; (default section)
SetOutPath "$INSTDIR"

File gpl.txt
File Release\hashish.exe
File readme.txt
File change.txt
; add files / whatever that need to be installed here.
WriteRegStr HKEY_LOCAL_MACHINE "SOFTWARE\ASLS\Hashish" "" "$INSTDIR"
WriteRegStr HKEY_LOCAL_MACHINE "Software\Microsoft\Windows\CurrentVersion\Uninstall\Hashish" "DisplayName" "Hashish (remove only)"
WriteRegStr HKEY_LOCAL_MACHINE "Software\Microsoft\Windows\CurrentVersion\Uninstall\Hashish" "UninstallString" '"$INSTDIR\uninst.exe"'
; write out uninstaller
WriteUninstaller "$INSTDIR\uninst.exe"

MessageBox MB_YESNO|MB_ICONQUESTION "Create Start Menu Group?" IDNO NoSTARTMENU
CreateDirectory "$SMPROGRAMS\Hashish"
CreateShortCut "$SMPROGRAMS\Hashish\Hashish.lnk" "$INSTDIR\hashish.exe"
CreateShortCut "$SMPROGRAMS\Hashish\View ChangeLog.lnk" "$INSTDIR\change.txt"
CreateShortCut "$SMPROGRAMS\Hashish\View License.lnk" "$INSTDIR\gpl.txt" 
CreateShortCut "$SMPROGRAMS\Hashish\View Readme.lnk" "$INSTDIR\readme.txt" 
CreateShortCut "$SMPROGRAMS\Hashish\Uninstall Hashish.lnk" "$INSTDIR\uninst.exe" 

NOSTARTMENU:
MessageBox MB_YESNO|MB_ICONQUESTION "Create Desktop Icon?" IDNO NODESKTOP
CreateShortCut "$DESKTOP\Hashish.lnk" "$INSTDIR\hashish.exe" 


NODESKTOP:
SectionEnd ; end of default section


; begin uninstall settings/section
UninstallText "This will uninstall Hashish from your system"

Section Uninstall
; add delete commands to delete whatever files/registry keys/etc you installed here.
Delete "$INSTDIR\uninst.exe"
Delete "$INSTDIR\hashish.exe"
Delete "$INSTDIR\gpl.txt"
Delete "$INSTDIR\readme.txt"
Delete "$INSTDIR\change.txt"
RMDir "$SMPROGRAMS\Hashish"
Delete "$SMPROGRAMS\Hashish\Hashish.lnk" 
Delete "$SMPROGRAMS\Hashish\View License.lnk" 
Delete "$SMPROGRAMS\Hashish\View Readme.lnk"
Delete "$SMPROGRAMS\Hashish\View ChangeLog.lnk" 
Delete "$SMPROGRAMS\Hashish\Uninstall Hashish.lnk" 
Delete "$DESKTOP\Hashish.lnk" 

DeleteRegKey HKEY_LOCAL_MACHINE "SOFTWARE\ASLS\Hashish"
DeleteRegKey HKEY_LOCAL_MACHINE "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\Hashish"
RMDir /r "$INSTDIR"
SectionEnd ; end of uninstall section

; eof
