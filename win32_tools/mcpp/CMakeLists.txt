set(PROJECT_NAME mcpp)

################################################################################
# Source groups
################################################################################
set(no_group_source_files
    "configed.H"
    "directive.c"
    "eval.c"
    "expand.c"
    "internal.H"
    "main.c"
    "mbchar.c"
    "mcpp_lib.def"
    "mcpp_lib.h"
    "mcpp_out.h"
    "noconfig.H"
    "preproc.c"
    "set_mcpp.sh"
    "support.c"
    "system.c"
    "system.H"
)
source_group("" FILES ${no_group_source_files})

set(ALL_FILES
    ${no_group_source_files}
)

################################################################################
# Target
################################################################################
add_executable(${PROJECT_NAME} ${ALL_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "apps")
