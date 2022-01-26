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

typedef struct InternalStringElement
{
    const char* Key;
    void* Data;
} InternalStringElement;

static i32 g_MAPrimes[] = {
    97, 193, 389,
    769, 1543, 3079, 6151,
    12289, 24593, 49157, 98317,
    196613, 393241, 786433, 1572869,
    3145739, 6291469, 12582917, 25165843,
    50331653, 100663319, 201326611, 402653189,
    805306457, 1610612741
};
static i32 g_NextPrimeIndex = 0;
static i32 g_PrimesCount = ARRAY_COUNT(g_MAPrimes);

force_inline i32
ma_get_prime(MATableHeader* header)
{
    i32 prime = g_MAPrimes[header->NextPrime];
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
    InternalStringElement* elem;
    do
    {
	index = ma_get_shash(key, capacity, i);
	elem = (InternalStringElement*) (table + index * elementSize);
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
    InternalStringElement* elem;
    do
    {
	index = ma_get_shash(key, capacity, i);
	elem = (InternalStringElement*) (table + index * elementSize);
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

#endif
