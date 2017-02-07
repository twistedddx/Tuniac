set ProgFilesRoot=%ProgramFiles%
set ProgFiles86Root=%ProgramFiles(x86)%
if not "%ProgFiles86Root%"=="" goto 64bitWindows
set ProgFiles86Root=%ProgramFiles%
:64bitWindows

set 64bitcl=true
if exist "%ProgFiles86Root%\Microsoft Visual Studio\2017\Community\VC\Tools\MSVC\14.10.24911\bin\HostX86\x86\cl.exe" goto 64bitcl
set 64bitcl=false
:64bitcl


call "%Programfiles(x86)%\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat" x86

cd ..\..\..\

rem ######## zlib
cd zlib\


rem #zlib Release x86
rmdir /s /q .\build32
mkdir .\build32
cd build32\
"%ProgFilesRoot%\CMake\bin\cmake" -DBUILD_SHARED_LIBS=OFF -G "Visual Studio 15" zlibstatic ..\.
devenv zlib.sln /project "zlibstatic" /Clean
devenv zlib.sln /project "zlibstatic" /Rebuild "Release"
if exist  ..\Release\x86\ rmdir /S /Q ..\Release\x86
mkdir ..\Release\x86
move /Y .\Release\zlibstatic.lib ..\Release\x86\zlibstatic.lib


cd ..


rem if "64bitcl"=="false" goto done
call "%Programfiles(x86)%\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat" amd64


rem #zlib Release x64
rmdir /s /q .\build64
mkdir .\build64
cd build64\
"%ProgFilesRoot%\CMake\bin\cmake" -DBUILD_SHARED_LIBS=OFF -G "Visual Studio 15 Win64" zlibstatic ..\.
devenv zlib.sln /project "zlibstatic" /Clean
devenv zlib.sln /project "zlibstatic" /Rebuild "Release"
if exist ..\Release\x64\ rmdir /S /Q ..\Release\x64
mkdir ..\Release\x64
move /Y .\Release\zlibstatic.lib ..\Release\x64\zlibstatic.lib

move /Y .\zconf.h ..\zconf.h

:done 

pause
