set(PROJECT_NAME mcp)

################################################################################
# Source groups
################################################################################
set(Source_Files
    "mcp.c"
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
