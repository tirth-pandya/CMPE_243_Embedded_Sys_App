#include "unity.h"

#include "Mockboard_io.h"
#include "Mockcan_bus.h"
#include "Mockcan_mia.h"
#include "Mockgpio.h"
#include "Mockmotor_node.h"
#include "can_handler.c"
#include "project.h"

void setup(void){};
void teardown(void){};

static const dbc_message_header_t wrong_header = {10, 20};

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

  const dbc_message_header_t header = {
      .message_id = msg.msg_id,
      .message_dlc = msg.frame_fields.data_len,
  };
  TEST_ASSERT_EQUAL(true, dbc_decode_DRIVER_TO_MOTOR(&motor_cmd, header, msg.data.bytes));
  motor_node__run_once_ExpectAnyArgs();

  can__rx_ExpectAndReturn(can1, &msg, 0, false);
  can__rx_IgnoreArg_can_message_ptr();

  can_handler__handle_all_incoming_message();
}

void test_can_handler__incoming_msg_failed_id_mismatch(void) {
  can__msg_t msg = {};
  dbc_DRIVER_TO_MOTOR_s motor_cmd;

  msg.msg_id = wrong_header.message_id;
  msg.frame_fields.data_len = dbc_header_DRIVER_TO_MOTOR.message_dlc;

  can__rx_ExpectAndReturn(can1, &msg, 0, true);
  can__rx_IgnoreArg_can_message_ptr();
  can__rx_ReturnThruPtr_can_message_ptr(&msg);

  const dbc_message_header_t header = {
      .message_id = msg.msg_id,
      .message_dlc = msg.frame_fields.data_len,
  };
  TEST_ASSERT_EQUAL(false, dbc_decode_DRIVER_TO_MOTOR(&motor_cmd, header, msg.data.bytes));

  can__rx_ExpectAndReturn(can1, &msg, 0, false);
  can__rx_IgnoreArg_can_message_ptr();

  can_handler__handle_all_incoming_message();
}

void test_can_handler__incoming_msg_failed_dlc_mismatch(void) {
  can__msg_t msg = {};
  dbc_DRIVER_TO_MOTOR_s motor_cmd;

  msg.msg_id = dbc_header_DRIVER_TO_MOTOR.message_id;
  msg.frame_fields.data_len = wrong_header.message_dlc;

  can__rx_ExpectAndReturn(can1, &msg, 0, true);
  can__rx_IgnoreArg_can_message_ptr();
  can__rx_ReturnThruPtr_can_message_ptr(&msg);

  const dbc_message_header_t header = {
      .message_id = msg.msg_id,
      .message_dlc = msg.frame_fields.data_len,
  };
  TEST_ASSERT_EQUAL(false, dbc_decode_DRIVER_TO_MOTOR(&motor_cmd, header, msg.data.bytes));

  can__rx_ExpectAndReturn(can1, &msg, 0, false);
  can__rx_IgnoreArg_can_message_ptr();

  can_handler__handle_all_incoming_message();
}

void test_can_handler__incoming_msg_receive_failed(void) {
  can__msg_t msg = {};
  msg.msg_id = wrong_header.message_id;
  msg.frame_fields.data_len = wrong_header.message_dlc;
  can__rx_ExpectAndReturn(can1, &msg, 0, false);
  can__rx_IgnoreArg_can_message_ptr();
  can__rx_ReturnThruPtr_can_message_ptr(&msg);

  can_handler__handle_all_incoming_message();
}