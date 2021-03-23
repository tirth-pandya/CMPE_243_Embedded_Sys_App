#ifndef __LAB_UART_H__
#define __LAB_UART_H__

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define PCUART0 3
#define PCUART1 4
#define PCUART2 24
#define PCUART3 25
#define DLAB_BIT 7

#define THRE 5
#define RDR 0

typedef LPC_UART_TypeDef uart_num;

typedef enum {
  uart__0 = 0,
  uart__1 = 1,
  uart__2 = 2,
  uart__3 = 3,
} uart_number_e;

typedef struct {
  uart_num *registers;
  uint8_t power_pin;
} uart_selection;

void uart_lab__init(uart_number_e uart, uint32_t peripheral_clock, uint32_t baud_rate);
bool uart_lab__polled_put(uart_number_e uart, char output_byte);
bool uart_lab__polled_get(uart_number_e uart, char *input_byte);
bool uart_lab__get_char_from_queue(char *input_byte, uint32_t timeout);
void uart__enable_receive_interrupt(uart_number_e uart_number);

#endif //__LAB_UART_H__