cmake_minimum_required(VERSION 3.10)

# set the project name and enable the C# builder for
# canfly apps
project(mscorlib VERSION 1.0.0 LANGUAGES CSharp)

# this is the build system that compiles the
# C# v1 files into a .pe assembly that is then packaged into
# an .app file

set(CMAKE_SYSTEM_NAME CanFly)

add_library(mscorlib 
    Math.cs
)

# set the property to flag building the core library

set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_DATA_VERSION "v1.0.0")

find_library(CORLIB "mscorlib.pe" REQUIRED)

set_target_properties(${PROJECT_NAME} PROPERTIES CANFLY_BUILD_REFERENCES ${CORLIB})
