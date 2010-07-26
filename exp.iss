; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{4739C3FB-74C1-411C-A97A-71F73F7E5671}
AppName=WZGM-TrunkBuild
AppVersion=svn
;AppVerName=Warzone2100GraphicsMods-BergExpTrunkBuild svn-build-0001
AppPublisher=WZGM
AppPublisherURL=http://www.wzgm.net
AppSupportURL=http://www.wzgm.net
AppUpdatesURL=http://www.wzgm.net
DefaultDirName={pf}\data
DefaultGroupName=WZGM-EXP
AllowNoIcons=yes
OutputDir=data\installer
OutputBaseFilename=setup
Compression=lzma
SolidCompression=yes

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
Source: "data\Warzone2100-Dbg.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "data\base\*"; DestDir: "{app}\base"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "data\etc\*"; DestDir: "{app}\etc"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "data\fonts\*"; DestDir: "{app}\fonts"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "data\mods\*"; DestDir: "{app}\mods"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "data\mp\*"; DestDir: "{app}\mp"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "data\music\*"; DestDir: "{app}\music"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "data\freetype6.dll"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "data\glc32.dll"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "data\libexpat.dll*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "data\libfontconfig-1.dll"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "data\libiconv-2.dll"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "data\libintl-2.dll"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "data\popt1.dll"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "data\SDL.dll"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "data\zlib1.dll"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs
; NOTE: Don't use "Flags: ignoreversion" on any shared system files

[Icons]
Name: "{group}\Warzone2100GraphicsMods-BergExpTrunkBuild"; Filename: "{app}\Warzone2100-Dbg.exe"
Name: "{group}\{cm:UninstallProgram,Warzone2100GraphicsMods-BergExpTrunkBuild}"; Filename: "{uninstallexe}"
Name: "{commondesktop}\Warzone2100GraphicsMods-BergExpTrunkBuild"; Filename: "{app}\Warzone2100-Dbg.exe"; Tasks: desktopicon

[Run]
Filename: "{app}\Warzone2100-Dbg.exe"; Description: "{cm:LaunchProgram,Warzone2100GraphicsMods-BergExpTrunkBuild}"; Flags: nowait postinstall skipifsilent
