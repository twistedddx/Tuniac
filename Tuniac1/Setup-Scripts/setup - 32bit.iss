#define DateTime GetDateTimeString('yymmdd','','')

[Setup]
AllowNoIcons=yes
AppID={{A2A3A9DE-A195-4A66-8DA6-59968E0EF943}
AppMutex=TUNIACWINDOWCLASS
AppName=Tuniac 1.0
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
Source: "isxdlfiles\isxdl.dll"; Flags: dontcopy
Source: "WizardImageSmall.bmp"; Flags: dontcopy

Source: ..\TuniacApp\images\NoAlbumArt.jpg; DestDir: {app}\; Flags: ignoreversion
Source: ..\Housekeeping\Change Log.txt; DestDir: {app}\; Flags: ignoreversion
Source: ..\Housekeeping\gpl.txt; DestDir: {app}\; Flags: ignoreversion
Source: ..\Housekeeping\lgpl.txt; DestDir: {app}\; Flags: ignoreversion
Source: ..\TuniacApp\icons\*.ico; DestDir: {app}\iconsets\; Flags: ignoreversion recursesubdirs
Source: ..\Guide\*; DestDir: {app}\Guide\; Flags: ignoreversion recursesubdirs

Source: ..\Win32\Release\TuniacApp.exe; DestDir: {app}\; Flags: ignoreversion
Source: ..\Win32\Release\*.dll; DestDir: {app}\; Flags: ignoreversion recursesubdirs
Source: ..\Win32\Release\visuals\verdana14.glf; DestDir: {app}\visuals\; Flags: ignoreversion

;external files
Source: "{tmp}\msvcp110.dll"; DestDir: {app}\; Check: VCRedistInstalling; Flags: external ignoreversion
Source: "{tmp}\msvcr110.dll"; DestDir: {app}\; Check: VCRedistInstalling; Flags: external ignoreversion

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
Filename: "{tmp}\DXSETUP.exe"; StatusMsg: "Installing DirectX XAudio 2.7...(Please wait!!)"; Parameters: "/silent"; Flags: skipifdoesntexist;
Filename: {app}\TuniacApp.exe; Description: {cm:LaunchProgram,Tuniac}; Flags: nowait postinstall skipifsilent

[Code]
var
  InstallLegacyPage: TWizardPage;
  InstallLegacyCheckBox: TCheckBox;

  FilesDownloaded: Boolean;

procedure isxdl_AddFile(URL, Filename: AnsiString);
external 'isxdl_AddFile@files:isxdl.dll stdcall';
function isxdl_DownloadFiles(hWnd: Integer): Integer;
external 'isxdl_DownloadFiles@files:isxdl.dll stdcall';
function isxdl_SetOption(Option, Value: AnsiString): Integer;
external 'isxdl_SetOption@files:isxdl.dll stdcall';

function DrawIconEx(hdc: LongInt; xLeft, yTop: Integer; hIcon: LongInt; cxWidth, cyWidth: Integer; istepIfAniCur: LongInt; hbrFlickerFreeDraw, diFlags: LongInt): LongInt;
external 'DrawIconEx@user32.dll stdcall';
function DestroyIcon(hIcon: LongInt): LongInt;
external 'DestroyIcon@user32.dll stdcall';


//about window
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

function HasVC2012x86Redist: Boolean;
var
  VCRedistx86: String;
begin
  if RegQueryStringValue( HKLM, 'SOFTWARE\Wow6432Node\Microsoft\Windows\CurrentVersion\Uninstall\{8e70e4e1-06d7-470b-9f74-a51bef21088e}', 'DisplayVersion', VCRedistx86 ) then
  begin
    Result := True;
  end
  else if RegQueryStringValue( HKLM, 'SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\{8e70e4e1-06d7-470b-9f74-a51bef21088e}', 'DisplayVersion', VCRedistx86 ) then
  begin
    Result := True;
  end
  else
  begin
    Result := False;
  end;
end;

//check if valid vc redist available
function Has11VCRedist: Boolean;
begin
  //local
  if FileExists(ExpandConstant('{app}\msvcr110.dll')) then
  begin
    Result := True;
  end
  //32bit install, x86 redist
  else if HasVC2012x86Redist then
  begin
    Result := True;
  end
  begin
    Result := False;
  end;
end;

//check for direct x 2.7
function HasDXJun2010: Boolean;
var
  XAudio2: String;
begin
  Result := RegQueryStringValue( HKCR, 'CLSID\{5a508685-a254-4fba-9b82-9a24b00306af}', '', XAudio2 );
end;

//check if we successfully downloaded redist
function VCRedistInstalling: Boolean;
begin
  if not Has11VCRedist then
  begin 
    Result:= FilesDownloaded;
  end
  else
  begin
    Result:= False;
  end
end;

procedure DownloadFiles();
var
  ErrorCode: Integer;
  hWnd: Integer;
  URL, FileName: String;
begin
  isxdl_SetOption('label', 'Downloading extra files');
  isxdl_SetOption('description', 'Please wait while Setup is downloading extra files to your computer.');

  try
    FileName := ExpandConstant('{tmp}\WizardImageSmall.bmp');
    if not FileExists(FileName) then
      ExtractTemporaryFile(ExtractFileName(FileName));
    isxdl_SetOption('smallwizardimage', FileName);
  except
  end;

  //turn off isxdl resume so it won't leave partially downloaded files behind
  //resuming wouldn't help anyway since we're going to download to {tmp}
  isxdl_SetOption('resume', 'false');

  hWnd := StrToInt(ExpandConstant('{wizardhwnd}'));
  
  if not HasDXJun2010 then
  begin
    URL := 'http://www.tuniac.org/extra/DSETUP.dll';
    FileName := ExpandConstant('{tmp}\DSETUP.dll');
    isxdl_AddFile(URL, FileName);
    URL := 'http://www.tuniac.org/extra/dsetup32.dll';
    FileName := ExpandConstant('{tmp}\dsetup32.dll');
    isxdl_AddFile(URL, FileName);
    URL := 'http://www.tuniac.org/extra/dxdllreg_x86.cab';
    FileName := ExpandConstant('{tmp}\dxdllreg_x86.cab');
    isxdl_AddFile(URL, FileName);
    URL := 'http://www.tuniac.org/extra/DXSETUP.exe';
    FileName := ExpandConstant('{tmp}\DXSETUP.exe');
    isxdl_AddFile(URL, FileName);
    URL := 'http://www.tuniac.org/extra/dxupdate.cab';
    FileName := ExpandConstant('{tmp}\dxupdate.cab');
    isxdl_AddFile(URL, FileName);
    URL := 'http://www.tuniac.org/extra/Jun2010_XAudio_x64.cab';
    FileName := ExpandConstant('{tmp}\Jun2010_XAudio_x64.cab');
    isxdl_AddFile(URL, FileName);
    URL := 'http://www.tuniac.org/extra/Jun2010_XAudio_x86.cab';
    FileName := ExpandConstant('{tmp}\Jun2010_XAudio_x86.cab');
    isxdl_AddFile(URL, FileName);
  end;

  if not Has11VCRedist then
  begin
    URL := 'http://www.tuniac.org/extra/32bit/msvcp110.dll';
    FileName := ExpandConstant('{tmp}\msvcp110.dll');
    isxdl_AddFile(URL, FileName);
    URL := 'http://www.tuniac.org/extra/32bit/msvcr110.dll';
    FileName := ExpandConstant('{tmp}\msvcr110.dll');
    isxdl_AddFile(URL, FileName);
  end;

  if isxdl_DownloadFiles(hWnd) <> 0 then
  begin
    FilesDownloaded := True;
  end
  else
  begin
    if not HasDXJun2010 then
    begin
      if MsgBox('Setup could not download files and you do not have DirectX End-User Runtimes (June 2010). Go to download?', mbConfirmation, MB_YESNO) = IDYES then
      begin
        ShellExec('', 'http://www.microsoft.com/en-au/download/details.aspx?id=8109', '', '', SW_SHOW, ewNoWait, ErrorCode);
      end;
    end;

    if not Has11VCRedist then
    begin
      if MsgBox('Setup could not download files and you do not have Visual C++ Redistributable for Visual Studio 2012 Update 1. Go to download?', mbConfirmation, MB_YESNO) = IDYES then
      begin
        ShellExec('', 'http://www.microsoft.com/en-us/download/details.aspx?id=30679', '', '', SW_SHOW, ewNoWait, ErrorCode);
      end;
    end;
  end;
end;

function PrepareToInstall(var NeedsRestart: Boolean): String;
begin
if not Has11VCRedist then
    DownloadFiles();
  Result := '';
end;

function InitializeSetup(): Boolean;
begin
  FilesDownloaded := False;
    
  Result := True;
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
