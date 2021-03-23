#include "c_gpio.h"
#include "lpc40xx.h"
#include <stddef.h>

/*******************************************************************************
 *
 *                        D E F I N E  V A R I A B L E S
 *
 ******************************************************************************/
#define GPIO_FUNCTION 0
#define GPIO_MODE 3
#define GPIO_ADMODE 7

/*******************************************************************************
 *
 *                      S T A T I C    V A R I A B L E S
 *
 ******************************************************************************/

/*******************************************************************************
 *
 *                      P R I V A T E    F U N C T I O N S
 *
 ******************************************************************************/

static LPC_GPIO_TypeDef *get__lpc_gpio_struct(uint8_t port_num) {
  return ((LPC_GPIO_TypeDef *)(LPC_GPIO0_BASE + (32 * port_num)));
}

/*******************************************************************************
 *
 *                      P U B L I C    F U N C T I O N S
 *
 ******************************************************************************/

void c_gpio__set_as_function(c_gpio_s gpio, c_gpio__function_e function) {
  volatile uint32_t *iocon_base = (volatile uint32_t *)LPC_IOCON;

  // Each port is offset by 32-bit register
  volatile uint32_t *port_offset = iocon_base + (32 * gpio.port_num);

  // Each pin configuration is a single 32-bit
  volatile uint32_t *pin_config = (port_offset + gpio.pin_num);

  const uint32_t function_mask = UINT32_C(7);
  *pin_config &= ~(function_mask << GPIO_FUNCTION);
  *pin_config |= ((uint32_t)function & function_mask);
}

void c_gpio__set_as_input(c_gpio_s gpio) { get__lpc_gpio_struct(gpio.port_num)->DIR &= ~(1 << gpio.pin_num); }

void c_gpio__set_as_output(c_gpio_s gpio) { get__lpc_gpio_struct(gpio.port_num)->DIR |= (1 << gpio.pin_num); }

void c_gpio__set_high(c_gpio_s gpio) { get__lpc_gpio_struct(gpio.port_num)->SET = (1 << gpio.pin_num); }

void c_gpio__set_low(c_gpio_s gpio) { get__lpc_gpio_struct(gpio.port_num)->CLR = (1 << gpio.pin_num); }

void c_gpio__set(c_gpio_s gpio, bool high) {
  if (high) {
    c_gpio__set_high(gpio);
  } else {
    c_gpio__set_low(gpio);
  }
}

bool c_gpio__get_level(c_gpio_s gpio) { return (get__lpc_gpio_struct(gpio.port_num)->PIN & (1 << gpio.pin_num)); }

void c_gpio__toggle(c_gpio_s gpio) {
  if (true == c_gpio__get_level(gpio)) {
    c_gpio__set_low(gpio);
  } else {
    c_gpio__set_high(gpio);
  }
}

void c_gpio__enable_pull_up(c_gpio_s gpio) {
  volatile uint32_t *IOCON_base = (volatile uint32_t *)LPC_IOCON;

  // Each port is offset by thirty-two 32-bit registers
  volatile uint32_t *port_offset = IOCON_base + (32 * gpio.port_num);

  // Each pin configuration is a single 32-bit
  volatile uint32_t *pin_config = (port_offset + gpio.pin_num);
  *pin_config &= ~(1 << 3); // Reset 3rd bit to 0
  *pin_config |= (1 << 4);  // Set 4th bit to 1
}

void c_gpio__enable_pull_down(c_gpio_s gpio) {
  volatile uint32_t *IOCON_base = (volatile uint32_t *)LPC_IOCON;

  // Each port is offset by thirty-two 32-bit registers
  volatile uint32_t *port_offset = IOCON_base + (32 * gpio.port_num);

  // Each pin configuration is a single 32-bit
  volatile uint32_t *pin_config = (port_offset + gpio.pin_num);
  *pin_config &= ~(1 << 4); // Reset 4th bit to 0
  *pin_config |= (1 << 3);  // Set 3rd bit to 1
}

void c_gpio__set_as_digital(c_gpio_s gpio) {
  volatile uint32_t *iocon_base = (volatile uint32_t *)LPC_IOCON;

  // Each port is offset by 32-bit register
  volatile uint32_t *port_offset = iocon_base + (32 * gpio.port_num);

  // Each pin configuration is a single 32-bit
  volatile uint32_t *pin_config = (port_offset + gpio.pin_num);

  const uint32_t ad_mask = UINT32_C(1);

  *pin_config &= ~(ad_mask << GPIO_ADMODE);

  *pin_config |= (ad_mask << GPIO_ADMODE);
}

void c_gpio__set_as_analog(c_gpio_s gpio) {
  volatile uint32_t *iocon_base = (volatile uint32_t *)LPC_IOCON;

  // Each port is offset by 32-bit register
  volatile uint32_t *port_offset = iocon_base + (32 * gpio.port_num);

  // Each pin configuration is a single 32-bit
  volatile uint32_t *pin_config = (port_offset + gpio.pin_num);

  const uint32_t ad_mask = UINT32_C(1);

  *pin_config &= ~(ad_mask << GPIO_ADMODE);
}