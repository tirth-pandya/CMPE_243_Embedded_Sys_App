#include "unity.h"

#include "steer_processor.h"

#include "Mocksteering.h"

void test_steer_processor__move_left(void) {
  steer_left_Expect();
  steer_processor(75, 25);
}

void test_steer_processor__move_right(void) {
  steer_right_Expect();
  steer_processor(25, 75);
}

void test_steer_processor__both_sensors_less_than_threshold(void) {
  steer_left_Expect();
  steer_processor(35, 25);

  steer_right_Expect();
  steer_processor(25, 35);
}

void test_steer_processor__both_sensors_more_than_threshold(void) { steer_processor(75, 65); }

void test_steer_processor(void) {
  steer_right_Expect();
  steer_processor(10, 20);

  steer_left_Expect();
  steer_processor(20, 10);
}