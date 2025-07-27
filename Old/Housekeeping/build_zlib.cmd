call "%Programfiles%\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x86

echo Working dir: %cd%

cd ..\..\..\
echo Working dir: %cd%

rem ######## zlib
cd zlib\
echo Working dir: %cd%

rem #zlib Release x86
rmdir /s /q .\build32
mkdir .\build32
cd build32\
echo Working dir: %cd%
"%ProgramFiles%\CMake\bin\cmake" -DBUILD_SHARED_LIBS=OFF -G "Visual Studio 17 2022" -A Win32 zlibstatic ..\.
devenv zlib.sln /project "zlibstatic" /Clean
devenv zlib.sln /project "zlibstatic" /Rebuild "Release"
if exist  ..\Release\x86\ rmdir /S /Q ..\Release\x86
timeout /t 1 /nobreak > NUL
mkdir ..\Release\x86
timeout /t 1 /nobreak > NUL
move /Y .\Release\zlibstatic.lib ..\Release\x86\zlibstatic.lib


cd ..
echo Working dir: %cd%

call "%Programfiles%\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" amd64


rem #zlib Release x64
rmdir /s /q .\build64
mkdir .\build64
cd build64\
echo Working dir: %cd%
"%ProgramFiles%\CMake\bin\cmake" -DBUILD_SHARED_LIBS=OFF -G "Visual Studio 17 2022" -A x64 zlibstatic ..\.
devenv zlib.sln /project "zlibstatic" /Clean
devenv zlib.sln /project "zlibstatic" /Rebuild "Release"
if exist ..\Release\x64\ rmdir /S /Q ..\Release\x64
timeout /t 1 /nobreak > NUL
mkdir ..\Release\x64
timeout /t 1 /nobreak > NUL
move /Y .\Release\zlibstatic.lib ..\Release\x64\zlibstatic.lib

move /Y .\zconf.h ..\zconf.h


pause
