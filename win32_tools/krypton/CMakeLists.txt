set(PROJECT_NAME krypton)

################################################################################
# Source groups
################################################################################
set(no_group_source_files
    "argv.c"
    "argv.h"
    "comm_device.c"
    "direct_draw_screen.c"
    "flash_disk.c"
     "getdelim.c"
    "getdelim.h"
    "getopt.c"
    "getopt.h"
    "json.c"
    "json.h"
    "krypton.c"
    "krypton.h"
    "krypton_filesystem.c"
    "mfd.cli"
    "mfd_cli.c"
    "mfd_cli.h"
    "mfd_cli_impl.c"
    "slcan.c"
    "slcan.h"
    "splash_320_240.c"
    "win_flash_image.c"
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
