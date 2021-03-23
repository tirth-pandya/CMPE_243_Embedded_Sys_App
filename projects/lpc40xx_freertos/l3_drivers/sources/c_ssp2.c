#include "clock.h"
#include "lpc40xx.h"
#include "lpc_peripherals.h"
#include <stdio.h>

void c_ssp2__init(uint32_t max_clock_mhz) {
  // Refer to LPC User manual and setup the register bits correctly
  // a) Power on Peripheral
  LPC_SC->PCONP |= (1U << 20);

  // b) Setup control registers CR0 and CR1
  LPC_SSP2->CR0 = (7 << 0);
  LPC_SSP2->CR1 = (1 << 1);
  // c) Setup prescalar register to be <= max_clock_mhz
  uint32_t divider = 2;
  const uint32_t max_cpu_clock_mhz = clock__get_core_clock_hz();

  while (((max_cpu_clock_mhz / divider) > max_clock_mhz) && (254 >= divider)) {
    divider += 2;
  }

  LPC_SSP2->CPSR = divider;
}

uint8_t c_ssp2__exchange_byte(uint8_t data_out) {
  LPC_SSP2->DR = data_out;

  while (LPC_SSP2->SR & (1 << 4)) {
    ; // Wait until Tx FIFO is not empty
  }
  uint8_t data_received = (uint8_t)(LPC_SSP2->DR & 0xFF);
  return data_received;
}