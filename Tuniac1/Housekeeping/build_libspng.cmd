call "%Programfiles%\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x86

echo Working dir: %cd%

cd ..\..\..\
echo Working dir: %cd%

rem ######## libpng
cd libspng\
echo Working dir: %cd%

rem #Release x86:
rmdir /s /q .\build32
mkdir .\build32
cd build32\
echo Working dir: %cd%
"%ProgramFiles%\CMake\bin\cmake" -DSPNG_STATIC=1 -DZLIB_INCLUDE_DIR=..\..\zlib\ -DZLIB_LIBRARY=..\..\zlib\Release\x86\zlibstatic.lib -G "Visual Studio 17 2022" -A Win32 ..\.
devenv libspng.sln /project "spng_static" /Clean
devenv libspng.sln /project "spng_static" /Rebuild "Release"
if exist ..\Release\x86\ rmdir /S /Q ..\Release\x86
timeout /t 1 /nobreak > NUL
mkdir ..\Release\x86
timeout /t 1 /nobreak > NUL
move /Y .\Release\spng_static.lib ..\Release\x86\spng_static.lib

rem #Debug x86:
devenv libspng.sln /project "spng_static" /Clean
devenv libspng.sln /project "spng_static" /Rebuild "Debug"
if exist ..\Debug\x86\ rmdir /S /Q ..\Debug\x86
timeout /t 1 /nobreak > NUL
mkdir ..\Debug\x86
timeout /t 1 /nobreak > NUL
move /Y .\Debug\spng_static.lib ..\Debug\x86\spng_static.lib
move /Y .\Debug\spng_static.pdb ..\Debug\x86\spng_static.pdb


cd ..
echo Working dir: %cd%

call "%Programfiles%\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" amd64


rem #Release x64:
rmdir /s /q .\build64
mkdir .\build64
cd build64\
echo Working dir: %cd%
"%ProgramFiles%\CMake\bin\cmake" -DSPNG_STATIC=1 -DZLIB_INCLUDE_DIR=..\..\zlib\ -DZLIB_LIBRARY=..\..\zlib\Release\x64\zlibstatic.lib -G "Visual Studio 17 2022" -A x64 ..\.
devenv libspng.sln /project "spng_static" /Clean
devenv libspng.sln /project "spng_static" /Rebuild "Release"
if exist ..\Release\x64\ rmdir /S /Q ..\Release\x64
timeout /t 1 /nobreak > NUL
mkdir ..\Release\x64
timeout /t 1 /nobreak > NUL
move /Y .\Release\spng_static.lib ..\Release\x64\spng_static.lib

rem #Debug x64:
devenv libspng.sln /project "spng_static" /Clean
devenv libspng.sln /project "spng_static" /Rebuild "Debug"
if exist ..\Debug\x64\ rmdir /S /Q ..\Debug\x64
timeout /t 1 /nobreak > NUL
mkdir ..\Debug\x64
timeout /t 1 /nobreak > NUL
move /Y .\Debug\spng_static.lib ..\Debug\x64\spng_static.lib
move /Y .\Debug\spng_static.pdb ..\Debug\x64\spng_static.pdb


pause