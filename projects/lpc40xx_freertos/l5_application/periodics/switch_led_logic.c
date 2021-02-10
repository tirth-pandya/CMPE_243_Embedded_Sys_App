#include "switch_led_logic.h"

#include "board_io.h"
#include "gpio.h"

static gpio_s led_0;
static gpio_s switch_0;

void switch_led_logic__initialize() {
  led_0 = gpio__construct_as_output(GPIO__PORT_2, 3);
  switch_0 = gpio__construct_as_input(GPIO__PORT_1, 19);
}

void switch_led_logic__run_once() {
  if (gpio__get(switch_0)) {
    gpio__set(led_0);
  } else {
    gpio__reset(led_0);
  }
}