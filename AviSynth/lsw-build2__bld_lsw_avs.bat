@echo off
chcp 1252
call "C:\Program Files (x86)\Microsoft Visual Studio 12.0\Common7\Tools\vsvars32.bat"
devenv LSMASHSourceVCX.sln /upgrade
set CL= /ID:\LSW-BU~1\msys64\mingw32\include
set LINK="libpthread.a" "libopenjpeg.a" "libopus.a" "libswresample.a" "libmsvcrt.a" /LIBPATH:D:\LSW-BU~1\msys64\mingw32\lib /LIBPATH:D:\LSW-BU~1\msys64\mingw32\X86_64~1\lib32 /LIBPATH:D:\LSW-BU~1\msys64\mingw32\lib\gcc\X86_64~1\498796~1.2\32
msbuild.exe LSMASHSourceVCX.sln /target:Rebuild /p:Configuration=Release;Platform="Win32";PlatformToolset=v120_xp
chcp 65001
