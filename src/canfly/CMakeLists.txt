cmake_minimum_required(VERSION 3.15)

project(canfly VERSION 1.0.0 LANGUAGES C ASM)

################################################################################
# Source files
################################################################################
set(CANFLY_SOURCES
        canfly_stubs.c
        newlib_stubs.c
        crt0.S
)

set(CANFLY_HEADERS
        ../../include/canfly.h
        ../../include/canfly_types.h
        ../../include/sys_canfly.h
        ../../include/vector.h
        ../../include/type_vector.h
)

################################################################################
# Create static library
################################################################################
add_library(canfly STATIC ${CANFLY_SOURCES} ${CANFLY_HEADERS})

# Set C standard
set_property(TARGET canfly PROPERTY C_STANDARD 11)
set_property(TARGET canfly PROPERTY C_STANDARD_REQUIRED ON)

################################################################################
# Include directories
################################################################################
target_include_directories(canfly PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

################################################################################
# Compiler definitions
################################################################################
if(WIN32)
    target_compile_definitions(canfly PRIVATE
        WIN32
        _CRT_SECURE_NO_WARNINGS
    )
    
    target_compile_definitions(canfly PRIVATE
        $<$<CONFIG:Debug>:_DEBUG;_CONSOLE>
        $<$<CONFIG:Release>:NDEBUG;_CONSOLE>
    )
endif()

################################################################################
# Compiler options
################################################################################
if(MSVC)
    # since the w32_canfly is not a DLL we link with the DLL runtime instead
    # of a static library
    target_compile_options(canfly PRIVATE
        /W3
        $<$<CONFIG:Debug>:/MDd>
        $<$<CONFIG:Release>:/MD /Ox /Oi>
    )
elseif(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(canfly PRIVATE
        -Wall
        -Wextra
    )
endif()

################################################################################
# Organize in IDE
################################################################################
set_target_properties(canfly PROPERTIES FOLDER "libs")
