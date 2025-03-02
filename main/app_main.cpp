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
#include "SensorLTR553.hpp"
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
// using namespace dl;

class M5CoreS3
{
public:

	static M5CoreS3& getInstance() 
	{
		static M5CoreS3 incetance;
		return incetance;
	}

	M5CoreS3(const M5CoreS3&) = delete;
	M5CoreS3& operator = (const M5CoreS3&) = delete;

	esp_err_t i2cInit() { return bsp_i2c_init(); }
	esp_err_t i2cDeinit() { return bsp_i2c_deinit(); }
	lv_disp_t* displayInit() { return bsp_display_start(); }
	esp_err_t displayOn() { return bsp_display_backlight_on(); }
	esp_err_t displayOff() { return bsp_display_backlight_off(); }
	// esp_err_t cameraInit(framesize_t framesize) { return bsp_camera_init(framesize); }
	// esp_err_t cameraDeinit() { return bsp_camera_deinit(); }

private:

	M5CoreS3()
	{
		
	}

	~M5CoreS3()
	{

	}

};

M5CoreS3& core = M5CoreS3::getInstance();	

extern "C" void app_main(void)
{

}