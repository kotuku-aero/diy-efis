set(PROJECT_NAME fixedpoint)

################################################################################
# Source groups
################################################################################
set(no_group_source_files
    "../../libs/fixedpoint/fix16.c"
    "../../libs/fixedpoint/fix16.h"
    "../../libs/fixedpoint/fix16.hpp"
    "../../libs/fixedpoint/fix16_exp.c"
    "../../libs/fixedpoint/fix16_fft.c"
    "../../libs/fixedpoint/fix16_sqrt.c"
    "../../libs/fixedpoint/fix16_str.c"
    "../../libs/fixedpoint/fix16_trig.c"
    "../../libs/fixedpoint/fix16_trig_sin_lut.h"
    "../../libs/fixedpoint/fixmath.h"
    "../../libs/fixedpoint/fract32.c"
    "../../libs/fixedpoint/fract32.h"
    "../../libs/fixedpoint/int64.h"
    "../../libs/fixedpoint/uint32.c"
    "../../libs/fixedpoint/uint32.h"
)
source_group("" FILES ${no_group_source_files})

set(ALL_FILES
    ${no_group_source_files}
)

################################################################################
# Target
################################################################################
add_library(${PROJECT_NAME} STATIC ${ALL_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "libs")
