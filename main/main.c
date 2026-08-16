#include "ble_server.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "led/led.h"
#include "nvs_flash.h"

static led_t *led_obj = NULL;

// Callback matching the ble_intensity_cb_t signature
static void on_intensity_change(uint8_t intensity) {
  if (led_obj != NULL) {
    led_set_intensity(led_obj, intensity);
  }
}

void app_main(void) {
  // Initialize NVS (Non-Volatile Storage)
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
      ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);

  led_obj = led_init(GPIO_NUM_18, LEDC_CHANNEL_0, LEDC_TIMER_0);

  uint8_t blink_pattern[12] = {1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0};

  led_set_pattern(led_obj, blink_pattern);

  led_start_pattern(led_obj);

  ble_server_init(on_intensity_change);

  while (1) {
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}
