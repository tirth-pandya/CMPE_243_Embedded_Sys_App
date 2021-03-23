#include "graphics_equalizer.h"
#include "adc.h"
#include "c_gpio.h"
#include "delay.h"
#include "sys_time.h"

static const c_gpio_s pin__graphics_equalizer_strobe = {2, 0U};
static const c_gpio_s pin__graphics_equalizer_reset = {0, 11U};
static const adc_channel_e adc_channel = ADC__CHANNEL_4;
static uint64_t time_point_stamp, time_point_now;

static void graphics_equalizer__gpio_pins_initialize(void) {
  c_gpio__set_as_output(pin__graphics_equalizer_reset);
  c_gpio__set_as_output(pin__graphics_equalizer_strobe);
  adc__enable_analog_mode_for_channel(adc_channel);
  adc__initialize();
  // adc__enable_burst_mode();
}

void graphics_equalizer__init(void) {
  graphics_equalizer__gpio_pins_initialize();
  graphics_equalizer__reset_module();
}

void graphics_equalizer__reset_high(void) { c_gpio__set_high(pin__graphics_equalizer_reset); }

void graphics_equalizer__reset_low(void) { c_gpio__set_low(pin__graphics_equalizer_reset); }

void graphics_equalizer__strobe_high(void) { c_gpio__set_high(pin__graphics_equalizer_strobe); }

void graphics_equalizer__strobe_low(void) { c_gpio__set_low(pin__graphics_equalizer_strobe); }

static uint16_t graphics_equalizer__get_adc_reading(void) {
  // return adc__get_channel_reading_with_burst_mode(adc_channel);
  return adc__get_adc_value(adc_channel);
}

void graphics_equalizer__read_frequency(uint16_t *value) {
  static bool reset_state = false;
  if (false == reset_state) {
    time_point_stamp = sys_time__get_uptime_ms();
    reset_state = true;
  } else {
    time_point_now = sys_time__get_uptime_ms();
    if (time_point_now - time_point_stamp > 3000) {
      graphics_equalizer__reset_module();
      reset_state = false;
    }
  }
  for (int band = 0; band < 7; band++) {
    // delay__us(10);
    // value[band] = graphics_equalizer__get_adc_reading();
    // delay__us(50);
    // graphics_equalizer__strobe_high();
    // delay__us(30);
    // graphics_equalizer__strobe_low();
    graphics_equalizer__strobe_low();
    delay__us(40);
    value[band] = graphics_equalizer__get_adc_reading();
    graphics_equalizer__strobe_high();
  }
}

void graphics_equalizer__reset_module(void) {
  // graphics_equalizer__strobe_low();
  graphics_equalizer__reset_high();
  delay__us(1);
  // graphics_equalizer__strobe_high();
  // graphics_equalizer__strobe_low();
  graphics_equalizer__reset_low();
  delay__us(72);
  graphics_equalizer__strobe_high();
  delay__us(72);
}