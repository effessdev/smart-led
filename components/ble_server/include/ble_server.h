#pragma once

#include "esp_err.h"
#include "freertos/FreeRTOS.h" // IWYU pragma: keep
#include "freertos/queue.h"
#include "led.h"
#include <stdint.h>

typedef enum {
  CMD_SET_INTENSITY,
  CMD_SET_PATTERN,
} app_cmd_type_t;

typedef struct {
  app_cmd_type_t type;
  union {
    uint8_t intensity;
    uint8_t pattern[LED_PATTERN_LEN];
  } payload;
} app_cmd_t;

esp_err_t ble_server_init(QueueHandle_t cmd_queue);
