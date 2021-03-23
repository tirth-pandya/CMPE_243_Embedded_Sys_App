#include "game_fruit_fury.h"
#include "c_gpio.h"

static const c_gpio_s sw1 = {1, 15};
static const c_gpio_s sw0 = {1, 19};

/* Draws the final game frame */
void draw_final_game_frame() {
  for (uint8_t k = 0; k < ((level == 3) ? 50 : 60); k++) { // Speed up fruit occurrence on reaching level 3
    for (uint8_t row = 0; row < (MAX_ROW / 2); row++) {
      disableOE();
      set_row(row);
      for (uint8_t col = 0; col < MAX_COL; col++) {
        LPC_GPIO0->PIN |= (1 << CLK);
        set_color_bottom(game_matrix[row + (MAX_ROW / 2)][col]);
        set_color_top(game_matrix[row][col]);
        LPC_GPIO0->PIN &= ~(1 << CLK);
      }
      LPC_GPIO0->PIN |= (1 << LAT);
      LPC_GPIO0->PIN &= ~(1 << LAT);
      enableOE();
      vTaskDelay(1);
    }
  }
}

void draw_game_over_screen() {
  for (uint8_t row = 0; row < (MAX_ROW / 2); row++) {
    disableOE();
    set_row(row);
    for (uint8_t col = 0; col < MAX_COL; col++) {
      LPC_GPIO0->PIN |= (1 << CLK);
      if (score >= 50) {
        draw_title_screen_border(game_win_matrix, (row * 2), col, CYAN, YELLOW);
        set_color_bottom(game_win_matrix[row + (MAX_ROW / 2)][col]);
        set_color_top(game_win_matrix[row][col]);
      } else {
        draw_title_screen_border(game_over_screen, (row * 2), col, PURPLE, YELLOW);
        set_color_bottom(game_over_screen[row + (MAX_ROW / 2)][col]);
        set_color_top(game_over_screen[row][col]);
      }
      LPC_GPIO0->PIN &= ~(1 << CLK);
    }
    LPC_GPIO0->PIN |= (1 << LAT);
    LPC_GPIO0->PIN &= ~(1 << LAT);
    enableOE();
    vTaskDelay(1);
  }
}

void draw_title_screen_border(uint8_t matrix_type[][MAX_COL], uint8_t row, uint8_t col, uint8_t color_h,
                              uint8_t color_v) {
  if (row == 0)
    matrix_type[row][col] = color_h;
  // if (row != 0 && (col == 0 || col == 63))
  //  matrix_type[row][col] = color_v;
  if (row == 30)
    matrix_type[row + 1][col] = color_h;
}

void draw_game_screen_border(uint8_t matrix_type[][MAX_COL], uint8_t row, uint8_t col, uint8_t color_h,
                             uint8_t color_v) {
  if (row == 15)
    matrix_type[row * 2][col] = color_h;

  if ((row % 4 == 1) && (col == 0 || col == 63)) {
    matrix_type[row][col] = color_v;
    matrix_type[row + 1][col] = color_v;
    matrix_type[row + (MAX_ROW / 2)][col] = color_v;
    matrix_type[(row + 1) + (MAX_ROW / 2)][col] = color_v;
  }

  if (row == 1)
    matrix_type[row][col] = color_h;
}

void draw_last_game_frame() {
  for (uint8_t row = 0; row < (MAX_ROW / 2); row++) {
    disableOE();
    set_row(row);
    for (uint8_t col = 0; col < MAX_COL; col++) {
      LPC_GPIO0->PIN |= (1 << CLK);
      set_color_bottom(game_matrix[row + (MAX_ROW / 2)][col]);
      set_color_top(game_matrix[row][col]);
      LPC_GPIO0->PIN &= ~(1 << CLK);
    }
    LPC_GPIO0->PIN |= (1 << LAT);
    LPC_GPIO0->PIN &= ~(1 << LAT);
    enableOE();
    vTaskDelay(1);
  }
}

/* Draws the game's initial screen with borders and title */
void draw_game_title_screen() {
  draw_banana(game_init_screen, 2, 2);
  draw_star(game_init_screen, 12, 2);
  draw_star(game_init_screen, 12, 55);
  draw_cherry(game_init_screen, 2, 55);
  draw_pear(game_init_screen, 22, 57);
  draw_apple(game_init_screen, 23, 2);
  for (uint8_t row = 0; row < (MAX_ROW / 2); row++) {
    disableOE();
    set_row(row);
    for (uint8_t col = 0; col < MAX_COL; col++) {
      draw_title_screen_border(game_init_screen, (row * 2), col, PURPLE, RED);
      LPC_GPIO0->PIN |= (1 << CLK);
      set_color_bottom(game_init_screen[row + (MAX_ROW / 2)][col]);
      set_color_top(game_init_screen[row][col]);
      LPC_GPIO0->PIN &= ~(1 << CLK);
    }
    LPC_GPIO0->PIN |= (1 << LAT);
    LPC_GPIO0->PIN &= ~(1 << LAT);
    enableOE();
    vTaskDelay(1);
  }
}

/* Only for score board - 5x5 pixels bounded by: *
 * BOX 1 - (2,27), (2,31), (6,27), (6,31)        *
 * BOX 2 - (2,33), (2,37), (6,33), (6,37)        */
void draw_h_line(uint8_t matrix_type[][MAX_COL], uint8_t row, uint8_t col_units, uint8_t box_num, uint8_t color) {
  uint8_t col, col_min = 0;
  if (box_num == 0) // Tens digit
    col_min = 27;
  else if (box_num == 1) // Units digit
    col_min = 33;
  else if (box_num == 2) // For level
    col_min = 10;

  for (col = col_min; col < (col_min + col_units); col++) {
    matrix_type[row][col] = color;
  }
}

/* Only for score board - 5x5 pixels bounded by: *
 * BOX 1 - (2,27), (2,31), (6,27), (6,31)        *
 * BOX 2 - (2,33), (2,37), (6,33), (6,37)        */
void draw_v_line(uint8_t matrix_type[][MAX_COL], uint8_t col, uint8_t row_units, uint8_t color) {
  uint8_t row_min = 1; // Since score board is always drawn from row 1
  for (uint8_t row = row_min; row < (row_min + row_units); row++) {
    matrix_type[row][col] = color;
  }
}

void draw_pixel(uint8_t matrix_type[][MAX_COL], uint8_t x, uint8_t y, uint8_t color) { matrix_type[x][y] = color; }

void draw_life_counter(uint8_t matrix_type[][MAX_COL]) {
  uint8_t life_counter_begin_row = 2;
  if (life_counter == 0) {
    draw_pixel(matrix_type, life_counter_begin_row, 59, BLACK);
    draw_pixel(matrix_type, life_counter_begin_row + 1, 58, BLACK);
    draw_pixel(matrix_type, life_counter_begin_row + 1, 59, BLACK);
    draw_pixel(matrix_type, life_counter_begin_row + 1, 60, BLACK);
    draw_pixel(matrix_type, life_counter_begin_row + 2, 58, BLACK);
    draw_pixel(matrix_type, life_counter_begin_row + 2, 59, BLACK);
    draw_pixel(matrix_type, life_counter_begin_row + 2, 60, BLACK);
    draw_pixel(matrix_type, life_counter_begin_row + 3, 59, BLACK);
  } else {
    // To ensure gaining of extra lives don't increase overall lives above 5
    life_counter = (life_counter >= 5) ? 5 : life_counter;
    for (uint8_t i = 0; i < life_counter; i++) {
      draw_pixel(matrix_type, life_counter_begin_row, 59 - i * 4, GREEN);
      draw_pixel(matrix_type, life_counter_begin_row + 1, 58 - i * 4, RED);
      draw_pixel(matrix_type, life_counter_begin_row + 1, 59 - i * 4, GREEN);
      draw_pixel(matrix_type, life_counter_begin_row + 1, 60 - i * 4, RED);
      draw_pixel(matrix_type, life_counter_begin_row + 2, 58 - i * 4, RED);
      draw_pixel(matrix_type, life_counter_begin_row + 2, 59 - i * 4, RED);
      draw_pixel(matrix_type, life_counter_begin_row + 2, 60 - i * 4, RED);
      draw_pixel(matrix_type, life_counter_begin_row + 3, 59 - i * 4, RED);
    }
  }
}

void draw_level(uint8_t matrix_type[][MAX_COL], uint8_t color) {
  draw_pixel(matrix_type, 2, 2, color);
  draw_pixel(matrix_type, 3, 2, color);
  draw_pixel(matrix_type, 4, 2, color);
  draw_pixel(matrix_type, 5, 2, color);
  draw_pixel(matrix_type, 6, 2, color);
  draw_pixel(matrix_type, 6, 3, color);
  draw_pixel(matrix_type, 6, 4, color);
  draw_pixel(matrix_type, 6, 5, color);

  draw_pixel(matrix_type, 4, 6, color);
  draw_pixel(matrix_type, 5, 6, color);
  draw_pixel(matrix_type, 6, 7, color);
  draw_pixel(matrix_type, 5, 8, color);
  draw_pixel(matrix_type, 4, 8, color);

  switch (level) {
  case 1:
    draw_pixel(matrix_type, 2, 11, color);
    draw_pixel(matrix_type, 3, 11, color);
    draw_pixel(matrix_type, 4, 11, color);
    draw_pixel(matrix_type, 5, 11, color);
    draw_pixel(matrix_type, 6, 11, color);
    break;

  case 2:
    draw_h_line(matrix_type, 2, 4, 2, color);
    draw_pixel(matrix_type, 3, 13, color);
    draw_h_line(matrix_type, 4, 4, 2, color);
    draw_pixel(matrix_type, 5, 10, color);
    draw_h_line(matrix_type, 6, 4, 2, color);
    break;

  case 3:
    draw_h_line(matrix_type, 2, 4, 2, color);
    draw_pixel(matrix_type, 3, 13, color);
    draw_h_line(matrix_type, 4, 4, 2, color);
    draw_pixel(matrix_type, 5, 13, color);
    draw_h_line(matrix_type, 6, 4, 2, color);
    break;
  }
}

/* Box0 - tens digit, Box1 - units digit */
void draw_score(uint8_t matrix_type[][MAX_COL], uint8_t color) {
  uint8_t score_arr[2] = {0}, i, j = 0, score_begin_row = 2;

  score_arr[0] = score / 10; // Tens digit
  score_arr[1] = score % 10; // Ones digit

  for (i = 0; i < 2; i++) {
    switch (score_arr[i]) // Reading tens digit initially and then units digit
    {
    case 0:
      draw_h_line(matrix_type, score_begin_row, 5, i, color);
      draw_pixel(matrix_type, score_begin_row + 1, 27 + j, color);
      draw_pixel(matrix_type, score_begin_row + 1, 31 + j, color);
      draw_pixel(matrix_type, score_begin_row + 2, 27 + j, color);
      draw_pixel(matrix_type, score_begin_row + 2, 31 + j, color);
      draw_pixel(matrix_type, score_begin_row + 3, 27 + j, color);
      draw_pixel(matrix_type, score_begin_row + 3, 31 + j, color);
      draw_h_line(matrix_type, score_begin_row + 4, 5, i, color);
      break;

    case 1:
      draw_pixel(matrix_type, score_begin_row, 31 + j, color);
      draw_pixel(matrix_type, score_begin_row + 1, 31 + j, color);
      draw_pixel(matrix_type, score_begin_row + 2, 31 + j, color);
      draw_pixel(matrix_type, score_begin_row + 3, 31 + j, color);
      draw_pixel(matrix_type, score_begin_row + 4, 31 + j, color);
      break;

    case 2:
      draw_h_line(matrix_type, score_begin_row, 5, i, color);
      draw_pixel(matrix_type, score_begin_row + 1, 31 + j, color);
      draw_h_line(matrix_type, score_begin_row + 2, 5, i, color);
      draw_pixel(matrix_type, score_begin_row + 3, 27 + j, color);
      draw_h_line(matrix_type, score_begin_row + 4, 5, i, color);
      break;

    case 3:
      draw_h_line(matrix_type, score_begin_row, 5, i, color);
      draw_pixel(matrix_type, score_begin_row + 1, 31 + j, color);
      draw_h_line(matrix_type, score_begin_row + 2, 5, i, color);
      draw_pixel(matrix_type, score_begin_row + 3, 31 + j, color);
      draw_h_line(matrix_type, score_begin_row + 4, 5, i, color);
      break;

    case 4:
      draw_pixel(matrix_type, score_begin_row, 27 + j, color);
      draw_pixel(matrix_type, score_begin_row, 31 + j, color);
      draw_pixel(matrix_type, score_begin_row + 1, 27 + j, color);
      draw_pixel(matrix_type, score_begin_row + 1, 31 + j, color);
      draw_h_line(matrix_type, score_begin_row + 2, 5, i, color);
      draw_pixel(matrix_type, score_begin_row + 3, 31 + j, color);
      draw_pixel(matrix_type, score_begin_row + 4, 31 + j, color);
      break;

    case 5:
      draw_h_line(matrix_type, score_begin_row, 5, i, color);
      draw_pixel(matrix_type, score_begin_row + 1, 27 + j, color);
      draw_h_line(matrix_type, score_begin_row + 2, 5, i, color);
      draw_pixel(matrix_type, score_begin_row + 3, 31 + j, color);
      draw_h_line(matrix_type, score_begin_row + 4, 5, i, color);
      break;

    case 6:
      draw_h_line(matrix_type, score_begin_row, 5, i, color);
      draw_pixel(matrix_type, score_begin_row + 1, 27 + j, color);
      draw_h_line(matrix_type, score_begin_row + 2, 5, i, color);
      draw_pixel(matrix_type, score_begin_row + 3, 27 + j, color);
      draw_pixel(matrix_type, score_begin_row + 3, 31 + j, color);
      draw_h_line(matrix_type, score_begin_row + 4, 5, i, color);
      break;

    case 7:
      draw_h_line(matrix_type, score_begin_row, 5, i, color);
      draw_pixel(matrix_type, score_begin_row + 1, 31 + j, color);
      draw_pixel(matrix_type, score_begin_row + 2, 31 + j, color);
      draw_pixel(matrix_type, score_begin_row + 3, 31 + j, color);
      draw_pixel(matrix_type, score_begin_row + 4, 31 + j, color);
      break;

    case 8:
      draw_h_line(matrix_type, score_begin_row, 5, i, color);
      draw_pixel(matrix_type, score_begin_row + 1, 27 + j, color);
      draw_pixel(matrix_type, score_begin_row + 1, 31 + j, color);
      draw_h_line(matrix_type, score_begin_row + 2, 5, i, color);
      draw_pixel(matrix_type, score_begin_row + 3, 27 + j, color);
      draw_pixel(matrix_type, score_begin_row + 3, 31 + j, color);
      draw_h_line(matrix_type, score_begin_row + 4, 5, i, color);
      break;

    case 9:
      draw_h_line(matrix_type, score_begin_row, 5, i, color);
      draw_pixel(matrix_type, score_begin_row + 1, 27 + j, color);
      draw_pixel(matrix_type, score_begin_row + 1, 31 + j, color);
      draw_h_line(matrix_type, score_begin_row + 2, 5, i, color);
      draw_pixel(matrix_type, score_begin_row + 3, 31 + j, color);
      draw_h_line(matrix_type, score_begin_row + 4, 5, i, color);
      break;

    default:
      draw_h_line(matrix_type, score_begin_row, 5, i, color);
      draw_pixel(matrix_type, score_begin_row + 1, 27 + j, color);
      draw_pixel(matrix_type, score_begin_row + 1, 31 + j, color);
      draw_pixel(matrix_type, score_begin_row + 2, 27 + j, color);
      draw_pixel(matrix_type, score_begin_row + 2, 31 + j, color);
      draw_pixel(matrix_type, score_begin_row + 3, 27 + j, color);
      draw_pixel(matrix_type, score_begin_row + 3, 31 + j, color);
      draw_h_line(matrix_type, score_begin_row + 4, 5, i, color);
    }
    j += 6; // Shifting 6 pixels to the right for drawing units digit in box 1
            // (units digit)
  }
}

void draw_pear(uint8_t matrix_type[][MAX_COL], uint8_t row_pt, uint8_t col_pt) {
  matrix_type[row_pt][col_pt + 3] = GREEN;
  matrix_type[row_pt + 1][col_pt + 2] = GREEN;
  matrix_type[row_pt + 2][col_pt + 1] = YELLOW;
  matrix_type[row_pt + 2][col_pt + 2] = YELLOW;
  matrix_type[row_pt + 2][col_pt + 3] = YELLOW;
  matrix_type[row_pt + 3][col_pt + 1] = YELLOW;
  matrix_type[row_pt + 3][col_pt + 2] = YELLOW;
  matrix_type[row_pt + 3][col_pt + 3] = YELLOW;
  matrix_type[row_pt + 4][col_pt] = YELLOW;
  matrix_type[row_pt + 4][col_pt + 1] = YELLOW;
  matrix_type[row_pt + 4][col_pt + 2] = YELLOW;
  matrix_type[row_pt + 4][col_pt + 3] = YELLOW;
  matrix_type[row_pt + 4][col_pt + 4] = YELLOW;
  matrix_type[row_pt + 5][col_pt] = YELLOW;
  matrix_type[row_pt + 5][col_pt + 1] = YELLOW;
  matrix_type[row_pt + 5][col_pt + 2] = YELLOW;
  matrix_type[row_pt + 5][col_pt + 3] = YELLOW;
  matrix_type[row_pt + 5][col_pt + 4] = YELLOW;
  matrix_type[row_pt + 6][col_pt + 1] = YELLOW;
  matrix_type[row_pt + 6][col_pt + 2] = YELLOW;
  matrix_type[row_pt + 6][col_pt + 3] = YELLOW;
}

void draw_cherry(uint8_t matrix_type[][MAX_COL], uint8_t row_pt, uint8_t col_pt) {
  matrix_type[row_pt][col_pt + 3] = GREEN;
  matrix_type[row_pt + 1][col_pt + 2] = GREEN;
  matrix_type[row_pt + 1][col_pt + 4] = GREEN;
  matrix_type[row_pt + 2][col_pt + 1] = GREEN;
  matrix_type[row_pt + 2][col_pt + 5] = GREEN;
  matrix_type[row_pt + 3][col_pt + 1] = GREEN;
  matrix_type[row_pt + 3][col_pt + 4] = RED;
  matrix_type[row_pt + 3][col_pt + 5] = RED;
  matrix_type[row_pt + 3][col_pt + 6] = RED;
  matrix_type[row_pt + 4][col_pt] = RED;
  matrix_type[row_pt + 4][col_pt + 1] = RED;
  matrix_type[row_pt + 4][col_pt + 2] = RED;
  matrix_type[row_pt + 4][col_pt + 4] = RED;
  matrix_type[row_pt + 4][col_pt + 5] = RED;
  matrix_type[row_pt + 4][col_pt + 6] = RED;
  matrix_type[row_pt + 5][col_pt] = RED;
  matrix_type[row_pt + 5][col_pt + 1] = RED;
  matrix_type[row_pt + 5][col_pt + 2] = RED;
  matrix_type[row_pt + 5][col_pt + 4] = RED;
  matrix_type[row_pt + 5][col_pt + 5] = RED;
  matrix_type[row_pt + 5][col_pt + 6] = RED;
  matrix_type[row_pt + 6][col_pt] = RED;
  matrix_type[row_pt + 6][col_pt + 1] = RED;
  matrix_type[row_pt + 6][col_pt + 2] = RED;
}

void draw_apple(uint8_t matrix_type[][MAX_COL], uint8_t row_pt, uint8_t col_pt) {
  matrix_type[row_pt][col_pt + 2] = GREEN;
  matrix_type[row_pt + 1][col_pt + 1] = GREEN;
  matrix_type[row_pt + 2][col_pt] = RED;
  matrix_type[row_pt + 2][col_pt + 1] = RED;
  matrix_type[row_pt + 2][col_pt + 2] = RED;
  matrix_type[row_pt + 3][col_pt] = RED;
  matrix_type[row_pt + 3][col_pt + 1] = RED;
  matrix_type[row_pt + 3][col_pt + 2] = RED;
  matrix_type[row_pt + 4][col_pt] = RED;
  matrix_type[row_pt + 4][col_pt + 1] = RED;
  matrix_type[row_pt + 4][col_pt + 2] = RED;
  matrix_type[row_pt + 5][col_pt + 1] = RED;
}

void draw_grapes(uint8_t matrix_type[][MAX_COL], uint8_t row_pt, uint8_t col_pt) {
  matrix_type[row_pt][col_pt + 3] = YELLOW;
  matrix_type[row_pt + 1][col_pt + 2] = YELLOW;
  matrix_type[row_pt + 2][col_pt] = GREEN;
  matrix_type[row_pt + 2][col_pt + 1] = GREEN;
  matrix_type[row_pt + 2][col_pt + 2] = GREEN;
  matrix_type[row_pt + 2][col_pt + 3] = GREEN;
  matrix_type[row_pt + 2][col_pt + 4] = GREEN;
  matrix_type[row_pt + 3][col_pt + 1] = GREEN;
  matrix_type[row_pt + 3][col_pt + 2] = GREEN;
  matrix_type[row_pt + 3][col_pt + 3] = GREEN;
  matrix_type[row_pt + 4][col_pt + 2] = GREEN;
}

void draw_banana(uint8_t matrix_type[][MAX_COL], uint8_t row_pt, uint8_t col_pt) {
  matrix_type[row_pt][col_pt] = GREEN;
  matrix_type[row_pt][col_pt + 1] = GREEN;
  matrix_type[row_pt + 1][col_pt] = GREEN;
  matrix_type[row_pt + 1][col_pt + 1] = GREEN;
  matrix_type[row_pt + 2][col_pt] = YELLOW;
  matrix_type[row_pt + 2][col_pt + 1] = YELLOW;
  matrix_type[row_pt + 3][col_pt] = YELLOW;
  matrix_type[row_pt + 3][col_pt + 1] = YELLOW;
  matrix_type[row_pt + 3][col_pt + 2] = YELLOW;
  matrix_type[row_pt + 4][col_pt] = YELLOW;
  matrix_type[row_pt + 4][col_pt + 1] = YELLOW;
  matrix_type[row_pt + 4][col_pt + 2] = YELLOW;
  matrix_type[row_pt + 4][col_pt + 3] = YELLOW;
  matrix_type[row_pt + 4][col_pt + 4] = YELLOW;
  matrix_type[row_pt + 4][col_pt + 5] = YELLOW;
  matrix_type[row_pt + 5][col_pt + 1] = YELLOW;
  matrix_type[row_pt + 5][col_pt + 2] = YELLOW;
  matrix_type[row_pt + 5][col_pt + 3] = YELLOW;
  matrix_type[row_pt + 5][col_pt + 4] = YELLOW;
  matrix_type[row_pt + 5][col_pt + 5] = YELLOW;
  matrix_type[row_pt + 6][col_pt + 2] = YELLOW;
  matrix_type[row_pt + 6][col_pt + 3] = YELLOW;
  matrix_type[row_pt + 6][col_pt + 4] = YELLOW;
}

void draw_star(uint8_t matrix_type[][MAX_COL], uint8_t row_pt, uint8_t col_pt) {
  matrix_type[row_pt][col_pt + 3] = YELLOW;
  matrix_type[row_pt + 1][col_pt + 3] = YELLOW;
  matrix_type[row_pt + 2][col_pt + 2] = RED;
  matrix_type[row_pt + 2][col_pt + 3] = GREEN;
  matrix_type[row_pt + 2][col_pt + 4] = BLUE;
  matrix_type[row_pt + 3][col_pt] = CYAN;
  matrix_type[row_pt + 3][col_pt + 1] = RED;
  matrix_type[row_pt + 3][col_pt + 2] = PURPLE;
  matrix_type[row_pt + 3][col_pt + 3] = RED;
  matrix_type[row_pt + 3][col_pt + 4] = CYAN;
  matrix_type[row_pt + 3][col_pt + 5] = RED;
  matrix_type[row_pt + 3][col_pt + 6] = GREEN;
  matrix_type[row_pt + 4][col_pt + 1] = PURPLE;
  matrix_type[row_pt + 4][col_pt + 2] = BLUE;
  matrix_type[row_pt + 4][col_pt + 3] = CYAN;
  matrix_type[row_pt + 4][col_pt + 4] = WHITE;
  matrix_type[row_pt + 4][col_pt + 5] = YELLOW;
  matrix_type[row_pt + 5][col_pt + 2] = GREEN;
  matrix_type[row_pt + 5][col_pt + 4] = YELLOW;
  matrix_type[row_pt + 6][col_pt + 1] = YELLOW;
  matrix_type[row_pt + 6][col_pt + 5] = RED;
}

void draw_bomb(uint8_t matrix_type[][MAX_COL], uint8_t row_pt, uint8_t col_pt) {
  matrix_type[row_pt][col_pt + 2] = YELLOW;
  matrix_type[row_pt + 1][col_pt + 2] = YELLOW;
  matrix_type[row_pt + 2][col_pt + 1] = RED;
  matrix_type[row_pt + 2][col_pt + 2] = RED;
  matrix_type[row_pt + 2][col_pt + 3] = RED;
  matrix_type[row_pt + 3][col_pt + 1] = RED;
  // matrix_type[row_pt + 3][col_pt + 2] = RED;
  matrix_type[row_pt + 3][col_pt + 3] = RED;
  matrix_type[row_pt + 4][col_pt] = RED;
  // matrix_type[row_pt + 4][col_pt + 1] = RED;
  // matrix_type[row_pt + 4][col_pt + 2] = RED;
  // matrix_type[row_pt + 4][col_pt + 3] = RED;
  matrix_type[row_pt + 4][col_pt + 4] = RED;
  matrix_type[row_pt + 5][col_pt] = RED;
  // matrix_type[row_pt + 5][col_pt + 1] = RED;
  // matrix_type[row_pt + 5][col_pt + 2] = RED;
  // matrix_type[row_pt + 5][col_pt + 3] = RED;
  matrix_type[row_pt + 5][col_pt + 4] = RED;
  matrix_type[row_pt + 6][col_pt] = RED;
  matrix_type[row_pt + 6][col_pt + 1] = RED;
  matrix_type[row_pt + 6][col_pt + 2] = RED;
  matrix_type[row_pt + 6][col_pt + 3] = RED;
  matrix_type[row_pt + 6][col_pt + 4] = RED;
}

void draw_fruit(uint8_t matrix_type[][MAX_COL], uint8_t row_pt, uint8_t col_pt) {
  fruit_type = rand() % 6;

  if (level >= 2) // Draw bomb only at level 2 and above
    fruit_type = rand() % 8;

  switch (fruit_type) {
  case 0:
    draw_star(matrix_type, row_pt, col_pt);
    break;
  case 1:
    draw_banana(matrix_type, row_pt, col_pt);
    break;
  case 2:
    draw_cherry(matrix_type, row_pt, col_pt);
    break;
  case 3:
    draw_pear(matrix_type, row_pt, col_pt);
    break;
  case 4:
    draw_grapes(matrix_type, row_pt, col_pt);
    break;
  case 5:
    draw_apple(matrix_type, row_pt, col_pt);
    break;
  case 6:
    draw_bomb(matrix_type, row_pt, col_pt);
    break;
  case 7:
    draw_bomb(matrix_type, row_pt, col_pt);
    break;
  default:
    draw_banana(matrix_type, row_pt, col_pt);
  }
}

/* Draws the final game frame */
void draw_effects_frame() {
  for (uint8_t row = 0; row < (MAX_ROW / 2); row++) {
    disableOE();
    set_row(row);
    for (uint8_t col = 0; col < MAX_COL; col++) {
      LPC_GPIO0->PIN |= (1 << CLK);
      set_color_bottom(game_matrix[row + (MAX_ROW / 2)][col]);
      set_color_top(game_matrix[row][col]);
      LPC_GPIO0->PIN &= ~(1 << CLK);
    }
    LPC_GPIO0->PIN |= (1 << LAT);
    LPC_GPIO0->PIN &= ~(1 << LAT);
    enableOE();
    vTaskDelay(1);
  }
}

void clear_screen() {
  uint8_t row, col;
  for (row = 0; row < (MAX_ROW / 2); row++) {
    disableOE();
    set_row(row);
    for (col = 0; col < MAX_COL; col++) {
      set_color_top(0);
      set_color_bottom(0);
      LPC_GPIO0->PIN |= (1 << CLK);
      LPC_GPIO0->PIN &= ~(1 << CLK);
    }
    LPC_GPIO0->PIN |= (1 << LAT);
    LPC_GPIO0->PIN &= ~(1 << LAT);
    enableOE();
  }
}

/* Get the board direction from the user and increment score count */
void compute_game_params(uint8_t row_pt, uint8_t col_pt, uint8_t quadrant) {
  acceleration__axis_data_s acc_s = acceleration__get_data();
  fruit_hit_success = 0;

#if GAME_DEBUG_ENABLE
  // printf("\n%s: Time (ticks): %lu | Actual r = %d c = %d acc value = X = %d |
  // Y = %d | Z = %d\n", xTaskGetTickCount(), row_pt, col_pt, acc_s.x, acc_s.y,
  // acc_s.z);
#endif
  if ((acc_s.x >= 3400 && acc_s.x <= 4200) && (acc_s.y >= 90 && acc_s.y <= 900) &&
      (acc_s.z >= 400 && acc_s.z <= 1200)) {
    if (quadrant == 1) {
      fruit_hit_success = 1;
    } else {
      life_counter--;
      streak = 0;
    }
#if GAME_DEBUG_ENABLE
    // printf("\nScore = %d Time = %lu North East\n", score,
    // xTaskGetTickCount());
#endif
  } else if ((acc_s.x >= 3300 && acc_s.x <= 4000) && (acc_s.y >= 3400 && acc_s.y <= 4100) &&
             (acc_s.z >= 600 && acc_s.z <= 1050)) {
    if (quadrant == 2) {
      fruit_hit_success = 1;
    } else {
      life_counter--;
      streak = 0;
    }
#if GAME_DEBUG_ENABLE
    // printf("\nScore = %d Time = %lu North West\n", score,
    // xTaskGetTickCount());
#endif
  } else if ((acc_s.x >= 200 && acc_s.x <= 650) && (acc_s.y >= 3200 && acc_s.y <= 3800) &&
             (acc_s.z >= 400 && acc_s.z <= 900)) {
    if (quadrant == 3) {
      fruit_hit_success = 1;
    } else {
      life_counter--;
      streak = 0;
    }
#if GAME_DEBUG_ENABLE
    // printf("\nScore = %d Time = %lu South West\n", score,
    // xTaskGetTickCount());
#endif
  } else if ((acc_s.x >= 50 && acc_s.x <= 750) && (acc_s.y >= 180 && acc_s.y <= 850) &&
             (acc_s.z >= 450 && acc_s.z <= 1000)) {
    if (quadrant == 4) {
      fruit_hit_success = 1;
    } else {
      life_counter--;
      streak = 0;
    }
#if GAME_DEBUG_ENABLE
    // printf("\nScore = %d Time = %lu South East\n", score,
    // xTaskGetTickCount());
#endif
  }

  if (fruit_hit_success) {
    if (fruit_type == 6 || fruit_type == 7) // Decrement life if player hits a bomb and reset streak counter
    {
      life_counter--;
      streak = 0;
    } else {
      score++;
      streak++;            // Increment stream for successive fruit hit
      if (fruit_type == 0) // Increment score another time if you hit star fruit
        score++;
      if (streak % 10 == 0) // Player gets +1 extra life for a streak of 10 fruit hits
      {
        streak = 0;
        life_counter++;
      }
    }
  }

  // Increment level upon reaching following scores
  if (score <= 15)
    level = 1;
  else if (score >= 16 && score <= 29)
    level = 2;
  else if (score >= 30)
    level = 3;

  if (fruit_hit_success) {
    row_fruit_pt = row_pt;
    col_fruit_pt = col_pt;
    display_slice_effects();
  }

  if (life_counter == 0 || score >= 50) {
    is_game_over = true;
    construct_game_matrix(row_pt, col_pt);
    draw_final_game_frame();
    vTaskResume(display_game_init_screen_t);
  }
}

/* Clear the fruit when displaying the slice effects */
void clear_fruit() {
  for (int i = 0; i < 7; i++)
    for (int j = 0; j < 7; j++)
      game_matrix[row_fruit_pt + i][col_fruit_pt + j] = BLACK;
}

void display_slice_effects() {
  uint8_t color = WHITE, color2 = PURPLE;
  if (fruit_hit_success) {
    fruit_hit_success = 0;
    if (fruit_type == 6 || fruit_type == 7) // Display different color for bomb
    {
      color = RED;
      color2 = RED;
    }
    clear_fruit();
    game_matrix[row_fruit_pt + 1][col_fruit_pt + 2] = color;
    draw_effects_frame();
    game_matrix[row_fruit_pt + 2][col_fruit_pt + 1] = color;
    draw_effects_frame();
    game_matrix[row_fruit_pt + 2][col_fruit_pt + 2] = color;
    draw_effects_frame();
    game_matrix[row_fruit_pt + 2][col_fruit_pt + 3] = color;
    draw_effects_frame();
    game_matrix[row_fruit_pt + 3][col_fruit_pt + 2] = color;
    draw_effects_frame();
    game_matrix[row_fruit_pt + 1][col_fruit_pt + 2] = BLACK;
    draw_effects_frame();
    game_matrix[row_fruit_pt + 2][col_fruit_pt + 1] = BLACK;
    draw_effects_frame();
    game_matrix[row_fruit_pt + 2][col_fruit_pt + 2] = BLACK;
    draw_effects_frame();
    game_matrix[row_fruit_pt + 2][col_fruit_pt + 3] = BLACK;
    draw_effects_frame();
    game_matrix[row_fruit_pt + 3][col_fruit_pt + 2] = BLACK;
    draw_effects_frame();
    game_matrix[row_fruit_pt + 1][col_fruit_pt + 1] = color;
    draw_effects_frame();
    game_matrix[row_fruit_pt][col_fruit_pt + 1] = color;
    draw_effects_frame();
    game_matrix[row_fruit_pt][col_fruit_pt + 3] = color;
    draw_effects_frame();
    game_matrix[row_fruit_pt + 1][col_fruit_pt] = color;
    draw_effects_frame();
    game_matrix[row_fruit_pt + 1][col_fruit_pt + 1] = color2;
    draw_effects_frame();
    game_matrix[row_fruit_pt + 1][col_fruit_pt + 3] = color2;
    draw_effects_frame();
    game_matrix[row_fruit_pt + 1][col_fruit_pt + 4] = color2;
    draw_effects_frame();
    game_matrix[row_fruit_pt + 3][col_fruit_pt] = color2;
    draw_effects_frame();
    game_matrix[row_fruit_pt + 3][col_fruit_pt + 1] = color;
    draw_effects_frame();
    game_matrix[row_fruit_pt + 3][col_fruit_pt + 3] = color;
    draw_effects_frame();
    game_matrix[row_fruit_pt + 3][col_fruit_pt + 4] = color;
    draw_effects_frame();
    game_matrix[row_fruit_pt + 4][col_fruit_pt + 1] = color;
    draw_effects_frame();
    game_matrix[row_fruit_pt + 4][col_fruit_pt + 3] = color;
    draw_effects_frame();
    game_matrix[row_fruit_pt + 1][col_fruit_pt + 1] = BLACK;
    draw_effects_frame();
    game_matrix[row_fruit_pt][col_fruit_pt + 1] = BLACK;
    draw_effects_frame();
    game_matrix[row_fruit_pt][col_fruit_pt + 3] = BLACK;
    draw_effects_frame();
    game_matrix[row_fruit_pt + 1][col_fruit_pt] = BLACK;
    draw_effects_frame();
    game_matrix[row_fruit_pt + 1][col_fruit_pt + 1] = BLACK;
    draw_effects_frame();
    game_matrix[row_fruit_pt + 1][col_fruit_pt + 3] = BLACK;
    draw_effects_frame();
    game_matrix[row_fruit_pt + 1][col_fruit_pt + 4] = BLACK;
    draw_effects_frame();
    game_matrix[row_fruit_pt + 3][col_fruit_pt] = BLACK;
    draw_effects_frame();
    game_matrix[row_fruit_pt + 3][col_fruit_pt + 1] = BLACK;
    draw_effects_frame();
    game_matrix[row_fruit_pt + 3][col_fruit_pt + 3] = BLACK;
    draw_effects_frame();
    game_matrix[row_fruit_pt + 3][col_fruit_pt + 4] = BLACK;
    draw_effects_frame();
    game_matrix[row_fruit_pt + 4][col_fruit_pt + 1] = BLACK;
    draw_effects_frame();
    game_matrix[row_fruit_pt + 4][col_fruit_pt + 3] = BLACK;
    draw_effects_frame();
  }
}

/* Constructs the frame matrix of the game */
void construct_game_matrix(uint8_t row_pt, uint8_t col_pt) {
  uint8_t row, col;
  memset(game_matrix, 0, sizeof(game_matrix));

  for (row = 0; row < MAX_ROW; row++) {
    for (col = 0; col < MAX_COL; col++) {
      draw_game_screen_border(game_matrix, row / 2, col, CYAN, YELLOW);
      if (row == row_pt && col == col_pt) // Initial coordinate points of the fruit
      {
        draw_fruit(game_matrix, row_pt, col_pt);
      }
    }
  }
  draw_score(game_matrix, WHITE);
  draw_level(game_matrix, WHITE);
  draw_life_counter(game_matrix);
}

/* Generate fruit's initial coordinate position */
void get_fruit_begin_coord(uint8_t *row_pt, uint8_t *col_pt, uint8_t *quadrant) {
  *quadrant = (rand() % 4) + 1; // 1 to 4

  /* Get initial coordinate to draw the fruit.       *
   * Offsets are given to reduce fruits' boundaries. */
  switch (*quadrant) {
  case 1:
    *row_pt = (rand() % 4) + 6;   // 6 to 9
    *col_pt = (rand() % 16) + 40; // 40 to 55
    break;
  case 2:
    *row_pt = (rand() % 3) + 7;  // 7 to 9
    *col_pt = (rand() % 15) + 3; // 3 to 17
    break;
  case 3:
    *row_pt = (rand() % 5) + 19; // 19 to 23
    *col_pt = (rand() % 15) + 3; // 3 to 17
    break;
  case 4:
    *row_pt = (rand() % 5) + 19;  // 19 to 23
    *col_pt = (rand() % 16) + 40; // 40 to 55
    break;
  default:
    *row_pt = 0;
    *col_pt = 0;
  }
}

void start_game() {
  uint8_t row_pt = -1, col_pt = -1, quadrant;
  while (1) {
    /* Push button to exit game */
    if (LPC_GPIO1->PIN & (1 << SW0))
      is_start_game = false;

    if (is_start_game == true) {
      /* Generate fruit's initial coordinate position */
      get_fruit_begin_coord(&row_pt, &col_pt, &quadrant);
      /* Generate final frame by considering fruit object at that position */
      construct_game_matrix(row_pt, col_pt);
      /* Draw final frame matrix of the game */
      draw_final_game_frame();
      /* Get the board direction from the user and increment score count */
      compute_game_params(row_pt, col_pt, quadrant);
    } else
      vTaskResume(display_game_init_screen_t);
  }
}

void display_game_init_screen() {
  while (1) {
    /* Push button to begin new game */
    if (LPC_GPIO1->PIN & (1 << SW1)) {
      is_game_over = false;
      is_start_game = true;
    }

    if (is_game_over == true) {
      vTaskSuspend(start_game_t);
      draw_game_over_screen();
    } else {
      if (is_start_game == true) {
        score = 0;
        life_counter = 5;
        streak = 0;
        vTaskResume(start_game_t);
        vTaskSuspend(display_game_init_screen_t);
      } else {
        draw_game_title_screen();
        vTaskSuspend(start_game_t);
      }
    }
  }
}

void game_entry_point() {
  c_gpio__enable_pull_down(sw0);
  c_gpio__enable_pull_down(sw1);
  led_init();
  is_start_game = false;
  is_game_over = false;
  xTaskCreate(display_game_init_screen, "display_game_init_screen", 2048, NULL, 3, &display_game_init_screen_t);
  xTaskCreate(start_game, "start_game", 2048, NULL, 3, &start_game_t);
}
