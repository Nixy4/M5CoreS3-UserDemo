#include "M5CoreS3.hpp"

/**************************************************************************************************
 *  BSP Capabilities
 **************************************************************************************************/

#define BSP_CAPS_DISPLAY        1
#define BSP_CAPS_TOUCH          1
#define BSP_CAPS_BUTTONS        0
#define BSP_CAPS_AUDIO          1
#define BSP_CAPS_AUDIO_SPEAKER  1
#define BSP_CAPS_AUDIO_MIC      1
#define BSP_CAPS_SDCARD         1
#define BSP_CAPS_IMU            0
#define BSP_CAPS_CAMERA         1

/**************************************************************************************************
 *  M5Stack-Core-S3 pinout
 **************************************************************************************************/
/* I2C */
#define BSP_I2C_SCL           (GPIO_NUM_11)
#define BSP_I2C_SDA           (GPIO_NUM_12)
#define BSP_I2C_NUM           CONFIG_BSP_I2C_NUM

/* Audio */
#define BSP_I2S_SCLK          (GPIO_NUM_34)
#define BSP_I2S_MCLK          (GPIO_NUM_0)
#define BSP_I2S_LCLK          (GPIO_NUM_33)
#define BSP_I2S_DOUT          (GPIO_NUM_13) // To Codec AW88298
#define BSP_I2S_DSIN          (GPIO_NUM_14) // From ADC ES7210
#define BSP_POWER_AMP_IO      (GPIO_NUM_NC)
#define BSP_MUTE_STATUS       (GPIO_NUM_NC)

/* Display */
#define BSP_LCD_MOSI          (GPIO_NUM_37)
#define BSP_LCD_MISO          (GPIO_NUM_35)
#define BSP_LCD_PCLK          (GPIO_NUM_36)
#define BSP_LCD_CS            (GPIO_NUM_3)
#define BSP_LCD_DC            (GPIO_NUM_35)
#define BSP_LCD_RST           (GPIO_NUM_NC)
#define BSP_LCD_BACKLIGHT     (GPIO_NUM_NC)
#define BSP_LCD_TOUCH_INT     (GPIO_NUM_NC)

/* Camera */
#define BSP_CAMERA_XCLK      (GPIO_NUM_NC)
#define BSP_CAMERA_PCLK      (GPIO_NUM_45)
#define BSP_CAMERA_VSYNC     (GPIO_NUM_46)
#define BSP_CAMERA_HSYNC     (GPIO_NUM_38)
#define BSP_CAMERA_D0        (GPIO_NUM_39)
#define BSP_CAMERA_D1        (GPIO_NUM_40)
#define BSP_CAMERA_D2        (GPIO_NUM_41)
#define BSP_CAMERA_D3        (GPIO_NUM_42)
#define BSP_CAMERA_D4        (GPIO_NUM_15)
#define BSP_CAMERA_D5        (GPIO_NUM_16)
#define BSP_CAMERA_D6        (GPIO_NUM_48)
#define BSP_CAMERA_D7        (GPIO_NUM_47)

/* SD card */
#define BSP_SD_MOSI           (GPIO_NUM_37)
#define BSP_SD_MISO           (GPIO_NUM_35)
#define BSP_SD_SCK            (GPIO_NUM_36)
#define BSP_SD_CS             (GPIO_NUM_4)
#define BSP_SD_MOUNT_POINT    CONFIG_BSP_SD_MOUNT_POINT

/* USB */
#define BSP_USB_POS           (GPIO_NUM_20)
#define BSP_USB_NEG           (GPIO_NUM_19)

/* I2C Device */
#define BSP_AXP2101_ADDR    0x34
#define BSP_AW9523_ADDR     0x58

/* Macro */
#define BSP_ERROR_CHECK_RETURN_ERR(x)    ESP_ERROR_CHECK(x)
#define BSP_ERROR_CHECK_RETURN_NULL(x)   ESP_ERROR_CHECK(x)
#define BSP_ERROR_CHECK(x, ret)          ESP_ERROR_CHECK(x)
#define BSP_NULL_CHECK(x, ret)           assert(x)
#define BSP_NULL_CHECK_GOTO(x, goto_tag) assert(x)

/* LCD */
// Bit number used to represent command and parameter
#define LCD_CMD_BITS           8
#define LCD_PARAM_BITS         8
#define LCD_LEDC_CH            CONFIG_BSP_DISPLAY_BRIGHTNESS_LEDC_CH
#define LCD_CMD_BITS           8
#define LCD_PARAM_BITS         8
#define LCD_LEDC_CH            CONFIG_BSP_DISPLAY_BRIGHTNESS_LEDC_CH
#define BSP_LCD_PIXEL_CLOCK_HZ (40 * 1000 * 1000)
#define BSP_LCD_SPI_NUM        (SPI3_HOST)

/* LCD color formats */
#define ESP_LCD_COLOR_FORMAT_RGB565    (1)
#define ESP_LCD_COLOR_FORMAT_RGB888    (2)

/* LCD display color format */
#define BSP_LCD_COLOR_FORMAT        (ESP_LCD_COLOR_FORMAT_RGB565)
/* LCD display color bytes endianess */
#define BSP_LCD_BIGENDIAN           (1)
/* LCD display color bits */
#define BSP_LCD_BITS_PER_PIXEL      (16)
/* LCD display color space */
#define BSP_LCD_COLOR_SPACE         (ESP_LCD_COLOR_SPACE_BGR)
/* LCD definition */
#define BSP_LCD_H_RES              (320)
#define BSP_LCD_V_RES              (240)
#define BSP_LCD_PIXEL_CLOCK_HZ     (40 * 1000 * 1000)
#define BSP_LCD_SPI_NUM            (SPI3_HOST)
#define BSP_LCD_DRAW_BUFF_SIZE     (BSP_LCD_H_RES * 50)
#define BSP_LCD_DRAW_BUFF_DOUBLE   (1)

#define BSP_CAMERA_DEFAULT_CONFIG         \
	{                                       \
		.pin_pwdn = GPIO_NUM_NC,              \
		.pin_reset = GPIO_NUM_NC,             \
		.pin_xclk = BSP_CAMERA_XCLK,          \
		.pin_sccb_sda = GPIO_NUM_NC,          \
		.pin_sccb_scl = GPIO_NUM_NC,          \
		.pin_d7 = BSP_CAMERA_D7,              \
		.pin_d6 = BSP_CAMERA_D6,              \
		.pin_d5 = BSP_CAMERA_D5,              \
		.pin_d4 = BSP_CAMERA_D4,              \
		.pin_d3 = BSP_CAMERA_D3,              \
		.pin_d2 = BSP_CAMERA_D2,              \
		.pin_d1 = BSP_CAMERA_D1,              \
		.pin_d0 = BSP_CAMERA_D0,              \
		.pin_vsync = BSP_CAMERA_VSYNC,        \
		.pin_href = BSP_CAMERA_HSYNC,         \
		.pin_pclk = BSP_CAMERA_PCLK,          \
		.xclk_freq_hz = 10000000,             \
		.ledc_timer = LEDC_TIMER_0,           \
		.ledc_channel = LEDC_CHANNEL_0,       \
		.pixel_format = PIXFORMAT_RGB565,     \
		.frame_size = FRAMESIZE_QVGA,         \
		.jpeg_quality = 12,                   \
		.fb_count = 2,                        \
		.fb_location = CAMERA_FB_IN_PSRAM,    \
    .grab_mode = CAMERA_GRAB_WHEN_EMPTY,  \
		.sccb_i2c_port = BSP_I2C_NUM,         \
	}

#define BSP_CAMERA_VFLIP        0
#define BSP_CAMERA_HMIRROR      1
#define BSP_CAMERA_FPS_MAX      15

/* I2S */
/* Can be used for i2s_std_gpio_config_t and/or i2s_std_config_t initialization */
#define BSP_I2S_GPIO_CFG       \
	{                          \
		.mclk = BSP_I2S_MCLK,  \
		.bclk = BSP_I2S_SCLK,  \
		.ws = BSP_I2S_LCLK,    \
		.dout = BSP_I2S_DOUT,  \
		.din = BSP_I2S_DSIN,   \
		.invert_flags = {      \
			.mclk_inv = false, \
			.bclk_inv = false, \
			.ws_inv = false,   \
		},                     \
	}

/* This configuration is used by default in bsp_audio_init() */
#define BSP_I2S_DUPLEX_MONO_CFG(_sample_rate)                                                         \
	{                                                                                                 \
		.clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(_sample_rate),                                          \
		.slot_cfg = I2S_STD_PHILIP_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_MONO), \
		.gpio_cfg = BSP_I2S_GPIO_CFG,                                                                 \
	}

esp_err_t M5CoreS3::i2c_init(void)
{
  /* I2C was initialized before */
  if (i2c_initialized) {
    return ESP_OK;
  }

  const i2c_master_bus_config_t i2c_config = {
    .i2c_port = BSP_I2C_NUM,
    .sda_io_num = BSP_I2C_SDA,
    .scl_io_num = BSP_I2C_SCL,
    .clk_source = I2C_CLK_SRC_DEFAULT,
    .glitch_ignore_cnt = 0,
    .intr_priority = 0,
    .trans_queue_depth = 0,
    .flags = {
      .enable_internal_pullup = false,
      .allow_pd = false,
    },
  };
  BSP_ERROR_CHECK_RETURN_ERR(i2c_new_master_bus(&i2c_config, &i2c_handle));
  // AXP2101 and AW9523 are managed by this BSP
  const i2c_device_config_t axp2101_config = {
    .dev_addr_length = I2C_ADDR_BIT_LEN_7,
    .device_address = BSP_AXP2101_ADDR,
    .scl_speed_hz = CONFIG_BSP_I2C_CLK_SPEED_HZ,
    .scl_wait_us = 0,
    .flags = {
      .disable_ack_check = 0,
    },
  };
  BSP_ERROR_CHECK_RETURN_ERR(i2c_master_bus_add_device(i2c_handle, &axp2101_config, &axp2101_h));
  const i2c_device_config_t aw9523_config = {
    .dev_addr_length = I2C_ADDR_BIT_LEN_7,
    .device_address = BSP_AW9523_ADDR,
    .scl_speed_hz = CONFIG_BSP_I2C_CLK_SPEED_HZ,
    .scl_wait_us = 0,
    .flags = {
      .disable_ack_check = 0,
    },
  };
  BSP_ERROR_CHECK_RETURN_ERR(i2c_master_bus_add_device(i2c_handle, &aw9523_config, &aw9523_h));

  i2c_initialized = true;
  return ESP_OK;
}

esp_err_t M5CoreS3::i2c_deinit(void)
{
  BSP_ERROR_CHECK_RETURN_ERR(i2c_del_master_bus(i2c_handle));
  i2c_initialized = false;
  return ESP_OK;
}

esp_err_t M5CoreS3::enable_feature(feature_t feature)
{
  esp_err_t err = ESP_OK;
  static uint8_t aw9523_P0 = 0b10;
  static uint8_t aw9523_P1 = 0b10100000;
  uint8_t data[2];

  /* Initilize I2C */
  BSP_ERROR_CHECK_RETURN_ERR(i2c_init());

  switch (feature) {
  case FEATURE_LCD:
    /* Enable LCD */
    aw9523_P1 |= (1 << 1);
    break;
  case FEATURE_TOUCH:
    /* Enable Touch */
    aw9523_P0 |= (1);
    break;
  case FEATURE_SD:
    /* AXP ALDO4 voltage / SD Card / 3V3 */
    data[0] = 0x95;
    data[1] = 0b00011100; //3V3
    err |= i2c_master_transmit(axp2101_h, data, sizeof(data), 1000);
    /* Enable SD */
    aw9523_P0 |= (1 << 4);
    break;
  case FEATURE_SPEAKER:
    /* AXP ALDO1 voltage / PA PVDD / 1V8 */
    data[0] = 0x92;
    data[1] = 0b00001101; //1V8
    err |= i2c_master_transmit(axp2101_h, data, sizeof(data), 1000);
    /* AXP ALDO2 voltage / Codec / 3V3 */
    data[0] = 0x93;
    data[1] = 0b00011100; //3V3
    err |= i2c_master_transmit(axp2101_h, data, sizeof(data), 1000);
    /* AXP ALDO3 voltage / Codec+Mic / 3V3 */
    data[0] = 0x94;
    data[1] = 0b00011100; //3V3
    err |= i2c_master_transmit(axp2101_h, data, sizeof(data), 1000);
    /* AW9523 P0 is in push-pull mode */
    data[0] = 0x11;
    data[1] = 0x10;
    err |= i2c_master_transmit(aw9523_h, data, sizeof(data), 1000);
    /* Enable Codec AW88298 */
    aw9523_P0 |= (1 << 2);
    break;
  case FEATURE_CAMERA:
    /* Enable Camera */
    aw9523_P1 |= (1);
    break;
  }

  data[0] = 0x02;
  data[1] = aw9523_P0;
  err |= i2c_master_transmit(aw9523_h, data, sizeof(data), 1000);

  data[0] = 0x03;
  data[1] = aw9523_P1;
  err |= i2c_master_transmit(aw9523_h, data, sizeof(data), 1000);

  return err;
}

esp_err_t M5CoreS3::spi_init(uint32_t max_transfer_sz)
{
	/* SPI was initialized before */
	if (spi_initialized) {
		return ESP_OK;
	}

	ESP_LOGD(TAG, "Initialize SPI bus");
	const spi_bus_config_t buscfg = {
		.mosi_io_num = BSP_LCD_MOSI,
		.miso_io_num = BSP_LCD_MISO,
		.sclk_io_num = BSP_LCD_PCLK,
		.quadwp_io_num = GPIO_NUM_NC,
		.quadhd_io_num = GPIO_NUM_NC,
    .data4_io_num = GPIO_NUM_NC,
    .data5_io_num = GPIO_NUM_NC,
    .data6_io_num = GPIO_NUM_NC,
    .data7_io_num = GPIO_NUM_NC,
    .data_io_default_level = false,
		.max_transfer_sz = (int)max_transfer_sz,
    .flags = 0,
    .isr_cpu_id = ESP_INTR_CPU_AFFINITY_AUTO,
    .intr_flags = 0
	};
	ESP_RETURN_ON_ERROR(spi_bus_initialize(BSP_LCD_SPI_NUM, &buscfg, SPI_DMA_CH_AUTO), TAG, "SPI init failed");

	spi_initialized = true;

	return ESP_OK;
}

esp_err_t M5CoreS3::spiffs_mount(void)
{
  esp_vfs_spiffs_conf_t conf = {
    .base_path = CONFIG_BSP_SPIFFS_MOUNT_POINT,
    .partition_label = CONFIG_BSP_SPIFFS_PARTITION_LABEL,
    .max_files = CONFIG_BSP_SPIFFS_MAX_FILES,
#ifdef CONFIG_BSP_SPIFFS_FORMAT_ON_MOUNT_FAIL
    .format_if_mount_failed = true,
#else
    .format_if_mount_failed = false,
#endif
  };

  esp_err_t ret_val = esp_vfs_spiffs_register(&conf);

  BSP_ERROR_CHECK_RETURN_ERR(ret_val);

  size_t total = 0, used = 0;
  ret_val = esp_spiffs_info(conf.partition_label, &total, &used);
  if (ret_val != ESP_OK) {
    ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret_val));
  } else {
    ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);
  }

  return ret_val;
}

esp_err_t M5CoreS3::spiffs_unmount(void)
{
  return esp_vfs_spiffs_unregister(CONFIG_BSP_SPIFFS_PARTITION_LABEL);
}

esp_err_t M5CoreS3::sdcard_mount(void)
{
  BSP_ERROR_CHECK_RETURN_ERR(enable_feature(FEATURE_SD));

  const esp_vfs_fat_sdmmc_mount_config_t mount_config = {
#ifdef CONFIG_BSP_SD_FORMAT_ON_MOUNT_FAIL
    .format_if_mount_failed = true,
#else
    .format_if_mount_failed = false,
#endif
    .max_files = 5,
    .allocation_unit_size = 16 * 1024,
    .disk_status_check_enable = false,
    .use_one_fat = false
  };

  sdmmc_host_t host = SDSPI_HOST_DEFAULT();
  host.slot = BSP_LCD_SPI_NUM;
  sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
  slot_config.gpio_cs = BSP_SD_CS;
  slot_config.host_id = BSP_LCD_SPI_NUM;

  ESP_RETURN_ON_ERROR(spi_init((BSP_LCD_H_RES * BSP_LCD_V_RES) * sizeof(uint16_t)), TAG, "");

  return esp_vfs_fat_sdspi_mount(BSP_SD_MOUNT_POINT, &host, &slot_config, &mount_config, &bsp_sdcard);
}	

esp_err_t M5CoreS3::sdcard_unmount(void)
{
  return esp_vfs_fat_sdcard_unmount(BSP_SD_MOUNT_POINT, bsp_sdcard);
}

esp_codec_dev_handle_t M5CoreS3::audio_codec_speaker_init(void)
{
  const audio_codec_data_if_t *i2s_data_if = audio_get_codec_itf();
  if (i2s_data_if == NULL) {
    /* Initilize I2C */
    BSP_ERROR_CHECK_RETURN_NULL(i2c_init());
    /* Configure I2S peripheral and Power Amplifier */
    BSP_ERROR_CHECK_RETURN_NULL(audio_init(NULL));
    i2s_data_if = audio_get_codec_itf();
  }
  assert(i2s_data_if);

  BSP_ERROR_CHECK_RETURN_ERR(enable_feature(FEATURE_SPEAKER));

  audio_codec_i2c_cfg_t i2c_cfg = {
    .port = BSP_I2C_NUM,
    .addr = AW88298_CODEC_DEFAULT_ADDR,
    .bus_handle = i2c_handle,
  };
  const audio_codec_ctrl_if_t *out_ctrl_if = audio_codec_new_i2c_ctrl(&i2c_cfg);

  const audio_codec_gpio_if_t *gpio_if = audio_codec_new_gpio();

  // New output codec interface
  aw88298_codec_cfg_t aw88298_cfg = {
    .ctrl_if = out_ctrl_if,
    .gpio_if = gpio_if,
    .reset_pin = 0,
    .hw_gain = {
      .pa_voltage = 0,
      .codec_dac_voltage = 0,
      .pa_gain = 15,
    }
  };
  const audio_codec_if_t *out_codec_if = aw88298_codec_new(&aw88298_cfg);

  esp_codec_dev_cfg_t codec_dev_cfg = {
    .dev_type = ESP_CODEC_DEV_TYPE_OUT,
    .codec_if = out_codec_if,
    .data_if = i2s_data_if,
  };
  return esp_codec_dev_new(&codec_dev_cfg);
}

esp_codec_dev_handle_t M5CoreS3::audio_codec_microphone_init(void)
{
  const audio_codec_data_if_t *i2s_data_if = audio_get_codec_itf();
  if (i2s_data_if == NULL) {
    /* Initialize I2C */
    BSP_ERROR_CHECK_RETURN_NULL(i2c_init());
    /* Configure I2S peripheral and Power Amplifier */
    BSP_ERROR_CHECK_RETURN_NULL(audio_init(NULL));
    i2s_data_if = audio_get_codec_itf();
  }
  assert(i2s_data_if);

  audio_codec_i2c_cfg_t i2c_cfg = {
    .port = BSP_I2C_NUM,
    .addr = ES7210_CODEC_DEFAULT_ADDR,
    .bus_handle = i2c_handle,
  };
  const audio_codec_ctrl_if_t *i2c_ctrl_if = audio_codec_new_i2c_ctrl(&i2c_cfg);
  BSP_NULL_CHECK(i2c_ctrl_if, NULL);

  es7210_codec_cfg_t es7210_cfg = {
    .ctrl_if = i2c_ctrl_if,
    .master_mode = 0,
    .mic_selected = 0,
    .mclk_src = ES7210_MCLK_FROM_PAD,
    .mclk_div = 0
  };
  const audio_codec_if_t *es7210_dev = es7210_codec_new(&es7210_cfg);
  BSP_NULL_CHECK(es7210_dev, NULL);

  esp_codec_dev_cfg_t codec_es7210_dev_cfg = {
    .dev_type = ESP_CODEC_DEV_TYPE_IN,
    .codec_if = es7210_dev,
    .data_if = i2s_data_if,
  };
  return esp_codec_dev_new(&codec_es7210_dev_cfg);
}

esp_err_t M5CoreS3::display_brightness_init(void)
{
  /* Initilize I2C */
  BSP_ERROR_CHECK_RETURN_ERR(i2c_init());

  const uint8_t lcd_bl_en[] = { 0x90, 0xBF }; // AXP DLDO1 Enable
  ESP_RETURN_ON_ERROR(i2c_master_transmit(axp2101_h, lcd_bl_en, sizeof(lcd_bl_en), 1000), TAG, "I2C write failed");
  const uint8_t lcd_bl_val[] = { 0x99, 0b00011000 };  // AXP DLDO1 voltage
  ESP_RETURN_ON_ERROR(i2c_master_transmit(axp2101_h, lcd_bl_val, sizeof(lcd_bl_val), 1000), TAG, "I2C write failed");

  return ESP_OK;
}

esp_err_t M5CoreS3::display_brightness_set(int brightness_percent)
{
  if (brightness_percent > 100) {
    brightness_percent = 100;
  }
  if (brightness_percent < 0) {
    brightness_percent = 0;
  }

  ESP_LOGI(TAG, "Setting LCD backlight: %d%%", brightness_percent);
  const uint8_t reg_val = 20 + ((8 * brightness_percent) / 100); // 0b00000 ~ 0b11100; under 20, it is too dark
  const uint8_t lcd_bl_val[] = { 0x99, reg_val }; // AXP DLDO1 voltage
  ESP_RETURN_ON_ERROR(i2c_master_transmit(axp2101_h, lcd_bl_val, sizeof(lcd_bl_val), 1000), TAG, "I2C write failed");

  return ESP_OK;
}	

esp_err_t M5CoreS3::display_backlight_off(void)
{
  return display_brightness_set(0);
}

esp_err_t M5CoreS3::display_backlight_on(void)
{
  return display_brightness_set(100);
}

esp_err_t M5CoreS3::display_new(const display_config_t *config, esp_lcd_panel_handle_t *ret_panel, esp_lcd_panel_io_handle_t *ret_io)
{
  esp_err_t ret = ESP_OK;
  assert(config != NULL && config->max_transfer_sz > 0);

  BSP_ERROR_CHECK_RETURN_ERR(enable_feature(FEATURE_LCD));
  BSP_ERROR_CHECK_RETURN_ERR(enable_feature(FEATURE_CAMERA));

  /* Initialize SPI */
  ESP_RETURN_ON_ERROR(spi_init(config->max_transfer_sz), TAG, "");

  ESP_LOGD(TAG, "Install panel IO");
  const esp_lcd_panel_io_spi_config_t io_config = {
    .cs_gpio_num = BSP_LCD_CS,
    .dc_gpio_num = BSP_LCD_DC,
    .spi_mode = 0,
    .pclk_hz = BSP_LCD_PIXEL_CLOCK_HZ,
    .trans_queue_depth = 10,
    .on_color_trans_done = NULL,
    .user_ctx = NULL,
    .lcd_cmd_bits = LCD_CMD_BITS,
    .lcd_param_bits = LCD_PARAM_BITS,
    .cs_ena_pretrans = 0,
    .cs_ena_posttrans = 0,
    .flags = {
      .dc_high_on_cmd = 0,
      .dc_low_on_data = 0,
      .dc_low_on_param = 0,
      .octal_mode = 0,
      .quad_mode = 0,
      .sio_mode = 0,
      .lsb_first = 0,
      .cs_high_active = 0
    }
  };
  ret = esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)BSP_LCD_SPI_NUM, &io_config, ret_io);

  ESP_LOGD(TAG, "Install LCD driver");
  const esp_lcd_panel_dev_config_t panel_config = {
    .reset_gpio_num = BSP_LCD_RST, // Shared with Touch reset
    .color_space = BSP_LCD_COLOR_SPACE,
    .data_endian = LCD_RGB_DATA_ENDIAN_BIG,
    .bits_per_pixel = BSP_LCD_BITS_PER_PIXEL,
    .flags = {
      .reset_active_high = 0
    },
    .vendor_config = NULL
  };
  ESP_GOTO_ON_ERROR(esp_lcd_new_panel_ili9341(*ret_io, &panel_config, ret_panel), err, TAG, "New panel failed");

  esp_lcd_panel_reset(*ret_panel);
  esp_lcd_panel_init(*ret_panel);
  esp_lcd_panel_invert_color(*ret_panel, true);
  return ret;

err:
  if (*ret_panel) {
    esp_lcd_panel_del(*ret_panel);
  }
  if (*ret_io) {
    esp_lcd_panel_io_del(*ret_io);
  }
  spi_bus_free(BSP_LCD_SPI_NUM);
  return ret;
}

esp_err_t M5CoreS3::touch_new(const touch_config_t *config, esp_lcd_touch_handle_t *ret_touch)
{
  BSP_ERROR_CHECK_RETURN_ERR(enable_feature(FEATURE_TOUCH));

  /* Initialize touch */
  const esp_lcd_touch_config_t tp_cfg = {
    .x_max = BSP_LCD_H_RES,
    .y_max = BSP_LCD_V_RES,
    .rst_gpio_num = GPIO_NUM_NC, // Shared with LCD reset
    .int_gpio_num = BSP_LCD_TOUCH_INT,
    .levels = {
      .reset = 0,
      .interrupt = 0,
    },
    .flags = {
      .swap_xy = 0,
      .mirror_x = 0,
      .mirror_y = 0,
    },
    .process_coordinates = NULL,
    .interrupt_callback = NULL,
    .user_data = NULL,
    .driver_data = NULL
  };
  esp_lcd_panel_io_handle_t tp_io_handle = NULL;
  esp_lcd_panel_io_i2c_config_t tp_io_config =     {
    .dev_addr = ESP_LCD_TOUCH_IO_I2C_FT5x06_ADDRESS,
    .on_color_trans_done = NULL,
    .user_ctx = NULL,
    .control_phase_bytes = 1,
    .dc_bit_offset = 0,
    .lcd_cmd_bits = 8,
    .lcd_param_bits = 0,
    .flags = { 
      .dc_low_on_data = 0,
      .disable_control_phase = 1,
    },
    .scl_speed_hz = CONFIG_BSP_I2C_CLK_SPEED_HZ // This parameter was introduced together with I2C Driver-NG in IDF v5.2
  };
  
  ESP_RETURN_ON_ERROR(esp_lcd_new_panel_io_i2c(i2c_handle, &tp_io_config, &tp_io_handle), TAG, "");
  return esp_lcd_touch_new_i2c_ft5x06(tp_io_handle, &tp_cfg, ret_touch);
}

lv_display_t *M5CoreS3::display_lcd_init(const display_cfg_t *cfg)
{
  assert(cfg != NULL);
  esp_lcd_panel_io_handle_t io_handle = NULL;
  esp_lcd_panel_handle_t panel_handle = NULL;
  const display_config_t bsp_disp_cfg = {
    .max_transfer_sz = BSP_LCD_DRAW_BUFF_SIZE * sizeof(uint16_t),
  };
  BSP_ERROR_CHECK_RETURN_NULL(display_new(&bsp_disp_cfg, &panel_handle, &io_handle));

  esp_lcd_panel_disp_on_off(panel_handle, true);

  /* Add LCD screen */
  ESP_LOGD(TAG, "Add LCD screen");
  const lvgl_port_display_cfg_t disp_cfg = {
    .io_handle = io_handle,
    .panel_handle = panel_handle,
    .control_handle = NULL,
    .buffer_size = cfg->buffer_size,
    .double_buffer = cfg->double_buffer,
    .hres = BSP_LCD_H_RES,
    .vres = BSP_LCD_V_RES,
    .monochrome = false,
    /* Rotation values must be same as used in esp_lcd for initial settings of the screen */
    .rotation = {
      .swap_xy = false,
      .mirror_x = false,
      .mirror_y = false,
    },
    .color_format = LV_COLOR_FORMAT_UNKNOWN,
    .flags = {
      .buff_dma = cfg->flags.buff_dma,
      .buff_spiram = false,
      .sw_rotate = 0,
#if LVGL_VERSION_MAJOR >= 9
      .swap_bytes = (BSP_LCD_BIGENDIAN ? true : false),
#endif
      .full_refresh = 0,
      .direct_mode = 0
    }
  };

  return lvgl_port_add_disp(&disp_cfg);
}

lv_indev_t *M5CoreS3::display_indev_init(lv_display_t *disp)
{
  BSP_ERROR_CHECK_RETURN_NULL(touch_new(NULL, &tp));
  assert(tp);

  /* Add touch input (for selected screen) */
  const lvgl_port_touch_cfg_t touch_cfg = {
    .disp = disp,
    .handle = tp,
  };

  return lvgl_port_add_touch(&touch_cfg);
}

lv_display_t *M5CoreS3::display_start(void)
{
  display_cfg_t cfg = {
    .lvgl_port_cfg = ESP_LVGL_PORT_INIT_CONFIG(),
    .buffer_size = BSP_LCD_DRAW_BUFF_SIZE,
    .double_buffer = BSP_LCD_DRAW_BUFF_DOUBLE,
    .flags = {
      .buff_dma = true,
      .buff_spiram = false,
    }
  };
  cfg.lvgl_port_cfg.task_affinity = 1; /* For camera */
  return display_start_with_config(&cfg);
}

lv_display_t *M5CoreS3::display_start_with_config(const display_cfg_t *cfg)
{
  assert(cfg != NULL);
  BSP_ERROR_CHECK_RETURN_NULL(lvgl_port_init(&cfg->lvgl_port_cfg));

  BSP_ERROR_CHECK_RETURN_NULL(display_brightness_init());

  BSP_NULL_CHECK(disp = display_lcd_init(cfg), NULL);

  BSP_NULL_CHECK(disp_indev = display_indev_init(disp), NULL);

  return disp;
}

lv_indev_t *M5CoreS3::display_get_input_dev(void)
{
  return disp_indev;
}

void M5CoreS3::display_rotate(lv_display_t *disp, lv_display_rotation_t rotation)
{
  lv_disp_set_rotation(disp, rotation);
}

bool M5CoreS3::display_lock(uint32_t timeout_ms)
{
  return lvgl_port_lock(timeout_ms);
}

void M5CoreS3::display_unlock(void)
{
  lvgl_port_unlock();
}

void M5CoreS3::display_buf_swap(uint16_t* buf, size_t w, size_t h)
{
  for (size_t i = 0; i < w * h; i++) {
    buf[i] = (buf[i] >> 8) | (buf[i] << 8);
  }
}

esp_err_t M5CoreS3::camera_init(framesize_t framesize)
{	
  //初始化互斥锁
  camera_lock = xSemaphoreCreateMutex();
  if (camera_lock == NULL) {
    ESP_LOGE(TAG, "Create camera lock failed");
    return ESP_FAIL;
  }
  // Initialize the camera
  camera_config_t camera_config = BSP_CAMERA_DEFAULT_CONFIG;
  camera_config.frame_size = framesize;
  esp_err_t err = esp_camera_init(&camera_config);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Camera Init Failed");
    return err;
  }
  sensor_t *s = esp_camera_sensor_get();
  s->set_vflip(s, BSP_CAMERA_VFLIP);
  s->set_hmirror(s, BSP_CAMERA_HMIRROR);
  ESP_LOGI(TAG, "Camera Init done");
  return ESP_OK;
}

esp_err_t M5CoreS3::camera_init()
{
  return camera_init(FRAMESIZE_QVGA);
}

esp_err_t M5CoreS3::camera_deinit()
{
  esp_err_t err = esp_camera_deinit();
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Camera Deinit Failed");
    return err;
  }
  vSemaphoreDelete(camera_lock);
  ESP_LOGI(TAG, "Camera Deinit done");
  return ESP_OK;
}

camera_fb_t* M5CoreS3::camera_fb_get()
{
  camera_fb_t* frame = NULL;

  if(camera_lock == NULL){
    ESP_LOGE(TAG, "Camera not initialized");
    return NULL;
  }

  if(xSemaphoreTake(camera_lock, 0)==pdFALSE){
    ESP_LOGE(TAG, "Camera lock failed");
    return NULL;
  }

  if(xTaskGetTickCount() - camera_last_frame_tick < (1000 / BSP_CAMERA_FPS_MAX)){
    ESP_LOGD(TAG, "Camera FPS too high");
    xSemaphoreGive(camera_lock);
    return NULL;
  }

  frame = esp_camera_fb_get();
  if (!frame) {
    ESP_LOGE(TAG, "Camera capture failed");
    xSemaphoreGive(camera_lock);
    return NULL;
  }

  camera_last_frame_tick = xTaskGetTickCount();
  xSemaphoreGive(camera_lock);
  return frame;
}

void M5CoreS3::camera_fb_return(camera_fb_t *frame)
{
  esp_camera_fb_return(frame);
}

esp_err_t M5CoreS3::audio_init(const i2s_std_config_t *i2s_config)
{
  esp_err_t ret = ESP_FAIL;
  if (i2s_tx_chan && i2s_rx_chan) {
      /* Audio was initialized before */
      return ESP_OK;
  }

  /* Setup I2S peripheral */
  i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG((i2s_port_t)CONFIG_BSP_I2S_NUM, I2S_ROLE_MASTER);
  chan_cfg.auto_clear = true; // Auto clear the legacy data in the DMA buffer
  BSP_ERROR_CHECK_RETURN_ERR(i2s_new_channel(&chan_cfg, &i2s_tx_chan, &i2s_rx_chan));

  /* Setup I2S channels */
  const i2s_std_config_t std_cfg_default = BSP_I2S_DUPLEX_MONO_CFG(22050);
  const i2s_std_config_t *p_i2s_cfg = &std_cfg_default;

  audio_codec_i2s_cfg_t i2s_cfg = {
      .port = CONFIG_BSP_I2S_NUM,
      .rx_handle = i2s_rx_chan,
      .tx_handle = i2s_tx_chan,
  };

  if (i2s_config != NULL) {
      p_i2s_cfg = i2s_config;
  }

  if (i2s_tx_chan != NULL) {
      ESP_GOTO_ON_ERROR(i2s_channel_init_std_mode(i2s_tx_chan, p_i2s_cfg), err, TAG, "I2S channel initialization failed");
      ESP_GOTO_ON_ERROR(i2s_channel_enable(i2s_tx_chan), err, TAG, "I2S enabling failed");
  }
  if (i2s_rx_chan != NULL) {
      ESP_GOTO_ON_ERROR(i2s_channel_init_std_mode(i2s_rx_chan, p_i2s_cfg), err, TAG, "I2S channel initialization failed");
      ESP_GOTO_ON_ERROR(i2s_channel_enable(i2s_rx_chan), err, TAG, "I2S enabling failed");
  }
  
  i2s_data_if = audio_codec_new_i2s_data(&i2s_cfg);
  BSP_NULL_CHECK_GOTO(i2s_data_if, err);

  return ESP_OK;

err:
  if (i2s_tx_chan) {
      i2s_del_channel(i2s_tx_chan);
  }
  if (i2s_rx_chan) {
      i2s_del_channel(i2s_rx_chan);
  }

  return ret;
}

const audio_codec_data_if_t* M5CoreS3::audio_get_codec_itf(void)
{
    return i2s_data_if;
}