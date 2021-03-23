#include "u8g2_wrapper.h"

#include "delay.h"
#include "u8g2/u8g2.h"
#include "ussp1.h"

static gpio_s dc;
static u8g2_t u8g2;

void u8g2_wrapper__init(gpio_s *dc_pin) {
  u8g2_Setup_ssd1306_128x64_vcomh0_1(&u8g2, U8G2_R0, u8x8_byte_4wire_hw_spi, u8x8_lpc_gpio_and_delay);
  u8g2_InitDisplay(&u8g2);
  u8g2_SetPowerSave(&u8g2, 0);

  dc.pin_number = dc_pin->pin_number;
  dc.port_number = dc_pin->port_number;
}

u8g2_t *u8g2_wrapper__get_instance() { return &u8g2; }

uint8_t u8x8_lpc_gpio_and_delay(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr) {
  // fprintf(stderr, "gpio_and_delay: %d\n", msg);
  switch (msg) {
  case U8X8_MSG_GPIO_AND_DELAY_INIT:
    // vTaskDelay(1);
    delay__ms(1);
    // HAL_Delay(1);
    break;
  case U8X8_MSG_DELAY_MILLI:
    delay__ms(arg_int);
    // vTaskDelay(arg_int);
    break;
  case U8X8_MSG_GPIO_DC:
    // DONE, Write to the GPIO_DC Pin
    if (arg_int == 0) {
      gpio__reset(dc);
    } else {
      gpio__set(dc);
    }
    HAL_GPIO_WritePin(OLED_DC_GPIO_Port, OLED_DC_Pin, arg_int);
    break;
  case U8X8_MSG_GPIO_RESET:
    // DONE, Reset the GPIO_Reset Pin
    // printf("Reset\n");
    HAL_GPIO_WritePin(OLED_RES_GPIO_Port, OLED_RES_Pin, arg_int);
    break;
  }
  return 1;
}

// DONE, Do this later
uint8_t u8x8_byte_4wire_hw_spi(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr) {
  // fprintf(stderr, "hw: %d\n", msg);
  uint8_t *data;
  switch (msg) {
  case U8X8_MSG_BYTE_SEND:
    // fprintf(stderr, "Data\n");
    data = (uint8_t *)arg_ptr;
    while (arg_int > 0) {
      // printf("U8X8_MSG_BYTE_SEND : %d\n", (uint8_t)*data);
      ussp1__exchange_byte((uint8_t)*data);
      data++;
      arg_int--;
    }
    // HAL_SPI_Transmit(&hspi1, (uint8_t *)arg_ptr, arg_int, 10000);
    break;
  case U8X8_MSG_BYTE_INIT:
    // fprintf(stderr, "U8X8_MSG_BYTE_INIT\n");
    // u8x8_gpio_SetCS();
    break;
  case U8X8_MSG_BYTE_SET_DC:
    if (arg_int == 0) {
      gpio__reset(dc);
    } else {
      gpio__set(dc);
    }
    // HAL_GPIO_WritePin(OLED_DC_GPIO_Port, OLED_DC_Pin, arg_int);
    break;
  case U8X8_MSG_BYTE_START_TRANSFER:
    // fprintf(stderr, "U8X8_MSG_BYTE_START_TRANSFER\n");
    break;
  case U8X8_MSG_BYTE_END_TRANSFER:
    // fprintf(stderr, "U8X8_MSG_BYTE_END_TRANSFER\n");
    break;
  default:
    return 0;
  }
  return 1;
}
