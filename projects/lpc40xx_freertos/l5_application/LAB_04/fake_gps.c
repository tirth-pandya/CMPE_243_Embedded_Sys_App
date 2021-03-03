#include "fake_gps.h"
#include <stdio.h>

#include "uart.h"
#include "uart_printf.h"
#include "gpio.h"
#include "FreeRTOS.h"
  
#include "clock.h" // needed for UART initialization

// Change this according to which UART you plan to use
static uart_e gps_uart = UART__2;

void fake_gps__init(void) {
  uart__init(gps_uart, clock__get_peripheral_clock_hz(), 38400);
  
  gpio__construct_with_function(GPIO__PORT_0,10,GPIO__FUNCTION_1);  //tx pin
  gpio__construct_with_function(GPIO__PORT_0,11,GPIO__FUNCTION_1);  //rx pin
  QueueHandle_t rxq_handle = xQueueCreate(4,   sizeof(char)); // Nothing to receive
  QueueHandle_t txq_handle = xQueueCreate(100, sizeof(char)); // We send a lot of data
  uart__enable_queues(gps_uart, rxq_handle, txq_handle);
}

void fake_gps__run_once(void) {
   static float longitude = 0;
   uart_printf(gps_uart, "$GPGGA,230612.015,%4.4f,N,12102.4634,W,0,04,5.7,508.3,M,-10.20,M,50,0010*13\r\n", longitude);
   longitude += 1.15; // random incrementing value
}