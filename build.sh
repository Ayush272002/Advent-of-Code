#!/bin/bash

set -e
set -o pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="$ROOT_DIR/build"
BUILD_TYPE="Debug"

# Arg Parser
COMMAND=""
ARG=""
for arg in "$@"; do
    case $arg in
        --release)
            BUILD_TYPE="Release"
            BUILD_DIR="$ROOT_DIR/build-release"
            ;;
        all|run|clean|format|help)
            COMMAND="$arg"
            ;;
        *)
            ARG="$arg"
            ;;
    esac
done

# Help
function show_help() {
    echo "Usage: ./build.sh [command] [options]"
    echo
    echo "Commands:"
    echo "  all                  Configure and build everything"
    echo "  run <day/year>       Run a specific Advent of Code solution (e.g., 01/2025)"
    echo "  clean                Remove build directory"
    echo "  format               Run clang-format on all source files"
    echo "  help                 Show this page"
    echo
    echo "Options:"
    echo "  --release            Build in Release mode (default: Debug)"
    echo
    echo "Examples:"
    echo "  ./build.sh all"
    echo "  ./build.sh run 01/2025"
    echo "  ./build.sh run 02/2025"
}

# clean
function clean_build() {
    echo "Cleaning build directory..."
    rm -rf "$BUILD_DIR"
}


# configure cmake
function configure() {
    echo "=== Configuring build ($BUILD_TYPE) ==="
    rm -rf "$BUILD_DIR"
    mkdir -p "$BUILD_DIR"
    cd "$BUILD_DIR"

    cmake "$ROOT_DIR" -DCMAKE_BUILD_TYPE="$BUILD_TYPE"
}

# Build all
function build_all() {
    configure
    echo "=== Building project ($BUILD_TYPE) ==="
    cmake --build "$BUILD_DIR" -- -j"$(sysctl -n hw.ncpu)"
}

# Run solution
function run_solution() {
    build_all

    if [[ -z "$ARG" ]]; then
        echo "Error: No solution specified. Use format: day/year (e.g., 01/2025)"
        exit 1
    fi

    # Convert day/year format (01/2025) to executable name (01_2025)
    EXEC_NAME="${ARG//\//_}"
    EXEC_PATH="$BUILD_DIR/bin/$EXEC_NAME"
    
    if [[ ! -f "$EXEC_PATH" ]]; then
        echo "Executable not found: $EXEC_PATH"
        echo "Available executables:"
        ls -1 "$BUILD_DIR/bin/" 2>/dev/null || echo "  (none)"
        exit 1
    fi
    
    echo "=== Running solution: $ARG ==="
    "$EXEC_PATH"
}

# format
function format_code() {
    echo "=== Formatting C++ code ==="
    find "$ROOT_DIR" -type f \( -name "*.cpp" -o -name "*.hpp" \) -exec clang-format -i {} +
    echo "=== Done formatting C++ code ==="
}

# dispatch
case "$COMMAND" in
    all)
        build_all
        ;;
    run)
        run_solution
        ;;
    clean)
        clean_build
        ;;
    format)
        format_code
        ;;
    help|"")
        show_help
        ;;
    *)
        echo "Unknown command: $COMMAND"
        show_help
        exit 1
        ;;
esac