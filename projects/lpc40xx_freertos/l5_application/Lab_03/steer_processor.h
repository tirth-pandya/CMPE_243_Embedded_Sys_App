#pragma once

#include <stdint.h>

#include "steering.h"

// Threshold Value : 50 cm
// Invoke steer when the sensor value is below threshold

void steer_processor(uint32_t left_sensor_cm, uint32_t right_sensor_cm);