#pragma once

#include "FreeRTOS.h"
#include "acceleration.h"
#include "board_io.h"
#include "clock.h"
#include "delay.h"
#include "lpc40xx.h"
#include "lpc_peripherals.h"
#include "math.h"
#include "queue.h"
#include "semphr.h"
#include "sys_time.h"
#include "task.h"
#include "time.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum MATRIX_RGB_PIN { B1 = 1, G1 = 2, R2 = 4, G2 = 5, B2 = 6, A = 7, B = 8, C = 9, R1 = 16 };

enum MATRIX_CONTROL_PIN { LAT = 8, CLK = 9, D = 22, OE = 31 };

enum SWITCH_PIN {
  SW3 = 29,
  SW2 = 30,
  SW1 = 15,
  SW0 = 19,
};

/**************
4 2 1
B G R VAL COLOR
0 0 0 0x0 OFF
0 0 1 0x1 RED
0 1 0 0x2 GREEN
0 1 1 0x3 YELLOW
1 0 0 0x4 BLUE
1 0 1 0x5 PURPLE
1 1 0 0x6 CYAN
1 1 1 0x7 WHITE
*****************/

enum COLOR_CODES {
  BLACK = 0x0,
  RED = 0x1,
  GREEN = 0x2,
  YELLOW = 0x3,
  BLUE = 0x4,
  PURPLE = 0x5,
  CYAN = 0x6,
  WHITE = 0x7
};

// Active low
void disableOE(void);

void enableOE(void);

void clock_reset(void);

void latch_reset(void);

void led_init(void);

void set_color_top(uint8_t color);

void set_color_bottom(uint8_t color);

void set_row(uint8_t row);
