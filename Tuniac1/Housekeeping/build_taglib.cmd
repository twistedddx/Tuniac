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

rem ######## zlib
cd zlib\

rem #zlib Release x86
del ".\CMakeCache.txt"
"%ProgFiles86Root%\CMake 2.8\bin\cmake" -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=OFF -G "NMake Makefiles" .
nmake clean
nmake
mkdir .\Release\x86
move /Y zlib.lib .\Release\x86\zlib.lib


cd ..\

rem ######## Taglib
cd taglib\

rem #taglib Release x86
del ".\CMakeCache.txt"
"%ProgFiles86Root%\CMake 2.8\bin\cmake" -DCMAKE_BUILD_TYPE=Release -DENABLE_STATIC=1 -DZLIB_INCLUDE_DIR=..\zlib\ -DZLIB_LIBRARY=..\Release\x86\zlib.lib -G "NMake Makefiles" .
nmake clean
nmake
mkdir .\taglib\Release\x86
move /Y .\taglib\tag.lib .\taglib\Release\x86\tag.lib

rem #taglib Debug x86
del ".\CMakeCache.txt"
"%ProgFiles86Root%\CMake 2.8\bin\cmake" -DCMAKE_BUILD_TYPE=Debug -DENABLE_STATIC=1 -DZLIB_INCLUDE_DIR=..\zlib\ -DZLIB_LIBRARY=..\Release\x86\zlib.lib -G "NMake Makefiles" .
nmake clean
nmake
mkdir .\taglib\Debug\x86
move /Y .\taglib\tag.lib .\taglib\Debug\x86\tag.lib
move /Y .\taglib\tag.pdb .\taglib\Debug\x86\tag.pdb

cd ..\


rem if "64bitcl"=="false" goto done
call "%Programfiles(x86)%\Microsoft Visual Studio 11.0\VC\vcvarsall.bat" x64



rem ######## zlib x64
cd zlib\

rem #zlib Release x64
del ".\CMakeCache.txt"
"%ProgFiles86Root%\CMake 2.8\bin\cmake" -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=OFF -G "NMake Makefiles" .
nmake clean
nmake
mkdir .\Release\x64
move /Y zlib.lib .\Release\x64\zlib.lib

cd ..\

rem ######## Taglib x64
cd taglib\


rem #taglib Release x64
del ".\CMakeCache.txt"
"%ProgFiles86Root%\CMake 2.8\bin\cmake" -DCMAKE_BUILD_TYPE=Release -DENABLE_STATIC=1 -DZLIB_INCLUDE_DIR=..\zlib\ -DZLIB_LIBRARY=..\Release\x64\zlib.lib -G "NMake Makefiles" .
nmake clean
nmake
mkdir .\taglib\Release\x64
move /Y .\taglib\tag.lib .\taglib\Release\x64\tag.lib

rem #taglib Debug x64
del ".\CMakeCache.txt"
"%ProgFiles86Root%\CMake 2.8\bin\cmake" -DCMAKE_BUILD_TYPE=Debug -DENABLE_STATIC=1 -DZLIB_INCLUDE_DIR=..\zlib\ -DZLIB_LIBRARY=..\Release\x64\zlib.lib -G "NMake Makefiles" .
nmake clean
nmake
mkdir .\taglib\Debug\x64
move /Y .\taglib\tag.lib .\taglib\Debug\x64\tag.lib
move /Y .\taglib\tag.pdb .\taglib\Debug\x64\tag.pdb


:done 

pause
