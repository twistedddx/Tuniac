cd ..\..\..\

set ProgFilesRoot=%ProgramFiles%
set ProgFiles86Root=%ProgramFiles(x86)%
if not "%ProgFiles86Root%"=="" goto 64bitWindows
set ProgFiles86Root=%ProgramFiles%
:64bitWindows

set 64bitcl=true
if not exist "%ProgFiles86Root%\Microsoft Visual Studio 10.0\VC\bin\amd64\cl.exe" goto 32bitcl
set 64bitcl=false
:32bitcl

rem ######## turbojpeglib
cd libjpeg-turbo\trunk\

rem #Release x86:
del ".\CMakeCache.txt"
call "%ProgFilesRoot%\Microsoft SDKs\Windows\v7.1\Bin\SetEnv.cmd" /Release /x86 /win7
call "%ProgFiles86Root%\Microsoft Visual Studio 10.0\VC\bin\vcvars32.bat"
"%ProgFiles86Root%\CMake 2.8\bin\cmake" -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release -DWITH_JPEG8=1 .
nmake clean
nmake
mkdir .\Release\x86
move /Y turbojpeg-static.lib .\Release\x86\turbojpeg-static.lib

if "64bitcl"=="false" goto done

rem #Release x64:
del ".\CMakeCache.txt"
call "%ProgFilesRoot%\Microsoft SDKs\Windows\v7.1\Bin\SetEnv.cmd" /Release /x64 /win7
call "%ProgFiles86Root%\Microsoft Visual Studio 10.0\VC\bin\x86_amd64\vcvarsx86_amd64.bat"
"%ProgFiles86Root%\CMake 2.8\bin\cmake" -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release -DWITH_JPEG8=1 .
nmake clean
nmake
mkdir .\Release\x64
move /Y turbojpeg-static.lib .\Release\x64\turbojpeg-static.lib

:done

cd ..\..\