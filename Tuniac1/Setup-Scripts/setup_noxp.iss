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
DisableDirPage=No
DisableProgramGroupPage=No
DisableWelcomePage=No
InternalCompressLevel=ultra
MinVersion=0,5.01.2600sp3
OutputDir=.
OutputBaseFilename=Tuniac_Setup_{#DateTime}_noxp
SetupIconFile=..\TuniacApp\icons\tuniac.ico
SetupMutex=TuniacSetup,Global\TuniacSetup
ShowTasksTreeLines=yes
SolidCompression=yes
UninstallFilesDir={app}\uninstaller
WizardImageFile=WizardImage.bmp
WizardSmallImageFile=WizardImageSmall.bmp

#include <idp.iss>

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
Source: "..\Guide\*"; DestDir: {app}\Guide\; Flags: ignoreversion recursesubdirs

Source: "..\Win32\Release_noxp\*.exe"; DestDir: {app}\; Flags: ignoreversion
Source: "..\Win32\Release_noxp\*.dll"; DestDir: {app}\; Flags: ignoreversion recursesubdirs; Excludes: "MMShellHookHelper.dll"
Source: "..\x64\Release\plugins\MMShellHookHelper.exe"; DestDir: {app}\plugins\; Flags: ignoreversion; Check: IsWin64
Source: "..\x64\Release\plugins\MMShellHook_Plugin.dll"; DestDir: {app}\plugins\; DestName: "MMShellHookHelper.dll"; Flags: ignoreversion; Check: IsWin64
Source: "..\Win32\Release_noxp\visuals\verdana14.glf"; DestDir: {app}\visuals\; Flags: ignoreversion
Source: "..\Win32\Release_noxp\visuals\vis\*.*"; DestDir: {app}\visuals\vis\; Flags: ignoreversion recursesubdirs

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

[Run]
Filename: "{tmp}\DXSETUP.exe"; StatusMsg: "Installing DirectX XAudio 2.7...(Please wait!)"; Parameters: "/silent"; Flags: skipifdoesntexist;
Filename: "{tmp}\vcredist_x86(2015).exe"; StatusMsg: "Installing Microsoft Visual C++ 2015 Update 3 x86 Runtime... (Please wait!)"; Parameters: "/q /norestart"; Flags: skipifdoesntexist;
Filename: "{tmp}\vcredist_x64(2015).exe"; StatusMsg: "Installing Microsoft Visual C++ 2015 Update 3 x64 Runtime... (Please wait!)"; Parameters: "/q /norestart"; Flags: skipifdoesntexist;
Filename: "{tmp}\vcredist_x86(2010).exe"; StatusMsg: "Installing Microsoft Visual C++ 2010 SP1 x86 Runtime... (Please wait!)"; Parameters: "/q /norestart"; Flags: skipifdoesntexist;
Filename: "{tmp}\vcredist_x64(2010).exe"; StatusMsg: "Installing Microsoft Visual C++ 2010 SP1 x64 Runtime... (Please wait!)"; Parameters: "/q /norestart"; Flags: skipifdoesntexist;
Filename: {app}\TuniacApp.exe; Description: {cm:LaunchProgram,Tuniac}; Flags: nowait postinstall skipifsilent

[Code]
var
  DownloadWantedPage: TWizardPage;
  DownloadWantedCheckBox: TCheckBox;
  DownloadString: String;
  DownloadSize: ShortInt;

function DrawIconEx(hdc: LongInt; xLeft, yTop: Integer; hIcon: LongInt; cxWidth, cyWidth: Integer; istepIfAniCur: LongInt; hbrFlickerFreeDraw, diFlags: LongInt): LongInt;
external 'DrawIconEx@user32.dll stdcall';
function DestroyIcon(hIcon: LongInt): LongInt;
external 'DestroyIcon@user32.dll stdcall';
      
function HasVC14x86Redist: Boolean;
var
  dwMajor: Cardinal;
  dwMinor: Cardinal;
  dwBld: Cardinal;

begin
  RegQueryDWordValue( HKLM, 'SOFTWARE\Microsoft\VisualStudio\14.0\VC\Runtimes\x86', 'Major', dwMajor);
  RegQueryDWordValue( HKLM, 'SOFTWARE\Wow6432Node\Microsoft\VisualStudio\14.0\VC\Runtimes\x86', 'Major', dwMajor);
  RegQueryDWordValue( HKLM, 'SOFTWARE\Microsoft\VisualStudio\14.0\VC\Runtimes\x86', 'Minor', dwMinor);
  RegQueryDWordValue( HKLM, 'SOFTWARE\Wow6432Node\Microsoft\VisualStudio\14.0\VC\Runtimes\x86', 'Minor', dwMinor);
  RegQueryDWordValue( HKLM, 'SOFTWARE\Microsoft\VisualStudio\14.0\VC\Runtimes\x86', 'Bld', dwBld);
  RegQueryDWordValue( HKLM, 'SOFTWARE\Wow6432Node\Microsoft\VisualStudio\14.0\VC\Runtimes\x86', 'Bld', dwBld);

  Result := False;

  if (dwMajor >= 14) then begin
    if (dwMinor >= 0) then begin
      if(dwBld >=  24212) then begin
        Result := True;
      end;
    end;
  end;
  
//  if RegValueExists( HKLM, 'SOFTWARE\Microsoft\VisualStudio\14.0\VC\Runtimes\x86', 'Installed') then begin
//    Result := True;
//  end else if RegValueExists( HKLM, 'SOFTWARE\Wow6432Node\Microsoft\VisualStudio\14.0\VC\Runtimes\x86', 'Installed') then begin
//    Result := True;
//  end else begin
//    Result := False;
//  end;
end;

function HasVC10x86Redist: Boolean;
var
  dwMajor: Cardinal;
  dwMinor: Cardinal;
  dwBld: Cardinal;

begin
  RegQueryDWordValue( HKLM, 'SOFTWARE\Microsoft\VisualStudio\10.0\VC\VCRedist\x86', 'MajorVersion', dwMajor);
  RegQueryDWordValue( HKLM, 'SOFTWARE\Wow6432Node\Microsoft\VisualStudio\10.0\VC\VCRedist\x86', 'MajorVersion', dwMajor);
  RegQueryDWordValue( HKLM, 'SOFTWARE\Microsoft\VisualStudio\10.0\VC\VCRedist\x86', 'MinorVersion', dwMinor);
  RegQueryDWordValue( HKLM, 'SOFTWARE\Wow6432Node\Microsoft\VisualStudio\10.0\VC\VCRedist\x86', 'MinorVersion', dwMinor);
  RegQueryDWordValue( HKLM, 'SOFTWARE\Microsoft\VisualStudio\10.0\VC\VCRedist\x86', 'Bld', dwBld);
  RegQueryDWordValue( HKLM, 'SOFTWARE\Wow6432Node\Microsoft\VisualStudio\10.0\VC\VCRedist\x86', 'Bld', dwBld);

  Result := False;

  if (dwMajor >= 10) then begin
    if (dwMinor >= 0) then begin
      if(dwBld >=  40219) then begin
        Result := True;
      end;
    end;
  end;

//  if RegValueExists( HKLM, 'SOFTWARE\Microsoft\VisualStudio\10.0\VC\VCRedist\x86', 'Installed') then begin
//    Result := True;
//  end else if RegValueExists( HKLM, 'SOFTWARE\Wow6432Node\Microsoft\VisualStudio\10.0\VC\VCRedist\x86', 'Installed') then begin
//    Result := True;
//  end else begin
//    Result := False;
//  end;
end;

function HasVC14x64Redist: Boolean;
var
  dwMajor: Cardinal;
  dwMinor: Cardinal;
  dwBld: Cardinal;

begin
  RegQueryDWordValue( HKLM, 'SOFTWARE\Microsoft\VisualStudio\14.0\VC\Runtimes\x64', 'Major', dwMajor);
  RegQueryDWordValue( HKLM, 'SOFTWARE\Wow6432Node\Microsoft\VisualStudio\14.0\VC\Runtimes\x64', 'Major', dwMajor);
  RegQueryDWordValue( HKLM, 'SOFTWARE\Microsoft\VisualStudio\14.0\VC\Runtimes\x64', 'Minor', dwMinor);
  RegQueryDWordValue( HKLM, 'SOFTWARE\Wow6432Node\Microsoft\VisualStudio\14.0\VC\Runtimes\x64', 'Minor', dwMinor);
  RegQueryDWordValue( HKLM, 'SOFTWARE\Microsoft\VisualStudio\14.0\VC\Runtimes\x64', 'Bld', dwBld);
  RegQueryDWordValue( HKLM, 'SOFTWARE\Wow6432Node\Microsoft\VisualStudio\14.0\VC\Runtimes\x64', 'Bld', dwBld);

  Result := False;

  if (dwMajor >= 14) then begin
    if (dwMinor >= 0) then begin
      if(dwBld >=  24212) then begin
        Result := True;
      end;
    end;
  end;

//  if RegValueExists( HKLM, 'SOFTWARE\Microsoft\VisualStudio\14.0\VC\Runtimes\x64', 'Installed' ) then begin
//    Result := True;
//  end else if RegValueExists( HKLM, 'SOFTWARE\Wow6432Node\Microsoft\VisualStudio\14.0\VC\Runtimes\x64', 'Installed' ) then begin
//    Result := True;
//  end else begin
//    Result := False;
//  end;
end;

function HasVC10x64Redist: Boolean;
var
  dwMajor: Cardinal;
  dwMinor: Cardinal;
  dwBld: Cardinal;

begin
  RegQueryDWordValue( HKLM, 'SOFTWARE\Microsoft\VisualStudio\10.0\VC\VCRedist\x64', 'MajorVersion', dwMajor);
  RegQueryDWordValue( HKLM, 'SOFTWARE\Wow6432Node\Microsoft\VisualStudio\10.0\VC\VCRedist\x64', 'MajorVersion', dwMajor);
  RegQueryDWordValue( HKLM, 'SOFTWARE\Microsoft\VisualStudio\10.0\VC\VCRedist\x64', 'MinorVersion', dwMinor);
  RegQueryDWordValue( HKLM, 'SOFTWARE\Wow6432Node\Microsoft\VisualStudio\10.0\VC\VCRedist\x64', 'MinorVersion', dwMinor);
  RegQueryDWordValue( HKLM, 'SOFTWARE\Microsoft\VisualStudio\10.0\VC\VCRedist\x64', 'Bld', dwBld);
  RegQueryDWordValue( HKLM, 'SOFTWARE\Wow6432Node\Microsoft\VisualStudio\10.0\VC\VCRedist\x64', 'Bld', dwBld);

  Result := False;

  if (dwMajor >= 10) then begin
    if (dwMinor >= 0) then begin
      if(dwBld >=  40219) then begin
        Result := True;
      end;
    end;
  end;

//  if RegValueExists( HKLM, 'SOFTWARE\Microsoft\VisualStudio\10.0\VC\VCRedist\x64', 'Installed') then begin
//    Result := True;
//  end else if RegValueExists( HKLM, 'SOFTWARE\Wow6432Node\Microsoft\VisualStudio\10.0\VC\VCRedist\x64', 'Installed') then begin
//    Result := True;
//  end else begin
//    Result := False;
//  end;
end;

//check for direct x 2.7
function HasDXJun2010: Boolean;
var
  XAudio2: String;
begin
  Result := RegQueryStringValue( HKCR, 'CLSID\{5a508685-a254-4fba-9b82-9a24b00306af}', '', XAudio2 );
end;

function DownloadWantedCheck: Boolean;
begin
  Result := DownloadWantedCheckBox.Checked;
end;    

function IsDownloadRequired: Boolean;
begin
  if not HasVC14x86Redist or not HasVC10x86Redist or not HasDXJun2010 then begin
    Result:= True;
  end else if not HasVC14x64Redist and IsWin64 then begin
    Result:= True;
  end else if not HasVC10x64Redist and IsWin64 then begin
    Result:= True;
  end else begin
    Result := False;
  end;
end;
      
function DownloadsNeeded: String;
begin
  DownloadString := '';

  if not HasDXJun2010  then begin
    DownloadString := DownloadString + 'DirectX XAudio 2.7';
  end;

  if not HasVC10x86Redist or (not HasVC10x64Redist and IsWin64) then begin
    if Length(DownloadString) > 0 then begin
      DownloadString := DownloadString + ', ';
    end;

    DownloadString := DownloadString + 'Visual C++ 2010 Runtime(';

    if not HasVC10x86Redist then begin
      DownloadString := DownloadString + 'x86';
      if not HasVC10x64Redist and IsWin64 then begin
        DownloadString := DownloadString + '/';
      end;
    end;

    if not HasVC10x64Redist and IsWin64 then begin
      DownloadString := DownloadString + 'x64';
    end;

    DownloadString := DownloadString + ')';
  end;

  if not HasVC14x86Redist or (not HasVC14x64Redist and IsWin64) then begin
    if Length(DownloadString) > 0 then begin
      DownloadString := DownloadString + ', ';
    end;

    DownloadString := DownloadString + 'Visual C++ 2015 Update 3 Runtime(';

    if not HasVC14x86Redist then begin
      DownloadString := DownloadString + 'x86';
      if not HasVC14x64Redist and IsWin64 then begin
        DownloadString := DownloadString + '/';
      end;
    end;

    if not HasVC14x64Redist and IsWin64 then begin
      DownloadString := DownloadString + 'x64';
    end;

    DownloadString := DownloadString + ')';
  end;

  Result := DownloadString;
end;

function DownloadsSize: ShortInt;
begin
  DownloadSize := 0;

  if not HasDXJun2010  then begin
    DownloadSize := DownloadSize + 3;
  end;

  if not HasVC10x86Redist then begin
    DownloadSize := DownloadSize + 13;
  end;

  if not HasVC14x86Redist then begin
    DownloadSize := DownloadSize + 13;
  end;

  if not HasVC10x64Redist and IsWin64 then begin
    DownloadSize := DownloadSize + 14;
  end;

  if not HasVC14x64Redist and IsWin64 then begin
    DownloadSize := DownloadSize + 14;
  end;

  Result := DownloadSize;
end;

//about window
procedure AboutButtonOnClick(Sender: TObject);
begin
  SuppressibleMsgBox('This installer will install the Tuniac 1.0 media player onto your computer', mbInformation, mb_Ok, IDOK);
end;

procedure URLLabelOnClick(Sender: TObject);
var
  ErrorCode: Integer;
begin
  ShellExec('open', 'http://www.tuniac.org', '', '', SW_SHOWNORMAL, ewNoWait, ErrorCode);
end;

//generic page
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
    Rect.Left := 0;
    Rect.Top := 0;
    Rect.Right := 32;
    Rect.Bottom := 32;

    try
      with TBitmapImage.Create(Page) do begin
        with Bitmap do begin
          Width := 32;
          Height := 32;
          Canvas.Brush.Color := WizardForm.Color;
          Canvas.FillRect(Rect);
          DrawIconEx(Canvas.Handle, 0, 0, hIcon, 32, 32, 0, 0, 3);
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

//should Tuniac go download pre-req's?
procedure CreateDownloadWantedPage;
var
  Caption, SubCaption1, IconFileName, Label1Caption, Label2Caption, CheckCaption: String;
begin
  Caption := 'Tuniac needs to install additional Microsoft files';
  SubCaption1 := 'Do you want Tuniac to download these additional Microsoft files now?';
  IconFileName := 'directx.ico';
  Label1Caption :=
    'Tuniac requires the following:' + #13#10 +
    DownloadsNeeded + #13#10#13#10 +
    'Note: Downloads required are about ' + IntToStr(DownloadsSize) + 'mb in total';
  Label2Caption := 'Select below if you want this installer to get and install these files, then click Next.';
  CheckCaption := '&Download and install required files';

  DownloadWantedPage := CreateCustomOptionPage(wpWelcome, Caption, SubCaption1, IconFileName, Label1Caption, Label2Caption, CheckCaption, DownloadWantedCheckBox);
end;

procedure DownloadFiles();
var
  URL, FileName: String;
begin
  idpClearFiles;

  idpSetOption('DetailedMode',  '1');

  if not HasDXJun2010 then begin
    URL := 'http://www.tuniac.org/extra/DirectX/DSETUP.dll';
    FileName := ExpandConstant('{tmp}\DSETUP.dll');
    idpAddFile(URL, FileName);
    URL := 'http://www.tuniac.org/extra/DirectX/dsetup32.dll';
    FileName := ExpandConstant('{tmp}\dsetup32.dll');
    idpAddFile(URL, FileName);
    URL := 'http://www.tuniac.org/extra/DirectX/dxdllreg_x86.cab';
    FileName := ExpandConstant('{tmp}\dxdllreg_x86.cab');
    idpAddFile(URL, FileName);
    URL := 'http://www.tuniac.org/extra/DirectX/DXSETUP.exe';
    FileName := ExpandConstant('{tmp}\DXSETUP.exe');
    idpAddFile(URL, FileName);
    URL := 'http://www.tuniac.org/extra/DirectX/dxupdate.cab';
    FileName := ExpandConstant('{tmp}\dxupdate.cab');
    idpAddFile(URL, FileName);
    URL := 'http://www.tuniac.org/extra/DirectX/Jun2010_XAudio_x64.cab';
    FileName := ExpandConstant('{tmp}\Jun2010_XAudio_x64.cab');
    idpAddFile(URL, FileName);
    URL := 'http://www.tuniac.org/extra/DirectX/Jun2010_XAudio_x86.cab';
    FileName := ExpandConstant('{tmp}\Jun2010_XAudio_x86.cab');
    idpAddFile(URL, FileName);
  end;                                    

  if not HasVC14x86Redist then begin
    URL := 'http://download.microsoft.com/download/9/a/2/9a2a7e36-a8af-46c0-8a78-a5eb111eefe2/vc_redist.x86.exe';
    FileName := ExpandConstant('{tmp}\vcredist_x86(2015).exe');
    idpAddFile(URL, FileName);
  end;      

  if not HasVC10x86Redist then begin
    URL := 'http://download.microsoft.com/download/1/6/5/165255E7-1014-4D0A-B094-B6A430A6BFFC/vcredist_x86.exe';
    FileName := ExpandConstant('{tmp}\vcredist_x86(2010).exe');
    idpAddFile(URL, FileName);
  end;

  if not HasVC14x64Redist and IsWin64 then begin
    URL := 'http://download.microsoft.com/download/2/a/2/2a2ef9ab-1b4b-49f0-9131-d33f79544e70/vc_redist.x64.exe';
    FileName := ExpandConstant('{tmp}\vcredist_x64(2015).exe');
    idpAddFile(URL, FileName);
  end;
 
  if not HasVC10x64Redist and IsWin64 then begin
    URL := 'http://download.microsoft.com/download/1/6/5/165255E7-1014-4D0A-B094-B6A430A6BFFC/vcredist_x64.exe';
    FileName := ExpandConstant('{tmp}\vcredist_x64(2010).exe');
    idpAddFile(URL, FileName);
  end;

  idpDownloadAfter(wpPreparing);
end;

function ShouldSkipPage(PageID: Integer): Boolean;
begin
  if (PageID = DownloadWantedPage.ID) and not IsDownloadRequired then begin
    Result := true;
  end else begin
    Result := false;
  end;
end;

function InitializeSetup(): Boolean;
begin
  Result := True;
end;

function PrepareToInstall(var NeedsRestart: Boolean): String;
begin
  if IsDownloadRequired and DownloadWantedCheck then begin
    DownloadFiles();
  end
  Result := '';
end;

procedure InitializeWizard();
var
  AboutButton, CancelButton: TButton;
  URLLabel: TNewStaticText;
begin
  CreateDownloadWantedPage;

  DownloadWantedCheckBox.Checked := IsDownloadRequired;

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

procedure DeinitializeSetup();
var
  ErrorCode: Integer;
begin
  if not HasDXJun2010 then begin
    if SuppressibleMsgBox('DirectX End-User Runtimes (June 2010) is required but not found and automatic download has failed. Go to manual download?', mbConfirmation, MB_YESNO, IDNO) = IDYES then begin
      ShellExec('', 'http://www.microsoft.com/en-au/download/details.aspx?id=8109', '', '', SW_SHOW, ewNoWait, ErrorCode);
    end;
  end;

  if not HasVC14x86Redist or (not HasVC14x64Redist and IsWin64) then begin
    if SuppressibleMsgBox('Visual C++ Redistributable for Visual Studio 2015 Update 3 is required but not found and automatic download has failed. Go to manual download?', mbConfirmation, MB_YESNO, IDNO) = IDYES then begin
      ShellExec('', 'http://www.microsoft.com/en-us/download/details.aspx?id=52982', '', '', SW_SHOW, ewNoWait, ErrorCode);
    end;
  end;

  if not HasVC10x86Redist or (not HasVC10x64Redist and IsWin64) then begin
    if SuppressibleMsgBox('Visual C++ Redistributable for Visual Studio 2010 SP1 is required but not found and automatic download has failed. Go to manual download?', mbConfirmation, MB_YESNO, IDNO) = IDYES then begin
      ShellExec('', 'http://www.microsoft.com/en-us/download/details.aspx?id=26999', '', '', SW_SHOW, ewNoWait, ErrorCode);
    end;
  end;
end;

procedure CurUninstallStepChanged(CurUninstallStep: TUninstallStep);
var
  ResultCode: Integer;
begin
	case CurUninstallStep of
		usUninstall:
		begin
			if SuppressibleMsgBox('Remove all settings?', mbInformation, mb_YesNo, IDYES) = IDYes then begin
        if not Exec(ExpandConstant('{app}\TuniacApp.exe'), '-dontsaveprefs -wipeprefs -wipefileassoc -exit', '', SW_HIDE, ewWaitUntilTerminated, ResultCode) then
				  SuppressibleMsgBox('Error removing settings & file associations.', mbError, MB_OK, IDOK);
			end	else begin
			  if not Exec(ExpandConstant('{app}\TuniacApp.exe'), '-dontsaveprefs -wipefileassoc -exit', '', SW_HIDE, ewWaitUntilTerminated, ResultCode) then
					SuppressibleMsgBox('Error removing file associations.', mbError, MB_OK, IDOK);
			end;
		end;
	end;
end;
