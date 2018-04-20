call "%Programfiles(x86)%\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat" x86

cd ..\..\..\

rem ######## libpng
cd libpng\


rem #Release x86:
rmdir /s /q .\build32
mkdir .\build32
cd build32\
"%ProgramFiles%\CMake\bin\cmake" -DPNG_STATIC=1 -DBUILD_SHARED_LIBS=OFF -DZLIB_INCLUDE_DIR=..\..\zlib\ -DZLIB_LIBRARY=..\..\zlib\Release\x86\zlibstatic.lib -G "Visual Studio 15" ..\.
devenv libpng.sln /project "png_static" /Clean
devenv libpng.sln /project "png_static" /Rebuild "Release"
if exist ..\Release\x86\ rmdir /S /Q ..\Release\x86
mkdir ..\Release\x86
move /Y .\Release\libpng17_static.lib ..\Release\x86\libpng17_static.lib

rem #Debug x86:
devenv libpng.sln /project "png_static" /Clean
devenv libpng.sln /project "png_static" /Rebuild "Debug"
if exist ..\Debug\x86\ rmdir /S /Q ..\Debug\x86
mkdir ..\Debug\x86
move /Y .\Debug\libpng17_staticd.lib ..\Debug\x86\libpng17_staticd.lib
move /Y .\png_static.dir\Debug\png_static.pdb ..\Debug\x86\png_static.pdb


cd ..


call "%Programfiles(x86)%\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat" amd64


rem #Release x64:
rmdir /s /q .\build64
mkdir .\build64
cd build64\
"%ProgramFiles%\CMake\bin\cmake" -DPNG_STATIC=1 -DBUILD_SHARED_LIBS=OFF -DZLIB_INCLUDE_DIR=..\..\zlib\ -DZLIB_LIBRARY=..\..\zlib\Release\x64\zlibstatic.lib -G "Visual Studio 15 Win64" ..\.
devenv libpng.sln /project "png_static" /Clean
devenv libpng.sln /project "png_static" /Rebuild "Release"
if exist ..\Release\x64\ rmdir /S /Q ..\Release\x64
mkdir ..\Release\x64
move /Y .\Release\libpng17_static.lib ..\Release\x64\libpng17_static.lib

rem #Debug x64:
devenv libpng.sln /project "png_static" /Clean
devenv libpng.sln /project "png_static" /Rebuild "Debug"
if exist ..\Debug\x64\ rmdir /S /Q ..\Debug\x64
mkdir ..\Debug\x64
move /Y .\Debug\libpng17_staticd.lib ..\Debug\x64\libpng17_staticd.lib
move /Y .\png_static.dir\Debug\png_static.pdb ..\Debug\x64\png_static.pdb

move /Y .\pnglibconf.h ..\pnglibconf.h


pause