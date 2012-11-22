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
cd libpng\

rem #Release x86:
del ".\CMakeCache.txt"
"%ProgFiles86Root%\CMake 2.8\bin\cmake" -DCMAKE_BUILD_TYPE=Release -DPNG_STATIC=1 -DBUILD_SHARED_LIBS=OFF -DZLIB_INCLUDE_DIR=..\zlib\ -DZLIB_LIBRARY=..\zlib\Release\x86\zlibstatic.lib -G "NMake Makefiles" .
nmake clean
nmake png16_static
mkdir .\Release\x86
move /Y libpng16_static.lib .\Release\x86\libpng15_static.lib

rem #Debug x86:
del ".\CMakeCache.txt"
"%ProgFiles86Root%\CMake 2.8\bin\cmake" -DCMAKE_BUILD_TYPE=Debug -DPNG_STATIC=1 -DBUILD_SHARED_LIBS=OFF -DZLIB_INCLUDE_DIR=..\zlib\ -DZLIB_LIBRARY=..\zlib\Release\x86\zlibstatic.lib -G "NMake Makefiles" .
nmake clean
nmake png16_static
mkdir .\Debug\x86
move /Y libpng16_staticd.lib .\Debug\x86\libpng15_staticd.lib
move /Y libpng16_staticd.pdb .\Debug\x86\libpng15_staticd.pdb

rem if "64bitcl"=="false" goto done
call "%Programfiles(x86)%\Microsoft Visual Studio 11.0\VC\vcvarsall.bat" x64


rem #Release x64:
del ".\CMakeCache.txt"
"%ProgFiles86Root%\CMake 2.8\bin\cmake" -DCMAKE_BUILD_TYPE=Release -DPNG_STATIC=1 -DBUILD_SHARED_LIBS=OFF -DZLIB_INCLUDE_DIR=..\zlib\ -DZLIB_LIBRARY=..\zlib\Release\x64\zlibstatic.lib -G "NMake Makefiles" .
nmake clean
nmake png16_static
mkdir .\Release\x64
move /Y libpng16_static.lib .\Release\x64\libpng16_static.lib

rem #Debug x64:
del ".\CMakeCache.txt"
"%ProgFiles86Root%\CMake 2.8\bin\cmake" -DCMAKE_BUILD_TYPE=Debug -DPNG_STATIC=1 -DBUILD_SHARED_LIBS=OFF -DZLIB_INCLUDE_DIR=..\zlib\ -DZLIB_LIBRARY=..\zlib\Release\x64\zlibstatic.lib -G "NMake Makefiles" .
nmake clean
nmake png16_static
mkdir .\Debug\x64
move /Y libpng16_staticd.lib .\Debug\x64\libpng15_staticd.lib
move /Y libpng16_staticd.pdb .\Debug\x64\libpng15_staticd.pdb

:done

pause