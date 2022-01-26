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



#endif
