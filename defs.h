#include <stdio.h>
#include <stdlib.h>

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                   DEBUG                                   */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

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
#define PANIC(format, ...)												\
	fprintf(stderr, "\033[1;31m[PANIC] \033[0m" format __VA_OPT__(, ) __VA_ARGS__);	\
exit(EXIT_FAILURE)

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                  GENERAL                                  */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#define global        static
#define internal      static
#define local_persist static

// Bitflags definitions
#define ALL8  0xff
#define ALL16 0xffff
#define ALL32 0xffffffff
#define ALL64 0xffffffffffffffff

#ifdef _WIN32
#define INLINE __forceinline
#else
#define INLINE __attribute__((always_inline)) inline
#endif

#ifndef __cplusplus
#define LEN(arr)           sizeof(arr) / sizeof(arr[0])
#define for_each(arr)      for (u64 i = 0; i < LEN(arr); i++)
#define for_eachr(arr)     for (i64 i = LEN(arr) - 1; i >= 0; i--)
#define MIN(X, Y)          ((X) < (Y) ? (X) : (Y))
#define MAX(X, Y)          ((X) > (Y) ? (X) : (Y))
#define CLAMP(X, min, max) (X) = MIN(MAX(X, min), max)

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

static const u8  MAX_U8  = ALL8;
static const u16 MAX_U16 = ALL16;
static const u32 MAX_U32 = ALL32;
static const u64 MAX_U64 = ALL64;

static const i8  MAX_I8  = 127;
static const i16 MAX_I16 = 32767;
static const i32 MAX_I32 = 2147483647;
static const i64 MAX_I64 = 9223372036854775807;

static const i8  MIN_I8  = -127 - 1;
static const i16 MIN_I16 = -32767 - 1;
static const i32 MIN_I32 = -2147483647 - 1;
static const i64 MIN_I64 = -9223372036854775807 - 1;

static const f32 MAX_F32 = 3.402823466e+38f;
static const f32 MIN_F32 = -MAX_F32;

static const f64 MAX_F64 = 1.79769313486231e+308;
static const f64 MIN_F64 = -MAX_F64;

static const f32 PI = 3.14159265359f;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                               ARENA/MEMORY                                */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#define  B(x)  (x)
#define KB(x) ((x) << 10)
#define MB(x) ((x) << 20)
#define GB(x) ((x) << 30)
#define TB(x) ((u64)(x) << 40)

#include <assert.h>
#include <stddef.h>
#include <string.h>

#ifdef __unix
#include <sys/mman.h>
#else
#include <windows.h>
#endif

const u64     BYTE =  B(1);
const u64 KILOBYTE = KB(1);
const u64 MEGABYTE = MB(1);
const u64 GIGABYTE = GB(1);
const u64 TERABYTE = TB(1);

#define DEFAULT_ARENA_SIZE GIGABYTE

#ifdef __cplusplus
extern "C" {
#endif
    
typedef struct {
	void* buffer;
	u64   pos;
	u64   size;
	u64   align;
} Arena;
    
typedef struct {
	Arena* arena;
	u64    pos;
} ArenaTemp;
    
// Creates a new `Arena` of size `size`.
static Arena arena_new(u64 size) {
	return (Arena) {
#ifdef __unix
		.buffer = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0),
#else
		.buffer = VirtualAlloc(NULL, size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE),
#endif
		.pos   = 0,
		.size  = size,
		.align = 8,
	};
}
    
// Creates a new `Arena` of size `DEFAULT_GROWING_SIZE`.
static Arena arena_default(void) {
	return (Arena) {
#ifdef __unix
		.buffer = mmap(NULL, DEFAULT_ARENA_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0),
#else
		.buffer = VirtualAlloc(NULL, DEFAULT_ARENA_SIZE, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE),
#endif
		.pos   = 0,
		.size  = DEFAULT_ARENA_SIZE,
		.align = 8,
	};
}

// Creates a new `Arena` using a stack allocator.
static Arena arena_new_stack(char* buffer, u64 buf_size) {
	return (Arena) {
		.buffer = buffer,
		.pos    = 0,
		.size   = buf_size,
		.align  = 8,
	};
}

#define stack_arena(buffer) arena_new_buffer(buffer, sizeof buffer)
    
// Allocates `size` bytes of memory on the `Arena`.
static void* arena_alloc(Arena* a, u64 size) {
	if (a->pos + size > a->size) return NULL;
	void* ptr = (char*)a->buffer + a->pos;
	a->pos += size;
	return ptr;
}
    
#define push_array(a, T, c) (T*)arena_alloc((a), sizeof(T) * (c))
    
// Sets the cursor to position 0.
static void arena_reset(Arena* a) { a->pos = 0; }
    
// Sets the cursor to position 0 and zeroes the memory.
static void arena_clear(Arena* a) {
	memset(a->buffer, 0, a->pos);
	a->pos = 0;
}
    
// Frees the memory allocated by `Arena`.
static void arena_free(Arena* a) {
#ifdef __unix
	munmap(a->buffer, a->size);
#else
	VirtualFree(a->buffer, 0, MEM_RELEASE);
#endif
	a->buffer = NULL;
}
    
// Creates a new temporary `Arena`.
static ArenaTemp arena_temp_begin(Arena* arena) {
	return (ArenaTemp) {
		.arena = arena,
		.pos   = arena->pos,
	};
}
    
// Resets the temporary `Arena` to it's initial state.
static void arena_temp_end(ArenaTemp temp) {
	temp.arena->pos = temp.pos;
}
    
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                  STRINGS                                  */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <stdarg.h>

// 8bit string.
typedef struct {
	u8* str;
	u64 length;
} String;

// 16bit string (not yet implemented fully).
typedef struct {
	u16* str;
	u64  length;
} String16;

// Creates a new 8bit string.
static String string_new(u8* str) {
	u64 length = 0;
        
	while (str[length]) length += 1;
        
	return (String) {
		.str    = (u8*)str,
		.length = length,
	};
}

// Creates a new 8bit string.
#define make_string(str)  string_new((u8*)str)

// Creates a new formated 8bit string.
static String string_format(Arena* arena, const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);

	ArenaTemp temp = arena_temp_begin(arena);
	u8* str = arena_alloc(arena, B(256));
	u64 new_str_len = 0;

	for (const char *c = fmt; *c != 0; c++) {
		if (*c == '%') {
			c++;
			char mod;
			if (*c == 'l' || *c == 'h') {
				mod = *c;
				c++;
			}
			i32 aux_len = 0;
			switch (*c) {
			case 'd':
			case 'i':
			{
				if (mod == 'l') {
					i64 arg = va_arg(args, i64);
					i64 arg_aux = arg;
					bool neg = (arg < 0);
					if (neg) {
						str[new_str_len] = '-';
						new_str_len += 1;
						arg = -arg;
					}
					while (arg_aux != 0) {
						arg_aux /= 10;
						aux_len += 1;
					}
					for (i32 i = aux_len - 1; i >= 0; i--) {
						str[new_str_len + i] = '0' + (arg % 10);
						arg /= 10;
					}
				} else if (mod == 'h') {
					i16 arg = va_arg(args, i32);
					i16 arg_aux = arg;
					bool neg = (arg < 0);
					if (neg) {
						str[new_str_len] = '-';
						new_str_len += 1;
						arg = -arg;
					}
					while (arg_aux != 0) {
						arg_aux /= 10;
						aux_len += 1;
					}
					for (i32 i = aux_len - 1; i >= 0; i--) {
						str[new_str_len + i] = '0' + (arg % 10);
						arg /= 10;
					}
				}else {
					i32 arg = va_arg(args, i32);
					i32 arg_aux = arg;
					bool neg = (arg < 0);
					if (neg) {
						str[new_str_len] = '-';
						new_str_len += 1;
						arg = -arg;
					}
					while (arg_aux != 0) {
						arg_aux /= 10;
						aux_len += 1;
					}
					for (i32 i = aux_len - 1; i >= 0; i--) {
						str[new_str_len + i] = '0' + (arg % 10);
						arg /= 10;
					}
				}
				new_str_len += aux_len;
			} break;
			case 'f':
			case 'F':
			{
				f64 arg = va_arg(args, f64);
				i64 whole = (i64)arg;
				arg -= whole;
				bool neg = (whole < 0);
				if (neg) {
					str[new_str_len] = '-';
					new_str_len += 1;
					whole = -whole;
				}
				i64 whole_aux = whole;
				while (whole_aux != 0) {
					whole_aux /= 10;
					aux_len += 1;
				}
				for (i32 i = aux_len - 1; i >= 0; i--) {
					str[new_str_len + i] = '0' + (whole % 10);
					whole /= 10;
				}
				new_str_len += aux_len;
				str[new_str_len] = '.';
				new_str_len += 1;
				for (i8 prec = 0; prec < 12; prec++) {
					arg *= 10;
					i32 digit = (i32)arg;
					str[new_str_len] = '0' + digit;
					new_str_len += 1;
					arg -= digit;
				}
			} break;
			case 'c': {
				u8 arg = (u8)va_arg(args, int);
				str[new_str_len] = arg;
				new_str_len += 1;
			} break;
			case 's':
			{
				char* arg = va_arg(args, char*);
				while (*arg != 0) {
					str[new_str_len] = *arg;
					arg++;
					new_str_len += 1;
				}
			} break;
			case 'u':
			{
				if (mod == 'l') {
					u64 arg = va_arg(args, u64);
					u64 arg_aux = arg;
					while (arg_aux != 0) {
						arg_aux /= 10;
						aux_len += 1;
					}
					for (i32 i = aux_len - 1; i >= 0; i--) {
						str[new_str_len + i] = '0' + (arg % 10);
						arg /= 10;
					}
				} else if (mod == 'h') {
					u16 arg = va_arg(args, u32);
					u16 arg_aux = arg;
					while (arg_aux != 0) {
						arg_aux /= 10;
						aux_len += 1;
					}
					for (i32 i = aux_len - 1; i >= 0; i--) {
						str[new_str_len + i] = '0' + (arg % 10);
						arg /= 10;
					}
				}else {
					u32 arg = va_arg(args, u32);
					u32 arg_aux = arg;
					while (arg_aux != 0) {
						arg_aux /= 10;
						aux_len += 1;
					}
					for (i32 i = aux_len - 1; i >= 0; i--) {
						str[new_str_len + i] = '0' + (arg % 10);
						arg /= 10;
					}
				}
				new_str_len += aux_len;
			} break;
			case 'p':
			{
				u64 arg = (u64)va_arg(args, void*);
				u64 arg_aux = arg;
				while (arg_aux != 0) {
					arg_aux /= 10;
					aux_len += 1;
				}
				for (i32 i = aux_len - 1; i >= 0; i--) {
					str[new_str_len + i] = '0' + (arg % 10);
					arg /= 10;
				}
				new_str_len += aux_len;
			} break;
			case '%':
			{
				str[new_str_len] = '%';
				new_str_len += 1;
			} break;
			default: break;
			}
		} else {
			str[new_str_len] = *c;
			new_str_len += 1;
		}
	}

	va_end(args);
	arena_temp_end(temp);
	push_array(arena, u8, new_str_len);

	return (String) {
		.str    = str,
		.length = new_str_len,
	};
}

// Prints the 8bit string to stdout.
static void string_print(const String str) {
#ifdef __unix
	write(1, str.str, str.length);
#elif defined(_WIN32)
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	WriteFile(handle, str.str, str.length, NULL, NULL);
#endif
}

// Prints the 8bit string to stderr.
static void string_eprint(const String str) {
#ifdef __unix
	write(2, str.str, str.length);
#elif defined(_WIN32)
	HANDLE handle = GetStdHandle(STD_ERROR_HANDLE);
	WriteFile(handle, str.str, str.length, NULL, NULL);
#endif
}

// Creates a new `String` resulting from the concatenation of `str1` and `str2`.
static String string_concat(Arena* arena, const String str1, const String str2) {
	u64 new_length = str1.length + str2.length;
	u8* new_str = push_array(arena, u8, new_length);
	memcpy(new_str, str1.str, str1.length);
	memcpy(new_str + str1.length, str2.str, str2.length);

	return (String) {
		.str    = new_str,
		.length = new_length,
	};
}

// Creates a new 16bit string.
static String16 string16_new(u16* str) {
	u64 length = 0;
        
	while (str[length]) length += 1;
        
	return (String16) {
		.str    = str,
		.length = length,
	};
}

// Creates a new 16bit string.
#define make_string16(str) string16_new((u16*)str)
    
#ifdef __cplusplus
}
#endif
