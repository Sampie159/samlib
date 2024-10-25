cmake_minimum_required(VERSION 3.28)
project(samlib LANGUAGES C)

add_library(
	samlib
	samlib.c
)

set_target_properties(
    samlib PROPERTIES
    PUBLIC_HEADER samlib.h
)

target_include_directories(
    samlib
    PUBLIC
    "./"
)
