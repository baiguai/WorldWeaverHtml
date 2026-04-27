WorldWeaver DataLoader
====================

A cross-platform GUI tool for editing database objects in WorldWeaver HTML files.
Features:
- Dark mode interface
- Open, Select Database, Save, Exit buttons
- Keyboard shortcuts: Ctrl+O, Ctrl+D, Ctrl+S, Ctrl+Q
- Selects database object including indentation and semicolon
- Large clipboard support for pasting database content

Prerequisites
=============

Linux (Debian/Ubuntu)
----------------------
sudo apt-get update
sudo apt-get install -y build-essential cmake libwxgtk3.0-gtk3-dev wx-common

Linux (Fedora/RHEL)
-------------------
sudo dnf install -y gcc-c++ cmake wxGTK3-devel

Windows
-------
1. Install MinGW-w64 (http://mingw-w64.org/)
   - Add MinGW bin directory to your PATH
   
2. Install CMake (https://cmake.org/download/)
   
3. Download wxWidgets (https://www.wxwidgets.org/downloads/)
   - Extract to C:\wxWidgets
   - Build with: cd C:\wxWidgets\build\msw && mingw32-make -f makefile.gcc

Note: Windows build scripts assume wxWidgets is at C:\wxWidgets
      Edit build.bat to change this path if needed.

macOS
----
1. Install Homebrew (https://brew.sh/)
   
2. Install dependencies:
   brew install cmake wxwidgets

Building
========

Linux/macOS:
    cd dataloader
    ./build.sh
    ./run.sh

Windows:
    cd dataloader
    build.bat
    run.bat

Manual Build (all platforms):
    mkdir build
    cd build
    cmake ..
    cmake --build .

Usage
=====

1. Run the application
2. Click "Open" or press Ctrl+O to select an HTML file
3. Click "Select DB" or press Ctrl+D to select the database object
   (starts from line indentation, includes "let database = [...];")
4. Paste your new database content (Ctrl+V)
5. Click "Save" or press Ctrl+S to save the file
6. Click "Exit" or press Ctrl+Q to quit

Troubleshooting
===============

Linux: If you get "wxWidgets not found" during build:
    - Make sure wxWidgets is installed: wx-config --version
    - If using a custom install path, set: export WX_CONFIG=/path/to/wx-config

Windows: If build fails:
    - Verify MinGW is in your PATH: where g++
    - Verify wxWidgets path in build.bat
    - Make sure you built wxWidgets libraries before building dataloader

macOS: If wxWidgets is not found:
    - Try: export CMAKE_PREFIX_PATH=/usr/local/opt/wxwidgets
    - Or: export CMAKE_PREFIX_PATH=$(brew --prefix wxwidgets)

Files
=====

dataloader.cpp  - Main application source code
CMakeLists.txt  - CMake build configuration
build.sh        - Linux/macOS build script
build.bat       - Windows build script
run.sh          - Linux/macOS run script
run.bat         - Windows run script
README.txt      - This file

License
=======

This tool is part of the WorldWeaver project.
See the main project for license information.
