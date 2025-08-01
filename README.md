# Quicksort Library in C

A flexible quicksort algorithm based on Hoare’s partitioning scheme, capable of sorting any underlying array type.
Supported types include all common primitive types, e.g., double, float, int, int8_t, int16_t, int32_t, int64_t, uint8_t, uint16_t, uint32_t, and uint64_t.
In addition, it is possible to sort arrays of arbitrary structs.

## Functionality

### Supports sorting of arrays of types:
	double, float, int
	int8_t, int16_t, int32_t, int64_t
	uint8_t, uint16_t, uint32_t, uint64_t
	Custom structs (marked as SPECIAL_STRUCT)

### In addition:

- Allows custom comparator funtions for specialized sorting needs

## Usage

### Preparation:

Please note that the algorithm is written in C99.
Copy both files 'quicksort.h' and 'quicksort.c' into your project. After that include the header 'quicksort.h'.
-> #include "quicksort.h"

### Calling:

You can call a quicksort like this:

	quicksort(ARRAY, TYPE_OF_ARRAY, BYTESIZEOFSTRUCT, ARRAY_LENGTH, COMPARATOR);

Please note:
- BYTESIZEOFSTRUCT is only necessary when your trying to sort an array of structs.
- Also when sorting structs you have to point on a comparator function as function pointer.
- COMPARATOR can be NULL (NOT when TYPE_OF_ARRAY equals SPECIAL_STRUCT) -> if so a default comparator is provided.



