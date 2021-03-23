#pragma once

#include <stdbool.h>
#include <stdint.h>

void ssp0__init(uint32_t max_clock_mhz);

uint8_t ssp0__exchange_byte(uint8_t data_out);