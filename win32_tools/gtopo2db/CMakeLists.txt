set(PROJECT_NAME gtopo2db)

################################################################################
# Source groups
################################################################################
set(no_group_source_files
    "cargs.cpp"
    "cargs.h"
    "main.cpp"
)
source_group("" FILES ${no_group_source_files})

set(ALL_FILES
    ${no_group_source_files}
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 99)

add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
add_compile_options(/EHsc)
################################################################################
# Target
################################################################################
add_executable(${PROJECT_NAME} ${ALL_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "apps/dbtools")
