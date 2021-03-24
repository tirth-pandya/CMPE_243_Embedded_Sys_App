#include <stdint.h>

uint32_t count_bits(uint32_t value) {
  uint32_t set_bits_counter = 0;

  while (value) {
    set_bits_counter += (value)&1;
    value = (value) >> 1;
  }

  return set_bits_counter;
}