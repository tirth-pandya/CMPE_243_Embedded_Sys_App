// #include "sensor.h"
// #include "FreeRTOS.h"
// #include "gpio.h"
// #include "i2c.h"
// #include "queue.h"
// #include "semphr.h"
// #include "task.h"

// #include "mp3_shield.h"

// gpio_s onboardLed0 = {GPIO__PORT_2, LED0};
// gpio_s onboardLed1 = {GPIO__PORT_1, LED1};
// gpio_s onboardLed2 = {GPIO__PORT_1, LED2};
// gpio_s onboardLed3 = {GPIO__PORT_1, LED3};

// SemaphoreHandle_t sem__next_song;
// SemaphoreHandle_t sem__prev_song;

// void reset_leds(void) {
//   gpio__set(onboardLed0);
//   gpio__set(onboardLed1);
//   gpio__set(onboardLed2);
//   gpio__set(onboardLed3);
// }

// void perform_action_on_orientation(void *p) {
//   orientation_e value;
//   while (1) {
//     value = GetOrientation();
//     switch (value) {
//     case Portrait_UP: // Prev Song
//       printf("Direction Left\n");
//       xSemaphoreGive(sem__prev_song);
//       break;
//     case Portrait_DOWN: // Next Song
//       printf("Direction Right\n");
//       xSemaphoreGive(sem__next_song);
//       break;
//     case Landscape_LEFT: // Resume/Play/
//       printf("Direction Up\n");
//       vTaskSuspend(handle__play_song);
//       vTaskSuspend(handle__freq_bands);
//       song_status = PLAY;
//       break;
//     case Landscape_RIGHT: // Pause/Stop/Volume Decrease
//       printf("Direction Down\n");
//       vTaskResume(handle__play_song);
//       vTaskResume(handle__freq_bands);
//       song_status = PAUSE;
//       break;
//     default:
//       break;
//     }
//     vTaskDelay(50);
//   }
// }