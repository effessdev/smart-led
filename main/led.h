#ifndef LED_H
#define LED_H

#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_err.h"
#include "stdbool.h"
#include "stdint.h"

#define LED_PATTERN_LEN 12

typedef struct led_t led_t;

led_t *led_init(gpio_num_t gpio, ledc_channel_t channel,
                ledc_timer_t timer_num);

esp_err_t led_set_intensity(led_t *led, uint8_t duty_percent);

esp_err_t led_set_pattern(led_t *led, const uint8_t pattern[LED_PATTERN_LEN]);

esp_err_t led_start_pattern(led_t *led);

esp_err_t led_stop_pattern(led_t *led);

void led_deinit(led_t *led);

#endif // LED_H
