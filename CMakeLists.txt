cmake_minimum_required(VERSION 3.21)

# Toolchain
if(DEFINED CMAKE_TOOLCHAIN_FILE)
    message(STATUS "Using toolchain: ${CMAKE_TOOLCHAIN_FILE}")
endif()

project(F411Sandbox
    VERSION     1.1
    DESCRIPTION "STM32F411 Sandbox"
    LANGUAGES   C CXX ASM
)

# cmake -DBUILD_TESTS=ON ..
option(BUILD_TESTS  "Build unit tests (runs on host, not target)" OFF)
option(BUILD_TARGET "Build firmware for STM32F411"                ON)

# STANDARDS C/C++
set(CMAKE_C_STANDARD   99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# SUBDIRECTORIES
add_subdirectory(external/drivers)
add_subdirectory(external/common)

if(BUILD_TARGET)
    add_subdirectory(interface)
    add_subdirectory(app)
endif()

if(BUILD_TESTS)
    # Enables the CMake testing framework.
    enable_testing()
    add_subdirectory(external/embedded-foundation/tests)
endif()