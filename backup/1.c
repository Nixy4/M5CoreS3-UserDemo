void ostk_cam(void* arg)
{
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
	bsp_display_unlock();//!
	while(1)
	{
		camera_fb_t *frame = esp_camera_fb_get();//!
		if (frame) {
			bsp_display_lock(0);//!
			memset((void*)img_dsc.data, 0, img_dsc.data_size);
			memcpy((void*)img_dsc.data, frame->buf, frame->len);
			esp_camera_fb_return(frame);//!
			lv_draw_sw_rgb565_swap((uint8_t*)img_dsc.data, img_dsc.data_size);
			lv_img_set_src(img, &img_dsc);
			bsp_display_unlock();//!
		}
	}
}
