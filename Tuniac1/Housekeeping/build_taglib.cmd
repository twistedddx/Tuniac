cd ..\..\..\

rem ######## zlib
cd zlib\

rem #Release x86
del ".\CMakeCache.txt"
call "C:\Program Files\Microsoft SDKs\Windows\v7.1\Bin\SetEnv.cmd" /Release /x86 /win7
call "C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\bin\vcvars32.bat"
"C:\Program Files (x86)\CMake 2.8\bin\cmake" -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=OFF .
nmake clean
nmake
del zlib32.lib
rename zlib.lib zlib32.lib

rem #Release x64
del ".\CMakeCache.txt"
call "C:\Program Files\Microsoft SDKs\Windows\v7.1\Bin\SetEnv.cmd" /Release /x64 /win7
call "C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\bin\x86_amd64\vcvarsx86_amd64.bat"
"C:\Program Files (x86)\CMake 2.8\bin\cmake" -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=OFF .
nmake clean
nmake
del zlib64.lib
rename zlib.lib zlib64.lib

cd ..\


rem ######## Taglib
cd taglib\

rem #Release x86
del ".\CMakeCache.txt"
call "C:\Program Files\Microsoft SDKs\Windows\v7.1\Bin\SetEnv.cmd" /Release /x86 /win7
call "C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\bin\vcvars32.bat"
"C:\Program Files (x86)\CMake 2.8\bin\cmake" -G "NMake Makefiles" -DZLIB_INCLUDE_DIR=..\zlib\ -DZLIB_LIBRARY=..\zlib32.lib -DCMAKE_BUILD_TYPE=Release -DENABLE_STATIC=1 -DWITH_MP4=1 -DWITH_MP4=1 .
nmake clean
nmake
del .\taglib\tag-static32.lib
rename .\taglib\tag.lib tag-static32.lib

rem #Release x64
del ".\CMakeCache.txt"
call "C:\Program Files\Microsoft SDKs\Windows\v7.1\Bin\SetEnv.cmd" /Release /x64 /win7
call "C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\bin\x86_amd64\vcvarsx86_amd64.bat"
"C:\Program Files (x86)\CMake 2.8\bin\cmake" -G "NMake Makefiles" -DZLIB_INCLUDE_DIR=..\zlib\ -DZLIB_LIBRARY=..\zlib64.lib -DCMAKE_BUILD_TYPE=Release -DENABLE_STATIC=1 -DWITH_MP4=1 -DWITH_MP4=1 .
nmake clean
nmake
del .\taglib\tag-static64.lib
rename .\taglib\tag.lib tag-static64.lib

rem #Debug x86
del ".\CMakeCache.txt"
call "C:\Program Files\Microsoft SDKs\Windows\v7.1\Bin\SetEnv.cmd" /Debug /x86 /win7
call "C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\bin\vcvars32.bat"
"C:\Program Files (x86)\CMake 2.8\bin\cmake" -G "NMake Makefiles" -DZLIB_INCLUDE_DIR=..\zlib\ -DZLIB_LIBRARY=..\zlib32.lib -DCMAKE_BUILD_TYPE=Debug -DENABLE_STATIC=1 -DWITH_MP4=1 -DWITH_MP4=1 .
nmake clean
nmake
del .\taglib\tag-static32d.lib
rename .\taglib\tag.lib tag-static32d.lib

rem #Debug x64
del ".\CMakeCache.txt"
call "C:\Program Files\Microsoft SDKs\Windows\v7.1\Bin\SetEnv.cmd" /Debug /x64 /win7
call "C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\bin\x86_amd64\vcvarsx86_amd64.bat"
"C:\Program Files (x86)\CMake 2.8\bin\cmake" -G "NMake Makefiles" -DZLIB_INCLUDE_DIR=..\zlib\ -DZLIB_LIBRARY=..\zlib64.lib -DCMAKE_BUILD_TYPE=Debug -DENABLE_STATIC=1 -DWITH_MP4=1 -DWITH_MP4=1 .
nmake clean
nmake
del .\taglib\tag-static64d.lib
rename .\taglib\tag.lib tag-static64d.lib

cd ..\
