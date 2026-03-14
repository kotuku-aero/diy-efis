cmake_minimum_required(VERSION 3.15)

project(mfdlib VERSION 1.0.0 LANGUAGES C)

################################################################################
# Source files
################################################################################
set(MFDLIB_SOURCES
    menu_handlers.c
    mfd.c
    event_handlers.c
        variant.c
        canfly.c
        canfly_user.c
)

set(MFDLIB_HEADERS
    menu_handlers.h
    mfd.h
    event_handlers.h
)

################################################################################
# Create static library
################################################################################
add_library(mfdlib STATIC ${MFDLIB_SOURCES} ${MFDLIB_HEADERS})

# Set C standard
set_property(TARGET mfdlib PROPERTY C_STANDARD 11)
set_property(TARGET mfdlib PROPERTY C_STANDARD_REQUIRED ON)

################################################################################
# Include directories
################################################################################
target_include_directories(mfdlib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

################################################################################
# Compiler definitions
################################################################################
if(WIN32)
    target_compile_definitions(mfdlib PRIVATE
        WIN32
        _CRT_SECURE_NO_WARNINGS
    )
    
    target_compile_definitions(mfdlib PRIVATE
        $<$<CONFIG:Debug>:_DEBUG;_CONSOLE>
        $<$<CONFIG:Release>:NDEBUG;_CONSOLE>
    )
endif()

################################################################################
# Compiler options
################################################################################
if(MSVC)
    target_compile_options(mfdlib PRIVATE
        /W3
        $<$<CONFIG:Debug>:/MDd>
        $<$<CONFIG:Release>:/MD /Ox /Oi>
    )
elseif(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(mfdlib PRIVATE
        -Wall
        -Wextra
    )
endif()

################################################################################
# Organize in IDE
################################################################################
set_target_properties(mfdlib PROPERTIES FOLDER "libs")
