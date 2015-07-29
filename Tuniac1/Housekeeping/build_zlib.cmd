set ProgFilesRoot=%ProgramFiles%
set ProgFiles86Root=%ProgramFiles(x86)%
if not "%ProgFiles86Root%"=="" goto 64bitWindows
set ProgFiles86Root=%ProgramFiles%
:64bitWindows

set 64bitcl=true
if exist "%ProgFiles86Root%\Microsoft Visual Studio 14.0\VC\bin\amd64\cl.exe" goto 64bitcl
set 64bitcl=false
:64bitcl


call "%Programfiles(x86)%\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" x86

cd ..\..\..\

rem ######## zlib
cd zlib\
mkdir .\build
cd build\

rem #zlib Release x86
rmdir /S /Q ..\build
"%ProgramFiles(x86)%\CMake\bin\cmake" -DBUILD_SHARED_LIBS=OFF -G "Visual Studio 14" zlibstatic ..\.
devenv zlib.sln /project "zlibstatic" /Clean
devenv zlib.sln /project "zlibstatic" /Rebuild "Release"
mkdir ..\Release\x86
move /Y .\Release\zlibstatic.lib ..\Release\x86\zlibstatic.lib


rem if "64bitcl"=="false" goto done
call "%Programfiles(x86)%\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" amd64


rem #zlib Release x64
rmdir /S /Q ..\build
"%ProgFiles86Root%\CMake\bin\cmake" -DBUILD_SHARED_LIBS=OFF -G "Visual Studio 14 Win64" zlibstatic ..\.
devenv zlib.sln /project "zlibstatic" /Clean
devenv zlib.sln /project "zlibstatic" /Rebuild "Release"
mkdir ..\Release\x64
move /Y .\Release\zlibstatic.lib ..\Release\x64\zlibstatic.lib

move /Y .\zconf.h ..\zconf.h

:done 

pause
