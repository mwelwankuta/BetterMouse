@echo off
if not exist "Release" mkdir Release

windres resource.rc -O coff -o resource.res
if errorlevel 1 (
    echo Error: Failed to compile resource file
    exit /b 1
)

g++ main.cpp resource.res -o .\Release\BetterMouse.exe -mwindows
if errorlevel 1 (
    echo Error: Failed to compile executable
    exit /b 1
)

if exist "resource.res" del "resource.res"