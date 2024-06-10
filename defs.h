#pragma once

#include <stdio.h>
#include <stdlib.h>

#ifndef NDEBUG
#define TODO(format, ...) fprintf(stderr, "\033[1m[TODO] \033[0m" format __VA_OPT__(,) __VA_ARGS__)
#define WARN(format, ...) fprintf(stderr, "\033[1;33m[WARNING] \033[0m" format __VA_OPT__(,) __VA_ARGS__)
#define ERROR(format, ...) fprintf(stderr, "\033[1;31m[ERROR] \033[0m" format __VA_OPT__(,) __VA_ARGS__)
#define OK(format, ...) fprintf(stderr, "\033[1;32m[OK] \033[0m" format __VA_OPT__(,) __VA_ARGS__)
#else
#define TODO(format, ...)
#define WARN(format, ...)
#define ERROR(format, ...)
#define OK(format, ...)
#endif

#define UNIMPLEMENTED() fprintf(stderr, "\033[1m[TODO] \033[0m%s is unimplemented!\n", __FUNCTION__)
#define PANIC(format, ...)                                                          \
	fprintf(stderr, "\033[1;31m[PANIC] \033[0m" format __VA_OPT__(,) __VA_ARGS__);	\
	exit(EXIT_FAILURE)

#define global        static
#define internal      static
#define local_persist static

//Bitflags definitions
#define ALL8  0xff
#define ALL16 0xffff
#define ALL32 0xffffffff
#define ALL64 0xffffffffffffffff

#ifdef _MSC_VER
#define INLINE __forceinline
#else
#define INLINE __attribute__((always_inline)) inline
#endif

#ifndef __cplusplus
#define LEN(arr) sizeof(arr)/sizeof(arr[0])
#define for_each(arr) for (u64 i = 0; i < LEN(arr); i++)
#define for_eachr(arr) for (i64 i = LEN(arr) - 1; i >= 0; i--)
#define MIN(X, Y) ((X) < (Y) ? (X) : (Y))
#define MAX(X, Y) ((X) > (Y) ? (X) : (Y))

#if !(__STDC_VERSION__ > 201710)
#define true  1
#define false 0
typedef char  bool;
#endif

typedef char * string;

#else

#define restrict __restrict

#endif

typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned int   u32;
typedef unsigned long  u64;

typedef signed char  i8;
typedef signed short i16;
typedef signed int   i32;
typedef signed long  i64;

typedef float  f32;
typedef double f64;
