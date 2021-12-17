REM @echo off

REM Copy assets folder from KingoEditor project
pushd %~dp0\..\
xcopy .\KingoEditor\Assets .\bin\Release-windows-x86_64\KingoEditor\Assets /E /C /H /I /Y
xcopy .\KingoEditor\imgui.ini .\bin\Release-windows-x86_64\KingoEditor /Y
popd


REM Generate zip folder
pushd %~dp0\..\bin\Release-windows-x86_64\KingoEditor

if exist KingoEditor.zip (
    del /f /q "KingoEditor.zip"       
)
powershell Compress-Archive . KingoEditor.zip

popd
