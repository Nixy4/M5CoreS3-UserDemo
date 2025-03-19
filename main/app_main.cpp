//* std
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <iostream>
#include <thread>
#include <chrono>
//*SoC
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_attr.h"
#include "esp_err.h"
#include "esp_check.h"
#include "esp_log.h"
#include "esp_vfs_fat.h"
#include "esp_spiffs.h"
#include "driver/gpio.h"
#include "driver/i2c_master.h"
#include "driver/spi_master.h"
#include "driver/sdspi_host.h"
//*Peripherals
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_ili9341.h"
#include "esp_lcd_touch_ft5x06.h"
#include "esp_codec_dev_defaults.h"
#include "esp_camera.h"
// #include "SensorLTR553.hpp"
//*Device
#include "bsp/esp-bsp.h"
// #include "M5CoreS3.hpp"

//*Middleware
#include "lvgl.h"
#include "mooncake.h"
//*APP
#include "faceRecogniz.hpp"

using namespace std;
using namespace mooncake;

extern "C" void app_main(void)
{

}