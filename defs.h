#pragma once

#define global static
#define internal static
#define local_persist static

#ifndef __cplusplus
#define len(arr) sizeof(arr)/sizeof(arr[0])
#define for_each(arr) for (u64 i = 0; i < len(arr); i++)
#define for_eachr(arr) for (i64 i = len(arr) - 1; i >= 0; i--)
#define true 1
#define false 0

typedef char bool;
typedef char * string;

#else

#define restrict __restrict

#endif

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long u64;

typedef signed char i8;
typedef signed short i16;
typedef signed int i32;
typedef signed long i64;

typedef float f32;
typedef double f64;
