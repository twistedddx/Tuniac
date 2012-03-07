cd ..\..\..\

rem ######## turbojpeglib
cd libjpeg-turbo\trunk\

rem #Release x86:
del ".\CMakeCache.txt"
call "C:\Program Files\Microsoft SDKs\Windows\v7.1\Bin\SetEnv.cmd" /Release /x86 /win7
call "C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\bin\vcvars32.bat"
"C:\Program Files (x86)\CMake 2.8\bin\cmake" -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release -DWITH_JPEG8=1 .
nmake clean
nmake
del turbojpeg-static32.lib
rename turbojpeg-static.lib turbojpeg-static32.lib

rem #Release x64:
del ".\CMakeCache.txt"
call "C:\Program Files\Microsoft SDKs\Windows\v7.1\Bin\SetEnv.cmd" /Release /x64 /win7
call "C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\bin\x86_amd64\vcvarsx86_amd64.bat"
"C:\Program Files (x86)\CMake 2.8\bin\cmake" -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release -DWITH_JPEG8=1 .
nmake clean
nmake
del turbojpeg-static64.lib
rename turbojpeg-static.lib turbojpeg-static64.lib

cd ..\..\