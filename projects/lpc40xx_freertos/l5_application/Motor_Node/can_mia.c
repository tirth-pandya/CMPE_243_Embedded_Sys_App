#include "can_mia.h"

const uint32_t dbc_mia_threshold_DRIVER_TO_MOTOR = 300;

const dbc_DRIVER_TO_MOTOR_s dbc_mia_replacement_DRIVER_TO_MOTOR;

bool can_mia__service_mia_MOTOR(dbc_DRIVER_TO_MOTOR_s *can_msg_motor, uint32_t can_mia_increment_value) {
  bool mia_service_occured = false;

  can_msg_motor->mia_info.mia_counter += can_mia_increment_value;
  if (can_msg_motor->mia_info.mia_counter >= dbc_mia_threshold_DRIVER_TO_MOTOR) {
    can_msg_motor->MOTOR_direction = dbc_mia_replacement_DRIVER_TO_MOTOR.MOTOR_direction;
    can_msg_motor->MOTOR_speed = dbc_mia_replacement_DRIVER_TO_MOTOR.MOTOR_speed;
    mia_service_occured = true;
  }
  return mia_service_occured;
}
