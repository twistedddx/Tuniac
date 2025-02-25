[Setup]
ArchitecturesInstallIn64BitMode=x64

[Code]
var
  Install32bitPage: TWizardPage;
  Install32bitCheckBox: TCheckBox;

//32bit install on 64bit machine
function Install32bitCheck: Boolean;
begin
  Result := Install32bitCheckBox.Checked;
end;

function IsNot64BitMode: String;
begin
  if Is64BitInstallMode then begin
    Result := '0';
  end else begin
    Result := '1';
  end;
end;

// 32bit install option for 64bit machines
procedure Create32bitPage;
var
  Caption, SubCaption1, IconFileName, Label1Caption, Label2Caption, CheckCaption: String;
begin
  Caption := '64bit operating system detected';
  SubCaption1 := 'Which version of Tuniac would you like to install?';
  IconFileName := 'directx.ico';
  Label1Caption :=
    'Tuniac comes in 32bit and 64bit.' + #13#10 +
    'We have detected your system is 64bit capable.' + #13#10#13#10 +
    'Note: Under 64bit the SHN and SVP plugins are not available.'
  Label2Caption := 'Select below which Tuniac you want, then click Next.';
  CheckCaption := '&Install Tuniac 32bit instead of Tuniac 64bit';

  Install32bitPage := CreateCustomOptionPage(wpWelcome, Caption, SubCaption1, IconFileName, Label1Caption, Label2Caption, CheckCaption, Install32bitCheckBox);
end;

//reset previous user setting for 32bit install on 64bit machine
procedure RegisterPreviousData(PreviousDataKey: Integer);
begin
  SetPreviousData(PreviousDataKey, 'Install32bit', IntToStr(Ord(Install32bitCheckBox.Checked)));
  SetPreviousData(PreviousDataKey, 'DownloadWanted', IntToStr(Ord(DownloadWantedCheckBox.Checked)));
end;