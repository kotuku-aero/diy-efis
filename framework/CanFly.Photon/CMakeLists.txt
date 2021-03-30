cmake_minimum_required(VERSION 3.10)

# set the project name and enable the C# builder for
# canfly apps
project(CanFly.Photon VERSION 1.0.0 LANGUAGES CSharp)

# this is the build system that compiles the
# C# v1 files into a .pe assembly that is then packaged into
# an .app file

set(CMAKE_SYSTEM_NAME CanFly)

add_library(CanFly.Photon 
    Canvas.cs
    Color.cs
    Extent.cs
    Font.cs
    GdiObject.cs
    Pen.cs
    Photon.cs
    Point.cs
    PointArray.cs
    Rect.cs
    Screen.cs
    Widget.cs
)

find_library(CORLIB "mscorlib.pe" REQUIRED)
find_library(NEUTRON "CanFly.Neutron.pe" REQUIRED)

LIST(APPEND CANFLY_LIBS ${CORLIB})
LIST(APPEND CANFLY_LIBS ${NEUTRON})

set_target_properties(${PROJECT_NAME} PROPERTIES CANFLY_BUILD_REFERENCES ${CANFLY_LIBS})
