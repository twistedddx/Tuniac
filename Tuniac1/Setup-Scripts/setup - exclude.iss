#include "setup-common.iss"

[Setup]
OutputBaseFilename=Tuniac_Setup_{#DateTime}(exclude)

[Files]
Source: "..\Win32\Release\*.exe"; DestDir: {app}\; Flags: ignoreversion
Source: "..\Win32\Release\*.dll"; DestDir: {app}\; Flags: ignoreversion recursesubdirs; Excludes: "MMShellHookHelper.dll"
Source: "..\x64\Release\plugins\MMShellHookHelper.exe"; DestDir: {app}\plugins\; Flags: ignoreversion; Check: IsWin64
Source: "..\x64\Release\plugins\MMShellHook_Plugin.dll"; DestDir: {app}\plugins\; DestName: "MMShellHookHelper.dll";  Flags: ignoreversion; Check: IsWin64
Source: "..\Win32\Release\visuals\verdana14.glf"; DestDir: {app}\visuals\; Flags: ignoreversion

[Code]
function GetModuleHandle(lpModuleName: LongInt): LongInt;
external 'GetModuleHandleA@kernel32.dll stdcall';
function ExtractIcon(hInst: LongInt; lpszExeFileName: PChar; nIconIndex: LongInt): LongInt;
external 'ExtractIconA@shell32.dll stdcall';
function DrawIconEx(hdc: LongInt; xLeft, yTop: Integer; hIcon: LongInt; cxWidth, cyWidth: Integer; istepIfAniCur: LongInt; hbrFlickerFreeDraw, diFlags: LongInt): LongInt;
external 'DrawIconEx@user32.dll stdcall';
function DestroyIcon(hIcon: LongInt): LongInt;
external 'DestroyIcon@user32.dll stdcall';

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