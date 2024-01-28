set(PROJECT_NAME graviton)

################################################################################
# Source groups
################################################################################
set(no_group_source_files
    "../../libs/graviton/aircraft.c"
    "../../libs/graviton/aircraft.h"
    "../../libs/graviton/airspace.c"
    "../../libs/graviton/cities.c"
    "../../libs/graviton/clipper.c"
    "../../libs/graviton/clipper.h"
    "../../libs/graviton/coastline.c"
    "../../libs/graviton/contours.c"
    "../../libs/graviton/fis.h"
    "../../libs/graviton/geography.h"
    "../../libs/graviton/layer.c"
    "../../libs/graviton/layer.h"
    "../../libs/graviton/obstacles.c"
    "../../libs/graviton/schema.h"
    "../../libs/graviton/surface_water.c"
    "../../libs/graviton/terrain.c"
    "../../libs/graviton/transport.c"
    "../../libs/graviton/viewport.c"
    "../../libs/graviton/viewport.h"
)
source_group("" FILES ${no_group_source_files})

set(CMAKE_C_STANDARD 99)

set(ALL_FILES
    ${no_group_source_files}
)

################################################################################
# Target
################################################################################
add_library(${PROJECT_NAME} STATIC ${ALL_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "libs")
