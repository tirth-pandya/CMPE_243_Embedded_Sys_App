#ifndef __LAB_UART_RX_TX_H__
#define __LAB_UART_RX_TX_H__

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "lab_uart.h"

#define UART_NUMBER uart__3
#define ACCEL_DATA_SIZE 5

typedef enum Ble_commands {
  GAMING = 0xA1,
  MUSIC = 0xA2,
  SONG_LIST = 0xA3,
  NEXT_SONG = 0xC2,
  TOGGLE_SONG = 0xC1,
  PREV_SONG = 0xC0,
  ACK = 0x00,
  NACK = 0xFF,
  VOL_INC = 0xB1,
  VOL_DEC = 0xB0
};

typedef struct {
  char accel_data[ACCEL_DATA_SIZE];
} app_data_s;

void uart_pin_config_as_io(uart_number_e uart);
// void send_song_on_bluetooth(void *p);
void get_app_data(void *p);

#endif //__LAB_UART_RX_TX_H__