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

/**
 * @brief Performs a Quicksort for the respective type.
 * The Hoare partitioning scheme is used for dividing the array.
 * The pivot element is always the first element of the array or of the specific partition.
 * Any type of array can be used, including specific structs (e.g., SPECIAL_STRUCT as the type).
 * Important: When sorting structs, a comparator must be provided!
 * For all other types, NULL can be passed instead, in which case a default comparator will be used.
 *
 * @param array array to be sorted
 * @param type type of array. Valid Types: DOUBLE, FLOAT, INT, INT8...INT64, UINT8...UINT64, SPECIAL_STRUCT
 * @param byteSizeOfStruct necessary when sorting struct array, otherwise insert 0
 * @param length length of array
 * @param cmp comparator for specific type, when NOT SPECIAL_STRUCT and NULL is inserted, a default comparator will
 * be used
 */
void quicksort(void *array, SortType type, const size_t byteSizeOfStruct, const int64_t length, int8_t (*cmp) (void*, void*));

#endif
