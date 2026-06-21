@echo off
IF NOT DEFINED VSCMD_ARG_TGT_ARCH (
    call "C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" x64
)

if NOT EXIST ..\build mkdir ..\build
pushd ..\build

cl -Zi ..\code\win32_handmade.cpp User32.lib Gdi32.lib
popd