#ifndef __QUICK_SORT_H
#define __QUICK_SORT_H
#ifdef __cplusplus
extern "C"{
#endif

int lomuto_qsort_ptr_arr(
  void *array[],
  unsigned int length,
  int (*compare)(const void *, const void *)
);

#ifdef __cplusplus
}
#endif
#endif
