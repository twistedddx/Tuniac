#define DateTime GetDateTimeString('yymmdd','','')

#define TuniacName 'Tuniac 1.0'

#define TuniacWWW 'http://www.tuniac.org'

#define VC14RedistText 'Microsoft Visual C++ 2017 Update 3'
#define VC14RedistManual 'https://www.visualstudio.com/downloads/?q=Redistributable'

#define VC14x86Redist 'https://download.visualstudio.microsoft.com/download/pr/11687613/88b50ce70017bf10f2d56d60fcba6ab1/VC_redist.x86.exe'
#define VC14x86RedistMIRROR 'http://www.tuniac.org/extra/Redist/VC_redist.x86.exe'
#define VC14x86RedistdwMajor 14
#define VC14x86RedistdwMinor 13
#define VC14x86RedistdwBld 26020
#define VC14x86RedistSize 14

#define VC14x64Redist 'https://download.visualstudio.microsoft.com/download/pr/11687625/2cd2dba5748dc95950a5c42c2d2d78e4/VC_redist.x64.exe'
#define VC14x64RedistMIRROR 'http://www.tuniac.org/extra/Redist/VC_redist.x64.exe'
#define VC14x64RedistdwMajor 14
#define VC14x64RedistdwMinor 13
#define VC14x64RedistdwBld 26020
#define VC14x64RedistSize 15

#define DXJun2010XAudioText 'DirectX XAudio 2.7'
#define DXJun2010Text 'DirectX End-User Runtimes (June 2010)'
#define DXJun2010Manual 'http://www.microsoft.com/en-au/download/details.aspx?id=8109'                        
#define DXJun2010DSETUP 'http://www.tuniac.org/extra/DirectX/DSETUP.dll'
#define DXJun2010dsetup32 'http://www.tuniac.org/extra/DirectX/dsetup32.dll'
#define DXJun2010dxdllreg_x86 'http://www.tuniac.org/extra/DirectX/dxdllreg_x86.cab'
#define DXJun2010DXSETUP 'http://www.tuniac.org/extra/DirectX/DXSETUP.exe'
#define DXJun2010dxupdate 'http://www.tuniac.org/extra/DirectX/dxupdate.cab'
#define DXJun2010Jun2010_XAudio_x64 'http://www.tuniac.org/extra/DirectX/Jun2010_XAudio_x64.cab'
#define DXJun2010Jun2010_XAudio_x86 'http://www.tuniac.org/extra/DirectX/Jun2010_XAudio_x86.cab'
#define DXJun2010Size = 3

[Setup]
AllowNoIcons=yes
AppID={{A2A3A9DE-A195-4A66-8DA6-59968E0EF943}
AppMutex=TUNIACWINDOWCLASS
AppName={#TuniacName}
AppPublisher=Tuniac Dev Team
AppPublisherURL={#TuniacWWW}
AppSupportURL={#TuniacWWW}
AppUpdatesURL={#TuniacWWW}
AppVerName={#TuniacName}
Compression=lzma2/ultra
DefaultDirName={pf}\Tuniac
DefaultGroupName=Tuniac
DisableDirPage=No
DisableProgramGroupPage=No
DisableWelcomePage=No
InternalCompressLevel=ultra
OutputDir=.
SetupIconFile=..\TuniacApp\icons\tuniac.ico
SetupMutex=TuniacSetup,Global\TuniacSetup
ShowTasksTreeLines=yes
SolidCompression=yes
UninstallFilesDir={app}\uninstaller
WizardImageFile=WizardImage.bmp
WizardSmallImageFile=WizardImageSmall.bmp

[Tasks]
Name: desktopicon; Description: {cm:CreateDesktopIcon}; GroupDescription: {cm:AdditionalIcons}; Flags: unchecked
Name: quicklaunchicon; Description: {cm:CreateQuickLaunchIcon}; GroupDescription: {cm:AdditionalIcons}; Flags: unchecked

[InstallDelete]
Name: {app}\Guide\*.html; Type: files
Name: {app}\Guide\*.css; Type: files
Name: {app}\Guide\Images\*.jpg; Type: files
Name: {app}\unins*.*; Type: files
Name: {app}\bass\bass_alac.dll; Type: files

[Files]
Source: "WizardImageSmall.bmp"; Flags: dontcopy

Source: "..\TuniacApp\images\NoAlbumArt.jpg"; DestDir: {app}\; Flags: ignoreversion
Source: "..\Housekeeping\Change Log.txt"; DestDir: {app}\; Flags: ignoreversion
Source: "..\Housekeeping\gpl.txt"; DestDir: {app}\; Flags: ignoreversion
Source: "..\Housekeeping\lgpl.txt"; DestDir: {app}\; Flags: ignoreversion
Source: "..\TuniacApp\icons\*.ico"; DestDir: {app}\iconsets\; Flags: ignoreversion recursesubdirs

[Run]
Filename: "{tmp}\DXSETUP.exe"; StatusMsg: "Installing {#DXJun2010XAudioText}...(Please wait!)"; Parameters: "/silent"; Flags: skipifdoesntexist;
Filename: "{tmp}\vcredist_x86(2017).exe"; StatusMsg: "Installing Microsoft {#VC14RedistText} x86 Runtime... (Please wait!)"; Parameters: "/q /norestart"; Flags: skipifdoesntexist;
Filename: "{tmp}\vcredist_x64(2017).exe"; StatusMsg: "Installing Microsoft {#VC14RedistText} x64 Runtime... (Please wait!)"; Parameters: "/q /norestart"; Flags: skipifdoesntexist;
Filename: {app}\TuniacApp.exe; Description: {cm:LaunchProgram,Tuniac}; Flags: nowait postinstall skipifsilent

[Registry]
Root: HKLM; Subkey: "SOFTWARE\Microsoft\Windows\CurrentVersion\App Paths\TuniacApp.exe"; ValueType: string; ValueName: ""; ValueData: "{app}\TuniacApp.exe"
Root: HKCU; Subkey: "SOFTWARE\MediaScience\Sonique\General Preferences 0.80"; ValueType: string; ValueName: "SoniquePath"; ValueData: "{app}\visuals\"
Root: HKCU; Subkey: "SOFTWARE\Microsoft\Keyboard\Native Media Players\TuniacApp"; ValueType: string; ValueName: "AppName"; ValueData: "TuniacApp"
Root: HKCU; Subkey: "SOFTWARE\Microsoft\Keyboard\Native Media Players\TuniacApp"; ValueType: string; ValueName: "ExePath"; ValueData: "{app}\TuniacApp.exe"

[Icons]
Name: {group}\Tuniac; Filename: {app}\TuniacApp.exe; Comment: "Launch Tuniac"
Name: {group}\Data Folder; Filename: {userappdata}\Tuniac; Comment: "Data Folder"
Name: {group}\Uninstaller\{cm:UninstallProgram,Tuniac}; Filename: {uninstallexe}; Comment: "Uninstall Tuniac"
Name: {userdesktop}\Tuniac; Filename: {app}\TuniacApp.exe; Comment: "Launch Tuniac"; Tasks: desktopicon
Name: {userappdata}\Microsoft\Internet Explorer\Quick Launch\Tuniac; Filename: {app}\TuniacApp.exe; Comment: "Launch Tuniac"; Tasks: quicklaunchicon
Name: {app}\Data Folder; Filename: {userappdata}\Tuniac; Comment: "Data Folder"

[Code]
procedure AboutButtonOnClick(Sender: TObject);
begin
  SuppressibleMsgBox('This installer will install the {#TuniacName} media player onto your computer', mbInformation, mb_Ok, IDOK);
end;

procedure URLLabelOnClick(Sender: TObject);
var
  ErrorCode: Integer;
begin
  ShellExec('open', '{#TuniacWWW}', '', '', SW_SHOWNORMAL, ewNoWait, ErrorCode);
end;

procedure CurUninstallStepChanged(CurUninstallStep: TUninstallStep);
var
  ResultCode: Integer;
begin
	case CurUninstallStep of
		usUninstall:
		begin
			if SuppressibleMsgBox('Remove all settings?', mbInformation, mb_YesNo, IDYES) = IDYES then begin
        if not Exec(ExpandConstant('{app}\TuniacApp.exe'), '-dontsaveprefs -wipeprefs -wipefileassoc -exit', '', SW_HIDE, ewWaitUntilTerminated, ResultCode) then
				  SuppressibleMsgBox('Error removing settings & file associations.', mbError, MB_OK, IDOK);
			end	else begin
			  if not Exec(ExpandConstant('{app}\TuniacApp.exe'), '-dontsaveprefs -wipefileassoc -exit', '', SW_HIDE, ewWaitUntilTerminated, ResultCode) then
					SuppressibleMsgBox('Error removing file associations.', mbError, MB_OK, IDOK);
			end;
		end;
	end;
end;
