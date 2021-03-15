// can_handler.c
#include "acceleration.h"
#include "can_bus.h"
#include "project.h"

void can_handler__transmit_message_10h(void) {
  acceleration__axis_data_s axis_data = acceleration__get_data();
  dbc_SENSOR_ACCMTR_s accmtr_struct = {};

  // Populate accmtr structure
  accmtr_struct.SENSOR_ACCMTR_x = axis_data.x;
  accmtr_struct.SENSOR_ACCMTR_y = axis_data.y;

  // Encode struct to bytes of the CAN message
  can__msg_t can_msg_tx = {};
  const dbc_message_header_t header = dbc_encode_SENSOR_ACCMTR(can_msg_tx.data.bytes, &accmtr_struct);

  can_msg_tx.msg_id = header.message_id;
  can_msg_tx.frame_fields.data_len = header.message_dlc;
  can__tx(can1, &can_msg_tx, 0);
}