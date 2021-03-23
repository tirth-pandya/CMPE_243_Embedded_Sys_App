#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef enum {
  C_UART_0 = 0,
  C_UART_1,
  C_UART_2,
  C_UART_3,
} c_uart_number_e;

void c_uart__init(c_uart_number_e uart_number, uint32_t peripheral_clock, uint32_t baud_rate, uint32_t rx_queue,
                  uint32_t tx_queue);

bool c_uart__polled_get(c_uart_number_e uart_number, char *input_byte);

bool c_uart__polled_put(c_uart_number_e uart_number, char output_byte);

void c_uart__enable_rx_tx_interrupt(c_uart_number_e uart_number);

bool c_uart__get_char_from_queue(c_uart_number_e uart_number, char *byte, uint32_t timeout);

bool c_uart__put_char_to_queue(c_uart_number_e uart_number, char output_byte, uint32_t timeout);

bool c_uart__puts(c_uart_number_e uart_number, const char *output_string, uint32_t timeout);

bool c_uart__polled_puts(c_uart_number_e uart_number, const char *output_string);