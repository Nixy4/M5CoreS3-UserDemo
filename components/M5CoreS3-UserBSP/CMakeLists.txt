idf_component_register(
    SRCS "M5CoreS3.cpp"
    INCLUDE_DIRS "."
    REQUIRES esp_driver_i2s esp_driver_gpio esp_driver_sdmmc spiffs
    PRIV_REQUIRES fatfs esp_lcd esp_driver_spi esp_driver_i2c
)
