ECHO OFF

cd ..\..\..\
echo Working dir: %cd%

if exist vcpkg GOTO SKIPINSTALL

rem ## Install vcpkg
call git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
call .\bootstrap-vcpkg.bat
.\vcpkg.exe integrate install
cd..


rem ## vcpkg install zlib utf8cpp
:SKIPINSTALL
cd vcpkg

if exist .\installed\x86-windows\include\utf8cpp GOTO UPGRADE

.\vcpkg.exe install utfcpp:x86-windows zlib:x86-windows
.\vcpkg.exe install utfcpp:x64-windows zlib:x64-windows
 cd..

GOTO END

rem ## vcpkg upgrade zlib utf8cpp
:UPGRADE
cd vcpkg
.\vcpkg.exe upgrade

:END
pause