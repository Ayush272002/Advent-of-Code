cmake_minimum_required(VERSION 4.0)

set(CMAKE_PROJECT_TOP_LEVEL_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/cmake/conan.cmake)

project(AdventOfCode)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_EXTENSIONS OFF)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/.env
        ${CMAKE_BINARY_DIR}/.env
        COPYONLY)

find_package(CURL REQUIRED)

add_library(AOC STATIC
        AOC/AOC.cpp
        AOC/AOC.hpp
)

target_include_directories(AOC PUBLIC AOC)
target_link_libraries(AOC PUBLIC CURL::libcurl)

file(GLOB_RECURSE AOC_SOLUTIONS "${CMAKE_CURRENT_SOURCE_DIR}/src/*/*.cpp")

foreach(CPP_FILE ${AOC_SOLUTIONS})
    # Extract day from filename (01.cpp → 01)
    get_filename_component(DAY_FILE ${CPP_FILE} NAME_WE)

    # Extract year from folder name (src/2025 → 2025)
    get_filename_component(DAY_DIR ${CPP_FILE} DIRECTORY)
    get_filename_component(YEAR ${DAY_DIR} NAME)

    # Executable name: 01_2025
    set(EXEC_NAME "${DAY_FILE}_${YEAR}")

    add_executable(${EXEC_NAME} ${CPP_FILE})
    target_link_libraries(${EXEC_NAME} PRIVATE AOC)

    set_target_properties(${EXEC_NAME} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    )
endforeach()

