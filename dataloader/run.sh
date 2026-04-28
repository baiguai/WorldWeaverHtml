#!/bin/bash
# Run script for Linux

cd "$(dirname "$0")"

if [ ! -f "build/dataloader" ]; then
    echo "dataloader not found. Building first..."
    ./build.sh
fi

./build/dataloader
