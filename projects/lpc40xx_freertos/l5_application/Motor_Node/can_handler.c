#include <stdio.h>

#include "board_io.h"
#include "can_bus.h"
#include "can_handler.h"
#include "can_mia.h"
#include "gpio.h"
#include "motor_node.h"
#include "project.h"

dbc_DRIVER_TO_MOTOR_s rx_motor_command;

void can_handler__handle_all_incoming_message(void) {
  can__msg_t rx_msg = {};

  while (can__rx(can1, &rx_msg, 0)) {
    const dbc_message_header_t rx_msg_header = {
        .message_id = rx_msg.msg_id,
        .message_dlc = rx_msg.frame_fields.data_len,
    };

    if (dbc_decode_DRIVER_TO_MOTOR(&rx_motor_command, rx_msg_header, rx_msg.data.bytes)) {
      fprintf(stderr, "Received \n");
      motor_node__run_once(rx_motor_command);
    }
  }
}

void can_handler__manage_mia_10hz(void) {
  const uint32_t mia_increment_value = 100;

  if (can_mia__service_mia_MOTOR(&rx_motor_command, mia_increment_value)) {
    gpio__reset(board_io__get_led3());
  } else {
    gpio__set(board_io__get_led3());
  }
}
