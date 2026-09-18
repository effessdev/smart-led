#pragma once

#include "esp_err.h"
#include "freertos/FreeRTOS.h" // IWYU pragma: keep
#include "freertos/queue.h"
#include "led.h"
#include <stdint.h>

#define OTA_URL_MAX_LEN 256

typedef enum {
  CMD_SET_INTENSITY,
  CMD_SET_PATTERN,
  CMD_TRIGGER_OTA,
} app_cmd_type_t;

typedef struct {
  app_cmd_type_t type;
  union {
    uint8_t intensity;
    uint8_t pattern[LED_PATTERN_LEN];
    char ota_url[OTA_URL_MAX_LEN];
  } payload;
} app_cmd_t;

esp_err_t ble_server_init(QueueHandle_t cmd_queue);
