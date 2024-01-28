set(PROJECT_NAME photon)

################################################################################
# Source groups
################################################################################
set(no_group_source_files
    "../../libs/photon/canvas.c"
    "../../libs/photon/font.c"
    "../../libs/photon/neo.c"
    "../../libs/photon/photon.c"
    "../../libs/photon/photon.h"
    "../../libs/photon/photon_priv.h"
    "../../libs/photon/png_canvas.c"
    "../../libs/photon/screen.c"
    "../../libs/photon/touch.c"
    "../../libs/photon/window.c"
    "../../libs/photon/window.h"
)
source_group("" FILES ${no_group_source_files})

set(ALL_FILES
    ${no_group_source_files}
)

set(CMAKE_C_STANDARD 99)

################################################################################
# Target
################################################################################
add_library(${PROJECT_NAME} STATIC ${ALL_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "libs")
