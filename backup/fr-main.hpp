#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#define FR_CAM_MODULE_NAME "ESP32-CAM"

#define FR_CAM_DEFAULT_CONFIG() {			    \
 .pin_pwdn     = BSP_CAM_PIN_PWDN,        \
 .pin_reset    = BSP_CAM_PIN_RESET,       \
 .pin_xclk     = BSP_CAM_PIN_XCLK,        \
 .pin_sccb_sda = BSP_CAM_PIN_SIOD,        \
 .pin_sccb_scl = BSP_CAM_PIN_SIOC,        \
 .pin_d7       = BSP_CAM_PIN_D7,          \
 .pin_d6       = BSP_CAM_PIN_D6,          \
 .pin_d5       = BSP_CAM_PIN_D5,          \
 .pin_d4       = BSP_CAM_PIN_D4,          \
 .pin_d3       = BSP_CAM_PIN_D3,          \
 .pin_d2       = BSP_CAM_PIN_D2,          \
 .pin_d1       = BSP_CAM_PIN_D1,          \
 .pin_d0       = BSP_CAM_PIN_D0,          \
 .pin_vsync    = BSP_CAM_PIN_VSYNC,       \
 .pin_href     = BSP_CAM_PIN_HREF,        \
 .pin_pclk     = BSP_CAM_PIN_PCLK,        \
 .xclk_freq_hz = 10000000,                \
 .ledc_timer   = LEDC_TIMER_0,            \
 .ledc_channel = LEDC_CHANNEL_0,          \
 .pixel_format = PIXFORMAT_RGB565,        \
 .frame_size   = FRAMESIZE_240X240,       \
 .jpeg_quality = 12,                      \
 .fb_count     = 2,                       \
 .fb_location  = CAMERA_FB_IN_PSRAM,      \
 .grab_mode    = CAMERA_GRAB_WHEN_EMPTY,  \
}

typedef enum
{
	IDLE = 0,
	DETECT,
	ENROLL,
	RECOGNIZE,
	DELETE, 
} fr_ai_cap_t;

void ostk_fr_main(void *arg);
void fr_main_init(QueueHandle_t frame_out, QueueHandle_t http_req);
