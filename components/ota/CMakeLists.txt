idf_component_register(
  SRCS "ota.c"
  INCLUDE_DIRS "include"
  REQUIRES esp_https_ota bootloader_support app_update mbedtls
)
