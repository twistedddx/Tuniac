#define DateTime GetDateTimeString('yymmdd','','')

[Setup]
AllowNoIcons=yes
AppID={{A2A3A9DE-A195-4A66-8DA6-59968E0EF943}
AppMutex=TUNIACWINDOWCLASS
AppName=Tuniac {code:TheVersion}
AppPublisher=Tuniac Dev Team
AppPublisherURL=http://www.tuniac.com
AppSupportURL=http://www.tuniac.com
AppUpdatesURL=http://www.tuniac.com
AppVerName=Tuniac (Beta)
ArchitecturesInstallIn64BitMode=x64
Compression=lzma/ultra
DefaultDirName={pf}\Tuniac
DefaultGroupName=Tuniac
InternalCompressLevel=ultra
MinVersion=0,5.01.2600sp2
OutputDir=.
OutputBaseFilename=..\Tuniac_Setup_{#DateTime}
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
Source: "isxdl.dll"; Flags: dontcopy
Source: "directx.ico"; Flags: dontcopy
Source: "WizModernSmallImage-IS.bmp"; Flags: dontcopy

Source: ..\TuniacApp\images\NoAlbumArt.jpg; DestDir: {app}\; Flags: ignoreversion
Source: ..\TuniacApp\icons\*.ico; DestDir: {app}\iconsets\; Flags: ignoreversion recursesubdirs
Source: ..\Guide\*; DestDir: {app}\Guide\; Flags: ignoreversion recursesubdirs createallsubdirs

Source: ..\x64\Release\TuniacApp.exe; DestDir: {app}\; Check: not InstallLegacyCheck; Flags: ignoreversion
Source: ..\x64\Release\*.dll; DestDir: {app}\; Check: not InstallLegacyCheck; Flags: ignoreversion
Source: ..\x64\Release\importexport\*.dll; DestDir: {app}\importexport\; Check: not InstallLegacyCheck; Flags: ignoreversion
Source: ..\x64\Release\plugins\*.dll; DestDir: {app}\plugins\; Check: not InstallLegacyCheck; Flags: ignoreversion

Source: ..\Win32\Release\TuniacApp.exe; DestDir: {app}\; Check: InstallLegacyCheck; Flags: ignoreversion
Source: ..\Win32\Release\*.dll; DestDir: {app}\; Check: InstallLegacyCheck; Flags: ignoreversion
Source: ..\Win32\Release\visuals\*.dll; DestDir: {app}\visuals; Check: InstallLegacyCheck; Flags: ignoreversion
Source: ..\Win32\Release\importexport\*.dll; DestDir: {app}\importexport\; Check: InstallLegacyCheck; Flags: ignoreversion
Source: ..\Win32\Release\plugins\*.dll; DestDir: {app}\plugins\; Check: InstallLegacyCheck; Flags: ignoreversion

[Registry]
Root: HKLM; Subkey: "SOFTWARE\Microsoft\Windows\CurrentVersion\App Paths\TuniacApp.exe"; ValueType: string; ValueName: ""; ValueData: "{app}\TuniacApp.exe"

[Icons]
Name: {group}\Tuniac; Filename: {app}\TuniacApp.exe
Name: {group}\{cm:UninstallProgram,Tuniac}; Filename: {uninstallexe}
Name: {userdesktop}\Tuniac; Filename: {app}\TuniacApp.exe; Tasks: desktopicon
Name: {userappdata}\Microsoft\Internet Explorer\Quick Launch\Tuniac; Filename: {app}\TuniacApp.exe; Tasks: quicklaunchicon

[Run]
Filename: "{tmp}\directx_mar2008_redist.exe"; StatusMsg: "Installing DirectX..."; Parameters: "/Q /T:{tmp}"; Flags: skipifdoesntexist; Check: DXMarch2008Check
Filename: "{tmp}\DXSETUP.exe"; StatusMsg: "Installing DirectX...(Please wait!!)"; Parameters: "/silent"; Flags: skipifdoesntexist; Check: DXMarch2008Check
Filename: {app}\TuniacApp.exe; Description: {cm:LaunchProgram,Tuniac}; Flags: nowait postinstall skipifsilent

[Code]
var
  InstallLegacyPage, DXMarch2008Page: TWizardPage;
  InstallLegacyCheckBox, DXMarch2008CheckBox: TCheckBox;

  FilesDownloaded: Boolean;

procedure isxdl_AddFile(URL, Filename: PChar);
external 'isxdl_AddFile@files:isxdl.dll stdcall';
function isxdl_DownloadFiles(hWnd: Integer): Integer;
external 'isxdl_DownloadFiles@files:isxdl.dll stdcall';
function isxdl_SetOption(Option, Value: PChar): Integer;
external 'isxdl_SetOption@files:isxdl.dll stdcall';

function GetModuleHandle(lpModuleName: LongInt): LongInt;
external 'GetModuleHandleA@kernel32.dll stdcall';
function ExtractIcon(hInst: LongInt; lpszExeFileName: PChar; nIconIndex: LongInt): LongInt;
external 'ExtractIconA@shell32.dll stdcall';
function DrawIconEx(hdc: LongInt; xLeft, yTop: Integer; hIcon: LongInt; cxWidth, cyWidth: Integer; istepIfAniCur: LongInt; hbrFlickerFreeDraw, diFlags: LongInt): LongInt;
external 'DrawIconEx@user32.dll stdcall';
function DestroyIcon(hIcon: LongInt): LongInt;
external 'DestroyIcon@user32.dll stdcall';

const
  DI_NORMAL = 3;


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

function InitializeSetup(): Boolean;
begin
  FilesDownloaded := False;

  Result := True;
end;

function CreateCustomOptionPage(AAfterId: Integer; ACaption, ASubCaption, AIconFileName, ALabel1Caption, ALabel2Caption,
  ACheckCaption: String; var CheckBox: TCheckBox): TWizardPage;
var
  Page: TWizardPage;
  Rect: TRect;
  hIcon: LongInt;
  Label1, Label2: TNewStaticText;
begin
  Page := CreateCustomPage(AAfterID, ACaption, ASubCaption);

  try
    AIconFileName := ExpandConstant('{tmp}\' + AIconFileName);
    if not FileExists(AIconFileName) then
      ExtractTemporaryFile(ExtractFileName(AIconFileName));

    Rect.Left := 0;
    Rect.Top := 0;
    Rect.Right := 32;
    Rect.Bottom := 32;

    hIcon := ExtractIcon(GetModuleHandle(0), AIconFileName, 0);
    try
      with TBitmapImage.Create(Page) do begin
        with Bitmap do begin
          Width := 32;
          Height := 32;
          Canvas.Brush.Color := WizardForm.Color;
          Canvas.FillRect(Rect);
          DrawIconEx(Canvas.Handle, 0, 0, hIcon, 32, 32, 0, 0, DI_NORMAL);
        end;
        Parent := Page.Surface;
      end;
    finally
      DestroyIcon(hIcon);
    end;
  except
  end;

  Label1 := TNewStaticText.Create(Page);
  with Label1 do begin
    AutoSize := False;
    Left := WizardForm.SelectDirLabel.Left;
    Width := Page.SurfaceWidth - Left;
    WordWrap := True;
    Caption := ALabel1Caption;
    Parent := Page.Surface;
  end;
  WizardForm.AdjustLabelHeight(Label1);

  Label2 := TNewStaticText.Create(Page);
  with Label2 do begin
    Top := Label1.Top + Label1.Height + ScaleY(12);
    Caption := ALabel2Caption;
    Parent := Page.Surface;
  end;
  WizardForm.AdjustLabelHeight(Label2);

  CheckBox := TCheckBox.Create(Page);
  with CheckBox do begin
    Top := Label2.Top + Label2.Height + ScaleY(12);
    Width := Page.SurfaceWidth;
    Caption := ACheckCaption;
    Parent := Page.Surface;
  end;

  Result := Page;
end;

procedure CreateCustomPages;
var
  Caption, SubCaption1, IconFileName, Label1Caption, Label2Caption, CheckCaption: String;
begin
  Caption := '64bit operating system detected';
  SubCaption1 := 'Which version of Tuniac would you like to install?';
  IconFileName := 'directx.ico';
  Label1Caption :=
    'Tuniac comes in 2 flavours, 32bit and 64bit.' + #13#10 +
    'We have detected your system is 64bit capable.' + #13#10#13#10 +
    'Note: Under 64bit the TAK, OptimFROG and SVP plugins are not available.'
  Label2Caption := 'Select below which Tuniac you want, then click Next.';
  CheckCaption := '&Install Tuniac 32bit instead of Tuniac 64bit';

  InstallLegacyPage := CreateCustomOptionPage(wpWelcome, Caption, SubCaption1, IconFileName, Label1Caption, Label2Caption, CheckCaption, InstallLegacyCheckBox);

  Caption := 'DirectX March 2008 Redist download';
  SubCaption1 := 'DirectX 9.0c March 2008 is currently not installed.';
  IconFileName := 'directx.ico';
  Label1Caption :=
    'Tuniac uses XAudio for audio output. XAudio is included in DirectX 9.0c March 2008 Redist. This is required for Tuniac to play files.'
  Label2Caption := 'Select whether you would like to download DirectX 9.0c Redist, then click Next.';
  CheckCaption := '&Download and install DirectX 9.0c Redist';

  DXMarch2008Page := CreateCustomOptionPage(wpSelectProgramGroup, Caption, SubCaption1, IconFileName, Label1Caption, Label2Caption, CheckCaption, DXMarch2008CheckBox);
end;

procedure RegisterPreviousData(PreviousDataKey: Integer);
begin
  SetPreviousData(PreviousDataKey, 'InstallLegacy', IntToStr(Ord(InstallLegacyCheckBox.Checked)));
end;

function DownloadFiles(DXMarch2008: Boolean): Boolean;
var
  hWnd: Integer;
  URL, FileName: String;
begin
  isxdl_SetOption('label', 'Downloading extra files');
  isxdl_SetOption('description', 'Please wait while Setup is downloading extra files to your computer.');

  try
    FileName := ExpandConstant('{tmp}\WizModernSmallImage-IS.bmp');
    if not FileExists(FileName) then
      ExtractTemporaryFile(ExtractFileName(FileName));
    isxdl_SetOption('smallwizardimage', FileName);
  except
  end;

  //turn off isxdl resume so it won't leave partially downloaded files behind
  //resuming wouldn't help anyway since we're going to download to {tmp}
  isxdl_SetOption('resume', 'false');

  hWnd := StrToInt(ExpandConstant('{wizardhwnd}'));

  if DXMarch2008 then begin
    URL := 'http://download.microsoft.com/download/6/4/c/64c3d3d6-c5da-47eb-9db4-86e45b9f579e/directx_mar2008_redist.exe';
    FileName := ExpandConstant('{tmp}\directx_mar2008_redist.exe');
    isxdl_AddFile(URL, FileName);
  end;

  if isxdl_DownloadFiles(hWnd) <> 0 then
    FilesDownloaded := True
  else
    SuppressibleMsgBox('Setup could not download the extra files. Try again later or download and install the extra files manually.' + #13#13 + 'Setup will now continue installing normally.', mbError, mb_Ok, idOk);

  Result := True;
end;

function NextButtonClick(CurPageID: Integer): Boolean;
begin
  if (CurPageID = wpReady) and (DXMarch2008CheckBox.Checked) then
    Result := DownloadFiles(DXMarch2008CheckBox.Checked)
  else
    Result := True;
end;

function InstallLegacyCheck: Boolean;
begin
  Result := InstallLegacyCheckBox.Checked;
end;

function DXMarch2008Check: Boolean;
begin
  Result := DXMarch2008CheckBox.Checked and FilesDownloaded;
end;

function IsNot64Mode: String;
begin
  if Is64BitInstallMode then
    Result := '0'
  else
    Result := '1';
end;

function TheVersion(Default:String): String;
begin
  if Is64BitInstallMode then
    Result := '64Bit'
  else
    Result := '32Bit';
end;

function GetXAudio2: String;
var
  XAudio: String;
begin
  if RegQueryStringValue( HKCR, 'CLSID\{fac23f48-31f5-45a8-b49b-5225d61401aa}', '', XAudio ) then
    Result := '0'
  else
    Result := '1';
end;

function ShouldSkipPage(PageID: Integer): Boolean;
begin
  if (PageID = DXMarch2008Page.ID) and (GetXAudio2 = '0') then begin
      Result := true;
  end;
  if (PageID = InstallLegacyPage.ID) and (IsNot64Mode = '1') then begin
      Result := true;
   end;
end;

procedure InitializeWizard();
var
  AboutButton, CancelButton: TButton;
  URLLabel: TNewStaticText;
begin
  CreateCustomPages;
  
  InstallLegacyCheckBox.Checked := GetPreviousData('InstallLegacy', IsNot64Mode) = '1';
  DXMarch2008CheckBox.Checked := GetPreviousData('DXMarch2008', GetXAudio2) = '1';

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
