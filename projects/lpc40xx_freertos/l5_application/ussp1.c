#include "ussp1.h"

#include "clock.h"
#include "gpio.h"
#include "lpc40xx.h"

#define PCSSP1 11

void ussp1__init(uint32_t max_clock_mhz) {
  // Enable power
  LPC_SC->PCONP |= (1 << PCSSP1);

  // Setup control registers
  LPC_SSP1->CR0 |= 0x07;
  LPC_SSP1->CR1 |= (1 << 1);

  // Set clock
  uint32_t clk_mhz = clock__get_core_clock_hz() / 1000000UL;
  uint8_t clk_prescale = clk_mhz / max_clock_mhz;
  LPC_SSP1->CPSR = clk_prescale; // 96 / 4
}

uint8_t ussp1__exchange_byte(uint8_t data_out) {
  // Configure the Data register(DR) to send and receive data by checking the
  // status register
  LPC_SSP1->DR = data_out;
  while ((LPC_SSP1->SR >> 4) & 0x01) {
    ;
  }

  uint8_t recv = (LPC_SSP1->DR & 0xFF);
  return recv;
}

/**
 * NOTE, gpio_s not attached to them
 */
void ussp1__iocon_init(gpio_s *dc, gpio_s *cs) {
  // sck
  gpio__construct_with_function(GPIO__PORT_0, 7, 2);

  // miso
  gpio__construct_with_function(GPIO__PORT_0, 8, 2);

  // mosi
  gpio__construct_with_function(GPIO__PORT_0, 9, 2);

  *dc = gpio__construct_as_output(GPIO__PORT_1, 25);
  *cs = gpio__construct_as_output(GPIO__PORT_1, 22);
}
