#include <stdlib.h>
#include "../error.h"

void top_down_merge_sort_split_merge(
  void *a[],
  void *b[],
  int start,
  int end,
  int (*compare)(const void*, const void*)
) {
  // Note: Start is inclusive, end is exclusive.
  // Base Case: The list contains 1 card and is thus already sorted.
  if(end - start < 2) return;
  // Find the middle of the run.
  int middle = (end + start) / 2;
  // Run recursively for the left and right run.
  top_down_merge_sort_split_merge(a, b, start, middle, compare);
  top_down_merge_sort_split_merge(a, b, middle, end, compare);
  // Merge the runs into the original array.
  int i = start;
  int j = middle;
  for(int k = start; k < end; k++) {
    // If we haven't run out of elements in the first run AND
    //   Either the second run is out of elements OR
    //   The element in the left run is <= the element in the right run.
    if(i < middle && (j >= end || (compare(b[i], b[j]) ) <= 0)) {
      a[k] = b[i];
      i++;
    } else {
      a[k] = b[j];
      j++;
    }
  }
}

int top_down_merge_sort(
  void *array[],
  unsigned int length,
  int (*compare)(const void*, const void*)
) {
  // https://en.wikipedia.org/wiki/Merge_sort#Top-down_implementation
  // Copy array into a work array.
  void **workArray = malloc(sizeof(void*) * length);
  if (workArray == NULL) return STATUS_ALLOC_ERR;
  for (int i = 0; i < length; i++) {
    workArray[i] = array[i];
  }

  top_down_merge_sort_split_merge(array, workArray, 0, length, compare);
  return STATUS_OK;
}