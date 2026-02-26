cmake_minimum_required(VERSION 3.31.0)

project(SoftRenderer VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)

add_executable(SoftRenderer
  main.cpp
  test_bresenham_lines.cpp
  test_wu_blended_lines.cpp
  )

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

# Disable Raylib examples to keep the build clean and fast
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

if(NOT TARGET raylib)
  include(FetchContent)
  FetchContent_Declare(
      raylib
      GIT_REPOSITORY https://github.com/raysan5/raylib.git
      GIT_TAG        5.0
  )
  FetchContent_MakeAvailable(raylib)
endif()

add_subdirectory(Math)
add_subdirectory(Drawing)
add_subdirectory(Geometry)
add_subdirectory(Graphics)

target_include_directories(SoftRenderer
    PRIVATE Math
    PRIVATE Drawing
    PRIVATE Geometry
    PRIVATE Graphics
)

target_link_libraries(SoftRenderer
  MathLib
  DrawingLib
  GeometryLib
  GraphicsLib
  raylib
  )