#include <stdio.h>
#include <string.h>

#include "unity.h"

// Include the Mocks
// - This will not pull the REAL source code of these modules (such as board_io.c)
// - This will auto-generate "Mock" versions based on the header file
#include "Mockboard_io.h"
#include "Mockcan_bus.h"
#include "Mockcan_handler.h"
#include "Mockgpio.h"
#include "Mockmotor_node.h"
// Include the source we wish to test
#include "periodic_callbacks.h"

void setUp(void) {}

void tearDown(void) {}

void test__periodic_callbacks__initialize(void) {
  can__num_e can_bus_num = can1;
  can__init_ExpectAndReturn(can_bus_num, 100, 50, 50, NULL, NULL, &can_bus_num);
  //  can__init_IgnoreArg_can();
  can__bypass_filter_accept_all_msgs_Expect();
  can__reset_bus_Expect(can_bus_num);
  motor_node__init_led_Expect();
  periodic_callbacks__initialize();
}

void test__periodic_callbacks__1Hz(void) {
  can_handler__handle_all_incoming_message_Expect();
  periodic_callbacks__1Hz(0);
}