#define MyAppName "Mouse Gestures"
#define MyAppVersion "1.7.2"
#define MyAppPublisher "Mouse Gestures"
#define MyAppURL "https://github.com/your-username/mouse-gestures"
#define MyAppExeName "MouseGestures.exe"
#define MyServiceName "MouseGesturesService"

[Setup]
; NOTE: The value of AppId uniquely identifies this application.
AppId={{A1B2C3D4-E5F6-4747-8899-AABBCCDDEEFF}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={commonpf}\{#MyAppName}
DefaultGroupName={#MyAppName}
OutputDir=Release
OutputBaseFilename=MouseGestures_Setup_{#MyAppVersion}
SetupIconFile=app.ico
Compression=lzma
SolidCompression=yes
PrivilegesRequired=admin
UninstallDisplayIcon={app}\{#MyAppExeName}
WizardStyle=modern

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked
Name: "quicklaunchicon"; Description: "{cm:CreateQuickLaunchIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked; OnlyBelowVersion: 0,6.1
Name: "installservice"; Description: "Install as Windows Service (runs at startup)"; GroupDescription: "Service Installation"; Flags: unchecked

[Files]
Source: "Release\{#MyAppExeName}"; DestDir: "{app}"; Flags: ignoreversion
Source: "README.md"; DestDir: "{app}"; Flags: ignoreversion
Source: "LICENSE"; DestDir: "{app}"; Flags: ignoreversion
; Add any additional files here

[Icons]
Name: "{group}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"
Name: "{group}\{cm:UninstallProgram,{#MyAppName}}"; Filename: "{uninstallexe}"
Name: "{commondesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: desktopicon
Name: "{userappdata}\Microsoft\Internet Explorer\Quick Launch\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: quicklaunchicon

[Run]
; Run application or install service based on user choice
Filename: "{app}\{#MyAppExeName}"; Description: "{cm:LaunchProgram,{#StringChange(MyAppName, '&', '&&')}}"; Flags: nowait postinstall skipifsilent; Check: not WasServiceInstallChosen

[UninstallRun]
; Stop and remove service if installed

[Code]
var
  ServicePage: TInputOptionWizardPage;

function InitializeSetup(): Boolean;
begin
  Result := True;
end;

procedure InitializeWizard;
begin
  // Create custom page for service installation option
  ServicePage := CreateInputOptionPage(wpSelectTasks,
    'Installation Type', 'Choose how you want to install Mouse Gestures',
    'Please select whether you want to install Mouse Gestures as a Windows Service or as a normal application.',
    True, False);
  ServicePage.Add('Install as Windows Service (runs at startup)');
  ServicePage.Add('Install as normal application (runs when launched)');
  ServicePage.Values[1] := True; // Default to normal application
end;

function WasServiceInstallChosen(): Boolean;
begin
  Result := ServicePage.Values[0];
end;

// Check for running instances before uninstall
function InitializeUninstall(): Boolean;
var
  ResultCode: Integer;
begin
  Result := True;
  
  // Try to stop the service if it's running
  Exec('net', 'stop {#MyServiceName}', '', SW_HIDE, ewWaitUntilTerminated, ResultCode);
  
  // Kill any running instances of the application
  Exec('taskkill', '/f /im {#MyAppExeName}', '', SW_HIDE, ewWaitUntilTerminated, ResultCode);
  
  // Give it a moment to shut down
  Sleep(1000);
end;

[Registry]
; Add registry entries for auto-start (normal application mode)
Root: HKCU; Subkey: "Software\Microsoft\Windows\CurrentVersion\Run"; ValueType: string; ValueName: "{#MyAppName}"; ValueData: """{app}\{#MyAppExeName}"""; Flags: uninsdeletevalue; Tasks: not installservice 