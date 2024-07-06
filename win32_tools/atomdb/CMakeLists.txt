set(PROJECT_NAME atomdb)

################################################################################
# Source groups
################################################################################
set(no_group_source_files
    "../../libs/atomdb/atomdb_schema.h"
    "../../libs/atomdb/db.c"
    "../../libs/atomdb/db.h"
    "../../libs/atomdb/db_priv.h"
    "../../libs/atomdb/index_db.c"
    "../../libs/atomdb/spatial.c"
    "../../libs/atomdb/spatial.h"
    "../../libs/atomdb/spatial_db.c"
    "../../libs/atomdb/terrain_db.c"
)
source_group("" FILES ${no_group_source_files})

set(CMAKE_C_STANDARD 99)

add_compile_definitions(_CRT_SECURE_NO_WARNINGS)

set(ALL_FILES
    ${no_group_source_files}
)

################################################################################
# Target
################################################################################
add_library(${PROJECT_NAME} STATIC ${ALL_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "libs")
