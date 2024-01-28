set(PROJECT_NAME shp2db)

################################################################################
# Source groups
################################################################################
set(no_group_source_files
    "cargs.cpp"
    "cargs.h"
    "db_geometry.h"
    "dbfopen.c"
    "geometry_db.cpp"
    "main.cpp"
    "safileio.c"
    "sbnsearch.c"
    "shapefil.h"
    "shpopen.c"
    "shptree.c"
    "spatial_entity.cpp"
    "spatial_linestring.cpp"
    "spatial_point.cpp"
    "spatial_polygon.cpp"
    "spatial_polyline.cpp"
)
source_group("" FILES ${no_group_source_files})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 99)

add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
add_compile_options(/EHsc)

set(ALL_FILES
    ${no_group_source_files}
)

################################################################################
# Target
################################################################################
add_executable(${PROJECT_NAME} ${ALL_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "apps/dbtools")
