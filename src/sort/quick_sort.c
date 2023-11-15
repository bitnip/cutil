int lomuto_qsort_ptr_arr_partition(
  void *array[],
  int start,
  int end,
  int (*compare)(const void*, const void*)
) {
    void *pivot = array[end];
    int i = start;
    for(unsigned int j = start; j <= end; j++) {
      if(compare(array[j], pivot) < 0) {
        void *temp = array[i];
        array[i] = array[j];
        array[j] = temp;
        i++;
      }
    }
    void *temp = array[i];
    array[i] = array[end];
    array[end] = temp;
    return i;
}

int lomuto_qsort_ptr_arr(
  void *array[],
  unsigned int length,
  int (*compare)(const void*, const void*)
) {
    // https://en.wikipedia.org/wiki/Quicksort#Lomuto_partition_scheme
    if(length <= 0) return 0;
    int p = lomuto_qsort_ptr_arr_partition(array, 0, length - 1, compare);
    lomuto_qsort_ptr_arr(array, p, compare);
    lomuto_qsort_ptr_arr(array + p + 1, length - p - 1, compare);
    return 0;
}