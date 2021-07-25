call "%Programfiles(x86)%\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" x86

echo Working dir: %cd%

cd ..\..\..\
echo Working dir: %cd%

rem ######## turbojpeglib
cd libjpeg-turbo\
echo Working dir: %cd%

rem #turbojpeg Release x86:
rmdir /s /q .\build32
mkdir .\build32
cd build32\
echo Working dir: %cd%
"%ProgramFiles%\CMake\bin\cmake" -DCMAKE_BUILD_TYPE=Release -G "Visual Studio 16 2019" -A Win32 ..\.
devenv libjpeg-turbo.sln /project "jpeg-static" /Clean
devenv libjpeg-turbo.sln /project "jpeg-static" /Rebuild "Release"
if exist ..\Release\x86\ rmdir /S /Q ..\Release\x86
timeout /t 1 /nobreak > NUL
mkdir ..\Release\x86
timeout /t 1 /nobreak > NUL
move /Y .\Release\jpeg-static.lib ..\Release\x86\jpeg-static.lib


cd ..
echo Working dir: %cd%

call "%Programfiles(x86)%\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" amd64


rem #turbojpeg Release x64
rmdir /s /q .\build64
mkdir .\build64
cd build64\
echo Working dir: %cd%
"%ProgramFiles%\CMake\bin\cmake" -DCMAKE_BUILD_TYPE=Release -G "Visual Studio 16 2019" -A x64 ..\.
devenv libjpeg-turbo.sln /Project "jpeg-static" /Clean
devenv libjpeg-turbo.sln /Project "jpeg-static" /Rebuild "Release"
if exist ..\Release\x64\ rmdir /S /Q ..\Release\x64
timeout /t 1 /nobreak > NUL
mkdir ..\Release\x64
timeout /t 1 /nobreak > NUL
move /Y .\Release\jpeg-static.lib ..\Release\x64\jpeg-static.lib

move /Y .\jconfig.h ..\jconfig.h


pause