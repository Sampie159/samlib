cmake_minimum_required(VERSION 3.28)
project(samlib LANGUAGES C)

add_library(
	samlib
	samlib.c
)

if (CMAKE_CXX_COMPILER)
    enable_language(CXX)
    target_sources(samlib PRIVATE samlib.cpp)
endif ()

set_target_properties(
    samlib PROPERTIES
    PUBLIC_HEADER samlib.h
)

if (NOT CMAKE_C_COMPILER_ID STREQUAL "MSVC")
    target_link_libraries(
        samlib
        PRIVATE
        m
    )
endif ()

target_include_directories(
    samlib
    PUBLIC
    "./"
)
