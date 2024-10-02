cmake_minimum_required(VERSION 3.28)
project(defs C)

add_library(
	defs
	defs.c
)

set_target_properties(
    defs PROPERTIES
    PUBLIC_HEADER defs.h
)

target_include_directories(
    defs
    PUBLIC
    "./"
)
