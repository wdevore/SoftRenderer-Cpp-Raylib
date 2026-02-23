cmake_minimum_required(VERSION 3.31.0)

set(DIR_SOURCES
    Object3D.cpp
)

add_library(GeometryLib STATIC
    ${DIR_SOURCES}
)

target_include_directories(GeometryLib 
    PUBLIC .
)

target_link_libraries(GeometryLib 
    PUBLIC MathLib
)