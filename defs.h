#pragma once

#define global static
#define internal static
#define local_persist static

#ifndef __cplusplus
#include <stdbool.h>

typedef char * string;
#endif

typedef char u8; // I don't know if char will actually be unsigned, but for the sake of clarity I chose `u8`

typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long u64;

typedef short i16;
typedef int i32;
typedef long i64;

typedef float f32;
typedef double f64;
