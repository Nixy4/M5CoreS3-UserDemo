//* std
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <thread>
#include <chrono>
//*Driver
#include "sdkconfig.h"
#include "bsp/esp-bsp.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_attr.h"
#include "esp_err.h"
#include "esp_log.h"
//*Middleware
#include "lvgl.h"
#include "mooncake.h"
//*APP
#include "faceRecogniz.hpp"

using namespace std;
using namespace mooncake;

Mooncake mc;

void taskMooncake(void* arg)
{
 	int id = mc.installApp(make_unique<AppFaceRecogniz>());
	mc.openApp(id);

	while(1)
	{
		mc.update();
		vTaskDelay(1);
	}

}

extern "C" void app_main(void)
{
	const char* _tag = "app_main";
	bsp_i2c_init();
	bsp_display_start();
	/*xTaskCreatePinnedToCore(lvgl_port_task,"taskLVGL",6144,NULL,4,xxx,APP_CPU_NUM);*/
	bsp_display_backlight_on();
	ESP_LOGI(_tag, "BSP Basic Init Success\n");

	mc.logAboutMsg();

	lv_obj_t* scr = lv_scr_act();
	lv_obj_t* label = lv_label_create(scr);
	lv_label_set_text(label, "Hello, Mooncake!");
	lv_obj_center(label);
	lv_obj_set_style_text_font(label, &lv_font_montserrat_12, 0);

	xTaskCreatePinnedToCore(taskMooncake, "taskMooncake", 1024*4, NULL, 5, NULL, APP_CPU_NUM);
}