@ECHO off
cls

Echo Locating correct DIR
cd ..
Echo ...hopefully there now...
cd ..

Echo Copying to temp folder(does not include .svn as they are hidden)
xcopy /Q Tuniac1 Tuniac1_src /E /I

Echo Entering temp DIR
cd Tuniac1_src

Echo Deleting .vs folders from temp DIR
FOR /F "tokens=*" %%G IN ('DIR /B /AD /S .vs*') DO RMDIR /S /Q "%%G"

Echo Deleting Win32 build folders from temp DIR
FOR /F "tokens=*" %%G IN ('DIR /B /AD /S Win32*') DO RMDIR /S /Q "%%G"

Echo Deleting x64 build folders from temp DIR
FOR /F "tokens=*" %%G IN ('DIR /B /AD /S x64*') DO RMDIR /S /Q "%%G"

Echo Deleting release build folders from temp DIR
FOR /F "tokens=*" %%G IN ('DIR /B /AD /S release*') DO RMDIR /S /Q "%%G"

Echo Deleting debug build folders from temp DIR
FOR /F "tokens=*" %%G IN ('DIR /B /AD /S debug*') DO RMDIR /S /Q "%%G"

Echo Deleting ipch build folders from temp DIR
FOR /F "tokens=*" %%G IN ('DIR /B /AD /S ipch*') DO RMDIR /S /Q "%%G"

Echo Deleting user files from temp DIR
FOR /F "tokens=*" %%G IN ('DIR /B /S *.user') DO DEL /Q "%%G"

Echo Deleting db files from temp DIR
FOR /F "tokens=*" %%G IN ('DIR /B /S *.db') DO DEL /Q "%%G"

Echo Deleting ncb files from temp DIR
FOR /F "tokens=*" %%G IN ('DIR /B /S *.ncb') DO DEL /Q "%%G"

Echo Deleting sdf files from temp DIR
FOR /F "tokens=*" %%G IN ('DIR /B /S *.sdf') DO DEL /Q "%%G"

Echo Deleting filters files from temp DIR
FOR /F "tokens=*" %%G IN ('DIR /B /S *.filters') DO DEL /Q "%%G"

Echo Deleting aps files from temp DIR
FOR /F "tokens=*" %%G IN ('DIR /B /S *.aps') DO DEL /Q "%%G"

Echo Deleting exe files from temp DIR
FOR /F "tokens=*" %%G IN ('DIR /B /S *.exe') DO DEL /Q "%%G"

Echo Date stamps will be wrong in many countries. Such as date and month reversed in America.
SET DATETIME=%date%%time%

SET YY=%DATETIME:~12,2%
SET MT=%DATETIME:~7,2%
SET DD=%DATETIME:~4,2%



Echo Looking for 7zip(32)...
if not exist "%programfiles%\7-Zip\7z.exe" goto 64bit7z
Echo Found 7zip(32). Compressing...
"%programfiles%\7-Zip\7z.exe" a -tzip ..\Tuniac1_src_%YY%%MT%%DD%.zip
goto successzip


:64bit7z
Echo Looking for 7zip(64)...
if not exist "%programfiles(x86)%\7-zip\7z.exe" goto no7z
Echo Found 7zip(64). Compressing...
"%programfiles(x86)%\7-zip\7z.exe" a -tzip ..\Tuniac1_src_%YY%%MT%%DD%.zip
goto successzip


:no7z
Echo looking for winrar(32)...
if not exist "%programfiles%\WinRaR\Rar.exe" goto 64bitwr
Echo Found winrar(32). Compressing...
"%programfiles%\WinRaR\Rar.exe" a -r -idq ..\Tuniac1_src_%YY%%MT%%DD%.zip
goto successzip


:64bitwr
Echo looking for winrar(64)...
if not exist "%programfiles(x86)%\WinRaR\Rar.exe" goto nowr
Echo Found for winrar(64). Compressing...
"%programfiles(x86)%\WinRaR\Rar.exe" a -r -idq ..\Tuniac1_src_%YY%%MT%%DD%.zip
goto successzip


:nowr
Echo can not automatically create zip file. Leaving Tuniac1_src folder behind.
goto exit



:successzip
Echo Zip file created
cd ..
Echo Removing temp dir
RMDIR /S /Q Tuniac1_src
goto exit




:exit
PAUSE