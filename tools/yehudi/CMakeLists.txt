cmake_minimum_required(VERSION 3.15)

project(yehudi VERSION 1.0.0 LANGUAGES CXX)

# ============================================================================
# C++ Standard
# ============================================================================
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ============================================================================
# Source Files
# ============================================================================
set(YEHUDI_SOURCES
        airspeed_widget.cpp
        alert_dialog.cpp
        altitude_widget.cpp
        annunciator_widgets.cpp
        application.cpp
        attitude_widget.cpp
        button_widget.cpp
        checkbox_widget.cpp
        checklist_action.cpp
        code_generator.cpp
        combobox_widget.cpp
        compass_widget.cpp
        definitions_generator.cpp
        dialog_widget.cpp
        edit_widget.cpp
        edutemps_widget.cpp
        event_action.cpp
        gauge_widget.cpp
        header_generator.cpp
        hsi_widget.cpp
        key_maps.cpp
        label_widget.cpp
        listbox_widget.cpp
        map_widget.cpp
        marquee_widget.cpp
        menus.cpp
        panel_widget.cpp
        popup_action.cpp
        pugixml.cpp
        spinedit_action.cpp
        tab_widget.cpp
        variant.cpp
        widget.cpp
        xml_parser.cpp
        yehudi.cpp
        pancake_widget.cpp
        groupbox_widget.cpp
        generate_panel_or_groupbox_widget_content.cpp
        data_bound_widget.cpp
)

# ============================================================================
# pugixml Dependency
# ============================================================================
# pugixml is a lightweight XML parser - we'll use it as a single header
# Download from: https://github.com/zeux/pugixml
# Place pugixml.hpp and pugixml.cpp in this directory

# Check if pugixml exists
if (NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/pugixml.cpp")
    message(WARNING "pugixml.cpp not found. Download from https://github.com/zeux/pugixml")
    message(WARNING "Place pugixml.hpp and pugixml.cpp in tools/yehudi/")
endif ()

list(APPEND YEHUDI_SOURCES pugixml.cpp)

# ============================================================================
# Executable Target
# ============================================================================
add_executable(yehudi ${YEHUDI_SOURCES})

# ============================================================================
# Include Directories
# ============================================================================
target_include_directories(yehudi PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# ============================================================================
# Compiler Settings
# ============================================================================
if (MSVC)
    target_compile_options(yehudi PRIVATE
            /W3
            /permissive-
    )

    target_compile_definitions(yehudi PRIVATE
            _CRT_SECURE_NO_WARNINGS
            WIN32
    )

    # Static runtime
    if (CMAKE_BUILD_TYPE MATCHES Debug)
        set_property(TARGET yehudi PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDebug")
    else ()
        set_property(TARGET yehudi PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded")
    endif ()
else ()
    # GCC/Clang
    target_compile_options(yehudi PRIVATE
            -Wall
            -Wextra
            -Wpedantic
            -Wno-unused-parameter
    )

    # Link filesystem library for GCC < 9
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.0)
        target_link_libraries(yehudi PRIVATE stdc++fs)
    endif ()
endif ()

# ============================================================================
# Output Directory
# ============================================================================
set_target_properties(yehudi PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# ============================================================================
# Install
# ============================================================================
install(TARGETS yehudi
        RUNTIME DESTINATION bin
)

# ============================================================================
# Configuration Summary
# ============================================================================
message(STATUS "========================================")
message(STATUS "Yehudi Configuration:")
message(STATUS "========================================")
message(STATUS "  C++ Standard: C++${CMAKE_CXX_STANDARD}")
message(STATUS "  Compiler: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "  Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  Output: ${CMAKE_BINARY_DIR}/bin/yehudi")
message(STATUS "========================================")

################################################################################
# Organize in IDE
################################################################################
set_target_properties(yehudi PROPERTIES FOLDER "tools")
