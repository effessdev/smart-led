#ifndef OTA_H
#define OTA_H

#include "esp_err.h"

esp_err_t perform_ota_update(const char *firmware_url);

#endif // OTA_H
