#include "steer_processor.h"
const int sensor_threshold_value = 50;

void steer_processor(uint32_t left_sensor_cm, uint32_t right_sensor_cm) {
  if ((left_sensor_cm < sensor_threshold_value) || (right_sensor_cm < sensor_threshold_value)) {
    if (left_sensor_cm < right_sensor_cm)
      steer_right();
    else
      steer_left();
  }
}