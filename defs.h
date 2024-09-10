/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                 INCLUDES                                  */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdarg.h>

#ifdef __unix
#include <sys/mman.h>
#include <unistd.h>
#else
#include <windows.h>
#endif

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

#define UNIMPLEMENTED() fprintf(stderr, "\033[1m[TODO] \033[0m%s is unimplemented!\n", __FUNCTION__)
#define PANIC(format, ...)												\
	fprintf(stderr, "\033[1;31m[PANIC] \033[0m" format __VA_OPT__(, ) __VA_ARGS__);	\
	exit(EXIT_FAILURE)

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

#ifndef __cplusplus
#if !(__STDC_VERSION__ > 201710)
#define true  1
#define false 0
typedef char bool;
#endif

typedef char* string;

#else

#define restrict __restrict

#endif

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

static const u8  MAX_U8  = ALL8;
static const u16 MAX_U16 = ALL16;
static const u32 MAX_U32 = ALL32;
static const u64 MAX_U64 = ALL64;

static const s8  MAX_S8  = 127;
static const s16 MAX_S16 = 32767;
static const s32 MAX_S32 = 2147483647;
static const s64 MAX_S64 = 9223372036854775807;

static const s8  MIN_S8  = -127 - 1;
static const s16 MIN_S16 = -32767 - 1;
static const s32 MIN_S32 = -2147483647 - 1;
static const s64 MIN_S64 = -9223372036854775807 - 1;

static const f32 MAX_F32 = 3.402823466e+38f;
static const f64 MAX_F64 = 1.79769313486231e+308;

static const f32 PI = 3.14159265359f;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                               ARENA/MEMORY                                */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#define  B(x)  (x)
#define KB(x) ((x) << 10)
#define MB(x) ((x) << 20)
#define GB(x) ((u64)(x) << 30)
#define TB(x) ((u64)(x) << 40)

#define DEFAULT_ARENA_SIZE GB(1)

#ifdef __cplusplus
extern "C" {
#endif

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

// Create a new `Arena` of size `size`.
static Arena arena_new(u64 cap) {
	return (Arena) {
#ifdef __unix
		.buffer = mmap(NULL, cap, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0),
#else
		.buffer = VirtualAlloc(NULL, cap, MEM_RESERVE, PAGE_READWRITE),
#endif
		.pos = 0,
		.cap = cap,
		.com = 0,
	};
}

// Create a new `Arena` of size `DEFAULT_ARENA_SIZE`.
#define arena_default() arena_new(DEFAULT_ARENA_SIZE)

// Allocate `size` bytes of memory on the `Arena`.
static void* arena_alloc(Arena* a, u64 size) {
	if (a->pos + size > a->cap) return NULL;
	if (a->pos + size > a->com) {
		// TODO(sampie): Test this shit.
		f64 div =  (f64)size/(f64)KB(4);
		u64 size_to_commit = (u64)div;
		if (div > (f64)size_to_commit) size_to_commit += 1;
		size_to_commit *= KB(4);
#ifdef __unix
		const s32 prot = PROT_READ | PROT_WRITE;
		const s32 flags = MAP_FIXED | MAP_PRIVATE | MAP_ANONYMOUS;
		mmap((char*)a->buffer + a->com, size_to_commit, prot, flags, -1, 0);
#else
		VirtualAlloc((char*)a->buffer + a->com, size_to_commit, MEM_COMMIT, PAGE_READWRITE);
#endif
		a->com += size_to_commit;
	}
	void* ptr = (char*)a->buffer + a->pos;
	a->pos += size;
	return ptr;
}

// Push `c` amount of type `T` into arena `a`.
#define push_array(a, T, c) (T*)arena_alloc((a), sizeof(T) * (c))
// Push `T` into arena `a`.
#define push_type(a, T) push_array(a, T, 1)

// Set the cursor to position 0.
static void arena_reset(Arena* a) { a->pos = 0; }

// Set the cursor to position 0 and zeroes the memory.
static void arena_clear(Arena* a) {
	memset(a->buffer, 0, a->pos);
	a->pos = 0;
}

// Free the memory allocated by `Arena`.
static void arena_free(Arena* a) {
#ifdef __unix
	munmap(a->buffer, a->cap);
#else
	VirtualFree(a->buffer, 0, MEM_RELEASE);
#endif
	a->buffer = NULL;
	a->cap = 0;
	a->pos = 0;
	a->com = 0;
}

// Create a new temporary `Arena`.
static ArenaTemp arena_temp_begin(Arena* arena) {
	return (ArenaTemp) {
		.arena = arena,
		.pos   = arena->pos,
	};
}

// Reset the temporary `Arena` to it's initial state.
static void arena_temp_end(ArenaTemp temp) {
	temp.arena->pos = temp.pos;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                  STRINGS                                  */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// 8bit string.
typedef struct {
	u8* str;
	u64 length;
} String;

// Create a new 8bit string.
static String string_new(Arena* arena, const u8* str) {
	u64 length = 0;

	while (str[length]) length += 1;
	u8* new_str = push_array(arena, u8, length);
	memcpy(new_str, str, length);

	return (String) {
		.str    = new_str,
		.length = length,
	};
}

// Create a new 8bit string.
#define make_string(arena, str) string_new((arena), (u8*)str)

// Allocate a new formated 8bit string.
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
			s32 aux_len = 0;
			switch (*c) {
			case 'd':
			case 'i': {
				if (mod == 'l') {
					s64 arg = va_arg(args, s64);
					s64 arg_aux = arg;
					bool neg = (arg < 0);
					if (neg) {
						str[new_str_len] = '-';
						new_str_len += 1;
						arg = -arg;
					}
					if (arg == 0) {
						str[new_str_len] = '0';
						new_str_len += 1;
					}
					while (arg_aux != 0) {
						arg_aux /= 10;
						aux_len += 1;
					}
					for (s32 i = aux_len - 1; i >= 0; i--) {
						str[new_str_len + i] = '0' + (arg % 10);
						arg /= 10;
					}
				} else if (mod == 'h') {
					s16 arg = va_arg(args, s32);
					s16 arg_aux = arg;
					bool neg = (arg < 0);
					if (neg) {
						str[new_str_len] = '-';
						new_str_len += 1;
						arg = -arg;
					}
					if (arg == 0) {
						str[new_str_len] = '0';
						new_str_len += 1;
					}
					while (arg_aux != 0) {
						arg_aux /= 10;
						aux_len += 1;
					}
					for (s32 i = aux_len - 1; i >= 0; i--) {
						str[new_str_len + i] = '0' + (arg % 10);
						arg /= 10;
					}
				} else {
					s32 arg = va_arg(args, s32);
					s32 arg_aux = arg;
					bool neg = (arg < 0);
					if (neg) {
						str[new_str_len] = '-';
						new_str_len += 1;
						arg = -arg;
					}
					if (arg == 0) {
						str[new_str_len] = '0';
						new_str_len += 1;
					}
					while (arg_aux != 0) {
						arg_aux /= 10;
						aux_len += 1;
					}
					for (s32 i = aux_len - 1; i >= 0; i--) {
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
				s64 whole = (s64)arg;
				arg -= whole;
				bool neg = (whole < 0);
				if (neg) {
					str[new_str_len] = '-';
					new_str_len += 1;
					whole = -whole;
				}
				if (whole == 0) {
					str[new_str_len] = '0';
					new_str_len += 1;
				}
				s64 whole_aux = whole;
				while (whole_aux != 0) {
					whole_aux /= 10;
					aux_len += 1;
				}
				for (s32 i = aux_len - 1; i >= 0; i--) {
					str[new_str_len + i] = '0' + (whole % 10);
					whole /= 10;
				}
				new_str_len += aux_len;
				str[new_str_len] = '.';
				new_str_len += 1;
				for (s8 prec = 0; prec < 12; prec++) {
					arg *= 10;
					s32 digit = (s32)arg;
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
					if (arg_aux == 0) {
						str[new_str_len] = '0';
						new_str_len += 1;
					}
					while (arg_aux != 0) {
						arg_aux /= 10;
						aux_len += 1;
					}
					for (s32 i = aux_len - 1; i >= 0; i--) {
						str[new_str_len + i] = '0' + (arg % 10);
						arg /= 10;
					}
				} else if (mod == 'h') {
					u16 arg = va_arg(args, u32);
					u16 arg_aux = arg;
					if (arg_aux == 0) {
						str[new_str_len] = '0';
						new_str_len += 1;
					}
					while (arg_aux != 0) {
						arg_aux /= 10;
						aux_len += 1;
					}
					for (s32 i = aux_len - 1; i >= 0; i--) {
						str[new_str_len + i] = '0' + (arg % 10);
						arg /= 10;
					}
				} else {
					u32 arg = va_arg(args, u32);
					u32 arg_aux = arg;
					if (arg_aux == 0) {
						str[new_str_len] = '0';
						new_str_len += 1;
					}
					while (arg_aux != 0) {
						arg_aux /= 10;
						aux_len += 1;
					}
					for (s32 i = aux_len - 1; i >= 0; i--) {
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
				if (arg_aux == 0) {
					str[new_str_len] = '0';
					new_str_len += 1;
				}
				while (arg_aux != 0) {
					arg_aux /= 10;
					aux_len += 1;
				}
				for (s32 i = aux_len - 1; i >= 0; i--) {
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
	str[new_str_len] = 0;

	return (String) {
		.str    = str,
		.length = new_str_len,
	};
}

// Print the 8bit string to stdout.
static void string_print(const String str) {
#ifdef __unix
	write(1, str.str, str.length);
#else
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	WriteFile(handle, str.str, str.length, NULL, NULL);
#endif
}

// Print the 8bit string to stderr.
static void string_eprint(const String str) {
#ifdef __unix
	write(2, str.str, str.length);
#else
	HANDLE handle = GetStdHandle(STD_ERROR_HANDLE);
	WriteFile(handle, str.str, str.length, NULL, NULL);
#endif
}

// Allocate a new `String` resulting from the concatenation of `str1` and `str2`.
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

// Create a new `String` slice, range not inclusive.
static String string_slice(const String str, const u64 init, const u64 end) {
	return (String) {
		.str    = str.str + init,
		.length = end - init,
	};
}

// Create a new `String` slice from `init` to the end.
#define str_slice_end(str, init)  string_slice(str, init, str.length)
// Create a new `String` slice from the beginning to `end`.
#define str_slice_until(str, end) string_slice(str, 0, end)

// Set the given `String` to upper case.
static void string_upper(String str) {
	for (u64 i = 0; i < str.length; i++) {
		u8* aux = str.str + i;
		if (*aux >= 'a' && *aux <= 'z') *aux -= 32;
	}
}

// Set the given `String` to lower case.
static void string_lower(String str) {
	for (u64 i = 0; i < str.length; i++) {
		u8* aux = str.str + i;
		if (*aux >= 'A' && *aux <= 'Z') *aux += 32;
	}
}

// Allocate a new uppercase `String` based on `str`.
static String string_upper_new(Arena* arena, const String str) {
	u8* new_str = push_array(arena, u8, str.length);
	for (u64 i = 0; i < str.length; i++) {
		u8* aux = str.str + i;
		if (*aux >= 'a' && *aux <= 'z') new_str[i] = (*aux) - 32;
		else new_str[i] = *aux;
	}

	return (String) {
		.str    = new_str,
		.length = str.length,
	};
}

// Allocate a new lowercase `String` based on `str`.
static String string_lower_new(Arena* arena, const String str) {
	u8* new_str = push_array(arena, u8, str.length);
	for (u64 i = 0; i < str.length; i++) {
		u8* aux = str.str + i;
		if (*aux >= 'A' && *aux <= 'Z') new_str[i] = (*aux) + 32;
		else new_str[i] = *aux;
	}

	return (String) {
		.str    = new_str,
		.length = str.length,
	};
}

// Check the equality of the two `String`s.
static bool string_equals(const String str1, const String str2) {
	if (str1.length != str2.length) return false;
	for (u64 i = 0; i < str1.length; i++) {
		if (str1.str[i] != str2.str[i]) return false;
	}
	return true;
}

// Convert `String` to `s8`.
static s8 string_to_s8(const String str) {
	s8 result = 0;
	s8 negative = 0;
	for (u64 i = 0; i < str.length; i++) {
		while (str.str[i] == ' ') {
			i++;
		}
		if (str.str[i] == '-') {
			negative = -1;
			i++;
		}
		while (str.str[i] >= '0' && str.str[i] <= '9') {
			result *= 10;
			result += str.str[i] - '0';
			i++;
		}
	}

	return result * negative;
}

// Convert `String` to `s16`.
static s16 string_to_s16(const String str) {
	s16 result = 0;
	s16 negative = 0;
	for (u64 i = 0; i < str.length; i++) {
		while (str.str[i] == ' ') {
			i++;
		}
		if (str.str[i] == '-') {
			negative = -1;
			i++;
		}
		while (str.str[i] >= '0' && str.str[i] <= '9') {
			result *= 10;
			result += str.str[i] - '0';
			i++;
		}
	}

	return result * negative;
}

// Convert `String` to `s32`.
static s32 string_to_s32(const String str) {
	s32 result = 0;
	s32 negative = 0;
	for (u64 i = 0; i < str.length; i++) {
		while (str.str[i] == ' ') {
			i++;
		}
		if (str.str[i] == '-') {
			negative = -1;
			i++;
		}
		while (str.str[i] >= '0' && str.str[i] <= '9') {
			result *= 10;
			result += str.str[i] - '0';
			i++;
		}
	}

	return result * negative;
}

// Convert `String` to `s64`.
static s64 string_to_s64(const String str) {
	s64 result = 0;
	s64 negative = 0;
	for (u64 i = 0; i < str.length; i++) {
		while (str.str[i] == ' ') i++;
		if (str.str[i] == '-') {
			negative = -1;
			i++;
		}
		while (str.str[i] >= '0' && str.str[i] <= '9') {
			result *= 10;
			result += str.str[i] - '0';
			i++;
		}
	}

	return result * negative;
}

// Convert `String` to `u8`.
static u8 string_to_u8(const String str) {
	u8 result = 0;
	for (u64 i = 0; i < str.length; i++) {
		while (str.str[i] == ' ') i++;
		while (str.str[i] >= '0' && str.str[i] <= '9') {
			result *= 10;
			result += str.str[i] - '0';
			i++;
		}
	}

	return result;
}

// Convert `String` to `u16`.
static u16 string_to_u16(const String str) {
	u16 result = 0;
	for (u64 i = 0; i < str.length; i++) {
		while (str.str[i] == ' ') i++;
		while (str.str[i] >= '0' && str.str[i] <= '9') {
			result *= 10;
			result += str.str[i] - '0';
			i++;
		}
	}

	return result;
}

// Convert `String` to `u32`.
static u32 string_to_u32(const String str) {
	u32 result = 0;
	for (u64 i = 0; i < str.length; i++) {
		while (str.str[i] == ' ') i++;
		while (str.str[i] >= '0' && str.str[i] <= '9') {
			result *= 10;
			result += str.str[i] - '0';
			i++;
		}
	}

	return result;
}

// Convert `String` to `u64`.
static u64 string_to_u64(const String str) {
	u64 result = 0;
	for (u64 i = 0; i < str.length; i++) {
		while (str.str[i] == ' ') i++;
		while (str.str[i] >= '0' && str.str[i] <= '9') {
			result *= 10;
			result += str.str[i] - '0';
			i++;
		}
	}

	return result;
}

// TODO(sampie): fix this shit
#if 0
static f32 string_to_f32(const String str) {
	f32 result = 0;
	f32 decimal = 0;
	s32 aux = 0;
	bool negative = false;
	for (u64 i = 0; i < str.length; i++) {
		while (str.str[i] == ' ') i++;
		if (str.str[i] == '-') negative = true;
		while ((str.str[i] >= '0' && str.str[i] <= '9') && str.str[i] != '.') {
			aux *= 10;
			aux += str.str[i] - '0';
			i++;
		}
		if (str.str[i] == '.') i++;
		else break;

		u64 decimal_length;
		while (str.str[i] >= '0' && str.str[i] <= '9') {
			decimal_length += 1;
		}
	}

	result = aux;
	result += decimal;
	return negative ? -result : result;
}

static f64 string_to_f64(const String str) {
	f64 result = 0;
	f64 decimal = 0;
	bool negative = false;
	for (u64 i = 0; i < str.length; i++) {
		while (str.str[i] == ' ') i++;
		if (str.str[i] == '-') negative = true;
		while ((str.str[i] >= '0' && str.str[i] <= '9') && str.str[i] != '.') {
			result *= 10;
			result += str.str[i] - '0';
			i++;
		}
		if (str.str[i] == '.') i++;
		else break;
		
		while (str.str[i] >= '0' && str.str[i] <= '9') {
			decimal /= 10;
			decimal += (f64)(str.str[i] - '0') / 10;
			i++;
		}
	}

	result += decimal;
	return negative ? -result : result;
}
#endif

/* typedef struct { */
/* 	u16* str; */
/* 	u64  length; */
/* } String16; */

/* static String16 string16_new(u16* str) { */
/* 	u64 length = 0; */

/* 	while (str[length]) length += 1; */

/* 	return (String16) { */
/* 		.str    = str, */
/* 		.length = length, */
/* 	}; */
/* } */

/* #define make_string16(str) string16_new((u16*)str) */

#ifdef __cplusplus
}
#endif
