#include "unity.h"

// Mocks
#include "Mockclock.h"
#include "Mockuart.h"

#include "Mockgpio.h"
#include "Mockqueue.h"

// Use the real implementation (not mocks) for:
#include "line_buffer.h"

// Include the source we wish to test
#include "gps.c"

void setUp(void) {}

void tearDown(void) {}

void test_init(void) {

  uint32_t ret_clock;
  clock__get_peripheral_clock_hz_ExpectAndReturn(ret_clock);
  uart__init_Expect(gps_uart, ret_clock, 38400);

  gpio__construct_with_function_ExpectAndReturn(GPIO__PORT_0, 10, GPIO__FUNCTION_1, gps_tx);
  gpio__construct_with_function_ExpectAndReturn(GPIO__PORT_0, 11, GPIO__FUNCTION_1, gps_rx);

  QueueHandle_t tx_qh, rx_qh;
  xQueueCreate_ExpectAndReturn(50, sizeof(char), rx_qh);
  xQueueCreate_ExpectAndReturn(8, sizeof(char), tx_qh);
  uart__enable_queues_ExpectAndReturn(gps_uart, rx_qh, tx_qh, true);
  gps__init();
}

void test_GPGGA_coordinates_are_parsed(void) {
  char *uart_driver_returned_data = "$GPGLL,hhmmss.ss,12345.67,a,76543.21,a,x,xx,x.x,x.x,M,x.x,M,x.x,xxxx*hh\r\n";
  char *ptr;
  for (size_t index = 0; index < strlen(uart_driver_returned_data); index++) {
    char the_char_to_return = uart_driver_returned_data[index];
    bool last_char = true;
    uart__get_ExpectAndReturn(gps_uart, ptr, 0, last_char);
    uart__get_IgnoreArg_input_byte();
    uart__get_ReturnThruPtr_input_byte(&the_char_to_return);
    last_char = false;
    uart__get_ExpectAndReturn(gps_uart, ptr, 0, last_char);
    uart__get_IgnoreArg_input_byte();
    gps__run_once();
  }
  gps_coordinates_t parsed_coordinates_for_test = gps__get_coordinates();
  TEST_ASSERT_EQUAL_FLOAT((float)12345.67, parsed_coordinates_for_test.latitude);
  TEST_ASSERT_EQUAL_FLOAT((float)76543.21, parsed_coordinates_for_test.longitude);
}

void test_GPGLL_line_is_ignored(void) {
  char *uart_driver_returned_data = "$GPGLL,1234.56,N,6543.21,S,*10\r\n";
  char *ptr;
  for (size_t index = 0; index < strlen(uart_driver_returned_data); index++) {
    char the_char_to_return = uart_driver_returned_data[index];
    bool last_char = true;
    uart__get_ExpectAndReturn(gps_uart, ptr, 0, last_char);
    uart__get_IgnoreArg_input_byte();
    uart__get_ReturnThruPtr_input_byte(&the_char_to_return);
    last_char = false;
    uart__get_ExpectAndReturn(gps_uart, ptr, 0, last_char);
    uart__get_IgnoreArg_input_byte();
    gps__run_once();
  }
  gps_coordinates_t gps_test_values = gps__get_coordinates();
  TEST_ASSERT_EQUAL_FLOAT(0.0, gps_test_values.latitude);
  TEST_ASSERT_EQUAL_FLOAT(0.0, gps_test_values.longitude);
}
