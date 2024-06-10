#ifndef _159_ARENA_H_
#define _159_ARENA_H_

#include <math.h>
#include <stdlib.h>
#include <string.h>

const unsigned long BYTE     = 1;
const unsigned long KILOBYTE = 1024 * BYTE;
const unsigned long MEGABYTE = 1024 * KILOBYTE;
const unsigned long GIGABYTE = 1024 * MEGABYTE;

#define DEFAULT_GROWING_SIZE MEGABYTE

typedef enum {
    NONE,
    STACK_BUFFER,
    GROWING_BUFFER,
} Buffer_Type;

typedef struct {
    void *buffer;
    unsigned long pos;
    unsigned long size;
} Stack_Buffer;

typedef struct growing_buffer {
    void *buffer;
    struct growing_buffer *next;
    unsigned long pos;
    unsigned long size;
} Growing_Buffer;

typedef struct {
    Buffer_Type buffer_type;
    union {
        Stack_Buffer sb;
        Growing_Buffer gb;
    };
} Arena;

// Initializes a new `Arena` with the given `buffer` of length `buf_len`.
// It's size does NOT grow.
void arena_init_buffer(Arena *a, void *buffer, unsigned long buf_len) {
    a->buffer_type = STACK_BUFFER;
    a->sb.buffer = buffer;
    a->sb.pos = 0;
    a->sb.size = buf_len;
}

// Initializes a new `Arena` that utilizes a Linked List structure.
void arena_init_growing(Arena *a) {
    a->buffer_type = GROWING_BUFFER;
    a->gb.buffer = malloc(DEFAULT_GROWING_SIZE);
    a->gb.pos = 0;
    a->gb.next = NULL;
    a->gb.size = DEFAULT_GROWING_SIZE;
}

// Initializes a new `Arena` that utilizes a Linked List structure with size `size`.
void arena_init_growing2(Arena *a, unsigned long size) {
    a->buffer_type = GROWING_BUFFER;
    a->gb.buffer = malloc(size);
    a->gb.pos = 0;
    a->gb.next = NULL;
    a->gb.size = size;
}

// Frees all the memory allocated by a `Growing_Buffer`, basically a no op for a `Stack_Buffer`.
// Makes the `Arena` unusable in either case, until another `arena_init`.
void arena_free(Arena *a) {
    switch (a->buffer_type) {
    case STACK_BUFFER:
        a->buffer_type = NONE;
        break;
    case GROWING_BUFFER:
        a->buffer_type = NONE;
        {
            Growing_Buffer *cur = a->gb.next;
            while (cur) {
                free(cur->buffer);
                Growing_Buffer *aux = cur;
                cur = cur->next;
                free(aux);
            }
            free(a->gb.buffer);
        }
        break;
    default: break;
    }
}

// Resets the memory used by the allocator to be reused.
void arena_reset(Arena *a) {
    switch (a->buffer_type) {
    case STACK_BUFFER:
        memset(a->sb.buffer, 0, a->sb.size);
        a->sb.pos = 0;
        break;
    case GROWING_BUFFER:
        {
            Growing_Buffer *cur = a->gb.next;
            while (cur) {
                free(cur->buffer);
                Growing_Buffer *aux = cur;
                cur = cur->next;
                free(aux);
            }
            memset(a->gb.buffer, 0, a->gb.size);
            a->gb.next = NULL;
            a->gb.pos = 0;
        }
        break;
    default: break;
    }
}

// Returns a `NULL` pointer if it's a `Stack_Buffer` and doesn't have enough space
// or if the `Arena` has been deinitialized.
void *arena_alloc(Arena *a, unsigned long size) {
    void *ret;

    switch (a->buffer_type) {
    case STACK_BUFFER:
        if (a->sb.pos + size > a->sb.size) return NULL;
        ret = (char *)a->sb.buffer + a->sb.pos;
        a->sb.pos += size;
        break;
    case GROWING_BUFFER:
        {
            Growing_Buffer *aux = &a->gb;
            while (1) {
                if (aux->pos + size > a->gb.size) {
                    if (aux->next) {
                        aux = aux->next;
                        continue;
                    }
                    
                    const unsigned long size_to_grow = (unsigned long)ceil((double)size / a->gb.size) * a->gb.size;

                    aux->next = (Growing_Buffer *)malloc(sizeof(Growing_Buffer));
                    aux = aux->next;
                    aux->buffer = malloc(size_to_grow);
                    aux->pos = size;
                    aux->size = size_to_grow;
                    aux->next = NULL;
                    ret = aux->buffer;
                    break;
                }

                ret = (char *)aux->buffer + aux->pos;
                aux->pos += size;
                break;
            }
        }
        break;
    default: ret = NULL;
    }

    return ret;
}

#endif // _159_ARENA_H_
