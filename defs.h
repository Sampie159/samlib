#pragma once

#define UNIMPLEMENTED() fprintf(stderr, "\033[1m[TODO] \033[0m%s is unimplemented!\n", __FUNCTION__)
#define TODO(MSG) fprintf(stderr, "\033[1m[TODO] \033[0m%s\n", MSG)
#define WARN(MSG) fprintf(stderr, "\033[1;33m[WARNING] \033[0m%s\n", MSG)
#define ERROR(MSG)											\
	fprintf(stderr, "\033[1;31m[ERROR] \033[0m%s\n", MSG);	\
	exit(EXIT_FAILURE)

#define global        static
#define internal      static
#define local_persist static

//Bitflags definitions
#define ALL8  0xff
#define ALL16 0xffff
#define ALL32 0xffffffff
#define ALL64 0xffffffffffffffff

#ifndef __cplusplus
#define len(arr) sizeof(arr)/sizeof(arr[0])
#define for_each(arr) for (u64 i = 0; i < len(arr); i++)
#define for_eachr(arr) for (i64 i = len(arr) - 1; i >= 0; i--)
#define min(X, Y) ((X) < (Y) ? (X) : (Y))
#define max(X, Y) ((X) > (Y) ? (X) : (Y))
#define true  1
#define false 0

typedef char * string;
typedef char   bool;

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
