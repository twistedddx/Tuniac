#include <idp.iss>

#include "setup-common.iss"
#include "setup-common_32bit.iss"
#include "setup-common_64bit.iss"

[Setup]
MinVersion=0,5.01.2600sp3
OutputBaseFilename=Tuniac_Setup_{#DateTime}(inc 64bit)

[Files]
Source: "..\Guide\*"; DestDir: {app}\Guide\; Flags: ignoreversion recursesubdirs

Source: "..\x64\Release\*.exe"; DestDir: {app}\; Check: not Install32bitCheck; Flags: ignoreversion
Source: "..\x64\Release\*.dll"; DestDir: {app}\; Check: not Install32bitCheck; Flags: ignoreversion recursesubdirs; Excludes: "MMShellHookHelper.dll"
Source: "..\Win32\Release\plugins\MMShellHookHelper.exe"; DestDir: {app}\plugins\; Check: not Install32bitCheck; Flags: ignoreversion
Source: "..\Win32\Release\plugins\MMShellHook_Plugin.dll"; DestDir: {app}\plugins\; DestName: "MMShellHookHelper.dll"; Check: not Install32bitCheck;  Flags: ignoreversion

Source: "..\Win32\Release\*.exe"; DestDir: {app}\; Check: Install32bitCheck; Flags: ignoreversion
Source: "..\Win32\Release\*.dll"; DestDir: {app}\; Check: Install32bitCheck; Flags: ignoreversion recursesubdirs; Excludes: "MMShellHookHelper.dll"
Source: "..\x64\Release\plugins\MMShellHookHelper.exe"; DestDir: {app}\plugins\; Check: Install32bitCheck and IsWin64; Flags: ignoreversion
Source: "..\x64\Release\plugins\MMShellHook_Plugin.dll"; DestDir: {app}\plugins\; DestName: "MMShellHookHelper.dll"; Check: Install32bitCheck and IsWin64;  Flags: ignoreversion
Source: "..\Win32\Release\visuals\verdana14.glf"; DestDir: {app}\visuals\; Check: Install32bitCheck; Flags: ignoreversion
Source: "..\Win32\Release\visuals\vis\*.*"; DestDir: {app}\visuals\vis\; Check: Install32bitCheck; Flags: ignoreversion recursesubdirs

[Code]
//skip 32bit install question on 32bit only machines
function ShouldSkipPage(PageID: Integer): Boolean;
begin
  if (PageID = Install32bitPage.ID) and not Is64BitInstallMode then begin
    Result := true;
  end else if (PageID = DownloadWantedPage.ID) and not IsDownloadRequired then begin
    Result := true;
  end else begin
    Result := false;
  end;
end;

procedure InitializeWizard();
var
  AboutButton, CancelButton: TButton;
  URLLabel: TNewStaticText;
begin
  Create32bitPage;

  Install32bitCheckBox.Checked := GetPreviousData('Install32bit', IsNot64BitMode) = '1';

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
  URLLabel.Caption := '{#TuniacWWW}';
  URLLabel.Cursor := crHand;
  URLLabel.OnClick := @URLLabelOnClick;
  URLLabel.Parent := WizardForm;
  { Alter Font *after* setting Parent so the correct defaults are inherited first }
  URLLabel.Font.Style := URLLabel.Font.Style + [fsUnderline];
  URLLabel.Font.Color := clBlue;
  URLLabel.Top := AboutButton.Top + AboutButton.Height - URLLabel.Height - 2;
  URLLabel.Left := AboutButton.Left + AboutButton.Width + ScaleX(20);
end;