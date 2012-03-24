cd ..\..\..\


set ProgFilesRoot=%ProgramFiles%
set ProgFiles86Root=%ProgramFiles(x86)%
if not "%ProgFiles86Root%"=="" GOTO 64bitWindows
set ProgFiles86Root=%ProgramFiles%
:64bitWindows

set 64bitcl=true
if not exist "%ProgFiles86Root%\Microsoft Visual Studio 10.0\VC\bin\amd64\cl.exe" goto 32bitcl
set 64bitcl=false
:32bitcl


rem ######## zlib
cd zlib\

rem #Release x86
del ".\CMakeCache.txt"
call "%ProgFilesRoot%\Microsoft SDKs\Windows\v7.1\Bin\SetEnv.cmd" /Release /x86 /win7
call "%ProgFiles86Root%\Microsoft Visual Studio 10.0\VC\bin\vcvars32.bat"
"%ProgFiles86Root%\CMake 2.8\bin\cmake" -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=OFF .
nmake clean
nmake
mkdir .\Release\x86
move /Y zlib.lib .\Release\x86\zlib.lib

if "64bitcl"=="false" goto taglibrelease

rem #Release x64
del ".\CMakeCache.txt"
call "%ProgFilesRoot%\Microsoft SDKs\Windows\v7.1\Bin\SetEnv.cmd" /Release /x64 /win7
call "%ProgFiles86Root%\Microsoft Visual Studio 10.0\VC\bin\x86_amd64\vcvarsx86_amd64.bat"
"%ProgFiles86Root%\CMake 2.8\bin\cmake" -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=OFF .
nmake clean
nmake
mkdir .\Release\x64
move /Y zlib.lib .\Release\x64\zlib.lib

cd ..\


:taglibrelease

rem ######## Taglib
cd taglib\

rem #Release x86
del ".\CMakeCache.txt"
call "%ProgFilesRoot%\Microsoft SDKs\Windows\v7.1\Bin\SetEnv.cmd" /Release /x86 /win7
call "%ProgFiles86Root%\Microsoft Visual Studio 10.0\VC\bin\vcvars32.bat"
"%ProgFiles86Root%\CMake 2.8\bin\cmake" -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release -DZLIB_INCLUDE_DIR=..\zlib\ -DZLIB_LIBRARY=..\Release\x86\zlib.lib -DENABLE_STATIC=1 -DWITH_MP4=1 -DWITH_MP4=1 .
nmake clean
nmake
mkdir .\taglib\Release\x86
move /Y .\taglib\tag.lib .\taglib\Release\x86\tag.lib

if "64bitcl"=="false" goto taglibdebug

rem #Release x64
del ".\CMakeCache.txt"
call "%ProgFilesRoot%\Microsoft SDKs\Windows\v7.1\Bin\SetEnv.cmd" /Release /x64 /win7
call "%ProgFiles86Root%\Microsoft Visual Studio 10.0\VC\bin\x86_amd64\vcvarsx86_amd64.bat"
"%ProgFiles86Root%\CMake 2.8\bin\cmake" -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release -DZLIB_INCLUDE_DIR=..\zlib\ -DZLIB_LIBRARY=..\Release\x64\zlib.lib -DENABLE_STATIC=1 -DWITH_MP4=1 -DWITH_MP4=1 .
nmake clean
nmake
mkdir .\taglib\Release\x64
move /Y .\taglib\tag.lib .\taglib\Release\x64\tag.lib

:taglibdebug

rem #Debug x86
del ".\CMakeCache.txt"
call "%ProgFilesRoot%\Microsoft SDKs\Windows\v7.1\Bin\SetEnv.cmd" /Debug /x86 /win7
call "%ProgFiles86Root%\Microsoft Visual Studio 10.0\VC\bin\vcvars32.bat"
"%ProgFiles86Root%\CMake 2.8\bin\cmake" -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Debug -DZLIB_INCLUDE_DIR=..\zlib\ -DZLIB_LIBRARY=..\Release\x86\zlib.lib -DENABLE_STATIC=1 -DWITH_MP4=1 -DWITH_MP4=1 .
nmake clean
nmake
mkdir .\taglib\Debug\x86
move /Y .\taglib\tag.lib .\taglib\Debug\x86\tag.lib
move /Y .\taglib\tag.pdb .\taglib\Debug\x86\tag.pdb

if "64bitcl"=="false" goto done

rem #Debug x64
del ".\CMakeCache.txt"
call "%ProgFilesRoot%\Microsoft SDKs\Windows\v7.1\Bin\SetEnv.cmd" /Debug /x64 /win7
call "%ProgFiles86Root%\Microsoft Visual Studio 10.0\VC\bin\x86_amd64\vcvarsx86_amd64.bat"
"%ProgFiles86Root%\CMake 2.8\bin\cmake" -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Debug -DZLIB_INCLUDE_DIR=..\zlib\ -DZLIB_LIBRARY=..\Release\x64\zlib.lib -DENABLE_STATIC=1 -DWITH_MP4=1 -DWITH_MP4=1 .
nmake clean
nmake
mkdir .\taglib\Debug\x64
move /Y .\taglib\tag.lib .\taglib\Debug\x64\tag.lib
move /Y .\taglib\tag.pdb .\taglib\Debug\x64\tag.pdb

:done

cd ..\
