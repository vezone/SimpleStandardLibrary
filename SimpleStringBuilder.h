#ifndef STRING_BUILDER_H
#define STRING_BUILDER_H

#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef TYPES_H
#define TYPES_H

#if defined(WIN32) || defined(_WIN32)
 #define force_inline static
#else
 #define force_inline static inline __attribute((always_inline))
#endif

#include <stdint.h>
#include <stddef.h>
#include <assert.h>

#ifndef vassert
#define vassert(a) assert(a)
#endif

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef float f32;
typedef double f64;
#define i8(x)  ((i8)(x))
#define i16(x) ((i16)(x))
#define i32(x) ((i32)(x))
#define i64(x) ((i64)(x))
#define u8(x)  ((u8)(x))
#define u16(x) ((u16)(x))
#define u32(x) ((u32)(x))
#define u64(x) ((u64)(x))
#define f32(x) ((f32)(x))
#define f64(x) ((f64)(x))

#define void_to_i32(ptr) (*((i32*)ptr))
#define void_to_f32(ptr) (*((f32*)ptr))
#define void_to_string(ptr) ((const char*)ptr)
#define i32_to_void_ptr(i32v) ((void*)(&(i32v)))

#define I8_MAX 127
#define I16_MAX 32767
#define I32_MAX 2147483647
#define I32_MAX_HALF 1123241323
#define I64_MAX 9223372036854775807
#define U8_MAX 255
#define U16_MAX 65535
#define U32_MAX 4294967295
#define U64_MAX 18446744073709551615

#define TYPE_REVERSE(x) (x = !(x))
#define OffsetOf(Type, Field) ( (size_t) (&(((Type*)0)->Field)) )
#define AlignmentOf(Type) ((u64)&(((struct{char c; Type i;}*)0)->i))
#define ARRAY_LENGTH(x) (sizeof(x) / sizeof(x[0]))
#define ARRAY_COUNT(x) (sizeof(x) / sizeof(x[0]))
#define TO_STRING(x) #x
#define FMOD(x, v) (f32) (((i32)(x)) % v + ((x) - (i32)(x)))
#define TO_STRING(x) #x
#define FMOD(x, v) (f32) (((i32)(x)) % v + ((x) - (i32)(x)))
#define KB(x) ((i64)1024 * (i64)x)
#define MB(x) ((i64)1024 * KB(x))
#define GB(x) ((i64)1024 * MB(x))
#define nullptr ((void*) 0)
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) > (y)) ? (y) : (x))
#define TOKB(x) (((f64) x) / 1024)
#define TOMB(x) (((f64) TOKB(x)) / 1024)
#define TOGB(x) (((f64) TOMB(x)) / 1024)
#define ABS(x) ((x > 0) ? x : -x)
#define DEFAULT(type) ((type) { })

#ifndef IS_NULL_OFFSET
 #define IS_NULL_OFFSET(ptr)						\
     ({									\
	vassert(sizeof(ptr) == 8 && "IS_NULL_OFFSET takes pointer as parameter!"); \
	u64 address = (u64)ptr;						\
	address < 10000 ? 1 : 0;					\
     })
#endif

#define DO_ONES(func, msg) { static i8 flag = 1; if (flag) { flag = 0; func(msg); } }
#define DO_ONESF(func, format, ...) { static i8 flag = 1; if (flag) { flag = 0; func(format, ##__VA_ARGS__); } }
#define DO_ONES3(func, msg, out) { static i8 flag = 1; if (flag) { flag = 0; func(msg, out); } }

#endif

#ifndef memory_allocate
 #define StringBuilderAllocateDelegate(size) malloc(size)
 #define StringBuilderFreeDelegate(data) free(data)
#else
 #define StringBuilderAllocateDelegate(size) memory_allocate(size)
 #define StringBuilderFreeDelegate(data) memory_free(data)
#endif

force_inline void
sb_string_set(char* string, char c, u32 length)
{
    i32 i;
    for (i = 0; i < length; ++i)
    {
	string[i] = c;
    }
}

force_inline i32
sb_string_number_rank(i32 number)
{
    i32 rank = 0;
    for (; ;)
    {
	number /= 10;
	if (number != 0)
	{
	    ++rank;
	}
	else
	{
	    return rank;
	}
    }
}

force_inline i32
sb_string_number_of_rank(i32 number, i32 rank)
{
    if (rank <= 0)
    {
	return number;
    }

    for (i32 i = 0; i < rank; i++)
    {
	number *= 10;
    }

    return number;
}

force_inline i32
sb_string_number_of_digit(i64 number, i8 digit)
{
    i32 i;

    if (sb_string_number_rank(number) < digit)
    {
	return 0;
    }

    if (number < 0)
    {
	number *= -1;
    }

    if (digit == 0)
    {
	return (number % 10);
    }

    number %= sb_string_number_of_rank(1, (digit + 1));
    number /= sb_string_number_of_rank(1, digit);

    return number;
}

#define sb_string_int_to_string(input, number)				\
    ({									\
	i8 isNumberNegative = ((number < 0) ? 1 : 0);			\
	i32 i, rank = sb_string_number_rank(number), numberLength = rank + isNumberNegative + 1; \
									\
	if (isNumberNegative)						\
	{								\
	    input[0] = '-';						\
	}								\
									\
	for (i = isNumberNegative; i < numberLength; ++i)		\
	{								\
	    input[i] = sb_string_number_of_digit(number, rank) + 48;	\
	    --rank;							\
	}								\
    })

#define sb_string_i32_to_string(input, number) sb_string_int_to_string(input, number)
#define sb_string_i64_to_string(input, number) sb_string_int_to_string(input, number)
#define sb_string_f64_to_string(input, number) sprintf(input, "%f", number)

typedef struct StringBuilderHeader
{
    i32 Count;
    i32 Capacity;
    char* Buffer;
} StringBuilderHeader;

#define START_ALLOCATION_SIZE 253

#define string_builder_header(s) ((StringBuilderHeader*) (((u8*)s) - sizeof(StringBuilderHeader)))
#define string_builder_count(s) ((s) != NULL ? string_builder_header((s))->Count : 0)
#define string_builder_capacity(s) ((s) != NULL ? string_builder_header((s))->Capacity : 0)
#define string_builder_buffer(s) ((s) != NULL ? string_builder_header((s))->Buffer : NULL)
#define string_builder_free(s) StringBuilderFreeDelegate(string_builder_header((s)))

#define string_builder_appendc(s, c)					\
    ({									\
	string_builder_append_base((s), 1);				\
	StringBuilderHeader* header = string_builder_header((s));	\
	header->Buffer[header->Count] = (c);				\
	++header->Count;						\
    })
#define string_builder_appends(s, str)					\
    ({									\
	i32 strLength = strlen((str));					\
	string_builder_append_base((s), strLength);			\
	StringBuilderHeader* header = string_builder_header((s));	\
	memcpy((header->Buffer + header->Count), (str), strLength*sizeof(*(s))); \
	header->Count += strLength;					\
    })
#define string_builder_appendf(s, f, ...)				\
    ({									\
	(s) = internal_string_builder_appendf((s), (f), ##__VA_ARGS__);	\
    })
#define string_builder_append_base(s, count)				\
    {									\
	i32 prevCapacity = string_builder_capacity((s));		\
	i32 doubledCount = (2 * (count) + 1);				\
									\
	if ((s) == NULL)						\
	{								\
	    i32 startAllocationSize = START_ALLOCATION_SIZE;		\
	    i32 newSize = (((doubledCount) > startAllocationSize) ? (doubledCount) : startAllocationSize); \
	    (s) = internal_string_builder_grow((s), newSize);		\
	}								\
	else if	((string_builder_count((s)) + doubledCount) >= prevCapacity) \
	{								\
	    i32 newCapacity = 2 * prevCapacity + 1;			\
	    i32 newSize = (((doubledCount) > newCapacity) ? doubledCount : newCapacity); \
	    (s) = internal_string_builder_grow((s), newSize);		\
	}								\
    }

force_inline char*
internal_string_builder_grow(char* builder, i32 newCount)
{
    i32 headerSize = sizeof(StringBuilderHeader);
    i32 bufferSize = newCount * sizeof(*builder);
    i32 newSize = bufferSize + headerSize;
    char* buffer =  NULL;

    if (builder == NULL)
    {
	StringBuilderHeader* header = (StringBuilderHeader*) StringBuilderAllocateDelegate(newSize);
	header->Count = 0;
	header->Capacity = newCount;
	header->Buffer = (char*) (((char*)header) + headerSize);
	memset(header->Buffer, '\0', bufferSize);

	buffer = header->Buffer;
    }
    else
    {
	StringBuilderHeader* header = string_builder_header(builder);
	StringBuilderHeader* newHeader = (StringBuilderHeader*) StringBuilderAllocateDelegate(newSize);
	size_t prevSize = header->Count * sizeof(*builder);
	newHeader->Capacity = newCount;
	newHeader->Count = header->Count;
	newHeader->Buffer = (char*) (((char*)newHeader) + headerSize);
	memset(newHeader->Buffer, '\0', bufferSize);
	memcpy(newHeader->Buffer, header->Buffer, prevSize);

	StringBuilderFreeDelegate(header);

	buffer = newHeader->Buffer;
    }

    return buffer;
}

static char*
internal_string_builder_appendf(char* builder, const char* format, ...)
{
    i32 state = 0;
    i32 argumentsCount = 0;
    va_list valist;

    for (char* ptr = (char*)format; *ptr != '\0'; ptr++)
    {
	char c = *ptr;
	if (c == '%')
	{
	    state = 1;
	}
	else if (c == 'c' || c == 's' || c == 'd' || c == 'b' || c == 'f')
	{
	    state = 0;
	    ++argumentsCount;
	}
	else
	{
	    state = 0;
	}
    }

    assert(argumentsCount > 0);

    state = 0;
    va_start(valist, format);
    while (*format != '\0')
    {
	char f = *format;

	switch (f)
	{
	case '%':
	{
	    state = 1;
	    break;
	}
	case 'c':
	{
	    if (state == 1)
	    {
		char elementc = (char) va_arg(valist, i32);
		string_builder_appendc(builder, elementc);

		state = 0;
	    }
	    else
	    {
		string_builder_appendc(builder, f);
	    }

	    break;
	}
	case 's':
	{
	    if (state == 1)
	    {
		const char* elements = va_arg(valist, const char *);
		string_builder_appends(builder, (char*) elements);

		state = 0;
	    }
	    else
	    {
		string_builder_appendc(builder, f);
	    }

	    break;
	}
	case 'l':
	{
	    if (state == 1)
	    {
		state = 2;
	    }
	    else
	    {
		string_builder_appendc(builder, f);
	    }

	    break;
	}
	case 'd':
	{
	    if (state == 1)
	    {
		i32 number = va_arg(valist, i32);
		char str[64];
		sb_string_set(str, '\0', 64);
		sb_string_i32_to_string(str, number);
		string_builder_appends(builder, str);

		state = 0;
	    }
	    else if (state == 2)
	    {
		i64 number = va_arg(valist, i64);
		char str[64];
		sb_string_set(str, '\0', 64);
		sb_string_i64_to_string(str, number);
		string_builder_appends(builder, str);

		state = 0;
	    }
	    else
	    {
		string_builder_appendc(builder, f);
	    }

	    break;
	}
	case 'b':
	{
	    if (state == 1)
	    {
		u8 number = (u8) va_arg(valist, i32);
		char str[4];
		sb_string_set(str, '\0', 4);
		sb_string_i32_to_string(str, number);
		string_builder_appends(builder, str);

		state = 0;
	    }
	    else
	    {
		string_builder_appendc(builder, f);
	    }

	    break;
	}
	case 'f':
	{
	    if (state == 1)
	    {
		f64 f64Number = va_arg(valist, f64);
		char str[64];
		sb_string_set(str, '\0', 64);
		sb_string_f64_to_string(str, f64Number);
		string_builder_appends(builder, str);

		state = 0;
	    }
	    else
	    {
		string_builder_appendc(builder, f);
	    }

	    break;
	}
	default:
	{
	    string_builder_appendc(builder, f);
	    state = 0;

	    break;
	}

	}

	format++;
    }

    va_end(valist);

    return builder;
}

#endif
