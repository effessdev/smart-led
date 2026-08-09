#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "led.h"

void app_main(void) {
  // 1. Create an LED instance on GPIO 18
  led_t *status_led = led_init(GPIO_NUM_18, LEDC_CHANNEL_0, LEDC_TIMER_0);

  // 2. Set brightness to 50%
  led_set_intensity(status_led, 50);

  // 3. Define a 12-step pattern
  uint8_t alert_pattern[12] = {1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  led_set_pattern(status_led, alert_pattern);

  // 4. Start execution loop (0.5s per step)
  led_start_pattern(status_led);

  // Let it blink for 10 seconds
  vTaskDelay(pdMS_TO_TICKS(10000));

  // Update brightness dynamically on the fly
  led_set_intensity(status_led, 100);

  while (1) {
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}
