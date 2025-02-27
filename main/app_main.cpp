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

void cppThreadFunction()
{
	const char* _tag = "cppThreadFunction";
	
	// 获取任务句柄
	TaskHandle_t taskHandle = xTaskGetCurrentTaskHandle();
	// 获取任务名称
	TaskStatus_t status = {};
	vTaskGetInfo(taskHandle, &status, pdTRUE, eInvalid);
	ESP_LOGI(_tag, "xHandle: %p\n", status.xHandle);
	ESP_LOGI(_tag, "pcTaskName: %s\n", status.pcTaskName);
	ESP_LOGI(_tag, "xTaskNumber: %u\n", (unsigned int)status.xTaskNumber);
	ESP_LOGI(_tag, "eCurrentState: %u\n", (unsigned int)status.eCurrentState);
	ESP_LOGI(_tag, "uxCurrentPriority: %u\n", (unsigned int)status.uxCurrentPriority);
	ESP_LOGI(_tag, "uxBasePriority: %u\n", (unsigned int)status.uxBasePriority);
	ESP_LOGI(_tag, "ulRunTimeCounter: %u\n", (unsigned int)status.ulRunTimeCounter);
	ESP_LOGI(_tag, "usStackHighWaterMark: %u\n", (unsigned int)status.usStackHighWaterMark); 

	while (true)
	{
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
}

extern "C" void app_main(void)
{
	const char* _tag = "app_main";
	bsp_i2c_init();
	bsp_display_start();// lvgl_port_task, "taskLVGL", 6144, NULL, 4, &lvgl_port_ctx.lvgl_task, APP_CPU_NUM
	bsp_display_backlight_on();
	ESP_LOGI(_tag, "BSP Basic Init Success\n");

	lv_obj_t* scr = lv_scr_act();
	lv_obj_t* label = lv_label_create(scr);
	lv_label_set_text(label, "Hello, Mooncake!");
	lv_obj_center(label);
	lv_obj_set_style_text_font(label, &lv_font_montserrat_12, 0);

	xTaskCreatePinnedToCore(taskMooncake, "taskMooncake", 1024*4, NULL, 5, NULL, APP_CPU_NUM);

	std::thread cppThread(cppThreadFunction);
	cppThread.detach();
}