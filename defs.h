#ifndef _DEFS_H_
#define _DEFS_H_

#include <stdio.h>
#include <stdlib.h>

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                   DEBUG                                   */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef NDEBUG
#define TODO(format, ...)  fprintf(stderr, "\033[1m[TODO] \033[0m" format __VA_OPT__(, ) __VA_ARGS__)
#define WARN(format, ...)  fprintf(stderr, "\033[1;33m[WARNING] \033[0m" format __VA_OPT__(, ) __VA_ARGS__)
#define ERR(format, ...)   fprintf(stderr, "\033[1;31m[ERROR] \033[0m" format __VA_OPT__(, ) __VA_ARGS__)
#define OK(format, ...)    fprintf(stderr, "\033[1;32m[OK] \033[0m" format __VA_OPT__(, ) __VA_ARGS__)
#else
#define TODO(format, ...)
#define WARN(format, ...)
#define ERR(format, ...)
#define OK(format, ...)
#endif

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                  GENERAL                                  */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// Bitflags definitions
#define ALL8  0xff
#define ALL16 0xffff
#define ALL32 0xffffffff
#define ALL64 0xffffffffffffffff

#ifdef __unix
#define INLINE __attribute__((always_inline)) inline
#else
#define INLINE __forceinline
#endif

#define LEN(arr)           sizeof(arr) / sizeof(arr[0])
#define MIN(X, Y)          ((X) < (Y) ? (X) : (Y))
#define MAX(X, Y)          ((X) > (Y) ? (X) : (Y))
#define CLAMP(X, min, max) (X) = MIN(MAX(X, min), max)
#define UNIMPLEMENTED()    fprintf(stderr, "\033[1m[TODO] \033[0m%s is unimplemented!\n", __FUNCTION__)
#define PANIC(format, ...) do {                                                     \
    fprintf(stderr, "\033[1;31m[PANIC] \033[0m" format __VA_OPT__(, ) __VA_ARGS__);	\
    exit(EXIT_FAILURE);                                                             \
} while (0)

#ifndef __cplusplus
#if !(__STDC_VERSION__ > 201710)
#define true  1
#define false 0
#endif

typedef char* string;

#else

#define restrict __restrict

#endif

#define func    static
#define global  static
#define persist static

typedef unsigned char       u8;
typedef unsigned short      u16;
typedef unsigned int        u32;
typedef unsigned long long  u64;

typedef signed char      s8;
typedef signed short     s16;
typedef signed int       s32;
typedef signed long long s64;

typedef float  f32;
typedef double f64;

typedef s8  b8;
typedef s16 b16;
typedef s32 b32;
typedef s64 b64;

global const u8  MAX_U8  = ALL8;
global const u16 MAX_U16 = ALL16;
global const u32 MAX_U32 = ALL32;
global const u64 MAX_U64 = ALL64;

global const s8  MAX_S8  = 127;
global const s16 MAX_S16 = 32767;
global const s32 MAX_S32 = 2147483647;
global const s64 MAX_S64 = 9223372036854775807;

global const s8  MIN_S8  = -127 - 1;
global const s16 MIN_S16 = -32767 - 1;
global const s32 MIN_S32 = -2147483647 - 1;
global const s64 MIN_S64 = -9223372036854775807 - 1;

global const f32 MAX_F32 = 3.402823466e+38f;
global const f64 MAX_F64 = 1.79769313486231e+308;

global const f32 PI = 3.14159265359f;

#ifdef __cplusplus
extern "C" {
#endif

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                               ARENA/MEMORY                                */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#define  B(x)  (x)
#define KB(x) ((x) << 10)
#define MB(x) ((x) << 20)
#define GB(x) ((u64)(x) << 30)
#define TB(x) ((u64)(x) << 40)

#define DEFAULT_ARENA_SIZE GB(1)

typedef struct {
	void* buffer;
	u64   pos;
	u64   cap;
	u64   com;
} Arena;

typedef struct {
	Arena* arena;
	u64    pos;
} ArenaTemp;

// Create a new `Arena` of size `size`. Minimum size is 4KB, if a smaller size is
// given it will still allocate 4KB.
Arena arena_new(u64 cap);
void* arena_alloc(Arena* a, u64 size);
void* arena_pop(Arena* arena, u64 size);
void  arena_reset(Arena* a);
void  arena_clear(Arena* a);
void  arena_free(Arena* a);

ArenaTemp arena_temp_begin(Arena* arena);
void      arena_temp_end(ArenaTemp temp);

#define arena_default()     arena_new(DEFAULT_ARENA_SIZE)
#define push_array(a, T, c) (T*)arena_alloc((a), sizeof(T) * (c))
#define push_type(a, T)     (T*)arena_alloc((a), sizeof(T))
#define pop_type(a, T)      (T*)arena_pop((a), sizeof(T))

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                  STRINGS                                  */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// 8bit string.
typedef struct {
	u8* str;
	u64 length;
} String;

String string_new(Arena* arena, const u8* str);
String string_format(Arena* arena, const char* fmt, ...);
void   string_print(const String str);
void   string_eprint(const String str);
String string_concat(Arena* arena, const String str1, const String str2);
String string_slice(const String str, const u64 init, const u64 end);
void   string_upper(String str);
void   string_lower(String str);
String string_upper_new(Arena* arena, const String str);
String string_lower_new(Arena* arena, const String str);
b8     string_equals(const String str1, const String str2);
b8     string_cmp(const String str1, const char* str2);
s8     string_to_s8(const String str);
s16    string_to_s16(const String str);
s32    string_to_s32(const String str);
s64    string_to_s64(const String str);
u8     string_to_u8(const String str);
u16    string_to_u16(const String str);
u32    string_to_u32(const String str);
u64    string_to_u64(const String str);
#if 0
f32    string_to_f32(const String str);
f64    string_to_f64(const String str);
#endif

#define make_string(arena, str) string_new((arena), (u8*)str)
#define str_slice_end(str, init)  string_slice(str, init, str.length)
#define str_slice_until(str, end) string_slice(str, 0, end)

#ifdef __cplusplus
}
#endif

#endif  // _DEFS_H_
