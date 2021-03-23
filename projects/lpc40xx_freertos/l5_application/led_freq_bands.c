#include "game_fruit_fury_led_driver.h"
#include "graphics_equalizer.h"

#define MAX_ROW 32
#define MAX_COL 64
#define MAX_FREQ_BANDS 7

static uint8_t band_matrix[MAX_ROW][MAX_COL];
static uint16_t freq[MAX_FREQ_BANDS], band_height[MAX_FREQ_BANDS];

TaskHandle_t handle__freq_bands = NULL;

static void construct_band_matrix(uint8_t band_num) {
  uint8_t row, col;
  memset(band_matrix, 0, sizeof(band_matrix));

  for (row = MAX_ROW - 1; row >= 1; row--) {
    for (col = 7; col <= 62; col++) {
      if ((row >= (MAX_ROW - band_height[0])) && col >= 7 && col <= 13 && band_num <= 0) {
        band_matrix[row][col] = RED;
      }
      if ((row >= (MAX_ROW - band_height[1])) && col >= 14 && col <= 20 && band_num <= 1) {
        band_matrix[row][col] = GREEN;
      }
      if ((row >= (MAX_ROW - band_height[2])) && col >= 21 && col <= 27 && band_num <= 2) {
        band_matrix[row][col] = YELLOW;
      }
      if ((row >= (MAX_ROW - band_height[3])) && col >= 28 && col <= 34 && band_num <= 3) {
        band_matrix[row][col] = BLUE;
      }
      if ((row >= (MAX_ROW - band_height[4])) && col >= 35 && col <= 41 && band_num <= 4) {
        band_matrix[row][col] = PURPLE;
      }
      if ((row >= (MAX_ROW - band_height[5])) && col >= 42 && col <= 48 && band_num <= 5) {
        band_matrix[row][col] = CYAN;
      }
      if ((row >= (MAX_ROW - band_height[6])) && col >= 49 && col <= 55 && band_num <= 6) {
        band_matrix[row][col] = WHITE;
      }
    }
  }
}

static void get_freq_values() {
  graphics_equalizer__read_frequency(&freq[0]);
  uint16_t result;
  for (int i = 0; i < MAX_FREQ_BANDS; i++) {
    result = freq[i] * 0.9;
    if (result < 500)
      result = 0;
    result = result / 128;
    if (result < 12)
      band_height[i] = 0;
    else {
      band_height[i] = result;
    }
  }
}

#if 0
static void display_band_matrix() {
  uint8_t row, col;
  // for (uint8_t k = 0; k < 10; k++) {
  for (row = 0; row < (MAX_ROW / 2); row++) {
    disableOE();
    set_row(row);
    for (col = 0; col < MAX_COL; col++) {
      LPC_GPIO0->PIN |= (1 << CLK);
      set_color_bottom(band_matrix[row + (MAX_ROW / 2)][col]);
      set_color_top(band_matrix[row][col]);
      LPC_GPIO0->PIN &= ~(1 << CLK);
    }
    LPC_GPIO0->PIN |= (1 << LAT);
    LPC_GPIO0->PIN &= ~(1 << LAT);
    enableOE();
    vTaskDelay(1);
  }
  vTaskDelay(2);
  // }
#endif
#if 1
static void display_band_matrix() {
  uint8_t row, col;
  for (uint8_t i = 0; i < MAX_FREQ_BANDS; i++) {
    construct_band_matrix(i);
    for (row = 0; row < MAX_ROW; row++) {
      disableOE();
      set_row(row);
      for (col = 0; col < MAX_COL; col++) {
        LPC_GPIO0->PIN |= (1 << CLK);
        if (row > 0 && row < (MAX_ROW / 2)) {
          set_color_top(band_matrix[row][col]);
        }
        set_color_bottom(band_matrix[row][col]);
        LPC_GPIO0->PIN &= ~(1 << CLK);
      }
      LPC_GPIO0->PIN |= (1 << LAT);
      LPC_GPIO0->PIN &= ~(1 << LAT);
      enableOE();
      vTaskDelay(1);
    }
  }
}
#endif

static void display_freq_bands() {
  while (1) {
    get_freq_values();
    display_band_matrix();
    vTaskDelay(20);
  }
}

void led_test(void) {
  led_init();
  graphics_equalizer__init();
  xTaskCreate(display_freq_bands, "display_freq_bands", 1024, NULL, PRIORITY_HIGH, &handle__freq_bands);
}
