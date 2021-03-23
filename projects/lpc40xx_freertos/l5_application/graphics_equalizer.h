#pragma once
#include <stdint.h>

void graphics_equalizer__init(void);
void graphics_equalizer__reset_high(void);
void graphics_equalizer__reset_low(void);
void graphics_equalizer__strobe_high(void);
void graphics_equalizer__strobe_low(void);
// uint16_t graphics_equalizer__get_adc_reading(void);
void graphics_equalizer__read_frequency(uint16_t *value);
void graphics_equalizer__reset_module(void);