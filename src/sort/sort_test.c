#include "quick_sort.h"
#include "merge_sort.h"
#include "../assertion.h"

int compare_int(const void *a_ptr, const void *b_ptr) {
    int a = *(int*)a_ptr;
    int b = *(int*)b_ptr;
    return a - b;
}

void testEmptyArray(
  int (*sortFn)(void *array[], unsigned int length, int (*compare)(const void*, const void*))
) {
    sortFn(0, 0, compare_int);
}

void testOneElement(
  int (*sortFn)(void* array[], unsigned int length, int (*compare)(const void*, const void*))
) {
    int valueArray[1] = {5};
    void *ptrArray[sizeof(valueArray) / sizeof(valueArray[0])];
    for (int i = 0; i < sizeof(valueArray) / sizeof(valueArray[0]); i++) {
      ptrArray[i] = valueArray + i;
    }
    sortFn(ptrArray, 1, compare_int);
    assertPointersEqual(ptrArray[0], valueArray+0);
}

void testTwoElements(
  int (*sortFn)(void* array[], unsigned int length, int (*compare)(const void*, const void*))
) {
    int valueArray[2] = {5, 4};
    void *ptrArray[sizeof(valueArray) / sizeof(valueArray[0])];
    for (int i = 0; i < sizeof(valueArray) / sizeof(valueArray[0]); i++) {
      ptrArray[i] = valueArray + i;
    }
    sortFn(ptrArray, 2, compare_int);
    assertPointersEqual(ptrArray[0], valueArray+1);
    assertPointersEqual(ptrArray[1], valueArray+0);
}

void testThreeElements(
  int (*sortFn)(void *array[], unsigned int length, int (*compare)(const void*, const void*))
) {
    int valueArray[3] = {5, 4, 6};
    void *ptrArray[sizeof(valueArray) / sizeof(valueArray[0])];
    for (int i = 0; i < sizeof(valueArray) / sizeof(valueArray[0]); i++) {
      ptrArray[i] = valueArray + i;
    }
    sortFn(ptrArray, 3, compare_int);

    assertPointersEqual(ptrArray[0], valueArray+1);
    assertPointersEqual(ptrArray[1], valueArray+0);
    assertPointersEqual(ptrArray[2], valueArray+2);
}

void sortTest() {
    testEmptyArray(lomuto_qsort_ptr_arr);
    testOneElement(lomuto_qsort_ptr_arr);
    testTwoElements(lomuto_qsort_ptr_arr);
    testThreeElements(lomuto_qsort_ptr_arr);

    testEmptyArray(top_down_merge_sort);
    testOneElement(top_down_merge_sort);
    testTwoElements(top_down_merge_sort);
    testThreeElements(top_down_merge_sort);
}
