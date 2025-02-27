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

	void uint16_swap(uint16_t* buf, size_t w, size_t h);

public:

	AppFaceRecogniz() : detector(0.3F,0.3F,10,0.3F), detector2(0.4F,0.3F,10), recognizer()
	{
		ESP_LOGI( _tag, "on construct\n");
		setAppInfo().name = _tag;
	}

	HumanFaceDetectMSR01 detector;
	HumanFaceDetectMNP01 detector2;
	FaceRecognition112V1S8 recognizer;

	void onCreate() override
	{
		ESP_LOGI( _tag, "onCreate\n");
		bsp_camera_init(FRAMESIZE_240X240);
		//*AI识别器初始化
		recognizer.set_partition(ESP_PARTITION_TYPE_DATA,ESP_PARTITION_SUBTYPE_ANY,"fr");//设置人脸识别数据存储段落
		recognizer.set_ids_from_flash(); //从 Flash 中读取已注册的人脸特征
		//*LVGL
		bsp_display_lock(0);//!
		_lvoScr = lv_obj_create(NULL);//!
		_lviCam = lv_img_create(_lvoScr);
		lv_obj_center(_lviCam);
		_lvidscCam.header.magic = LV_IMAGE_HEADER_MAGIC;
		_lvidscCam.header.cf = LV_COLOR_FORMAT_RGB565;
		_lvidscCam.header.w = DL_OUT_W;
		_lvidscCam.header.h = DL_OUT_H;
		_lvidscCam.header.stride = DL_OUT_W * 2;
		_lvidscCam.data_size = DL_OUT_W * DL_OUT_H * 2;
		_lvidscCam.data = (const uint8_t*)heap_caps_malloc(_lvidscCam.data_size, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
		assert(_lvidscCam.data);
		lv_img_set_src(_lviCam, &_lvidscCam);
		lv_scr_load_anim(_lvoScr, LV_SCR_LOAD_ANIM_FADE_IN, 250, 0, 1);//!
		bsp_display_unlock();//!
	}

	void onRunning() override
	{
		ESP_LOGI( _tag, "onRunning: %p %p %p\n", _lvoScr, _lviCam, _lvidscCam.data);
		camera_fb_t* frame = bsp_camera_fb_get();
		if (frame!=NULL) 
		{
			bsp_display_lock(0);//!
			std::list<dl::detect::result_t> &detect_candidates = detector.infer(
				(uint16_t *)frame->buf, {(int)frame->height, (int)frame->width, 3});
			std::list<dl::detect::result_t> &detect_results = detector2.infer(
				(uint16_t *)frame->buf, {(int)frame->height, (int)frame->width, 3}, detect_candidates);
			if(detect_results.size())
			{
				// 绘制检测结果
				draw_detection_result((uint16_t *)frame->buf, frame->height, frame->width, detect_results);
				if(xTaskGetTickCount() - _lastTick > 1000) //识别间隔大于1秒钟才��别
				{
					//识别图像,获取结果
					face_info_t recognize_result = recognizer.recognize(
						(uint16_t *)frame->buf, {(int)frame->height, (int)frame->width, 3}, detect_results.front().keypoint);
					//分析识别结果
					if (recognize_result.id > 0) {
						ESP_LOGI("RECOGNIZE", "相似度: %f, 匹配ID: %d", recognize_result.similarity, recognize_result.id);
					} else {
						ESP_LOGE("RECOGNIZE", "相似度: %f, 匹配ID: %d", recognize_result.similarity, recognize_result.id);
					}
					///更新最后识别时间
					_lastTick = xTaskGetTickCount();    
				}
			}
			memcpy((void*)_lvidscCam.data, frame->buf, frame->len);
			bsp_camera_fb_return(frame);//!
			uint16_swap((uint16_t*)_lvidscCam.data, DL_OUT_W, DL_OUT_H);
			lv_obj_invalidate(_lviCam);
			bsp_display_unlock();//!
		}
	}

	void onDestroy() override
	{
		ESP_LOGI( _tag, "onDestroy\n");
		free((void*)_lvidscCam.data);
		detector.~HumanFaceDetectMSR01();
		detector2.~HumanFaceDetectMNP01();
		recognizer.~FaceRecognition112V1S8();
	}

	void uint16_swap(uint16_t* buf, size_t w, size_t h)
	{
		for (size_t i = 0; i < w * h; i++) {
			buf[i] = (buf[i] >> 8) | (buf[i] << 8);
		}
	}
};