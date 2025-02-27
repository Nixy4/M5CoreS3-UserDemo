#pragma once
//* std
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <thread>
//*Driver
#include "sdkconfig.h"
#include "bsp/esp-bsp.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "driver/spi_master.h"
#include "esp_attr.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_ops.h"
#include "esp_camera.h"
//*Middleware
#include "lvgl.h"
#include "mooncake.h"
//*APP
#include "dl_image.hpp"
#include "human_face_detect_msr01.hpp"
#include "human_face_detect_mnp01.hpp"
#include "face_recognition_tool.hpp"
#include "face_recognition_112_v1_s8.hpp"
#include "who_ai_utils.hpp"

/**
 * GC0308 小端
 * OV2640 大端
 * ILI9342C 大端
 * DL 大端
 */

using namespace std;
using namespace dl;
using namespace mooncake;

class AppFaceRecogniz : public AppAbility 
{
private:

	const char* _tag = "APP 人脸识别";

	const uint32_t  DL_OUT_W = 240;
	const uint32_t  DL_OUT_H = 240;
	const uint32_t  DL_RECOG_INTERVAL = 1000;

	lv_obj_t*    _lvoScr    = nullptr;
	lv_obj_t*    _lviCam    = nullptr;
	lv_img_dsc_t _lvidscCam = {0};

	volatile TickType_t _lastTick = 0;

	HumanFaceDetectMSR01 detector;
	HumanFaceDetectMNP01 detector2;
	FaceRecognition112V1S8 recognizer;

	void uint16_swap(uint16_t* buf, size_t w, size_t h);
	
public:
	AppFaceRecogniz();
	void onCreate();
	void onRunning();
	void onDestroy();
};