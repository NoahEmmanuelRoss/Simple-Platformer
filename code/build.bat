@echo off

mkdir ..\build
pushd ..\build
cl -Zi ..\code\win32_platformer.cpp user32.lib gdi32.lib
popd