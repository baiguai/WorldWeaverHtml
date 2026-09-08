@echo off
REM Build script for Windows (requires MinGW-w64 and wxWidgets)

echo WorldWeaver DataLoader - Build Script
echo ====================================

REM Check if CMake is installed
where cmake >nul 2>nul
if %errorlevel% neq 0 (
    echo CMake not found. Please install CMake.
    exit /b 1
)

REM Check if g++ is available
where g++ >nul 2>nul
if %errorlevel% neq 0 (
    echo MinGW g++ not found. Please install MinGW-w64.
    exit /b 1
)

REM Create build directory
if not exist build mkdir build
cd build

REM Run CMake and build
echo Configuring with CMake...
cmake -G "MinGW Makefiles" ..

echo Building...
mingw32-make -j4

if %errorlevel% equ 0 (
    echo.
    echo Build successful!
    echo Run with: build\dataloader.exe
    echo Or use: run.bat
) else (
    echo Build failed!
    exit /b 1
)
