#pragma once

#include "gpio.h"
#include "u8g2/u8g2.h"

/**
 * Init the u8g2 driver
 * Give reference to an initialized dc pin for usage
 */
void u8g2_wrapper__init(gpio_s *dc_pin);

u8g2_t *u8g2_wrapper__get_instance();

/**
 * TODO, Improve, make this static
 * gpio control and delay Wrapper function
 */
uint8_t u8x8_lpc_gpio_and_delay(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);

/**
 * TODO, Improve, make this static
 * Hardware SPI Wrapper
 */
uint8_t u8x8_byte_4wire_hw_spi(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);
