#ifndef _USSP_ONE_H
#define _USSP_ONE_H

#include "gpio.h"
#include <stdint.h>

void ussp1__init(uint32_t max_clock_mhz);
uint8_t ussp1__exchange_byte(uint8_t data_out);

void ussp1__iocon_init(gpio_s *dc, gpio_s *cs);

#endif
