#include "clock.h"
#include "lpc40xx.h"
#include "lpc_peripherals.h"
#include <stdio.h>

void ssp0__init(uint32_t max_clock_mhz) {
  // Turn on SSP peripheral
  LPC_SC->PCONP |= (1U << 20);

  // Set control register CR0 and CR1
  LPC_SSP0->CR0 = (7 << 0);
  LPC_SSP0->CR1 = (1 << 1);

  // Set prescalar register
  uint32_t divider = 2;
  const uint32_t max_cpu_clock_mhz = clock__get_core_clock_hz();

  while (((max_cpu_clock_mhz / divider) > max_clock_mhz) && (254 >= divider)) {
    divider += 2;
  }

  LPC_SSP0->CPSR = divider;
}

uint8_t ssp0__exchange_byte(uint8_t data_out) {
  LPC_SSP0->DR = data_out;
  while (LPC_SSP0->SR & (1 << 4)) {
    ; // Wait until Tx FIFO is not empty
  }
  uint8_t data_received = (LPC_SSP0->DR & 0xFF);
  return data_received;
}