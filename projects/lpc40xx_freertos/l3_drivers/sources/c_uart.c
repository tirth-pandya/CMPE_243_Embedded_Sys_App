#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"

#include <stdio.h>
#include <stdlib.h>

#include "c_uart.h"
#include "lpc40xx.h"
#include "lpc_peripherals.h"

#include "c_gpio.h"

/*******************************************************************************
 *
 *                                M A C R O S
 *
 ******************************************************************************/

// MACROS for LCR register
#define WORD_LENGTH_8 (3 << 0)
#define DLAB (1 << 7)
#define RESET_FDR_MULVAL (1 << 4)

// MACROS for LSR register
#define RECEIVE_DATA_READY (1 << 0)
#define TRANSMIT_HOLD_REG_EMPTY (1 << 5)

typedef LPC_UART_TypeDef lpc_uart;

/*******************************************************************************
 *
 *            S T R U C T U R E S    A N D    E N U M E R A T I O N S
 *
 ******************************************************************************/

// Enumerations of Interrupt ID
typedef enum {
  THRE_INTERRUPT = 0x1,
  RECEIVE_DATA_AVAILABLE = 0x2,
  RECEIVE_LINE_STATUS = 0x3,
  CHARACTER_TIMEOUT_INDICATOR = 0x6,
} interrupt_id_e;

typedef struct {
  lpc_uart *registers;
  lpc_peripheral_e peripheral;
  function__void_f isr_callback;
  bool is_initialized;
  QueueHandle_t rx_queue;
  QueueHandle_t tx_queue;
} c_uart__configuration;

/*******************************************************************************
 *
 *                    P R I V A T E    D E C L A R A T I O N S
 *
 ******************************************************************************/

static void uart__common_isr(c_uart_number_e uart_number);

static void uart0__isr(void);

static void uart1__isr(void);

static void uart2__isr(void);

static void uart3__isr(void);

static c_uart__configuration uart[] = {
    {(lpc_uart *)LPC_UART0, LPC_PERIPHERAL__UART0, uart0__isr, false},
    {(lpc_uart *)LPC_UART1, LPC_PERIPHERAL__UART1, uart1__isr, false},
    {(lpc_uart *)LPC_UART2, LPC_PERIPHERAL__UART2, uart2__isr, false},
    {(lpc_uart *)LPC_UART3, LPC_PERIPHERAL__UART3, uart3__isr, false},
};

static c_gpio_s led3 = {1, 18};

/*******************************************************************************
 *
 *                     P R I V A T E    F U N C T I O N S
 *
 ******************************************************************************/

static void uart__common_isr(c_uart_number_e uart_number) {
  // Read the IIR register to figure out why you got interrupted
  const uint32_t read_interrupt_reason = uart[uart_number].registers->IIR & 0xFF;
  // Read Interrupt status
  const uint32_t read_interrupt_status = read_interrupt_reason & (1 << 0);
  // Read interrupt ID
  const interrupt_id_e read_interrupt_id = (read_interrupt_reason >> 1);
  // It will be used when Rx interrupt occur
  char byte_to_read;
  // It will be used when Tx interrupt occur
  char byte_to_send;
  // Check if atleast one pending interrupt
  if (!read_interrupt_status) {
    // Check which interrupt occured using interrupt id
    switch (read_interrupt_id) {
    // If transmitter holding register empty interrupt occured. When THR FIFO is
    // empty
    case THRE_INTERRUPT:
      if (xQueueReceiveFromISR(uart[uart_number].tx_queue, &byte_to_send, NULL)) {
        uart[uart_number].registers->THR = byte_to_send;
      }
      break;
    // If receiver data available interrupt occured
    case RECEIVE_DATA_AVAILABLE:
    // Receive line status interupt occured (overrun error, parity error,
    // framing error)
    case RECEIVE_LINE_STATUS:
      // Check the status of LSR and read the RBR register.
      // The below function does the same thing.
      // Check LSR for Receive Data Ready
      while ((uart[uart_number].registers->LSR & RECEIVE_DATA_READY) == 1) {
        // Copy data from RBR register to byte_to_read
        byte_to_read = uart[uart_number].registers->RBR & (0xFF << 0);
        // Send the data in queue
        xQueueSendFromISR(uart[uart_number].rx_queue, &byte_to_read, NULL);
      }
      break;
    // Character timeout occured. Intended to flush the UARTs RBR
    case CHARACTER_TIMEOUT_INDICATOR:
      break;
    default:
      break;
    }
  }
}

/**
 * @brief This ISR is for UART0 and calls a common ISR function.
 *
 */
static void uart0__isr(void) { uart__common_isr(C_UART_0); }

/**
 * @brief This ISR is for UART1 and calls a common ISR function.
 *
 */
static void uart1__isr(void) { uart__common_isr(C_UART_1); }

/**
 * @brief This ISR is for UART2 and calls a common ISR function.
 *
 */
static void uart2__isr(void) { uart__common_isr(C_UART_2); }

/**
 * @brief This ISR is for UART3 and calls a common ISR function.
 *
 */
static void uart3__isr(void) { uart__common_isr(C_UART_3); }

/*******************************************************************************
 *
 *                     P U B L I C    F U N C T I O N S
 *
 ******************************************************************************/

void c_uart__enable_rx_tx_interrupt(c_uart_number_e uart_number) {
  // Attach interrupt
  lpc_peripheral__enable_interrupt(uart[uart_number].peripheral, uart[uart_number].isr_callback);

  // Set and enable uart rx and tx interrupt
  // Bit 1 is for THRE i.e tx interrupt
  // Bit 0 and 2 corresponds to Rx interrupt
  const uint32_t enable_rx_tx_interrupt = (1 << 0) | (1 << 2);
  uart[uart_number].registers->IER = enable_rx_tx_interrupt;
}

bool c_uart__get_char_from_queue(c_uart_number_e uart_number, char *byte, uint32_t timeout) {
  // Check is queue is empty or not
  if (!uart[uart_number].rx_queue)
    return false;
  else
    return xQueueReceive(uart[uart_number].rx_queue, byte, timeout);
}

bool c_uart__put_char_to_queue(c_uart_number_e uart_number, char output_byte, uint32_t timeout) {
  // Send data to queue, and return false if queue is full
  if (!xQueueSend(uart[uart_number].tx_queue, &output_byte, timeout)) {
    return false;
  }

  const uint32_t tx_is_empty = (1 << 6);
  // Trigger the interrupt
  if (uart[uart_number].registers->LSR & tx_is_empty) {
    if (xQueueReceive(uart[uart_number].tx_queue, &output_byte, 0)) {
      // Copy output_byte to THR register
      uart[uart_number].registers->THR = (output_byte << 0);
    }
  }
  return true;
}

bool c_uart__puts(c_uart_number_e uart_number, const char *output_string, uint32_t timeout) {
  if (!output_string)
    return false;
  while (*output_string) {
    if (!c_uart__put_char_to_queue(uart_number, *output_string++, timeout)) {
      return false;
    }
  }
  if (!c_uart__put_char_to_queue(uart_number, '\0', timeout)) {
    return false;
  }
  return true;
}

bool c_uart__polled_puts(c_uart_number_e uart_number, const char *output_string) {
  if (!output_string)
    return false;
  while (*output_string) {
    if (!c_uart__polled_put(uart_number, *output_string++)) {
      return false;
    }
  }
  if (!c_uart__polled_put(uart_number, '\0')) {
    return false;
  }
  return true;
}

void c_uart__init(c_uart_number_e uart_number, uint32_t peripheral_clock, uint32_t baud_rate, uint32_t rx_queue,
                  uint32_t tx_queue) {
  c_gpio__set_as_output(led3);
  c_gpio__set_high(led3);
  // Safety check whether UART is initialized or not
  if (uart[uart_number].is_initialized)
    return;
  // Get divider value based on peripheral clock and baud rate
  uint16_t divider = peripheral_clock / (16 * baud_rate);
  // Set bits in PCONP register to power on respective UART
  lpc_peripheral__turn_on_power_to(uart[uart_number].peripheral);
  // Enable DLAB bit from LCR register to set the baud rate
  uart[uart_number].registers->LCR = DLAB;
  // MSB of the baud rate
  uart[uart_number].registers->DLM = (divider >> 8) & 0xFF;
  // LSB of the baud rate
  uart[uart_number].registers->DLL = (divider >> 0) & 0xFF;
  // Good practice to reset(1) this bit as MulVal is in the denominator
  uart[uart_number].registers->FDR = RESET_FDR_MULVAL;
  // Set 8-bit character length which will reset the DLAB bit to 0
  uart[uart_number].registers->LCR = WORD_LENGTH_8;
  // Create Rx queue of user defined size
  uart[uart_number].rx_queue = xQueueCreate(rx_queue, sizeof(char));
  // Create Tx queue of user defined size
  uart[uart_number].tx_queue = xQueueCreate(tx_queue, sizeof(char));
  // Set this flag to true in order to avoid re-initialization and make sure
  // uart is initialized
  uart[uart_number].is_initialized = true;
}

bool c_uart__polled_get(c_uart_number_e uart_number, char *input_byte) {
  bool status = false;
  // Safety check whether uart is initialized or not
  if (uart[uart_number].is_initialized == true) {
    // Success
    status = true;
    // Check LSR for Receive Data Ready
    while ((uart[uart_number].registers->LSR & RECEIVE_DATA_READY) == 0) {
      ;
    }
    // Copy data from RBR register to input_byte
    *input_byte = uart[uart_number].registers->RBR & (0xFF << 0);
  }
  return status;
}

bool c_uart__polled_put(c_uart_number_e uart_number, char output_byte) {
  bool status = false;
  // Safety check whether uart is initialized or not
  if (uart[uart_number].is_initialized == true) {
    // Success
    status = true;
    // Wait for transmit hold register to get empty in LSR
    while ((uart[uart_number].registers->LSR & TRANSMIT_HOLD_REG_EMPTY) == 0) {
      ;
    }
    // Copy output_byte to THR register
    uart[uart_number].registers->THR = (output_byte << 0);
  }
  return status;
}