cmake_minimum_required(VERSION 3.31.0)

file(GLOB DIR_SOURCES CONFIGURE_DEPENDS *.c)

add_library(BaseImagesLib STATIC
    ${DIR_SOURCES}
)

target_include_directories(BaseImagesLib
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

