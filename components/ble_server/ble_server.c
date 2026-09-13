#include "ble_server.h"
#include "esp_log.h"
#include <string.h>

#include "host/ble_att.h"
#include "host/ble_gatt.h"
#include "host/ble_hs.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"

static const char *TAG = "BLE_SERVER";
static uint8_t g_ble_addr_type;
static QueueHandle_t g_cmd_queue = NULL; // Replaced callback with queue handle

static const ble_uuid16_t LED_SVC_UUID = BLE_UUID16_INIT(0x1815);
static const ble_uuid16_t BRIGHTNESS_CHR_UUID = BLE_UUID16_INIT(0x2a58);

static int brightness_write_handler(uint16_t conn_handle, uint16_t attr_handle,
                                    struct ble_gatt_access_ctxt *ctxt,
                                    void *arg) {
  if (ctxt->op == BLE_GATT_ACCESS_OP_WRITE_CHR) {
    if (ctxt->om->om_len > 0) {
      uint8_t intensity = ctxt->om->om_data[0];

      ESP_LOGI(TAG, "BLE Received Intensity: %d%%", intensity);

      // Package data and send to queue
      if (g_cmd_queue != NULL) {
        app_cmd_t cmd;
        cmd.type = CMD_SET_INTENSITY;
        cmd.payload.intensity = intensity;

        // Push to queue immediately (0 ticks blocking)
        xQueueSend(g_cmd_queue, &cmd, 0);
      }
    }
    return 0;
  }
  return BLE_ATT_ERR_UNLIKELY;
}

static const struct ble_gatt_svc_def LED_SVC[] = {
    {
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = &LED_SVC_UUID.u,
        .characteristics =
            (struct ble_gatt_chr_def[]){
                {
                    .uuid = &BRIGHTNESS_CHR_UUID.u,
                    .access_cb = brightness_write_handler,
                    .flags = BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_READ,
                },
                {0}},
    },
    {0}};

static void ble_start_advertising(void) {
  struct ble_gap_adv_params adv_params; // adv = advertising
  struct ble_hs_adv_fields fields;

  memset(&fields, 0, sizeof(fields));
  fields.flags = BLE_HS_ADV_F_DISC_GEN | BLE_HS_ADV_F_BREDR_UNSUP;

  const char *device_name = "Smart LED";
  fields.name = (uint8_t *)device_name;
  fields.name_len = strlen(device_name);
  fields.name_is_complete = 1;

  ble_gap_adv_set_fields(&fields);

  memset(&adv_params, 0, sizeof(adv_params));
  adv_params.conn_mode = BLE_GAP_CONN_MODE_UND;
  adv_params.disc_mode = BLE_GAP_DISC_MODE_GEN;

  ble_gap_adv_start(g_ble_addr_type, NULL, BLE_HS_FOREVER, &adv_params, NULL,
                    NULL);
  ESP_LOGI(TAG, "Advertising started as '%s'", device_name);
}

static void ble_on_sync(void) {
  ble_hs_id_infer_auto(0, &g_ble_addr_type);
  ble_start_advertising();
}

static void nimble_host_task(void *param) {
  nimble_port_run();
  nimble_port_freertos_deinit();
}

esp_err_t ble_server_init(QueueHandle_t cmd_queue) {
  g_cmd_queue = cmd_queue; // Store the queue handle

  ESP_ERROR_CHECK(nimble_port_init());

  ble_svc_gap_init();
  ble_svc_gatt_init();
  ble_gatts_count_cfg(LED_SVC);
  ble_gatts_add_svcs(LED_SVC);

  ble_hs_cfg.sync_cb = ble_on_sync;

  nimble_port_freertos_init(nimble_host_task);

  return ESP_OK;
}