set(PROJECT_NAME proton)

################################################################################
# Source groups
################################################################################
set(no_group_source_files
    "../../libs/proton/airspeed_window.c"
    "../../libs/proton/airspeed_window.h"
    "../../libs/proton/altitude_window.c"
    "../../libs/proton/altitude_window.h"
    "../../libs/proton/annunciator_window.c"
    "../../libs/proton/annunciator_window.h"
    "../../libs/proton/attitude_window.c"
    "../../libs/proton/attitude_window.h"
    "../../libs/proton/au_nz_map_renderer.c"
    "../../libs/proton/carousel_window.c"
    "../../libs/proton/carousel_window.h"
    "../../libs/proton/comm_window.c"
    "../../libs/proton/comm_window.h"
    "../../libs/proton/converters.c"
    "../../libs/proton/converters.h"
    "../../libs/proton/edutemp_window.c"
    "../../libs/proton/edutemp_window.h"
    "../../libs/proton/gauge_window.c"
    "../../libs/proton/gauge_window.h"
    "../../libs/proton/gps_window.c"
    "../../libs/proton/gps_window.h"
    "../../libs/proton/hsi_window.c"
    "../../libs/proton/hsi_window.h"
    "../../libs/proton/map_window.c"
    "../../libs/proton/map_window.h"
    "../../libs/proton/marquee_window.c"
    "../../libs/proton/marquee_window.h"
    "../../libs/proton/menu_window.c"
    "../../libs/proton/menu_window.h"
    "../../libs/proton/pancake_indicator_window.c"
    "../../libs/proton/pancake_indicator_window.h"
    "../../libs/proton/proton.c"
    "../../libs/proton/proton.cli"
    "../../libs/proton/proton.h"
    "../../libs/proton/regex.c"
    "../../libs/proton/regex.h"
    "../../libs/proton/softkey_window.c"
    "../../libs/proton/softkey_window.h"
    "../../libs/proton/widget.c"
    "../../libs/proton/widget.h"
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
