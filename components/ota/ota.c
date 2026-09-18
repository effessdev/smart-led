#include "esp_crt_bundle.h"
#include "esp_https_ota.h"
#include "esp_log.h"
#include "esp_ota_ops.h"

static const char *TAG = "OTA";

// For production, embed your server's CA certificate
extern const uint8_t server_cert_pem_start[] asm("_binary_ca_cert_pem_start");
extern const uint8_t server_cert_pem_end[] asm("_binary_ca_cert_pem_end");

esp_err_t perform_ota_update(const char *firmware_url) {
  ESP_LOGI(TAG, "Starting OTA update from %s", firmware_url);

  esp_http_client_config_t http_config = {
      .url = firmware_url,
      .crt_bundle_attach = esp_crt_bundle_attach,
      .timeout_ms = 30000,
      .keep_alive_enable = true,
      .max_redirection_count = 5,
      .buffer_size = 4096,
      .buffer_size_tx = 1024,
  };

  esp_https_ota_config_t ota_config = {
      .http_config = &http_config,
  };

  esp_err_t ret = esp_https_ota(&ota_config);

  if (ret == ESP_OK) {
    ESP_LOGI(TAG, "OTA update successful. Rebooting...");
    esp_restart();
  } else {
    ESP_LOGE(TAG, "OTA update failed: %s", esp_err_to_name(ret));
  }

  return ret;
}
