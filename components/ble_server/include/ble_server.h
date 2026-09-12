#pragma once

#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include <stdint.h>

typedef enum {
  CMD_SET_INTENSITY,
  // (CMD_SET_PATTERN will go here later)
} app_cmd_type_t;

typedef struct {
  app_cmd_type_t type;
  union {
    uint8_t intensity;
  } payload;
} app_cmd_t;

esp_err_t ble_server_init(QueueHandle_t cmd_queue);
