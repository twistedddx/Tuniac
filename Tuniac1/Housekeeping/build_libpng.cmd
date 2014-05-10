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
cd libpng\

rem #Release x86:
del ".\CMakeCache.txt"
"%ProgFiles86Root%\CMake 2.8\bin\cmake" -DPNG_STATIC=1 -DBUILD_SHARED_LIBS=OFF -DZLIB_INCLUDE_DIR=..\zlib\ -DZLIB_LIBRARY=..\zlib\Release\x86\zlibstatic.lib -G "Visual Studio 12" .
devenv libpng.sln /project "png17_static" /Clean
devenv libpng.sln /project "png17_static" /Rebuild "Release"
mkdir .\Release\x86
move /Y .\Release\libpng17_static.lib .\Release\x86\libpng17_static.lib

rem #Debug x86:
devenv libpng.sln /project "png17_static" /Clean
devenv libpng.sln /project "png17_static" /Rebuild "Debug"
mkdir .\Debug\x86
move /Y .\Debug\libpng17_staticd.lib .\Debug\x86\libpng17_staticd.lib
move /Y .\Debug\libpng17_staticd.pdb .\Debug\x86\libpng17_staticd.pdb

rem if "64bitcl"=="false" goto done
call "%Programfiles(x86)%\Microsoft Visual Studio 12.0\VC\vcvarsall.bat" x64


rem #Release x64:
del ".\CMakeCache.txt"
"%ProgFiles86Root%\CMake 2.8\bin\cmake" -DPNG_STATIC=1 -DBUILD_SHARED_LIBS=OFF -DZLIB_INCLUDE_DIR=..\zlib\ -DZLIB_LIBRARY=..\zlib\Release\x64\zlibstatic.lib -G "Visual Studio 12 Win64" .
devenv libpng.sln /project "png17_static" /Clean
devenv libpng.sln /project "png17_static" /Rebuild "Release"
mkdir .\Release\x64
move /Y .\Release\libpng17_static.lib .\Release\x64\libpng17_static.lib

rem #Debug x64:
devenv libpng.sln /project "png17_static" /Clean
devenv libpng.sln /project "png17_static" /Rebuild "Debug"
mkdir .\Debug\x64
move /Y .\Debug\libpng17_staticd.lib .\Debug\x64\libpng17_staticd.lib
move /Y .\Debug\libpng17_staticd.pdb .\Debug\x64\libpng17_staticd.pdb

:done

pause