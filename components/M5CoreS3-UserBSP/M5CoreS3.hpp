#pragma once
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
#include "driver/i2s_std.h"
#include "driver/i2s_common.h"
#include "driver/sdmmc_host.h"
//*Peripherals
#include "esp_codec_dev.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_ili9341.h"
#include "esp_lcd_touch_ft5x06.h"
#include "esp_codec_dev_defaults.h"
#include "esp_camera.h"
#include "SensorLTR553.hpp"
//*Middleware

#include "lvgl.h"
#include "esp_lvgl_port.h"

class M5CoreS3 
{
private:
	/* Features */
	typedef enum {
		FEATURE_LCD,
		FEATURE_TOUCH,
		FEATURE_SD,
		FEATURE_SPEAKER,
		FEATURE_CAMERA,
	} feature_t;

	typedef struct {
			void *dummy;    /*!< Prepared for future use. */
	} touch_config_t;

  typedef struct {
	  int max_transfer_sz;    /*!< Maximum transfer size, in bytes. */
  } display_config_t;

  typedef struct {
	  lvgl_port_cfg_t lvgl_port_cfg;  /*!< LVGL port configuration */
	  uint32_t        buffer_size;    /*!< Size of the buffer for the screen in pixels */
	  bool            double_buffer;  /*!< True, if should be allocated two buffers */
	  struct {
		  unsigned int buff_dma: 1;    /*!< Allocated LVGL buffer will be DMA capable */
		  unsigned int buff_spiram: 1; /*!< Allocated LVGL buffer will be in PSRAM */
	  } flags;
  } display_cfg_t;

	const char *TAG = "M5Stack";
	bool i2c_initialized = false;
	bool spi_initialized = false;
	bool camera_initialized = false;

public:
	lv_display_t *disp;
	lv_indev_t *disp_indev = NULL;
	
	esp_lcd_touch_handle_t tp;
	sdmmc_card_t *bsp_sdcard = NULL;

	i2c_master_bus_handle_t i2c_handle = NULL;
	i2c_master_dev_handle_t axp2101_h = NULL;
	i2c_master_dev_handle_t aw9523_h = NULL;

  i2s_chan_handle_t i2s_tx_chan = NULL;
  i2s_chan_handle_t i2s_rx_chan = NULL;
  const audio_codec_data_if_t* i2s_data_if = NULL;  /* Codec data interface */

	SemaphoreHandle_t camera_lock = NULL;
	volatile TickType_t camera_last_frame_tick = 0;

	esp_err_t i2c_init(void);
	esp_err_t i2c_deinit(void);
	esp_err_t enable_feature(feature_t feature);
	esp_err_t spi_init(uint32_t max_transfer_sz);
	
	esp_err_t spiffs_mount(void);
	esp_err_t spiffs_unmount(void);
	esp_err_t sdcard_mount(void);
	esp_err_t sdcard_unmount(void);

	esp_codec_dev_handle_t audio_codec_speaker_init(void);
	esp_codec_dev_handle_t audio_codec_microphone_init(void);

	esp_err_t display_brightness_init(void);
	esp_err_t display_brightness_set(int brightness_percent);
	esp_err_t display_backlight_off(void);
	esp_err_t display_backlight_on(void);
	esp_err_t display_new(const display_config_t *config, esp_lcd_panel_handle_t *ret_panel, esp_lcd_panel_io_handle_t *ret_io);
	esp_err_t touch_new(const touch_config_t *config, esp_lcd_touch_handle_t *ret_touch);
	lv_display_t *display_lcd_init(const display_cfg_t *cfg);
	lv_indev_t *display_indev_init(lv_display_t *disp);
	lv_display_t *display_start(void);
	lv_display_t *display_start_with_config(const display_cfg_t *cfg);
	lv_indev_t *display_get_input_dev(void);
	void display_rotate(lv_display_t *disp, lv_display_rotation_t rotation);
	bool display_lock(uint32_t timeout_ms);
	void display_unlock(void);
	void display_buf_swap(uint16_t* buf, size_t w, size_t h);

	esp_err_t camera_init(framesize_t framesize);
	esp_err_t camera_init();
	esp_err_t camera_deinit();
	camera_fb_t* camera_fb_get();
	void camera_fb_return(camera_fb_t *frame);

  esp_err_t audio_init(const i2s_std_config_t *i2s_config);
  const audio_codec_data_if_t* audio_get_codec_itf(void);
};