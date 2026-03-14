cmake_minimum_required(VERSION 3.15)

set(CMAKE_C_COMPILER_WORKS 1)
set(CMAKE_CXX_COMPILER_WORKS 1)

cmake_policy(SET CMP0194 NEW)

project(CanFly VERSION 1.0.0 LANGUAGES C CXX)


################################################################################
# MSVC Runtime Library
################################################################################
if(NOT PIC32_BUILD)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

# Set default build type if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

################################################################################
# Build all libraries first (order matters due to dependencies)
################################################################################
message(STATUS "")
message(STATUS "========================================")
message(STATUS "Building Core Libraries")
message(STATUS "========================================")

# Base libraries (no dependencies)
add_subdirectory(src/mfdlib)
add_subdirectory(src/proton)

if(PIC32)
    add_subdirectory(src/canfly)
endif()

link_directories(${CMAKE_SOURCE_DIR}/lib)

################################################################################
# Build Embedded applications (32-bit to match embedded behavior)
################################################################################
message(STATUS "")
message(STATUS "============================================")
message(STATUS "Building Embedded Applications")
message(STATUS "============================================")

add_subdirectory(apps/att_au_nz)
add_subdirectory(apps/edd4_au_nz)
add_subdirectory(apps/hsi_au_nz)
add_subdirectory(apps/nav_au_nz)
add_subdirectory(apps/pfd_au_nz)

add_subdirectory(tools/yehudi)

################################################################################
# Configuration Summary
################################################################################
message(STATUS "")
message(STATUS "========================================")
message(STATUS "CanFly Build Configuration Summary")
message(STATUS "========================================")
message(STATUS "Build Type:           ${CMAKE_BUILD_TYPE}")
message(STATUS "C Compiler:           ${CMAKE_C_COMPILER}")
message(STATUS "C++ Compiler:         ${CMAKE_CXX_COMPILER}")
message(STATUS "========================================")
message(STATUS "")
