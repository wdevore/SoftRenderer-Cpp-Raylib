cmake_minimum_required(VERSION 3.31.0)
project(SoftRenderer)

set(CMAKE_CXX_STANDARD 17)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

set(DIR_SOURCES
    Canvas.cpp
)

# Add library as a ".a" file
add_library(DrawingLib STATIC
    ${DIR_SOURCES}
)

target_include_directories(DrawingLib
    PUBLIC .
)

# Although raylib is fetched in the root CMake file, the DrawingLib library 
# target defined in the subdirectory doesn't know about it yet. In CMake, 
# you must explicitly link libraries to targets so they inherit the necessary 
# include paths (where raylib.h lives).
target_link_libraries(DrawingLib raylib)