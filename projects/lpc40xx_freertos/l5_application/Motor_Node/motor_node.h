#pragma once

#include "project.h"

void motor_node__run_once(dbc_DRIVER_TO_MOTOR_s);
bool motor_node__handle_speed(float);
bool motor_node__handle_steering(int8_t);