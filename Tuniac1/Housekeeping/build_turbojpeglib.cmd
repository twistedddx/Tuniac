call "%Programfiles%\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x86

echo Working dir: %cd%

cd ..\..\..\
echo Working dir: %cd%

rem #set VCPKG_ROOT for cppunit/zlib/utf8cpp
set VCPKG_ROOT="%cd%\vcpkg"
set PATH=%VCPKG_ROOT%;%PATH%
echo VCPKG_ROOT: %VCPKG_ROOT%

rem ######## libjpeg-turbo
echo libjpeg-turbo
cd libjpeg-turbo\
echo Working dir: %cd%

rem #turbojpeg x86:
echo x86
rmdir /s /q .\build32
mkdir .\build32

"%ProgramFiles%\CMake\bin\cmake" -S . -B .\build32 -DCMAKE_TOOLCHAIN_FILE="%VCPKG_ROOT%/scripts/buildsystems/vcpkg.cmake" -A=Win32

rem # libjpeg-turbo Release x86:
echo Release x86
"%ProgramFiles%\CMake\bin\cmake" --build .\build32 --target jpeg-static --config Release

rem # libjpeg-turbo Debug x86:
echo Debug x86
"%ProgramFiles%\CMake\bin\cmake" --build .\build32 --target jpeg-static --config Debug

rem ######## libjpeg-turbo x64
echo x64
call "%Programfiles%\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" amd64

rem #set VCPKG_ROOT for cppunit/zlib/utf8cpp
cd ..
set VCPKG_ROOT="%cd%\vcpkg"
set PATH=%VCPKG_ROOT%;%PATH%
echo VCPKG_ROOT: %VCPKG_ROOT%
cd libjpeg-turbo\

rmdir /s /q .\build64
mkdir .\build64
timeout /t 1 /nobreak > NUL
"%ProgramFiles%\CMake\bin\cmake" -S . -B .\build64 -DCMAKE_TOOLCHAIN_FILE="%VCPKG_ROOT%/scripts/buildsystems/vcpkg.cmake" -A=x64

rem # LibSPNG Release x64:
echo Release x64
"%ProgramFiles%\CMake\bin\cmake" --build .\build64 --target jpeg-static --config Release

rem # LibSPNG Debug x64:
echo Debug x64
"%ProgramFiles%\CMake\bin\cmake" --build .\build64 --target jpeg-static --config Debug


pause