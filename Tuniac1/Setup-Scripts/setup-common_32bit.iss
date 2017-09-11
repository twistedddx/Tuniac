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

  if (dwMajor >= {#VC14x86RedistdwMajor}) then begin
    if (dwMinor >= {#VC14x86RedistdwMinor}) then begin
      if(dwBld >=  {#VC14x86RedistdwBld}) then begin
        Result := True;
      end;
    end;
  end;
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

  if (dwMajor >= {#VC14x64RedistdwMajor}) then begin
    if (dwMinor >= {#VC14x64RedistdwMinor}) then begin
      if(dwBld >=  {#VC14x64RedistdwBld}) then begin
        Result := True;
      end;
    end;
  end;
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
  if not HasVC14x86Redist or not HasDXJun2010 then begin
    Result:= True;
  end else if not HasVC14x64Redist and IsWin64 then begin
    Result:= True;
  end else begin
    Result := False;
  end;
end;
      
function DownloadsNeeded: String;
begin
  DownloadString := '';

  if not HasDXJun2010  then begin
    DownloadString := DownloadString + '{#DXJun2010XAudioText }';
  end;

  if not HasVC14x86Redist or (not HasVC14x64Redist and IsWin64) then begin
    if Length(DownloadString) > 0 then begin
      DownloadString := DownloadString + ', ';
    end;

    DownloadString := DownloadString + '{#VC14RedistText} Runtime(';

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
    DownloadSize := DownloadSize + {#DXJun2010Size};
  end;

  if not HasVC14x86Redist then begin
    DownloadSize := DownloadSize + {#VC14x86RedistSize};
  end;

  if not HasVC14x64Redist and IsWin64 then begin
    DownloadSize := DownloadSize + {#VC14x64RedistSize};
  end;

  Result := DownloadSize;
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
  URL, URLMIRROR, FileName: String;
begin
  idpClearFiles;

  idpSetOption('DetailedMode',  '1');

  if not HasDXJun2010 then begin
    URL := '{#DXJun2010DSETUP}';
    FileName := ExpandConstant('{tmp}\DSETUP.dll');
    idpAddFile(URL, FileName);
    URL := '{#DXJun2010dsetup32}';
    FileName := ExpandConstant('{tmp}\dsetup32.dll');
    idpAddFile(URL, FileName);
    URL := '{#DXJun2010dxdllreg_x86}';
    FileName := ExpandConstant('{tmp}\dxdllreg_x86.cab');
    idpAddFile(URL, FileName);
    URL := '{#DXJun2010DXSETUP}';
    FileName := ExpandConstant('{tmp}\DXSETUP.exe');
    idpAddFile(URL, FileName);
    URL := '{#DXJun2010dxupdate}';
    FileName := ExpandConstant('{tmp}\dxupdate.cab');
    idpAddFile(URL, FileName);
    URL := '{#DXJun2010Jun2010_XAudio_x64}';
    FileName := ExpandConstant('{tmp}\Jun2010_XAudio_x64.cab');
    idpAddFile(URL, FileName);
    URL := '{#DXJun2010Jun2010_XAudio_x86}';
    FileName := ExpandConstant('{tmp}\Jun2010_XAudio_x86.cab');
    idpAddFile(URL, FileName);
  end;                                    

  if not HasVC14x86Redist then begin
    URL := '{#VC14x86Redist}';
    URLMIRROR := '{#VC14x86RedistMIRROR}';
    FileName := ExpandConstant('{tmp}\vcredist_x86(2015).exe');
    idpAddFile(URL, FileName);
    idpAddMirror(URL, URLMirror);
  end;      

  if not HasVC14x64Redist and IsWin64 then begin
    URL := '{#VC14x64Redist}';
    URLMIRROR := '{#VC14x64RedistMIRROR}';
    FileName := ExpandConstant('{tmp}\vcredist_x64(2015).exe');
    idpAddFile(URL, FileName);
    idpAddMirror(URL, URLMIRROR);
  end;

  idpDownloadAfter(wpPreparing);
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

procedure DeinitializeSetup();
var
  ErrorCode: Integer;
begin
  if not HasDXJun2010 then begin
    if SuppressibleMsgBox('{#DXJun2010Text} is required but not found and automatic download has failed. Go to manual download?', mbConfirmation, MB_YESNO, IDNO) = IDYES then begin
      ShellExec('', '{#DXJun2010Manual}', '', '', SW_SHOW, ewNoWait, ErrorCode);
    end;
  end;

  if not HasVC14x86Redist or (not HasVC14x64Redist and IsWin64) then begin
    if SuppressibleMsgBox('Visual C++ Redistributable for {#VC14RedistText} is required but not found and automatic download has failed. Go to manual download?', mbConfirmation, MB_YESNO, IDNO) = IDYES then begin
      ShellExec('', '{#VC14RedistManual}', '', '', SW_SHOW, ewNoWait, ErrorCode);
    end;
  end;
end;
