cmake_minimum_required(VERSION 3.28)
project(samlib LANGUAGES C CXX)

add_library(
	samlib
	samlib.c
    samlib.cpp
)

set_target_properties(
    samlib PROPERTIES
    PUBLIC_HEADER samlib.h
)

if (NOT WIN32)
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
