#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"

#include "board_io.h"
#include "common_macros.h"
#include "delay.h"
#include "gpio.h"
#include "sj2_cli.h"

static void blink_task(void *params);
static void uart_task(void *params);
static void blink_on_startup(gpio_s gpio, int count);

static gpio_s led0, led1;

int main(void) {
  // Construct the LEDs and blink a startup sequence
  led0 = board_io__get_led0();
  led1 = board_io__get_led1();
  blink_on_startup(led1, 2);

  xTaskCreate(blink_task, "led0", (512U / sizeof(void *)), (void *)&led0, PRIORITY_LOW, NULL);
  xTaskCreate(blink_task, "led1", (512U / sizeof(void *)), (void *)&led1, PRIORITY_LOW, NULL);

  // It is advised to either run the uart_task, or the SJ2 command-line (CLI), but not both
  // Change '#if 0' to '#if 1' and vice versa to try it out
#if 0
  // printf() takes more stack space, size this tasks' stack higher
  xTaskCreate(uart_task, "uart", (512U * 8) / sizeof(void *), NULL, PRIORITY_LOW, NULL);
#else
  sj2_cli__init();
  UNUSED(uart_task); // uart_task is un-used in if we are doing cli init()
#endif

  puts("Starting RTOS");
  vTaskStartScheduler(); // This function never returns unless RTOS scheduler runs out of memory and fails

  return 0;
}

static void blink_task(void *params) {
  const gpio_s led = *((gpio_s *)params);

  while (true) {
    gpio__toggle(led);
    vTaskDelay(500);
  }
}

// This sends periodic messages over printf() which uses system_calls.c to send them to UART0
static void uart_task(void *params) {
  TickType_t previous_tick = 0;
  long ticks = 0;

  while (true) {
    // This loop will repeat at precise task delay, even if the logic below takes variable amount of ticks
    vTaskDelayUntil(&previous_tick, 2000);

    /* Calls to fprintf(stderr, ...) uses polled UART driver, so this entire output will be fully sent out
     * before this function returns. See system_calls.c for actual implementation.
     * This is useful to print information inside of interrupts as you cannot use printf() inside an ISR
     */
    ticks = xTaskGetTickCount();
    fprintf(stderr, "This is a polled version of printf used for debugging ... finished in");
    fprintf(stderr, " %lu ticks\n", (xTaskGetTickCount() - ticks));

    /* This deposits data to an outgoing queue and doesn't block the CPU
     * Data will be sent later, but this function would return earlier
     */
    ticks = xTaskGetTickCount();
    printf("This is a more efficient printf ... finished in");
    printf(" %lu ticks\n\n", (xTaskGetTickCount() - ticks));
  }
}

static void blink_on_startup(gpio_s gpio, int blinks) {
  const int toggles = (2 * blinks);
  for (int i = 0; i < toggles; i++) {
    delay__ms(250);
    gpio__toggle(gpio);
  }
}
