#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

# native code directory
set(BASE_PATH_FOR_THIS_MODULE ${BASE_PATH_FOR_CLASS_LIBRARIES_MODULES}/mscorlib)


# set include directories
list(APPEND mscorlib_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/CLR/Core)
list(APPEND mscorlib_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/CLR/Include)
list(APPEND mscorlib_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/HAL/Include)
list(APPEND mscorlib_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/PAL/Include)
list(APPEND mscorlib_INCLUDE_DIRS ${BASE_PATH_FOR_THIS_MODULE})
list(APPEND mscorlib_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/mscorlib)

# source files
set(mscorlib_SRCS

    corlib_native.cpp


    corlib_native_CanFly_Runtime.cpp
    corlib_native_CanFly_Syscall.cpp

)

foreach(SRC_FILE ${mscorlib_SRCS})
    set(mscorlib_SRC_FILE SRC_FILE-NOTFOUND)
    find_file(mscorlib_SRC_FILE ${SRC_FILE}
        PATHS
	        ${BASE_PATH_FOR_THIS_MODULE}
	        ${TARGET_BASE_LOCATION}
            ${PROJECT_SOURCE_DIR}/src/mscorlib

	    CMAKE_FIND_ROOT_PATH_BOTH
    )
    # message("${SRC_FILE} >> ${mscorlib_SRC_FILE}") # debug helper
    list(APPEND mscorlib_SOURCES ${mscorlib_SRC_FILE})
endforeach()

include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(mscorlib DEFAULT_MSG mscorlib_INCLUDE_DIRS mscorlib_SOURCES)
