#pragma once

#include <stdbool.h>
#include <stdint.h>

void c_ssp2__init(uint32_t max_clock_mhz);

uint8_t c_ssp2__exchange_byte(uint8_t data_out);