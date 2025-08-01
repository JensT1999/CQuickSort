#ifndef QUICKSORT_H
#define QUICKSORT_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef enum SortType {
    DOUBLE,
    FLOAT,
    INT,
    INT8,
    INT16,
    INT32,
    INT64,
    UINT8,
    UINT16,
    UINT32,
    UINT64,
    SPECIAL_STRUCT
} SortType;

void quicksort(void *array, SortType type, const size_t byteSizeOfStruct,
     const size_t length, int (*cmp) (void*, void*));

#endif
