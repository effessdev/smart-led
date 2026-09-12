
#include "esp_log.h"
#include "nvs_flash.h"

static const char *TAG = "LED_STORAGE";

#define LED_DATA_NAMESPACE "led_data"
// Note: Keys must be under 15 characters, including the null terminator
#define LED_INTENSITY_PERCENT_KEY "intensity_pct"

esp_err_t led_set_intensity_in_flash(uint8_t intensity_percent) {
  nvs_handle_t my_handle;

  esp_err_t err = nvs_open(LED_DATA_NAMESPACE, NVS_READWRITE, &my_handle);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Error opening NVS handle (%s)", esp_err_to_name(err));
    return err;
  }

  err = nvs_set_u8(my_handle, LED_INTENSITY_PERCENT_KEY, intensity_percent);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Error writing key (%s)", esp_err_to_name(err));
    nvs_close(my_handle);
    return err;
  }

  err = nvs_commit(my_handle);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Error committing NVS (%s)", esp_err_to_name(err));
  } else {
    ESP_LOGI(TAG, "LED intensity set to %" PRIu8 "%% in flash",
             intensity_percent);
  }

  nvs_close(my_handle);
  return err;
}

esp_err_t led_get_intensity_from_flash(uint8_t *intensity_percent) {
  nvs_handle_t my_handle;
  esp_err_t err = nvs_open(LED_DATA_NAMESPACE, NVS_READONLY, &my_handle);

  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Error opening NVS handle (%s): ", esp_err_to_name(err));
    return err;
  }

  err = nvs_get_u8(my_handle, LED_INTENSITY_PERCENT_KEY, intensity_percent);

  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Error reading key (%s): ", esp_err_to_name(err));
  }

  nvs_close(my_handle);
  return err;
}