set(PROJECT_NAME neutron)

################################################################################
# Source groups
################################################################################
set(no_group_source_files
    "../../libs/neutron/canfly.c"
    "../../libs/neutron/crc.c"
    "../../libs/neutron/decompress.c"
    "../../libs/neutron/deque.c"
    "../../libs/neutron/filesystem.c"
    "../../libs/neutron/flash_filesystem.c"
    "../../libs/neutron/manifest.c"
    "../../libs/neutron/neutron-proto.c"
    "../../libs/neutron/neutron-service.pb-c.c"
    "../../libs/neutron/neutron-service.pb-c.h"
    "../../libs/neutron/neutron.c"
    "../../libs/neutron/neutron.h"
    "../../libs/neutron/neutron_services.c"
    "../../libs/neutron/pipe.c"
    "../../libs/neutron/protobuf-c.c"
    "../../libs/neutron/protobuf-service.c"
    "../../libs/neutron/protobuf-service.h"
    "../../libs/neutron/registry.c"
    "../../libs/neutron/registry.h"
    "../../libs/neutron/registry_filesystem.c"
    "../../libs/neutron/regstream.c"
    "../../libs/neutron/stream.c"
    "../../libs/neutron/stream.h"
    "../../libs/neutron/stream_printf.c"
    "../../libs/neutron/string_to_enum.c"
    "../../libs/neutron/strstream.c"
    "../../libs/neutron/type_vector.h"
    "../../libs/neutron/variant.c"
    "../../libs/neutron/vector.h"
    "../../resources/CanFlyID.def"
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
