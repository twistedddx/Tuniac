#include <idp.iss>

#include "setup-common.iss"
#include "setup-common_32bit.iss"

[Setup]
MinVersion=0,6.0.6000
OutputBaseFilename=Tuniac_Setup_{#DateTime}_noxp

[Files]
Source: "..\Guide\*"; DestDir: {app}\Guide\; Flags: ignoreversion recursesubdirs

Source: "..\Win32\Release_noxp\*.exe"; DestDir: {app}\; Flags: ignoreversion
Source: "..\Win32\Release_noxp\*.dll"; DestDir: {app}\; Flags: ignoreversion recursesubdirs; Excludes: "MMShellHookHelper.dll"
Source: "..\x64\Release\plugins\MMShellHookHelper.exe"; DestDir: {app}\plugins\; Flags: ignoreversion; Check: IsWin64
Source: "..\x64\Release\plugins\MMShellHook_Plugin.dll"; DestDir: {app}\plugins\; DestName: "MMShellHookHelper.dll"; Flags: ignoreversion; Check: IsWin64
Source: "..\Win32\Release_noxp\visuals\verdana14.glf"; DestDir: {app}\visuals\; Flags: ignoreversion
Source: "..\Win32\Release_noxp\visuals\vis\*.*"; DestDir: {app}\visuals\vis\; Flags: ignoreversion recursesubdirs

[Code]
function ShouldSkipPage(PageID: Integer): Boolean;
begin
  if (PageID = DownloadWantedPage.ID) and not IsDownloadRequired then begin
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