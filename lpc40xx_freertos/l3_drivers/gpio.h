#pragma once

#include <stdbool.h>
#include <stdint.h>

/// Note that some ports may not be available on your microcontroller
typedef enum {
  gpio__port_0 = 0,
  gpio__port_1,
  gpio__port_2,
  gpio__port_3,
  gpio__port_4,
  gpio__port_5,
} gpio__port_e;

/**
 * Each pin on LPC40xx has up to 8 functions
 * The functions are specific to a pin and vary; only function 0 is common, which is a GPIO
 */
typedef enum {
  gpio__function_0_io_pin = 0,
  gpio__function_1,
  gpio__function_2,
  gpio__function_3,
  gpio__function_4,
  gpio__function_5,
  gpio__function_6,
  gpio__function_7,
} gpio__function_e;

/**
 * This struct is returned gpio__construct()
 * All further APIs need instance of this struct to operate
 */
typedef struct {
  uint8_t port_number;
  uint8_t pin_number;
} gpio_s;

/**
 * A pin must be constructed to use further API
 * @param pin_number_0_to_31 The pin number from 0-31
 */
gpio_s gpio__construct(gpio__port_e port, uint8_t pin_number_0_to_31);
gpio_s gpio__construct_as_input(gpio__port_e port, uint8_t pin_number_0_to_31);  ///< construct then set as input
gpio_s gpio__construct_as_output(gpio__port_e port, uint8_t pin_number_0_to_31); ///< construct then set as output

/// Construct and then set the pin function. In this case, DIR will not impact the pin
gpio_s gpio__construct_with_function(gpio__port_e port, uint8_t pin_number_0_to_31, gpio__function_e function);

/// Sets the function for a pin; @see gpio__function_e
void gpio__set_function(gpio_s gpio, gpio__function_e function);

/**
 * @{
 * @name Sets direction of the pin if used as an input/output pin
 * @warning This API is not thread safe
 */
void gpio__set_as_input(gpio_s gpio);
void gpio__set_as_output(gpio_s gpio);
/** @} */

bool gpio__get(gpio_s gpio);
void gpio__set(gpio_s gpio);   ///< Sets the pin value as 'high' -> 3.3v
void gpio__reset(gpio_s gpio); ///< Sets the pin value as 'low' -> ground
void gpio__toggle(gpio_s gpio);
