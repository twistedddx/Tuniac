call "%Programfiles(x86)%\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat" x86

cd ..\..\..\

rem ######## turbojpeglib
cd libjpeg-turbo\


rem #turbojpeg Release x86:
rmdir /s /q .\build32
mkdir .\build32
cd build32\
"%ProgramFiles%\CMake\bin\cmake" -DCMAKE_BUILD_TYPE=Release -DWITH_JPEG8=1 -G "Visual Studio 15" ..\.
devenv libjpeg-turbo.sln /project "jpeg-static" /Clean
devenv libjpeg-turbo.sln /project "jpeg-static" /Rebuild "Release"
if exist ..\Release\x86\ rmdir /S /Q ..\Release\x86
mkdir ..\Release\x86
move /Y .\Release\jpeg-static.lib ..\Release\x86\jpeg-static.lib


cd ..


call "%Programfiles(x86)%\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat" amd64


rem #turbojpeg Release x64
rmdir /s /q .\build64
mkdir .\build64
cd build64\
"%ProgramFiles%\CMake\bin\cmake" -DCMAKE_BUILD_TYPE=Release -DWITH_JPEG8=1 -G "Visual Studio 15 Win64" ..\.
devenv libjpeg-turbo.sln /Project "jpeg-static" /Clean
devenv libjpeg-turbo.sln /Project "jpeg-static" /Rebuild "Release"
if exist ..\Release\x64\ rmdir /S /Q ..\Release\x64
mkdir ..\Release\x64
move /Y .\Release\jpeg-static.lib ..\Release\x64\jpeg-static.lib

move /Y .\jconfig.h ..\jconfig.h


pause