# MFDCommon.cmake - Shared configuration for MFD/EDD applications
# Include this in app CMakeLists.txt after project() declaration
#
# This module provides common functionality for both PIC32 and MSVC builds:
# - XML layout processing and code generation
# - Common include directories
# - Common library sets
# - Configuration summary output

cmake_minimum_required(VERSION 3.15)

# ============================================================================
# Common Library Sets (used by both platforms)
# ============================================================================

set(MFD_COMMON_LIBS
        mfdlib
        proton
)

# ============================================================================
# Platform Detection Helper
# ============================================================================

# ============================================================================
# Yehudi Tool Configuration
# ============================================================================

# The yehudi tool must be built before processing XML files
# This assumes yehudi is defined as a target elsewhere in the project
function(msvc_get_yehudi_tool OUT_VAR)
    set(${OUT_VAR} $<TARGET_FILE:yehudi> PARENT_SCOPE)
endfunction()

# ============================================================================
# XML Code Generation
# ============================================================================

# Process XML layouts and generate C code
# Sets ${PREFIX}_GENERATED_SOURCES in parent scope with list of generated .c files
#
# Usage: mfd_process_xml_layouts(
#            PREFIX prefix_name
#            XML_DIR /path/to/xml/files
#            LAYOUTS layout1 layout2 layout3
#            [XSL_FILES file1.xsl file2.xsl]
#        )
function(mfd_process_xml_layouts)
    cmake_parse_arguments(ARG "" "PREFIX;XML_DIR" "LAYOUTS;XSL_FILES" ${ARGN})

    set(GENERATED_SOURCES "")

    msvc_get_yehudi_tool(YEHUDI_TOOL)

    foreach(layout ${ARG_LAYOUTS})
        set(XML_FILE ${ARG_XML_DIR}/${layout}.xml)
        set(GENERATED_C ${ARG_XML_DIR}/${layout}.c)
        set(GENERATED_H ${ARG_XML_DIR}/${layout}.h)
        set(GENERATED_PRIV_H ${ARG_XML_DIR}/${layout}_priv.h)

        # Only generate on MSVC (PIC32 uses pre-generated files)
        if(MSVC)
            add_custom_command(
                    OUTPUT ${GENERATED_C} ${GENERATED_H} ${GENERATED_PRIV_H}
                    COMMAND ${YEHUDI_TOOL} -a=${layout} ${XML_FILE}
                    DEPENDS
                    ${XML_FILE}
                    yehudi
                    ${ARG_XSL_FILES}
                    WORKING_DIRECTORY ${ARG_XML_DIR}
                    COMMENT "Generating ${layout}.c from ${layout}.xml using yehudi"
                    VERBATIM
            )
        endif()

        list(APPEND GENERATED_SOURCES ${GENERATED_C})

        # Mark as generated
        set_source_files_properties(
                ${GENERATED_C} ${GENERATED_H} ${GENERATED_PRIV_H}
                PROPERTIES GENERATED TRUE
        )

        # IDE organization
        source_group("Generated\\${layout}" FILES
                ${GENERATED_C} ${GENERATED_H} ${GENERATED_PRIV_H}
        )
    endforeach()

    # Return generated sources list
    set(${ARG_PREFIX}_GENERATED_SOURCES ${GENERATED_SOURCES} PARENT_SCOPE)

    # IDE organization for XML files
    source_group("XML Layouts" FILES ${ARG_LAYOUTS})
endfunction()

# ============================================================================
# Common Include Directories
# ============================================================================

# Add common include directories to a target
# Usage: mfd_add_common_includes(TARGET_NAME)
function(mfd_add_common_includes TARGET_NAME)
    target_include_directories(${TARGET_NAME} PRIVATE
            ${CMAKE_SOURCE_DIR}/libs/proton
            ${CMAKE_SOURCE_DIR}/libs/mfdlib
    )
endfunction()

# Add MSVC-specific includes
# Usage: mfd_add_msvc_includes(TARGET_NAME)
function(mfd_add_msvc_includes TARGET_NAME)
    target_include_directories(${TARGET_NAME} PRIVATE
    )
endfunction()

# ============================================================================
# PIC32 Target Configuration
# ============================================================================

# Configure a PIC32 MFD target with all standard settings
# Usage: mfd_configure_pic32_target(
#            TARGET target_name
#            DEVICE device_variant (e.g., 32MZ2064DAR176)
#        )
function(mfd_configure_pic32_target)
    cmake_parse_arguments(ARG "" "TARGET;DEVICE" "" ${ARGN})
    set(LINKER_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/../../src/canfly/canfly_app.ld")

    # Compile definitions
    target_compile_definitions(${ARG_TARGET} PRIVATE
            -DMIPS32_BUILD
    )

    # Includes
    mfd_add_common_includes(${ARG_TARGET})

    # Link libraries (order matters for dependency resolution)
    target_link_libraries(${ARG_TARGET} PRIVATE
            ${MFD_COMMON_LIBS}
            c
            m
            gcc
            canfly
    )

    # Linker options
    target_link_options(${ARG_TARGET} PRIVATE
            -T${LINKER_SCRIPT}
            # this is for a 384k ram size.  Puts the stack pointer
            # at the top of the available memory
            -Wl,--defsym=_stack=0x60000
            -Wl,--gc-sections
            -Wl,-Map=${CMAKE_BINARY_DIR}/${ARG_TARGET}.map
    )

    # Release optimization
    if(NOT CMAKE_BUILD_TYPE MATCHES Debug)
        target_compile_options(${ARG_TARGET} PRIVATE -O2 -Wall )
    endif()

    # Target properties
    set_target_properties(${ARG_TARGET} PROPERTIES
            C_STANDARD 17
            C_STANDARD_REQUIRED ON
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/firmware
            SUFFIX ".elf"
            FOLDER "devices"
    )

    # Post-build steps
    add_custom_command(TARGET ${ARG_TARGET} POST_BUILD
            COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${ARG_TARGET}>
            COMMENT "Program size:")

    # Package the ELF into a binary image with header and SHA256
    find_package(Python3 REQUIRED COMPONENTS Interpreter)
    set(PACKAGE_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/scripts/package_usermode.py")
    set(IMAGE_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/images")

    add_custom_command(TARGET ${ARG_TARGET} POST_BUILD
            COMMAND ${Python3_EXECUTABLE} ${PACKAGE_SCRIPT}
            -e $<TARGET_FILE:${ARG_TARGET}>
            -o ${IMAGE_OUTPUT_DIR}
            -n ${ARG_TARGET}
            COMMENT "Packaging user-mode application: ${ARG_TARGET}.bin"
            VERBATIM)
endfunction()

# ============================================================================
# MSVC Target Configuration
# ============================================================================

# Configure an MSVC MFD target with all standard settings
# Usage: mfd_configure_win32_target(TARGET target_name)
function(mfd_configure_win32_target)
    cmake_parse_arguments(ARG "" "TARGET" "" ${ARGN})

    # Compile definitions
    target_compile_definitions(${ARG_TARGET} PRIVATE
            _CRT_SECURE_NO_WARNINGS
            win32
    )

    # Includes
    mfd_add_common_includes(${ARG_TARGET})
    mfd_add_msvc_includes(${ARG_TARGET})

    # Link libraries - platform-specific w32_canfly library
    if(WIN32)
        # Windows: link with .lib import library
        target_link_libraries(${ARG_TARGET} PRIVATE
                ${MFD_COMMON_LIBS}
                ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/w32_canfly.lib
        )
    else()
        # Linux: link with .so shared library
        target_link_libraries(${ARG_TARGET} PRIVATE
                ${MFD_COMMON_LIBS}
                ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/libw32_canfly.so
        )
    endif()

    # Static runtime
    if(CMAKE_BUILD_TYPE MATCHES Debug)
        set_property(TARGET ${ARG_TARGET} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDebugDLL")
    else()
        set_property(TARGET ${ARG_TARGET} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")
    endif()

    # Target properties
    set_target_properties(${ARG_TARGET} PROPERTIES
            C_STANDARD 17
            C_STANDARD_REQUIRED ON
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
            FOLDER "devices"
    )

    # Install
    install(TARGETS ${ARG_TARGET}
            RUNTIME DESTINATION bin
    )
endfunction()

# ============================================================================
# Configuration Summary
# ============================================================================

# Print configuration summary
# Usage: mfd_print_summary(
#            TARGET target_name
#            LAYOUTS layout_list
#            [RESOURCES_DIR dir]      # For MSVC
#        )
function(mfd_print_summary)
    cmake_parse_arguments(ARG "" "TARGET;DEVICE;RESOURCES_DIR" "LAYOUTS" ${ARGN})

    message(STATUS "========================================")
    message(STATUS "${ARG_TARGET} Configuration Summary:")
    message(STATUS "========================================")
    message(STATUS "  Platform: ${CMAKE_SYSTEM_NAME}")
    message(STATUS "  Processor: ${CMAKE_SYSTEM_PROCESSOR}")
    message(STATUS "  Build Type: Windows/MSVC")
    message(STATUS "  Platform Library: w32_canfly")
    message(STATUS "  Output Format: EXE")
    if(ARG_RESOURCES_DIR)
        message(STATUS "  Resources Dir: ${ARG_RESOURCES_DIR}")
    endif()
    message(STATUS "  XML Layouts: ${ARG_LAYOUTS}")
    message(STATUS "  C Standard: C17")
    message(STATUS "  Build Type: ${CMAKE_BUILD_TYPE}")
    message(STATUS "========================================")
endfunction()
