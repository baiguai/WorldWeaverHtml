#!/bin/bash
# Cross-compile script for Windows from Linux
# This requires MinGW-w64 cross-compiler and wxWidgets for Windows

set -e

echo "WorldWeaver DataLoader - Windows Cross-Compile Script"
echo "=================================================="

# Check for MinGW-w64 cross-compiler
if ! command -v x86_64-w64-mingw32-g++ &> /dev/null; then
    echo "MinGW-w64 cross-compiler not found."
    echo "Install with: sudo apt-get install mingw-w64"
    exit 1
fi

echo "MinGW-w64 found."

# Check for wxWidgets - this is tricky for cross-compilation
# Option 1: Use MXE (M Cross Environment)
if command -v mxe &> /dev/null; then
    echo "MEX found, using MXE..."
    export PATH=/path/to/mxe/usr/bin:$PATH
    # MXE has pre-built wxWidgets
    i686-w64-mingw32.static-cmake ..
else
    echo ""
    echo "WARNING: Cross-compiling wxWidgets applications requires"
    echo "wxWidgets libraries built for Windows."
    echo ""
    echo "Options:"
    echo "1. Install MXE (recommended):"
    echo "   git clone https://github.com/mxe/mxe.git"
    echo "   cd mxe && make wxwidgets"
    echo ""
    echo "2. Build wxWidgets for Windows manually:"
    echo "   - Download wxWidgets source"
    echo "   - Cross-compile using MinGW-w64"
    echo ""
    echo "3. Build natively on Windows (simplest)"
    echo ""
    echo "Continuing with basic cross-compilation attempt..."
    echo "This will likely fail without wxWidgets Windows libraries."
fi

# Create build directory for Windows build
mkdir -p build-windows
cd build-windows

# Set up cross-compilation
export CC=x86_64-w64-mingw32-gcc
export CXX=x86_64-w64-mingw32-g++

echo ""
echo "Running CMake for cross-compilation..."
echo "Note: You need wxWidgets built for Windows for this to work."

# Try to run cmake
cmake .. -DCMAKE_SYSTEM_NAME=Windows \
       -DCMAKE_C_COMPILER=x86_64-w64-mingw32-gcc \
       -DCMAKE_CX_COMPILER=x86_64-w64-mingw32-g++ \
       || echo "CMake configuration failed - see note above about wxWidgets"

echo ""
echo "If CMake succeeded, run: make -j$(nproc)"
echo "Output will be: build-windows/dataloader.exe"

# Alternative: Simple static build attempt
echo ""
echo "======================================="
echo "ALTERNATIVE: Simple build (if wxWidgets is available)"
echo "======================================="
echo "If you have wxWidgets for Windows libraries, try:"
echo "  cd build-windows"
echo "  x86_64-w64-mingw32-cmake .."
echo "  make"
