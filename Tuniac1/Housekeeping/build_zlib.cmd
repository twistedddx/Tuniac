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

rem ######## zlib
cd zlib\

rem #zlib Release x86
del ".\CMakeCache.txt"
"%ProgramFiles(x86)%\CMake 2.8\bin\cmake" -DBUILD_SHARED_LIBS=OFF -G "Visual Studio 12" zlibstatic .
devenv zlib.sln /project "zlibstatic" /Clean
devenv zlib.sln /project "zlibstatic" /Rebuild "Release"
mkdir .\Release\x86
move /Y .\Release\zlibstatic.lib .\Release\x86\zlibstatic.lib


rem if "64bitcl"=="false" goto done
call "%Programfiles(x86)%\Microsoft Visual Studio 11.0\VC\vcvarsall.bat" x64


rem ######## zlib x64

rem #zlib Release x64
del ".\CMakeCache.txt"
"%ProgFiles86Root%\CMake 2.8\bin\cmake" -DBUILD_SHARED_LIBS=OFF -G "Visual Studio 12 Win64" zlibstatic .
devenv zlib.sln /project "zlibstatic" /Clean
devenv zlib.sln /project "zlibstatic" /Rebuild "Release"
mkdir .\Release\x64
move /Y .\Release\zlibstatic.lib .\Release\x64\zlibstatic.lib

cd ..\




:done 

pause
