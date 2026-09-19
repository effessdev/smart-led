#include "wifi_manager.h"
#include "esp_err.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "freertos/event_groups.h"
#include "lwip/ip4_addr.h"
#include "string.h"

static const char *TAG = "WIFI_MANAGER";
static EventGroupHandle_t s_wifi_event_group;
static int s_retry_num = 0;

/* Bits for event group */
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_MAX_RETRY 5

static void wifi_event_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data) {
  if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
    esp_wifi_connect();
  } else if (event_base == WIFI_EVENT &&
             event_id == WIFI_EVENT_STA_DISCONNECTED) {
    if (s_retry_num < WIFI_MAX_RETRY) {
      esp_wifi_connect();
      s_retry_num++;
      ESP_LOGI(TAG, "Retrying to connect to the AP (%d/%d)", s_retry_num,
               WIFI_MAX_RETRY);
    } else {
      xEventGroupClearBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
      ESP_LOGW(TAG, "Failed to connect to AP after %d attempts",
               WIFI_MAX_RETRY);
    }
  } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
    ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
    ESP_LOGI(TAG, "Got IP Address: %s",
             ip4addr_ntoa((const ip4_addr_t *)&event->ip_info.ip));
    s_retry_num = 0;
    xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
  }
}

void wifi_manager_init_sta(void) {
#ifdef CONFIG_WIFI_CONNECT
  if (strlen(CONFIG_WIFI_SSID) == 0) {
    ESP_LOGW(TAG, "CONFIG_WIFI_SSID is empty; skipping WiFi connect");
    return;
  }

  s_wifi_event_group = xEventGroupCreate();

  ESP_ERROR_CHECK(esp_netif_init());
  ESP_ERROR_CHECK(esp_event_loop_create_default());
  esp_netif_create_default_wifi_sta();

  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));

  ESP_ERROR_CHECK(esp_event_handler_instance_register(
      WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL, NULL));
  ESP_ERROR_CHECK(esp_event_handler_instance_register(
      IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL, NULL));

  wifi_config_t wifi_config = {0};
  strncpy((char *)wifi_config.sta.ssid, CONFIG_WIFI_SSID,
          sizeof(wifi_config.sta.ssid) - 1);
#ifdef CONFIG_WIFI_PASSWORD
  strncpy((char *)wifi_config.sta.password, CONFIG_WIFI_PASSWORD,
          sizeof(wifi_config.sta.password) - 1);
#endif

  ESP_LOGI(TAG, "Connecting to SSID: %s", CONFIG_WIFI_SSID);
  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
  ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
  ESP_ERROR_CHECK(esp_wifi_start());

  /* Wait for connection */
  EventBits_t bits =
      xEventGroupWaitBits(s_wifi_event_group, WIFI_CONNECTED_BIT, pdFALSE,
                          pdFALSE, pdMS_TO_TICKS(10000));
  if (bits & WIFI_CONNECTED_BIT) {
    ESP_LOGI(TAG, "Connected to AP: %s", CONFIG_WIFI_SSID);
  } else {
    ESP_LOGW(TAG, "Failed to connect to AP within timeout");
  }
#else
  ESP_LOGI(TAG,
           "WiFi connect disabled in menuconfig (CONFIG_WIFI_CONNECT not set)");
#endif
}
