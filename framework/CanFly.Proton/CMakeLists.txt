cmake_minimum_required(VERSION 3.10)

# set the project name and enable the C# builder for
# canfly apps
project(CanFly.Proton VERSION 1.0.0 LANGUAGES CSharp)

# this is the build system that compiles the
# C# v1 files into a .pe assembly that is then packaged into
# an .app file

set(CMAKE_SYSTEM_NAME CanFly)

add_library(CanFly.Proton
    AirspeedWidget.cs
    AltitudeWidget.cs
    AnnunciatorWidget.cs
    AttitudeWidget.cs
    GaugeWidget.cs
    GpsWidget.cs
    HSIWidget.cs
    Keys.cs
    LayoutWidget.cs
    Menu.cs
    MenuItem.cs
    MenuItemCancel.cs
    MenuItemChecklist.cs
    MenuItemEdit.cs
    MenuItemEnter.cs
    MenuItemEvent.cs
    MenuItemItem.cs
    MenuItemMenu.cs
    MenuItemPopup.cs
    Pens.cs
)

find_library(CORLIB "mscorlib.pe" REQUIRED)
find_library(NEUTRON "CanFly.Neutron.pe" REQUIRED)
find_library(PHOTON "CanFly.Photon.pe" REQUIRED)

LIST(APPEND CANFLY_LIBS ${CORLIB})
LIST(APPEND CANFLY_LIBS ${NEUTRON})
LIST(APPEND CANFLY_LIBS ${PHOTON})

set_target_properties(${PROJECT_NAME} PROPERTIES CANFLY_BUILD_REFERENCES ${CANFLY_LIBS})
