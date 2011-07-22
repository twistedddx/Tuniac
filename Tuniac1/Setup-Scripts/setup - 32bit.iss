#define DateTime GetDateTimeString('yymmdd','','')

[Setup]
AllowNoIcons=yes
AppID={{A2A3A9DE-A195-4A66-8DA6-59968E0EF943}
AppMutex=TUNIACWINDOWCLASS
AppName=Tuniac
AppPublisher=Tuniac Dev Team
AppPublisherURL=http://www.tuniac.org
AppSupportURL=http://www.tuniac.org
AppUpdatesURL=http://www.tuniac.org
AppVerName=Tuniac 1.0
Compression=lzma2/ultra
DefaultDirName={pf}\Tuniac
DefaultGroupName=Tuniac
InternalCompressLevel=ultra
MinVersion=0,5.01.2600sp3
OutputDir=.
OutputBaseFilename=..\Tuniac_Setup_{#DateTime}(32bit)
SetupIconFile=..\TuniacApp\icons\tuniac.ico
ShowTasksTreeLines=yes
SolidCompression=yes
WizardImageFile=WizardImage.bmp
WizardSmallImageFile=WizardImageSmall.bmp

[Tasks]
Name: desktopicon; Description: {cm:CreateDesktopIcon}; GroupDescription: {cm:AdditionalIcons}; Flags: unchecked
Name: quicklaunchicon; Description: {cm:CreateQuickLaunchIcon}; GroupDescription: {cm:AdditionalIcons}; Flags: unchecked

[InstallDelete]
Name: {app}\Guide\*.html; Type: files
Name: {app}\Guide\*.css; Type: files
Name: {app}\Guide\Images\*.jpg; Type: files

[Files]

Source: C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\redist\x86\Microsoft.VC100.CRT\*.dll; DestDir: {app}\; Flags: ignoreversion

Source: .\DirectX\*.dll; DestDir: {tmp}\; Check: DXJun2010Check; Flags: ignoreversion
Source: .\DirectX\*.exe; DestDir: {tmp}\; Check: DXJun2010Check; Flags: ignoreversion
Source: .\DirectX\dxdllreg_x86.cab; DestDir: {tmp}\; Check: DXJun2010Check; Flags: ignoreversion
Source: .\DirectX\dxupdate.cab; DestDir: {tmp}\; Check: DXJun2010Check; Flags: ignoreversion
Source: .\DirectX\Jun2010_XAudio_x86.cab; DestDir: {tmp}\; Check: DXJun2010Check; Flags: ignoreversion

Source: "WizModernSmallImage-IS.bmp"; Flags: dontcopy

Source: ..\TuniacApp\images\NoAlbumArt.jpg; DestDir: {app}\; Flags: ignoreversion
Source: ..\Housekeeping\Change Log.txt; DestDir: {app}\; Flags: ignoreversion
Source: ..\Housekeeping\gpl.txt; DestDir: {app}\; Flags: ignoreversion
Source: ..\Housekeeping\lgpl.txt; DestDir: {app}\; Flags: ignoreversion
Source: ..\TuniacApp\icons\*.ico; DestDir: {app}\iconsets\; Flags: ignoreversion recursesubdirs
Source: ..\Guide\*; DestDir: {app}\Guide\; Flags: ignoreversion recursesubdirs

Source: ..\Win32\Release\TuniacApp.exe; DestDir: {app}\; Flags: ignoreversion
Source: ..\Win32\Release\*.dll; DestDir: {app}\; Flags: ignoreversion recursesubdirs

[Registry]
Root: HKLM; Subkey: "SOFTWARE\Microsoft\Windows\CurrentVersion\App Paths\TuniacApp.exe"; ValueType: string; ValueName: ""; ValueData: "{app}\TuniacApp.exe"
Root: HKCU; Subkey: "SOFTWARE\MediaScience\Sonique\General Preferences 0.80"; ValueType: string; ValueName: "SoniquePath"; ValueData: "{app}\visuals\"
Root: HKCU; Subkey: "SOFTWARE\Microsoft\Keyboard\Native Media Players\TuniacApp"; ValueType: string; ValueName: "AppName"; ValueData: "TuniacApp"
Root: HKCU; Subkey: "SOFTWARE\Microsoft\Keyboard\Native Media Players\TuniacApp"; ValueType: string; ValueName: "ExePath"; ValueData: "{app}\TuniacApp.exe"

[Icons]
Name: {group}\Tuniac; Filename: {app}\TuniacApp.exe
Name: {group}\{cm:UninstallProgram,Tuniac}; Filename: {uninstallexe}
Name: {userdesktop}\Tuniac; Filename: {app}\TuniacApp.exe; Tasks: desktopicon
Name: {userappdata}\Microsoft\Internet Explorer\Quick Launch\Tuniac; Filename: {app}\TuniacApp.exe; Tasks: quicklaunchicon

[Run]
Filename: "{tmp}\DXSETUP.exe"; StatusMsg: "Installing DirectX XAudio 2.7...(Please wait!!)"; Parameters: "/silent"; Flags: skipifdoesntexist; Check: DXJun2010Check
Filename: {app}\TuniacApp.exe; Description: {cm:LaunchProgram,Tuniac}; Flags: nowait postinstall skipifsilent

[Code]
var
  InstallLegacyPage: TWizardPage;
  InstallLegacyCheckBox: TCheckBox;

  FilesDownloaded: Boolean;

function GetModuleHandle(lpModuleName: LongInt): LongInt;
external 'GetModuleHandleA@kernel32.dll stdcall';
function ExtractIcon(hInst: LongInt; lpszExeFileName: PChar; nIconIndex: LongInt): LongInt;
external 'ExtractIconA@shell32.dll stdcall';
function DrawIconEx(hdc: LongInt; xLeft, yTop: Integer; hIcon: LongInt; cxWidth, cyWidth: Integer; istepIfAniCur: LongInt; hbrFlickerFreeDraw, diFlags: LongInt): LongInt;
external 'DrawIconEx@user32.dll stdcall';
function DestroyIcon(hIcon: LongInt): LongInt;
external 'DestroyIcon@user32.dll stdcall';

procedure AboutButtonOnClick(Sender: TObject);
begin
  MsgBox('This installer will install the Tuniac 1.0 media player onto your computer', mbInformation, mb_Ok);
end;

procedure URLLabelOnClick(Sender: TObject);
var
  ErrorCode: Integer;
begin
  ShellExec('open', 'http://www.tuniac.org', '', '', SW_SHOWNORMAL, ewNoWait, ErrorCode);
end;

function DXJun2010Check: Boolean;
var
  XAudio2: String;
begin
  if RegQueryStringValue( HKCR, 'CLSID\{5a508685-a254-4fba-9b82-9a24b00306af}', '', XAudio2 ) then
    Result := false
  else
    Result := true;
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
  URLLabel.Caption := 'www.tuniac.org';
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
