#pragma once

#include "project.h"
#include <stdint.h>

bool can_mia__service_mia_MOTOR(dbc_DRIVER_TO_MOTOR_s *can_msg_motor, const uint32_t mia_increment_value);