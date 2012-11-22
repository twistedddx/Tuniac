set ProgFilesRoot=%ProgramFiles%
set ProgFiles86Root=%ProgramFiles(x86)%
if not "%ProgFiles86Root%"=="" goto 64bitWindows
set ProgFiles86Root=%ProgramFiles%
:64bitWindows

set 64bitcl=true
if exist "%ProgFiles86Root%\Microsoft Visual Studio 11.0\VC\bin\amd64\cl.exe" goto 64bitcl
set 64bitcl=false
:64bitcl


call "%VS110COMNTOOLS%\VsDevCmd.bat"
call "%Programfiles(x86)%\Microsoft Visual Studio 11.0\VC\vcvarsall.bat" x86

cd ..\..\..\


rem ######## turbojpeglib
cd libjpeg-turbo\trunk\

rem #Release x86:
del ".\CMakeCache.txt"
"%ProgFiles86Root%\CMake 2.8\bin\cmake" -DCMAKE_BUILD_TYPE=Release -DWITH_JPEG8=1 -G "NMake Makefiles" .
nmake clean
nmake turbojpeg-static
mkdir .\Release\x86
move /Y turbojpeg-static.lib .\Release\x86\turbojpeg-static.lib


rem if "64bitcl"=="false" goto done
call "%Programfiles(x86)%\Microsoft Visual Studio 11.0\VC\vcvarsall.bat" x64


rem #Release x64:
del ".\CMakeCache.txt"
"%ProgFiles86Root%\CMake 2.8\bin\cmake" -DCMAKE_BUILD_TYPE=Release -DWITH_JPEG8=1 -G "NMake Makefiles" .
nmake clean
nmake turbojpeg-static
mkdir .\Release\x64
move /Y turbojpeg-static.lib .\Release\x64\turbojpeg-static.lib

:done

pause