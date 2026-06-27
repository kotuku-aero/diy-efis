cmake_minimum_required(VERSION 3.15)

# ============================================================================
# Platform Detection and Project Declaration
# ============================================================================
project(dial VERSION 1.0.0 LANGUAGES C)

# Include common MFD configuration
include(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/MFDCommon.cmake)

# ============================================================================
# Project-Specific Configuration
# ============================================================================

set(TARGET_NAME dial)

# XML layouts for this application
set(XML_LAYOUTS dial)

# ============================================================================
# Source Files
# ============================================================================

set(APP_SOURCES
    main.c
	dial.xml
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

add_executable(${TARGET_NAME} ${APP_SOURCES})

# Local include directories
target_include_directories(${TARGET_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# ============================================================================
# Platform Configuration
# ============================================================================

if(MIPS32_BUILD)
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
set_target_properties(${TARGET_NAME} PROPERTIES FOLDER "c-apps")
