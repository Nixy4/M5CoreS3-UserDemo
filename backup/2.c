#define DL_OUT_W 240
#define DL_OUT_H 240
#define DLFR_RECOG_INTERVAL 1000

HumanFaceDetectMSR01 detector(0.3F,0.3F,10,0.3F);
HumanFaceDetectMNP01 detector2(0.4F,0.3F,10);
FaceRecognition112V1S8 recognizer;

void rgb565_swap(uint16_t* buf, size_t w, size_t h)
{
	for (size_t i = 0; i < w * h; i++) {
		buf[i] = (buf[i] >> 8) | (buf[i] << 8);
	}
}

void ostk_fr(void* arg)
{
  //*AI识别器初始化
  recognizer.set_partition(ESP_PARTITION_TYPE_DATA,ESP_PARTITION_SUBTYPE_ANY,"fr");//设置人脸识别数据存储段落
  recognizer.set_ids_from_flash(); //从 Flash 中读取已注册的人脸特征

	bsp_display_lock(0);//!
	lv_obj_t* img = lv_img_create(lv_scr_act());
	lv_img_dsc_t img_dsc;
	img_dsc.header.magic = LV_IMAGE_HEADER_MAGIC;
	img_dsc.header.cf = LV_COLOR_FORMAT_RGB565;
	img_dsc.header.w = DL_OUT_W;
	img_dsc.header.h = DL_OUT_H;
	img_dsc.header.stride = DL_OUT_W * 2;
	img_dsc.data_size = DL_OUT_W * DL_OUT_H * 2;
	img_dsc.data = (const uint8_t*)heap_caps_malloc(img_dsc.data_size, MALLOC_CAP_SPIRAM|MALLOC_CAP_8BIT);
	assert(img_dsc.data);
	lv_img_set_src(img, &img_dsc);
	bsp_display_unlock();//!

	camera_fb_t *frame;
	volatile TickType_t recog_last_tick = 0;

	while (1) 
	{
		frame = bsp_camera_fb_get();//!
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
				if(xTaskGetTickCount()-recog_last_tick>DLFR_RECOG_INTERVAL) //识别间隔大于1秒钟才��别
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
					recog_last_tick = xTaskGetTickCount();    
				}
			}
			memcpy((void*)img_dsc.data, frame->buf, frame->len);
			bsp_camera_fb_return(frame);//!
			rgb565_swap((uint16_t*)img_dsc.data, DL_OUT_W, DL_OUT_H);
			lv_obj_invalidate(img);
			bsp_display_unlock();//!
		}
		vTaskDelay(1);
	}
}