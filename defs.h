#include <stdio.h>
#include <stdlib.h>

#ifndef NDEBUG
    #define TODO(format, ...)  fprintf(stderr, "\033[1m[TODO] \033[0m" format __VA_OPT__(, ) __VA_ARGS__)
    #define WARN(format, ...)  fprintf(stderr, "\033[1;33m[WARNING] \033[0m" format __VA_OPT__(, ) __VA_ARGS__)
    #define ERROR(format, ...) fprintf(stderr, "\033[1;31m[ERROR] \033[0m" format __VA_OPT__(, ) __VA_ARGS__)
    #define OK(format, ...)    fprintf(stderr, "\033[1;32m[OK] \033[0m" format __VA_OPT__(, ) __VA_ARGS__)
#else
    #define TODO(format, ...)
    #define WARN(format, ...)
    #define ERROR(format, ...)
    #define OK(format, ...)
#endif

#define UNIMPLEMENTED() fprintf(stderr, "\033[1m[TODO] \033[0m%s is unimplemented!\n", __FUNCTION__)
#define PANIC(format, ...)                                                                                             \
    fprintf(stderr, "\033[1;31m[PANIC] \033[0m" format __VA_OPT__(, ) __VA_ARGS__);                                    \
    exit(EXIT_FAILURE)

#define global        static
#define internal      static
#define local_persist static

// Bitflags definitions
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
    #define LEN(arr)       sizeof(arr) / sizeof(arr[0])
    #define for_each(arr)  for (u64 i = 0; i < LEN(arr); i++)
    #define for_eachr(arr) for (i64 i = LEN(arr) - 1; i >= 0; i--)
    #define MIN(X, Y)      ((X) < (Y) ? (X) : (Y))
    #define MAX(X, Y)      ((X) > (Y) ? (X) : (Y))

    #if !(__STDC_VERSION__ > 201710)
        #define true  1
        #define false 0
typedef char bool;
    #endif

typedef char* string;

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

#define B(x)  (x)
#define KB(x) ((x) << 10)
#define MB(x) ((x) << 20)
#define GB(x) ((x) << 30)
#define TB(x) ((u64)(x) << 40)

#include <assert.h>
#include <stddef.h>
#include <string.h>

#ifdef __linux
    #include <sys/mman.h>
#else
    #include <windows.h>
#endif

const unsigned long BYTE     = B(1);
const unsigned long KILOBYTE = KB(1);
const unsigned long MEGABYTE = MB(1);
const unsigned long GIGABYTE = GB(1);
const unsigned long TERABYTE = TB(1);

#define DEFAULT_ARENA_SIZE GIGABYTE

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    void*         buffer;
    unsigned long pos;
    unsigned long size;
} Arena;

// Creates a new `Arena` of size `size`.
static Arena arena_new(unsigned long size) {
    return (Arena) {
#ifdef __linux
        .buffer = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0),
#else
        .buffer = VirtualAlloc(NULL, size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
#endif
        .pos = 0, .size = size,
    };
}

// Creates a new `Arena` of size `DEFAULT_GROWING_SIZE`.
static Arena arena_default(void) {
    return (Arena) {
#ifdef __linux
        .buffer = mmap(NULL, DEFAULT_ARENA_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0),
#else
        .buffer = VirtualAlloc(NULL, DEFAULT_ARENA_SIZE, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
#endif
        .pos = 0, .size = DEFAULT_ARENA_SIZE,
    };
}

// Allocates `size` bytes of memory on the `Arena`.
static void* arena_alloc(Arena* a, unsigned long size) {
    assert(a->pos + size < a->size);
    void* ptr  = (char*)a->buffer + a->pos;
    a->pos    += size;
    return ptr;
}

// Sets the cursor to position 0.
static void arena_reset(Arena* a) { a->pos = 0; }

// Sets the cursor to position 0 and zeroes the memory.
static void arena_clear(Arena* a) {
    memset(a->buffer, 0, a->pos);
    a->pos = 0;
}

// Frees the memory allocated by `Arena`.
static void arena_free(Arena* a) {
#ifdef __linux
    munmap(a->buffer, a->size);
#else
    VirtualFree(a->buffer, 0, MEM_RELEASE);
#endif
    a->buffer = NULL;
}

#ifdef __cplusplus
}
#endif
