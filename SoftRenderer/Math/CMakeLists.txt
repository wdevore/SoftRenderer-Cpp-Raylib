cmake_minimum_required(VERSION 3.31.0)

set(DIR_SOURCES
    AxisAngle4f.cpp
    Vector3f.cpp
    Point3f.cpp
    Matrix4f.cpp
    Vertex3f.cpp
    Quat4f.cpp
)

add_library(MathLib STATIC
    ${DIR_SOURCES}
)

target_include_directories(MathLib
    PUBLIC .
)

target_link_libraries(MathLib)