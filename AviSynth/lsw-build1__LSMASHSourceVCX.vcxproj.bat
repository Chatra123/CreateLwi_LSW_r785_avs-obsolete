set CL=/I..\..\..\..\..\include /I..\..\msinttypes
set LINK="libpthread.a" "libiconv.a" "libswresample.a" "libmsvcrt.a" /LIBPATH:..\..\..\..\..\i686-w64-mingw32\lib /LIBPATH:..\..\..\..\..\lib\gcc\i686-w64-mingw32\4.8.2 /LIBPATH:..\..\bzip2 /LIBPATH:..\..\..\..\..\lib
@for /d %%1 in (%SystemRoot%\Microsoft.NET\Framework\v*) do @if exist "%%~1\msbuild.exe" set "MSBUILD=%%~1\msbuild.exe"
"%MSBUILD%" LSMASHSourceVCX.vcxproj /target:Rebuild /property:Configuration=Release;Platform=Win32;PlatformToolset=v140 /toolsversion:14.0