#include "gpio_isr.h"
#include "lpc40xx.h"
#include "lpc_peripherals.h"
#include <stddef.h>
#include <stdlib.h>

static function_pointer_t gpio0__falling_callbacks[32];
static function_pointer_t gpio0__rising_callbacks[32];

static function_pointer_t gpio2__falling_callbacks[32];
static function_pointer_t gpio2__rising_callbacks[32];

static int32_t return_pin_that_triggered_interrupt(uint32_t intr_status_reg) {
  if (intr_status_reg) {
    for (int i = 0; i < 32; i++) {
      if (intr_status_reg & (UINT32_C(1) << i)) {
        return i;
      }
    }
  }
  return -1;
}

static void handle__gpio_interrupt(uint32_t *isr_status_regs, volatile uint32_t *intr_clear_reg_ptr,
                                   function_pointer_t *gpio_isr_callbacks) {
  // Check which pin generated the interrupt
  const int32_t pin_that_generated_interrupt = return_pin_that_triggered_interrupt(*isr_status_regs);
  if (pin_that_generated_interrupt == -1)
    return;

  function_pointer_t attached_user_handler = gpio_isr_callbacks[pin_that_generated_interrupt];
  // Invoke the user registered callback, and then clear the interrupt
  attached_user_handler();
  *isr_status_regs &= ~(UINT32_C(1) << pin_that_generated_interrupt);
  *intr_clear_reg_ptr |= (UINT32_C(1) << pin_that_generated_interrupt);
}

static void gpio__interrupt_dispatcher(void) {
  // Read all ports' interrupt status
  uint32_t port0_falling_intr = LPC_GPIOINT->IO0IntStatF;
  uint32_t port0_rising_intr = LPC_GPIOINT->IO0IntStatR;
  uint32_t port2_falling_intr = LPC_GPIOINT->IO2IntStatF;
  uint32_t port2_rising_intr = LPC_GPIOINT->IO2IntStatR;

  /**
   * Go through each array list to handle the ISR using the pin number on
   * which interrupt occurred. It can be checked using IO0IntStatF, IO0IntStatR,
   * IO2IntStatF, and IO2IntStatR
   * It will also clear that particular pin on which interrupt occured by
   * writing either to IO0IntClr or IO2IntClr
   */
  handle__gpio_interrupt(&port0_falling_intr, &LPC_GPIOINT->IO0IntClr, gpio0__falling_callbacks);
  handle__gpio_interrupt(&port0_rising_intr, &LPC_GPIOINT->IO0IntClr, gpio0__rising_callbacks);
  handle__gpio_interrupt(&port2_falling_intr, &LPC_GPIOINT->IO2IntClr, gpio2__falling_callbacks);
  handle__gpio_interrupt(&port2_rising_intr, &LPC_GPIOINT->IO2IntClr, gpio2__rising_callbacks);

  if (port0_falling_intr || port0_rising_intr)
    LPC_GPIOINT->IO0IntClr = 0xFFFFFFFF;
  if (port2_falling_intr || port2_rising_intr)
    LPC_GPIOINT->IO2IntClr = 0xFFFFFFFF;
}

void gpio0__attach_interrupt(uint32_t pin_num, gpio_interrupt_e interrupt_type, function_pointer_t call_back) {
  uint32_t pin_mask = (UINT32_C(1) << pin_num);
  switch (interrupt_type) {
  case GPIO_INTR__RISING_EDGE:
    gpio0__rising_callbacks[pin_num] = call_back;
    LPC_GPIOINT->IO0IntEnR |= pin_mask;
    break;
  case GPIO_INTR__FALLING_EDGE:
    gpio0__falling_callbacks[pin_num] = call_back;
    LPC_GPIOINT->IO0IntEnF |= pin_mask;
    break;
  default:
    break;
  }
  lpc_peripheral__enable_interrupt(LPC_PERIPHERAL__GPIO, gpio__interrupt_dispatcher);
}

void gpio2__attach_interrupt(uint32_t pin_num, gpio_interrupt_e interrupt_type, function_pointer_t call_back) {
  uint32_t pin_mask = (UINT32_C(1) << pin_num);
  switch (interrupt_type) {
  case GPIO_INTR__RISING_EDGE:
    gpio2__rising_callbacks[pin_num] = call_back;
    LPC_GPIOINT->IO2IntEnR |= pin_mask;
    break;
  case GPIO_INTR__FALLING_EDGE:
    gpio2__falling_callbacks[pin_num] = call_back;
    LPC_GPIOINT->IO2IntEnF |= pin_mask;
    break;
  default:
    break;
  }
  lpc_peripheral__enable_interrupt(LPC_PERIPHERAL__GPIO, gpio__interrupt_dispatcher);
}
