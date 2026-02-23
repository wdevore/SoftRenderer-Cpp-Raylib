cmake_minimum_required(VERSION 3.31.0)

set(DIR_SOURCES
    ZBuffer.cpp
    ArcBall.cpp
)

add_library(GraphicsLib STATIC
    ${DIR_SOURCES}
)

target_include_directories(GraphicsLib 
    PUBLIC .
)

target_link_libraries(GraphicsLib 
    PUBLIC MathLib
)