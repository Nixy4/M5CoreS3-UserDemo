#include "faceRecogniz.hpp"

void AppFaceRecogniz::uint16_swap(uint16_t* buf, size_t w, size_t h)
{
	for (size_t i = 0; i < w * h; i++) {
		buf[i] = (buf[i] >> 8) | (buf[i] << 8);
	}
}

AppFaceRecogniz::AppFaceRecogniz() : detector1(0.3F,0.3F,10,0.3F), detector2(0.4F,0.3F,10), recognizer()
// AppFaceRecogniz::AppFaceRecogniz(M5CoreS3* core) : detector1(0.3F,0.3F,10,0.3F), detector2(0.4F,0.3F,10), recognizer()
{
	// _core = core;
	ESP_LOGI(TAG, "on construct\n");
	setAppInfo().name = TAG;
}

void AppFaceRecogniz::onCreate()
{
	ESP_LOGI(TAG, "onCreate\n");
	bsp_camera_init(FRAMESIZE_QVGA);
	recognizer.set_partition(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, "fr");
	recognizer.set_ids_from_flash();
	bsp_display_lock(0);//!
	_lvoScr = lv_obj_create(NULL);
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
	lv_scr_load_anim(_lvoScr, LV_SCR_LOAD_ANIM_FADE_IN, 250, 0, 1);
	bsp_display_unlock();//!
}

void AppFaceRecogniz::onRunning()
{
	ESP_LOGI(TAG, "onRunning: %p %p %p\n", _lvoScr, _lviCam, _lvidscCam.data);
	camera_fb_t* frame = bsp_camera_fb_get();//!
	if (frame != NULL) 
	{
		bsp_display_lock(0);//!
		std::list<dl::detect::result_t>& detect_candidates = detector1.infer(
			(uint16_t*)frame->buf, { (int)frame->height, (int)frame->width, 3 });
		std::list<dl::detect::result_t>& detect_results = detector2.infer(
			(uint16_t*)frame->buf, { (int)frame->height, (int)frame->width, 3 }, detect_candidates);
		if (detect_results.size())
		{
			draw_detection_result((uint16_t*)frame->buf, frame->height, frame->width, detect_results);
			if (xTaskGetTickCount() - _lastTick > 1000)
			{
				face_info_t recognize_result = recognizer.recognize(
					(uint16_t*)frame->buf, { (int)frame->height, (int)frame->width, 3 }, detect_results.front().keypoint);
				if (recognize_result.id > 0) {
					ESP_LOGI("RECOGNIZE", "相似度: %f, 匹配ID: %d", recognize_result.similarity, recognize_result.id);
				}
				else {
					ESP_LOGE("RECOGNIZE", "相似度: %f, 匹配ID: %d", recognize_result.similarity, recognize_result.id);
				}
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

void AppFaceRecogniz::onDestroy()
{
	ESP_LOGI(TAG, "onDestroy\n");
	free((void*)_lvidscCam.data);
	detector1.~HumanFaceDetectMSR01();
	detector2.~HumanFaceDetectMNP01();
	recognizer.~FaceRecognition112V1S8();
}
