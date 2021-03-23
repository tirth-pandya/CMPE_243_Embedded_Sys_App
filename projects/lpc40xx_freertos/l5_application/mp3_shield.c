#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "task.h"

#include "VS1053B_map.h"
#include "c_gpio.h"
#include "c_ssp2.h"
#include "delay.h"
#include "mp3_shield.h"
#include "ssp0.h"
#include "ssp2.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MP3__WRITE 0x02
#define MP3__READ 0x03

extern uint32_t total_number_of_songs;
extern song__info_s song__recent_info;
extern char songs_list[SONG_CAPACITY][SONG_NAME_LENGTH];

// Private Variables
static const c_gpio_s pin__mp3_dreq = {0, 6};
static const c_gpio_s pin__mp3_cs = {0, 7};
static const c_gpio_s pin__mp3_data_cs = {0, 26};
static const c_gpio_s pin__mp3_reset = {0, 25};

static const uint16_t MP3__MODE = 0x4800;
static const uint16_t MP3__CLOCKF = 0xD800; // 0xFBE8; // 0x8BE8; // 0xBBE8; // was 9800, EBE8, B3E8, BBE8
volatile static int16_t MP3__VOL = 0x0101;  // full vol
static const uint16_t MP3__BASS = 0x00F6;   // was 0076
static const uint16_t MP3__AUDATA = 0xAC80; // for stereo decoding, AC45,AC80, BB80-check

int16_t mp3_get_volume(void) { return MP3__VOL; }

static void mp3__gpio_pins_initialize(void) {
  // Initialize gpio pins for mp3 shield
  c_gpio__set_as_output(pin__mp3_cs);
  c_gpio__set_as_output(pin__mp3_data_cs);
  c_gpio__set_as_output(pin__mp3_reset);
}

static void mp3__set_volume(int8_t left_channel, int8_t right_channel) {
  MP3__VOL = (left_channel << 8) | right_channel;
  (MP3__VOL < 0x0000) ? MP3__VOL = 0x0000 : MP3__VOL;
  (MP3__VOL > 0xFEFE) ? MP3__VOL = 0xFEFE : MP3__VOL;
  printf("Volume: 0x%04X\n", MP3__VOL);
}

uint8_t mp3__exchange_byte(uint8_t byte_to_transmit) { return ssp0__exchange_byte(byte_to_transmit); }

void mp3__get_pins_status(void) {
  printf("Get pin__mp3_cs status: %i\n", c_gpio__get_level(pin__mp3_cs));
  printf("Get pin__mp3_data_cs status: %i\n", c_gpio__get_level(pin__mp3_data_cs));
  printf("Get pin__mp3_reset status: %i\n", c_gpio__get_level(pin__mp3_reset));
  printf("Get pin__mp3_dreq status: %i\n", c_gpio__get_level(pin__mp3_dreq));
  printf("\n");
}

void mp3__cs(void) { c_gpio__set_low(pin__mp3_cs); }

void mp3__ds(void) { c_gpio__set_high(pin__mp3_cs); }

void mp3__data_cs(void) { c_gpio__set_low(pin__mp3_data_cs); }

void mp3__data_ds(void) { c_gpio__set_high(pin__mp3_data_cs); }

void mp3__reset_low(void) { c_gpio__set_low(pin__mp3_reset); }

void mp3__reset_high(void) { c_gpio__set_high(pin__mp3_reset); }

void mp3__reset(void) {
  mp3__reset_low();
  delay__ms(20);
  mp3__reset_high();
  delay__ms(20);
}

void mp3__hardware_reset(void) {
  mp3__ds();
  mp3__data_ds();
  mp3__reset();
}

void mp3__dreq_init(void) { c_gpio__set_as_input(pin__mp3_dreq); }

bool mp3__dreq_get_status(void) { return c_gpio__get_level(pin__mp3_dreq); }

void mp3__write_data(uint8_t address, uint16_t data) {
  mp3__cs(); // Assert chip select line to LOW
  (void)mp3__exchange_byte(MP3__WRITE);
  (void)mp3__exchange_byte(address);
  uint8_t data_lsb = data & 0xff;
  uint8_t data_msb = (data >> 8) & 0xff;
  (void)mp3__exchange_byte(data_msb);
  (void)mp3__exchange_byte(data_lsb);
  mp3__ds(); // Assert chip select line to HIGH
}

uint16_t mp3__read_data(uint8_t address) {
  uint16_t result = 0;
  mp3__cs();
  (void)mp3__exchange_byte(MP3__READ);
  (void)mp3__exchange_byte(address);
  uint8_t result_msb = mp3__exchange_byte(0xFF);
  uint8_t result_lsb = mp3__exchange_byte(0xFF);
  mp3__ds();
  result = (result_msb << 8);
  result |= result_lsb;
  return result;
}

void mp3__decoder_initialize(void) {
  // Initalize all gpio pins to control MP3 shield
  mp3__gpio_pins_initialize();

  // Set the MP3 registers to dfault values
  mp3__decoder_refresh();

  song_status = PLAY;
}

void mp3__decoder_refresh(void) {
  mp3__write_data(SCI__MODE, MP3__MODE);
  mp3__write_data(SCI__CLOCKF, MP3__CLOCKF);
  mp3__write_data(SCI__VOL, MP3__VOL);
  mp3__write_data(SCI__BASS, MP3__BASS);
  mp3__write_data(SCI__AUDATA, MP3__AUDATA);
}

void mp3__decoder_refresh_volume(void) { mp3__write_data(SCI__VOL, MP3__VOL); }

void mp3__inc_vol(void) {
  // Store MP3__VOL value in local variable
  // Maybe its not needed.
  int16_t new_vol = MP3__VOL;

  // Get MSB byte
  int8_t left_channel = (new_vol >> 8) & 0xFF;

  // Get LSB byte
  int8_t right_channel = new_vol & 0xFF;

  // Decrement left and right channel by 5
  // Note: Decrementing value, increases volume
  // Incrementing value, decreases volume
  left_channel -= 5;
  right_channel = left_channel;

  // Call mp3__set_volume function
  mp3__set_volume(left_channel, right_channel);
}

void mp3__dec_vol(void) {
  // Store MP3__VOL value in local variable
  // Maybe its not needed.
  int16_t new_vol = MP3__VOL;

  // Get MSB byte
  int8_t left_channel = (new_vol >> 8) & 0xFF;

  // Get LSB byte
  int8_t right_channel = new_vol & 0xFF;

  // Increment left and right channel by 5
  // Note: Decrementing value, increases volume
  // Incrementing value, decreases volume
  left_channel += 5;
  right_channel = left_channel;

  // Call mp3__set_volume function
  mp3__set_volume(left_channel, right_channel);
}

void mp3__wait_for_dreq_pin_to_get_high(void) {
  // Loop here until DREQ pin is HIGH
  // Pin HIGH indicates MP3 decoder can receive atleast 32bytes of data
  while (!mp3__dreq_get_status()) {
    ;
  }
}

void mp3__get_file_name(song__info_s *song) {
  memset(song->name, 0, strlen(song->name));
  char *dir = "/Songs/";

  strcpy(song->name, dir);
  strncat(song->name, songs_list[song->number], strlen(songs_list[song->number]));
}

void mp3__get_next_song(song__info_s *song) {
  song->number++;
  if (song->number >= total_number_of_songs) {
    song->number = 0;
  }
  mp3__get_file_name(song);
}
void mp3__get_prev_song(song__info_s *song) {
  song->number--;
  if (song->number < 0) {
    song->number = total_number_of_songs - 1;
  }
  mp3__get_file_name(song);
}