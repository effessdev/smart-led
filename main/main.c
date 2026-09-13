#include "ble_server.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h" // IWYU pragma: keep
#include "freertos/queue.h"
#include "freertos/task.h"
#include "led.h"
#include "nvs_flash.h"

static led_t *led_obj = NULL;
static QueueHandle_t app_queue = NULL;

// The Consumer Task: waits for commands and executes them
static void app_manager_task(void *arg) {
  app_cmd_t cmd;

  while (1) {
    // Blocks here until a message arrives in the queue
    if (xQueueReceive(app_queue, &cmd, portMAX_DELAY) == pdTRUE) {
      switch (cmd.type) {
      case CMD_SET_INTENSITY:
        if (led_obj != NULL) {
          ESP_LOGI("APP_MGR", "Applying new intensity: %d%%",
                   cmd.payload.intensity);
          led_set_intensity(led_obj, cmd.payload.intensity);
        }
        break;

      case CMD_SET_PATTERN:
        if (led_obj != NULL) {
          ESP_LOGI("APP_MGR", "Applying new pattern");
          led_set_pattern(led_obj, cmd.payload.pattern);
        }
        break;
      }
    }
  }
}

void app_main(void) {
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
      ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);

  led_obj = led_init(GPIO_NUM_18, LEDC_CHANNEL_0, LEDC_TIMER_0);

  uint8_t blink_pattern[12] = {1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0};
  led_set_pattern(led_obj, blink_pattern);
  led_start_pattern(led_obj);

  // 1. Create a queue capable of holding 10 commands
  app_queue = xQueueCreate(10, sizeof(app_cmd_t));

  // 2. Start the application manager task (2048 bytes stack, priority 5)
  xTaskCreate(app_manager_task, "app_manager", 2048, NULL, 5, NULL);

  // 3. Pass the queue to BLE so it can push messages to us
  ble_server_init(app_queue);

  // The main task can just clean up and delete itself now,
  // or you can use it for something else.
  vTaskDelete(NULL);
}
