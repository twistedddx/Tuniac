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

rem ######## Taglib
cd taglib\

rem #taglib Release x86
del ".\CMakeCache.txt"
"%ProgFiles86Root%\CMake\bin\cmake" -DENABLE_STATIC=1 -DBUILD_SHARED_LIBS=OFF -DZLIB_INCLUDE_DIR=..\zlib\ -DZLIB_LIBRARY=..\zlib\Release\x86\zlibstatic.lib -G "Visual Studio 12" .
devenv taglib.sln /project "tag" /Clean
devenv taglib.sln /project "tag" /Rebuild "Release"
mkdir .\taglib\Release\x86
move /Y .\taglib\Release\tag.lib .\taglib\Release\x86\tag.lib

rem #taglib Debug x86
devenv taglib.sln /project "tag" /Clean
devenv taglib.sln /project "tag" /Rebuild "Debug"
mkdir .\taglib\Debug\x86
move /Y .\taglib\Debug\tag.lib .\taglib\Debug\x86\tag.lib
move /Y .\taglib\Debug\tag.pdb .\taglib\Debug\x86\tag.pdb


rem if "64bitcl"=="false" goto done
call "%Programfiles(x86)%\Microsoft Visual Studio 12.0\VC\vcvarsall.bat" x64


rem ######## Taglib x64


rem #taglib Release x64
del ".\CMakeCache.txt"
"%ProgFiles86Root%\CMake\bin\cmake" -DENABLE_STATIC=1 -DBUILD_SHARED_LIBS=OFF -DZLIB_INCLUDE_DIR=..\zlib\ -DZLIB_LIBRARY=..\zlib\Release\x64\zlibstatic.lib -G "Visual Studio 12 Win64" .
devenv taglib.sln /project "tag" /Clean
devenv taglib.sln /project "tag" /Rebuild "Release"
mkdir .\taglib\Release\x64
move /Y .\taglib\Release\tag.lib .\taglib\Release\x64\tag.lib

rem #taglib Debug x64
devenv taglib.sln /project "tag" /Clean
devenv taglib.sln /project "tag" /Rebuild "Debug"
mkdir .\taglib\Debug\x64
move /Y .\taglib\Debug\tag.lib .\taglib\Debug\x64\tag.lib
move /Y .\taglib\Debug\tag.pdb .\taglib\Debug\x64\tag.pdb


:done 

pause
