set(PROJECT_NAME msh)

################################################################################
# Source groups
################################################################################
set(Source_Files
    "msh.c"
)
source_group("Source Files" FILES ${Source_Files})

set(ALL_FILES
    ${Source_Files}
)

################################################################################
# Target
################################################################################
add_executable(${PROJECT_NAME} ${ALL_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "apps")
