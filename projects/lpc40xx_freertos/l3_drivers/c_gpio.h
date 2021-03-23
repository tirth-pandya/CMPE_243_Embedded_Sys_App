#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "lpc40xx.h"

typedef enum {
  GPIO_PORT_0 = 0,
  GPIO_PORT_1,
  GPIO_PORT_2,
  GPIO_PORT_3,
  GPIO_PORT_4,
  GPIO_PORT_5,
} c_gpio__port_e;

typedef enum {
  c_gpio__function_0_io_pin = 0,
  c_gpio__function_1,
  c_gpio__function_2,
  c_gpio__function_3,
  c_gpio__function_4,
  c_gpio__function_5,
  c_gpio__function_6,
  c_gpio__function_7,
} c_gpio__function_e;

typedef struct {
  uint8_t port_num;
  uint8_t pin_num;
} c_gpio_s;

/// Alter the hardware register to set the function of the port pin
void c_gpio__set_as_function(c_gpio_s gpio, c_gpio__function_e function);

/// Alter the hardware registers to set the pin as input
void c_gpio__set_as_input(c_gpio_s gpio);

/// Alter the hardware registers to set the pin as output
void c_gpio__set_as_output(c_gpio_s gpio);

/// Alter the hardware registers to set the pin as high
void c_gpio__set_high(c_gpio_s gpio);

/// Alter the hardware registers to set the pin as low
void c_gpio__set_low(c_gpio_s gpio);

/**
 *
 * @param {bool} high - true => set pin high, false => set pin low
 */
void c_gpio__set(c_gpio_s gpio, bool high);

/**
 * Should return the state of the pin (input or output, doesn't matter)
 *
 * @return {bool} level of pin high => true, low => false
 */
bool c_gpio__get_level(c_gpio_s gpio);

void c_gpio__toggle(c_gpio_s gpio);

void c_gpio__enable_pull_up(c_gpio_s gpio);

void c_gpio__enable_pull_down(c_gpio_s gpio);

void c_gpio__set_as_digital(c_gpio_s gpio);

void c_gpio__set_as_analog(c_gpio_s gpio);
