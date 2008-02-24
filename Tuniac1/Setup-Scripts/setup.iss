#define DateTime GetDateTimeString('yymmdd','','')

[Setup]
AppName=Tuniac
AppVerName=Tuniac (Beta)
AppPublisher=Tuniac Dev Team
AppPublisherURL=http://www.tuniac.com
AppSupportURL=http://www.tuniac.com
AppUpdatesURL=http://www.tuniac.com
AppMutex=TUNIACWINDOWCLASS
ArchitecturesInstallIn64BitMode=x64
DefaultDirName={pf}\Tuniac
DefaultGroupName=Tuniac
AllowNoIcons=yes
OutputDir=.
OutputBaseFilename=..\Tuniac_Setup_{#DateTime}
SetupIconFile=..\TuniacApp\icons\tuniac.ico
Compression=lzma/ultra
SolidCompression=yes
InternalCompressLevel=ultra
MinVersion=0,5.0.2195
AppID={{A2A3A9DE-A195-4A66-8DA6-59968E0EF943}
ShowTasksTreeLines=yes
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
Source: ..\TuniacApp\icons\*.ico; DestDir: {app}\iconsets\; Flags: ignoreversion recursesubdirs
Source: ..\Guide\*; DestDir: {app}\Guide\; Flags: ignoreversion recursesubdirs createallsubdirs; Components: Guide

Source: ..\x64\Release\TuniacApp.exe; DestDir: {app}\; Check: Is64BitInstallMode; Flags: ignoreversion
Source: ..\x64\Release\std_InfoManager.dll; DestDir: {app}\; Check: Is64BitInstallMode; Flags: ignoreversion
Source: ..\x64\Release\mp3_Plugin.dll; DestDir: {app}\; Check: Is64BitInstallMode; Flags: ignoreversion; Components: Decoder_MP3
Source: ..\x64\Release\ogg_Plugin.dll; DestDir: {app}\; Check: Is64BitInstallMode; Flags: ignoreversion; Components: Decoder_OGG
Source: ..\x64\Release\ape_Plugin.dll; DestDir: {app}\; Check: Is64BitInstallMode; Flags: ignoreversion; Components: Decoder_APE
Source: ..\x64\Release\m4a_Plugin.dll; DestDir: {app}\; Check: Is64BitInstallMode; Flags: ignoreversion; Components: Decoder_M4A
Source: ..\x64\Release\mp3streamer_Plugin.dll; DestDir: {app}\; Check: Is64BitInstallMode; Flags: ignoreversion; Components: Decoder_MP3Streamer
Source: ..\x64\Release\importexport\m3u_Plugin.dll; DestDir: {app}\importexport\; Check: Is64BitInstallMode; Flags: ignoreversion; Components: ImportExport_M3U
Source: ..\x64\Release\importexport\pls_Plugin.dll; DestDir: {app}\importexport\; Check: Is64BitInstallMode; Flags: ignoreversion; Components: ImportExport_PLS
Source: ..\x64\Release\plugins\MSNInfo_Plugin.dll; DestDir: {app}\plugins\; Check: Is64BitInstallMode; Flags: ignoreversion; Components: Plugin_MSNInfo
Source: ..\x64\Release\plugins\NowPlayingTxt_Plugin.dll; DestDir: {app}\plugins\; Check: Is64BitInstallMode; Flags: ignoreversion; Components: Plugin_NowPlayingTxt
Source: ..\x64\Release\plugins\PopupNotify_plugin.dll; DestDir: {app}\plugins\; Check: Is64BitInstallMode; Flags: ignoreversion; Components: Plugin_PopupNotify

Source: ..\Win32\Release\TuniacApp.exe; DestDir: {app}\; Check: not Is64BitInstallMode; Flags: ignoreversion
Source: ..\Win32\Release\std_InfoManager.dll; DestDir: {app}\; Check: not Is64BitInstallMode; Flags: ignoreversion
Source: ..\Win32\Release\visuals\*.dll; DestDir: {app}\visuals; Check: not Is64BitInstallMode; Flags: ignoreversion; Components: Visuals
Source: ..\Win32\Release\mp3_Plugin.dll; DestDir: {app}\; Check: not Is64BitInstallMode; Flags: ignoreversion; Components: Decoder_MP3
Source: ..\Win32\Release\ogg_Plugin.dll; DestDir: {app}\; Check: not Is64BitInstallMode; Flags: ignoreversion; Components: Decoder_OGG
Source: ..\Win32\Release\ape_Plugin.dll; DestDir: {app}\; Check: not Is64BitInstallMode; Flags: ignoreversion; Components: Decoder_APE
Source: ..\Win32\Release\m4a_Plugin.dll; DestDir: {app}\; Check: not Is64BitInstallMode; Flags: ignoreversion; Components: Decoder_M4A
Source: ..\Win32\Release\mp3streamer_Plugin.dll; DestDir: {app}\; Check: not Is64BitInstallMode; Flags: ignoreversion; Components: Decoder_MP3Streamer
Source: ..\Win32\Release\importexport\m3u_Plugin.dll; DestDir: {app}\importexport\; Check: not Is64BitInstallMode; Flags: ignoreversion; Components: ImportExport_M3U
Source: ..\Win32\Release\importexport\pls_Plugin.dll; DestDir: {app}\importexport\; Check: not Is64BitInstallMode; Flags: ignoreversion; Components: ImportExport_PLS
Source: ..\Win32\Release\plugins\MSNInfo_Plugin.dll; DestDir: {app}\plugins\; Check: not Is64BitInstallMode; Flags: ignoreversion; Components: Plugin_MSNInfo
Source: ..\Win32\Release\plugins\NowPlayingTxt_Plugin.dll; DestDir: {app}\plugins\; Check: not Is64BitInstallMode; Flags: ignoreversion; Components: Plugin_NowPlayingTxt
Source: ..\Win32\Release\plugins\PopupNotify_plugin.dll; DestDir: {app}\plugins\; Check: not Is64BitInstallMode; Flags: ignoreversion; Components: Plugin_PopupNotify


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
Name: Visuals; Description: Visuals; Check: not Is64BitInstallMode; Types: custom full
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
procedure AboutButtonOnClick(Sender: TObject);
begin
  MsgBox('This installer will install the Tuniac (Beta) media player onto your computer', mbInformation, mb_Ok);
end;

procedure URLLabelOnClick(Sender: TObject);
var
  ErrorCode: Integer;
begin
  ShellExec('open', 'http://www.tuniac.com', '', '', SW_SHOWNORMAL, ewNoWait, ErrorCode);
end;

procedure InitializeWizard();
var
  AboutButton, CancelButton: TButton;
  URLLabel: TNewStaticText;
begin

  { Other custom controls }

  CancelButton := WizardForm.CancelButton;

  AboutButton := TButton.Create(WizardForm);
  AboutButton.Left := WizardForm.ClientWidth - CancelButton.Left - CancelButton.Width;
  AboutButton.Top := CancelButton.Top;
  AboutButton.Width := CancelButton.Width;
  AboutButton.Height := CancelButton.Height;
  AboutButton.Caption := '&About...';
  AboutButton.OnClick := @AboutButtonOnClick;
  AboutButton.Parent := WizardForm;

  URLLabel := TNewStaticText.Create(WizardForm);
  URLLabel.Caption := 'www.tuniac.com';
  URLLabel.Cursor := crHand;
  URLLabel.OnClick := @URLLabelOnClick;
  URLLabel.Parent := WizardForm;
  { Alter Font *after* setting Parent so the correct defaults are inherited first }
  URLLabel.Font.Style := URLLabel.Font.Style + [fsUnderline];
  URLLabel.Font.Color := clBlue;
  URLLabel.Top := AboutButton.Top + AboutButton.Height - URLLabel.Height - 2;
  URLLabel.Left := AboutButton.Left + AboutButton.Width + ScaleX(20);
end;

procedure CurUninstallStepChanged(CurUninstallStep: TUninstallStep);
var
  ResultCode: Integer;
begin
	case CurUninstallStep of
		usUninstall:
		begin
			if MsgBox('Remove all settings?', mbInformation, mb_YesNo) = idYes then
			begin
                if not Exec(ExpandConstant('{app}\TuniacApp.exe'), '-dontsaveprefs -wipeprefs -wipefileassoc -exit', '', SW_HIDE, ewWaitUntilTerminated, ResultCode) then
				    MsgBox('Error removing settings & file associations.', mbError, MB_OK);
			end
			else
			begin
			    if not Exec(ExpandConstant('{app}\TuniacApp.exe'), '-dontsaveprefs -wipefileassoc -exit', '', SW_HIDE, ewWaitUntilTerminated, ResultCode) then
					MsgBox('Error removing file associations.', mbError, MB_OK);
			end;
		end;
	end;
end;
