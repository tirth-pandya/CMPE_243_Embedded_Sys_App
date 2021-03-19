#include "unity.h"

#include "Mockboard_io.h"
#include "Mockcan_bus.h"
#include "Mockcan_mia.h"
#include "Mockgpio.h"
#include "Mockmotor_node.h"
#include "can_handler.h"
#include "project.h"

void setup(void){};
void teardown(void){};

void test_can_handler__manage_mia_10hz_occured_true(void) {
  dbc_DRIVER_TO_MOTOR_s rx_motor_command;
  gpio_s gpio = {};
  board_io__get_led3_ExpectAndReturn(gpio);
  can_mia__service_mia_MOTOR_ExpectAndReturn(&rx_motor_command, 100, true);
  can_mia__service_mia_MOTOR_IgnoreArg_can_msg_motor();
  gpio__reset_Expect(gpio);
  can_handler__manage_mia_10hz();
}

void test_can_handler__manage_mia_10hz_occured_flase(void) {
  dbc_DRIVER_TO_MOTOR_s rx_motor_command;
  gpio_s gpio = {};
  board_io__get_led3_ExpectAndReturn(gpio);
  can_mia__service_mia_MOTOR_ExpectAndReturn(&rx_motor_command, 100, false);
  can_mia__service_mia_MOTOR_IgnoreArg_can_msg_motor();
  gpio__set_Expect(gpio);
  can_handler__manage_mia_10hz();
}

void test_can_handler__incoming_msg_success_motor_run_once(void) {
  can__msg_t msg = {};
  dbc_DRIVER_TO_MOTOR_s motor_cmd;

  msg.msg_id = dbc_header_DRIVER_TO_MOTOR.message_id;
  msg.frame_fields.data_len = dbc_header_DRIVER_TO_MOTOR.message_dlc;

  can__rx_ExpectAndReturn(can1, &msg, 0, true);
  can__rx_IgnoreArg_can_message_ptr();
  can__rx_ReturnThruPtr_can_message_ptr(&msg);

  motor_cmd.MOTOR_speed = 5.5;
  motor_cmd.MOTOR_direction = 30;

  const dbc_message_header_t header = {
      .message_id = msg.msg_id,
      .message_dlc = msg.frame_fields.data_len,
  };

  motor_node__run_once_ExpectAnyArgs();

  TEST_ASSERT_EQUAL(true, dbc_decode_DRIVER_TO_MOTOR(&motor_cmd, header, msg.data.bytes));

  can_handler__handle_all_incoming_message();
}
