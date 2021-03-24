#include <stdint.h>

uint32_t get_distance_value_from_can_message_bytes(const uint8_t *bytes) {
  uint32_t distance = 0;

  distance = ((uint32_t)((bytes[0] >> 4) & 0x0f)); // Starting from b4
  distance |= ((uint32_t)((bytes[1]) & 0x0f)) << 4;

  return distance;
}