#include <stdint.h>

#include "adc.h"

#include "c_gpio.h"
#include "clock.h"
#include "lpc40xx.h"
#include "lpc_peripherals.h"

#define ADC_DONE 31U

static const c_gpio_s pin__adc_2 = {0U, 25U};
static const c_gpio_s pin__adc_4 = {1U, 30U};
static const c_gpio_s pin__adc_5 = {1U, 31U};

/*******************************************************************************
 *
 *                      P U B L I C    F U N C T I O N S
 *
 ******************************************************************************/

void adc__initialize(void) {
  lpc_peripheral__turn_on_power_to(LPC_PERIPHERAL__ADC);

  const uint32_t enable_adc_mask = (1 << 21);
  LPC_ADC->CR = enable_adc_mask;

  const uint32_t max_adc_clock = (12 * 1000UL * 1000UL); // 12.4Mhz : max ADC clock in datasheet for lpc40xx
  const uint32_t adc_clock = clock__get_peripheral_clock_hz();

  // APB clock divicer to support max ADC clock
  for (uint32_t divider = 2; divider < 255; divider += 2) {
    if ((adc_clock / divider) < max_adc_clock) {
      LPC_ADC->CR |= (divider << 8);
      break;
    }
  }
}

void adc__enable_analog_mode_for_channel(adc_channel_e channel_num) {
  switch (channel_num) {
  case ADC__CHANNEL_2:
    // Set P0[25] as the ADC channel 2 with function 001
    c_gpio__set_as_function(pin__adc_2, c_gpio__function_1);

    // Enable Analog mode for pin P0[25]
    c_gpio__set_as_analog(pin__adc_2);
    break;
  case ADC__CHANNEL_4:
    // Set P1[30] as the ADC channel 4 with function 011
    c_gpio__set_as_function(pin__adc_4, c_gpio__function_3);

    // Enable Analog mode for pin P1[30]
    c_gpio__set_as_analog(pin__adc_4);
    break;
  case ADC__CHANNEL_5:
    // Set P1[31] as the ADC channel 5 with function 011
    c_gpio__set_as_function(pin__adc_5, c_gpio__function_3);

    // Enable Analog mode for pin P1[31]
    c_gpio__set_as_analog(pin__adc_5);
    break;
  default:
    break;
  }
}

uint16_t adc__get_adc_value(adc_channel_e channel_num) {
  uint16_t result = 0;
  const uint16_t twelve_bits = 0x0FFF;
  const uint32_t channel_masks = 0xFF;
  const uint32_t start_conversion = (1 << 24);
  const uint32_t start_conversion_mask = (7 << 24); // 3bits - B26:B25:B24
  const uint32_t adc_conversion_complete = (1 << 31);

  if ((ADC__CHANNEL_2 == channel_num) || (ADC__CHANNEL_4 == channel_num) || (ADC__CHANNEL_5 == channel_num)) {
    LPC_ADC->CR &= ~(channel_masks | start_conversion_mask);
    // Set the channel number and start the conversion now
    LPC_ADC->CR |= (1 << channel_num) | start_conversion;

    while (!(LPC_ADC->GDR & adc_conversion_complete)) { // Wait till conversion is complete
      ;
    }
    result = (LPC_ADC->GDR >> 4) & twelve_bits; // 12bits - B15:B4
  }

  return result;
}

/**
 * @brief Enable the burst mode by setting a correct bit in
 * the CR register and by clearing the START bits to 0.
 * Note: Burst mode is enabled for all channels by setting
 * CR[7:0] to 11111111. You can enable a respective channel
 * for burst mode by setting respective bits from CR[7:0]
 */
void adc__enable_burst_mode(void) {
  const uint32_t enable_burst_mode = (1 << 16);
  const uint32_t adc_channel_masks = 0xFF;
  const uint32_t start_conversion_mask = (7 << 24); // 3bits - B26:B25:B24

  // Clear the adc channels CR[7:0] -> 00000000
  LPC_ADC->CR &= ~adc_channel_masks;

  // Set all the channels CR[7:0] -> 11111111 for adc conversion and enable the
  // burst mode CR[16]
  LPC_ADC->CR |= (adc_channel_masks | enable_burst_mode);

  // Make the START bits to 000 i.e CR[26:24] after enabling the BURST mode
  LPC_ADC->CR &= ~(start_conversion_mask);
}

uint16_t adc__get_channel_reading_with_burst_mode(adc_channel_e channel_num) {
  uint16_t result = 0;
  const uint32_t read_twelve_bits = 0x0FFF;

  if ((ADC__CHANNEL_2 == channel_num) || (ADC__CHANNEL_4 == channel_num) || (ADC__CHANNEL_5 == channel_num)) {
    // Wait for the conversion to complete on the adc channel by checking the
    // bit 31
    while (((LPC_ADC->DR[channel_num] >> ADC_DONE) & 1) == 0) {
      ;
    }

    // Read the adc conversion from respective channel data register i.e
    // DR[15:4]
    result = (LPC_ADC->DR[channel_num] >> 4) & read_twelve_bits;
  }
  return result;
}