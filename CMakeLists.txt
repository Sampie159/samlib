add_library(
	defs
	defs.c
)

set_target_properties(
    defs PROPERTIES
    PUBLIC_HEADER defs.h
)
