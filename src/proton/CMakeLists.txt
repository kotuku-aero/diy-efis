cmake_minimum_required(VERSION 3.15)

project(proton VERSION 1.0.0 LANGUAGES C)

################################################################################
# Source files
################################################################################
set(PROTON_SOURCES
        src/instruments/airspeed_gauge.c
        src/instruments/airspeed_widget.c
        src/instruments/altitude_widget.c
        src/instruments/annunciator_widget.c
        src/instruments/apmode_annunciator.c
        src/instruments/attitude_widget.c
        src/instruments/auto_annunciator.c
        src/dialogs/button_widget.c
        src/dialogs/checkbox_widget.c
        src/dialogs/combobox_widget.c
        src/dialogs/bound_widget.c
        src/instruments/comm_widget.c
        src/instruments/compass_widget.c
        src/converters.c
        src/instruments/datetime_annunciator.c
        src/dialogs/edit_widget.c
        src/instruments/edutemps_widget.c
        src/instruments/gauge_widget.c
        src/dialogs/groupbox_widget.c
        src/instruments/hobbs_annunciator.c
        src/instruments/hp_annunciator.c
        src/instruments/hsi_widget.c
        src/instruments/kmag_annunciator.c
        src/dialogs/label_widget.c
        src/instruments/map_aircraft.c
        src/instruments/map_widget.c
        src/instruments/marquee_widget.c
        src/menu_window.c
        src/neo_12.c
        src/neo_12_bold.c
        src/neo_15.c
        src/neo_18.c
        src/neo_9.c
        src/neo_9_bold.c
        src/instruments/pancake_indicator_widget.c
        src/proton.c
        src/regex.c
        src/instruments/softkey_widget.c
        src/spin_edit.c
        src/strftm.c
        src/dialogs/tab_widget.c
        src/instruments/text_annunciator.c
        src/instruments/waypoint_annunciator.c
        src/widget.c
        src/dialogs/listbox_widget.c
        src/dialogs/dialog_theme.c
        src/dialogs/dialog_widget.c
        src/dialogs/data_bound_widget.c
)

set(PROTON_HEADERS
        include/airspeed_gauge.h
        include/airspeed_widget.h
        include/altitude_widget.h
        include/annunciator_widget.h
        include/attitude_widget.h
        include/button_widget.h
        include/checkbox_widget.h
        include/combobox_widget.h
        include/bound_widget.h
        include/comm_widget.h
        include/compass_widget.h
        include/converters.h
        include/edit_widget.h
        include/edutemps_widget.h
        include/gauge_widget.h
        include/groupbox_widget.h
        include/hsi_widget.h
        include/label_widget.h
        include/map_widget.h
        include/marquee_widget.h
        include/menu_window.h
        include/pancake_indicator_widget.h
        include/proton.h
        include/regex.h
        include/softkey_widget.h
        include/tab_widget.h
        include/widget.h
        include/panel_widget.h
        include/listbox_widget.h
        src/dialogs/panel_widget.c
        include/dialog_widget.h
        include/dialog_theme.h
        dialog_widgets.h
        instrument_widgets.h
)

################################################################################
# Create static library
################################################################################
add_library(proton STATIC ${PROTON_SOURCES} ${PROTON_HEADERS})

# Set C standard
set_property(TARGET proton PROPERTY C_STANDARD 99)
set_property(TARGET proton PROPERTY C_STANDARD_REQUIRED ON)

################################################################################
# Include directories
################################################################################
target_include_directories(proton PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Additional include directories for neutron and graviton
target_include_directories(proton PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
)

################################################################################
# Compiler definitions
################################################################################
if(WIN32)
    target_compile_definitions(proton PRIVATE
        WIN32
        _CRT_SECURE_NO_WARNINGS
    )
endif()

################################################################################
# Compiler options
################################################################################
if(MSVC)
    target_compile_options(proton PRIVATE
        $<$<CONFIG:Debug>:/MDd>
        $<$<CONFIG:Release>:/MD>
    )
elseif(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(proton PRIVATE
        -Wall
        -Wextra
    )
endif()

################################################################################
# Organize in IDE
################################################################################
set_target_properties(proton PROPERTIES FOLDER "libs")
