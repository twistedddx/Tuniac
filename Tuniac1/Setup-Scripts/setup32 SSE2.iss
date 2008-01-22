#define DateTime GetDateTimeString('yymmdd','','')

[Setup]
AppName=Tuniac
AppVerName=Tuniac (Beta)
AppPublisher=Tuniac Dev Team
AppPublisherURL=http://www.tuniac.com
AppSupportURL=http://www.tuniac.com
AppUpdatesURL=http://www.tuniac.com
DefaultDirName={pf}\Tuniac
DefaultGroupName=Tuniac
AllowNoIcons=true
OutputDir=.
OutputBaseFilename=..\Tuniac_Setup32_SSE2_{#DateTime}
SetupIconFile=..\TuniacApp\icons\tuniac.ico
Compression=lzma/ultra
SolidCompression=true
Uninstallable=true
InternalCompressLevel=ultra
MinVersion=0,5.0.2195
AppID={{A2A3A9DE-A195-4A66-8DA6-59968E0EF943}
ShowLanguageDialog=yes
ShowTasksTreeLines=true
ShowComponentSizes=true
WizardImageFile=WizardImage.bmp
WizardSmallImageFile=WizardImageSmall.bmp

[Tasks]
Name: desktopicon; Description: {cm:CreateDesktopIcon}; GroupDescription: {cm:AdditionalIcons}; Flags: unchecked
Name: quicklaunchicon; Description: {cm:CreateQuickLaunchIcon}; GroupDescription: {cm:AdditionalIcons}; Flags: unchecked

[InstallDelete]
Name: {app}\Guide\*.html; Type: files; Components: Guide
Name: {app}\Guide\*.css; Type: files; Components: Guide
Name: {app}\Guide\Images\*.jpg; Type: files; Components: Guide

[Files]
Source: ..\Win32\Release SSE2\TuniacApp.exe; DestDir: {app}\; Flags: ignoreversion
Source: ..\TuniacApp\icons\*.ico; DestDir: {app}\iconsets\; Flags: recursesubdirs
Source: ..\Win32\Release SSE2\std_InfoManager.dll; DestDir: {app}\; Flags: ignoreversion;
Source: ..\Win32\Release SSE2\visuals\*.dll; DestDir: {app}\visuals; Flags: ignoreversion; Components: Visuals
Source: ..\Win32\Release SSE2\mp3_Plugin.dll; DestDir: {app}\; Flags: ignoreversion; Components: Decoder_MP3
Source: ..\Win32\Release SSE2\ogg_Plugin.dll; DestDir: {app}\; Flags: ignoreversion; Components: Decoder_OGG
Source: ..\Win32\Release SSE2\ape_Plugin.dll; DestDir: {app}\; Flags: ignoreversion; Components: Decoder_APE
Source: ..\Win32\Release SSE2\m4a_Plugin.dll; DestDir: {app}\; Flags: ignoreversion; Components: Decoder_M4A
Source: ..\Win32\Release SSE2\mp3streamer_Plugin.dll; DestDir: {app}\; Flags: ignoreversion; Components: Decoder_MP3Streamer
Source: ..\Win32\Release SSE2\importexport\m3u_Plugin.dll; DestDir: {app}\importexport\; Flags: ignoreversion; Components: ImportExport_M3U
Source: ..\Win32\Release SSE2\importexport\pls_Plugin.dll; DestDir: {app}\importexport\; Flags: ignoreversion; Components: ImportExport_PLS
Source: ..\Win32\Release SSE2\plugins\MSNInfo_Plugin.dll; DestDir: {app}\plugins\; Flags: ignoreversion; Components: Plugin_MSNInfo
Source: ..\Win32\Release SSE2\plugins\NowPlayingTxt_Plugin.dll; DestDir: {app}\plugins\; Flags: ignoreversion; Components: Plugin_NowPlayingTxt
Source: ..\Win32\Release SSE2\plugins\PopupNotify_plugin.dll; DestDir: {app}\plugins\; Flags: ignoreversion; Components: Plugin_PopupNotify
Source: ..\Guide\*; DestDir: {app}\Guide\; Flags: recursesubdirs createallsubdirs; Components: Guide

[Registry]
Root: HKLM; Subkey: "SOFTWARE\Microsoft\Windows\CurrentVersion\App Paths\TuniacApp.exe"; ValueType: string; ValueName: ""; ValueData: "{app}\TuniacApp.exe"

[Icons]
Name: {group}\Tuniac; Filename: {app}\TuniacApp.exe
Name: {group}\{cm:UninstallProgram,Tuniac}; Filename: {uninstallexe}
Name: {userdesktop}\Tuniac; Filename: {app}\TuniacApp.exe; Tasks: desktopicon
Name: {userappdata}\Microsoft\Internet Explorer\Quick Launch\Tuniac; Filename: {app}\TuniacApp.exe; Tasks: quicklaunchicon

[Run]
Filename: {app}\TuniacApp.exe; Description: {cm:LaunchProgram,Tuniac}; Flags: nowait postinstall skipifsilent

[Components]
Name: Guide; Description: Tuniac Guide; Types: custom full
Name: Visuals; Description: Visuals; Types: custom full
Name: Themes; Description: Additional themes; Types: custom full
Name: Decoder_MP3; Description: MP3 playback support; Types: custom compact full
Name: Decoder_OGG; Description: OGG playback support; Types: custom full
Name: Decoder_APE; Description: APE playback support; Types: custom full
Name: Decoder_M4A; Description: M4A/MP4 playback support; Types: custom full
Name: Decoder_MP3Streamer; Description: MP3 Streaming playback support; Types: custom full
Name: ImportExport_M3U; Description: M3U importing and exporting; Types: custom compact full
Name: ImportExport_PLS; Description: PLS importing and exporting; Types: custom compact full
Name: Plugin_NowPlayingTxt; Description: Now-Playing Text plugin; Types: custom full
Name: Plugin_PopupNotify; Description: Popup Notify plugin; Types: custom full
Name: Plugin_MSNInfo; Description: MSN v7+ Info plugin; Types: custom


[Code]
procedure CurUninstallStepChanged(CurUninstallStep: TUninstallStep);
var
  ResultCode: Integer;
begin
	case CurUninstallStep of
		usUninstall:
		begin
			if MsgBox('Remove all settings?', mbInformation, mb_YesNo) = idYes then
			begin
                if not Exec(ExpandConstant('{app}\TuniacApp.exe'), '-nosaveprefs -wipeprefs -wipefileassoc -exit', '', SW_HIDE, ewWaitUntilTerminated, ResultCode) then
				    MsgBox('Error removing settings & file associations.', mbError, MB_OK);
			end
			else
			begin
			    if not Exec(ExpandConstant('{app}\TuniacApp.exe'), '-nosaveprefs -wipefileassoc -exit', '', SW_HIDE, ewWaitUntilTerminated, ResultCode) then
					MsgBox('Error removing file associations.', mbError, MB_OK);
			end;
		end;
	end;
end;
