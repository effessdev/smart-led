#include "esp_log.h"
#include "nvs_flash.h"

/**
 * Note: Errors are logged and povided to the caller. No need to log them twice.
 */
esp_err_t led_get_intensity_from_flash(int8_t *intensity_percent);

/**
 * Note: Errors are logged and povided to the caller. No need to log them twice.
 */
esp_err_t led_set_intensity_in_flash(uint8_t intensity_percent);
