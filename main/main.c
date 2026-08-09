#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "led.h"

void app_main(void) {
  led_t *led_obj = led_init(GPIO_NUM_18, LEDC_CHANNEL_0, LEDC_TIMER_0);

  uint8_t blink_pattern[12] = {1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0};

  led_set_pattern(led_obj, blink_pattern);

  led_start_pattern(led_obj);

  while (1) {
    led_set_intensity(led_obj, 100);
    vTaskDelay(pdMS_TO_TICKS(10000));
    led_set_intensity(led_obj, 25);
    vTaskDelay(pdMS_TO_TICKS(10000));
  }
}
