# CMakeLists.txt for edd4_au_nz Application
# Supports both Windows and PIC32 (atom) platforms

cmake_minimum_required(VERSION 3.15)

# ============================================================================
# Platform Detection and Project Declaration
# ============================================================================

if(DEFINED PIC32_BUILD AND PIC32_FAMILY MATCHES "DA")
    set(IS_PIC32 TRUE)
    project(edd4_au_nz VERSION 1.0.0 LANGUAGES C ASM)
else()
    set(IS_PIC32 FALSE)
    project(edd4_au_nz VERSION 1.0.0 LANGUAGES C)
endif()

# Include common MFD configuration
include(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/MFDCommon.cmake)

# ============================================================================
# Project-Specific Configuration
# ============================================================================

set(TARGET_NAME edd4_au_nz)
set(PIC32_DEVICE "32MZ2064DAR176")
set(RESOURCES_DIR ${CMAKE_SOURCE_DIR}/resources)

# XML layouts for this application
set(XML_LAYOUTS
    edd4_au_nz
)

# ============================================================================
# Source Files
# ============================================================================

set(APP_SOURCES
    main.c
)

# ============================================================================
# XML Code Generation
# ============================================================================

mfd_process_xml_layouts(
    PREFIX ${TARGET_NAME}
    XML_DIR ${CMAKE_CURRENT_SOURCE_DIR}
    LAYOUTS ${XML_LAYOUTS}
    XSL_FILES ${XSL_FILES}
)

list(APPEND APP_SOURCES ${${TARGET_NAME}_GENERATED_SOURCES})

# ============================================================================
# Executable Target
# ============================================================================

if(COMMAND set_target_32bit)
    set_target_32bit(${TARGET_NAME})
endif()

add_executable(${TARGET_NAME} ${APP_SOURCES})

# Local include directories
target_include_directories(${TARGET_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# ============================================================================
# Platform Configuration
# ============================================================================

if(IS_PIC32)
    mfd_configure_pic32_target(
        TARGET ${TARGET_NAME}
        DEVICE ${PIC32_DEVICE}
    )
else()
    mfd_configure_win32_target(TARGET ${TARGET_NAME})
endif()

# ============================================================================
# Summary
# ============================================================================

mfd_print_summary(
    TARGET ${TARGET_NAME}
    LAYOUTS ${XML_LAYOUTS}
    DEVICE ${PIC32_DEVICE}
    RESOURCES_DIR ${RESOURCES_DIR}
)

################################################################################
# Organize in IDE
################################################################################
set_target_properties(edd4_au_nz PROPERTIES FOLDER "c-apps")
