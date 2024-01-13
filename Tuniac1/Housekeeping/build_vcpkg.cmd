ECHO OFF

cd ..\..\..\
echo Working dir: %cd%

if exist vcpkg GOTO SKIPINSTALL

rem ## Install
call git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
call .\bootstrap-vcpkg.bat
.\vcpkg.exe integrate install

:SKIPINSTALL
if exist .\installed\x86-windows\include\utf8cpp GOTO UPGRADE

.\vcpkg.exe install utfcpp:x86-windows zlib:x86-windows
.\vcpkg.exe install utfcpp:x64-windows zlib:x64-windows

GOTO END

rem ## Upgrade
:UPGRADE
cd vcpkg
.\vcpkg.exe upgrade

:END
pause