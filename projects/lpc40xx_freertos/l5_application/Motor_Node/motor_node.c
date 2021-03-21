#include "motor_node.h"
#include "board_io.h"
#include "gpio.h"
#include <stdio.h>

static void steer_left(void) {
  gpio__reset(board_io__get_led1());
  gpio__set(board_io__get_led0());
}

static void steer_right(void) {
  gpio__set(board_io__get_led1());
  gpio__reset(board_io__get_led0());
}

static void steer_straight(void) {
  gpio__set(board_io__get_led1());
  gpio__set(board_io__get_led0());
}
static bool motor_node__handle_speed(float motor_speed) {
  bool status;

  if (motor_speed < 0) {
    gpio__reset(board_io__get_led2());
    fprintf(stderr, "motor speed %f\n", motor_speed);
    status = true;
  } else if (motor_speed >= 0) {
    gpio__set(board_io__get_led2());
    status = true;
  } else {
    status = false;
  }
  return status;
}

static bool motor_node__handle_steering(int8_t steer_angle) {
  bool status;

  if (steer_angle == 0) {
    steer_straight();
    status = true;
  }

  else if (steer_angle < 0) {
    steer_left();
    status = true;
  }

  else if (steer_angle > 0) {
    steer_right();
    status = true;
  }

  else {
    status = false;
  }

  return status;
}

void motor_node__run_once(dbc_DRIVER_TO_MOTOR_s motor_command) {
  motor_node__handle_speed(motor_command.MOTOR_speed);
  motor_node__handle_steering(motor_command.MOTOR_direction);
}