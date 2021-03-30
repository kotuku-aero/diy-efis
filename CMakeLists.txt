cmake_minimum_required(VERSION 3.10)

# set the project name and enable the C# builder for
# canfly apps
project(diy-efis)

# this is the build system that compiles the
# C# v1 files into a .pe assembly that is then packaged into
# a file

set(CMAKE_SYSTEM_NAME CanFly)

add_library(CoreLibrary
)