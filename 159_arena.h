#ifndef _159_ARENA_H_
#define _159_ARENA_H_

#include <stdlib.h>

#define DEFAULT_GROWING_SIZE (1024 * 1024)
const unsigned long BYTE     = 1;
const unsigned long KILOBYTE = 1024 * BYTE;
const unsigned long MEGABYTE = 1024 * KILOBYTE;
const unsigned long GIGABYTE = 1024 * MEGABYTE;

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
}

// Frees all the memory allocated by a `Growing_Buffer`, basically a no op for a `Stack_Buffer`.
// Makes the `Arena` unusable in either case, until another `arena_init`.
void arena_deinit(Arena *a) {
    if (a->buffer_type == STACK_BUFFER) {
        a->buffer_type = NONE;
        return;
    }

    a->buffer_type = NONE;
    Growing_Buffer *cur = a->gb.next;
    while (cur) {
        free(cur->buffer);
        Growing_Buffer *aux = cur;
        cur = cur->next;
        free(aux);
    }
    free(a->gb.buffer);
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
                if (aux->pos + size > DEFAULT_GROWING_SIZE) {
                    if (aux->next) {
                        aux = aux->next;
                        continue;
                    }
                    aux->next = (Growing_Buffer *)malloc(sizeof(Growing_Buffer));
                    aux = aux->next;
                    aux->buffer = malloc(DEFAULT_GROWING_SIZE);
                    aux->pos = size;
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
