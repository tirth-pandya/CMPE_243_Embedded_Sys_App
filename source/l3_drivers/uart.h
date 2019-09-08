#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "FreeRTOS.h"
#include "queue.h"

typedef enum {
  UART__0,
  UART__1,
  UART__2,
  UART__3,
} uart_e;

/**
 * This initializes minimal UART communication
 * If RTOS is being utilized, you can use uart__enable_queues() to enable event driven implementation
 *
 * @param peripheral_clock Provide the clock frequency the peripheral is running at in order to set the baud_rate
 */
void uart__init(uart_e uart, uint32_t peripheral_clock, uint32_t baud_rate);

/**
 * If the RTOS is going to be used, then enabling the queues provides event driven implementation such that uart_get()
 * and uart__put() operate off of queues and you do not have to resort to using polled (cpu eating) version
 *
 * The transmit queue is used to hold data while the receive queue holds the arriving data
 * These queues need to be created by you before they are passed into the function.
 */
bool uart__enable_queues(uart_e uart, QueueHandle_t queue_receive, QueueHandle_t queue_transmit);

/**
 * Polled version to get a char; this never returns until a char is obtained
 * @returns false if UART was not initialized
 */
bool uart__polled_get(uart_e uart, char *input_byte);

/**
 * Polled version to output a char; this returns after outputting the byte
 * @returns false if UART was not initialized
 */
bool uart__polled_put(uart_e uart, char output_byte);

/**
 * @{
 * @name RTOS should be running to use these APIs
 * @note These APIs needs queues enabled, @see uart__enable_queues()
 *
 * For uart__get(), API will wait up to timeout_ms to receive a byte
 * For uart__put(), API will wait up to timeout_ms to write transmit queue
 */
bool uart__get(uart_e uart, char *input_byte, uint32_t timeout_ms);
bool uart__put(uart_e uart, char output_byte, uint32_t timeout_ms);
/** @} */

bool uart__is_enabled(uart_e uart);
