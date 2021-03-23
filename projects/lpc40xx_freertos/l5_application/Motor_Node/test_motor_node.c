#include "unity.h"

#include "Mockboard_io.h"
#include "Mockgpio.h"

#include "motor_node.c"
#include "project.h"

void setup(void){};

void tearDown(void){};

void test_motor_node__moving_forward(void) {
  // gpio_s gpio = {};

  // Assuming steering straight at angle 0
  dbc_DRIVER_TO_MOTOR_s test_val = {5, 0};

  // Speed
  gpio__reset_Expect(led_blue_s);

  // Steer
  gpio__set_Expect(led_red_l);
  gpio__set_Expect(led_green_r);

  motor_node__run_once(test_val);
}

void test_motor_node__moving_forward_zero_speed(void) {
  // gpio_s gpio = {};

  // Assuming steering straight at angle 0
  dbc_DRIVER_TO_MOTOR_s test_val = {0, 0};

  // Speed
  gpio__reset_Expect(led_blue_s);

  // Steer
  gpio__set_Expect(led_red_l);
  gpio__set_Expect(led_green_r);

  motor_node__run_once(test_val);
}

void test_motor_node__moving_reverse(void) {
  // gpio_s gpio = {};

  // Assuming steering straight at angle 0
  dbc_DRIVER_TO_MOTOR_s test_val;
  test_val.MOTOR_speed = -5;
  test_val.MOTOR_direction = 0;

  // Speed
  gpio__set_Expect(led_blue_s);

  // Steer
  gpio__set_Expect(led_red_l);
  gpio__set_Expect(led_green_r);

  motor_node__run_once(test_val);
}

void test_motor_node__steer_right(void) {
  dbc_DRIVER_TO_MOTOR_s test_val;

  // Assuming speed positive val of 5;
  test_val.MOTOR_speed = 5;
  test_val.MOTOR_direction = 30;

  // Speed
  gpio__reset_Expect(led_blue_s);

  // Steer
  gpio__reset_Expect(led_red_l);
  gpio__set_Expect(led_green_r);

  motor_node__run_once(test_val);
}

void test_motor_node__steer_left(void) {
  dbc_DRIVER_TO_MOTOR_s test_val;

  // Assuming speed positive val of 10;
  test_val.MOTOR_speed = 10;
  test_val.MOTOR_direction = -30;

  // Speed
  gpio__reset_Expect(led_blue_s);

  // Steer
  gpio__set_Expect(led_red_l);
  gpio__reset_Expect(led_green_r);

  motor_node__run_once(test_val);
}

void test_motor_node__steer_straigh(void) {
  dbc_DRIVER_TO_MOTOR_s test_val;

  // Assuming speed positive val of 10;
  test_val.MOTOR_speed = 10;
  test_val.MOTOR_direction = 0;

  // Speed
  gpio__reset_Expect(led_blue_s);

  // Steer
  gpio__set_Expect(led_red_l);
  gpio__set_Expect(led_green_r);

  motor_node__run_once(test_val);
}