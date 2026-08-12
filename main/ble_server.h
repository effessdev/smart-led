#ifndef BLE_SERVER_H
#define BLE_SERVER_H

#include "esp_err.h"
#include <stdint.h>


// Function pointer signature for handling intensity changes
typedef void (*ble_intensity_cb_t)(uint8_t intensity);

/**
 * @brief Initializes NVS, NimBLE, GATT services, and starts advertising.
 * @param cb Callback function to invoke when a mobile write occurs.
 * @return ESP_OK on success.
 */
esp_err_t ble_server_init(ble_intensity_cb_t cb);

#endif // BLE_SERVER_H
