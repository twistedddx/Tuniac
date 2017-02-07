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

rem ######## Taglib
cd taglib\

rem #taglib Release x86
rmdir /s /q .\build32
mkdir .\build32
cd build32\
"%ProgFilesRoot%\CMake\bin\cmake" -DBUILD_SHARED_LIBS=OFF -DZLIB_INCLUDE_DIR=..\..\zlib\ -DZLIB_LIBRARY=..\..\zlib\Release\x86\zlibstatic.lib -G "Visual Studio 15" ..\.
devenv taglib.sln /project "tag" /Clean
devenv taglib.sln /project "tag" /Rebuild "Release"
if exist ..\taglib\Release\x86\ rmdir /S /Q ..\taglib\Release\x86
mkdir ..\taglib\Release\x86
move /Y .\taglib\Release\tag.lib ..\taglib\Release\x86\tag.lib

rem #taglib Debug x86
devenv taglib.sln /project "tag" /Clean
devenv taglib.sln /project "tag" /Rebuild "Debug"
if exist ..\taglib\Debug\x86\ rmdir /S /Q ..\taglib\Debug\x86
mkdir ..\taglib\Debug\x86
move /Y .\taglib\Debug\tag.lib ..\taglib\Debug\x86\tag.lib
move /Y .\taglib\Debug\tag.pdb ..\taglib\Debug\x86\tag.pdb


cd ..


rem if "64bitcl"=="false" goto done
call "%Programfiles(x86)%\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat" amd64


rem ######## Taglib x64


rem #taglib Release x64
rmdir /s /q .\build64
mkdir .\build64
cd build64\
"%ProgFilesRoot%\CMake\bin\cmake" -DBUILD_SHARED_LIBS=OFF -DZLIB_INCLUDE_DIR=..\..\zlib\ -DZLIB_LIBRARY=..\..\zlib\Release\x64\zlibstatic.lib -G "Visual Studio 15 Win64" ..\.
devenv taglib.sln /project "tag" /Clean
devenv taglib.sln /project "tag" /Rebuild "Release"
if exist ..\taglib\Release\x64\ rmdir /S /Q ..\taglib\Release\x64
mkdir ..\taglib\Release\x64
move /Y .\taglib\Release\tag.lib ..\taglib\Release\x64\tag.lib

rem #taglib Debug x64
devenv taglib.sln /project "tag" /Clean
devenv taglib.sln /project "tag" /Rebuild "Debug"
if exist ..\taglib\Debug\x64\ rmdir /S /Q ..\taglib\Debug\x64
mkdir ..\taglib\Debug\x64
move /Y .\taglib\Debug\tag.lib ..\taglib\Debug\x64\tag.lib
move /Y .\taglib\Debug\tag.pdb ..\taglib\Debug\x64\tag.pdb

move /Y .\taglib_config.h ..\taglib_config.h

:done 

pause
