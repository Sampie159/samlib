#include "defs.h"

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

Arena arena_new(u64 cap) {
	if (cap < KB(4)) cap = KB(4);
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

void* arena_alloc(Arena* a, u64 size) {
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

void* arena_pop(Arena* arena, u64 size) {
    arena->pos -= size;
    return (u8*)arena->buffer + arena->pos;
}

void arena_reset(Arena* a) { a->pos = 0; }

void arena_clear(Arena* a) {
	memset(a->buffer, 0, a->pos);
	a->pos = 0;
}

void arena_free(Arena* a) {
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

ArenaTemp arena_temp_begin(Arena* arena) {
	return (ArenaTemp) {
		.arena = arena,
		.pos   = arena->pos,
	};
}

void arena_temp_end(ArenaTemp temp) {
	temp.arena->pos = temp.pos;
}

String string_new(Arena* arena, const u8* str) {
	u64 length = 0;

	while (str[length]) length += 1;
	u8* new_str = push_array(arena, u8, length);
	memcpy(new_str, str, length);

	return (String) {
		.str    = new_str,
		.length = length,
	};
}

String string_format(Arena* arena, const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);

	ArenaTemp temp = arena_temp_begin(arena);
	u8* str = (u8*)arena_alloc(arena, B(256));
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
                        b8 neg = (arg < 0);
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
                        b8 neg = (arg < 0);
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
                        b8 neg = (arg < 0);
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
                    b8 neg = (whole < 0);
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

void string_print(const String str) {
#ifdef __unix
	write(1, str.str, str.length);
#else
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	WriteFile(handle, str.str, str.length, NULL, NULL);
#endif
}

void string_println(const String str) {
#ifdef __unix
    write(1, str.str, str.length);
    write(1, "\n", 1);
#else
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    WriteFile(handle, str.str, str.length, NULL, NULL);
    WriteFile(handle, "\n", 1, NULL, NULL);
#endif
}

void string_eprint(const String str) {
#ifdef __unix
	write(2, str.str, str.length);
#else
	HANDLE handle = GetStdHandle(STD_ERROR_HANDLE);
	WriteFile(handle, str.str, str.length, NULL, NULL);
#endif
}

void string_eprintln(const String str) {
#ifdef __unix
    write(2, str.str, str.length);
    write(2, "\n", 1);
#else
    HANDLE handle = GetStdHandle(STD_ERROR_HANDLE);
    WriteFile(handle, str.str, str.length, NULL, NULL);
    WriteFile(handle, "\n", 1, NULL, NULL);
#endif
}


String string_concat(Arena* arena, const String str1, const String str2) {
	u64 new_length = str1.length + str2.length;
	u8* new_str = push_array(arena, u8, new_length);
	memcpy(new_str, str1.str, str1.length);
	memcpy(new_str + str1.length, str2.str, str2.length);

	return (String) {
		.str    = new_str,
		.length = new_length,
	};
}

String string_slice(const String str, const u64 init, const u64 end) {
	return (String) {
		.str    = str.str + init,
		.length = end - init,
	};
}

void string_upper(String str) {
	for (u64 i = 0; i < str.length; i++) {
		u8* aux = str.str + i;
		if (*aux >= 'a' && *aux <= 'z') *aux -= 32;
	}
}

String string_upper_new(Arena* arena, const String str) {
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

void string_lower(String str) {
    for (u64 i = 0; i < str.length; i++) {
		u8* aux = str.str + i;
		if (*aux >= 'A' && *aux <= 'Z') *aux += 32;
	}
}

String string_lower_new(Arena* arena, const String str) {
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

b8 string_equals(const String str1, const String str2) {
    if (str1.length != str2.length) return false;
	for (u64 i = 0; i < str1.length; i++) {
		if (str1.str[i] != str2.str[i]) return false;
	}
	return true;
}

b8 string_cmp(const String str1, const char* str2) {
    if (str2[str1.length] != 0) return false;
    for (u64 i = 0; i < str1.length; i++) {
        if (str1.str[i] != str2[i]) return false;
    }
    return true;
}

s8 string_to_s8(const String str) {
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

s16 string_to_s16(const String str) {
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

s32 string_to_s32(const String str) {
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

s64 string_to_s64(const String str) {
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

u8 string_to_u8(const String str) {
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

u16 string_to_u16(const String str) {
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

u32 string_to_u32(const String str) {
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

u64 string_to_u64(const String str) {
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
