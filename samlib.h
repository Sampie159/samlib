#if !defined(_SAMLIB_H_)

#include <stdio.h>
#include <stdlib.h>

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                  GENERAL                                  */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// Bitflags definitions
#define ALL8  0xff
#define ALL16 0xffff
#define ALL32 0xffffffff
#define ALL64 0xffffffffffffffff

#if defined(_WIN32)
    #define INLINE __forceinline
    #define SAMAPI __declspec(dllexport)
#else
    #define INLINE __attribute__((always_inline)) inline
    #define SAMAPI
#endif

#define LEN(arr)           sizeof(arr) / sizeof(arr[0])
#define MIN(X, Y)          ((X) < (Y) ? (X) : (Y))
#define MAX(X, Y)          ((X) > (Y) ? (X) : (Y))
#define CLAMP(X, min, max) (X) = MIN(MAX(X, min), max)
#define UNIMPLEMENTED()    fprintf(stderr, "\033[1m[TODO] \033[0m%s is unimplemented!\n", __FUNCTION__)
#define PANIC(format, ...) do {                                                     \
    fprintf(stderr, "\033[1;31m[PANIC] \033[0m" format __VA_OPT__(, ) __VA_ARGS__);	\
    __builtin_trap();                                                               \
    exit(EXIT_FAILURE);                                                             \
} while (0)

#if !defined(__cplusplus)
    #if (__STDC_VERSION__ < 202311)
        #define true  1
        #define false 0
        #define alignof _Alignof
    #endif
#else
    #define restrict __restrict
#endif

#define local   static
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

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                   DEBUG                                   */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#if !defined(NDEBUG)
    #define TODO(format, ...)  fprintf(stderr, "\033[1m[TODO] \033[0m" format __VA_OPT__(, ) __VA_ARGS__)
    #define WARN(format, ...)  fprintf(stderr, "\033[1;33m[WARNING] \033[0m" format __VA_OPT__(, ) __VA_ARGS__)
    #define ERR(format, ...)   fprintf(stderr, "\033[1;31m[ERROR] \033[0m" format __VA_OPT__(, ) __VA_ARGS__)
    #define OK(format, ...)    fprintf(stderr, "\033[1;32m[OK] \033[0m" format __VA_OPT__(, ) __VA_ARGS__)
    #define DEBUG if (1)
    #define ASSERT(cond)                                                                          \
        do {                                                                                      \
            if (!(cond)) PANIC("Assertion failed: %s\nfile: %s:%d\n", #cond, __FILE__, __LINE__); \
        } while (0)
    #define ASSERTF(cond, format, ...)                             \
        do {                                                       \
            if (!(cond)) PANIC(format __VA_OPT__(, ) __VA_ARGS__); \
        } while (0)
#else
    #define TODO(format, ...)
    #define WARN(format, ...)
    #define ERR(format, ...)
    #define OK(format, ...)
    #define DEBUG if (0)
    #define ASSERT(cond)
    #define ASSERTF(cond, format, ...)
#endif

#if defined(__cplusplus)
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
} Arena;

typedef struct {
	Arena* arena;
	u64    pos;
} TempArena;

// Create a new `Arena` of size `cap`. Minimum size is 4KB, if a smaller size is
// given it will still allocate 4KB.
Arena arena_new(u64 cap);
void* arena_alloc(Arena* a, u64 size, u64 alignment);
void* arena_pop(Arena* a, u64 size);
void  arena_reset(Arena* a);
void  arena_clear(Arena* a);
void  arena_free(Arena* a);

TempArena temp_arena_begin(Arena* a);
void      temp_arena_end(TempArena temp);

#define arena_default()     arena_new(DEFAULT_ARENA_SIZE)
#define push_array(a, T, c) (T*)arena_alloc((a), sizeof(T) * (c), alignof(T))
#define push_type(a, T)     (T*)arena_alloc((a), sizeof(T), alignof(T))
#define pop_type(a, T)      (T*)arena_pop((a), sizeof(T))

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                  STRINGS                                  */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// 8bit string.
typedef struct {
	u8* buffer;
	u64 length;
} String;

String string_init(u8* buffer);
void   string_write_str(String* str, const char* s);
void   string_write_s8(String* str, s8 val);
void   string_write_s16(String* str, s16 val);
void   string_write_s32(String* str, s32 val);
void   string_write_s64(String* str, s64 val);
void   string_write_u8(String* str, u8 val);
void   string_write_u16(String* str, u16 val);
void   string_write_u32(String* str, u32 val);
void   string_write_u64(String* str, u64 val);
void   string_write_f32(String* str, f32 val);
void   string_write_f64(String* str, f64 val);
void   string_write_ptr(String* str, const void* ptr);
void   string_null(String* str);
void   string_newline(String* str);
void   string_reset(String* str);
char*  string_to_cstr(String* str);
void   string_print(const String str);
void   string_println(const String str);
void   string_eprint(const String str);
void   string_eprintln(const String str);
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

#define str_slice_end(str, init)  string_slice(str, init, str.length)
#define str_slice_until(str, end) string_slice(str, 0, end)

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                               DYNAMIC ARRAY                               */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#if !defined(__cplusplus)

typedef struct {
    void*  data;
    u64    cap;
    u64    len;

    const u64 type_size;
} Array;

Array array_create(u64 type_size);
void  array_reserve(Array* da, u64 cap);
void  array_resize(Array* da, u64 new_cap);
void  array_push(Array* da, const void* val);
void  array_pushf(Array* da, const void* val);
void  array_pushi(Array* da, const void* val, u64 idx);
void  array_pop(Array* da);
void  array_popf(Array* da);
void  array_popi(Array* da, u64 idx);
void  array_clear(Array* da);
void  array_destroy(Array* da);

#define make_array(T)        array_create(sizeof(T))
#define push(da, v)          array_push((da), (void*)&(v))
#define push_front(da, v)    array_pushf((da), (void*)&(v))
#define push_idx(da, v, idx) array_pushi((da), (void*)&(v), idx)
#define at(da, T, idx)       *((T*)(da)->data + (idx))
#define at_ref(da, T, idx)   ((T*)(da)->data + (idx))

#endif

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                   MATH                                    */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

typedef union {
    struct {
        f32 x, y;
    };
    f32 e[2];
} Vec2;

typedef union {
    struct {
        f32 x, y, z;
    };
    struct {
        f32 r, g, b;
    };
    f32 e[3];
} Vec3;

typedef union {
    struct {
        f32 x, y, z, w;
    };
    struct {
        f32 r, g, b, a;
    };
    f32 e[4];
} Vec4;

#if !defined(__cplusplus)

Vec2 vec2_addvec(Vec2 a, Vec2 b);
Vec2 vec2_addval(Vec2 a, f32 val);
Vec2 vec2_subvec(Vec2 a, Vec2 b);
Vec2 vec2_subval(Vec2 a, f32 val);
Vec2 vec2_mul(Vec2 a, f32 scalar);
Vec2 vec2_div(Vec2 a, f32 scalar);
b8   vec2_eq(Vec2 a, Vec2 b);
void vec2_clamp(Vec2* vec, Vec2 min, Vec2 max);
f32  vec2_square(Vec2 vec);
f32  vec2_dot(Vec2 a, Vec2 b);
f32  vec2_length(Vec2 vec);
f32  vec2_length_sq(Vec2 vec);

Vec3 vec3_addvec(Vec3 a, Vec3 b);
Vec3 vec3_addval(Vec3 a, f32 val);
Vec3 vec3_subvec(Vec3 a, Vec3 b);
Vec3 vec3_subval(Vec3 a, f32 val);
Vec3 vec3_mul(Vec3 a, f32 scalar);
Vec3 vec3_div(Vec3 a, f32 scalar);
b8   vec3_eq(Vec3 a, Vec3 b);
void vec3_clamp(Vec3* vec, Vec3 min, Vec3 max);
f32  vec3_dot(Vec3 a, Vec3 b);
f32  vec3_length(Vec3 vec);
f32  vec3_length_sq(Vec3 vec);

Vec4 vec4_addvec(Vec4 a, Vec4 b);
Vec4 vec4_addval(Vec4 a, f32 val);
Vec4 vec4_subvec(Vec4 a, Vec4 b);
Vec4 vec4_subval(Vec4 a, f32 val);
Vec4 vec4_mul(Vec4 a, f32 scalar);
Vec4 vec4_div(Vec4 a, f32 scalar);
b8   vec4_eq(Vec4 a, Vec4 b);
void vec4_clamp(Vec4* vec, Vec4 min, Vec4 max);
f32  vec4_dot(Vec4 a, Vec4 b);
f32  vec4_length(Vec4 vec);
f32  vec4_length_sq(Vec4 vec);

#endif

f32 square(f32 val);
f32 root(f32 val);

#if defined(__cplusplus)
}

Vec2 operator+(Vec2 a, Vec2 b);
void operator+=(Vec2& a, Vec2 b);
Vec2 operator+(Vec2 a, f32 val);
void operator+=(Vec2& a, f32 val);
Vec2 operator-(Vec2 a, Vec2 b);
void operator-=(Vec2& a, Vec2 b);
Vec2 operator-(Vec2 a, f32 val);
void operator-=(Vec2& a, f32 val);
Vec2 operator*(Vec2 a, f32 scalar);
void operator*=(Vec2& a, f32 scalar);
Vec2 operator/(Vec2 a, f32 scalar);
void operator/=(Vec2& a, f32 scalar);
bool operator==(Vec2 a, Vec2 b);
bool operator!=(Vec2 a, Vec2 b);
void clamp(Vec2& vec, Vec2 min, Vec2 max);
f32  dot(Vec2 a, Vec2 b);
f32  length(Vec2 vec);
f32  length_sq(Vec2 vec);

Vec3 operator+(Vec3 a, Vec3 b);
void operator+=(Vec3& a, Vec3 b);
Vec3 operator+(Vec3 a, f32 val);
void operator+=(Vec3& a, f32 val);
Vec3 operator-(Vec3 a, Vec3 b);
void operator-=(Vec3& a, Vec3 b);
Vec3 operator-(Vec3 a, f32 val);
void operator-=(Vec3& a, f32 val);
Vec3 operator*(Vec3 a, f32 scalar);
void operator*=(Vec3& a, f32 scalar);
Vec3 operator/(Vec3 a, f32 scalar);
void operator/=(Vec3& a, f32 scalar);
bool operator==(Vec3 a, Vec3 b);
bool operator!=(Vec3 a, Vec3 b);
void clamp(Vec3& vec, Vec3 min, Vec3 max);
f32  dot(Vec3 a, Vec3 b);
f32  length(Vec3 vec);
f32  length_sq(Vec3 vec);

Vec4 operator+(Vec4 a, Vec4 b);
void operator+=(Vec4& a, Vec4 b);
Vec4 operator+(Vec4 a, f32 val);
void operator+=(Vec4& a, f32 val);
Vec4 operator-(Vec4 a, Vec4 b);
void operator-=(Vec4& a, Vec4 b);
Vec4 operator-(Vec4 a, f32 val);
void operator-=(Vec4& a, f32 val);
Vec4 operator*(Vec4 a, f32 scalar);
void operator*=(Vec4& a, f32 scalar);
Vec4 operator/(Vec4 a, f32 scalar);
void operator/=(Vec4& a, f32 scalar);
bool operator==(Vec4 a, Vec4 b);
bool operator!=(Vec4 a, Vec4 b);
void clamp(Vec4& vec, Vec4 min, Vec4 max);
f32  dot(Vec4 a, Vec4 b);
f32  length(Vec4 vec);
f32  length_sq(Vec4 vec);

#endif

#define _SAMLIB_H_
#endif  // _SAMLIB_H_
