#ifndef _159_ARENA_H_
#define _159_ARENA_H_

#include <assert.h>
#include <stddef.h>
#include <string.h>

#ifdef __linux
    #include <sys/mman.h>
#else
    #include <windows.h>
#endif

const unsigned long BYTE     = 1;
const unsigned long KILOBYTE = 1024 * BYTE;
const unsigned long MEGABYTE = 1024 * KILOBYTE;
const unsigned long GIGABYTE = 1024 * MEGABYTE;
const unsigned long TERABYTE = 1024 * GIGABYTE;

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

#endif   // _159_ARENA_H_
