#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "esp_log.h"
#include "esp_timer.h"
#include "led.h"
#include "nvs_flash.h"

static const char *TAG = "LED";

#define LED_DEFAULT_INTENSITY_PERCENT 100
#define LED_DATA_NAMESPACE "led_data"
// Note: Keys must be under 15 characters, including the null terminator
#define LED_INTENSITY_PERCENT_KEY "intensity_pct"

static esp_err_t led_set_intensity_in_flash(led_t *led,
                                            uint8_t intensity_percent) {
  // Store the new intensity in NVS
  nvs_handle_t my_handle;
  esp_err_t err = nvs_open(LED_DATA_NAMESPACE, NVS_READWRITE, &my_handle);
  if (err == ESP_OK) {
    err = nvs_set_i8(my_handle, LED_INTENSITY_PERCENT_KEY, intensity_percent);
    if (err == ESP_OK) {
      nvs_commit(my_handle);
    } else {
      ESP_LOGE(TAG, "Error writing key (%s)!\n", esp_err_to_name(err));
    }
  } else if (err == ESP_ERR_NVS_NOT_FOUND) {
    ESP_LOGW(TAG, "NVS namespace not found. Attempting to create it.");
    err = nvs_open(LED_DATA_NAMESPACE, NVS_READWRITE, &my_handle);
    if (err == ESP_OK) {
      err = nvs_set_i8(my_handle, LED_INTENSITY_PERCENT_KEY, intensity_percent);
      if (err == ESP_OK) {
        nvs_commit(my_handle);
        ESP_LOGI(TAG,
                 "NVS namespace created and intensity percent set to %" PRIu8
                 "\n",
                 intensity_percent);
      } else {
        ESP_LOGE(TAG, "Error writing key after creating namespace (%s)!\n",
                 esp_err_to_name(err));
      }
    } else {
      ESP_LOGE(TAG, "Error creating NVS namespace (%s)!\n",
               esp_err_to_name(err));
    }
  } else {
    ESP_LOGE(TAG, "Error opening NVS handle (%s)!\n", esp_err_to_name(err));
  }
  nvs_close(my_handle);
  return ESP_OK;
}

static esp_err_t led_get_intensity_from_flash(int8_t *intensity_percent) {
  nvs_handle_t my_handle;
  esp_err_t err = nvs_open(LED_DATA_NAMESPACE, NVS_READONLY, &my_handle);
  if (err == ESP_OK) {
    err = nvs_get_i8(my_handle, LED_INTENSITY_PERCENT_KEY, intensity_percent);
    if (err == ESP_OK) {
      ESP_LOGI(TAG, "Intensity percent read successfully: %" PRIi8 "\n",
               *intensity_percent);
    } else if (err == ESP_ERR_NVS_NOT_FOUND) {
      ESP_LOGW(TAG,
               "Key '" LED_INTENSITY_PERCENT_KEY
               "' was not found in NVS. Using default "
               "value: %" PRIi8 "\n",
               *intensity_percent);
      *intensity_percent = LED_DEFAULT_INTENSITY_PERCENT;
    } else {
      ESP_LOGE(TAG, "Error reading key (%s)!\n", esp_err_to_name(err));
    }
  } else {
    ESP_LOGE(TAG, "Error opening NVS handle (%s)!\n", esp_err_to_name(err));
    *intensity_percent = LED_DEFAULT_INTENSITY_PERCENT;
  }
  nvs_close(my_handle);
  return err;
}

struct led_t {
  gpio_num_t gpio;
  ledc_channel_t channel;
  uint8_t intensity_percent;
  uint8_t pattern[LED_PATTERN_LEN];
  uint64_t pattern_frame_duration_us;
  uint8_t current_step;
  esp_timer_handle_t timer;
  bool is_running;
};

static void led_timer_callback(void *arg) {
  led_t *led = (led_t *)arg;
  if (!led || !led->is_running)
    return;

  uint8_t step_state = led->pattern[led->current_step];

  if (step_state == 1) {
    uint32_t duty =
        (8191 * led->intensity_percent) / 100; // 13-bit depth max = 8191
    ledc_set_duty(LEDC_LOW_SPEED_MODE, led->channel, duty);
  } else {
    ledc_set_duty(LEDC_LOW_SPEED_MODE, led->channel, 0);
  }
  ledc_update_duty(LEDC_LOW_SPEED_MODE, led->channel);

  led->current_step = (led->current_step + 1) % LED_PATTERN_LEN;
}

led_t *led_init(gpio_num_t gpio, ledc_channel_t channel,
                ledc_timer_t timer_num) {
  led_t *led = calloc(1, sizeof(led_t));
  if (!led)
    return NULL;

  int8_t stored_intensity;
  esp_err_t err = led_get_intensity_from_flash(&stored_intensity);
  int8_t intensity_percent;

  if (err == ESP_OK) {
    intensity_percent = stored_intensity;
  } else {
    intensity_percent = LED_DEFAULT_INTENSITY_PERCENT;
    esp_err_t err = led_set_intensity_in_flash(
        led, intensity_percent); // Store the default intensity in NVS
    if (err != ESP_OK) {
      ESP_LOGE(TAG, "Failed to store default intensity in NVS: %s",
               esp_err_to_name(err));
    }
  }

  led->gpio = gpio;
  led->channel = channel;
  led->intensity_percent = intensity_percent;
  led->pattern_frame_duration_us = 250000;
  led->current_step = 0;
  led->is_running = false;

  ledc_timer_config_t timer_cfg = {.speed_mode = LEDC_LOW_SPEED_MODE,
                                   .duty_resolution = LEDC_TIMER_13_BIT,
                                   .timer_num = timer_num,
                                   .freq_hz = 5000,
                                   .clk_cfg = LEDC_AUTO_CLK};
  ESP_ERROR_CHECK(ledc_timer_config(&timer_cfg));

  ledc_channel_config_t channel_cfg = {.gpio_num = gpio,
                                       .speed_mode = LEDC_LOW_SPEED_MODE,
                                       .channel = channel,
                                       .timer_sel = timer_num,
                                       .duty = 0,
                                       .hpoint = 0};
  ESP_ERROR_CHECK(ledc_channel_config(&channel_cfg));

  const esp_timer_create_args_t timer_args = {
      .callback = &led_timer_callback, .arg = led, .name = "led_timer"};
  ESP_ERROR_CHECK(esp_timer_create(&timer_args, &led->timer));

  return led;
}

esp_err_t led_set_intensity(led_t *led, uint8_t duty_percent) {
  if (!led)
    return ESP_ERR_INVALID_ARG;
  if (duty_percent > 100)
    duty_percent = 100;

  led->intensity_percent = duty_percent;

  esp_err_t err = led_set_intensity_in_flash(led, duty_percent);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Failed to store intensity in NVS: %s", esp_err_to_name(err));
  }

  return ESP_OK;
}

esp_err_t led_set_pattern(led_t *led, const uint8_t pattern[LED_PATTERN_LEN]) {
  if (!led || !pattern)
    return ESP_ERR_INVALID_ARG;

  memcpy(led->pattern, pattern, LED_PATTERN_LEN);
  led->current_step = 0;
  return ESP_OK;
}

esp_err_t led_start_pattern(led_t *led) {
  if (!led)
    return ESP_ERR_INVALID_ARG;
  if (led->is_running)
    return ESP_OK;

  led->is_running = true;
  led->current_step = 0;

  return esp_timer_start_periodic(led->timer, led->pattern_frame_duration_us);
}

esp_err_t led_stop_pattern(led_t *led) {
  if (!led)
    return ESP_ERR_INVALID_ARG;
  if (!led->is_running)
    return ESP_OK;

  led->is_running = false;
  esp_timer_stop(led->timer);

  ledc_set_duty(LEDC_LOW_SPEED_MODE, led->channel, 0);
  ledc_update_duty(LEDC_LOW_SPEED_MODE, led->channel);

  return ESP_OK;
}

void led_deinit(led_t *led) {
  if (led) {
    led_stop_pattern(led);
    esp_timer_delete(led->timer);
    free(led);
  }
}
