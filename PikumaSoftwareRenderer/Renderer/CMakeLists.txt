cmake_minimum_required(VERSION 3.31.0)

file(GLOB DIR_SOURCES CONFIGURE_DEPENDS *.cpp)

add_library(RendererLib STATIC
    ${DIR_SOURCES}
)

target_include_directories(RendererLib
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(RendererLib
  PUBLIC BaseLib
  PUBLIC BaseMathsLib
  PUBLIC BaseViewLib
  PUBLIC BaseLightsLib
  PUBLIC BaseGeometryLib
  PUBLIC ParticlesLib
  PUBLIC raylib
)