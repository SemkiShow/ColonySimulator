#!/bin/bash

set -e
shopt -s globstar
executable_name=ColonySimulator

if [ -z "$1" ]; then
    echo "Usage: $0 <VERSION>"
fi

# Generate bindings
python3 tools/lua_bindings.py

# Compile for Linux
cmake -B build_release -DCMAKE_BUILD_TYPE=Release -DLUA_BINDINGS=ON
cmake --build build_release -j$(nproc)
cp build_release/bin/$executable_name .

# Compile for Windows
cmake -B build_release_windows -DCMAKE_TOOLCHAIN_FILE=mingw-w64-x86_64.cmake -DCMAKE_BUILD_TYPE=Release -DLUA_BINDINGS=ON
cmake --build build_release_windows -j$(nproc)
cp build_release_windows/bin/$executable_name.exe .

# Zip the dependencies
archive_name=$executable_name-$1
zip $archive_name.zip LICENSE README.md resources/**

# Create the Linux release
cp $archive_name.zip $archive_name-linux-x86_64.zip
zip $archive_name-linux-x86_64.zip $executable_name
rm $executable_name

# Create the Windows release
cp $archive_name.zip $archive_name-windows-x86_64.zip
zip $archive_name-windows-x86_64.zip $executable_name.exe
rm $executable_name.exe

# Create a GitHub release
rm $archive_name.zip
gh release create $1 $archive_name*
rm $archive_name*
