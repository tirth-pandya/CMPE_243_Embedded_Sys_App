#include "game_fruit_fury_led_driver.h"

void disableOE(void) { LPC_GPIO1->SET = (1 << OE); }

void enableOE(void) { LPC_GPIO1->CLR = (1 << OE); }

void clock_reset(void) {
  LPC_GPIO0->SET = (1 << CLK);
  LPC_GPIO0->CLR = (1 << CLK);
}

void latch_reset(void) {
  LPC_GPIO0->PIN |= (1 << LAT);
  LPC_GPIO0->PIN &= ~(1 << LAT);
}

void led_init(void) {
  LPC_GPIO0->DIR |= (1 << LAT);  // P0.8 -> LAT
  LPC_GPIO1->DIR |= (1 << OE);   // P1.31 -> OE
  LPC_GPIO0->DIR |= (1 << R1);   // P0.16 -> R1
  LPC_GPIO2->DIR |= (1 << G1);   // P2.1 -> G1
  LPC_GPIO2->DIR |= (1 << B1);   // P2.2 -> B1
  LPC_GPIO2->DIR |= (1 << R2);   // P2.4 -> R2
  LPC_GPIO2->DIR |= (1 << G2);   // P2.5 -> G2
  LPC_GPIO2->DIR |= (1 << B2);   // P2.6 -> B2
  LPC_GPIO0->DIR |= (1 << CLK);  // P0.9 -> CLK
  LPC_GPIO2->DIR |= (1 << A);    // P2.7 -> A
  LPC_GPIO2->DIR |= (1 << B);    // P2.8 -> B
  LPC_GPIO2->DIR |= (1 << C);    // P2.9 -> C
  LPC_GPIO0->DIR |= (1 << D);    // P0.22 -> D
  LPC_GPIO0->DIR &= ~(1 << SW3); // SW 0.29
  LPC_GPIO0->DIR &= ~(1 << SW2); // SW 0.30
}

void set_color_top(uint8_t color) {
  uint8_t r1_bit = color & 1;
  uint8_t g1_bit = (color >> 1) & 1;
  uint8_t b1_bit = (color >> 2) & 1;
  r1_bit ? (LPC_GPIO0->SET = (1 << R1)) : (LPC_GPIO0->CLR = (1 << R1));
  g1_bit ? (LPC_GPIO2->SET = (1 << G1)) : (LPC_GPIO2->CLR = (1 << G1));
  b1_bit ? (LPC_GPIO2->SET = (1 << B1)) : (LPC_GPIO2->CLR = (1 << B1));
}

void set_color_bottom(uint8_t color) {
  uint8_t r2_bit = color & 1;
  uint8_t g2_bit = (color >> 1) & 1;
  uint8_t b2_bit = (color >> 2) & 1;
  r2_bit ? (LPC_GPIO2->SET = (1 << R2)) : (LPC_GPIO2->CLR = (1 << R2));
  g2_bit ? (LPC_GPIO2->SET = (1 << G2)) : (LPC_GPIO2->CLR = (1 << G2));
  b2_bit ? (LPC_GPIO2->SET = (1 << B2)) : (LPC_GPIO2->CLR = (1 << B2));
}

void set_row(uint8_t row) {
  uint8_t a_bit = (row >> 0) & 1;
  uint8_t b_bit = (row >> 1) & 1;
  uint8_t c_bit = (row >> 2) & 1;
  uint8_t d_bit = (row >> 3) & 1;
  a_bit ? (LPC_GPIO2->SET = (1 << A)) : (LPC_GPIO2->CLR = (1 << A));
  b_bit ? (LPC_GPIO2->SET = (1 << B)) : (LPC_GPIO2->CLR = (1 << B));
  c_bit ? (LPC_GPIO2->SET = (1 << C)) : (LPC_GPIO2->CLR = (1 << C));
  d_bit ? (LPC_GPIO0->SET = (1 << D)) : (LPC_GPIO0->CLR = (1 << D));
}