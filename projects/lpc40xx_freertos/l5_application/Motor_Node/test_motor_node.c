#include "unity.h"

#include "Mockboard_io.h"
#include "Mockgpio.h"

#include "motor_node.c"
#include "project.h"

void setup(void){};

void tearDown(void){};

// void test_motor_node__moving_forward(void) {
//   float speed = 1;
//   gpio_s gpio = {};

//   board_io__get_led2_ExpectAndReturn(gpio);
//   gpio__reset_Expect(gpio);

//   // motor_node__run_once(motr_command);
//   //   motor_node__handle_speed(speed);
// }

void test_motor_node__moving_reverse(void) {
  dbc_DRIVER_TO_MOTOR_s test = {};
  float speed = -1;
  test.MOTOR_speed = speed;

  gpio_s gpio = {};
  board_io__get_led2_ExpectAndReturn(gpio);
  gpio__reset_Expect(gpio);

  motor_node__run_once(test);
}

void test_motor_node__move_forward_speed_zero(void) {
  float speed = 0;
  gpio_s gpio = {};

  board_io__get_led2_ExpectAndReturn(gpio);
  gpio__set_Expect(gpio);

  motor_node__handle_speed(speed);
}

void test_motor_node__move_forward_speed_non_zero(void) {
  float speed = 1;
  gpio_s gpio = {};

  board_io__get_led2_ExpectAndReturn(gpio);
  gpio__set_Expect(gpio);

  motor_node__handle_speed(speed);
}

void test_handle_steering__right(void) {
  bool retVal;
  int8_t angle = 20;
  gpio_s led_left = {}, led_right = {};
}