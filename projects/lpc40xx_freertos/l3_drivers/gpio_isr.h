#pragma once
#include <stdbool.h>
#include <stdint.h>

typedef enum { GPIO_INTR__FALLING_EDGE, GPIO_INTR__RISING_EDGE, GPIO_INTR__BOTH_EDGE } gpio_interrupt_e;

// Function pointer type (demonstrated later in the code sample)
typedef void (*function_pointer_t)(void);

void gpio0__attach_interrupt(uint32_t pin_num, gpio_interrupt_e interrupt_type, function_pointer_t call_back);
void gpio2__attach_interrupt(uint32_t pin_num, gpio_interrupt_e interrupt_type, function_pointer_t call_back);
