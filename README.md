# Advent of Code

My solutions for [Advent of Code](https://adventofcode.com/) challenges, written in C++.

## Project Structure

```text
AdventOfCode/
├── src/
│   └── 2025/          # Solutions for 2025
│       ├── 01.cpp
│       ├── 02.cpp
│       └── 03.cpp
├── AOC/               # Shared utilities and helper code
│   ├── AOC.cpp
│   └── AOC.hpp
├── build.sh           # Build and run script
└── CMakeLists.txt     # CMake configuration
```

## Prerequisites

- CMake (4.0 or higher)
- C++23 compatible compiler
- Conan package manager
- clang-format (optional, for code formatting)

## Setup

The project uses Conan for dependency management. Dependencies will be automatically installed during the build process.

## Building

To build all solutions:

```bash
./build.sh all
```

Build options:

- `--release` - Build in Release mode (default is Debug)

Example:

```bash
./build.sh all --release
```

## Running Solutions

To run a specific day's solution:

```bash
./build.sh run <day>/<year>
```

Examples:

```bash
./build.sh run 01/2025
./build.sh run 02/2025
./build.sh run 03/2025
```

The script will automatically build the project if needed and then execute the specified solution.

## Other Commands

### Clean Build

Remove the build directory:

```bash
./build.sh clean
```

### Format Code

Run clang-format on all C++ source files:

```bash
./build.sh format
```

### Help

Display all available commands:

```bash
./build.sh help
```

## Adding New Solutions

1. Create a new `.cpp` file in the appropriate year directory (e.g., `src/2025/04.cpp`)
2. Update `CMakeLists.txt` to add the new executable
3. Build and run using `./build.sh run <day>/<year>`

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.
