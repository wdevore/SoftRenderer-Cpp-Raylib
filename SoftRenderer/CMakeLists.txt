cmake_minimum_required(VERSION 3.14)
project(SoftRenderer)

set(CMAKE_CXX_STANDARD 17)

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

add_executable(SoftRenderer main.cpp)
target_link_libraries(SoftRenderer raylib)