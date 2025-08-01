#include "quicksort.h"

typedef enum ShiftDirection {
    LEFT,
    RIGHT
} ShiftDirection;

static void quicksortIntern(void*, SortType, const size_t, const int64_t, const int64_t, int (*cmp) (void*, void*));
static int64_t hoarePartition(void*, SortType, const size_t, const int64_t, const int64_t, int (*cmp) (void*, void*));

static bool tryShift(void*, SortType, const size_t, int64_t*, const int64_t, int (*cmp) (void*, void*), ShiftDirection);
static inline bool tryShiftDoubleType(double*, int64_t*, const double, int (*cmp) (void*, void*), ShiftDirection);
static inline bool tryShiftFloatType(float*, int64_t*, const float, int (*cmp) (void*, void*), ShiftDirection);
static inline bool tryShiftIntType(int*, int64_t*, const int, int (*cmp) (void*, void*), ShiftDirection);
static inline bool tryShiftInt8Type(int8_t*, int64_t*, const int8_t, int (*cmp) (void*, void*), ShiftDirection);
static inline bool tryShiftInt16Type(int16_t*, int64_t*, const int16_t, int (*cmp) (void*, void*), ShiftDirection);
static inline bool tryShiftInt32Type(int32_t*, int64_t*, const int32_t, int (*cmp) (void*, void*), ShiftDirection);
static inline bool tryShiftInt64Type(int64_t*, int64_t*, const int64_t, int (*cmp) (void*, void*), ShiftDirection);
static inline bool tryShiftUInt8Type(uint8_t*, int64_t*, const uint8_t, int (*cmp) (void*, void*), ShiftDirection);
static inline bool tryShiftUInt16Type(uint16_t*, int64_t*, const uint16_t, int (*cmp) (void*, void*), ShiftDirection);
static inline bool tryShiftUInt32Type(uint32_t*, int64_t*, const uint32_t, int (*cmp) (void*, void*), ShiftDirection);
static inline bool tryShiftUInt64Type(uint64_t*, int64_t*, const uint64_t, int (*cmp) (void*, void*), ShiftDirection);
static inline bool tryShiftSpecialStruct(char*, int64_t*, const void*, const size_t, int (*cmp) (void*, void*),
ShiftDirection);

static void swap(void*, SortType, const size_t, size_t, size_t);
static inline void swapDoubles(double*, size_t, size_t);
static inline void swapFloats(float*, size_t, size_t);
static inline void swapInts(int*, size_t, size_t);
static inline void swapInts8(int8_t *array, size_t index1, size_t index2);
static inline void swapInts16(int16_t *array, size_t index1, size_t index2);
static inline void swapInts32(int32_t *array, size_t index1, size_t index2);
static inline void swapInts64(int64_t *array, size_t index1, size_t index2);
static inline void swapUInts8(uint8_t *array, size_t index1, size_t index2);
static inline void swapUInts16(uint16_t *array, size_t index1, size_t index2);
static inline void swapUInts32(uint32_t *array, size_t index1, size_t index2);
static inline void swapUInts64(uint64_t *array, size_t index1, size_t index2);
static inline void swapSpecialStructs(char*, const size_t, size_t, size_t);

void quicksort(void *array, SortType type, const size_t byteSizeOfStruct,
    const size_t length, int (*cmp) (void*, void*)) {
    if((array == NULL) || (length == 0) || (cmp == NULL)) return;
    if((type == SPECIAL_STRUCT) && (byteSizeOfStruct == 0)) return;

    quicksortIntern(array, type, byteSizeOfStruct, 0, length - 1, cmp);
}

static void quicksortIntern(void *array, SortType type, const size_t byteSizeOfStruct,
    const int64_t startingIndex, const int64_t endingIndex, int (*cmp) (void*, void*)) {
    if(startingIndex < endingIndex) {
        const int64_t partitionIndex = hoarePartition(array, type, byteSizeOfStruct, startingIndex, endingIndex, cmp);

        quicksortIntern(array, type, byteSizeOfStruct, startingIndex, partitionIndex, cmp);
        quicksortIntern(array, type, byteSizeOfStruct, partitionIndex + 1, endingIndex, cmp);
    }
}

static int64_t hoarePartition(void *array, SortType type, const size_t byteSizeOfStruct,
    const int64_t startingIndex, const int64_t endingIndex, int (*cmp) (void*, void*)) {
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

static bool tryShift(void *array, SortType type, const size_t byteSizeOfStruct,
    int64_t *ptrIndex, const int64_t pivotIndex, int (*cmp) (void*, void*),
    ShiftDirection shiftDirection) {

    if(shiftDirection == LEFT) {
        *ptrIndex += 1;
    } else {
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
    int (*cmp) (void*, void*), ShiftDirection shiftDirection) {
    const double ptrCurrentValue = array[*ptrIndex];

    const bool result = (shiftDirection == LEFT) ? cmp((void*) &pivotValue, (void*) &ptrCurrentValue) > 0 :
        cmp((void*) &pivotValue, (void*) &ptrCurrentValue) < 0;

    return result;
}

static inline bool tryShiftFloatType(float *array, int64_t *ptrIndex, const float pivotValue,
    int (*cmp) (void*, void*), ShiftDirection shiftDirection) {
    const float ptrCurrentValue = array[*ptrIndex];

    const bool result = (shiftDirection == LEFT) ? cmp((void*) &pivotValue, (void*) &ptrCurrentValue) > 0 :
        cmp((void*) &pivotValue, (void*) &ptrCurrentValue) < 0;

    return result;
}

static inline bool tryShiftIntType(int *array, int64_t *ptrIndex, const int pivotValue,
    int (*cmp) (void*, void*), ShiftDirection shiftDirection) {
    const int ptrCurrentValue = array[*ptrIndex];

    const bool result = (shiftDirection == LEFT) ? cmp((void*) &pivotValue, (void*) &ptrCurrentValue) > 0 :
        cmp((void*) &pivotValue, (void*) &ptrCurrentValue) < 0;

    return result;
}

static inline bool tryShiftInt8Type(int8_t *array, int64_t *ptrIndex, const int8_t pivotValue,
    int (*cmp) (void*, void*), ShiftDirection shiftDirection) {
    const int8_t ptrCurrentValue = array[*ptrIndex];

    const bool result = (shiftDirection == LEFT) ? cmp((void*) &pivotValue, (void*) &ptrCurrentValue) > 0 :
        cmp((void*) &pivotValue, (void*) &ptrCurrentValue) < 0;

    return result;
}

static inline bool tryShiftInt16Type(int16_t *array, int64_t *ptrIndex, const int16_t pivotValue,
    int (*cmp) (void*, void*), ShiftDirection shiftDirection) {
    const int16_t ptrCurrentValue = array[*ptrIndex];

    const bool result = (shiftDirection == LEFT) ? cmp((void*) &pivotValue, (void*) &ptrCurrentValue) > 0 :
        cmp((void*) &pivotValue, (void*) &ptrCurrentValue) < 0;

    return result;
}

static inline bool tryShiftInt32Type(int32_t *array, int64_t *ptrIndex, const int32_t pivotValue,
    int (*cmp) (void*, void*), ShiftDirection shiftDirection) {
    const int32_t ptrCurrentValue = array[*ptrIndex];

    const bool result = (shiftDirection == LEFT) ? cmp((void*) &pivotValue, (void*) &ptrCurrentValue) > 0 :
        cmp((void*) &pivotValue, (void*) &ptrCurrentValue) < 0;

    return result;
}

static inline bool tryShiftInt64Type(int64_t *array, int64_t *ptrIndex, const int64_t pivotValue,
    int (*cmp) (void*, void*), ShiftDirection shiftDirection) {
    const int64_t ptrCurrentValue = array[*ptrIndex];

    const bool result = (shiftDirection == LEFT) ? cmp((void*) &pivotValue, (void*) &ptrCurrentValue) > 0 :
        cmp((void*) &pivotValue, (void*) &ptrCurrentValue) < 0;

    return result;
}

static inline bool tryShiftUInt8Type(uint8_t *array, int64_t *ptrIndex, const uint8_t pivotValue,
    int (*cmp) (void*, void*), ShiftDirection shiftDirection) {
    const uint8_t ptrCurrentValue = array[*ptrIndex];

    const bool result = (shiftDirection == LEFT) ? cmp((void*) &pivotValue, (void*) &ptrCurrentValue) > 0 :
        cmp((void*) &pivotValue, (void*) &ptrCurrentValue) < 0;

    return result;
}

static inline bool tryShiftUInt16Type(uint16_t *array, int64_t *ptrIndex, const uint16_t pivotValue,
    int (*cmp) (void*, void*), ShiftDirection shiftDirection) {
    const uint16_t ptrCurrentValue = array[*ptrIndex];

    const bool result = (shiftDirection == LEFT) ? cmp((void*) &pivotValue, (void*) &ptrCurrentValue) > 0 :
        cmp((void*) &pivotValue, (void*) &ptrCurrentValue) < 0;

    return result;
}

static inline bool tryShiftUInt32Type(uint32_t *array, int64_t *ptrIndex, const uint32_t pivotValue,
    int (*cmp) (void*, void*), ShiftDirection shiftDirection) {
    const uint32_t ptrCurrentValue = array[*ptrIndex];

    const bool result = (shiftDirection == LEFT) ? cmp((void*) &pivotValue, (void*) &ptrCurrentValue) > 0 :
        cmp((void*) &pivotValue, (void*) &ptrCurrentValue) < 0;

    return result;
}

static inline bool tryShiftUInt64Type(uint64_t *array, int64_t *ptrIndex, const uint64_t pivotValue,
    int (*cmp) (void*, void*), ShiftDirection shiftDirection) {
    const uint64_t ptrCurrentValue = array[*ptrIndex];

    const bool result = (shiftDirection == LEFT) ? cmp((void*) &pivotValue, (void*) &ptrCurrentValue) > 0 :
        cmp((void*) &pivotValue, (void*) &ptrCurrentValue) < 0;

    return result;
}

static inline bool tryShiftSpecialStruct(char *array, int64_t *ptrIndex, const void *pivotElement,
    const size_t byteSizeOfStruct, int (*cmp) (void*, void*), ShiftDirection shiftDirection) {
    const size_t elementPos = (((size_t) *ptrIndex) * byteSizeOfStruct);
    const void *ptrCurrentElement = array + elementPos;

    const bool result = (shiftDirection == LEFT) ? cmp((void*) pivotElement, (void*) ptrCurrentElement) > 0 :
        cmp((void*) pivotElement, (void*) ptrCurrentElement) < 0;

    return result;
}

static void swap(void *array, SortType type, const size_t byteSizeOfStruct, size_t index1, size_t index2) {
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

static inline void swapDoubles(double *array, size_t index1, size_t index2) {
    const double tempValueOne = array[index1];
    array[index1] = array[index2];
    array[index2] = tempValueOne;
}

static inline void swapFloats(float *array, size_t index1, size_t index2) {
    const float tempValueOne = array[index1];
    array[index1] = array[index2];
    array[index2] = tempValueOne;
}

static inline void swapInts(int *array, size_t index1, size_t index2) {
    const int tempValueOne = array[index1];
    array[index1] = array[index2];
    array[index2] = tempValueOne;
}

static inline void swapInts8(int8_t *array, size_t index1, size_t index2) {
    const int8_t tempValueOne = array[index1];
    array[index1] = array[index2];
    array[index2] = tempValueOne;
}

static inline void swapInts16(int16_t *array, size_t index1, size_t index2) {
    const int16_t tempValueOne = array[index1];
    array[index1] = array[index2];
    array[index2] = tempValueOne;
}

static inline void swapInts32(int32_t *array, size_t index1, size_t index2) {
    const int32_t tempValueOne = array[index1];
    array[index1] = array[index2];
    array[index2] = tempValueOne;
}

static inline void swapInts64(int64_t *array, size_t index1, size_t index2) {
    const int64_t tempValueOne = array[index1];
    array[index1] = array[index2];
    array[index2] = tempValueOne;
}

static inline void swapUInts8(uint8_t *array, size_t index1, size_t index2) {
    const uint8_t tempValueOne = array[index1];
    array[index1] = array[index2];
    array[index2] = tempValueOne;
}

static inline void swapUInts16(uint16_t *array, size_t index1, size_t index2) {
    const uint16_t tempValueOne = array[index1];
    array[index1] = array[index2];
    array[index2] = tempValueOne;
}

static inline void swapUInts32(uint32_t *array, size_t index1, size_t index2) {
    const uint32_t tempValueOne = array[index1];
    array[index1] = array[index2];
    array[index2] = tempValueOne;
}

static inline void swapUInts64(uint64_t *array, size_t index1, size_t index2) {
    const uint64_t tempValueOne = array[index1];
    array[index1] = array[index2];
    array[index2] = tempValueOne;
}

static inline void swapSpecialStructs(char *array, const size_t byteSizeOfStruct, size_t index1, size_t index2) {
    char buffer[byteSizeOfStruct];

    const size_t elementPosOne = ((size_t) index1 * byteSizeOfStruct);
    const size_t elementPosTwo = ((size_t) index2 * byteSizeOfStruct);

    memcpy(buffer, (array + elementPosOne), byteSizeOfStruct);
    memcpy((array + elementPosOne), (array + elementPosTwo), byteSizeOfStruct);
    memcpy((array + elementPosTwo), buffer, byteSizeOfStruct);
}
