cmake_minimum_required(VERSION 3.31.0)

project(PipelineRenderer VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)

file(GLOB DIR_SOURCES CONFIGURE_DEPENDS *.cpp)

add_executable(PipelineRenderer
  ${DIR_SOURCES}
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

add_subdirectory(Maths)
add_subdirectory(Objects3D)
add_subdirectory(Core)

target_include_directories(PipelineRenderer
    PRIVATE Maths
    PRIVATE Objects3D
    PRIVATE Core
)

target_link_libraries(PipelineRenderer
  PUBLIC MathsLib
  PUBLIC Objects3DLib
  PUBLIC CoreLib
  PUBLIC raylib
  )