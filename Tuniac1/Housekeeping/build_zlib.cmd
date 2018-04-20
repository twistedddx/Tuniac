call "%Programfiles(x86)%\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat" x86

cd ..\..\..\

rem ######## zlib
cd zlib\


rem #zlib Release x86
rmdir /s /q .\build32
mkdir .\build32
cd build32\
"%ProgramFiles%\CMake\bin\cmake" -DBUILD_SHARED_LIBS=OFF -G "Visual Studio 15" zlibstatic ..\.
devenv zlib.sln /project "zlibstatic" /Clean
devenv zlib.sln /project "zlibstatic" /Rebuild "Release"
if exist  ..\Release\x86\ rmdir /S /Q ..\Release\x86
mkdir ..\Release\x86
move /Y .\Release\zlibstatic.lib ..\Release\x86\zlibstatic.lib


cd ..


call "%Programfiles(x86)%\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat" amd64


rem #zlib Release x64
rmdir /s /q .\build64
mkdir .\build64
cd build64\
"%ProgramFiles%\CMake\bin\cmake" -DBUILD_SHARED_LIBS=OFF -G "Visual Studio 15 Win64" zlibstatic ..\.
devenv zlib.sln /project "zlibstatic" /Clean
devenv zlib.sln /project "zlibstatic" /Rebuild "Release"
if exist ..\Release\x64\ rmdir /S /Q ..\Release\x64
mkdir ..\Release\x64
move /Y .\Release\zlibstatic.lib ..\Release\x64\zlibstatic.lib

move /Y .\zconf.h ..\zconf.h


pause
