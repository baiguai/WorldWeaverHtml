@echo off
REM Run script for Windows

cd "%~dp0"

if not exist "build\dataloader.exe" (
    echo dataloader.exe not found. Building first...
    call build.bat
)

build\dataloader.exe
