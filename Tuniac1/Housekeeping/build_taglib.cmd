call "%Programfiles%\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x86

echo Working dir: %cd%

cd ..\..\..\
echo Working dir: %cd%

rem #set VCPKG_ROOT for cppunit/zlib/utf8cpp
set VCPKG_ROOT="%cd%\vcpkg"
set PATH=%VCPKG_ROOT%;%PATH%
echo VCPKG_ROOT: %VCPKG_ROOT%

rem ######## Taglib
echo Taglib
cd taglib\
echo Working dir: %cd%

rem ######## Taglib x86
echo x86
rmdir /s /q .\build32
mkdir .\build32
timeout /t 1 /nobreak > NUL
"%ProgramFiles%\CMake\bin\cmake" -S . -B .\build32 -DCMAKE_TOOLCHAIN_FILE="%VCPKG_ROOT%/scripts/buildsystems/vcpkg.cmake" -DBUILD_SHARED_LIBS=OFF -DWITH_ZLIB=ON -A=Win32

rem # Taglib Release x86
echo Release x86
"%ProgramFiles%\CMake\bin\cmake" --build .\build32 --target tag --config Release

rem # Taglib Debug x86
echo Debug x86
"%ProgramFiles%\CMake\bin\cmake" --build .\build32 --target tag --config Debug

rem ######## Taglib x64
echo x64
call "%Programfiles%\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" amd64

rem #set VCPKG_ROOT for cppunit/zlib/utf8cpp
cd ..
set VCPKG_ROOT="%cd%\vcpkg"
set PATH=%VCPKG_ROOT%;%PATH%
echo VCPKG_ROOT: %VCPKG_ROOT%
cd taglib\

rmdir /s /q .\build64
mkdir .\build64
timeout /t 1 /nobreak > NUL
"%ProgramFiles%\CMake\bin\cmake" -S . -B .\build64 -DCMAKE_TOOLCHAIN_FILE="%VCPKG_ROOT%/scripts/buildsystems/vcpkg.cmake" -DBUILD_SHARED_LIBS=OFF -DWITH_ZLIB=ON -A=x64

rem #taglib Release x64
echo Release x64
"%ProgramFiles%\CMake\bin\cmake" --build .\build64 --target tag --config Release

rem #taglib Debug x64
echo Debug x64
"%ProgramFiles%\CMake\bin\cmake" --build .\build64 --target tag --config Debug

pause
