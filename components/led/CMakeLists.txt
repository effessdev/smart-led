idf_component_register(
  SRCS "led.c" "storage.c"
  INCLUDE_DIRS "include"
  REQUIRES "log" "nvs_flash" "esp_timer" "esp_driver_gpio" "esp_driver_ledc"
)
