cmake_minimum_required(VERSION 3.14)
project(SoftRenderer-Cpp-Raylib)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Prevent Raylib from building examples/games to speed up build
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES    OFF CACHE BOOL "" FORCE)

# Add subdirectories if they exist
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/SimpleWindow/CMakeLists.txt")
    add_subdirectory(SimpleWindow)
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/SoftRenderer/CMakeLists.txt")
    add_subdirectory(SoftRenderer)
endif()