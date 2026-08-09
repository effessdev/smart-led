#include <stdlib.h>
#include <string.h>

#include "esp_log.h"
#include "esp_timer.h"
#include "led.h"

static const char *TAG = "LED";

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

  led->gpio = gpio;
  led->channel = channel;
  led->intensity_percent = 100; // Default brightness
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
