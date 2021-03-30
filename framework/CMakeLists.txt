cmake_minimum_required(VERSION 3.10)

# set the project name and enable the C# builder for
# canfly apps
project(diy-efis VERSION 1.0.0 LANGUAGES CSharp)

# this is the build system that compiles the
# C# v1 files into a .pe assembly that is then packaged into
# an .app file

set(CMAKE_SYSTEM_NAME CanFly)

# this is the core library.  It has special build flags.  Please
# read the comments in the Syscall.cs and Runtime.cs files.
add_subdirectory(CoreLibrary)

# C# support libraries
add_subdirectory(System.Collections)
add_subdirectory(System.Math)
add_subdirectory(System.Text)

# CanFly specific libraries
# Neutron kernel libraries
add_subdirectory(CanFly.Neutron)
# Photon 2D GDI library
add_subdirectory(CanFly.Photon)

# CanFly Widgets
add_subdirectory(CanFLy.Proton)

# DiyEfis application
add_executable(diy-efis
    Canfly.DiyEfis/Program.cs
    )

target_link_libraries(diy-efis
    "mscorelib"
    "System.Collections"
    "System.Math"
    "System.Text"
    "CanFly.Neutron"
    "CanFly.Photon"
    "CanFly.Proton"
)
