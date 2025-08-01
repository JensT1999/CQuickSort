#include "quicksort.h"

typedef enum ShiftDirection {
    LEFT,
    RIGHT
} ShiftDirection;

static void quicksortIntern(char*, SortType, const size_t, const int64_t, const int64_t, int8_t (*cmp) (void*, void*));
static int64_t hoarePartition(char*, SortType, const size_t, const int64_t, const int64_t, int8_t (*cmp) (void*, void*));

static bool tryShift(char*, SortType, const size_t, int64_t*, const int64_t, int8_t (*cmp) (void*, void*), ShiftDirection);
static inline bool tryShiftPointer(char*, int64_t*, const void*, const size_t, int8_t (*cmp) (void*, void*), ShiftDirection);

static void swap(char*, SortType, const size_t, int64_t, int64_t);
static void swapMemoryInArray(char*, const size_t, int64_t, int64_t);

static int8_t (*getDefaultComparator(SortType)) (void*, void*);
static int8_t compareDoubles(void*, void*);
static int8_t compareFloats(void*, void*);
static int8_t compareInts(void*, void*);
static int8_t compareInts8(void*, void*);
static int8_t compareInts16(void*, void*);
static int8_t compareInts32(void*, void*);
static int8_t compareInts64(void*, void*);
static int8_t compareUInts8(void*, void*);
static int8_t compareUInts16(void*, void*);
static int8_t compareUInts32(void*, void*);
static int8_t compareUInts64(void*, void*);
static int8_t compareCharPtrs(void*, void*);

void quicksort(void *array, SortType type, const size_t byteSizeOfStruct,
    const int64_t length, int8_t (*cmp) (void*, void*)) {
    if((array == NULL) || (length == 0)) return;
    if((type == SPECIAL_STRUCT) && ((byteSizeOfStruct == 0) || (cmp == NULL))) return;
    if(cmp == NULL) cmp = getDefaultComparator(type);

    char *tempArray = (char* ) array;
    quicksortIntern(tempArray, type, byteSizeOfStruct, 0, length - 1, cmp);
}

/**
 * @brief Recursively calls itself until startingIndex equals endingIndex,
 * performing partitioning using the Hoare scheme along the way.
 * @param array array to be sorted
 * @param type type of array. Valid Types: DOUBLE, FLOAT, INT, INT8...INT64, UINT8...UINT64, SPECIAL_STRUCT
 * @param byteSizeOfStruct necessary when sorting struct array, otherwise insert 0
 * @param startingIndex the index from where the partition begins
 * @param endingIndex the index where the partition ends
 * @param cmp comparator for specific type, when NOT SPECIAL_STRUCT and NULL is inserted, a default comparator will
 * be used
 */
static void quicksortIntern(char *array, SortType type, const size_t byteSizeOfStruct,
    const int64_t startingIndex, const int64_t endingIndex, int8_t (*cmp) (void*, void*)) {
    if(startingIndex < endingIndex) {
        const int64_t partitionIndex = hoarePartition(array, type, byteSizeOfStruct, startingIndex, endingIndex, cmp);

        quicksortIntern(array, type, byteSizeOfStruct, startingIndex, partitionIndex, cmp);
        quicksortIntern(array, type, byteSizeOfStruct, partitionIndex + 1, endingIndex, cmp);
    }
}

/**
 * @brief Used to partition and sort the underlying array.
 * All elements smaller than the pivot element are placed on the left,
 * and all elements greater than the pivot element are placed on the right.
 * The first element will always be the pivot element.
 * @param array array to be partitioned
 * @param type type of array. Valid Types: DOUBLE, FLOAT, INT, INT8...INT64, UINT8...UINT64, SPECIAL_STRUCT
 * @param byteSizeOfStruct necessary when sorting struct array, otherwise insert 0
 * @param startingIndex the index from where the partition begins
 * @param endingIndex the index where the partition ends
 * @param cmp comparator for specific type, when NOT SPECIAL_STRUCT and NULL is inserted, a default comparator will
 * be used
 * @return partition index for next partition
 */
static int64_t hoarePartition(char *array, SortType type, const size_t byteSizeOfStruct,
    const int64_t startingIndex, const int64_t endingIndex, int8_t (*cmp) (void*, void*)) {
    const int64_t pivotIndex = startingIndex;
    int64_t pointerLeft = (startingIndex - 1);
    int64_t pointerRight = (endingIndex + 1);

    bool pointerLeftMoving = true;
    bool pointerRightMoving = true;

    while(true) {
        do {
            pointerLeftMoving = tryShift(array, type, byteSizeOfStruct, &pointerLeft, pivotIndex, cmp, RIGHT);
        } while(pointerLeftMoving);

        do {
            pointerRightMoving = tryShift(array, type, byteSizeOfStruct, &pointerRight, pivotIndex, cmp, LEFT);
        } while(pointerRightMoving);

        if(pointerLeft >= pointerRight)
            return pointerRight;

        swap(array, type, byteSizeOfStruct, pointerLeft, pointerRight);
        pointerLeftMoving = false;
        pointerRightMoving = false;
    }
}

/**
 * @brief Attempts to perform a shift of one of the two pointers used in the Hoare algorithm.
 * @param array array to be partitioned
 * @param type type of array. Valid Types: DOUBLE, FLOAT, INT, INT8...INT64, UINT8...UINT64, SPECIAL_STRUCT
 * @param byteSizeOfStruct necessary when sorting struct array, otherwise insert 0
 * @param ptrIndex index of left or right pointer
 * @param pivotIndex index of pivot value/element
 * @param cmp comparator for specific type, when NOT SPECIAL_STRUCT and NULL is inserted, a default comparator will
 * be used
 * @param shiftDirection direction of shift e.g. left Pointer -> performs "left shift", right pointer -> performs
 * "right shift"
 * @return true -> if pointer can move further, false -> if pointer can not move further
 */
static bool tryShift(char *array, SortType type, const size_t byteSizeOfStruct,
    int64_t *ptrIndex, const int64_t pivotIndex, int8_t (*cmp) (void*, void*),
    ShiftDirection shiftDirection) {

    if(shiftDirection == RIGHT) {
        *ptrIndex += 1;
    } else if(shiftDirection == LEFT) {
        *ptrIndex -= 1;
    }

    switch (type) {
    case DOUBLE: {
        const size_t pivotPos = ((size_t) pivotIndex * SIZE_OF_DOUBLE);
        const void *pivotElement = array + pivotPos;

        return tryShiftPointer(array, ptrIndex, pivotElement, SIZE_OF_DOUBLE, cmp, shiftDirection);
    }

    case FLOAT: {
        const size_t pivotPos = ((size_t) pivotIndex * SIZE_OF_FLOAT);
        const void *pivotElement = array + pivotPos;

        return tryShiftPointer(array, ptrIndex, pivotElement, SIZE_OF_FLOAT, cmp, shiftDirection);
    }

    case INT: {
        const size_t pivotPos = ((size_t) pivotIndex * SIZE_OF_INT);
        const void *pivotElement = array + pivotPos;

        return tryShiftPointer(array, ptrIndex, pivotElement, SIZE_OF_INT, cmp, shiftDirection);
    }

    case INT8: {
        const size_t pivotPos = ((size_t) pivotIndex * SIZE_OF_INT8);
        const void *pivotElement = array + pivotPos;

        return tryShiftPointer(array, ptrIndex, pivotElement, SIZE_OF_INT8, cmp, shiftDirection);
    }

    case INT16: {
        const size_t pivotPos = ((size_t) pivotIndex * SIZE_OF_INT16);
        const void *pivotElement = array + pivotPos;

        return tryShiftPointer(array, ptrIndex, pivotElement, SIZE_OF_INT16, cmp, shiftDirection);
    }

    case INT32: {
        const size_t pivotPos = ((size_t) pivotIndex * SIZE_OF_INT32);
        const void *pivotElement = array + pivotPos;

        return tryShiftPointer(array, ptrIndex, pivotElement, SIZE_OF_INT32, cmp, shiftDirection);
    }

    case INT64: {
        const size_t pivotPos = ((size_t) pivotIndex * SIZE_OF_INT64);
        const void *pivotElement = array + pivotPos;

        return tryShiftPointer(array, ptrIndex, pivotElement, SIZE_OF_INT64, cmp, shiftDirection);
    }

    case UINT8: {
        const size_t pivotPos = ((size_t) pivotIndex * SIZE_OF_UINT8);
        const void *pivotElement = array + pivotPos;

        return tryShiftPointer(array, ptrIndex, pivotElement, SIZE_OF_UINT8, cmp, shiftDirection);
    }

    case UINT16: {
        const size_t pivotPos = ((size_t) pivotIndex * SIZE_OF_UINT16);
        const void *pivotElement = array + pivotPos;

        return tryShiftPointer(array, ptrIndex, pivotElement, SIZE_OF_UINT16, cmp, shiftDirection);
    }

    case UINT32: {
        const size_t pivotPos = ((size_t) pivotIndex * SIZE_OF_UINT32);
        const void *pivotElement = array + pivotPos;

        return tryShiftPointer(array, ptrIndex, pivotElement, SIZE_OF_UINT32, cmp, shiftDirection);
    }

    case UINT64: {
        const size_t pivotPos = ((size_t) pivotIndex * SIZE_OF_UINT64);
        const void *pivotElement = array + pivotPos;

        return tryShiftPointer(array, ptrIndex, pivotElement, SIZE_OF_UINT64, cmp, shiftDirection);
    }

    case CHARPTR_ARRAY: {
        const size_t pivotPos = ((size_t) pivotIndex * SIZE_OF_CHAR_PTR);
        const void *pivotElement = array + pivotPos;

        return tryShiftPointer(array, ptrIndex, pivotElement, SIZE_OF_CHAR_PTR, cmp, shiftDirection);
    }

    case SPECIAL_STRUCT: {
        const size_t pivotPos = ((size_t) pivotIndex * byteSizeOfStruct);
        const void *pivotElement = array + pivotPos;

        return tryShiftPointer(array, ptrIndex, pivotElement, byteSizeOfStruct, cmp, shiftDirection);
    }

    default:
        exit(EXIT_FAILURE);
    }
}

static inline bool tryShiftPointer(char *array, int64_t *ptrIndex, const void *pivotElement,
    const size_t byteSize, int8_t (*cmp) (void*, void*), ShiftDirection shiftDirection) {
    const size_t elementPos = (((size_t) *ptrIndex) * byteSize);
    const void* ptrCurrentElement = array + elementPos;

    const bool result = (shiftDirection == RIGHT) ? cmp((void*) pivotElement, (void*) ptrCurrentElement) > 0 :
        cmp((void*) pivotElement, (void*) ptrCurrentElement) < 0;

    return result;
}

/**
 * @brief Performs a swap of two elements in the array, provided they meet the conditions for
 * swapping according to the Hoare algorithm.
 * @param array array, where the swap happens
 * @param type type of array. Valid Types: DOUBLE, FLOAT, INT, INT8...INT64, UINT8...UINT64, SPECIAL_STRUCT
 * @param byteSizeOfStruct necessary when sorting struct array, otherwise insert 0
 * @param index1 index of first element to be swapped
 * @param index2 index of second element to be swapped
 */
static void swap(char *array, SortType type, const size_t byteSizeOfStruct, int64_t index1, int64_t index2) {
    switch (type) {
    case DOUBLE: {
        swapMemoryInArray(array, SIZE_OF_DOUBLE, index1, index2);
        break;
    }

    case FLOAT: {
        swapMemoryInArray(array, SIZE_OF_FLOAT, index1, index2);
        break;
    }

    case INT: {
        swapMemoryInArray(array, SIZE_OF_INT, index1, index2);
        break;
    }

    case INT8: {
        swapMemoryInArray(array, SIZE_OF_INT8, index1, index2);
        break;
    }

    case INT16: {
        swapMemoryInArray(array, SIZE_OF_INT16, index1, index2);
        break;
    }

    case INT32: {
        swapMemoryInArray(array, SIZE_OF_INT32, index1, index2);
        break;
    }

    case INT64: {
        swapMemoryInArray(array, SIZE_OF_INT64, index1, index2);
        break;
    }

    case UINT8: {
        swapMemoryInArray(array, SIZE_OF_UINT8, index1, index2);
        break;
    }

    case UINT16: {
        swapMemoryInArray(array, SIZE_OF_UINT16, index1, index2);
        break;
    }

    case UINT32: {
        swapMemoryInArray(array, SIZE_OF_UINT32, index1, index2);
        break;
    }

    case UINT64: {
        swapMemoryInArray(array, SIZE_OF_UINT64, index1, index2);
        break;
    }

    case CHARPTR_ARRAY: {
        swapMemoryInArray(array, SIZE_OF_CHAR_PTR, index1, index2);
        break;
    }

    case SPECIAL_STRUCT: {
        swapMemoryInArray(array, byteSizeOfStruct, index1, index2);
        break;
    }

    default:
        exit(EXIT_FAILURE);
    }
}

static void swapMemoryInArray(char* array, const size_t byteSize, int64_t index1, int64_t index2) {
        char buffer[byteSize];

    const size_t elementPosOne = ((size_t) index1 * byteSize);
    const size_t elementPosTwo = ((size_t) index2 * byteSize);

    memcpy(buffer, (array + elementPosOne), byteSize);
    memcpy((array + elementPosOne), (array + elementPosTwo), byteSize);
    memcpy((array + elementPosTwo), buffer, byteSize);
}

/**
 * @brief Get the default comparator function pointer.
 * @param type type of array. Valid Types: DOUBLE, FLOAT, INT, INT8...INT64, UINT8...UINT64
 * @return function pointer to the default comparator
 */
int8_t (*getDefaultComparator(SortType type)) (void*, void*) {
    switch (type)
    {
    case DOUBLE:
        return &compareDoubles;
    case FLOAT:
        return &compareFloats;
    case INT:
        return &compareInts;
    case INT8:
        return &compareInts8;
    case INT16:
        return &compareInts16;
    case INT32:
        return &compareInts32;
    case INT64:
        return &compareInts64;
    case UINT8:
        return &compareUInts8;
    case UINT16:
        return &compareUInts16;
    case UINT32:
        return &compareUInts32;
    case UINT64:
        return &compareUInts64;
    case CHARPTR_ARRAY:
        return &compareCharPtrs;
    default:
        exit(EXIT_FAILURE);
    }
}

static int8_t compareDoubles(void *pivotPtr, void *compareElementPtr) {
    const double d1Value = *((double* ) pivotPtr);
    const double d2Value = *((double* ) compareElementPtr);

    return (d1Value < d2Value) ? -1 : (d1Value == d2Value) ? 0 : 1;
}

static int8_t compareFloats(void *pivotPtr, void *compareElementPtr) {
    const float d1Value = *((float* ) pivotPtr);
    const float d2Value = *((float* ) compareElementPtr);

    return (d1Value < d2Value) ? -1 : (d1Value == d2Value) ? 0 : 1;
}

static int8_t compareInts(void *pivotPtr, void *compareElementPtr) {
    const int d1Value = *((int* ) pivotPtr);
    const int d2Value = *((int* ) compareElementPtr);

    return (d1Value < d2Value) ? -1 : (d1Value == d2Value) ? 0 : 1;
}

static int8_t compareInts8(void *pivotPtr, void *compareElementPtr) {
    const int8_t d1Value = *((int8_t* ) pivotPtr);
    const int8_t d2Value = *((int8_t* ) compareElementPtr);

    return (d1Value < d2Value) ? -1 : (d1Value == d2Value) ? 0 : 1;
}

static int8_t compareInts16(void *pivotPtr, void *compareElementPtr) {
    const int16_t d1Value = *((int16_t* ) pivotPtr);
    const int16_t d2Value = *((int16_t* ) compareElementPtr);

    return (d1Value < d2Value) ? -1 : (d1Value == d2Value) ? 0 : 1;
}

static int8_t compareInts32(void *pivotPtr, void *compareElementPtr) {
    const int32_t d1Value = *((int32_t* ) pivotPtr);
    const int32_t d2Value = *((int32_t* ) compareElementPtr);

    return (d1Value < d2Value) ? -1 : (d1Value == d2Value) ? 0 : 1;
}

static int8_t compareInts64(void *pivotPtr, void *compareElementPtr) {
    const int64_t d1Value = *((int64_t* ) pivotPtr);
    const int64_t d2Value = *((int64_t* ) compareElementPtr);

    return (d1Value < d2Value) ? -1 : (d1Value == d2Value) ? 0 : 1;
}

static int8_t compareUInts8(void *pivotPtr, void *compareElementPtr){
    const uint8_t d1Value = *((uint8_t* ) pivotPtr);
    const uint8_t d2Value = *((uint8_t* ) compareElementPtr);

    return (d1Value < d2Value) ? -1 : (d1Value == d2Value) ? 0 : 1;
}

static int8_t compareUInts16(void *pivotPtr, void *compareElementPtr) {
    const uint16_t d1Value = *((uint16_t* ) pivotPtr);
    const uint16_t d2Value = *((uint16_t* ) compareElementPtr);

    return (d1Value < d2Value) ? -1 : (d1Value == d2Value) ? 0 : 1;
}

static int8_t compareUInts32(void *pivotPtr, void *compareElementPtr) {
    const uint32_t d1Value = *((uint32_t* ) pivotPtr);
    const uint32_t d2Value = *((uint32_t* ) compareElementPtr);

    return (d1Value < d2Value) ? -1 : (d1Value == d2Value) ? 0 : 1;
}

static int8_t compareUInts64(void *pivotPtr, void *compareElementPtr) {
    const uint64_t d1Value = *((uint64_t* ) pivotPtr);
    const uint64_t d2Value = *((uint64_t* ) compareElementPtr);

    return (d1Value < d2Value) ? -1 : (d1Value == d2Value) ? 0 : 1;
}

static int8_t compareCharPtrs(void *pivotPtr, void *compareElementPtr) {
    char **pivotElement = (char** ) pivotPtr;
    char **comparedElement = (char** ) compareElementPtr;

    const int comparedValue = strcmp(*pivotElement, *comparedElement);
    return (comparedValue < 0) ? -1 : (comparedValue == 0) ? 0 : 1;
}
