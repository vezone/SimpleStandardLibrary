#include <stdlib.h>
#include <stdio.h>

#define memory_allocate(size) malloc(size)
#define memory_free(size) free(size)
#include "SimpleStandardLibrary.h"

int main()
{
    printf("Everything fine!\n");

    return 0;
}
