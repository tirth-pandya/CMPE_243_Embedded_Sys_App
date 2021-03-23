#pragma once

#include <stdbool.h>
#include <stdint.h>

#define SONG_CAPACITY 64
#define SONG_NAME_LENGTH 64

typedef struct {
  int16_t number;
  char name[SONG_NAME_LENGTH];
} song__info_s;

typedef enum {
  GAME_MODE = 1,
  MP3_MODE,
} sj2__mode_e;

sj2__mode_e sj2__mode;

char song_status;
enum SONG_STATUS { PLAY = 1, PAUSE = 2 };

extern TaskHandle_t handle__play_song;
extern TaskHandle_t handle__freq_bands;

void mp3__cs(void);

void mp3__ds(void);

void mp3__data_cs(void);

void mp3__data_ds(void);

void mp3__reset(void);

bool mp3__dreq_get_status(void);

void mp3__write_data(uint8_t address, uint16_t data);

uint16_t mp3__read_data(uint8_t address);

void mp3__decoder_initialize(void);

void mp3__decoder_refresh(void);

void mp3__decoder_refresh_volume(void);

bool mp3__start(void);

bool mp3__stop(void);

void mp3__inc_vol(void);

void mp3__dec_vol(void);

void mp3__get_pins_status(void);

void mp3__dreq_init(void);

void mp3__hardware_reset(void);

uint8_t mp3__exchange_byte(uint8_t byte_to_transmit);

void mp3__wait_for_dreq_pin_to_get_high(void);

void mp3__get_file_name(song__info_s *song);

void mp3__get_next_song(song__info_s *song);

void mp3__get_prev_song(song__info_s *song);

int16_t mp3_get_volume(void);