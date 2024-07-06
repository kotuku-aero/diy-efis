set(PROJECT_NAME muon)

################################################################################
# Source groups
################################################################################
set(no_group_source_files
    "../../libs/muon/cansend.c"
    "../../libs/muon/cli.c"
    "../../libs/muon/cli.h"
    "../../libs/muon/cli_enumerations.c"
    "../../libs/muon/cli_enumerations.h"
    "../../libs/muon/cli_fsm.c"
    "../../libs/muon/cli_line.c"
    "../../libs/muon/cli_publisher.c"
    "../../libs/muon/cli_regedit.c"
    "../../libs/muon/cli_string.c"
    "../../libs/muon/cli_token.c"
    "../../libs/muon/cli_token_tbl.c"
    "../../libs/muon/muon.h"
    "../../libs/muon/neutron.cli"
    "../../libs/muon/neutron_cli.c"
    "../../libs/muon/neutron_cli.h"
    "../../libs/muon/neutron_cli_impl.c"
    "../../libs/muon/neutron_cli_impl.h"
)
source_group("" FILES ${no_group_source_files})

set(ALL_FILES
    ${no_group_source_files}
)

################################################################################
# Target
################################################################################
add_library(${PROJECT_NAME} STATIC ${ALL_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "libs")

use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")
set(ROOT_NAMESPACE muon)

set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_GLOBAL_KEYWORD "Win32Proj"
)
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        INTERPROCEDURAL_OPTIMIZATION_RELEASE                    "TRUE"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "ARM")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        INTERPROCEDURAL_OPTIMIZATION_DEBUGNEWCONFIG             "TRUE"
        INTERPROCEDURAL_OPTIMIZATION_RELEASE                    "TRUE"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "ARM64")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        INTERPROCEDURAL_OPTIMIZATION_RELEASEDLL                 "TRUE"
        INTERPROCEDURAL_OPTIMIZATION_RELEASESTATICLIB           "TRUE"
        INTERPROCEDURAL_OPTIMIZATION_RELEASEMULTITHREADED       "TRUE"
        INTERPROCEDURAL_OPTIMIZATION_RELEASESINGLETHREADED      "TRUE"
        INTERPROCEDURAL_OPTIMIZATION_RELEASE                    "TRUE"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        INTERPROCEDURAL_OPTIMIZATION_RELEASEDLL                 "TRUE"
        INTERPROCEDURAL_OPTIMIZATION_RELEASESTATICLIB           "TRUE"
        INTERPROCEDURAL_OPTIMIZATION_RELEASEMULTITHREADED       "TRUE"
        INTERPROCEDURAL_OPTIMIZATION_RELEASESINGLETHREADED      "TRUE"
        INTERPROCEDURAL_OPTIMIZATION_RELEASE                    "TRUE"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        INTERPROCEDURAL_OPTIMIZATION_RELEASEDLL                 "TRUE"
        INTERPROCEDURAL_OPTIMIZATION_RELEASESTATICLIB           "TRUE"
        INTERPROCEDURAL_OPTIMIZATION_RELEASEMULTITHREADED       "TRUE"
        INTERPROCEDURAL_OPTIMIZATION_RELEASESINGLETHREADED      "TRUE"
        INTERPROCEDURAL_OPTIMIZATION_RELEASE                    "TRUE"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        INTERPROCEDURAL_OPTIMIZATION_RELEASEDLL                 "TRUE"
        INTERPROCEDURAL_OPTIMIZATION_RELEASESTATICLIB           "TRUE"
        INTERPROCEDURAL_OPTIMIZATION_RELEASEMULTITHREADED       "TRUE"
        INTERPROCEDURAL_OPTIMIZATION_RELEASESINGLETHREADED      "TRUE"
        INTERPROCEDURAL_OPTIMIZATION_RELEASE                    "TRUE"
    )
endif()
################################################################################
# MSVC runtime library
################################################################################
get_property(MSVC_RUNTIME_LIBRARY_DEFAULT TARGET ${PROJECT_NAME} PROPERTY MSVC_RUNTIME_LIBRARY)
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
    string(CONCAT "MSVC_RUNTIME_LIBRARY_STR"
        $<$<CONFIG:DebugDLL>:
            MultiThreadedDebug
        >
        $<$<CONFIG:DebugStaticLibwithMEMTRACE>:
            MultiThreadedDebug
        >
        $<$<CONFIG:DebugStaticLib>:
            MultiThreadedDebug
        >
        $<$<CONFIG:DebugMultithreaded>:
            MultiThreadedDebug
        >
        $<$<CONFIG:DebugSinglethreaded>:
            MultiThreadedDebug
        >
        $<$<CONFIG:Debug>:
            MultiThreadedDebug
        >
        $<$<CONFIG:DebugNewConfig>:
            MultiThreadedDebug
        >
        $<$<CONFIG:ReleaseDLL>:
            MultiThreadedDebug
        >
        $<$<CONFIG:ReleaseStaticLib>:
            MultiThreadedDebug
        >
        $<$<CONFIG:ReleaseMultithreaded>:
            MultiThreadedDebug
        >
        $<$<CONFIG:ReleaseSinglethreaded>:
            MultiThreadedDebug
        >
        $<$<CONFIG:Release>:
            MultiThreaded
        >
        $<$<NOT:$<OR:$<CONFIG:DebugDLL>,$<CONFIG:DebugStaticLibwithMEMTRACE>,$<CONFIG:DebugStaticLib>,$<CONFIG:DebugMultithreaded>,$<CONFIG:DebugSinglethreaded>,$<CONFIG:Debug>,$<CONFIG:DebugNewConfig>,$<CONFIG:ReleaseDLL>,$<CONFIG:ReleaseStaticLib>,$<CONFIG:ReleaseMultithreaded>,$<CONFIG:ReleaseSinglethreaded>,$<CONFIG:Release>>>:${MSVC_RUNTIME_LIBRARY_DEFAULT}>
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "ARM")
    string(CONCAT "MSVC_RUNTIME_LIBRARY_STR"
        $<$<CONFIG:DebugDLL>:
            MultiThreadedDebug
        >
        $<$<CONFIG:DebugStaticLibwithMEMTRACE>:
            MultiThreadedDebug
        >
        $<$<CONFIG:DebugStaticLib>:
            MultiThreadedDebug
        >
        $<$<CONFIG:DebugMultithreaded>:
            MultiThreadedDebug
        >
        $<$<CONFIG:DebugSinglethreaded>:
            MultiThreadedDebug
        >
        $<$<CONFIG:Debug>:
            MultiThreadedDebug
        >
        $<$<CONFIG:DebugNewConfig>:
            MultiThreaded
        >
        $<$<CONFIG:ReleaseDLL>:
            MultiThreadedDebug
        >
        $<$<CONFIG:ReleaseStaticLib>:
            MultiThreadedDebug
        >
        $<$<CONFIG:ReleaseMultithreaded>:
            MultiThreadedDebug
        >
        $<$<CONFIG:ReleaseSinglethreaded>:
            MultiThreadedDebug
        >
        $<$<CONFIG:Release>:
            MultiThreaded
        >
        $<$<NOT:$<OR:$<CONFIG:DebugDLL>,$<CONFIG:DebugStaticLibwithMEMTRACE>,$<CONFIG:DebugStaticLib>,$<CONFIG:DebugMultithreaded>,$<CONFIG:DebugSinglethreaded>,$<CONFIG:Debug>,$<CONFIG:DebugNewConfig>,$<CONFIG:ReleaseDLL>,$<CONFIG:ReleaseStaticLib>,$<CONFIG:ReleaseMultithreaded>,$<CONFIG:ReleaseSinglethreaded>,$<CONFIG:Release>>>:${MSVC_RUNTIME_LIBRARY_DEFAULT}>
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "ARM64")
    string(CONCAT "MSVC_RUNTIME_LIBRARY_STR"
        $<$<CONFIG:DebugDLL>:
            MultiThreadedDebug
        >
        $<$<CONFIG:DebugStaticLibwithMEMTRACE>:
            MultiThreadedDebug
        >
        $<$<CONFIG:DebugStaticLib>:
            MultiThreadedDebug
        >
        $<$<CONFIG:DebugMultithreaded>:
            MultiThreadedDebug
        >
        $<$<CONFIG:DebugSinglethreaded>:
            MultiThreadedDebug
        >
        $<$<CONFIG:Debug>:
            MultiThreadedDebug
        >
        $<$<CONFIG:DebugNewConfig>:
            MultiThreadedDebug
        >
        $<$<NOT:$<OR:$<CONFIG:DebugDLL>,$<CONFIG:DebugStaticLibwithMEMTRACE>,$<CONFIG:DebugStaticLib>,$<CONFIG:DebugMultithreaded>,$<CONFIG:DebugSinglethreaded>,$<CONFIG:Debug>,$<CONFIG:DebugNewConfig>>>:${MSVC_RUNTIME_LIBRARY_DEFAULT}>
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
    string(CONCAT "MSVC_RUNTIME_LIBRARY_STR"
        $<$<CONFIG:DebugDLL>:
            MultiThreadedDebug
        >
        $<$<CONFIG:DebugStaticLibwithMEMTRACE>:
            MultiThreadedDebug
        >
        $<$<CONFIG:DebugStaticLib>:
            MultiThreadedDebug
        >
        $<$<CONFIG:DebugMultithreaded>:
            MultiThreadedDebug
        >
        $<$<CONFIG:DebugSinglethreaded>:
            MultiThreadedDebug
        >
        $<$<CONFIG:Debug>:
            MultiThreadedDebug
        >
        $<$<CONFIG:DebugNewConfig>:
            MultiThreadedDebug
        >
        $<$<CONFIG:ReleaseDLL>:
            MultiThreaded
        >
        $<$<CONFIG:ReleaseStaticLib>:
            MultiThreaded
        >
        $<$<CONFIG:ReleaseMultithreaded>:
            MultiThreaded
        >
        $<$<CONFIG:ReleaseSinglethreaded>:
            MultiThreaded
        >
        $<$<CONFIG:Release>:
            MultiThreaded
        >
        $<$<NOT:$<OR:$<CONFIG:DebugDLL>,$<CONFIG:DebugStaticLibwithMEMTRACE>,$<CONFIG:DebugStaticLib>,$<CONFIG:DebugMultithreaded>,$<CONFIG:DebugSinglethreaded>,$<CONFIG:Debug>,$<CONFIG:DebugNewConfig>,$<CONFIG:ReleaseDLL>,$<CONFIG:ReleaseStaticLib>,$<CONFIG:ReleaseMultithreaded>,$<CONFIG:ReleaseSinglethreaded>,$<CONFIG:Release>>>:${MSVC_RUNTIME_LIBRARY_DEFAULT}>
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    string(CONCAT "MSVC_RUNTIME_LIBRARY_STR"
        $<$<CONFIG:DebugDLL>:
            MultiThreadedDebug
        >
        $<$<CONFIG:DebugStaticLibwithMEMTRACE>:
            MultiThreadedDebug
        >
        $<$<CONFIG:DebugStaticLib>:
            MultiThreadedDebug
        >
        $<$<CONFIG:DebugMultithreaded>:
            MultiThreadedDebug
        >
        $<$<CONFIG:DebugSinglethreaded>:
            MultiThreadedDebug
        >
        $<$<CONFIG:Debug>:
            MultiThreadedDebug
        >
        $<$<CONFIG:DebugNewConfig>:
            MultiThreadedDebug
        >
        $<$<NOT:$<OR:$<CONFIG:DebugDLL>,$<CONFIG:DebugStaticLibwithMEMTRACE>,$<CONFIG:DebugStaticLib>,$<CONFIG:DebugMultithreaded>,$<CONFIG:DebugSinglethreaded>,$<CONFIG:Debug>,$<CONFIG:DebugNewConfig>>>:${MSVC_RUNTIME_LIBRARY_DEFAULT}>
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    string(CONCAT "MSVC_RUNTIME_LIBRARY_STR"
        $<$<CONFIG:DebugDLL>:
            MultiThreadedDebug
        >
        $<$<CONFIG:DebugStaticLibwithMEMTRACE>:
            MultiThreadedDebug
        >
        $<$<CONFIG:DebugStaticLib>:
            MultiThreadedDebug
        >
        $<$<CONFIG:DebugMultithreaded>:
            MultiThreadedDebug
        >
        $<$<CONFIG:DebugSinglethreaded>:
            MultiThreadedDebug
        >
        $<$<CONFIG:Debug>:
            MultiThreadedDebug
        >
        $<$<CONFIG:DebugNewConfig>:
            MultiThreadedDebug
        >
        $<$<CONFIG:ReleaseDLL>:
            MultiThreaded
        >
        $<$<CONFIG:ReleaseStaticLib>:
            MultiThreaded
        >
        $<$<CONFIG:ReleaseMultithreaded>:
            MultiThreaded
        >
        $<$<CONFIG:ReleaseSinglethreaded>:
            MultiThreaded
        >
        $<$<CONFIG:Release>:
            MultiThreaded
        >
        $<$<NOT:$<OR:$<CONFIG:DebugDLL>,$<CONFIG:DebugStaticLibwithMEMTRACE>,$<CONFIG:DebugStaticLib>,$<CONFIG:DebugMultithreaded>,$<CONFIG:DebugSinglethreaded>,$<CONFIG:Debug>,$<CONFIG:DebugNewConfig>,$<CONFIG:ReleaseDLL>,$<CONFIG:ReleaseStaticLib>,$<CONFIG:ReleaseMultithreaded>,$<CONFIG:ReleaseSinglethreaded>,$<CONFIG:Release>>>:${MSVC_RUNTIME_LIBRARY_DEFAULT}>
    )
endif()
set_target_properties(${PROJECT_NAME} PROPERTIES MSVC_RUNTIME_LIBRARY ${MSVC_RUNTIME_LIBRARY_STR})

################################################################################
# Compile definitions
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:DebugDLL>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:DebugStaticLibwithMEMTRACE>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:DebugStaticLib>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:DebugMultithreaded>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:DebugSinglethreaded>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:Debug>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:DebugNewConfig>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:ReleaseDLL>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:ReleaseStaticLib>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:ReleaseMultithreaded>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:ReleaseSinglethreaded>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:Release>:"
            "NDEBUG"
        ">"
        "WIN32;"
        "_LIB;"
        "_MBCS"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "ARM")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:DebugDLL>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:DebugStaticLibwithMEMTRACE>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:DebugStaticLib>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:DebugMultithreaded>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:DebugSinglethreaded>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:Debug>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:DebugNewConfig>:"
            "NDEBUG"
        ">"
        "$<$<CONFIG:ReleaseDLL>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:ReleaseStaticLib>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:ReleaseMultithreaded>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:ReleaseSinglethreaded>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:Release>:"
            "NDEBUG"
        ">"
        "WIN32;"
        "_LIB;"
        "_MBCS"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "ARM64")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:DebugDLL>:"
            "_DEBUG;"
            "_CRT_SECURE_NO_WARNINGS;"
            "_MBCS"
        ">"
        "$<$<CONFIG:DebugStaticLibwithMEMTRACE>:"
            "_DEBUG;"
            "_CRT_SECURE_NO_WARNINGS;"
            "_MBCS"
        ">"
        "$<$<CONFIG:DebugStaticLib>:"
            "_DEBUG;"
            "_CRT_SECURE_NO_WARNINGS;"
            "_MBCS"
        ">"
        "$<$<CONFIG:DebugMultithreaded>:"
            "_DEBUG;"
            "_CRT_SECURE_NO_WARNINGS;"
            "_MBCS"
        ">"
        "$<$<CONFIG:DebugSinglethreaded>:"
            "_DEBUG;"
            "_CRT_SECURE_NO_WARNINGS;"
            "_MBCS"
        ">"
        "$<$<CONFIG:Debug>:"
            "_DEBUG;"
            "_CRT_SECURE_NO_WARNINGS;"
            "_MBCS"
        ">"
        "$<$<CONFIG:DebugNewConfig>:"
            "_DEBUG;"
            "_CRT_SECURE_NO_WARNINGS;"
            "_MBCS"
        ">"
        "$<$<CONFIG:ReleaseDLL>:"
            "NDEBUG;"
            "UNICODE;"
            "_UNICODE"
        ">"
        "$<$<CONFIG:ReleaseStaticLib>:"
            "NDEBUG;"
            "UNICODE;"
            "_UNICODE"
        ">"
        "$<$<CONFIG:ReleaseMultithreaded>:"
            "NDEBUG;"
            "UNICODE;"
            "_UNICODE"
        ">"
        "$<$<CONFIG:ReleaseSinglethreaded>:"
            "NDEBUG;"
            "UNICODE;"
            "_UNICODE"
        ">"
        "$<$<CONFIG:Release>:"
            "NDEBUG;"
            "UNICODE;"
            "_UNICODE"
        ">"
        "_LIB"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:DebugDLL>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:DebugStaticLibwithMEMTRACE>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:DebugStaticLib>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:DebugMultithreaded>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:DebugSinglethreaded>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:Debug>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:DebugNewConfig>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:ReleaseDLL>:"
            "NDEBUG"
        ">"
        "$<$<CONFIG:ReleaseStaticLib>:"
            "NDEBUG"
        ">"
        "$<$<CONFIG:ReleaseMultithreaded>:"
            "NDEBUG"
        ">"
        "$<$<CONFIG:ReleaseSinglethreaded>:"
            "NDEBUG"
        ">"
        "$<$<CONFIG:Release>:"
            "NDEBUG"
        ">"
        "WIN32;"
        "_LIB;"
        "_MBCS"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:DebugDLL>:"
            "_DEBUG;"
            "_CRT_SECURE_NO_WARNINGS;"
            "_MBCS"
        ">"
        "$<$<CONFIG:DebugStaticLibwithMEMTRACE>:"
            "_DEBUG;"
            "_CRT_SECURE_NO_WARNINGS;"
            "_MBCS"
        ">"
        "$<$<CONFIG:DebugStaticLib>:"
            "_DEBUG;"
            "_CRT_SECURE_NO_WARNINGS;"
            "_MBCS"
        ">"
        "$<$<CONFIG:DebugMultithreaded>:"
            "_DEBUG;"
            "_CRT_SECURE_NO_WARNINGS;"
            "_MBCS"
        ">"
        "$<$<CONFIG:DebugSinglethreaded>:"
            "_DEBUG;"
            "_CRT_SECURE_NO_WARNINGS;"
            "_MBCS"
        ">"
        "$<$<CONFIG:Debug>:"
            "_DEBUG;"
            "_CRT_SECURE_NO_WARNINGS;"
            "_MBCS"
        ">"
        "$<$<CONFIG:DebugNewConfig>:"
            "_DEBUG;"
            "_CRT_SECURE_NO_WARNINGS;"
            "_MBCS"
        ">"
        "$<$<CONFIG:ReleaseDLL>:"
            "NDEBUG;"
            "UNICODE;"
            "_UNICODE"
        ">"
        "$<$<CONFIG:ReleaseStaticLib>:"
            "NDEBUG;"
            "UNICODE;"
            "_UNICODE"
        ">"
        "$<$<CONFIG:ReleaseMultithreaded>:"
            "NDEBUG;"
            "UNICODE;"
            "_UNICODE"
        ">"
        "$<$<CONFIG:ReleaseSinglethreaded>:"
            "NDEBUG;"
            "UNICODE;"
            "_UNICODE"
        ">"
        "$<$<CONFIG:Release>:"
            "NDEBUG;"
            "UNICODE;"
            "_UNICODE"
        ">"
        "_LIB"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:DebugDLL>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:DebugStaticLibwithMEMTRACE>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:DebugStaticLib>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:DebugMultithreaded>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:DebugSinglethreaded>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:Debug>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:DebugNewConfig>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:ReleaseDLL>:"
            "NDEBUG"
        ">"
        "$<$<CONFIG:ReleaseStaticLib>:"
            "NDEBUG"
        ">"
        "$<$<CONFIG:ReleaseMultithreaded>:"
            "NDEBUG"
        ">"
        "$<$<CONFIG:ReleaseSinglethreaded>:"
            "NDEBUG"
        ">"
        "$<$<CONFIG:Release>:"
            "NDEBUG"
        ">"
        "WIN32;"
        "_LIB;"
        "_MBCS"
    )
endif()

################################################################################
# Compile and link options
################################################################################
if(MSVC)
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:DebugDLL>:
                /Od
            >
            $<$<CONFIG:DebugStaticLibwithMEMTRACE>:
                /Od
            >
            $<$<CONFIG:DebugStaticLib>:
                /Od
            >
            $<$<CONFIG:DebugMultithreaded>:
                /Od
            >
            $<$<CONFIG:DebugSinglethreaded>:
                /Od
            >
            $<$<CONFIG:Debug>:
                /Od
            >
            $<$<CONFIG:DebugNewConfig>:
                /Od
            >
            $<$<CONFIG:ReleaseDLL>:
                /Od
            >
            $<$<CONFIG:ReleaseStaticLib>:
                /Od
            >
            $<$<CONFIG:ReleaseMultithreaded>:
                /Od
            >
            $<$<CONFIG:ReleaseSinglethreaded>:
                /Od
            >
            $<$<CONFIG:Release>:
                /O2;
                /Oi;
                /Gy
            >
            /W3;
            ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
            /GR-;
            /Y-
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "ARM")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:DebugDLL>:
                /Od
            >
            $<$<CONFIG:DebugStaticLibwithMEMTRACE>:
                /Od
            >
            $<$<CONFIG:DebugStaticLib>:
                /Od
            >
            $<$<CONFIG:DebugMultithreaded>:
                /Od
            >
            $<$<CONFIG:DebugSinglethreaded>:
                /Od
            >
            $<$<CONFIG:Debug>:
                /Od
            >
            $<$<CONFIG:DebugNewConfig>:
                /O2;
                /Oi;
                /Gy
            >
            $<$<CONFIG:ReleaseDLL>:
                /Od
            >
            $<$<CONFIG:ReleaseStaticLib>:
                /Od
            >
            $<$<CONFIG:ReleaseMultithreaded>:
                /Od
            >
            $<$<CONFIG:ReleaseSinglethreaded>:
                /Od
            >
            $<$<CONFIG:Release>:
                /O2;
                /Oi;
                /Gy
            >
            /W3;
            ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
            /GR-;
            /Y-
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "ARM64")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:DebugDLL>:
                /Od;
                /GS
            >
            $<$<CONFIG:DebugStaticLibwithMEMTRACE>:
                /Od;
                /GS
            >
            $<$<CONFIG:DebugStaticLib>:
                /Od;
                /GS
            >
            $<$<CONFIG:DebugMultithreaded>:
                /Od;
                /GS
            >
            $<$<CONFIG:DebugSinglethreaded>:
                /Od;
                /GS
            >
            $<$<CONFIG:Debug>:
                /Od;
                /GS
            >
            $<$<CONFIG:DebugNewConfig>:
                /Od;
                /GS
            >
            $<$<CONFIG:ReleaseDLL>:
                /O2;
                /Oi;
                /Gy
            >
            $<$<CONFIG:ReleaseStaticLib>:
                /O2;
                /Oi;
                /Gy
            >
            $<$<CONFIG:ReleaseMultithreaded>:
                /O2;
                /Oi;
                /Gy
            >
            $<$<CONFIG:ReleaseSinglethreaded>:
                /O2;
                /Oi;
                /Gy
            >
            $<$<CONFIG:Release>:
                /O2;
                /Oi;
                /Gy
            >
            /W3;
            ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
            ${DEFAULT_CXX_EXCEPTION_HANDLING};
            /Y-
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:DebugDLL>:
                /Od
            >
            $<$<CONFIG:DebugStaticLibwithMEMTRACE>:
                /Od
            >
            $<$<CONFIG:DebugStaticLib>:
                /Od
            >
            $<$<CONFIG:DebugMultithreaded>:
                /Od
            >
            $<$<CONFIG:DebugSinglethreaded>:
                /Od
            >
            $<$<CONFIG:Debug>:
                /Od
            >
            $<$<CONFIG:DebugNewConfig>:
                /Od
            >
            $<$<CONFIG:ReleaseDLL>:
                /O2;
                /Oi;
                /Gy
            >
            $<$<CONFIG:ReleaseStaticLib>:
                /O2;
                /Oi;
                /Gy
            >
            $<$<CONFIG:ReleaseMultithreaded>:
                /O2;
                /Oi;
                /Gy
            >
            $<$<CONFIG:ReleaseSinglethreaded>:
                /O2;
                /Oi;
                /Gy
            >
            $<$<CONFIG:Release>:
                /O2;
                /Oi;
                /Gy
            >
            /W3;
            ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
            /GR-;
            /Y-
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:DebugDLL>:
                /Od;
                /GS
            >
            $<$<CONFIG:DebugStaticLibwithMEMTRACE>:
                /Od;
                /GS
            >
            $<$<CONFIG:DebugStaticLib>:
                /Od;
                /GS
            >
            $<$<CONFIG:DebugMultithreaded>:
                /Od;
                /GS
            >
            $<$<CONFIG:DebugSinglethreaded>:
                /Od;
                /GS
            >
            $<$<CONFIG:Debug>:
                /Od;
                /GS
            >
            $<$<CONFIG:DebugNewConfig>:
                /Od;
                /GS
            >
            $<$<CONFIG:ReleaseDLL>:
                /O2;
                /Oi;
                /Gy
            >
            $<$<CONFIG:ReleaseStaticLib>:
                /O2;
                /Oi;
                /Gy
            >
            $<$<CONFIG:ReleaseMultithreaded>:
                /O2;
                /Oi;
                /Gy
            >
            $<$<CONFIG:ReleaseSinglethreaded>:
                /O2;
                /Oi;
                /Gy
            >
            $<$<CONFIG:Release>:
                /O2;
                /Oi;
                /Gy
            >
            /W3;
            ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
            ${DEFAULT_CXX_EXCEPTION_HANDLING};
            /Y-
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:DebugDLL>:
                /Od
            >
            $<$<CONFIG:DebugStaticLibwithMEMTRACE>:
                /Od
            >
            $<$<CONFIG:DebugStaticLib>:
                /Od
            >
            $<$<CONFIG:DebugMultithreaded>:
                /Od
            >
            $<$<CONFIG:DebugSinglethreaded>:
                /Od
            >
            $<$<CONFIG:Debug>:
                /Od
            >
            $<$<CONFIG:DebugNewConfig>:
                /Od
            >
            $<$<CONFIG:ReleaseDLL>:
                /O2;
                /Oi;
                /Gy
            >
            $<$<CONFIG:ReleaseStaticLib>:
                /O2;
                /Oi;
                /Gy
            >
            $<$<CONFIG:ReleaseMultithreaded>:
                /O2;
                /Oi;
                /Gy
            >
            $<$<CONFIG:ReleaseSinglethreaded>:
                /O2;
                /Oi;
                /Gy
            >
            $<$<CONFIG:Release>:
                /O2;
                /Oi;
                /Gy
            >
            /W3;
            ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
            /GR-;
            /Y-
        )
    endif()
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
        target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Release>:
                /OPT:REF;
                /OPT:ICF
            >
            /SUBSYSTEM:WINDOWS
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "ARM")
        target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:DebugNewConfig>:
                /OPT:REF;
                /OPT:ICF
            >
            $<$<CONFIG:Release>:
                /OPT:REF;
                /OPT:ICF
            >
            /SUBSYSTEM:WINDOWS
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "ARM64")
        target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:ReleaseDLL>:
                /OPT:REF;
                /OPT:ICF
            >
            $<$<CONFIG:ReleaseStaticLib>:
                /OPT:REF;
                /OPT:ICF
            >
            $<$<CONFIG:ReleaseMultithreaded>:
                /OPT:REF;
                /OPT:ICF
            >
            $<$<CONFIG:ReleaseSinglethreaded>:
                /OPT:REF;
                /OPT:ICF
            >
            $<$<CONFIG:Release>:
                /OPT:REF;
                /OPT:ICF
            >
            /SUBSYSTEM:WINDOWS
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
        target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:ReleaseDLL>:
                /OPT:REF;
                /OPT:ICF
            >
            $<$<CONFIG:ReleaseStaticLib>:
                /OPT:REF;
                /OPT:ICF
            >
            $<$<CONFIG:ReleaseMultithreaded>:
                /OPT:REF;
                /OPT:ICF
            >
            $<$<CONFIG:ReleaseSinglethreaded>:
                /OPT:REF;
                /OPT:ICF
            >
            $<$<CONFIG:Release>:
                /OPT:REF;
                /OPT:ICF
            >
            /SUBSYSTEM:WINDOWS
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:ReleaseDLL>:
                /OPT:REF;
                /OPT:ICF
            >
            $<$<CONFIG:ReleaseStaticLib>:
                /OPT:REF;
                /OPT:ICF
            >
            $<$<CONFIG:ReleaseMultithreaded>:
                /OPT:REF;
                /OPT:ICF
            >
            $<$<CONFIG:ReleaseSinglethreaded>:
                /OPT:REF;
                /OPT:ICF
            >
            $<$<CONFIG:Release>:
                /OPT:REF;
                /OPT:ICF
            >
            /SUBSYSTEM:WINDOWS
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
        target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:ReleaseDLL>:
                /OPT:REF;
                /OPT:ICF
            >
            $<$<CONFIG:ReleaseStaticLib>:
                /OPT:REF;
                /OPT:ICF
            >
            $<$<CONFIG:ReleaseMultithreaded>:
                /OPT:REF;
                /OPT:ICF
            >
            $<$<CONFIG:ReleaseSinglethreaded>:
                /OPT:REF;
                /OPT:ICF
            >
            $<$<CONFIG:Release>:
                /OPT:REF;
                /OPT:ICF
            >
            /SUBSYSTEM:WINDOWS
        )
    endif()
endif()

