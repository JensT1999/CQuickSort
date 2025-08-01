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

- Allows custom comparator functions for specialized sorting needs

## Usage

### Preparation:

Please note that the algorithm is written in C99.
Copy both files 'quicksort.h' and 'quicksort.c' into your project. After that include the header 'quicksort.h'.
-> #include "quicksort.h"

### Calling:

You can call a quicksort like this:

	quicksort(ARRAY, TYPE_OF_ARRAY, BYTESIZEOFSTRUCT, ARRAY_LENGTH, COMPARATOR);

Please note:
- BYTESIZEOFSTRUCT is only necessary when you're trying to sort an array of structs
- When sorting structs, you must pass a comparator function pointer
- COMPARATOR can be NULL (NOT when TYPE_OF_ARRAY equals SPECIAL_STRUCT) -> in that case, a default comparator will be used

### Examples

1. Sorting a DOUBLE array:

```c
double[10] array  = { ... };
quicksort(array, DOUBLE, 0, 10, NULL);
```

2. Sorting a SPECIAL_STRUCT array with comparator:

```c
Person *arrayOfPersons = ...;
quicksort(arrayOfPersons, SPECIAL_STRUCT, sizeof(Person), 100, &comparePersons);

int8_t comparePersons(void *p1, void *p2) {
	const Person *person1 = (Person* ) p1;
       	const Person *person2 = (Person* ) p2;

        return (person1->age < person2->age) ? -1 :
           (person1->age == person2->age) ? 0 : 1;
}
```

### Contact

If you have any questions, feel free to contact me!






	
    
   

   
   

