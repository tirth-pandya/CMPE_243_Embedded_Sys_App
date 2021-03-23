#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"
#include <string.h>

#include "ble.h"
#include "lab_uart.h"
#include "mp3_shield.h"

extern char songs_list[SONG_CAPACITY][SONG_NAME_LENGTH];
extern uint32_t total_number_of_songs;
extern song__info_s song__recent_info;
extern SemaphoreHandle_t sem__next_song;
extern SemaphoreHandle_t sem__prev_song;

void uart_pin_config_as_io(uart_number_e uart) {
  if (uart == uart__3) {
    LPC_IOCON->P4_28 &= ~((1 << 2) | (1 << 0)); // tx
    LPC_IOCON->P4_28 |= (1 << 1);

    LPC_IOCON->P4_29 &= ~((1 << 2) | (1 << 0));
    LPC_IOCON->P4_29 |= (1 << 1);
  } else if (uart == uart__2) {
    LPC_IOCON->P2_8 &= ~((1 << 2) | (1 << 0)); // tx
    LPC_IOCON->P2_8 |= (1 << 1);

    LPC_IOCON->P2_9 &= ~((1 << 2) | (1 << 0));
    LPC_IOCON->P2_9 |= (1 << 1);
  }
}

void send_song_on_bluetooth(void) {
  for (int song_index = 0; song_index < total_number_of_songs; song_index++) {
    for (int j = 0; j < strlen(songs_list[song_index]); j++) {
      uart_lab__polled_put(UART_NUMBER, songs_list[song_index][j]);
      // printf("%c", songs_list[song_index][j]);
    }
    uart_lab__polled_put(UART_NUMBER, ',');
    vTaskDelay(2);
  }
}
void send_ack(char ack_data) {
  char str[7];
  sprintf(str, "%x,%x,", ack_data, ACK);
  for (int i = 0; i < strlen(str); i++)
    uart_lab__polled_put(UART_NUMBER, str[i]);
}

void packet_terminator(void) {
  uart_lab__polled_put(UART_NUMBER, '#');
  uart_lab__polled_put(UART_NUMBER, '#');
}

void send_recent_song(void) {
  // a2$ack/nack$volumeInfo$currentSong$isPlaying/isPaused$##
  int get_current_volume = mp3_get_volume();
  char str[100];
  sprintf(str, "%x,%s,%x,", get_current_volume, song__recent_info.name, song_status);
  for (int i = 0; i < strlen(str); i++) {
    uart_lab__polled_put(UART_NUMBER, str[i]);
  }
}

void get_app_data(void *p) {
  int counter = 0;
  app_data_s app_s;
  while (true) {
    char byte = 0;
    if (uart_lab__get_char_from_queue(&byte, portMAX_DELAY)) {
      printf("Received: 0x%2x\n", byte);

      switch (byte) {
      case GAMING:
        send_ack(GAMING);
        sj2__mode = GAME_MODE;
        break;
      case MUSIC:
        send_ack(MUSIC);
        sj2__mode = MP3_MODE;
        send_recent_song();
        packet_terminator();
        break;
      case SONG_LIST:
        send_ack(SONG_LIST);
        send_song_on_bluetooth();
        packet_terminator();
        break;
      case NEXT_SONG:
        xSemaphoreGive(sem__prev_song);
        vTaskDelay(2000);
        send_ack(MUSIC);
        send_recent_song();
        packet_terminator();
        break;
      case PREV_SONG:
        xSemaphoreGive(sem__next_song);
        vTaskDelay(2000);
        send_ack(MUSIC);
        send_recent_song();
        packet_terminator();
        break;

      case VOL_INC:
        mp3__inc_vol();
        send_ack(MUSIC);
        send_recent_song();
        packet_terminator();
        break;

      case VOL_DEC:
        mp3__dec_vol();
        send_ack(MUSIC);
        send_recent_song();
        packet_terminator();
        break;

      case TOGGLE_SONG:
        if (song_status == PLAY) {
          song_status = PAUSE;
          vTaskSuspend(handle__play_song);
          if (handle__freq_bands != NULL)
            vTaskSuspend(handle__freq_bands);
        } else if (song_status == PAUSE) {
          song_status = PLAY;
          vTaskResume(handle__play_song);
          if (handle__freq_bands != NULL)
            vTaskResume(handle__freq_bands);
        }
        send_ack(MUSIC);
        send_recent_song();
        packet_terminator();
        break;
      default:
        if ('\0' == byte) {
          app_s.accel_data[counter] = '\0';
          counter = 0;
          printf("Received this number from the other board: %s\n", app_s.accel_data);
        } else {
          if (counter < ACCEL_DATA_SIZE) {
            app_s.accel_data[counter++] = byte;
          }
        }
        break;
      }
    }
  }
}