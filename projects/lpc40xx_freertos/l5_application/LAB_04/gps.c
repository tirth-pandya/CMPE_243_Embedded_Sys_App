#include <stdio.h>
// gps.c
#include "gps.h"
#include <string.h>
// GPS module dependency
#include "line_buffer.h"
#include "uart.h"

// Rtos dependency for the UART driver
#include "FreeRTOS.h"
#include "gpio.h"
#include "queue.h"

#include "clock.h" // needed for UART initialization

// Change this according to which UART you plan to use
static const uart_e gps_uart = UART__2;

// Space for the line buffer, and the line buffer data structure instance
static char line_buffer[200];
static line_buffer_s line;

static gps_coordinates_t parsed_coordinates;

static const gpio_s gps_tx = {GPIO__PORT_0, 10};
static const gpio_s gps_rx = {GPIO__PORT_0, 11};

void gps__init(void) {
  line_buffer__init(&line, line_buffer, sizeof(line_buffer));
  uart__init(gps_uart, clock__get_peripheral_clock_hz(), 38400);
  gpio__construct_with_function(gps_tx.port_number, gps_tx.pin_number, GPIO__FUNCTION_1);
  gpio__construct_with_function(gps_rx.port_number, gps_rx.pin_number, GPIO__FUNCTION_1);
  // RX queue should be sized such that can buffer data in UART driver until gps__run_once() is called
  // Note: Assuming 38400bps, we can get 4 chars per ms, and 40 chars per 10ms (100Hz)
  QueueHandle_t rxq_handle = xQueueCreate(50, sizeof(char));
  QueueHandle_t txq_handle = xQueueCreate(8, sizeof(char)); // We don't send anything to the GPS
  uart__enable_queues(gps_uart, rxq_handle, txq_handle);
}

/*
//Helper Functions for string operations; C++ function - strtof//
*/
static bool isDigit(char c) {
  if (c >= '0' && c <= '9')
    return true;
  else
    return false;
}
static float strtof(const char *str) {
  float res = 0.0F;
  char *ptr = (char *)str;
  bool afterDecimalPoint = false;
  float div = 1; // Divider to place digits after the deciaml point

  while (*ptr != '\0') {
    if (isDigit(*ptr)) {
      // Integer part
      if (!afterDecimalPoint) {
        res *= 10;         // Shift the previous digits to the left
        res += *ptr - '0'; // Add the new one
      }
      // Decimal part
      else {
        div *= 10;
        res += (float)(*ptr - '0') / div;
      }
    } else if (*ptr == '.') {
      afterDecimalPoint = true;
    } else {
      break;
    }

    ptr++;
  }

  return res;
}
////////////////////////////
//**************************
////////////////////////////

static void gps__parse_helper_GPGGA(char *gps_line) {
  static char *delimiter = ",";
  static const char *GPGGA_identifier = "$GPGGA";
  static const char *GPGLL_identifier = "$GPGLL";

  char *parsed_gps_parameter = strtok(gps_line, delimiter);
  if (*GPGGA_identifier == *parsed_gps_parameter) {
    parsed_gps_parameter = strtok(NULL, delimiter);              // Time : Ignored
    parsed_gps_parameter = strtok(NULL, delimiter);              // Latitude
    parsed_coordinates.latitude = strtof(parsed_gps_parameter);  // Converted string to float
    parsed_gps_parameter = strtok(NULL, delimiter);              // N/S : Ignored
    parsed_gps_parameter = strtok(NULL, delimiter);              // Longitude
    parsed_coordinates.longitude = strtof(parsed_gps_parameter); // Converted string to float
  }

  if (*GPGLL_identifier == *parsed_gps_parameter) {
    parsed_coordinates.latitude = (float)0.0;
    parsed_coordinates.longitude = (float)0.0;
  }
}

static void gps__transfer_data_from_uart_driver_to_line_buffer(void) {
  char byte;
  const uint32_t zero_timeout = 0;

  while (uart__get(gps_uart, &byte, zero_timeout)) {
    line_buffer__add_byte(&line, byte);
  }
}

static void gps__parse_coordinates_from_line(void) {
  char gps_line[120];
  if (line_buffer__remove_line(&line, gps_line, sizeof(gps_line))) {
    // TODO: Parse the line to store GPS coordinates etc.
    // TODO: parse and store to parsed_coordinates
    gps__parse_helper_GPGGA(gps_line);
    // printf("Latitude : %f ; \t Longitude : %f\n", parsed_coordinates.latitude, parsed_coordinates.longitude);
  }
}

/// Public functions:
///
void gps__run_once(void) {
  gps__transfer_data_from_uart_driver_to_line_buffer();
  gps__parse_coordinates_from_line();
}

gps_coordinates_t gps__get_coordinates(void) {
  // TODO return parsed_coordinates
  return parsed_coordinates;
}