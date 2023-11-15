#ifndef __MERGE_SORT_H
#define __MERGE_SORT_H
#ifdef __cplusplus
extern "C"{
#endif

int top_down_merge_sort(
  void *array[],
  unsigned int length,
  int (*compare)(const void*, const void*)
);

#ifdef __cplusplus
}
#endif
#endif
