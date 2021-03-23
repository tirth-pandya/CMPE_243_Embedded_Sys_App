
#include "FreeRTOS.h"
#include "queue.h"

#include "lab_uart.h"
#include "lpc40xx.h"
#include "lpc_peripherals.h"

// Private queue handle of our uart_lab.c
static QueueHandle_t your_uart_rx_queue;

static uart_selection uart_table[] = {
    {(uart_num *)LPC_UART0, PCUART0},
    {(uart_num *)LPC_UART1, PCUART1},
    {(uart_num *)LPC_UART2, PCUART2},
    {(uart_num *)LPC_UART3, PCUART3},
};

void uart_lab__init(uart_number_e uart, uint32_t peripheral_clock, uint32_t baud_rate) {
  uint32_t baud_val = (peripheral_clock / (16 * baud_rate));

  uart_num *uart_reg = uart_table[uart].registers;
  uint8_t power_pin = uart_table[uart].power_pin;

  // a) Power on Peripheral
  LPC_SC->PCONP |= (1 << power_pin);
  // b) Setup DLL, DLM, FDR, LCR registers
  uart_reg->LCR = (1 << DLAB_BIT); // Set DLAB = 1
  uart_reg->DLL = baud_val & 0xFF;
  uart_reg->DLM = (baud_val >> 0x08) & 0xFF;

  uart_reg->FDR = (1 << 4);
  uart_reg->LCR = 3; // reset DLAB
}

bool uart_lab__polled_get(uart_number_e uart, char *input_byte) {
  bool rdr_status = 0;
  uart_num *uart_reg = uart_table[uart].registers;

  // a) Check LSR for Receive Data Ready
  while (!(uart_reg->LSR & (1 << RDR))) {
    ;
  }

  rdr_status = true;
  // b) Copy data from RBR register to input_byte
  *input_byte = uart_reg->RBR;

  return rdr_status;
}

bool uart_lab__polled_put(uart_number_e uart, char output_byte) {
  bool thre_status = 0;
  uart_num *uart_reg = uart_table[uart].registers;
  // a) Check LSR for Transmit Hold Register Empty
  // thre_status = uart_reg->LSR & (1 << THRE);

  while (!(uart_reg->LSR & (1 << THRE))) {
    ;
  }
  // b) Copy output_byte to THR register
  thre_status = true;
  uart_reg->THR = output_byte;

  return thre_status;
}

// Private function of our uart_lab.c
static void uart3_receive_interrupt(void) {
  // TODO: Read the IIR register to figure out why you got interrupted
  if ((LPC_UART3->IIR >> 1) & 0x03) {
    // TODO: Based on IIR status, read the LSR register to confirm if there is
    // data to be read
    while (!(LPC_UART3->LSR & (1 << RDR))) {
    }

    if (LPC_UART3->LSR & (1 << RDR)) {
      // TODO: Read the RBR register and input the data to the RX Queue
      const char byte = LPC_UART3->RBR;
      xQueueSendFromISR(your_uart_rx_queue, &byte, NULL);
    }
  }
}

static void uart2_receive_interrupt(void) {
  if ((LPC_UART2->IIR >> 1) & 0x03) {
    // TODO: Based on IIR status, read the LSR register to confirm if there is
    // data to be read
    while (!(LPC_UART2->LSR & (1 << RDR))) {
    }

    if (LPC_UART2->LSR & (1 << RDR)) {
      // TODO: Read the RBR register and input the data to the RX Queue
      const char byte = LPC_UART2->RBR;
      xQueueSendFromISR(your_uart_rx_queue, &byte, NULL);
    }
  }
}

// Public function to enable UART interrupt
void uart__enable_receive_interrupt(uart_number_e uart_number) {
  uart_num *uart_reg = uart_table[uart_number].registers;

  if (uart_number == uart__3) {
    lpc_peripheral__enable_interrupt(LPC_PERIPHERAL__UART3, uart3_receive_interrupt);
    uart_reg->IER = (1 << 0) | (1 << 2);
  } else if (uart_number == uart__2) {
    lpc_peripheral__enable_interrupt(LPC_PERIPHERAL__UART2, uart2_receive_interrupt);
    uart_reg->IER = (1 << 0) | (1 << 2);
  }

  your_uart_rx_queue = xQueueCreate(16, sizeof(char));
}

// Public function to get a char from the queue (this function should work
// without modification)
bool uart_lab__get_char_from_queue(char *input_byte, uint32_t timeout) {
  bool status = xQueueReceive(your_uart_rx_queue, input_byte, timeout);
  return status;
}