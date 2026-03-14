# PIC32MZ-DA Cross-Compilation Toolchain File (No FPU - Soft Float)
# For use with custom pic32-gcc toolchain
# Use with: cmake -DCMAKE_TOOLCHAIN_FILE=pic32-DA-toolchain.cmake

cmake_minimum_required(VERSION 3.16)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR mips)

set(CMAKE_C_COMPILER_ID GNU)
set(CMAKE_C_COMPILER_VERSION "15.2.0")
set(CMAKE_CXX_COMPILER_ID GNU)
set(CMAKE_CXX_COMPILER_VERSION "15.2.0")

# ============================================================================
# Toolchain Paths
# ============================================================================

# Custom pic32-gcc toolchain location
if(WIN32)
    set(TOOLCHAIN_PATH "C:/pic32" CACHE PATH "Path to pic32 toolchain")
    set(TOOLCHAIN_EXT ".exe")
else()
    set(TOOLCHAIN_PATH "/opt/pic32" CACHE PATH "Path to pic32 toolchain")
    set(TOOLCHAIN_EXT "")
endif()

set(TOOLCHAIN_PREFIX "mipsisa32r2-elf-")

# Compilers - use explicit extension since CMAKE_EXECUTABLE_SUFFIX isn't set yet
set(CMAKE_C_COMPILER "${TOOLCHAIN_PATH}/bin/${TOOLCHAIN_PREFIX}gcc${TOOLCHAIN_EXT}")
set(CMAKE_CXX_COMPILER "${TOOLCHAIN_PATH}/bin/${TOOLCHAIN_PREFIX}g++${TOOLCHAIN_EXT}")
set(CMAKE_ASM_COMPILER "${TOOLCHAIN_PATH}/bin/${TOOLCHAIN_PREFIX}gcc${TOOLCHAIN_EXT}")

# Binary utilities
set(CMAKE_AR "${TOOLCHAIN_PATH}/bin/${TOOLCHAIN_PREFIX}ar${TOOLCHAIN_EXT}" CACHE FILEPATH "Archiver")
set(CMAKE_RANLIB "${TOOLCHAIN_PATH}/bin/${TOOLCHAIN_PREFIX}ranlib${TOOLCHAIN_EXT}" CACHE FILEPATH "Ranlib")
set(CMAKE_OBJCOPY "${TOOLCHAIN_PATH}/bin/${TOOLCHAIN_PREFIX}objcopy${TOOLCHAIN_EXT}" CACHE FILEPATH "Objcopy")
set(CMAKE_OBJDUMP "${TOOLCHAIN_PATH}/bin/${TOOLCHAIN_PREFIX}objdump${TOOLCHAIN_EXT}" CACHE FILEPATH "Objdump")
set(CMAKE_SIZE "${TOOLCHAIN_PATH}/bin/${TOOLCHAIN_PREFIX}size${TOOLCHAIN_EXT}" CACHE FILEPATH "Size")
set(CMAKE_NM "${TOOLCHAIN_PATH}/bin/${TOOLCHAIN_PREFIX}nm${TOOLCHAIN_EXT}" CACHE FILEPATH "NM")

# ============================================================================
# MIPS32MZ-DA Specific Flags (No FPU - Soft Float)
# ============================================================================

# Additional MIPS flags
set(MIPS32_MIPS_FLAGS "-mno-mips16 -mno-micromips -march=mips32r2 -EL -msoft-float -Wno-extra -Wno-pedantic -std=c99 -mabi=32")

# Warning and optimization defaults
set(MIPS32_WARN_FLAGS "-Wall -Wextra")

# Combine architecture flags
set(MIPS32_CPU_FLAGS "${MIPS32_MIPS_FLAGS}")

# Build include flags string
set(MIPS32_INCLUDE_FLAGS "")

if(MIPS32_PLATFORM_INCLUDE_PATH)
    set(MIPS32_INCLUDE_FLAGS "${MIPS32_INCLUDE_FLAGS} -I${MIPS32_PLATFORM_INCLUDE_PATH}")
endif()

# ============================================================================
# Compiler Flags
# ============================================================================

# Skip compiler tests that would try to link Windows executables
set(CMAKE_C_COMPILER_WORKS 1)
set(CMAKE_CXX_COMPILER_WORKS 1)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# C Flags - include atom and DFP paths
set(CMAKE_C_FLAGS_INIT "${MIPS32_CPU_FLAGS} ${MIPS32_INCLUDE_FLAGS} ${MIPS32_WARN_FLAGS} -G0")
set(CMAKE_C_FLAGS_DEBUG_INIT "-Og -g3 -DDEBUG")
set(CMAKE_C_FLAGS_RELEASE_INIT "-O3 -DNDEBUG")
set(CMAKE_C_FLAGS_MINSIZEREL_INIT "-Os -DNDEBUG")
set(CMAKE_C_FLAGS_RELWITHDEBINFO_INIT "-O3 -g -DNDEBUG")

# C++ Flags - include atom and DFP paths
set(CMAKE_CXX_FLAGS_INIT "${MIPS32_CPU_FLAGS}${MIPS32_INCLUDE_FLAGS} ${MIPS32_WARN_FLAGS} -fno-exceptions -fno-rtti")
set(CMAKE_CXX_FLAGS_DEBUG_INIT "-Og -g3 -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE_INIT "-O3 -DNDEBUG")
set(CMAKE_CXX_FLAGS_MINSIZEREL_INIT "-Os -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO_INIT "-O3 -g -DNDEBUG")

# ASM Flags
set(CMAKE_ASM_FLAGS_INIT "${MIPS32_CPU_FLAGS} ${MIPS32_INCLUDE_FLAGS}")

# ============================================================================
# Linker Flags
# ============================================================================

# Base linker flags
set(MIPS32_LINKER_FLAGS "-nostartfiles -Wl,--gc-sections")

set(CMAKE_EXE_LINKER_FLAGS_INIT "${MIPS32_CPU_FLAGS} ${MIPS32_LINKER_FLAGS}")

# ============================================================================
# Search Paths
# ============================================================================

set(CMAKE_FIND_ROOT_PATH "${TOOLCHAIN_PATH}")
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

# ============================================================================
# Build Identification - Export as CACHE variables for use in CMakeLists.txt
# ============================================================================

set(MIPS32_BUILD TRUE CACHE BOOL "Building for MIPS32" FORCE)
set(MIPS32_TOOLCHAIN_PATH "${TOOLCHAIN_PATH}" CACHE PATH "Path to MIPS32 toolchain" FORCE)

# ============================================================================
# Helper Functions - Basic Firmware Operations
# ============================================================================

# Function to generate BIN file from ELF
function(mips32_generate_bin TARGET)
    add_custom_command(TARGET ${TARGET} POST_BUILD
            COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${TARGET}> $<TARGET_FILE_DIR:${TARGET}>/${TARGET}.bin
            COMMENT "Generating BIN file: ${TARGET}.bin"
    )
endfunction()

# Function to print size information
function(mips32_print_size TARGET)
    add_custom_command(TARGET ${TARGET} POST_BUILD
            COMMAND ${CMAKE_SIZE} --format=berkeley $<TARGET_FILE:${TARGET}>
            COMMENT "Size of ${TARGET}:"
    )
endfunction()

# Convenience function to add all post-build steps
function(mips32_firmware TARGET)
    mips32_generate_bin(${TARGET})
    pic32_print_size(${TARGET})
endfunction()

# ============================================================================
# MFD/EDD Application Helper Functions
# ============================================================================

# Common libraries used by all MFD/EDD applications
set(MIPS32_MFD_COMMON_LIBS
    mfdlib
    proton
    canfly
    CACHE STRING "Common libraries for MFD/EDD applications"
)

# Function to configure a MIPS32 MFD/EDD target with standard settings
# Usage: mips32_configure_mfd_target(TARGET_NAME DEVICE_VARIANT CONFIG_FILE)
#   TARGET_NAME   - Name of the target (e.g., kMFD10, kEDD)
#   DEVICE_VARIANT - Device variant string (e.g., 32MZ2064DAR176, 32MZ1064DAR176)
#   CONFIG_FILE   - Boot config file name (e.g., config_mfd10.c, config_mfd3.c)
function(mips32_configure_mfd_target TARGET_NAME DEVICE_VARIANT CONFIG_FILE)
    # Common MIPS32 compile definitions
    target_compile_definitions(${TARGET_NAME} PRIVATE
        __MIPS32MZ__
        _SUPPRESS_PLIB_WARNING
    )

    # Common include directories
    target_include_directories(${TARGET_NAME} PRIVATE
        ${CMAKE_SOURCE_DIR}/lib/proton
        ${CMAKE_SOURCE_DIR}/libs/mfdlib
        ${CMAKE_SOURCE_DIR}/lib/canfly
    )

    # Link libraries in correct order
    target_link_libraries(${TARGET_NAME} PRIVATE
        ${MIPS32_MFD_COMMON_LIBS}
        c
        m
        gcc
    )

    # Linker options
    target_link_options(${TARGET_NAME} PRIVATE
        -T${LINKER_SCRIPT}
        -Wl,--gc-sections
        -Wl,-Map=${CMAKE_BINARY_DIR}/${TARGET_NAME}.map
    )

    # Non-debug optimization
    if(NOT CMAKE_BUILD_TYPE MATCHES Debug)
        target_compile_options(${TARGET_NAME} PRIVATE -O2 -Wall)
    endif()

    # Output properties
    set_target_properties(${TARGET_NAME} PROPERTIES
        C_STANDARD 17
        C_STANDARD_REQUIRED ON
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/firmware
        SUFFIX ".elf"
        FOLDER "devices"
    )

    # Post-build: generate HEX and print size
    mips32_firmware(${TARGET_NAME})

    # Install rule
    install(FILES
        ${CMAKE_BINARY_DIR}/firmware/${TARGET_NAME}.hex
        DESTINATION firmware
    )
endfunction()
