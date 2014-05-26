set ProgFilesRoot=%ProgramFiles%
set ProgFiles86Root=%ProgramFiles(x86)%
if not "%ProgFiles86Root%"=="" goto 64bitWindows
set ProgFiles86Root=%ProgramFiles%
:64bitWindows

set 64bitcl=true
if exist "%ProgFiles86Root%\Microsoft Visual Studio 12.0\VC\bin\amd64\cl.exe" goto 64bitcl
set 64bitcl=false
:64bitcl


call "%VS120COMNTOOLS%VsDevCmd.bat"
call "%Programfiles(x86)%\Microsoft Visual Studio 12.0\VC\vcvarsall.bat" x86

cd ..\..\..\

rem ######## turbojpeglib
cd libjpeg-turbo\

rem #Release x86:
del ".\CMakeCache.txt"
"%ProgFiles86Root%\CMake\bin\cmake" -DCMAKE_BUILD_TYPE=Release -DWITH_JPEG8=1 -G "Visual Studio 12" .
devenv libjpeg-turbo.sln /project "turbojpeg-static" /Clean
devenv libjpeg-turbo.sln /project "turbojpeg-static" /Rebuild "Release"
mkdir .\Release\x86
move /Y .\Release\turbojpeg-static.lib .\Release\x86\turbojpeg-static.lib


rem if "64bitcl"=="false" goto done
call "%Programfiles(x86)%\Microsoft Visual Studio 12.0\VC\vcvarsall.bat" x64


rem #Release x64:
del ".\CMakeCache.txt"
"%ProgFiles86Root%\CMake\bin\cmake" -DCMAKE_BUILD_TYPE=Release -DWITH_JPEG8=1 -G "Visual Studio 12 Win64" .
devenv libjpeg-turbo.sln /Project "turbojpeg-static" /Clean
devenv libjpeg-turbo.sln /Project "turbojpeg-static" /Rebuild "Release"
mkdir .\Release\x64
move /Y .\Release\turbojpeg-static.lib .\Release\x64\turbojpeg-static.lib

:done

pause