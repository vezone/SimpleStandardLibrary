#ifndef SIMPLE_STANDARD_LIBRARY_H
#define SIMPLE_STANDARD_LIBRARY_H

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

/*
Content:
[Types.h]
[Environment.h]
[Logger.h]
[MemoryAllocator.h]
[Array.h]
[String.h]
[StringBuilder.h]
[HashTable.h]
[IO.h]
[Path.h]
[Profiler.h]
[JsonParser.h]


*/


/*

		 #####################################
		 #####################################
			       Types.h
		 #####################################
		 #####################################

*/
#ifndef TYPES_H
#define TYPES_H

#if defined(WIN32) || defined(_WIN32)
 #define force_inline static
 #ifndef WINDOWS_PLATFORM
  #define WINDOWS_PLATFORM
 #endif
#else
 #define force_inline static inline __attribute((always_inline))
 #ifndef LINUX_PLATFORM
  #define LINUX_PLATFORM
 #endif
#endif

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

#define DO_MANY_TIME(action, time) {		\
	static i32 count = 0;			\
	if (count < time){			\
	    action;				\
	    ++count;				\
	}}

#endif // Types.h

#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H
 #ifdef LINUX_PLATFORM
  #define NEW_LINE "\n"
  #define PATH_SEPARATOR '/'
  #define PATH_SEPARATOR_STRING "/"
  #define ROOT_DIRECTORY "/"
 #elif WINDOWS_PLATFORM
  #define NEW_LINE "\r\n"
  #define PATH_SEPARATOR "\\"
  #define PATH_SEPARATOR_STRING "\\"
  #define ROOT_DIRECTORY "C:\\"
 #endif
#endif // Environment.h

/*

		 #####################################
		 #####################################
		       Logger.h (CONSOLE LOGGER)
		 #####################################
		 #####################################

*/
#ifndef LOGGER_H
#define LOGGER_H

/*
  ==================
  ==================
  ==              ==
  ==    COLORS    ==
  ==              ==
  ==================
  ==================

  Name            BG  FG
  Black           30  40
  Red             31  41
  Green           32  42
  Yellow          33  43
  Blue            34  44
  Magenta         35  45
  Cyan            36  46
  White           37  47
  Bright Black    90  100
  Bright Red      91  101
  Bright Green    92  102
  Bright Yellow   93  103
  Bright Blue     94  104
  Bright Magenta  95  105
  Bright Cyan     96  106
  Bright White    97  107

  [a;b;cmTEXT\033[0m
  a - mode
  b - background color
  c - foreground color
  \033 - foreground & background

  examples:
  printf("\033[36mTexting\033[0m \n");

  //Background
  printf("\033[1;42;30mTexting\033[0m\t\t");
  printf("\033[3;44;30mTexting\033[0m\t\t");
  printf("\033[4;104;30mTexting\033[0m\t\t");
  printf("\033[5;100;30mTexting\033[0m\n");

  printf("\033[4;47;35mTexting\033[0m\t\t");
  printf("\033[3;47;35mTexting\033[0m\t\t");
  printf("\033[3;43;30mTexting\033[0m\t\t");
*/

#define BLACK(x) "\x1B[30m"x"\033[0m"
#define RED(x) "\x1B[31m"x"\033[0m"
#define GREEN(x) "\x1B[32m"x"\033[0m"
#define GREENQ(x) "\x1B[32m"x"\033[0m"
#define YELLOW(x) "\x1B[33m"x"\033[0m"
#define BLUE(x) "\x1B[34m"x"\033[0m"
#define MAGNETA(x) "\x1B[35m"x"\033[0m"
#define CYAN(x) "\x1B[36m"x"\033[0m"
#define WHITE(x) "\x1B[37m"x"\033[0m"

#define BRIGHTBLACK(x) "\x1B[90m"x"\033[0m"
#define BRIGHTRED(x) "\x1B[91m"x"\033[0m"
#define BRIGHTGREEN(x) "\x1B[92m"x"\033[0m"
#define BRIGHTYELLOW(x) "\x1B[93m"x"\033[0m"
#define BRIGHTBLUE(x) "\x1B[94m"x"\033[0m"
#define BRIGHTMAGNETA(x) "\x1B[95m"x"\033[0m"
#define BRIGHTCYAN(x) "\x1B[96m"x"\033[0m"
#define BRIGHTWHITE(x) "\x1B[97m"x"\033[0m"

#define RED5(x) "\033[5;1;31m"x"\033[0m"
#define REDBG5(x) "\033[5;101;30m"x"\033[0m"
#define GREEN5(x) "\033[5;1;32m"x"\033[0m"

#define GLOG(format, ...) printf("[LOG] file: %s, line: %d, message: ", __FILE__, __LINE__);printf(format, ##__VA_ARGS__)
#define GSUCCESS(format, ...) printf(GREENQ("[SUCCESS]")" file: %s, line: %d, message: ", __FILE__, __LINE__);printf(format, ##__VA_ARGS__)
#define GERROR(format, ...) printf(RED("[ERROR]")" file: %s, line: %d, message: ", __FILE__, __LINE__);printf(format, ##__VA_ARGS__)
#define GWARNING(format, ...) printf(YELLOW("[WARNING]")" file: %s, line: %d, message: ", __FILE__, __LINE__);printf(format, ##__VA_ARGS__)
#ifdef DEBUG
#define GDEBUG(format, ...)
#else
#define GDEBUG(format, ...) printf(BRIGHTGREEN("[DEBUG]")" file: %s, line: %d, message: ", __FILE__, __LINE__);printf(format, ##__VA_ARGS__)
#endif
#define GFORMAT(string, format, ...) sprintf(string, format, __VA_ARGS__)

#endif // Logger.h

/*

		 ###################################
		 ###################################
			   MemoryAllocator.h
		 ###################################
		 ###################################

*/
#ifndef MEMORY_ALLOCATOR_H
#define MEMORY_ALLOCATOR_H

typedef struct MemoryBlock
{
    struct MemoryBlock* Prev;
    struct MemoryBlock* Next;
    void* Address;
    const char* File;
    i32 Line;
    //NOTE(bies): this is size wo header
    //TODO: rename to AllocatedSize;
    u64 AllocatedSize;
} MemoryBlock;

typedef struct MemoryList
{
    i32 IsInitialized;
    i32 Size;
    i32 Count;
    MemoryBlock* Begin;
    MemoryBlock* End;
} MemoryList;

typedef enum PrintAllocationSourceType
{
    PRINT_ALLOCATION_SOURCE_TYPE_NONE = 0,
    PRINT_ALLOCATION_SOURCE_TYPE_TERMINAL
} PrintAllocationSourceType;

#ifdef memory_allocate
 #undef memory_allocate
 #undef memory_allocate_type
 #undef memory_free
 #undef memory_reallocate
#endif

#define memory_allocate(size) memory_helper_allocate(size, __LINE__, __FILE__)
#define memory_allocate_type(type) (type*)memory_helper_allocate(sizeof(type), __LINE__, __FILE__)
#define memory_free(data) memory_helper_free(data, __LINE__, __FILE__)
#define memory_reallocate(data, size) memory_helper_reallocate(data, size, __LINE__, __FILE__)

///                               ///
///     INTERNAL MEMORY LIST      ///
///                               ///

static void
print_address(const char* text, void* address)
{
    u64 addr = u64(address);
    if (addr)
    {
	u64 lowAddress = addr % 1000;
	char add[3];
	if (lowAddress < 100 && lowAddress >= 10)
	{
	    add[0] = '0';
	    add[1] = '\0';
	}
	else if (lowAddress < 10)
	{
	    add[0] = '0';
	    add[1] = '0';
	    add[2] = '\0';
	}
	else
	{
	    add[0] = '\0';
	    add[1] = ' ';
	    add[2] = ' ';
	}

	printf("%s: %lu"YELLOW("%s")YELLOW("%lu "), text, addr / 1000,  add, lowAddress);
    }
    else
    {
	printf("%s: "RED("      NULL      "), text);
    }
}

static void
block_create(MemoryBlock* block, void* address, u64 size, const char* file, i32 line)
{
    block->Address = address;
    block->AllocatedSize = size;
    block->File = file;
    block->Line = line;
}

static void
block_show(MemoryBlock* block)
{
    printf("Size: %lu ", block->AllocatedSize);
    print_address("Prev", (void*) block->Prev);
    print_address("Current", (void*) block);
    print_address("Next", (void*) block->Next);
    printf("\n");
}

static void
list_init(MemoryList* list)
{
    list->Size = 0;
    list->Count = 0;
    list->Begin = NULL;
    list->End = NULL;
}

static MemoryBlock*
list_find(MemoryList* list, void* data)
{
    MemoryBlock* block = list->Begin;
    for (i32 i = 0; i < list->Count; ++i)
    {
	if (block->Address == data)
	{
	    return block;
	}

	block = block->Next;
    }

    assert(0 && "Can't find block in allocation list!!!");
    return NULL;
}

static void
list_add(MemoryList* list, MemoryBlock* element)
{
    if (list->End == NULL)
    {
	list->Begin = element;
	list->End = element;
	element->Prev = NULL;
	element->Next = NULL;
    }
    else
    {
	// [prev:0 cur: 1 next:2] [prev: 1 cur: 2: next 0]
	list->End->Next = element;
	element->Prev = list->End;
	element->Next = NULL;

	list->End = element;
    }

    list->Size += element->AllocatedSize;
    list->Count++;
}

static void
list_add_in_pointer_order(MemoryList* list, MemoryBlock* element)
{
    if (list->Begin == NULL || list->End == NULL)
    {
	list->Begin = element;
	list->End = element;
    }
    else
    {
	for (MemoryBlock* ptr = list->Begin; ptr != NULL; ptr = ptr->Next)
	{
	    if (u64(ptr) > u64(element))
	    {
		if (ptr == list->Begin)
		{
		    MemoryBlock* begin = list->Begin;
		    list->Begin = element;
		    element->Prev = NULL;
		    element->Next = begin;
		}
		else
		{
		    MemoryBlock* prev = ptr->Prev;
		    prev->Next = element;
		    ptr->Prev = element;
		    element->Prev = prev;
		    element->Next = ptr;
		}
	    }
	    else if (u64(ptr) < u64(element) && ptr == list->End)
	    {
		ptr->Next = element;
		list->End = element;
		element->Prev = ptr;
	    }
	}
    }

    list->Size += element->AllocatedSize;
    list->Count++;
}

static void
list_remove(MemoryList* list, MemoryBlock* element)
{
    MemoryBlock* prev = element->Prev;
    MemoryBlock* next = element->Next;

    if (list->Count == 1)
    {
	list->Begin = NULL;
	list->End = NULL;
    }
    else
    {
	//begin
	if (element == list->Begin)
	{
	    element->Next->Prev = NULL;
	    list->Begin = element->Next;
	}
	//end
	else if (element == list->End)
	{
	    element->Prev->Next = NULL;
	    list->End = element->Prev;
	}
	//mid
	else
	{
	    element->Prev->Next = element->Next;
	    element->Next->Prev = element->Prev;
	}

	//delete links on neighboring elements
	element->Prev = NULL;
	element->Next = NULL;
    }

    list->Size -= element->AllocatedSize;
    list->Count--;
}

static void
list_clean(MemoryList* list)
{
    while (list->Begin != NULL)
    {
	block_show(list->Begin);
	list_remove(list, list->Begin);
    }

    list->Count = 0;
    list->Size = 0;
}

///                               ///
///         DYNAMIC ARRAY         ///
///                               ///
typedef struct MAArrayHeader
{
    i32 ElementSize;
    i32 Count;
    i32 Capacity;
    u8 Buffer[0];
} MAArrayHeader;

#define ma_array_header(b) ((MAArrayHeader*) (((u8*)b) - sizeof(MAArrayHeader)))
#define ma_array_len(b) ((b != NULL) ? ma_array_header(b)->Count : 0)
#define ma_array_count(b) ((b != NULL) ? ma_array_header(b)->Count : 0)
#define ma_array_cap(b) ((b != NULL) ? ma_array_header(b)->Capacity : 0)
#define ma_array_capacity(b) ((b != NULL) ? ma_array_header(b)->Capacity : 0)
#define ma_array_element_size(b) ((b != NULL) ? ma_array_header(b)->ElementSize : 0)

#define ma_array_push(b, ...)					\
    {								\
	if ((b) == NULL || ma_array_len(b) >= ma_array_cap(b))	\
	{							\
	    (b) = _ma_array_grow(b, sizeof(*b));		\
	}							\
	b[ma_array_len(b)] = (__VA_ARGS__);			\
	ma_array_header(b)->Count++;				\
    }

#define ma_array_free(b)			\
    {						\
	if ((b))				\
	{					\
	    free(ma_array_header(b));		\
	    (b) = NULL;				\
	}					\
    }
force_inline void*
_ma_array_grow(const void* array, i32 elementSize)
{
    MAArrayHeader* newHeader = NULL;

    if (array != NULL)
    {
	i32 newCapacity = 2 * ma_array_cap(array) + 1;
	i32 newSize = newCapacity * elementSize + sizeof(MAArrayHeader);
	MAArrayHeader* header = ma_array_header(array);
	newHeader = (MAArrayHeader*) realloc(header, newSize);
	newHeader->Capacity = newCapacity;
    }
    else
    {
	i32 size = 1 * elementSize + sizeof(MAArrayHeader);
	newHeader = (MAArrayHeader*) malloc(size);
	newHeader->Count = 0;
	newHeader->Capacity = 1;
	newHeader->ElementSize = elementSize;
    }

    return newHeader->Buffer;
}

///                               ///
///          HASH TABLE           ///
///                               ///

typedef struct MATableHeader
{
    i32 ElementSize;
    i32 Count;
    i32 Capacity;
    i32 Index;
    i32 NextPrime;
    u8 Buffer[0];
} MATableHeader;

#define ma_table_header(b) ((MATableHeader*) (((u8*)b) - sizeof(MATableHeader)))
#define ma_table_count(b) ((b != NULL) ? ma_table_header(b)->Count : 0)
#define ma_table_capacity(b) ((b != NULL) ? ma_table_header(b)->Capacity : 0)
#define ma_table_element_size(b) ((b != NULL) ? ma_table_header(b)->ElementSize : 0)
#define ma_table_next_prime(b) ((b != NULL) ? ma_table_header(b)->NextPrime : 0)
#define ma_table_free(b) ((b) ? free(ma_table_header(b)) : 0)

#define MA_PRIME_1 117
#define MA_PRIME_2 119

force_inline i32
ma_i32_pow(i32 x, i32 n)
{
    i32 result = 1;

    if (n == 0 || x == 1)
    {
	return 1;
    }

    while (n)
    {
	result *= x;
	--n;
    }

    return result;
}

force_inline i32
ma_string_length(const char* string)
{
    i32 i;
    i32 length;
    for (length = 0; string[length] != '\0'; ++length);
    return length;
}

force_inline i32
ma_string_compare_length(const char* left, const char* right, i32 length)
{
    assert(left);
    assert(right);

    i32 i;
    for (i = 0; i < length; ++i)
    {
	if (left[i] != right[i])
	{
	    return 0;
	}
    }

    return 1;
}

typedef struct MAInternalStringElement
{
    const char* Key;
    void* Data;
} MAInternalStringElement;

static i32 MAPrimes[] = {
    97, 193, 389,
    769, 1543, 3079, 6151,
    12289, 24593, 49157, 98317,
    196613, 393241, 786433, 1572869,
    3145739, 6291469, 12582917, 25165843,
    50331653, 100663319, 201326611, 402653189,
    805306457, 1610612741
};

force_inline i32
ma_get_prime(MATableHeader* header)
{
    i32 prime = MAPrimes[header->NextPrime];
    ++header->NextPrime;
    return prime;
}

force_inline void*
ma_table_grow(void* table, i32 elemSize, i32 isInt)
{
    MATableHeader* newHeader;
    i32 capacity;
    i32 count;

    if (table == NULL)
    {
	count = 53;
	capacity = count * elemSize;

	newHeader = (MATableHeader*) malloc(capacity + sizeof(MATableHeader));
    }
    else
    {
	MATableHeader* prevHeader = ma_table_header(table);
	count = ma_get_prime(prevHeader);
	capacity = count * elemSize;

	newHeader = (MATableHeader*) malloc(capacity + sizeof(MATableHeader));
	newHeader->NextPrime = prevHeader->NextPrime;
    }

    newHeader->Count = 0;
    newHeader->Capacity = count;
    newHeader->ElementSize = elemSize;

    GWARNING("MA_TABLE REALLOC: prevCount=%d newCapacity=%d!\n", ma_table_count(table), newHeader->Capacity);

    if (isInt)
    {
	memset(newHeader->Buffer, -1, capacity);
    }
    else
    {
	memset(newHeader->Buffer, 0, capacity);
    }

    return newHeader->Buffer;
}

force_inline i32
ma_shash(const char* key, i32 prime, i32 bucketNumber)
{
    assert(bucketNumber != 0);

    i32 i,
	shash = 0,
	keyLength = ma_string_length(key);

    for (i = 0; i < keyLength; i++)
    {
	shash += ma_i32_pow(prime, (keyLength - (i + 1))) * key[i];
	shash %= bucketNumber;
    }

    return shash;
}

force_inline i32
ma_get_shash(const char* key, i32 bucketNumber, i32 attempt)
{
    i32 hashA = ma_shash(key, MA_PRIME_1, bucketNumber);
    i32 hashB = ma_shash(key, MA_PRIME_2, bucketNumber);
    return (hashA + (attempt * (hashB + 1))) % bucketNumber;
}

static void*
_ma_shash_put(void* table, const char* key, i32 capacity, i32 elementSize)
{
    if (table == NULL)
    {
	return NULL;
    }

    MATableHeader* header = ma_table_header(table);

    i32 i = 0,
	keyLength = ma_string_length(key),
	index;
    MAInternalStringElement* elem;
    do
    {
	index = ma_get_shash(key, capacity, i);
	elem = (MAInternalStringElement*) (table + index * elementSize);
	if (ma_string_compare_length(elem->Key, key, keyLength))
	{
	    goto return_label;
	}
	++i;
    } while (elem->Key != NULL && *elem->Key != '\0');

return_label:
    header->Index = -1;

    return table;
}

static void*
_ma_shash_get(void* table, const char* key, i32 capacity, i32 elementSize)
{
    if (table == NULL)
    {
	return NULL;
    }

    MATableHeader* header = ma_table_header(table);

    i32 i = 0,
	keyLength = ma_string_length(key),
	index;
    MAInternalStringElement* elem;
    do
    {
	index = ma_get_shash(key, capacity, i);
	elem = (MAInternalStringElement*) (table + index * elementSize);
	if (ma_string_compare_length(elem->Key, key, keyLength))
	{
	    goto return_label;
	}
	++i;
    } while (elem->Key != NULL && *elem->Key != '\0');

return_label:
    header->Index = -1;

    return table;
}

#define ma_shash_put(table, key, value)					\
    ({									\
	MATableHeader* header = ma_table_header((table));		\
									\
	if ((table) == NULL)						\
	{								\
	    (table) = ma_table_grow((table), sizeof(*(table)), 0);	\
	}								\
	else if (header->Count >= i32(0.7 * header->Capacity))		\
	{								\
	    i32 i;							\
	    __typeof__((table)) newTable = ma_table_grow((table), sizeof(*(table)), 0); \
	    MATableHeader* newHeader = ma_table_header((newTable));	\
	    for (i = 0; i < header->Capacity; i++)			\
	    {								\
		if ((table)[i].Key != NULL)				\
		{							\
		    (newTable) = _ma_shash_put((newTable), (table)[i].Key, newHeader->Capacity, newHeader->ElementSize); \
		    (newTable)[(ma_table_header((newTable))->Index)].Key = ((table)[i].Key); \
		    (newTable)[(ma_table_header((newTable))->Index)].Value = ((table)[i].Value); \
		}							\
	    }								\
									\
	    ma_table_free(table);					\
									\
	    (table) = (newTable);					\
	}								\
									\
	header = ma_table_header((table));				\
	(table) = _ma_shash_put((table), (key), header->Capacity, header->ElementSize); \
	(table)[(header->Index)].Key = (key);				\
	(table)[(header->Index)].Value = (value);			\
    })

#define ma_shash_get(table, key)					\
    ({									\
	(table) = _ma_shash_get((table), (key), ma_table_capacity((table)), ma_table_element_size((table))); \
	((table) != NULL &&						\
	 ma_table_header((table))->Index != -1)				\
	    ? (table)[ma_table_header((table))->Index].Value		\
	    : ((__typeof__(table[0].Value)) { 0 });			\
    })

#define ma_shash_geti(table, key)				\
    ({								\
	(table) = _ma_shash_get((table), (key), ma_table_capacity((table)), ma_table_element_size((table))); \
	((table) == NULL) ? -1 : ma_table_header((table))->Index;	\
    })

static i32 g_Size;
static MemoryList g_AllocList;
typedef struct ScopeLabel
{
    const char* Key;
    void** Value;
} ScopeLabel;

static ScopeLabel* g_ScopesTable = NULL;
static char* g_ActiveScope = NULL;
static PrintAllocationSourceType g_SourceType = PRINT_ALLOCATION_SOURCE_TYPE_NONE;

static void*
memory_helper_allocate(i32 size, i32 line, const char* file)
{
    g_Size += size;

    if (g_SourceType == PRINT_ALLOCATION_SOURCE_TYPE_TERMINAL)
    {
	GERROR("Allocation in Size: %d, File: %s, Line: %d\n", size, file, line);
    }

    if (g_AllocList.IsInitialized != 1)
    {
	g_AllocList.IsInitialized = 1;
	list_init(&g_AllocList);
    }

    assert(size > 0 && "memory_allocate(size) where size > 0 !!!");

    void* data = malloc(size);

    size_t memoryBlockSize = sizeof(MemoryBlock);
    MemoryBlock* block = (MemoryBlock*) malloc(memoryBlockSize);
    block_create(block, data, size, file, line);
    list_add(&g_AllocList, block);

    if (g_ActiveScope != NULL)
    {
	void** scopesAddrs = (void**) ma_shash_get(g_ScopesTable, g_ActiveScope);
	ma_array_push(scopesAddrs, data);
	ma_shash_put(g_ScopesTable, g_ActiveScope, scopesAddrs);
    }

    return data;
}

static void*
memory_helper_reallocate(void* data, i32 size, i32 line, const char* file)
{
    MemoryBlock* block = list_find(&g_AllocList, data);
    list_remove(&g_AllocList, block);


    void* newData = realloc(data, size);
    if (newData == NULL)
    {
	GERROR("Realloc failed!\n");
	assert(0 && "Realloc failed!");
    }

    MemoryBlock* newBlock = (MemoryBlock*) malloc(sizeof(MemoryBlock));
    block_create(newBlock, newData, size, file, line);
    list_add(&g_AllocList, newBlock);

    if (g_SourceType == PRINT_ALLOCATION_SOURCE_TYPE_TERMINAL)
    {
	GSUCCESS("REALLOC Size: %d NewSize: %d\n", block->AllocatedSize, size);
    }

    g_Size -= block->AllocatedSize;
    g_Size += size;

    if (block)
	free(block);

    return newData;
}

static void
memory_helper_free(void* data, i32 line, const char* file)
{
    MemoryBlock* block = list_find(&g_AllocList, data);
    list_remove(&g_AllocList, block);

    if (g_SourceType == PRINT_ALLOCATION_SOURCE_TYPE_TERMINAL)
    {
	GSUCCESS("Free memory %lld, Size: %d, FILE: %s LINE: %d\n", (size_t)data, block->AllocatedSize, file, line);
    }

    g_Size -= block->AllocatedSize;

    free(data);
    free(block);

    data = NULL;
}

static i32
memory_helper_get_allocated_size()
{
    return g_Size;
}

static void
memory_helper_print_allocations_in_terminal(PrintAllocationSourceType type)
{
    g_SourceType = type;
}

static MemoryList*
memory_get_list()
{
    return &g_AllocList;
}

static void
scope_new(const char* name)
{
    assert(g_ActiveScope == NULL && "You fogot to call scope_delete before calling scope new");

    g_ActiveScope = (char*) name;

    void** arr = NULL;
    ma_shash_put(g_ScopesTable, name, arr);
}

static void
scope_delete(const char* name)
{
    i32 isScopeExist = ma_shash_geti(g_ScopesTable, name);
    assert(isScopeExist != -1 && "Trying delete scope that doesn't exist!");

    void** arr = (void**) ma_shash_get(g_ScopesTable, name);
    i32 i, count = ma_array_count(arr);
    for (i = 0; i < count; ++i)
    {
	memory_free(arr[i]);
    }

    g_ActiveScope = NULL;
}

#endif // MemoryAllocator.h

/*

		 ###################################
		 ###################################
			   Array.h (ARRAY)
		 ###################################
		 ###################################

*/
#ifndef ARRAY_H
#define ARRAY_H

#ifndef memory_allocate
 #include <stdlib.h>
 #define ArrayAllocateDelegate(s) malloc(s)
 #define ArrayReallocateDelegate(d, s) realloc(d, s)
 #define ArrayFreeDelegate(d) free(d)
#else
 #define ArrayAllocateDelegate(s) memory_allocate(s)
 #define ArrayReallocateDelegate(d, s) memory_reallocate(d, s)
 #define ArrayFreeDelegate(d) memory_free(d)
#endif

typedef struct ArrayHeader
{
    int ElementSize;
    int Count;
    int Capacity;
    char Buffer[0];
} ArrayHeader;

#define array_header(b) ((ArrayHeader*) (((char*)b) - sizeof(ArrayHeader)))
#define array_len(b) ((b != NULL) ? array_header(b)->Count : 0)
#define array_count(b) ((b != NULL) ? array_header(b)->Count : 0)
#define array_cap(b) ((b != NULL) ? array_header(b)->Capacity : 0)
#define array_capacity(b) ((b != NULL) ? array_header(b)->Capacity : 0)
#define array_element_size(b) ((b != NULL) ? array_header(b)->ElementSize : 0)

#define array_reserve(b, elementsCount) internal_array_reserve(b, elementsCount, sizeof(*b))

#define array_set(b, v)						\
    {								\
	assert(b);						\
	int count = array_count(b);				\
	for (int i = 0; i < count; i++)				\
	{							\
	    b[i] = v;						\
	};							\
    }

#define array_push(b, ...)					\
    {								\
	if ((b) == NULL || array_len(b) >= array_cap(b))	\
	{							\
	    (b) = internal_array_grow(b, sizeof(*b));		\
	}							\
	b[array_count(b)] = (__VA_ARGS__);			\
	++array_header(b)->Count;				\
    }

#define array_push_at(b, i, ...)				\
    {								\
	if ((b) == NULL || array_len(b) >= array_cap(b))	\
	{							\
	    (b) = internal_array_grow(b, sizeof(*b));		\
	}							\
	b[i] = (__VA_ARGS__);					\
	++array_header(b)->Count;				\
    }

#define array_remove(b, ind)					\
    {								\
	if ((b) != NULL && (ind + 1) != array_count((b)))	\
	{							\
	    internal_array_copy_from_block(b, ind);		\
	}							\
								\
	--array_header(b)->Count;				\
    }

#define array_end(b) (((void*)b) + array_count(b) * sizeof(*b))
#define array_copy(src)	internal_array_copy(src)
#define array_clear(b)					\
    ({							\
	for (int i = 0; i < array_count(b); i++)	\
	{						\
	    b[i] = (__typeof__(b[0]))0;			\
	}						\
    })
#define array_clearv(b, val)			\
    ({						\
	if ((b) != NULL)			\
	{					\
	    int i, count = array_count(b);	\
	    for (i = 0; i < count; ++i)		\
	    {					\
		b[i] = val;			\
	    }					\
	    array_header(b)->Count = 0;		\
	}					\
    })

#define array_free(b)				\
    ({						\
	if ((b))				\
	{					\
	    ArrayFreeDelegate(array_header(b));	\
	}					\
	(b) = NULL;				\
    })						\


static int StartSize = 1;

static void
interanl_array_init(void* (*alloc)(size_t size), void* (*realloc)(void* data, size_t size), void (*free)(void* data))
{
    //ArrayAllocateDelegate   = alloc;
    //ArrayReallocateDelegate = realloc;
    //ArrayFreeDelegate       = free;
}

static void*
internal_array_reserve(void* array, int elementsCount, int elementSize)
{
    assert(array == NULL);

    size_t capacity = elementsCount * elementSize;
    size_t size = capacity + sizeof(ArrayHeader);
    ArrayHeader* newHeader = (ArrayHeader*) ArrayAllocateDelegate(size);
    newHeader->Count = 0;
    newHeader->Capacity = elementsCount;
    newHeader->ElementSize = elementSize;

    return newHeader->Buffer;
}

static void*
internal_array_grow(const void* array, int elementSize)
{
    ArrayHeader* newHeader = NULL;

    if (array != NULL)
    {
	size_t newCapacity = 2 * array_cap(array) + 1;
	size_t newSize = newCapacity * elementSize + sizeof(ArrayHeader);
	ArrayHeader* header = array_header(array);
	newHeader = (ArrayHeader*) ArrayReallocateDelegate(header, newSize);
	newHeader->Capacity = newCapacity;
    }
    else
    {
	size_t size = StartSize * elementSize + sizeof(ArrayHeader);
	newHeader = (ArrayHeader*) ArrayAllocateDelegate(size);
	newHeader->Count = 0;
	newHeader->Capacity = StartSize;
	newHeader->ElementSize = elementSize;
    }

    return newHeader->Buffer;
}

static void*
internal_array_copy(const void* src)
{
    if ((src) != NULL)
    {
	ArrayHeader* header = array_header(src);
	int count = header->Count;
	int elementSize = header->ElementSize;
	int size = count * elementSize + sizeof(ArrayHeader);

	ArrayHeader* newHeader = (ArrayHeader*) ArrayAllocateDelegate(size);
	newHeader->Count = count;
	newHeader->Capacity = count;
	newHeader->ElementSize = elementSize;
	memcpy(newHeader->Buffer, src, count * elementSize);

	return newHeader->Buffer;
    }

    return NULL;
}

static void
internal_array_copy_from_block(void* array, int index)
{
    ArrayHeader* header = array_header(array);

    void* __restrict__ dest = header->Buffer + (index * header->ElementSize);
    void* __restrict__ lastDest = (void*) (header->Buffer + (header->Count - 1) * header->ElementSize);
    const void* __restrict__ from = (const void*) (dest + header->ElementSize);
    int copySize = (header->Count - index - 1) * header->ElementSize;

    memcpy(dest, from, copySize);
    memset(lastDest, '\0', header->ElementSize);
}

#endif // Array.h

/*

		 ###################################
		 ###################################
			      String.h
		 ###################################
		 ###################################

*/
#ifndef STRING_H
#define STRING_H

#define STRING_EMPTY ""
#define STRING_NULL "\0"

typedef struct IStringHeader
{
    i32 Length;
    i32 Exist;
    const char* Buffer;
} IStringHeader;

#define istring_header(istr) ((IStringHeader*) (((u8*)istr) - sizeof(IStringHeader)))
#define istring_length(istr) ((istr) ? istring_header(istr)->Length : -1)
#define istring(str)							\
    ({									\
	const char* istr = istring_get_buffer(str);			\
	(istr) ? istr : _istring(str);					\
    })

static IStringHeader** g_IStringHeaders = NULL;

static i32 string_length(const char* str);
static i32 string_compare(const char* left, const char* right);

static const char*
_istring(const char* src)
{
    assert(src);

    i32 length = string_length(src);
    i32 size = sizeof(IStringHeader) + (length + 1) * sizeof(*src);
    IStringHeader* interning = (IStringHeader*) memory_allocate(size);
    char* buffer = (char*) (((void*)interning) + sizeof(IStringHeader));
    memcpy(buffer, src, length * sizeof(*src));
    buffer[length] = '\0';
    assert(buffer != NULL);

    interning->Buffer = (const char*) buffer;
    interning->Length = length;
    interning->Exist = 1;

    assert(interning->Buffer != NULL);
    assert(interning->Buffer == ((void*)interning) + sizeof(*interning));

    array_push(g_IStringHeaders, interning);

    return interning->Buffer;
}

static const char*
istring_get_buffer(const char* src)
{
    i32 i;
    i32 count = array_count(g_IStringHeaders);

    for (i = 0; i < count; ++i)
    {
	if (string_compare(src, g_IStringHeaders[i]->Buffer))
	{
	    return g_IStringHeaders[i]->Buffer;
	}
    }

    return NULL;
}

static IStringHeader**
istring_get_headers()
{
    return g_IStringHeaders;
}

static void
istring_free(char* istring)
{
    IStringHeader* iheader = istring_header(istring);
    i32 i, count = array_count(g_IStringHeaders);
    for (i = 0; i < count; ++i)
    {
	IStringHeader* istrHeader = g_IStringHeaders[i];
	if (istrHeader == iheader)
	{
	    array_remove(g_IStringHeaders, i);
	}
    }

    memory_free(iheader);
}

static void
istring_free_headers()
{
    i32 i, count = array_count(g_IStringHeaders);
    for (i = 0; i < count; ++i)
    {
	IStringHeader* istrHeader = g_IStringHeaders[i];
	memory_free(istrHeader);
    }

    array_free(g_IStringHeaders);
}

#define string_format(out, format, ...) sprintf(out, format, __VA_ARGS__)

static char*
string(const char* string)
{
    i32 length = string_length(string);
    char* newString = (char*) memory_allocate((length + 1) * sizeof(char));
    newString[length] = '\0';
    memcpy(newString, string, length * sizeof(char));

    return newString;
}

force_inline i32
_string_number_rank(i32 number)
{
    i32 rank = 0;
    for (; ;)
    {
	number /= 10;
	if (number == 0)
	{
	    return rank;
	}

	++rank;
    }
}

force_inline i32
_string_number_of_rank(i32 number, i32 rank)
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
_string_number_of_digit(i32 number, i32 digit)
{
    i32 i;

    if (_string_number_rank(number) < digit)
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

    number %= _string_number_of_rank(1, (digit + 1));
    number /= _string_number_of_rank(1, digit);

    return number;
}

#define _string_int(input, number)					\
    ({									\
	i8 isNumberNegative = ((number < 0) ? 1 : 0);			\
	i32 i, rank = _string_number_rank(number), numberLength = rank + isNumberNegative + 1; \
									\
	if (isNumberNegative)						\
	{								\
	    input[0] = '-';						\
	}								\
									\
	for (i = isNumberNegative; i < numberLength; ++i)		\
	{								\
	    input[i] = _string_number_of_digit(number, rank) + 48;	\
	    --rank;							\
	}								\
    })

static void
string_i32(char* input, i32 number)
{
    _string_int(input, number);
}

static char*
string_allocate(i32 length)
{
    i32 size = (length + 1) * sizeof(char);
    char* newString = (char*) memory_allocate(size);
    memset(newString, '\0', size);

    return newString;
}

static i32
string_count_of_fast(const char* string, i32 length, char c)
{
    i32 count = 0;
    for (i32 i = 0; i < length; ++i)
    {
	if (string[i] == c)
	    ++count;
    }

    return count;
}

static i32
string_count_of(const char* string, char c)
{
    i32 length = string_length(string);
    i32 count = string_count_of_fast(string, length, c);

    return count;
}

static void
string_set(char* string, char c, u32 length)
{
    i32 i;
    for (i = 0; i < length; ++i)
    {
	string[i] = c;
    }
}

static i32
string_length(const char* str)
{
    assert(str);

    u32 index;
    for (index = 0; str[index] != '\0'; index++);
    return index;
}

static char*
string_copy(const char* oth, i32 length)
{
    assert(oth);

    char* result = (char*) memory_allocate((length + 1) * sizeof(char));
    memcpy(result, oth, length);
    result[length] = '\0';
    return result;
}

static char*
string_concat(const char* left, const char* right)
{
    i32 leftLength = string_length(left);
    i32 rightLength = string_length(right);

    char* newString = (char*) memory_allocate(leftLength + rightLength + 1);

    memcpy(newString, left, leftLength);
    memcpy(newString + leftLength, right, rightLength);
    newString[leftLength + rightLength] = '\0';

    return newString;
}

static char*
string_concat_with_space_between(const char* left, const char* right, i32 length, char c)
{
    assert(left);
    assert(right);

    i32 leftLength = string_length(left);
    i32 rightLength = string_length(right);

    assert(length > leftLength + rightLength);

    i32 totalLength = (length + rightLength + 1);
    char* result = (char*) memory_allocate(totalLength * sizeof(char));
    memset(result, c, totalLength);
    memcpy(result, left, leftLength);
    memcpy(result + length, right, rightLength);

    result[totalLength - 1] = '\0';

    return result;
}

static char*
string_concat3(const char* left, const char* middle, const char* right)
{
    i32 leftLength = string_length(left);
    i32 middleLength = string_length(middle);
    i32 rightLength = string_length(right);

    char* newString = (char*) memory_allocate(leftLength + middleLength + rightLength + 1);

    memcpy(newString, left, leftLength);
    memcpy(newString + leftLength, middle, middleLength);
    memcpy(newString + leftLength + middleLength, right, rightLength);
    newString[leftLength + middleLength + rightLength] = '\0';

    return newString;
}

static char*
string_concat3l(const char* left, const char* middle, const char* right, i32 leftLength, i32 middleLength, i32 rightLength)
{
    char* newString = (char*) memory_allocate(leftLength + middleLength + rightLength + 1);

    memcpy(newString, left, leftLength);
    memcpy(newString + leftLength, middle, middleLength);
    memcpy(newString + leftLength + middleLength, right, rightLength);

    newString[leftLength + middleLength + rightLength] = '\0';

    return newString;
}

static i32
string_compare(const char* left, const char* right)
{
    assert(left);
    assert(right);

    i32 leftLength  = string_length(left);
    i32 rightLength = string_length(right);

    if (leftLength != rightLength || leftLength == 0 || rightLength == 0)
    {
	return 0;
    }

    for (rightLength = 0; rightLength < leftLength; rightLength++)
    {
	if (left[rightLength] != right[rightLength])
	{
	    return 0;
	}
    }

    return 1;
}

static i32
string_compare_length(const char* left, const char* right, i32 length)
{
    assert(left);
    assert(right);

    i32 i;
    for (i = 0; i < length; ++i)
    {
	if (left[i] != right[i])
	{
	    return 0;
	}
    }

    return 1;
}

static char*
string_to_upper(const char* input)
{
    i32 i;
    i32 inputLength = string_length(input);
    char* result = memory_allocate((inputLength + 1) * sizeof(char));

    for (i = 0; i < inputLength; ++i)
    {
	char element = input[i];
	if (element >= 'a' && element <= 'z')
	{
	    result[i] = element - 'a' + 'A';
	}
	else
	{
	    result[i] = element;
	}
    }
    result[inputLength] = '\0';
    return result;
}

static char*
string_to_lower(const char* input)
{
    i32 i;
    i32 input_length;
    char  element;
    char* result;

    if (input == NULL)
    {
	return NULL;
    }

    input_length = string_length(input);
    result = memory_allocate((input_length + 1) * sizeof(char));
    for (i = 0; i < input_length; ++i)
    {
	element = input[i];
	if (element >= 'A' && element <= 'Z')
	{
	    result[i] = element - 'A' + 'a';
	}
	else
	{
	    result[i] = element;
	}
    }
    result[input_length] = '\0';
    return result;
}

static i32
string_index_of(const char* input, char character)
{
    i32 i;

    if (input == NULL)
    {
	return -1;
    }

    for (i = 0; input[i] != '\0'; ++i)
    {
	if (input[i] == character)
	{
	    return i;
	}
    }

    return -1;
}

static i32
string_index_of_string(const char* input, const char* string)
{
    i32 i, j, flag, inputLength, stringLength;

    assert(input != NULL);
    assert(string != NULL);

    inputLength = string_length(input);
    stringLength = string_length(string);

    if (inputLength <= 0 || stringLength <= 0)
    {
	return -1;
    }

    flag = -1;
    for (i = 0; i < inputLength; ++i)
    {
	for (j = 0; j < stringLength; ++j)
	{
	    if (input[i + j] == string[j])
	    {
		flag = 1;
	    }
	    else
	    {
		flag = -1;
		break;
	    }
	}

	if (flag == 1)
	{
	    return i;
	}
    }

    return -1;
}

static i32
string_last_index_of(const char* input, char character)
{
    i32 i, startIndex;

    assert(input != NULL && "Input can't be NULL!!!");

    startIndex = string_length(input) - 1;

    for (i = startIndex; i >= 0; i--)
    {
	if (input[i] == character)
	{
	    return i;
	}
    }

    return -1;
}

static i32
string_last_index_of_string(const char* input, const char* string)
{
    i32 i, j, flag, inputLength, stringLength;

    assert(input != NULL && "Input can't be NULL!!!");
    assert(string != NULL && "String can't be NULL!!!");

    inputLength = string_length(input);
    stringLength = string_length(string);

    if (inputLength <= 0 || stringLength <= 0)
    {
	return -1;
    }

    flag = -1;
    i32 temp = 0;
    for (i = inputLength; i >= 0; i--)
    {
	for (j = stringLength; j >= 0; j--)
	{
	    if (input[i - temp] == string[j])
	    {
		flag = 1;
	    }
	    else
	    {
		flag = -1;
		break;
	    }
	    ++temp;
	}

	if (flag == 1)
	{
	    return temp;
	}
    }

    return -1;
}

static char*
string_substring(const char* input, i32 startIndex)
{
    i32 i, newLength, inputLength;
    char* result;

    assert(input && input != NULL && "input length is NULL !!!");

    inputLength = string_length(input);
    assert(startIndex < inputLength && "start index >= input length !!!");
    assert(startIndex > 0 && "start index < 0!!!");

    newLength = inputLength - startIndex;
    result = memory_allocate((newLength + 1) * sizeof(char));
    memcpy(result, input + startIndex, newLength);
    result[newLength] = '\0';

    return result;
}

static char*
string_substring_range(const char* input, i32 startIndex, i32 endIndex)
{
    i32 i, inputLength, newLength;
    char* result;

    assert(input && input != NULL && "input can't be NULL!!!");

    inputLength = string_length(input);
    newLength = endIndex - startIndex + 1;
    result = memory_allocate((newLength + 1) * sizeof(char));

    assert(startIndex < inputLength);
    assert(startIndex >= 0);
    assert(inputLength > endIndex);
    assert(startIndex <= endIndex);

    memcpy(result, input + startIndex, newLength);
    result[newLength] = '\0';

    return result;
}

/*
3, 5
"01234567" - len: 8
"01267" - len: 5
*/
static char*
string_cut(const char* input, u32 begin, u32 end)
{
    i32 i, inputLength = string_length(input), resultLength = inputLength - (end - begin + 1);
    char* result;

    assert(input);
    assert(begin >= 0);
    assert(end < inputLength);
    assert(begin < end);
    assert(inputLength);
    assert(resultLength);

    result = memory_allocate(resultLength);
    for (i = 0; i < begin; ++i)
    {
	result[i] = input[i];
    }

    for (i = (end + 1); i < inputLength; ++i)
    {
	result[begin + i - end - 1] = input[i];
    }

    return result;
}

static char**
string_split(char* input, char splitCharacter)
{
    i32 i, inputLength = string_length(input), wordBeginIndex = 0, isWordIndexSet = 0;
    char** result = NULL;

    for (i = 0; i < inputLength; ++i)
    {
	char character = input[i];
	if (character != splitCharacter && !isWordIndexSet)
	{
	    isWordIndexSet = 1;
	    wordBeginIndex = i;
	}

	char* word;
	if (character == splitCharacter && isWordIndexSet)
	{
	    isWordIndexSet = 0;

	    word = string_substring_range(input, wordBeginIndex, i - 1);
	    array_push(result, word);
	}
	else if (i == (inputLength - 1))
	{
	    word = string_substring(input, wordBeginIndex);
	    array_push(result, word);
	}
    }

    return result;
}

static char*
string_join(const char** list, char joinCharacter)
{
    i32 i, listCount, finalLength, curLength = 0, strLength;
    char* finalString = NULL;
    const char* str = NULL;

    assert(list && "List is NULL or Undefined!!!");
    listCount = array_count(list);
    assert(listCount && "List is empty !!!");

    finalLength = listCount;
    for (i = 0; i < listCount; ++i)
    {
	finalLength += string_length(list[i]);
    }

    finalString = (char*) memory_allocate(finalLength);
    for (i = 0; i < (listCount - 1); ++i)
    {
	str = list[i];
	strLength = string_length(str);
	memcpy(finalString + curLength, str, strLength);
	finalString[curLength + strLength] = joinCharacter;
	curLength += strLength + 1;
    }

    str = list[listCount - 1];
    strLength = string_length(str);
    memcpy(finalString + curLength, str, strLength);

    finalString[finalLength - 1] = '\0';

    return finalString;
}

static char*
string_join_i32(const i32* list, char joinCharacter)
{
    assert(0 && "Not tested yet, let's write some Unit Test's!");

    char* result = NULL;
    char stringValue[32];

    i32 i, count = array_count(list), el;
    for (i = 0; i < count; ++i)
    {
	el = list[i];
	string_i32(stringValue, el);
    }

    return NULL;
}

static void
string_i64(char* input, i64 number)
{
    _string_int(input, number);
}

static void
string_f32(char* input, f32 number)
{
    sprintf(input, "%f", number);
}

static void
string_f64(char* input, f64 number)
{
    sprintf(input, "%f", number);
}

static i32
string_to_i32(char* input)
{
    i32 digit,
	i = 0, result = 0, isNegative = 0,
	multiplier = 10,
	rank = string_length(input);

    if (input[0] == '-')
    {
	isNegative = 1;
	i = 1;
    }

    for (; i < rank; ++i)
    {
	digit = input[i] - '0';
	result = (result * multiplier) + digit;
    }

    if (isNegative)
	result *= -1;

    return result;
}

static f32
string_to_f32(char* input)
{
    i32 digit,
	i = 0, isNegative = 0,
	multiplier = 10,
	rank = string_length(input);
    f32 intResult = 0,
	floatMultiplier = 0.1;
    char c;

    if (input[0] == '-')
    {
	isNegative = 1;
	i = 1;
    }

    for (; i < rank; ++i)
    {
	c = input[i];
	if (c == '.')
	{
	    ++i;
	    break;
	}

	digit = c - '0';
	intResult = (intResult * multiplier) + digit;
    }

    f32 floatResult = 0.0f;
    for (i32 f = (rank - 1); f >= i; --f)
    {
	c = input[f];

	digit = c - '0';

	/*
	  12.318
	       ^
	  0.8
	  0.08 + 0.1 = 0.18
	 */

	floatResult = (floatResult * floatMultiplier) + digit * floatMultiplier;
    }

    intResult += floatResult;

    if (isNegative)
	intResult *= -1;

    return intResult;
}

force_inline char
char_to_upper(char character)
{
    if (character >= 'a' && character <= 'z')
	return (character - 'a' + 'A');
    else
	return character;
}

force_inline char
char_to_lower(char character)
{
    if (character >= 'A' && character <= 'Z')
	return (character - 'A' + 'a');
    else
	return character;
}

#endif // String.h

/*

		 ###################################
		 ###################################
			    StringBuilder.h
		 ###################################
		 ###################################

*/
#ifndef STRING_BUILDER_H
#define STRING_BUILDER_H

#ifndef memory_allocate
 #define StringBuilderAllocateDelegate(size) malloc(size)
 #define StringBuilderFreeDelegate(data) free(data)
#else
 #define StringBuilderAllocateDelegate(size) memory_allocate(size)
 #define StringBuilderFreeDelegate(data) memory_free(data)
#endif

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
		memset(str, '\0', 64);
		sb_string_i32_to_string(str, number);
		string_builder_appends(builder, str);

		state = 0;
	    }
	    else if (state == 2)
	    {
		i64 number = va_arg(valist, i64);
		char str[64];
		memset(str, '\0', 64);
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
		memset(str, '\0', 4);
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
		memset(str, '\0', 64);
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

#endif // StringBuilder.h

/*

		 ###################################
		 ###################################
			     HashTable.h
		 ###################################
		 ###################################

*/
#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#ifndef memory_allocate
 #include <stdlib.h>
 #define HashTableAllocate(size) malloc(size)
 #define HashTableFree(data) free(data)
#else
 #define HashTableAllocate(size) memory_allocate(size)
 #define HashTableFree(data) memory_free(data)
#endif

force_inline i32
hash_table_i32_pow(i32 x, i32 n)
{
    i32 result = 1;

    if (n == 0 || x == 1)
    {
	return 1;
    }

    while (n)
    {
	result *= x;
	--n;
    }

    return result;
}

static i32
hash_table_string_length(const char* str)
{
    assert(str);
    i32 index;
    for (index = 0; str[index] != '\0'; index++);
    return index;
}

static i32
hash_table_string_compare_length(const char* left, const char* right, i32 length)
{
    assert(left);
    assert(right);

    for (i32 i = 0; i < length; ++i)
    {
	if (left[i] != right[i])
	{
	    return 0;
	}
    }

    return 1;
}

/*
	   Hash Table Statistics (for profiling purposes)
*/
#if HASH_TABLE_PROFILING == 1
typedef struct TableStatistics
{
    i32 PutAttempt;
    i32 GetAttempt;
} TableStatistics;
static TableStatistics g_Statistics;
static TableStatistics
table_get_statistics()
{
    return g_Statistics;
}
#endif

typedef struct TableHeader
{
    i32 ElementSize;
    i32 Count;
    i32 Capacity;
    i32 Index;
    i32 NextPrime;
    u8 Buffer[0];
} TableHeader;

#define table_header(b) ((TableHeader*) (((u8*)b) - sizeof(TableHeader)))
#define table_count(b) ((b != NULL) ? table_header(b)->Count : 0)
#define table_capacity(b) ((b != NULL) ? table_header(b)->Capacity : 0)
#define table_element_size(b) ((b != NULL) ? table_header(b)->ElementSize : 0)
#define table_next_prime(b) ((b != NULL) ? table_header(b)->NextPrime : 0)
#define table_free(b) ((b) ? HashTableFree(table_header(b)) : 0)
#define table_reserve(b, elementsCount) _table_reserve(elementsCount, sizeof(*b))

#define shash_free(table) table_free((table))
#define hash_free(table) table_free((table))

/*
  Base
*/

force_inline i32
i32_comparer(i32 key)
{
    return key != -1;
}

force_inline i32
string_comparer(const char* key)
{
    return key != NULL;
}

#define base_hash_put(table, key, value, hashPutDelegate, defValComparer, isInt) \
    ({									\
	TableHeader* header = table_header((table));			\
									\
	if ((table) == NULL)						\
	{								\
	    (table) = _table_new((table), sizeof(*(table)), isInt);	\
	}								\
	else if (header->Count >= i32(0.7 * header->Capacity))		\
	{								\
	    i32 j;							\
	    __typeof__((table)) newTable = _table_grow((table), sizeof(*(table)), isInt); \
	    TableHeader* newHeader = table_header((newTable));		\
	    for (j = 0; j < header->Capacity; j++)			\
	    {								\
		if (defValComparer((table)[j].Key) && (table)[j].Key)	\
		{							\
		    (newTable) = hashPutDelegate((newTable), (table)[j].Key); \
		    (newTable)[(table_header((newTable))->Index)].Key = ((table)[j].Key); \
		    (newTable)[(table_header((newTable))->Index)].Value = ((table)[j].Value); \
		}							\
	    }								\
									\
	    table_free(table);						\
									\
	    (table) = (newTable);					\
	}								\
									\
	header = table_header((table));					\
	(table) = hashPutDelegate((table), (key));			\
	(table)[(header->Index)].Key = (key);				\
	(table)[(header->Index)].Value = (value);			\
    })

#define base_hash_get(table, key, hashGetDelegate, tableHeader)		\
    ({									\
	(table) = hashGetDelegate((table), (key));			\
	((table) != NULL &&					\
	 tableHeader((table))->Index != -1)			\
	    ? (table)[tableHeader((table))->Index].Value	\
	    : ((__typeof__(table[0].Value)) { 0 });		\
    })

/*
  String Hash Table (const char* Key)
*/
// base_hash_put(table, key, value, hashPutDelegate, tableHeader, HeaderType, tableGrow, tableFree, defValComparer, isInt)

#define shash_put(table, key, value)					\
    base_hash_put((table), (key), (value), _base_shash_put, string_comparer, 0)
#define shash_get(table, key)						\
    ({									\
	base_hash_get((table), (key), _base_shash_get, table_header);	\
    })
#define shash_geti(table, key)					\
    ({								\
	_base_shash_geti((table), (key));			\
    })

/*
  Int Hash Table (int Key)
*/

#define hash_put(table, key, value)					\
    base_hash_put((table), (key), (value), _base_hash_put, i32_comparer, 1)
#define hash_get(table, key)					\
    base_hash_get((table), (key), _base_hash_get, table_header)
#define hash_geti(table, key)						\
    ({									\
	_base_hash_geti((table), (key));				\
    })

#define PRIME_1 117
#define PRIME_2 119

typedef struct InternalStringElement
{
    const char* Key;
    void* Data;
} InternalStringElement;

typedef struct InternalIntElement
{
    i32 Key;
    void* Data;
} InternalIntElement;

static i32 g_Primes[] = {
    53, 97, 193, 389,
    769, 1543, 3079, 6151,
    12289, 24593, 49157, 98317,
    196613, 393241, 786433, 1572869,
    3145739, 6291469, 12582917, 25165843,
    50331653, 100663319, 201326611, 402653189,
    805306457, 1610612741
};
static i32 g_NextPrimeIndex = 0;
static i32 g_PrimesCount = ARRAY_COUNT(g_Primes);

static i32
_get_prime(TableHeader* header)
{
#define HASH_CLAMP(a, min, max) ((a < min) ? min : ((a > max) ? max : a))
    // TODO(bies):
    // fix bug with NextPrime much bigger then array index
    i32 nextIndex = 0;
    if (header != NULL)
    {
	nextIndex = HASH_CLAMP(header->NextPrime, 0, 24);
    }

    i32 prime = g_Primes[nextIndex];
#undef HASH_CLAMP
    return prime;
}

static void*
_table_new(void* table, i32 elemSize, i32 isInt)
{
    TableHeader* newHeader;
    i32 prime;
    i32 newSize;

    prime = _get_prime(NULL);
    newSize = prime * elemSize;
    newHeader = (TableHeader*) HashTableAllocate(newSize + sizeof(TableHeader));
    newHeader->Count = 0;
    newHeader->Capacity = prime;
    newHeader->ElementSize = elemSize;

    newHeader->NextPrime = 1;

    if (isInt)
    {
	memset(newHeader->Buffer, -1, newSize);
    }
    else
    {
	memset(newHeader->Buffer, '\0', newSize);
    }

    return newHeader->Buffer;
}

static void*
_table_grow(void* table, i32 elemSize, i32 isInt)
{
    assert(table != NULL && "_table_grow where table != NULL!!!");

    TableHeader* newHeader;
    i32 prime;
    i32 newSize;

    TableHeader* prevHeader = table_header(table);
    prime = _get_prime(prevHeader);
    newSize = prime * elemSize;
    newHeader = (TableHeader*) HashTableAllocate(newSize + sizeof(TableHeader));
    newHeader->Count = 0;
    newHeader->Capacity = prime;
    newHeader->ElementSize = elemSize;
    newHeader->NextPrime = prevHeader->NextPrime + 1;

    if (isInt)
    {
	memset(newHeader->Buffer, -1, newSize);
    }
    else
    {
	memset(newHeader->Buffer, '\0', newSize);
    }

    return newHeader->Buffer;
}

static void*
_table_reserve(i32 elementsCount, i32 elementSize, i32 nextPrime)
{
    i32 capacity = elementsCount * elementSize;
    TableHeader* newHeader;
    newHeader = (TableHeader*) HashTableAllocate(capacity + sizeof(*newHeader));
    newHeader->Count = 0;
    newHeader->Capacity = elementsCount;
    newHeader->ElementSize = elementSize;
    newHeader->NextPrime = nextPrime;

    return newHeader->Buffer;
}

force_inline i32
shash(const char* key, i32 prime, i32 bucketNumber)
{
    assert(bucketNumber != 0);
    i32 shash = 0;
    i32 keyLength = hash_table_string_length(key);
    for (i32 i = 0; i < keyLength; i++)
    {
	shash += hash_table_i32_pow(prime, (keyLength - (i + 1))) * key[i];
	shash %= bucketNumber;
    }

    return shash;
}

force_inline i32
get_shash(const char* key, i32 bucketNumber, i32 attempt)
{
    i32 hashA = shash(key, PRIME_1, bucketNumber);
    i32 hashB = shash(key, PRIME_2, bucketNumber);
    return (hashA + (attempt * (hashB + 1))) % bucketNumber;
}

static void*
_base_shash_put(void* table, const char* key)
{
    assert(table);

    TableHeader* header = table_header(table);

    i32 i = -1,
	keyLength = hash_table_string_length(key),
	index;
    char* itemsKey = (char*) key;
    do
    {
	++i;
	index = get_shash(itemsKey, header->Capacity, i);
	itemsKey = (char*) (table + index * header->ElementSize);
    } while (*itemsKey != '\0' && hash_table_string_compare_length(itemsKey, key, keyLength));

    header->Index = index;
    ++header->Count;
#if HASH_TABLE_PROFILING == 1
    g_Statistics.PutAttempt = i;
#endif

    return table;
}

static void*
_base_shash_get(void* table, const char* key)
{
    if (table == NULL)
    {
	return NULL;
    }

    TableHeader* header = table_header(table);

    i32 i = 1,
	keyLength = hash_table_string_length(key),
	index = get_shash(key, header->Capacity, 0);
    InternalStringElement* elem = (InternalStringElement*) (table + index * header->ElementSize);

    while (elem->Key != NULL)
    {
	if (hash_table_string_compare_length(elem->Key, key, keyLength))
	{
	    goto return_label;
	}

	index = get_shash(elem->Key, header->Capacity, i);
	elem = (InternalStringElement*) (table + (index) * header->ElementSize);

	++i;
    }

return_label:
    header->Index = index;
#if HASH_TABLE_PROFILING == 1
    g_Statistics.GetAttempt = i;
#endif

    return table;
}

static i32
_base_shash_geti(void* table, const char* key)
{
    if (table == NULL)
    {
	return -1;
    }

    TableHeader* header = table_header(table);

    i32 i = 1,
	keyLength = hash_table_string_length(key),
	index = get_shash(key, header->Capacity, 0);
    InternalStringElement* elem = (InternalStringElement*) (table + index * header->ElementSize);

    while (elem->Key != NULL)
    {
	if (hash_table_string_compare_length(elem->Key, key, keyLength))
	{
	    return 1;
	}

	index = get_shash(elem->Key, header->Capacity, i);
	elem = (InternalStringElement*) (table + (index) * header->ElementSize);

	++i;
    }

    return -1;
}

/*
  Int Hash Table (int Key)
*/

force_inline i32
hash(i32 key)
{
    u32 ukey = (u32) key;
    ukey = ((ukey >> 16) ^ ukey) * 0x45d9f3b;
    ukey = ((ukey >> 16) ^ ukey) * 0x45d9f3b;
    ukey = (ukey >> 16) ^ ukey;
    return i32(key % I32_MAX_HALF);
}

force_inline i32
hash_new(i32 key)
{
    u32 ukey = (u32) key;

    ukey += ~(key << 9);
    ukey ^= ((key >> 14) | (key << 18));
    ukey += (key << 4);
    ukey ^= ((key >> 10) | (key << 22));

    return i32(key);
}

force_inline i32
get_hash(i32 key, i32 bucketNumber, i32 attempt)
{
    i32 hashA = hash(key);
    i32 hashB = attempt * (hash(key) + 1);
    return (hashA + hashB) % bucketNumber;
}

static void*
_base_hash_put(void* table, i32 key)
{
    assert(table);

    TableHeader* header = table_header(table);

    i32 i = 0, index, itemsKey = key;
    do
    {
	index = get_hash(itemsKey, header->Capacity, i);
	itemsKey = *((i32*) (table + index * header->ElementSize));
	if (itemsKey == key)
	{
	    goto return_label;
	}
	++i;
    } while (itemsKey != -1);

return_label:
    header->Index = index;
    ++header->Count;

#if HASH_TABLE_PROFILING == 1
    g_Statistics.PutAttempt = i;
#endif

    return table;
}

static void*
_base_hash_get(void* table, i32 key)
{
    if (table == NULL)
    {
	return NULL;
    }

    TableHeader* header = table_header(table);
    i32 i = 0, index, itemsKey = key;
    InternalIntElement* elem;
    do
    {
	index = get_hash(itemsKey, header->Capacity, i);
	elem = (InternalIntElement*) (table + index * header->ElementSize);
	if (elem->Key == key)
	{
	    goto return_label;
	}
	++i;
    } while (elem != NULL && elem->Key != -1);

return_label:
    header->Index = index;

#if HASH_TABLE_PROFILING == 1
    g_Statistics.GetAttempt = i;
#endif

    return table;
}

static i32
_base_hash_geti(void* table, i32 key)
{
    if (table == NULL)
    {
	return -1;
    }

    TableHeader* header = table_header(table);
    i32 i = 0, index, itemsKey = key;
    InternalIntElement* elem;
    do
    {
	index = get_hash(itemsKey, header->Capacity, i);
	elem = (InternalIntElement*) (table + index * header->ElementSize);
	if (elem->Key == key)
	{
	    return 1;
	}
	itemsKey = elem->Key;
	++i;
    } while (elem != NULL && elem->Key != -1);

    return -1;
}

#endif // HashTable.h

/*

		 #####################################
		 #####################################
			       IO.h
		 #####################################
		 #####################################

*/
#ifndef IO_H
#define IO_H

static char*
file_get_name_with_extension(const char* path)
{
    i32 i, nameIndex, newLength,
	lastIndex = 0, pathLength = string_length(path);
    char* fileName;

    for (i = 0; i < pathLength; i++)
    {
	if (path[i] == '/')
	{
	    lastIndex = i;
	}
    }

    if (lastIndex != 0)
    {
	nameIndex = (lastIndex + 1);
    }
    else
    {
	nameIndex = 0;
    }

    newLength = pathLength - nameIndex;
    fileName = malloc((newLength + 1) * sizeof(char));
    for (i = nameIndex; i < pathLength; i++)
    {
	fileName[i - nameIndex] = path[i];
    }
    fileName[newLength] = '\0';
    return fileName;
}

static void
file_write_string(const char* filePath, char* data, i32 len)
{
    FILE* file;
    file = fopen(filePath, "w");
    fwrite(data, 1, len, file);
    fclose(file);
}

static char*
file_read_string(const char* filePath)
{
    FILE* file;
    char* result;
    i32 fileLength;

    file = fopen(filePath, "r");
    if (file)
    {
	fseek(file, 0, SEEK_END);
	fileLength = (ftell(file));
	fseek(file, 0, SEEK_SET);
	result = malloc((fileLength + 1) * sizeof(char));

	fread(result, sizeof(char), (fileLength), file);
	result[fileLength] = '\0';

	fclose(file);
	return((char*)result);
    }

    return NULL;
}

static u8*
file_read_bytes(const char* filePath)
{
    FILE* file;
    u8* result;
    i32 fileLength;

    file = fopen(filePath, "rb");
    if (file)
    {
	fseek(file, 0, SEEK_END);
	fileLength = (i32)ftell(file);
	fseek(file, 0, SEEK_SET);
	result = malloc(fileLength * sizeof(u8));

	fread(result, sizeof(u8), fileLength, file);

	fclose(file);
	return((u8*)result);
    }

    return NULL;
}

static i32
file_get_size(const char* filePath)
{
    FILE* file = fopen(filePath, "rb");
    if (file)
    {
	fseek(file, 0, SEEK_END);
	i32 fileLength = (i32)ftell(file);
	return fileLength;
    }

    return 0;
}

#endif // IO.h

/*

		 #####################################
		 #####################################
			       Path.h
		 #####################################
		 #####################################

*/
#ifndef PATH_H
#define PATH_H

typedef struct IElement
{
    i32 AbsolutePathLength;
    i32 NameLength;
    i32 DirLength;
    char* AbsolutePath;
    char* Directory;
    char* Name;
    // NOTE(bies); Extension is part of AbsolutePath
    char* NameWithExtension;
    // NOTE(bies); Extension is part of Filename
    char* Extension;
} IElement;

#define ielement_header(e) ((IElement*) (((u8*)e) - sizeof(IElement)))
#define ielement_absolute_length(e) ((e != NULL)? ielement_header(e)->AbsolutePathLength : 0)
#define ielement_name_length(e) ((e != NULL)? ielement_header(e)->NameLength : 0)
#define ielement_directory_length(e) ((e != NULL)? ielement_header(e)->DirLength : 0)
#define ielement_absolute_path(e) ((e != NULL)? ielement_header(e)->AbsolutePath : 0)
#define ielement_directory(e) ((e != NULL)? ielement_header(e)->Directory : 0)
#define ielement_name(e) ((e != NULL)? ielement_header(e)->Name : 0)
#define ielement_name_with_extension(e) ((e != NULL)? ielement_header(e)->NameWithExtension : 0)
#define ielement_extension(e) ((e != NULL)? ielement_header(e)->Extension : 0)

enum Path
{
    PATH_IS_SOMETHING = 0,
    PATH_IS_FILE,
    PATH_IS_DIRECTORY
};

static char g_CurrentDirectory[4096] = "\0";

#ifdef LINUX_PLATFORM

#include <assert.h>
#include <unistd.h>
#include <pwd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

#define PATH_SEPARATOR_STRING "/"
#define ROOT_DIRECTORY "/"

static IElement** g_Elements = NULL;
static struct passwd* g_UserInfo = NULL;

static char*
ielement(const char* directory, const char* name)
{
    i32 i, count = array_count(g_Elements);
    for (i = 0; i < count; ++i)
    {
	IElement* ielement = g_Elements[i];
	if (string_compare(ielement->Directory, directory)
	    && string_compare(ielement->Name, name))
	{
	    return ielement->AbsolutePath;
	}
    }

    i32 lastSlashIndex = string_last_index_of(name, '/');
    i32 extIndex = string_last_index_of(name, '.');

    i32 dirLength = string_length(directory);
    i32 nameLength = string_length(name);
    i32 absoluteLength = dirLength + 1 + nameLength;

    if (lastSlashIndex != -1)
    {
	--absoluteLength;
    }

    size_t headerSize = sizeof(IElement);
    i32 dirSize = dirLength * sizeof(char);
    size_t nameSize = nameLength * sizeof(char);
    size_t absolutePathSize = absoluteLength * sizeof(char);

/*
      /home/bies/dir/file.ext

      char* AbsolutePath;       /home/bies/dir/file.ext => new
      char* Name;               file\0                  => new
      char* Directory;          /home/bies/dir/\0       => new
      char* NameWithExtension;  file.ext\0              => poap
      char* Extension;          .ext\0                  => poap
*/

    IElement* element = memory_allocate(headerSize
					+ absolutePathSize + 1
					+ nameSize + 1
					+ dirSize + 1);
    element->AbsolutePathLength = absoluteLength;
    element->DirLength = dirLength;
    element->NameLength = nameLength;
    element->AbsolutePath = ((void*) element) + headerSize;
    element->Directory = ((void*) element->AbsolutePath) + absolutePathSize + 1;
    element->Name = ((void*) element->Directory) + dirSize + 1;
    if (lastSlashIndex != -1)
    {
	element->NameWithExtension = element->AbsolutePath + lastSlashIndex + 1;
    }
    else
    {
	element->NameWithExtension = STRING_NULL;
    }
    if (extIndex != -1 && extIndex != 0)
    {
	element->Extension = element->Name + extIndex + 1;
    }
    else
    {
	element->Extension = STRING_NULL;
    }

    memset(element->AbsolutePath, '\0', (absolutePathSize + 1));
    memset(element->Directory, '\0', (dirSize + 1));
    memset(element->Name, '\0', (nameSize + 1));

    char lastChar = directory[dirLength - 1];
    memcpy(element->AbsolutePath, directory, dirSize);
    if (lastChar != '/' && lastChar != '\\' )
    {
	memcpy(element->AbsolutePath + dirSize, "/", sizeof(char));
	memcpy(element->AbsolutePath + dirSize + 1, name, nameSize);
    }
    else
    {
	memcpy(element->AbsolutePath + dirSize, name, nameSize);
    }
    memcpy(element->Directory, directory, dirSize);
    memcpy(element->Name, name, nameSize);

    array_push(g_Elements, element);

    return element->AbsolutePath;
}

static void
ielement_free_all()
{
    i32 i, count = array_count(g_Elements);
    for (i = 0; i < count; ++i)
    {
	IElement* element = g_Elements[i];
	memory_free(element);
    }

    array_free(g_Elements);

    g_Elements = NULL;
}

force_inline u8
path(const char* path)
{
    struct stat fileInfo;

    if (stat(path, &fileInfo) != 0)
    {
	return PATH_IS_SOMETHING;
    }

    if (S_ISDIR(fileInfo.st_mode))
    {
	return PATH_IS_DIRECTORY;
    }
    else if (S_ISREG(fileInfo.st_mode))
    {
	return PATH_IS_FILE;
    }

    return PATH_IS_SOMETHING;
}

force_inline char*
path_get_home_directory()
{
    if (!g_UserInfo)
    {
	g_UserInfo = getpwuid(geteuid());
    }

    return g_UserInfo->pw_dir;
}

force_inline const char*
path_get_extension(const char* path)
{
    i32 extensionIndex = string_last_index_of(path, '.');
    return (const char*) (path + extensionIndex * sizeof(char));
}

force_inline const char*
path_get_name(const char* path)
{
    i32 extensionIndex = string_last_index_of(path, '/');
    return (const char*) (path + (extensionIndex + 1) * sizeof(char));
}

force_inline char*
path_combine(const char* left, const char* right)
{
    assert(left != NULL && "Left can't be NULL!");
    assert(right != NULL && "Right can't be NULL!");

    i32 leftLength = string_length(left);
    i32 rightLength = string_length(right);
    char lastChar = left[leftLength - 1];

    if (lastChar == '/' || lastChar == '/')
    {
	return string_concat(left, right);
    }

    i32 middleLength = string_length(PATH_SEPARATOR_STRING);
    return string_concat3l(left, PATH_SEPARATOR_STRING, right, leftLength, middleLength, rightLength);
}

force_inline const char*
path_combine_interning(const char* left, const char* right)
{
    assert(left != NULL && "Left can't be NULL!");
    assert(right != NULL && "Right can't be NULL!");

    char* path = path_combine(left, right);
    const char* iPath = istring(path);
    memory_free(path);
    return iPath;
}

static const char*
path_get_current_directory()
{
    if (g_CurrentDirectory[0] == '\0')
    {
	getcwd(g_CurrentDirectory, 4096);
    }
    return (const char*) g_CurrentDirectory;
}

force_inline char*
path_get_absolute(char* path)
{
    const char* currentDirectory = path_get_current_directory();
    char* absolutePath = string_concat3(currentDirectory, PATH_SEPARATOR_STRING, path);
    return absolutePath;
}

static i32
path_is_file_exist(const char* path)
{
    struct stat buf;
    i32 result = stat(path, &buf);
    if (result == -1)
	return 0;
    else
	return 1;
}

static i32
path_is_directory_exist(const char* path)
{
    return 0;
}

static char*
path_get_filename(const char* path)
{
    i32 index = string_last_index_of(path, '/');
    if (index)
    {
	char* result = string_substring(path, (index + 1));
	return result;
    }

    return NULL;
}

static const char*
path_get_filename_interning(const char* path)
{
    //index can be -1
    i32 index = string_last_index_of(path, '/');
    const char* iPath = istring(path + index + 1);
    return iPath;
}

static char*
path_get_prev_directory(const char* currentDirectory)
{
    if (!string_compare(currentDirectory, ROOT_DIRECTORY))
    {
	i32 index = string_last_index_of(currentDirectory, '/');
	if (index != 0)
	{
	    --index;
	}

	char* prevDirectoryPath = string_substring_range(currentDirectory, 0, index);
	return prevDirectoryPath;
    }

    return NULL;
}

static const char*
path_get_prev_directory_interning(const char* currentDirectory)
{
    char* prevDirectory = path_get_prev_directory(currentDirectory);
    const char* iPrevDirectory = istring(prevDirectory);
    memory_free(prevDirectory);
    return iPrevDirectory;
}

static i32
path_string_comparer(const struct dirent **a, const struct dirent **b)
{
    char* left  = (char*) (*a)->d_name;
    char* right = (char*) (*b)->d_name;
    u32 leftLength = string_length(left);
    u32 rightLength = string_length(right);

    for (u32 i = 0; i < leftLength; i++)
    {
	char l = char_to_lower(left[i]);
	char r = char_to_lower(right[i]);

	if (l < r)
	{
	    return 1;
	}
	else if (l > r)
	{
	    return -1;
	}
    }

    return (rightLength - leftLength);
}

force_inline const char**
_directory_get(const char* directory, i32 elemCode)
{
    const char** elements = NULL;
    struct dirent** namelist = NULL;
    i32 n = scandir(directory, &namelist, 0, path_string_comparer);

    while (n > 0)
    {
	const char* dName = namelist[n - 1]->d_name;
	assert(dName);

	char* absolutePath = ielement(directory, dName);
	if (path(absolutePath) == elemCode)
	{
	    array_push(elements, absolutePath);
	}

	free(namelist[n - 1]);

	--n;
    }

    free(namelist);

    return elements;
}

static const char**
directory_get_files(const char* directory)
{
    const char** files = _directory_get(directory, PATH_IS_FILE);
    return files;
}

static const char**
directory_get_directories(const char* directory)
{
    const char** dirs = _directory_get(directory, PATH_IS_DIRECTORY);
    return dirs;
}

#elif defined(WINDOWS_PLATFORM)
#error "Windows sucks!"
#include "Windows.h"
#include <shlwapi.h>
#define PATH_SEPARATOR_STRING "/"
#define ROOT_DIRECTORY "/"
#else
#error "Platform unsupported"
#endif

#endif //Path.h

/*

		 #####################################
		 #####################################
			      Profiler.h
		 #####################################
		 #####################################

*/
#ifndef PROFILER_H
#define PROFILER_H

/*

USAGE(bies):

TimeState state;
profiler_start(&state);
// do smth we want profile
profiler_end(&state);

state.Result; <- time in ns
i64 ms = profiler_get_microseconds(&state);

*/

#define PROFILER_NS_TO_S(ns)   (ns / (1000 * 1000 * 1000))
#define PROFILER_NS_TO_MS(ns)  (ns / (1000 * 1000))
#define PROFILER_NS_TO_MCS(ns) (ns / (1000))

typedef enum ProfilerTimeType
{
    PROFILER_TIME_NS = 0,
    PROFILER_TIME_MCS,
    PROFILER_TIME_MS,
    PROFILER_TIME_S,
} ProfilerTimeType;

#ifdef LINUX_PLATFORM

#include <time.h>
static i64 g_DeltaInNanoseconds;
static struct timespec g_StartTime;
static struct timespec g_EndTime;

typedef struct TimeState
{
    struct timespec Start;
    struct timespec End;
    i64 Result;
} TimeState;

force_inline void
profiler_start(TimeState* state)
{
    clock_gettime(CLOCK_REALTIME, &state->Start);
}

force_inline void
profiler_end(TimeState* state)
{
    clock_gettime(CLOCK_REALTIME, &state->End);
    state->Result = (1000 * 1000 * 1000 * (state->End.tv_sec - state->Start.tv_sec)) + (state->End.tv_nsec - state->Start.tv_nsec);
}


#elif WINDOWS_PLATFORM

// NOTE: check if delta in nanoseconds
static i64 g_StartTime;
static i64 g_EndTime;
#include <profileapi.h>

typedef struct TimeState
{
    LARGE_INTEGER Start;
    LARGE_INTEGER End;
    LARGE_INTEGER Result;
} TimeState;

force_inline void
profiler_start(TimeState* state)
{
    QueryPerformanceCounter(&state->Start);
}

force_inline void
profiler_end(TimeState* state)
{
    QueryPerformanceCounter(&state->End);
    state->Result = state->End - state->Start;
}

#endif

force_inline ProfilerTimeType
profiler_get_time_type(TimeState* state)
{
    if (PROFILER_NS_TO_S(state->Result))
    {
	return PROFILER_TIME_S;
    }
    else if (PROFILER_NS_TO_MS(state->Result))
    {
	return PROFILER_TIME_MS;
    }
    else if (PROFILER_NS_TO_MCS(state->Result))
    {
	return PROFILER_TIME_MCS;
    }
    else if (state->Result)
    {
	return PROFILER_TIME_NS;
    }

    assert(0 && "");
    return PROFILER_TIME_NS;
}

force_inline i64
profiler_get_nanoseconds(TimeState* state)
{
    return state->Result;
}

force_inline i64
profiler_get_microseconds(TimeState* state)
{
    return state->Result / 1000;
}

force_inline i64
profiler_get_milliseconds(TimeState* state)
{
    return state->Result / (1000 * 1000);
}

force_inline i64
profiler_get_seconds(TimeState* state)
{
    return state->Result / (1000 * 1000 * 1000);
}

force_inline f64
profiler_get_microseconds_as_float(TimeState* state)
{
    return ((f64)state->Result) / 1000;
}

force_inline f64
profiler_get_milliseconds_as_float(TimeState* state)
{
    return ((f64)state->Result) / (1000 * 1000);
}

force_inline f64
profiler_get_seconds_as_float(TimeState* state)
{
    return ((f64)state->Result) / (1000 * 1000 * 1000);
}

force_inline void
profiler_print(TimeState* state)
{
    ProfilerTimeType timeType = profiler_get_time_type(state);
    switch (timeType)
    {
    case PROFILER_TIME_NS:
	printf("%ld %s\n", profiler_get_nanoseconds(state), "ns");
	break;
    case PROFILER_TIME_MCS:
	printf("%ld %s\n", profiler_get_microseconds(state), "mcs");
	break;
    case PROFILER_TIME_MS:
	printf("%ld %s\n", profiler_get_milliseconds(state), "ms");
	break;
    case PROFILER_TIME_S:
	printf("%ld %s\n", profiler_get_seconds(state), "s");
	break;
    default:
	assert(0 && "Just a thing to delete compiler warning message, this code never ever ll be executed!");
	break;
    }
}

force_inline void
profiler_print_as_float(TimeState* state)
{
    ProfilerTimeType timeType = profiler_get_time_type(state);

    switch (timeType)
    {
    case PROFILER_TIME_NS:
	printf("%ld %s\n", profiler_get_nanoseconds(state), "ns");
	break;
    case PROFILER_TIME_MCS:
	printf("%.4f %s\n", profiler_get_microseconds_as_float(state), "mcs");
	break;
    case PROFILER_TIME_MS:
	printf("%.4f %s\n", profiler_get_milliseconds_as_float(state), "ms");
	break;
    case PROFILER_TIME_S:
	printf("%.4f %s\n", profiler_get_seconds_as_float(state), "s");
	break;
    default:
	assert(0 && "Just a thing to delete compiler warning message, this code never ever ll be executed!");
	break;
    }
}

static char g_TimeString[512];

force_inline char*
profiler_get_string(TimeState* state)
{
    ProfilerTimeType timeType = profiler_get_time_type(state);

    switch (timeType)
    {
    case PROFILER_TIME_NS:
	string_format(g_TimeString, "%ld %s", profiler_get_nanoseconds(state), "ns");
	break;
    case PROFILER_TIME_MCS:
	string_format(g_TimeString, "%ld %s", profiler_get_microseconds(state), "mcs");
	break;
    case PROFILER_TIME_MS:
	string_format(g_TimeString, "%ld %s", profiler_get_milliseconds(state), "ms");
	break;
    case PROFILER_TIME_S:
	string_format(g_TimeString, "%ld %s", profiler_get_seconds(state), "s");
	break;
    default:
	assert(0 && "Just a thing to delete compiler warning message, this code never ever ll be executed!");
	break;
    }

    return (char*)g_TimeString;
}

force_inline char*
profiler_get_string_as_float(TimeState* state)
{
    ProfilerTimeType timeType = profiler_get_time_type(state);

    switch (timeType)
    {
    case PROFILER_TIME_NS:
    {
	f64 temp = profiler_get_nanoseconds(state);
	string_format(g_TimeString, "%f %s\n", temp, "ns");
	break;
    }
    case PROFILER_TIME_MCS:
    {
	f64 temp = profiler_get_microseconds_as_float(state);
	string_format(g_TimeString, "%f %s\n", temp, "mcs");
	break;
    }
    case PROFILER_TIME_MS:
    {
	f64 temp = profiler_get_milliseconds_as_float(state);
	string_format(g_TimeString, "%f %s\n", temp, "ms");
	break;
    }
    case PROFILER_TIME_S:
    {
	f64 temp = profiler_get_seconds_as_float(state);
	string_format(g_TimeString, "%f %s\n", temp, "s");
	break;
    }
    default:
    {
	assert(0 && "Just a thing to delete compiler warning message, this code never ever ll be executed!");
	break;
    }
    }

    return (char*)g_TimeString;
}


#endif // Profiler.h

/*

		 ###################################
		 ###################################
			     JsonParser.h
		 ###################################
		 ###################################

*/
#ifndef JSON_PARSER_H
#define JSON_PARSER_H

typedef enum JsonTokenType
{
    JSON_TOKEN_TYPE_NONE = 0,
    JSON_TOKEN_TYPE_KEYVALUE_SEPARATOR,
    JSON_TOKEN_TYPE_STRING,
    JSON_TOKEN_TYPE_INT_NUMBER,
    JSON_TOKEN_TYPE_FLOAT_NUMBER,
    JSON_TOKEN_TYPE_BOOL_TRUE,
    JSON_TOKEN_TYPE_BOOL_FALSE,
    JSON_TOKEN_TYPE_NULL,
    JSON_TOKEN_TYPE_OBJECT_OPEN_BRACKET,
    JSON_TOKEN_TYPE_OBJECT_CLOSE_BRACKET,
    JSON_TOKEN_TYPE_ARRAY_OPEN_BRACKET,
    JSON_TOKEN_TYPE_ARRAY_CLOSE_BRACKET,
    JSON_TOKEN_TYPE_COMMA,

    JSON_TOKEN_TYPE_COUNT
} JsonTokenType;
typedef struct JsonToken
{
    JsonTokenType Type;
    void* Data;
} JsonToken;

typedef enum JsonValueType
{
    JSON_VALUE_TYPE_INT,
    JSON_VALUE_TYPE_FLOAT,
    JSON_VALUE_TYPE_BOOL,
    JSON_VALUE_TYPE_NULL,
    JSON_VALUE_TYPE_STRING,
    JSON_VALUE_TYPE_I32_ARRAY,
    JSON_VALUE_TYPE_F32_ARRAY,
    JSON_VALUE_TYPE_STRING_ARRAY,
    JSON_VALUE_TYPE_OBJECT
} JsonValueType;
typedef struct JsonValue
{
    JsonValueType Type;
    void* Data;
} JsonValue;
typedef struct JsonObject
{
    const char** Keys;
    JsonValue* Values;
} JsonObject;

typedef struct JsonParser
{
    char* SourceContent;
    JsonToken* Tokens;
    JsonObject* Object;
} JsonParser;

/*
  CREATE

  void json_add_key_value(JsonObject* object, char* key, void* data)
  {
  array_push(object->Keys, key);
  array_push(object->Values, data);
  }

*/

#define JSON_INT(num)				\
    ({						\
	i32* alloc = memory_allocate_type(i32);	\
	*alloc = num;				\
	JsonValue value;			\
	value.Data = alloc;			\
	value.Type = JSON_VALUE_TYPE_INT;	\
	value;					\
    })
#define JSON_FLOAT(num)				\
    ({						\
	f32* alloc = memory_allocate_type(f32);	\
	*alloc = num;				\
	JsonValue value;			\
	value.Data = alloc;			\
	value.Type = JSON_VALUE_TYPE_FLOAT;	\
	value;					\
    })
#define JSON_STRING(str)			\
    ({						\
	char* sstr = string(str);		\
	JsonValue value;			\
	value.Data = sstr;			\
	value.Type = JSON_VALUE_TYPE_STRING;	\
	value;					\
    })
#define JSON_TRUE(str)				\
    ({						\
	JSON_STRING("TRUE");			\
    })
#define JSON_FALSE(str)				\
    ({						\
	JSON_STRING("FALSE");			\
    })
#define JSON_NULL()				\
    ({						\
	JsonValue value;			\
	value.Data = (void*) istring("NULL");	\
	value.Type = JSON_VALUE_TYPE_NULL;	\
	value;					\
    })
#define JSON_I32_ARRAY(arr)			\
    ({						\
	JsonValue value;			\
	value.Data = arr;			\
	value.Type = JSON_VALUE_TYPE_I32_ARRAY;	\
	value;					\
    })
#define JSON_F32_ARRAY(arr)			\
    ({						\
	JsonValue value;			\
	value.Data = arr;			\
	value.Type = JSON_VALUE_TYPE_F32_ARRAY;	\
	value;					\
    })
//TODO(bies): optimize this
#define JSON_F32_ARRAY_NEW(arr, count)		\
    ({						\
	i32 i;					\
	f32* oldArr = (f32*)arr;		\
	f32* newArr = NULL;			\
	array_reserve(newArr, count);		\
	for(i = 0; i < count; ++i)		\
	{					\
	    array_push(newArr, oldArr[i]);	\
	}					\
	JsonValue value;			\
	value.Data = newArr;			\
	value.Type = JSON_VALUE_TYPE_F32_ARRAY;	\
	value;					\
    })
#define JSON_STRING_ARRAY(arr)				\
    ({							\
	JsonValue value;				\
	value.Data = arr;				\
	value.Type = JSON_VALUE_TYPE_STRING_ARRAY;	\
	value;						\
    })
#define JSON_OBJECT(obj)			\
    ({						\
	JsonValue value;			\
	value.Data = obj;			\
	value.Type = JSON_VALUE_TYPE_OBJECT;	\
	value;					\
    })

/*
  PUBLIC JSON UTILS
*/
static const char* json_token_to_string(JsonToken token);
static char* json_tokens_to_string(JsonToken* tokens);
static void json_object_free(JsonObject* obj);

force_inline JsonObject*
json_object_create()
{
    JsonObject* obj = memory_allocate_type(JsonObject);
    obj->Keys = NULL;
    obj->Values = NULL;
    return obj;
}

force_inline void
json_object_destroy(JsonObject* obj)
{
    i32 i, count = array_count(obj->Values);
    for (i = 0; i < count; ++i)
    {
	JsonValue value = obj->Values[i];
	if (value.Type == JSON_VALUE_TYPE_OBJECT)
	{
	    json_object_free((JsonObject*)value.Data);
	}
	else if (value.Type == JSON_VALUE_TYPE_I32_ARRAY
		 || value.Type == JSON_VALUE_TYPE_F32_ARRAY
		 || value.Type == JSON_VALUE_TYPE_STRING_ARRAY)
	{
	    array_free(value.Data);
	}
    }

    memory_free(obj);
}

typedef enum JsonLetterType
{
    JSON_LETTER_TYPE_NONE = 0,
    JSON_LETTER_TYPE_OPEN_BRACKET,
    JSON_LETTER_TYPE_CLOSE_BRACKET,
    JSON_LETTER_TYPE_OPEN_STRING
} JsonLetterType;

static const char* TrueKeyWord;
static const char* FalseKeyWord;
static const char* NullKeyWord;

force_inline void
_register_keywords()
{
    TrueKeyWord = istring("true");
    FalseKeyWord = istring("false");
    NullKeyWord = istring("null");
}

force_inline void
_skip_spaces(char* stream)
{
    while (*stream == ' ' && *stream == '\n' && *stream == '\t' && *stream == '\r')
    {
	++stream;
    }
}

force_inline i32
_json_is_array_of_type(JsonToken* tokens, JsonTokenType type, i32 index)
{
    i32 count = array_count(tokens);
    while (1)
    {
	JsonToken token = tokens[index];
	++index;

	if (token.Type == JSON_TOKEN_TYPE_ARRAY_CLOSE_BRACKET)
	{
	    return 1;
	}

	if (token.Type == JSON_TOKEN_TYPE_COMMA)
	    continue;

	if (token.Type != type || index >= count)
	    return 0;
    }
}

force_inline JsonValue
_json_create_array(JsonToken* tokens, JsonTokenType type, i32* index)
{
    JsonValue result;

    switch (type)
    {
    case JSON_TOKEN_TYPE_STRING:
	result.Type = JSON_VALUE_TYPE_STRING_ARRAY;
	break;
    case JSON_TOKEN_TYPE_INT_NUMBER:
	result.Type = JSON_VALUE_TYPE_INT;
	break;
    case JSON_TOKEN_TYPE_FLOAT_NUMBER:
	result.Type = JSON_VALUE_TYPE_FLOAT;
	break;
    }

    void* data = NULL;
    i32 i = *index;

    while (1)
    {
	JsonToken token = tokens[i];

	switch (token.Type)
	{
	case JSON_TOKEN_TYPE_STRING:
	{
	    char** strData = (char**) data;
	    result.Type = JSON_VALUE_TYPE_STRING_ARRAY;
	    array_push(strData, ((char*)(token.Data)));
	    data = strData;
	    break;
	}
	case JSON_TOKEN_TYPE_INT_NUMBER:
	{
	    i32* iData = (i32*) data;
	    result.Type = JSON_VALUE_TYPE_I32_ARRAY;
	    array_push(iData, void_to_i32(token.Data));
	    data = iData;
	    break;
	}
	case JSON_TOKEN_TYPE_FLOAT_NUMBER:
	{
	    f32* fData = (f32*) data;
	    result.Type = JSON_VALUE_TYPE_F32_ARRAY;
	    array_push(fData, void_to_f32(token.Data));
	    data = fData;
	    break;
	}
	case JSON_TOKEN_TYPE_COMMA:
	    ++i;
	    continue;
	case JSON_TOKEN_TYPE_ARRAY_CLOSE_BRACKET:
	    goto endLabel;
	default:
	    assert(0 && "We don't support this type of arrays");
	    break;
	}

	++i;
    }

    //TODO (bies): delete this in the future
    assert(tokens[i].Type == JSON_TOKEN_TYPE_ARRAY_CLOSE_BRACKET);

endLabel:
    result.Data = data;
    *index = i;

    return result;
}

static JsonToken
_json_read_string_token(char* stream, i32* length)
{
    const i32 bufferSize = 256;
    char tempStringBuffer[bufferSize];
    i32 index = 0;

    memset(tempStringBuffer, '\0', bufferSize*sizeof(char));

    if (stream[1] == '\\')
    {
	JsonToken stringToken;
	stringToken.Type = JSON_TOKEN_TYPE_STRING;
	stringToken.Data = "";
	*length = 0;
	return stringToken;
    }

    /*

      we are here
      |
      v
      "string content"

    */
    ++stream;

    while (*stream != '"' && *stream != '\0')
    {
	tempStringBuffer[index] = *stream;
	++index;
	++stream;
    }

    *length = index + 1;

    /*

      we are here (we ++ in main loop)
      |
      v
      "string content"

    */

    JsonToken stringToken;
    stringToken.Type = JSON_TOKEN_TYPE_STRING;
    stringToken.Data = string(tempStringBuffer);

    return stringToken;
}

static JsonToken
_json_read_number_token(char* stream, i32* length, i32 negativePart)
{
    /*
      0123.456
      ^
    */
    i32 isIntPart = 1;
    i32 intIndex = 0;
    i32 floatIndex = 0;
    char c = *stream;
    const i32 intBufferLength = 128;
    const i32 floatBufferLength = 128;
    char intBuffer[intBufferLength];
    char floatBuffer[floatBufferLength];

    memset(intBuffer  , '\0', intBufferLength   * sizeof(char));
    memset(floatBuffer, '\0', floatBufferLength * sizeof(char));

    while ((c >= '0' && c <= '9') || c == '.')
    {
	switch (c)
	{
	case '0': case '1': case '2':
	case '3': case '4': case '5':
	case '6': case '7': case '8':
	case '9':
	{
	    if (isIntPart)
	    {
		intBuffer[intIndex] = c;
		++intIndex;
	    }
	    else
	    {
		floatBuffer[floatIndex] = c;
		++floatIndex;
	    }
	    break;
	}

	case '.':
	{
	    if (!isIntPart)
	    {
		assert(0 && "Two dots in number are not acceptable!");
	    }

	    floatBuffer[floatIndex] = '0';
	    ++floatIndex;
	    floatBuffer[floatIndex] = '.';
	    ++floatIndex;

	    isIntPart = 0;

	    break;
	}
	}

	++stream;
	c = *stream;
    }

    *length = intIndex - 1;

    f32 result = string_to_i32(intBuffer);

    if (!isIntPart)
    {
	f32 floatPart = string_to_f32(floatBuffer);
	result += floatPart;
	*length += floatIndex - 1;
    }

    result *= negativePart;

    JsonToken token;
    if (!isIntPart)
    {
	token.Data = memory_allocate_type(f32);
	*((f32*)token.Data) = result;
	token.Type = JSON_TOKEN_TYPE_FLOAT_NUMBER;
    }
    else
    {
	token.Data = memory_allocate_type(i32);
	*((i32*)token.Data) = result;
	token.Type = JSON_TOKEN_TYPE_INT_NUMBER;
    }

    return token;
}

force_inline i32
_json_check_key_word(char* stream, const char* keyWord)
{
    i32 i;
    char* ptr = stream;
    i32 keyWordLength = istring_length(keyWord);

    for (i = 0; i < keyWordLength; ++i)
    {
	if (*ptr != keyWord[i])
	{
	    return 0;
	}

	++ptr;
    }

    return 1;
}

static i32
_json_next_bool(char* stream)
{
    char* ptr = stream;
    ++ptr;

    if (*ptr != 'r') return 0;
    if (*ptr != 'u') return 0;
    if (*ptr != 'e') return 0;

    return 1;
}

static JsonToken*
_json_read_tokens(JsonParser* parser, char* stream)
{
    char c;
    JsonToken* tokens = NULL;

    while ((c = *stream) != '\0')
    {
	_skip_spaces(stream);

	switch (c)
	{
	case '{':
	{
	    JsonToken token;
	    token.Type = JSON_TOKEN_TYPE_OBJECT_OPEN_BRACKET;
	    token.Data = "{";
	    array_push(tokens, token);
	    break;
	}

	case '}':
	{
	    JsonToken token;
	    token.Type = JSON_TOKEN_TYPE_OBJECT_CLOSE_BRACKET;
	    token.Data = "}";
	    array_push(tokens, token);
	    break;
	}

	case ',':
	{
	    JsonToken token;
	    token.Type = JSON_TOKEN_TYPE_COMMA;
	    token.Data = ",";
	    array_push(tokens, token);
	    break;
	}

	case '"':
	{
	    i32 length;
	    JsonToken stringToken = _json_read_string_token(stream, &length);
	    array_push(tokens, stringToken);

	    stream += length;

	    break;
	}

	case ':':
	{
	    JsonToken keyValueToken;
	    keyValueToken.Type = JSON_TOKEN_TYPE_KEYVALUE_SEPARATOR;
	    keyValueToken.Data = ":";
	    array_push(tokens, keyValueToken);
	    break;
	}

	case 't':
	{
	    if (_json_check_key_word(stream, TrueKeyWord))
	    {
		JsonToken trueValueToken;
		trueValueToken.Type = JSON_TOKEN_TYPE_BOOL_TRUE;
		trueValueToken.Data = (void*)TrueKeyWord;
		stream += 2;
		array_push(tokens, trueValueToken);
	    }
	    break;
	}
	case 'f':
	{
	    if (_json_check_key_word(stream, FalseKeyWord))
	    {
		JsonToken falseValueToken;
		falseValueToken.Type = JSON_TOKEN_TYPE_BOOL_FALSE;
		falseValueToken.Data = (void*)FalseKeyWord;
		stream += 3;
		array_push(tokens, falseValueToken);
	    }
	    break;
	}
	case 'n':
	{
	    if (_json_check_key_word(stream, NullKeyWord))
	    {
		JsonToken token;
		token.Type = JSON_TOKEN_TYPE_NULL;
		token.Data = (void*)NullKeyWord;
		stream += 3;
		array_push(tokens, token);
	    }
	    break;
	}

	case '[':
	{
	    JsonToken token;
	    token.Type = JSON_TOKEN_TYPE_ARRAY_OPEN_BRACKET;
	    token.Data = "[";
	    array_push(tokens, token);
	    break;
	}

	case ']':
	{
	    JsonToken token;
	    token.Type = JSON_TOKEN_TYPE_ARRAY_CLOSE_BRACKET;
	    token.Data = "]";
	    array_push(tokens, token);
	    break;
	}

	case '0': case '1': case '2':
	case '3': case '4': case '5':
	case '6': case '7': case '8':
	case '9':
	{
	    i32 length, negativePart = (*(stream - 1) == '-') ? -1 : 1;

	    JsonToken token = _json_read_number_token(stream, &length, negativePart);
	    array_push(tokens, token);
	    stream += length;

	    break;
	}

	}

	++stream;
    }

    return tokens;
}

static JsonValue _json_parse_value(JsonToken* tokens, i32* index);

static JsonObject*
json_parser_create_json_object(JsonToken* tokens, i32* index)
{
    i32 i = *index,
	isAfterKey = 0,
	count = array_count(tokens);
    JsonToken token;
    JsonObject* object = memory_allocate_type(JsonObject);
    object->Keys = NULL;
    object->Values = NULL;

    while (1)
    {
	token = tokens[i];

	if (token.Type == JSON_TOKEN_TYPE_OBJECT_CLOSE_BRACKET || i >= count)
	    break;

	if (!isAfterKey && token.Type == JSON_TOKEN_TYPE_STRING)
	{
	    array_push(object->Keys, token.Data);
	    isAfterKey = 1;
	}
	else if (isAfterKey && token.Type != JSON_TOKEN_TYPE_KEYVALUE_SEPARATOR && token.Type != JSON_TOKEN_TYPE_COMMA)
	{
	    // json value can contains a json object
	    JsonValue value = _json_parse_value(tokens, &i);
	    array_push(object->Values, value);
	    isAfterKey = 0;
	}

	++i;
    }

    *index = i;

    return object;
}

static JsonValue
_json_parse_value(JsonToken* tokens, i32* index)
{
    JsonValue result;
    i32 i = *index;
    JsonToken token = tokens[i];

    switch (token.Type)
    {
    case JSON_TOKEN_TYPE_NULL:
    {
	result.Type = JSON_VALUE_TYPE_NULL;
	result.Data = token.Data;
	break;
    }

    case JSON_TOKEN_TYPE_STRING:
    {
	result.Type = JSON_VALUE_TYPE_STRING;
	result.Data = token.Data;
	break;
    }

    case JSON_TOKEN_TYPE_INT_NUMBER:
    {
	result.Type = JSON_VALUE_TYPE_INT;
	result.Data = token.Data;
	break;
    }

    case JSON_TOKEN_TYPE_FLOAT_NUMBER:
    {
	result.Type = JSON_VALUE_TYPE_FLOAT;
	result.Data = token.Data;
	break;
    }

    case JSON_TOKEN_TYPE_BOOL_TRUE:
    case JSON_TOKEN_TYPE_BOOL_FALSE:
    {
	result.Type = JSON_VALUE_TYPE_BOOL;
	result.Data = token.Data;
	break;
    }

    case JSON_TOKEN_TYPE_ARRAY_OPEN_BRACKET:
    {
	++i;

	if (_json_is_array_of_type(tokens, JSON_TOKEN_TYPE_STRING, i))
	{
	    result = _json_create_array(tokens, JSON_TOKEN_TYPE_STRING, &i);
	}
	else if (_json_is_array_of_type(tokens, JSON_TOKEN_TYPE_INT_NUMBER, i))
	{
	    result = _json_create_array(tokens, JSON_TOKEN_TYPE_INT_NUMBER, &i);
	}
	else if (_json_is_array_of_type(tokens, JSON_TOKEN_TYPE_FLOAT_NUMBER, i))
	{
	    result = _json_create_array(tokens, JSON_TOKEN_TYPE_FLOAT_NUMBER, &i);
	}
	else
	{
	    assert(0 && "Not supported array!");
	}

	break;
    }

    case JSON_TOKEN_TYPE_OBJECT_OPEN_BRACKET:
    {
	result.Type = JSON_VALUE_TYPE_OBJECT;
	result.Data = json_parser_create_json_object(tokens, &i);
	assert(tokens[i].Type == JSON_TOKEN_TYPE_OBJECT_CLOSE_BRACKET);
	break;
    }

    }

    *index = i;

    return result;
}

static void
json_parse_string(JsonParser* parser, char* string)
{
    _register_keywords();

    assert(string != NULL && "JsonParser can't parse NULL string!");

    i32 index = 0;
    parser->SourceContent = string;
    parser->Tokens = _json_read_tokens(parser, string);
    parser->Object = json_parser_create_json_object(parser->Tokens, &index);
}

static void
json_parse_file(JsonParser* parser, const char* path)
{
    char* data = file_read_string(path);
    json_parse_string(parser, data);
}

static void
json_destroy(JsonParser* parser)
{
    JsonObject* obj = parser->Object;

    json_object_destroy(obj);

    memory_free(parser->SourceContent);
    memory_free(parser->Tokens);

    parser->SourceContent = NULL;
    parser->Object = NULL;
    parser->Tokens = NULL;
}

static void
json_write_file(JsonParser* parser, const char* path)
{
    char* sb = NULL;
    i32 i, count = array_count(parser->Tokens),
	isInArray = 0;

    char intBuffer[32];
    char floatBuffer[32];
    i32 indentationLevel = 0;
    const char* tab4 = "    ";

    for (i = 0; i < count; ++i)
    {
	JsonToken token = parser->Tokens[i];

	if (token.Type == JSON_TOKEN_TYPE_OBJECT_OPEN_BRACKET)
	{
	    ++indentationLevel;
	}
	else if (token.Type == JSON_TOKEN_TYPE_OBJECT_CLOSE_BRACKET)
	{
	    --indentationLevel;
	}

	if (token.Type == JSON_TOKEN_TYPE_ARRAY_OPEN_BRACKET)
	    isInArray = 1;
	else if (token.Type == JSON_TOKEN_TYPE_ARRAY_CLOSE_BRACKET)
	    isInArray = 0;

	switch (token.Type)
	{
	case JSON_TOKEN_TYPE_INT_NUMBER:
	{
	    memset(intBuffer, '\0', sizeof(intBuffer));
	    i32 number = void_to_i32(token.Data);
	    string_i32(intBuffer, number);
	    string_builder_appends(sb, intBuffer);
	    break;
	}

	case JSON_TOKEN_TYPE_FLOAT_NUMBER:
	{
	    memset(floatBuffer, '\0', sizeof(floatBuffer));
	    f32 number = void_to_f32(token.Data);
	    string_f64(floatBuffer, number);
	    string_builder_appends(sb, floatBuffer);
	    break;
	}

	case JSON_TOKEN_TYPE_KEYVALUE_SEPARATOR:
	{
	    string_builder_appends(sb, token.Data);
	    string_builder_appends(sb, " ");
	    break;
	}

	case JSON_TOKEN_TYPE_STRING:
	{
	    string_builder_appends(sb, "\"");
	    string_builder_appends(sb, token.Data);
	    string_builder_appends(sb, "\"");

	    break;
	}

	default:
	{
	    string_builder_appends(sb, token.Data);
	    break;
	}
	}

	if ((token.Type == JSON_TOKEN_TYPE_COMMA
	     || token.Type == JSON_TOKEN_TYPE_OBJECT_OPEN_BRACKET)
	    && !isInArray)
	{
	    string_builder_appends(sb, "\n");
	    for (i32 i = 0; i < indentationLevel; i++)
		string_builder_appends(sb, tab4);
	}

	if (parser->Tokens[i + 1].Type == JSON_TOKEN_TYPE_OBJECT_OPEN_BRACKET)
	{
	    string_builder_appends(sb, "\n");
	    for (i32 i = 0; i < indentationLevel; i++)
		string_builder_appends(sb, tab4);
	}
	else if (parser->Tokens[i + 1].Type == JSON_TOKEN_TYPE_OBJECT_CLOSE_BRACKET)
	{
	    string_builder_appends(sb, "\n");
	    for (i32 i = 0; i < (indentationLevel - 1); i++)
		string_builder_appends(sb, tab4);
	}
    }

    file_write_string(path, sb, string_builder_count(sb));
}

static JsonValue
json_object_get_value(JsonObject* object, char* key, JsonValueType type)
{
    const char** keys = object->Keys;
    i32 i, count = array_count(keys);
    for (i = 0; i < count; ++i)
    {
	if (string_compare(keys[i], key))
	{
	    return object->Values[i];
	}
    }

    assert(0 && "Expected key that exist in current object!");
    return (JsonValue) {};
}

#define ind(sb, indentation)			\
    ({						\
	const char* tab4 = "    ";		\
	for (i32 i = 0; i < indentation; ++i)	\
	{					\
	    string_builder_appends(sb, tab4);	\
	}					\
    })

static char*
json_object_to_string(JsonObject* root, i32 startIndentationLevel)
{
    char** keys = (char**)root->Keys;
    JsonValue* values = (JsonValue*)root->Values;

    assert(keys != NULL && "Keys is NULL wtf");
    assert(values != NULL && "Values is NULL wtf");

    char* sb = NULL;
    i32 indentationLevel = startIndentationLevel;

    ind(sb, indentationLevel);

    string_builder_appends(sb, "{\n");
    ++indentationLevel;

    i32 keysCount = array_count(keys);
    for (i32 i = 0; i < keysCount; ++i)
    {
	char* key = keys[i];
	JsonValue value = values[i];

	ind(sb, indentationLevel);

	string_builder_appendf(sb, "\"%s\": ", key);

	switch (value.Type)
	{
	case JSON_VALUE_TYPE_INT:
	{
	    string_builder_appendf(sb, "%d", void_to_i32(value.Data));
	    break;
	}
	case JSON_VALUE_TYPE_FLOAT:
	{
	    string_builder_appendf(sb, "%f", void_to_f32(value.Data));
	    break;
	}
	case JSON_VALUE_TYPE_NULL:
	case JSON_VALUE_TYPE_BOOL:
	{
	    string_builder_appendf(sb, "%s", void_to_string(value.Data));
	    break;
	}
	case JSON_VALUE_TYPE_STRING:
	{
	    string_builder_appendf(sb, "\"%s\"", void_to_string(value.Data));
	    break;
	}
	case JSON_VALUE_TYPE_I32_ARRAY:
	{
	    i32* array = (i32*)value.Data;
	    i32 count = array_count(array);
	    string_builder_appendc(sb, '[');
	    for (i32 i = 0; i < count; ++i)
	    {
		if (i < (count - 1))
		{
		    string_builder_appendf(sb, "%d, ", array[i]);
		}
		else
		{
		    string_builder_appendf(sb, "%d", array[i]);
		}
	    }
	    string_builder_appendc(sb, ']');
	    break;
	}
	case JSON_VALUE_TYPE_F32_ARRAY:
	{
	    f32* array = (f32*)value.Data;
	    i32 count = array_count(array);
	    string_builder_appendc(sb, '[');
	    for (i32 i = 0; i < count; ++i)
	    {
		if (i < (count - 1))
		{
		    string_builder_appendf(sb, "%f, ", array[i]);
		}
		else
		{
		    string_builder_appendf(sb, "%f", array[i]);
		}
	    }
	    string_builder_appendc(sb, ']');
	    break;
	}
	case JSON_VALUE_TYPE_STRING_ARRAY:
	{
	    char** array = (char**)value.Data;
	    i32 count = array_count(array);
	    string_builder_appendc(sb, '[');
	    for (i32 i = 0; i < count; ++i)
	    {
		if (i < (count - 1))
		{
		    string_builder_appendf(sb, "\"%s\", ", array[i]);
		}
		else
		{
		    string_builder_appendf(sb, "\"%s\"", array[i]);
		}
	    }
	    string_builder_appendc(sb, ']');

	    break;
	}
	case JSON_VALUE_TYPE_OBJECT:
	{
	    JsonObject* obj = (JsonObject*) value.Data;
	    if (obj->Keys != NULL && obj->Values != NULL)
	    {
		char* objToStr = json_object_to_string(obj, indentationLevel);
		string_builder_appendf(sb, "\n%s", objToStr);
	    }
	    else
	    {
		string_builder_appends(sb, " \"NULL\"");
	    }

	    break;
	}

	}

	if (i < (keysCount - 1))
	{
	    string_builder_appends(sb, ",\n");
	}
	else
	{
	    string_builder_appends(sb, "\n");
	}
    }

    ind(sb, indentationLevel - 1);
    string_builder_appendc(sb, '}');

    return sb;
}

/*
  PUBLIC JSON UTILS
*/

static const char* g_TokensTypeToString[] = {
    "JSON_TOKEN_TYPE_NONE",
    "JSON_TOKEN_TYPE_KEYVALUE_SEPARATOR",
    "JSON_TOKEN_TYPE_STRING",
    "JSON_TOKEN_TYPE_INT_NUMBER",
    "JSON_TOKEN_TYPE_FLOAT_NUMBER",
    "JSON_TOKEN_TYPE_BOOL_TRUE",
    "JSON_TOKEN_TYPE_BOOL_FALSE",
    "JSON_TOKEN_TYPE_NULL",
    "JSON_TOKEN_TYPE_OBJECT_OPEN_BRACKET",
    "JSON_TOKEN_TYPE_OBJECT_CLOSE_BRACKET",
    "JSON_TOKEN_TYPE_ARRAY_OPEN_BRACKET",
    "JSON_TOKEN_TYPE_ARRAY_CLOSE_BRACKET",
    "JSON_TOKEN_TYPE_COMMA"
};

static const char*
json_token_to_string(JsonToken token)
{
    return g_TokensTypeToString[token.Type];
}

static char*
json_tokens_to_string(JsonToken* tokens)
{
    i32 i;
    i32 count = array_count(tokens);
    char* sb = NULL;
    i32 indentation = 0;

    for (i = 0; i < count; ++i)
    {
	JsonToken token = tokens[i];

	switch (token.Type)
	{
	case JSON_TOKEN_TYPE_OBJECT_OPEN_BRACKET:
	{
	    if (i != 0)
		string_builder_appends(sb, "\n");
	    ind(sb, indentation);
	    string_builder_appends(sb, "{\n");
	    ++indentation;
	    break;
	}
	case JSON_TOKEN_TYPE_OBJECT_CLOSE_BRACKET:
	{
	    --indentation;
	    ind(sb, indentation);
	    string_builder_appends(sb, "}");
	    break;
	}
	case JSON_TOKEN_TYPE_ARRAY_OPEN_BRACKET:
	{
	    string_builder_appends(sb, "[ ");
	    ++i;
	    while (1)
	    {
		token = tokens[i];

		if (token.Type == JSON_TOKEN_TYPE_ARRAY_CLOSE_BRACKET)
		{
		    break;
		}

		if (token.Type == JSON_TOKEN_TYPE_COMMA)
		{
		    ++i;
		    continue;
		}

		const char* tokenStr = NULL;// = json_token_to_string(token);

		switch (token.Type)
		{
		case JSON_TOKEN_TYPE_STRING:
		{
		    tokenStr = token.Data;
		    break;
		}
		case JSON_TOKEN_TYPE_INT_NUMBER:
		{
		    tokenStr = "I32";
		    break;
		}
		case JSON_TOKEN_TYPE_FLOAT_NUMBER:
		{
		    tokenStr = "F32";
		    break;
		}
		}

		if (tokens[i + 1].Type != JSON_TOKEN_TYPE_ARRAY_CLOSE_BRACKET)
		{
		    string_builder_appendf(sb, "%s, ", tokenStr);
		}
		else
		{
		    string_builder_appendf(sb, "%s", tokenStr);
		}

		++i;
	    }

	    string_builder_appends(sb, " ]");

	    break;
	}
	case JSON_TOKEN_TYPE_COMMA:
	{
	    string_builder_appends(sb, ",\n");
	    break;
	}
	case JSON_TOKEN_TYPE_KEYVALUE_SEPARATOR:
	{
	    string_builder_appends(sb, ": ");
	    break;
	}
	case JSON_TOKEN_TYPE_STRING:
	{
	    if (tokens[i - 1].Type != JSON_TOKEN_TYPE_KEYVALUE_SEPARATOR)
		ind(sb, indentation);
	    string_builder_appendf(sb, "\"%s\"", token.Data);
	    break;
	}
	case JSON_TOKEN_TYPE_INT_NUMBER:
	{
	    string_builder_appends(sb, "I32");
	    break;
	}
	case JSON_TOKEN_TYPE_FLOAT_NUMBER:
	{
	    string_builder_appends(sb, "F32");
	    break;
	}
	default:
	{
	    const char* tokenStr = json_token_to_string(token);
	    string_builder_appendf(sb, "%s", tokenStr);
	    break;
	}
	}

	if (tokens[i + 1].Type == JSON_TOKEN_TYPE_OBJECT_CLOSE_BRACKET)
	{
	    string_builder_appends(sb, "\n");
	}
    }

    return sb;
}

static void
json_object_free(JsonObject* obj)
{
    i32 i, count = array_count(obj->Values);
    for (i = 0; i < count; ++i)
    {
	JsonValue value = obj->Values[i];
	if (value.Type == JSON_VALUE_TYPE_OBJECT)
	{
	    json_object_free((JsonObject*)value.Data);
	}
	else if (value.Type == JSON_VALUE_TYPE_I32_ARRAY
		 || value.Type == JSON_VALUE_TYPE_F32_ARRAY
		 || value.Type == JSON_VALUE_TYPE_STRING_ARRAY)
	{
	    array_free(value.Data);
	}
    }

    array_free(obj->Keys);
    array_free(obj->Values);

    memory_free(obj);
}

static void
_json_print_value(JsonValue value)
{
    switch (value.Type)
    {
    case JSON_VALUE_TYPE_INT:
    {
	GERROR("JsonValue: %d\n", void_to_i32(value.Data));
	break;
    }

    case JSON_VALUE_TYPE_FLOAT:
    {
	GERROR("JsonValue: %f\n", void_to_f32(value.Data));
	break;
    }

    case JSON_VALUE_TYPE_NULL:
    {
	GERROR("JsonValue: NULL\n");
	break;
    }

    case JSON_VALUE_TYPE_BOOL:
    case JSON_VALUE_TYPE_STRING:
    {
	GERROR("JsonValue: %s\n", void_to_string(value.Data));
	break;
    }

    case JSON_VALUE_TYPE_I32_ARRAY:
    case JSON_VALUE_TYPE_F32_ARRAY:
    case JSON_VALUE_TYPE_STRING_ARRAY:
    {
	GERROR("JsonValue: THIS IS ARRAY\n");
	break;
    }

    case JSON_VALUE_TYPE_OBJECT:
    {
	GERROR("JsonValue: THIS IS OBJECT\n");
	break;
    }
    }
}

#endif

#endif
