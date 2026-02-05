#!/bin/bash

set -e

executable_name="ColonySimulator"
build_type="RelWithDebInfo"
build_dir="build"
cmake_flags=""
run_wrapper=""
is_windows=false
use_perf=false

print_help() {
    echo "Usage: $0 [OPTION]..."
    echo "Compile and run $executable_name"
    echo ""
    echo "With no OPTION, compile and run the release build"
    echo ""
    echo "-h, --help           Print help"
    echo "-d, --debug          Compile the debug build and run it with gdb"
    echo "-w, --windows        Compile the Windows build and run it with Wine"
    echo "-p, --profile        Compile the profile build, profile it with perf and display the data with hotspot"
    echo "-m, --memory-leak    Compile the memory leak build and run it"
    echo "-M, --mods           Compile with modding support (doesn't create generate bindings automatically)"
    exit 0
}

while [[ $# -gt 0 ]]; do
    case "$1" in
        --*) # Handle long flags
            case "$1" in
                --help)         print_help ;;
                --debug)        build_type="Debug"; build_dir="build_debug"; run_wrapper="gdb -ex run --args" ;;
                --windows)      build_dir="build_windows"; is_windows=true; cmake_flags="$cmake_flags -DCMAKE_TOOLCHAIN_FILE=$(pwd)/mingw-w64-x86_64.cmake" ;;
                --profile)      build_dir="build_profile"; use_perf=true; cmake_flags="$cmake_flags -DCMAKE_CXX_FLAGS='-fno-omit-frame-pointer'" ;;
                --memory-leak)  build_dir="build_memory"; cmake_flags="$cmake_flags -DCMAKE_CXX_FLAGS='-fsanitize=address'" ;;
                --mods)         cmake_flags="$cmake_flags -DLUA_BINDINGS=ON" ;;
                *) echo "Unknown option: $1"; print_help ;;
            esac
            shift
            ;;
        -*) # Handle short flags
            if [[ "$1" == "-" ]]; then shift; continue; fi
            
            # Extract flags string (everything after the dash)
            flags="${1#-}"
            for (( i=0; i<${#flags}; i++ )); do
                char="${flags:i:1}"
                case "$char" in
                    h) print_help ;;
                    d) build_type="Debug"; build_dir="build_debug"; run_wrapper="gdb -ex run --args" ;;
                    w) build_dir="build_windows"; is_windows=true; cmake_flags="$cmake_flags -DCMAKE_TOOLCHAIN_FILE=$(pwd)/mingw-w64-x86_64.cmake" ;;
                    p) build_dir="build_profile"; use_perf=true; cmake_flags="$cmake_flags -DCMAKE_CXX_FLAGS='-fno-omit-frame-pointer'" ;;
                    m) build_dir="build_memory"; cmake_flags="$cmake_flags -DCMAKE_CXX_FLAGS='-fsanitize=address'" ;;
                    M) cmake_flags="$cmake_flags -DLUA_BINDINGS=ON" ;;
                    c) rm -rf build build_debug build_windows build_profile build_memory ;;
                    *) echo "Unknown flag: -$char"; print_help ;;
                esac
            done
            shift
            ;;
        *) # Unrecognized
            echo "Unknown argument: $1"
            print_help
            ;;
    esac
done

clear
cmake -B "$build_dir" -DCMAKE_build_type="$build_type" $cmake_flags
cmake --build "$build_dir" -j$(nproc 2>/dev/null || sysctl -n hw.ncpu)

binary_path="./$build_dir/bin/$executable_name"
[ "$is_windows" = true ] && binary_path="${binary_path}.exe"

if [ "$use_perf" = true ]; then
    perf record --call-graph dwarf "$binary_path"
    hotspot perf.data
elif [ "$is_windows" = true ]; then
    wine "$binary_path"
else
    $run_wrapper "$binary_path"
fi
