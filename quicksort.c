#include "quicksort.h"

typedef enum ShiftDirection {
    LEFT,
    RIGHT
} ShiftDirection;

static void quicksortIntern(void*, SortType, const size_t, const int64_t, const int64_t, int8_t (*cmp) (void*, void*));
static int64_t hoarePartition(void*, SortType, const size_t, const int64_t, const int64_t, int8_t (*cmp) (void*, void*));

static bool tryShift(void*, SortType, const size_t, int64_t*, const int64_t, int8_t (*cmp) (void*, void*), ShiftDirection);
static inline bool tryShiftDoubleType(double*, int64_t*, const double, int8_t (*cmp) (void*, void*), ShiftDirection);
static inline bool tryShiftFloatType(float*, int64_t*, const float, int8_t (*cmp) (void*, void*), ShiftDirection);
static inline bool tryShiftIntType(int*, int64_t*, const int, int8_t (*cmp) (void*, void*), ShiftDirection);
static inline bool tryShiftInt8Type(int8_t*, int64_t*, const int8_t, int8_t (*cmp) (void*, void*), ShiftDirection);
static inline bool tryShiftInt16Type(int16_t*, int64_t*, const int16_t, int8_t (*cmp) (void*, void*), ShiftDirection);
static inline bool tryShiftInt32Type(int32_t*, int64_t*, const int32_t, int8_t (*cmp) (void*, void*), ShiftDirection);
static inline bool tryShiftInt64Type(int64_t*, int64_t*, const int64_t, int8_t (*cmp) (void*, void*), ShiftDirection);
static inline bool tryShiftUInt8Type(uint8_t*, int64_t*, const uint8_t, int8_t (*cmp) (void*, void*), ShiftDirection);
static inline bool tryShiftUInt16Type(uint16_t*, int64_t*, const uint16_t, int8_t (*cmp) (void*, void*), ShiftDirection);
static inline bool tryShiftUInt32Type(uint32_t*, int64_t*, const uint32_t, int8_t (*cmp) (void*, void*), ShiftDirection);
static inline bool tryShiftUInt64Type(uint64_t*, int64_t*, const uint64_t, int8_t (*cmp) (void*, void*), ShiftDirection);
static inline bool tryShiftSpecialStruct(char*, int64_t*, const void*, const size_t, int8_t (*cmp) (void*, void*),
ShiftDirection);

static void swap(void*, SortType, const size_t, int64_t, int64_t);
static inline void swapDoubles(double*, int64_t, int64_t);
static inline void swapFloats(float*, int64_t, int64_t);
static inline void swapInts(int*, int64_t, int64_t);
static inline void swapInts8(int8_t*, int64_t, int64_t);
static inline void swapInts16(int16_t*, int64_t, int64_t);
static inline void swapInts32(int32_t*, int64_t , int64_t);
static inline void swapInts64(int64_t*, int64_t, int64_t);
static inline void swapUInts8(uint8_t*, int64_t, int64_t);
static inline void swapUInts16(uint16_t*, int64_t, int64_t);
static inline void swapUInts32(uint32_t*, int64_t, int64_t);
static inline void swapUInts64(uint64_t*, int64_t, int64_t);
static inline void swapSpecialStructs(char*, const size_t, int64_t, int64_t);

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

void quicksort(void *array, SortType type, const size_t byteSizeOfStruct,
    const int64_t length, int8_t (*cmp) (void*, void*)) {
    if((array == NULL) || (length == 0)) return;
    if((type == SPECIAL_STRUCT) && ((byteSizeOfStruct == 0) || (cmp == NULL))) return;
    if(cmp == NULL) cmp = getDefaultComparator(type);

    quicksortIntern(array, type, byteSizeOfStruct, 0, length - 1, cmp);
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
static void quicksortIntern(void *array, SortType type, const size_t byteSizeOfStruct,
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
static int64_t hoarePartition(void *array, SortType type, const size_t byteSizeOfStruct,
    const int64_t startingIndex, const int64_t endingIndex, int8_t (*cmp) (void*, void*)) {
    const int64_t pivotIndex = startingIndex;
    int64_t pointerLeft = (startingIndex - 1);
    int64_t pointerRight = (endingIndex + 1);

    bool pointerLeftMoving = true;
    bool pointerRightMoving = true;

    while(true) {
        do {
            pointerLeftMoving = tryShift(array, type, byteSizeOfStruct, &pointerLeft, pivotIndex, cmp, LEFT);
        } while(pointerLeftMoving);

        do {
            pointerRightMoving = tryShift(array, type, byteSizeOfStruct, &pointerRight, pivotIndex, cmp, RIGHT);
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
static bool tryShift(void *array, SortType type, const size_t byteSizeOfStruct,
    int64_t *ptrIndex, const int64_t pivotIndex, int8_t (*cmp) (void*, void*),
    ShiftDirection shiftDirection) {

    if(shiftDirection == LEFT) {
        *ptrIndex += 1;
    } else if(shiftDirection == RIGHT) {
        *ptrIndex -= 1;
    }

    switch (type) {
    case DOUBLE: {
        double *tempArray = (double* ) array;
        const double pivotValue = tempArray[pivotIndex];
        return tryShiftDoubleType(tempArray, ptrIndex, pivotValue, cmp, shiftDirection);
    }

    case FLOAT: {
        float *tempArray = (float* ) array;
        const float pivotValue = tempArray[pivotIndex];
        return tryShiftFloatType(tempArray, ptrIndex, pivotValue, cmp, shiftDirection);
    }

    case INT: {
        int *tempArray = (int* ) array;
        const int pivotValue = tempArray[pivotIndex];
        return tryShiftIntType(tempArray, ptrIndex, pivotValue, cmp, shiftDirection);
    }

    case INT8: {
        int8_t *tempArray = (int8_t* ) array;
        const int8_t pivotValue = tempArray[pivotIndex];
        return tryShiftInt8Type(tempArray, ptrIndex, pivotValue, cmp, shiftDirection);
    }

    case INT16: {
        int16_t *tempArray = (int16_t* ) array;
        const int16_t pivotValue = tempArray[pivotIndex];
        return tryShiftInt16Type(tempArray, ptrIndex, pivotValue, cmp, shiftDirection);
    }

    case INT32: {
        int32_t *tempArray = (int32_t* ) array;
        const int32_t pivotValue = tempArray[pivotIndex];
        return tryShiftInt32Type(tempArray, ptrIndex, pivotValue, cmp, shiftDirection);
    }

    case INT64: {
        int64_t *tempArray = (int64_t* ) array;
        const int64_t pivotValue = tempArray[pivotIndex];
        return tryShiftInt64Type(tempArray, ptrIndex, pivotValue, cmp, shiftDirection);
    }

    case UINT8: {
        uint8_t *tempArray = (uint8_t* ) array;
        const uint8_t pivotValue = tempArray[pivotIndex];
        return tryShiftUInt8Type(tempArray, ptrIndex, pivotValue, cmp, shiftDirection);
    }

    case UINT16: {
        uint16_t *tempArray = (uint16_t* ) array;
        const uint16_t pivotValue = tempArray[pivotIndex];
        return tryShiftUInt16Type(tempArray, ptrIndex, pivotValue, cmp, shiftDirection);
    }

    case UINT32: {
        uint32_t *tempArray = (uint32_t* ) array;
        const uint32_t pivotValue = tempArray[pivotIndex];
        return tryShiftUInt32Type(tempArray, ptrIndex, pivotValue, cmp, shiftDirection);
    }

    case UINT64: {
        uint64_t *tempArray = (uint64_t* ) array;
        const uint64_t pivotValue = tempArray[pivotIndex];
        return tryShiftUInt64Type(tempArray, ptrIndex, pivotValue, cmp, shiftDirection);
    }

    case SPECIAL_STRUCT: {
        const size_t elementPos = ((size_t) pivotIndex * byteSizeOfStruct);
        char *tempArray = (char* ) array;
        const void *pivotElement = tempArray + elementPos;
        return tryShiftSpecialStruct(tempArray, ptrIndex, pivotElement, byteSizeOfStruct, cmp, shiftDirection);
    }

    default:
        exit(EXIT_FAILURE);
    }
}

static inline bool tryShiftDoubleType(double *array, int64_t *ptrIndex, const double pivotValue,
    int8_t (*cmp) (void*, void*), ShiftDirection shiftDirection) {
    const double ptrCurrentValue = array[*ptrIndex];

    const bool result = (shiftDirection == LEFT) ? cmp((void*) &pivotValue, (void*) &ptrCurrentValue) > 0 :
        cmp((void*) &pivotValue, (void*) &ptrCurrentValue) < 0;

    return result;
}

static inline bool tryShiftFloatType(float *array, int64_t *ptrIndex, const float pivotValue,
    int8_t (*cmp) (void*, void*), ShiftDirection shiftDirection) {
    const float ptrCurrentValue = array[*ptrIndex];

    const bool result = (shiftDirection == LEFT) ? cmp((void*) &pivotValue, (void*) &ptrCurrentValue) > 0 :
        cmp((void*) &pivotValue, (void*) &ptrCurrentValue) < 0;

    return result;
}

static inline bool tryShiftIntType(int *array, int64_t *ptrIndex, const int pivotValue,
    int8_t (*cmp) (void*, void*), ShiftDirection shiftDirection) {
    const int ptrCurrentValue = array[*ptrIndex];

    const bool result = (shiftDirection == LEFT) ? cmp((void*) &pivotValue, (void*) &ptrCurrentValue) > 0 :
        cmp((void*) &pivotValue, (void*) &ptrCurrentValue) < 0;

    return result;
}

static inline bool tryShiftInt8Type(int8_t *array, int64_t *ptrIndex, const int8_t pivotValue,
    int8_t (*cmp) (void*, void*), ShiftDirection shiftDirection) {
    const int8_t ptrCurrentValue = array[*ptrIndex];

    const bool result = (shiftDirection == LEFT) ? cmp((void*) &pivotValue, (void*) &ptrCurrentValue) > 0 :
        cmp((void*) &pivotValue, (void*) &ptrCurrentValue) < 0;

    return result;
}

static inline bool tryShiftInt16Type(int16_t *array, int64_t *ptrIndex, const int16_t pivotValue,
    int8_t (*cmp) (void*, void*), ShiftDirection shiftDirection) {
    const int16_t ptrCurrentValue = array[*ptrIndex];

    const bool result = (shiftDirection == LEFT) ? cmp((void*) &pivotValue, (void*) &ptrCurrentValue) > 0 :
        cmp((void*) &pivotValue, (void*) &ptrCurrentValue) < 0;

    return result;
}

static inline bool tryShiftInt32Type(int32_t *array, int64_t *ptrIndex, const int32_t pivotValue,
    int8_t (*cmp) (void*, void*), ShiftDirection shiftDirection) {
    const int32_t ptrCurrentValue = array[*ptrIndex];

    const bool result = (shiftDirection == LEFT) ? cmp((void*) &pivotValue, (void*) &ptrCurrentValue) > 0 :
        cmp((void*) &pivotValue, (void*) &ptrCurrentValue) < 0;

    return result;
}

static inline bool tryShiftInt64Type(int64_t *array, int64_t *ptrIndex, const int64_t pivotValue,
    int8_t (*cmp) (void*, void*), ShiftDirection shiftDirection) {
    const int64_t ptrCurrentValue = array[*ptrIndex];

    const bool result = (shiftDirection == LEFT) ? cmp((void*) &pivotValue, (void*) &ptrCurrentValue) > 0 :
        cmp((void*) &pivotValue, (void*) &ptrCurrentValue) < 0;

    return result;
}

static inline bool tryShiftUInt8Type(uint8_t *array, int64_t *ptrIndex, const uint8_t pivotValue,
    int8_t (*cmp) (void*, void*), ShiftDirection shiftDirection) {
    const uint8_t ptrCurrentValue = array[*ptrIndex];

    const bool result = (shiftDirection == LEFT) ? cmp((void*) &pivotValue, (void*) &ptrCurrentValue) > 0 :
        cmp((void*) &pivotValue, (void*) &ptrCurrentValue) < 0;

    return result;
}

static inline bool tryShiftUInt16Type(uint16_t *array, int64_t *ptrIndex, const uint16_t pivotValue,
    int8_t (*cmp) (void*, void*), ShiftDirection shiftDirection) {
    const uint16_t ptrCurrentValue = array[*ptrIndex];

    const bool result = (shiftDirection == LEFT) ? cmp((void*) &pivotValue, (void*) &ptrCurrentValue) > 0 :
        cmp((void*) &pivotValue, (void*) &ptrCurrentValue) < 0;

    return result;
}

static inline bool tryShiftUInt32Type(uint32_t *array, int64_t *ptrIndex, const uint32_t pivotValue,
    int8_t (*cmp) (void*, void*), ShiftDirection shiftDirection) {
    const uint32_t ptrCurrentValue = array[*ptrIndex];

    const bool result = (shiftDirection == LEFT) ? cmp((void*) &pivotValue, (void*) &ptrCurrentValue) > 0 :
        cmp((void*) &pivotValue, (void*) &ptrCurrentValue) < 0;

    return result;
}

static inline bool tryShiftUInt64Type(uint64_t *array, int64_t *ptrIndex, const uint64_t pivotValue,
    int8_t (*cmp) (void*, void*), ShiftDirection shiftDirection) {
    const uint64_t ptrCurrentValue = array[*ptrIndex];

    const bool result = (shiftDirection == LEFT) ? cmp((void*) &pivotValue, (void*) &ptrCurrentValue) > 0 :
        cmp((void*) &pivotValue, (void*) &ptrCurrentValue) < 0;

    return result;
}

static inline bool tryShiftSpecialStruct(char *array, int64_t *ptrIndex, const void *pivotElement,
    const size_t byteSizeOfStruct, int8_t (*cmp) (void*, void*), ShiftDirection shiftDirection) {
    const size_t elementPos = (((size_t) *ptrIndex) * byteSizeOfStruct);
    const void *ptrCurrentElement = array + elementPos;

    const bool result = (shiftDirection == LEFT) ? cmp((void*) pivotElement, (void*) ptrCurrentElement) > 0 :
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
static void swap(void *array, SortType type, const size_t byteSizeOfStruct, int64_t index1, int64_t index2) {
    switch (type) {
    case DOUBLE: {
        double *doubleArray = (double* ) array;
        swapDoubles(doubleArray, index1, index2);
        break;
    }

    case FLOAT: {
        float *floatArray = (float* ) array;
        swapFloats(floatArray, index1, index2);
        break;
    }

    case INT: {
        int *intArray = (int* ) array;
        swapInts(intArray, index1, index2);
        break;
    }

    case INT8: {
        int8_t *intArray = (int8_t* ) array;
        swapInts8(intArray, index1, index2);
        break;
    }

    case INT16: {
        int16_t *intArray = (int16_t* ) array;
        swapInts16(intArray, index1, index2);
        break;
    }

    case INT32: {
        int32_t *intArray = (int32_t* ) array;
        swapInts32(intArray, index1, index2);
        break;
    }

    case INT64: {
        int64_t *intArray = (int64_t* ) array;
        swapInts64(intArray, index1, index2);
        break;
    }

    case UINT8: {
        uint8_t *intArray = (uint8_t* ) array;
        swapUInts8(intArray, index1, index2);
        break;
    }

    case UINT16: {
        uint16_t *intArray = (uint16_t* ) array;
        swapUInts16(intArray, index1, index2);
        break;
    }

    case UINT32: {
        uint32_t *intArray = (uint32_t* ) array;
        swapUInts32(intArray, index1, index2);
        break;
    }

    case UINT64: {
        uint64_t *intArray = (uint64_t* ) array;
        swapUInts64(intArray, index1, index2);
        break;
    }

    case SPECIAL_STRUCT: {
        char *tempBuffer = (char* ) array;
        swapSpecialStructs(tempBuffer, byteSizeOfStruct, index1, index2);
        break;
    }

    default:
        exit(EXIT_FAILURE);
    }
}

static inline void swapDoubles(double *array, int64_t index1, int64_t index2) {
    const double tempValueOne = array[index1];
    array[index1] = array[index2];
    array[index2] = tempValueOne;
}

static inline void swapFloats(float *array, int64_t index1, int64_t index2) {
    const float tempValueOne = array[index1];
    array[index1] = array[index2];
    array[index2] = tempValueOne;
}

static inline void swapInts(int *array, int64_t index1, int64_t index2) {
    const int tempValueOne = array[index1];
    array[index1] = array[index2];
    array[index2] = tempValueOne;
}

static inline void swapInts8(int8_t *array, int64_t index1, int64_t index2) {
    const int8_t tempValueOne = array[index1];
    array[index1] = array[index2];
    array[index2] = tempValueOne;
}

static inline void swapInts16(int16_t *array, int64_t index1, int64_t index2) {
    const int16_t tempValueOne = array[index1];
    array[index1] = array[index2];
    array[index2] = tempValueOne;
}

static inline void swapInts32(int32_t *array, int64_t index1, int64_t index2) {
    const int32_t tempValueOne = array[index1];
    array[index1] = array[index2];
    array[index2] = tempValueOne;
}

static inline void swapInts64(int64_t *array, int64_t index1, int64_t index2) {
    const int64_t tempValueOne = array[index1];
    array[index1] = array[index2];
    array[index2] = tempValueOne;
}

static inline void swapUInts8(uint8_t *array, int64_t index1, int64_t index2) {
    const uint8_t tempValueOne = array[index1];
    array[index1] = array[index2];
    array[index2] = tempValueOne;
}

static inline void swapUInts16(uint16_t *array, int64_t index1, int64_t index2) {
    const uint16_t tempValueOne = array[index1];
    array[index1] = array[index2];
    array[index2] = tempValueOne;
}

static inline void swapUInts32(uint32_t *array, int64_t index1, int64_t index2) {
    const uint32_t tempValueOne = array[index1];
    array[index1] = array[index2];
    array[index2] = tempValueOne;
}

static inline void swapUInts64(uint64_t *array, int64_t index1, int64_t index2) {
    const uint64_t tempValueOne = array[index1];
    array[index1] = array[index2];
    array[index2] = tempValueOne;
}

static inline void swapSpecialStructs(char *array, const size_t byteSizeOfStruct, int64_t index1, int64_t index2) {
    char buffer[byteSizeOfStruct];

    const size_t elementPosOne = ((size_t) index1 * byteSizeOfStruct);
    const size_t elementPosTwo = ((size_t) index2 * byteSizeOfStruct);

    memcpy(buffer, (array + elementPosOne), byteSizeOfStruct);
    memcpy((array + elementPosOne), (array + elementPosTwo), byteSizeOfStruct);
    memcpy((array + elementPosTwo), buffer, byteSizeOfStruct);
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
    default:
        exit(EXIT_FAILURE);
    }
}

static int8_t compareDoubles(void *input1, void *input2) {
    const double d1Value = *((double* ) input1);
    const double d2Value = *((double* ) input2);

    return (d1Value < d2Value) ? -1 : (d1Value == d2Value) ? 0 : 1;
}

static int8_t compareFloats(void *input1, void *input2) {
    const float d1Value = *((float* ) input1);
    const float d2Value = *((float* ) input2);

    return (d1Value < d2Value) ? -1 : (d1Value == d2Value) ? 0 : 1;
}

static int8_t compareInts(void *input1, void *input2) {
    const int d1Value = *((int* ) input1);
    const int d2Value = *((int* ) input2);

    return (d1Value < d2Value) ? -1 : (d1Value == d2Value) ? 0 : 1;
}

static int8_t compareInts8(void *input1, void *input2) {
    const int8_t d1Value = *((int8_t* ) input1);
    const int8_t d2Value = *((int8_t* ) input2);

    return (d1Value < d2Value) ? -1 : (d1Value == d2Value) ? 0 : 1;
}

static int8_t compareInts16(void *input1, void *input2) {
    const int16_t d1Value = *((int16_t* ) input1);
    const int16_t d2Value = *((int16_t* ) input2);

    return (d1Value < d2Value) ? -1 : (d1Value == d2Value) ? 0 : 1;
}

static int8_t compareInts32(void *input1, void *input2) {
    const int32_t d1Value = *((int32_t* ) input1);
    const int32_t d2Value = *((int32_t* ) input2);

    return (d1Value < d2Value) ? -1 : (d1Value == d2Value) ? 0 : 1;
}

static int8_t compareInts64(void *input1, void *input2) {
    const int64_t d1Value = *((int64_t* ) input1);
    const int64_t d2Value = *((int64_t* ) input2);

    return (d1Value < d2Value) ? -1 : (d1Value == d2Value) ? 0 : 1;
}

static int8_t compareUInts8(void *input1, void *input2) {
    const uint8_t d1Value = *((uint8_t* ) input1);
    const uint8_t d2Value = *((uint8_t* ) input2);

    return (d1Value < d2Value) ? -1 : (d1Value == d2Value) ? 0 : 1;
}

static int8_t compareUInts16(void *input1, void *input2) {
    const uint16_t d1Value = *((uint16_t* ) input1);
    const uint16_t d2Value = *((uint16_t* ) input2);

    return (d1Value < d2Value) ? -1 : (d1Value == d2Value) ? 0 : 1;
}

static int8_t compareUInts32(void *input1, void *input2) {
    const uint32_t d1Value = *((uint32_t* ) input1);
    const uint32_t d2Value = *((uint32_t* ) input2);

    return (d1Value < d2Value) ? -1 : (d1Value == d2Value) ? 0 : 1;
}

static int8_t compareUInts64(void *input1, void *input2) {
    const uint64_t d1Value = *((uint64_t* ) input1);
    const uint64_t d2Value = *((uint64_t* ) input2);

    return (d1Value < d2Value) ? -1 : (d1Value == d2Value) ? 0 : 1;
}
