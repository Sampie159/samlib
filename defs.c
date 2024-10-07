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

String string_init(u8* buffer) {
    return (String) {
        .buffer = buffer,
        .length = 0,
    };
}

void string_write_str(String* str, const char* s) {
    u64 length = 0;
    while (s[length]) length += 1;
    memcpy(str->buffer + str->length, s, length);
    str->length += length;
}

void string_write_s8(String* str, s8 val) {
    u64 length = 0;
    b8 neg = (val < 0);
    if (neg) {
        str->buffer[str->length + length] = '-';
        str->length += 1;
        val = -val;
    }
    if (val == 0) {
        str->buffer[str->length + length] = '0';
        str->length += 1;
    }
    s8 aux_val = val;
    while (aux_val != 0) {
        aux_val /= 10;
        length += 1;
    }
    for (s32 i = length - 1; i >= 0; i--) {
        str->buffer[str->length + i] = '0' + (val % 10);
        val /= 10;
    }
    str->length += length;
}

void string_write_s16(String* str, s16 val) {
    u64 length = 0;
    b8 neg = (val < 0);
    if (neg) {
        str->buffer[str->length + length] = '-';
        str->length += 1;
        val = -val;
    }
    if (val == 0) {
        str->buffer[str->length + length] = '0';
        str->length += 1;
    }
    s16 aux_val = val;
    while (aux_val != 0) {
        aux_val /= 10;
        length += 1;
    }
    for (s32 i = length - 1; i >= 0; i--) {
        str->buffer[str->length + i] = '0' + (val % 10);
        val /= 10;
    }
    str->length += length;
}

void string_write_s32(String* str, s32 val) {
    u64 length = 0;
    b8 neg = (val < 0);
    if (neg) {
        str->buffer[str->length + length] = '-';
        str->length += 1;
        val = -val;
    }
    if (val == 0) {
        str->buffer[str->length + length] = '0';
        str->length += 1;
    }
    s32 aux_val = val;
    while (aux_val != 0) {
        aux_val /= 10;
        length += 1;
    }
    for (s32 i = length - 1; i >= 0; i--) {
        str->buffer[str->length + i] = '0' + (val % 10);
        val /= 10;
    }
    str->length += length;
}

void string_write_s64(String* str, s64 val) {
    u64 length = 0;
    b8 neg = (val < 0);
    if (neg) {
        str->buffer[str->length + length] = '-';
        str->length += 1;
        val = -val;
    }
    if (val == 0) {
        str->buffer[str->length + length] = '0';
        str->length += 1;
    }
    s64 aux_val = val;
    while (aux_val != 0) {
        aux_val /= 10;
        length += 1;
    }
    for (s32 i = length - 1; i >= 0; i--) {
        str->buffer[str->length + i] = '0' + (val % 10);
        val /= 10;
    }
    str->length += length;
}

void string_write_u16(String* str, u16 val) {
    u64 length = 0;
    if (val == 0) {
        str->buffer[str->length + length] = '0';
        str->length += 1;
    }
    u16 aux_val = val;
    while (aux_val != 0) {
        aux_val /= 10;
        length += 1;
    }
    for (s32 i = length - 1; i >= 0; i--) {
        str->buffer[str->length + i] = '0' + (val % 10);
        val /= 10;
    }
    str->length += length;
}

void string_write_u32(String* str, u32 val) {
    u64 length = 0;
    if (val == 0) {
        str->buffer[str->length + length] = '0';
        str->length += 1;
    }
    u32 aux_val = val;
    while (aux_val != 0) {
        aux_val /= 10;
        length += 1;
    }
    for (s32 i = length - 1; i >= 0; i--) {
        str->buffer[str->length + i] = '0' + (val % 10);
        val /= 10;
    }
    str->length += length;
}

void string_write_u64(String* str, u64 val) {
    u64 length = 0;
    if (val == 0) {
        str->buffer[str->length + length] = '0';
        str->length += 1;
    }
    u64 aux_val = val;
    while (aux_val != 0) {
        aux_val /= 10;
        length += 1;
    }
    for (s32 i = length - 1; i >= 0; i--) {
        str->buffer[str->length + i] = '0' + (val % 10);
        val /= 10;
    }
    str->length += length;
}

void string_write_f32(String* str, f32 val) {
    u64 length = 0;
    s32 whole = (s32)val;
    val -= whole;
    b8 neg = (whole < 0);
    if (neg) {
        str->buffer[str->length] = '-';
        str->length += 1;
        whole = -whole;
    }
    if (whole == 0) {
        str->buffer[str->length] = '0';
        str->length += 1;
    }
    s32 whole_aux = whole;
    while (whole_aux != 0) {
        whole_aux /= 10;
        length += 1;
    }
    for (s32 i = length - 1; i >= 0; i--) {
        str->buffer[str->length + i] = '0' + (whole % 10);
        whole /= 10;
    }
    str->length += length;
    str->buffer[str->length] = '.';
    str->length += 1;
    for (s8 prec = 0; prec < 12; prec++) {
        val *= 10;
        s32 digit = (s32)val;
        str->buffer[str->length] = '0' + digit;
        str->length += 1;
        val -= digit;
    }
}


void string_write_f64(String* str, f64 val) {
    u64 length = 0;
    s64 whole = (s64)val;
    val -= whole;
    b8 neg = (whole < 0);
    if (neg) {
        str->buffer[str->length] = '-';
        str->length += 1;
        whole = -whole;
    }
    if (whole == 0) {
        str->buffer[str->length] = '0';
        str->length += 1;
    }
    s64 whole_aux = whole;
    while (whole_aux != 0) {
        whole_aux /= 10;
        length += 1;
    }
    for (s32 i = length - 1; i >= 0; i--) {
        str->buffer[str->length + i] = '0' + (whole % 10);
        whole /= 10;
    }
    str->length += length;
    str->buffer[str->length] = '.';
    str->length += 1;
    for (s8 prec = 0; prec < 12; prec++) {
        val *= 10;
        s32 digit = (s32)val;
        str->buffer[str->length] = '0' + digit;
        str->length += 1;
        val -= digit;
    }
}

void string_write_ptr(String* str, const void* ptr) {
    u64 val = (u64)ptr;
    u64 length = 0;
    if (val == 0) {
        str->buffer[str->length + length] = '0';
        str->length += 1;
    }
    u64 aux_val = val;
    while (aux_val != 0) {
        aux_val /= 10;
        length += 1;
    }
    for (s32 i = length - 1; i >= 0; i--) {
        str->buffer[str->length + i] = '0' + (val % 10);
        val /= 10;
    }
    str->length += length;
}

void string_newline(String* str) {
    str->buffer[str->length] = '\n';
    str->length += 1;
}

void string_print(const String str) {
#ifdef __unix
	write(1, str.buffer, str.length);
#else
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	WriteFile(handle, str.buffer, str.length, NULL, NULL);
#endif
}

void string_println(const String str) {
#ifdef __unix
    write(1, str.buffer, str.length);
    write(1, "\n", 1);
#else
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    WriteFile(handle, str.buffer, str.length, NULL, NULL);
    WriteFile(handle, "\n", 1, NULL, NULL);
#endif
}

void string_eprint(const String str) {
#ifdef __unix
	write(2, str.buffer, str.length);
#else
	HANDLE handle = GetStdHandle(STD_ERROR_HANDLE);
	WriteFile(handle, str.buffer, str.length, NULL, NULL);
#endif
}

void string_eprintln(const String str) {
#ifdef __unix
    write(2, str.buffer, str.length);
    write(2, "\n", 1);
#else
    HANDLE handle = GetStdHandle(STD_ERROR_HANDLE);
    WriteFile(handle, str.buffer, str.length, NULL, NULL);
    WriteFile(handle, "\n", 1, NULL, NULL);
#endif
}


String string_concat(Arena* arena, const String str1, const String str2) {
	u64 new_length = str1.length + str2.length;
	u8* new_str = push_array(arena, u8, new_length);
	memcpy(new_str, str1.buffer, str1.length);
	memcpy(new_str + str1.length, str2.buffer, str2.length);

	return (String) {
		.buffer = new_str,
		.length = new_length,
	};
}

String string_slice(const String str, const u64 init, const u64 end) {
	return (String) {
		.buffer = str.buffer + init,
		.length = end - init,
	};
}

void string_upper(String str) {
	for (u64 i = 0; i < str.length; i++) {
		u8* aux = str.buffer + i;
		if (*aux >= 'a' && *aux <= 'z') *aux -= 32;
	}
}

String string_upper_new(Arena* arena, const String str) {
	u8* new_str = push_array(arena, u8, str.length);
	for (u64 i = 0; i < str.length; i++) {
		u8* aux = str.buffer + i;
		if (*aux >= 'a' && *aux <= 'z') new_str[i] = (*aux) - 32;
		else new_str[i] = *aux;
	}

	return (String) {
		.buffer = new_str,
		.length = str.length,
	};
}

void string_lower(String str) {
    for (u64 i = 0; i < str.length; i++) {
		u8* aux = str.buffer + i;
		if (*aux >= 'A' && *aux <= 'Z') *aux += 32;
	}
}

String string_lower_new(Arena* arena, const String str) {
    u8* new_str = push_array(arena, u8, str.length);
	for (u64 i = 0; i < str.length; i++) {
		u8* aux = str.buffer + i;
		if (*aux >= 'A' && *aux <= 'Z') new_str[i] = (*aux) + 32;
		else new_str[i] = *aux;
	}

	return (String) {
		.buffer    = new_str,
		.length = str.length,
	};
}

b8 string_equals(const String str1, const String str2) {
    if (str1.length != str2.length) return false;
	for (u64 i = 0; i < str1.length; i++) {
		if (str1.buffer[i] != str2.buffer[i]) return false;
	}
	return true;
}

b8 string_cmp(const String str1, const char* str2) {
    if (str2[str1.length] != 0) return false;
    for (u64 i = 0; i < str1.length; i++) {
        if (str1.buffer[i] != str2[i]) return false;
    }
    return true;
}

s8 string_to_s8(const String str) {
    s8 result = 0;
	s8 negative = 0;
	for (u64 i = 0; i < str.length; i++) {
		while (str.buffer[i] == ' ') {
			i++;
		}
		if (str.buffer[i] == '-') {
			negative = -1;
			i++;
		}
		while (str.buffer[i] >= '0' && str.buffer[i] <= '9') {
			result *= 10;
			result += str.buffer[i] - '0';
			i++;
		}
	}

	return result * negative;
}

s16 string_to_s16(const String str) {
    s16 result = 0;
	s16 negative = 0;
	for (u64 i = 0; i < str.length; i++) {
		while (str.buffer[i] == ' ') {
			i++;
		}
		if (str.buffer[i] == '-') {
			negative = -1;
			i++;
		}
		while (str.buffer[i] >= '0' && str.buffer[i] <= '9') {
			result *= 10;
			result += str.buffer[i] - '0';
			i++;
		}
	}

	return result * negative;
}

s32 string_to_s32(const String str) {
    s32 result = 0;
	s32 negative = 0;
	for (u64 i = 0; i < str.length; i++) {
		while (str.buffer[i] == ' ') {
			i++;
		}
		if (str.buffer[i] == '-') {
			negative = -1;
			i++;
		}
		while (str.buffer[i] >= '0' && str.buffer[i] <= '9') {
			result *= 10;
			result += str.buffer[i] - '0';
			i++;
		}
	}

	return result * negative;
}

s64 string_to_s64(const String str) {
    s64 result = 0;
	s64 negative = 0;
	for (u64 i = 0; i < str.length; i++) {
		while (str.buffer[i] == ' ') i++;
		if (str.buffer[i] == '-') {
			negative = -1;
			i++;
		}
		while (str.buffer[i] >= '0' && str.buffer[i] <= '9') {
			result *= 10;
			result += str.buffer[i] - '0';
			i++;
		}
	}

	return result * negative;
}

u8 string_to_u8(const String str) {
    u8 result = 0;
	for (u64 i = 0; i < str.length; i++) {
		while (str.buffer[i] == ' ') i++;
		while (str.buffer[i] >= '0' && str.buffer[i] <= '9') {
			result *= 10;
			result += str.buffer[i] - '0';
			i++;
		}
	}

	return result;
}

u16 string_to_u16(const String str) {
    u16 result = 0;
	for (u64 i = 0; i < str.length; i++) {
		while (str.buffer[i] == ' ') i++;
		while (str.buffer[i] >= '0' && str.buffer[i] <= '9') {
			result *= 10;
			result += str.buffer[i] - '0';
			i++;
		}
	}

	return result;
}

u32 string_to_u32(const String str) {
    u32 result = 0;
	for (u64 i = 0; i < str.length; i++) {
		while (str.buffer[i] == ' ') i++;
		while (str.buffer[i] >= '0' && str.buffer[i] <= '9') {
			result *= 10;
			result += str.buffer[i] - '0';
			i++;
		}
	}

	return result;
}

u64 string_to_u64(const String str) {
    u64 result = 0;
	for (u64 i = 0; i < str.length; i++) {
		while (str.buffer[i] == ' ') i++;
		while (str.buffer[i] >= '0' && str.buffer[i] <= '9') {
			result *= 10;
			result += str.buffer[i] - '0';
			i++;
		}
	}

	return result;
}
