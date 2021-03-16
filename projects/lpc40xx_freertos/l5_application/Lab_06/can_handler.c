#include "acceleration.h"
#include "board_io.h"
#include "can_bus.h"
#include "can_mia_configuration.h"
#include "gpio.h"
#include "project.h"
#include <stdio.h>

dbc_SENSOR_ACCMTR_s accmtr_recv_values;

void handle_led_with_accmtr(int16_t accmtr_val) {
  gpio__set(board_io__get_led0());
  gpio__set(board_io__get_led1());
  gpio__set(board_io__get_led2());
  gpio__set(board_io__get_led3());
  if (accmtr_val <= (-512) && accmtr_val > (-1024))
    gpio__reset(board_io__get_led0());
  else if (accmtr_val <= 0 && accmtr_val > (-512))
    gpio__reset(board_io__get_led1());
  else if (accmtr_val <= 512 && accmtr_val > 0)
    gpio__reset(board_io__get_led2());
  else if (accmtr_val <= 1024 && accmtr_val > 512)
    gpio__reset(board_io__get_led3());
  else
    fprintf(stderr, "Unable to toggle leds");
}

void handle_led_with_button(bool button) {
  if (button) {
    gpio_s ext_led = {GPIO__PORT_0, 15};
    gpio__toggle(ext_led);
  }
}

void can_handler__manage_mia_10hz(void) {
  // We are in 10hz slot, so increment MIA counter by 100ms
  const uint32_t mia_increment_value = 100;

  if (can_mia__service_mia_SENSOR_ACCMTR(&accmtr_recv_values, mia_increment_value)) {
    // Take action when a message has gone MIA?
    // Maybe light up an LED?
    // fprintf(stderr, "MIA occured\n");
  }
}

void can_handler__handle_all_incoming_messages(void) {
  can__msg_t can_msg = {};
  // dbc_SENSOR_ACCMTR_s decoded_sensor_accmtr = {};

  while (can__rx(can1, &can_msg, 5)) {
    // Construct "message header" that we need for the decode_*() API
    const dbc_message_header_t header = {
        .message_id = can_msg.msg_id,
        .message_dlc = can_msg.frame_fields.data_len,
    };

    if (dbc_decode_SENSOR_ACCMTR(&accmtr_recv_values, header, can_msg.data.bytes)) {
      handle_led_with_accmtr(accmtr_recv_values.SENSOR_ACCMTR_x);
    } else {
      fprintf(stderr, "failed to decode\n");
    }
    // dbc_decode_SENSOR_BUTTON(&decoded_sensor_accmtr, header, can_msg.data.bytes);
  }
}

void can_handler__transmit_message_10hz_accmtr(void) {
  acceleration__axis_data_s axis_data = acceleration__get_data();
  dbc_SENSOR_ACCMTR_s accmtr_struct = {};

  // Populate accmtr structure
  accmtr_struct.SENSOR_ACCMTR_x = axis_data.x;
  accmtr_struct.SENSOR_ACCMTR_y = axis_data.y;

  // Encode struct to bytes of the CAN message
  can__msg_t can_msg_tx = {};
  const dbc_message_header_t accmtr_header = dbc_encode_SENSOR_ACCMTR(can_msg_tx.data.bytes, &accmtr_struct);

  can_msg_tx.msg_id = accmtr_header.message_id;
  can_msg_tx.frame_fields.data_len = accmtr_header.message_dlc;
  if (!can__tx(can1, &can_msg_tx, 0)) {
    fprintf(stderr, "Not able sent\n");
  } else {
    fprintf(stderr, "failed to send\n");
  }
}

void can_handler__transmit_message_10hz_button(void) {
  dbc_SENSOR_BUTTON_s button_val;
  button_val.SENSOR_BUTTON_bool = gpio__get(board_io__get_sw0());
  // Encode struct to bytes of the CAN message
  can__msg_t can_msg_tx = {};
  const dbc_message_header_t button_header = dbc_encode_SENSOR_BUTTON(can_msg_tx.data.bytes, &button_val);
  can_msg_tx.msg_id = button_header.message_id;
  can_msg_tx.frame_fields.data_len = button_header.message_dlc;
  if (!can__tx(can1, &can_msg_tx, 0)) {
    fprintf(stderr, "Not able sent\n");
  } else {
    fprintf(stderr, "failed to send\n");
  }
}