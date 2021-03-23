#include <stdio.h>
#include <string.h>

#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "task.h"

#include "VS1053B_map.h"
#include "apds.h"
#include "ble.h"
#include "c_gpio.h"
#include "common_macros.h"
#include "ff.h"
#include "gpio_isr.h"
#include "graphics_equalizer.h"
#include "led_freq_bands.h"
#include "mp3_shield.h"
#include "sdcard.h"
#include "sensor.h"
#include "sj2_cli.h"
#include "ssp0.h"
#include <math.h>
#include <stdlib.h>
#include <time.h>

#include "u8g2_wrapper.h"
#include "ussp1.h"

/*******************************************************************************
 *
 *       D E F I N E    A N D    E N U M E R A T I O N    V A R I A B L E S
 *
 ******************************************************************************/
#define READ_BYTES_FROM_FILE 4 * 1024

/*******************************************************************************
 *
 *                      S T A T I C    V A R I A B L E S
 *
 ******************************************************************************/

static const c_gpio_s ssp0_mosi = {0, 18U}; // Function 2
static const c_gpio_s ssp0_miso = {0, 17U}; // Function 2
static const c_gpio_s ssp0_sck = {0, 15U};  // Function 2

static const c_gpio_s sw3__prev_song = {0, 29U}; // Switch 3
static const c_gpio_s sw2__next_song = {0, 30U}; // Switch 2
static const c_gpio_s sw1__vol_low = {1U, 15U};  // Switch 1. Internal pull down must be enabled
static const c_gpio_s sw0__vol_high = {1U, 10U}; // Switch 0. Internal pull down must be enabled

static const uint16_t TRANSMIT_BYTES_TO_DECODER = 32U;

static const uint16_t COUNT = READ_BYTES_FROM_FILE / TRANSMIT_BYTES_TO_DECODER;
static SemaphoreHandle_t mp3_mutex = NULL;
static QueueHandle_t mp3_queue = NULL;
static SemaphoreHandle_t sem__sw3;
static SemaphoreHandle_t sem__sw2;

/*******************************************************************************
 *
 *                      G L O B A L    V A R I A B L E S
 *
 ******************************************************************************/
uint32_t total_number_of_songs;
song__info_s song__recent_info;
char songs_list[SONG_CAPACITY][SONG_NAME_LENGTH];
extern SemaphoreHandle_t sem__next_song;
extern SemaphoreHandle_t sem__prev_song;

TaskHandle_t handle__read_song;
TaskHandle_t handle__play_song;

// Used for u8g2 function
static gpio_s dc, cs;

/*******************************************************************************
 *
 *          P R I V A T E    F U N C T I O N S    D E C L A R A T I O N
 *
 ******************************************************************************/
static void sj2__configure_ssp0_pins(void);
static void sj2__configure_onboard_switches_to_control_mp3_player(void);
static void sj2__enable_gpio_interuppt(void);
static void task__medium_priority_to_read_mp3_file(void *pvParam);
static void task__high_priority_to_send_mp3_data_to_decoder(void *pvParam);
static void get_song_list_from_sd_card(void);
static void sw3__falling_edge_isr(void);
static void sw2__falling_edge_isr(void);

// static void mp3__get_next_song(song__info_s *song);
// static void mp3__get_prev_song(song__info_s *song);
// static void mp3__get_file_name(song__info_s *song);
static void send_32_bytes_to_decoder_using_ssp(const uint32_t start, const uint32_t end, const uint8_t *bytes_to_send);

void game_entry_point();

/*******************************************************************************
 *
 *                        M A I N    F U N C T I O N
 *
 ******************************************************************************/
int main(void) {
  sj2_cli__init();

  graphics_equalizer__init();
  // Configue SJ2 onboard switches
  sj2__configure_onboard_switches_to_control_mp3_player();

  // Configure SJ2 SSP2 pins
  sj2__configure_ssp0_pins();

  // Initialize SSP2 with 1MHz because
  // From page 22 of datasheet, max SCI reads are CLKI/7. Input clock
  // is 12.288MHz. Internal clock multiplier is 1.0x after power up. Therefore,
  // max SPI speed is 1.75MHz. We will use 1MHz to be safe.
  uint32_t spi_clock_mhz = 1 * 1000 * 1000;
  ssp0__init(spi_clock_mhz);

  // Configure SJ2 pins to interface with MP3 Shield & initialize MP3 Shield
  // Decode Module
  mp3__dreq_init();
  mp3__hardware_reset();
  mp3__decoder_initialize();

  // Internal clock multiplier is now 4.5x
  // Therefore, max SPI speed is 12.XXMHz. 15MHz will be safe.
  spi_clock_mhz = spi_clock_mhz * 12;
  ssp0__init(spi_clock_mhz);

  ussp1__init(16);
  ussp1__iocon_init(&dc, &cs);
  gpio__reset(cs);

  u8g2_wrapper__init(&dc);
  u8g2_t *u8g2_i = u8g2_wrapper__get_instance();
  u8g2_FirstPage(u8g2_i);

  u8g2_DrawHLine(u8g2_i, 1, 1, 20);
  // u8g2_SetFont(u8g2_i, u8g2_font_ncenB14_tr);
  // u8g2_DrawStr(u8g2_i, 0, 15, "Infinity");

  // mp3_mutex = xSemaphoreCreateMutex();
  // mp3_queue = xQueueCreate(1, sizeof(uint8_t[READ_BYTES_FROM_FILE]));

  // sem__sw3 = xSemaphoreCreateBinary();
  // sem__sw2 = xSemaphoreCreateBinary();

  // sem__next_song = xSemaphoreCreateBinary();
  // sem__prev_song = xSemaphoreCreateBinary();

  // get_song_list_from_sd_card();

  sj2__enable_gpio_interuppt();

  if (true == c_gpio__get_level(sw3__prev_song)) {
    printf("STARTING MP3 PLAYER\n");
    sj2__mode = MP3_MODE;
    // orient_data = xQueueCreate(1, sizeof(orientation_e));
    // clang-format off
    //xTaskCreate(perform_action_on_orientation, "control_mp3", (512U * 8) / sizeof(void *), NULL, PRIORITY_LOW, NULL);
    led_test();
    // clang-format on
  } else {
    printf("STARTING FRUIT FURY GAME\n");
    sj2__mode = GAME_MODE;
    // Enable SJ2 GPIO interrupt
    srand(time(0));
    game_entry_point();
  }

  uart_lab__init(UART_NUMBER, clock__get_peripheral_clock_hz(), 38400);
  uart_pin_config_as_io(UART_NUMBER);
  uart__enable_receive_interrupt(UART_NUMBER);
  // xTaskCreate(get_app_data, "Get_APP_data", (512U * 8) / sizeof(void *), NULL, PRIORITY_LOW, NULL);

  // clang-format off
 // xTaskCreate(task__medium_priority_to_read_mp3_file, "Read_song", configMINIMAL_STACK_SIZE * 14, NULL, PRIORITY_LOW, NULL);
 // xTaskCreate(task__high_priority_to_send_mp3_data_to_decoder, "Send_song", configMINIMAL_STACK_SIZE * 12, NULL, PRIORITY_MEDIUM, &handle__play_song);
  // clang-format on

  puts("Starting RTOS");
  vTaskStartScheduler(); // This function never returns unless RTOS scheduler
                         // runs out of memory and fails
  return 0;
}

/*******************************************************************************
 *
 *                      P R I V A T E    F U N C T I O N S
 *
 ******************************************************************************/
static void sj2__configure_ssp0_pins(void) {
  c_gpio__set_as_function(ssp0_mosi,
                          c_gpio__function_2); // Pin 11 of MP3 shield
  c_gpio__set_as_function(ssp0_miso,
                          c_gpio__function_2);           // Pin 12 of MP3 shield
  c_gpio__set_as_function(ssp0_sck, c_gpio__function_2); // Pin 13 of MP3 shield
}

static void sj2__configure_onboard_switches_to_control_mp3_player(void) {
  c_gpio__set_as_input(sw3__prev_song);
  c_gpio__set_as_input(sw2__next_song);
  c_gpio__set_as_input(sw1__vol_low);
  c_gpio__set_as_input(sw0__vol_high);
  c_gpio__enable_pull_down(sw1__vol_low);
  c_gpio__enable_pull_down(sw0__vol_high);
}

static void sj2__enable_gpio_interuppt(void) {
  gpio0__attach_interrupt(29, GPIO_INTR__FALLING_EDGE, sw3__falling_edge_isr);
  gpio0__attach_interrupt(30, GPIO_INTR__FALLING_EDGE, sw2__falling_edge_isr);
}

static void send_32_bytes_to_decoder_using_ssp(const uint32_t start, const uint32_t end, const uint8_t *bytes_to_send) {
  for (uint32_t i = start; i < end; i++) {
    UNUSED mp3__exchange_byte(bytes_to_send[i]);
  }
}

static void task__medium_priority_to_read_mp3_file(void *pvParam) {
  static uint8_t bytes_to_send_to_mp3_decoder[READ_BYTES_FROM_FILE];
  UINT br;
  FIL file;
  FRESULT result;
  mp3__get_file_name(&song__recent_info);
  result = f_open(&file, song__recent_info.name, FA_OPEN_EXISTING | FA_READ);

  while (1) {
    switch (sj2__mode) {
    case GAME_MODE:
      // Wait for semaphore signal from SW3
      // Once semaphore is taken, play the previous track
      if (xSemaphoreTake(sem__sw3, 0)) {
        result = f_close(&file);
        mp3__get_prev_song(&song__recent_info);
        result = f_open(&file, song__recent_info.name, FA_OPEN_EXISTING | FA_READ);
        printf("Prev Song: %s\n", song__recent_info.name);
      }

      // Wait for semaphore signal from SW3
      // Once semaphore is taken, play the next track
      // Or if file reaches EOF; play the next track
      if (xSemaphoreTake(sem__sw2, 0) || (f_eof(&file))) {
        result = f_close(&file);
        mp3__get_next_song(&song__recent_info);
        result = f_open(&file, song__recent_info.name, FA_OPEN_EXISTING | FA_READ);
        printf("Next Song: %s\n", song__recent_info.name);
      }
      break;
    case MP3_MODE:
      if (xSemaphoreTake(sem__prev_song, 0)) {
        result = f_close(&file);
        mp3__get_prev_song(&song__recent_info);
        result = f_open(&file, song__recent_info.name, FA_OPEN_EXISTING | FA_READ);
        printf("Prev Song: %s\n", song__recent_info.name);
      }

      // Wait for semaphore signal from SW3
      // Once semaphore is taken, play the next track
      if (xSemaphoreTake(sem__next_song, 0) || (f_eof(&file))) {
        result = f_close(&file);
        mp3__get_next_song(&song__recent_info);
        result = f_open(&file, song__recent_info.name, FA_OPEN_EXISTING | FA_READ);
        printf("Next Song: %s\n", song__recent_info.name);
      }

      // Wait for semaphore signal from SW3
      // Once semaphore is taken, Increaase the volume
      if (xSemaphoreTake(sem__sw3, 0)) {
        mp3__dec_vol();
      }

      // Wait for semaphore signal from SW2
      // Once semaphore is taken, Increaase the volume
      if (xSemaphoreTake(sem__sw2, 0)) {
        mp3__inc_vol();
      }
      break;
    default:
      break;
    }

    xSemaphoreTake(mp3_mutex, portMAX_DELAY);
    // clang-format off
    result = f_read(&file, &bytes_to_send_to_mp3_decoder[0], READ_BYTES_FROM_FILE, &br);
    if (0 != result) {
      printf("Result of %s is %i\n", &song__recent_info.name[0], result);
    }
    // clang-format on
    xSemaphoreGive(mp3_mutex);
    xQueueSend(mp3_queue, &bytes_to_send_to_mp3_decoder[0], portMAX_DELAY);
  }
}

static void task__high_priority_to_send_mp3_data_to_decoder(void *pvParam) {
  static uint8_t bytes_to_be_sent_to_decoder[READ_BYTES_FROM_FILE];
  static uint8_t current_count = 0;
  uint32_t start = 0, end = 0;
  while (1) {
    if (0 == current_count) {
      xQueueReceive(mp3_queue, &bytes_to_be_sent_to_decoder[0], portMAX_DELAY);
    }
    start = (current_count * TRANSMIT_BYTES_TO_DECODER);
    end = ((current_count + 1) * TRANSMIT_BYTES_TO_DECODER);
    while (!mp3__dreq_get_status()) {
      mp3__decoder_refresh();
      vTaskDelay(2);
    }
    if (xSemaphoreTake(mp3_mutex, portMAX_DELAY)) {
      mp3__data_cs();
      send_32_bytes_to_decoder_using_ssp(start, end, &bytes_to_be_sent_to_decoder[0]);
      mp3__data_ds();
      xSemaphoreGive(mp3_mutex);
      if (current_count == COUNT - 1) {
        current_count = start = end = 0;
      } else {
        current_count += 1;
      }
    }
  }
}

static void get_song_list_from_sd_card(void) {
  FILINFO file;
  DIR dir;
  FRESULT result;
  const char *dirPath = "0:Songs";
  unsigned int fileBytesTotal = 0, numFiles = 0, numDirs = 0;
  result = f_opendir(&dir, dirPath);
  if (FR_OK == result) {
    for (;;) {
      result = f_readdir(&dir, &file);
      if ((FR_OK != result) || !file.fname[0]) {
        break;
      }
      // To skip through hidden files and folders
      if (file.fname[0] == '.') {
        continue;
      }

      /* Check if it is a directory type */
      if (file.fattrib & AM_DIR) {
        printf("Dir: %s\n", &(file.fname[0]));
        numDirs++;
      } else {
        sprintf(songs_list[numFiles], "%s", &(file.fname[0]));
        printf("%s\n", &(file.fname[0]));
        total_number_of_songs = ++numFiles;
        fileBytesTotal += file.fsize;
      }
    }
    printf("Number of Directories: %i\n", numDirs);
    printf("Number of Files: %i\n", numFiles);
    result = f_closedir(&dir);
  } else {
    printf("Error opening directory: Error %i\n", result);
  }
}

static void sw3__falling_edge_isr(void) { xSemaphoreGiveFromISR(sem__sw3, NULL); }

static void sw2__falling_edge_isr(void) { xSemaphoreGiveFromISR(sem__sw2, NULL); }