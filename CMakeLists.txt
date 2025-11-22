cmake_minimum_required(VERSION 3.20)

# Read version from VERSION file
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/VERSION" PROJECT_VERSION)
string(STRIP "${PROJECT_VERSION}" PROJECT_VERSION)

project(graph_descriptors 
    VERSION ${PROJECT_VERSION}
    DESCRIPTION "Modern C++20 graph library"
    HOMEPAGE_URL "https://github.com/pratzl/desc"
    LANGUAGES CXX
)

# Include custom CMake modules
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
include(StandardProjectSettings)
include(CompilerWarnings)

# C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Options
option(BUILD_TESTS "Build unit tests" ON)
option(BUILD_EXAMPLES "Build examples" ON)
option(BUILD_BENCHMARKS "Build benchmarks" OFF)
option(BUILD_DOCS "Build documentation" OFF)
option(ENABLE_COVERAGE "Enable code coverage" OFF)
option(ENABLE_SANITIZERS "Enable sanitizers (address, undefined)" OFF)

# Library target (header-only)
add_library(descriptors INTERFACE)
add_library(graph::descriptors ALIAS descriptors)

target_include_directories(descriptors INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_compile_features(descriptors INTERFACE cxx_std_20)

# Apply compiler warnings
set_project_warnings(descriptors)

# Testing
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Examples
if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# Benchmarks
if(BUILD_BENCHMARKS)
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/benchmark/CMakeLists.txt")
        add_subdirectory(benchmark)
    else()
        message(STATUS "Benchmark directory exists but no CMakeLists.txt found - skipping")
    endif()
endif()

# Documentation
if(BUILD_DOCS)
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/docs/CMakeLists.txt")
        add_subdirectory(docs)
    else()
        message(STATUS "Docs directory exists but no CMakeLists.txt found - skipping")
    endif()
endif()
