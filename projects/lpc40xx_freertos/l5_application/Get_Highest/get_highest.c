#include <stddef.h>
#include <stdint.h>

uint32_t get_highest(const uint32_t *array, size_t array_size) {
  uint32_t result;

  if (array_size <= 0U || array == NULL) {
    result = 0;
  }

  else {
    result = array[0];
    for (int i = 1; i < array_size; i++) {
      if (array[i] > result) {
        result = array[i];
      }
    }
  }

  return result;
}