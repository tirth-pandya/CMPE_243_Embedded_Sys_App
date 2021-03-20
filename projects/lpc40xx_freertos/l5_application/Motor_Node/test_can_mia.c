#include "unity.h"

#include "can_mia.c"
#include "project.h"

void setup(){};

void teardown(){};

void test_can_mia__service_mia_MOTOR_at_mia_threshold(void) {
  dbc_DRIVER_TO_MOTOR_s can_msg_motor = {};
  can_msg_motor.mia_info.mia_counter = dbc_mia_threshold_DRIVER_TO_MOTOR - 100;
  TEST_ASSERT_EQUAL(true, can_mia__service_mia_MOTOR(&can_msg_motor, 100));
}

void test_can_mia__service_mia_MOTOR_below_mia_threshold(void) {
  dbc_DRIVER_TO_MOTOR_s can_msg_motor = {};
  can_msg_motor.mia_info.mia_counter = dbc_mia_threshold_DRIVER_TO_MOTOR - 100 - 1;
  TEST_ASSERT_EQUAL(false, can_mia__service_mia_MOTOR(&can_msg_motor, 100));
}

void test_can_mia__service_mia_MOTOR_above_mia_threshold(void) {
  dbc_DRIVER_TO_MOTOR_s can_msg_motor = {};
  can_msg_motor.mia_info.mia_counter = dbc_mia_threshold_DRIVER_TO_MOTOR - 100 + 1;
  TEST_ASSERT_EQUAL(true, can_mia__service_mia_MOTOR(&can_msg_motor, 100));
}