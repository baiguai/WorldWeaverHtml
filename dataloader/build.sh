#!/bin/bash
# Build script for Linux

set -e

echo "WorldWeaver DataLoader - Build Script"
echo "===================================="

# Check if wxWidgets is installed
if ! command -v wx-config &> /dev/null; then
    echo "wxWidgets not found. Installing..."
    sudo apt-get update
    sudo apt-get install -y libwxgtk3.0-gtk3-dev wx-common
fi

# Create build directory
mkdir -p build
cd build

# Run cmake and build
echo "Configuring with CMake..."
cmake ..

echo "Building..."
make -j$(nproc)

echo ""
echo "Build successful!"
echo "Run with: ./build/dataloader"
echo "Or use: ../run.sh"
