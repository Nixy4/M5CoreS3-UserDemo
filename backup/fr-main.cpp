#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_camera.h"
#include "lvgl.h"
#include "dl_image.hpp"
#include "fb_gfx.h"
#include "human_face_detect_msr01.hpp"
#include "human_face_detect_mnp01.hpp"
#include "face_recognition_tool.hpp"
#include "bsp/esp-bsp.h"
#include "fr-main.hpp"

#if CONFIG_MFN_V1
#if CONFIG_S8
#include "face_recognition_112_v1_s8.hpp"
#elif CONFIG_S16
#include "face_recognition_112_v1_s16.hpp"
#endif
#endif

#include "who_ai_utils.hpp"
// #include "fr-user.h"

using namespace std;
using namespace dl;

static const char* TAG = "FR-MAIN";

#define FR_RECOG_OUT_HOR_RES 240   //模型输出图像水平分辨率
#define FR_RECOG_OUT_VER_RES 240   //模型输出图像垂直分辨率
#define FR_RECOG_INTERVAL    1000  //识别间隔

#define FR_PAGE_BTN_WIDTH  192 //手动计算的 按钮宽度 lv_pct(80), 使用lv_pct设置对象宽度, 无法获取到对应的值
#define FR_PAGE_BTN_HEITHT 28  //lv_pct(15)

enum page_id
{
  PAGE_ID_BOOT = 0,
  PAGE_ID_RECOG = 1,
  PAGE_ID_SETTING,
  PAGE_ID_LIST,
  PAGE_ID_HTTP,
  PAGE_ID_ENROLL,
  PAGE_ID_INFO,
};

LV_FONT_DECLARE(msyh14);

HumanFaceDetectMSR01 detector(0.3F,0.3F,10,0.3F);
HumanFaceDetectMNP01 detector2(0.4F,0.3F,10);
FaceRecognition112V1S8 recognizer;

static QueueHandle_t osque_frame_out = NULL;
static QueueHandle_t osque_http_req  = NULL;
static QueueHandle_t osque_bep       = NULL;

static lv_style_t lvs_default;

// static fr_user_t user_buf0;

lv_obj_t* lvo_cam;
uint8_t *cam_buff;
const uint32_t cam_buff_size = FR_RECOG_OUT_HOR_RES * FR_RECOG_OUT_VER_RES * 2;

lv_obj_t* lvl_recog_info;
//设置GUI人脸画面的LVGL图像转换器
lv_img_dsc_t lvid_cam = {
  .header = {
    .cf = LV_IMG_CF_TRUE_COLOR,
    .always_zero = 0,
    .reserved = 0,
    .w = FR_RECOG_OUT_HOR_RES,
    .h = FR_RECOG_OUT_VER_RES
  },
  .data_size = FR_RECOG_OUT_HOR_RES * FR_RECOG_OUT_VER_RES * sizeof(uint16_t),
  .data = NULL
};

static lv_obj_t* lv_create_page_base()
{
  lv_obj_t* page = lv_obj_create(NULL);
  lv_obj_set_size(page, LV_HOR_RES, LV_VER_RES);
  lv_obj_set_style_pad_top(page, 10, 0);
  lv_obj_set_style_pad_bottom(page, 10, 0);
  lv_obj_set_style_pad_left(page, 10, 0);
  lv_obj_set_style_pad_right(page, 10, 0);
  return page;
}

static lv_obj_t* lv_create_page_recog();
// static lv_obj_t* lv_create_page_setting();
// static lv_obj_t* lv_create_page_userList();
// static lv_obj_t* lv_create_page_http();
// static lv_obj_t* lv_create_page_faceAdd();
// static lv_obj_t* lv_create_page_userInfo();

static void page_load(int id)
{
  switch(id)
  {
    case PAGE_ID_RECOG  : lv_scr_load_anim(lv_create_page_recog(), LV_SCR_LOAD_ANIM_FADE_OUT, 250, 0, true);break;
    // case PAGE_ID_SETTING: lv_scr_load_anim(lv_create_page_setting(), LV_SCR_LOAD_ANIM_FADE_OUT, 250, 0, true);break;
    // case PAGE_ID_LIST   : lv_scr_load_anim(lv_create_page_userList(), LV_SCR_LOAD_ANIM_FADE_OUT, 250, 0, true);break;
    // case PAGE_ID_HTTP   : lv_scr_load_anim(lv_create_page_http(), LV_SCR_LOAD_ANIM_FADE_OUT, 250, 0, true);break;
    // case PAGE_ID_ENROLL : lv_scr_load_anim(lv_create_page_faceAdd(), LV_SCR_LOAD_ANIM_FADE_OUT, 250, 0, true);break;
    // case PAGE_ID_INFO   : lv_scr_load_anim(lv_create_page_userInfo(), LV_SCR_LOAD_ANIM_FADE_OUT, 250, 0, true);break;
         default        : break;
  }
  xQueueSend(osque_bep, &id, portMAX_DELAY);
  ESP_LOGI(TAG, "加载页面: %d", id);
}

// static void lv_btn_event_cb_page_recog_back(lv_event_t* event)
// {
//   page_load(PAGE_ID_SETTING);
// }

// static void lv_btn_event_cb_page_setting_faceAdd(lv_event_t* event)
// {
//   page_load(PAGE_ID_HTTP);
// }

// static void lv_btn_event_cb_page_setting_faceDel(lv_event_t* event)
// {
//   int del_index = recognizer.get_enrolled_id_num()-1;
//   int face_num = recognizer.get_enrolled_id_num();

//   if(face_num==0) {
//     ESP_LOGW("DELETE", "没有人脸数据可删除");
//   } else {
//     recognizer.delete_id(face_num,true);
//     ESP_LOGW("DELETE", "已删除ID:%d, 剩余人脸数:%d", face_num, recognizer.get_enrolled_id_num() );
//     // fr_user_delete(del_index);
//     // fr_user_save();
//   }
// }

// static void lv_btn_event_cb_page_setting_userList(lv_event_t* event)
// {
//   page_load(PAGE_ID_LIST);
// }

// static void lv_btn_event_cb_page_http_back(lv_event_t* event)
// {
//   page_load(PAGE_ID_SETTING);
// }

// static void lv_btn_event_cb_page_setting_back(lv_event_t* event)
// {
//   page_load(PAGE_ID_RECOG);
// }

// static void lv_btn_event_cb_page_faceAdd_back(lv_event_t * e)
// {
//   page_load(PAGE_ID_SETTING);
// }

// static void lv_btn_event_cb_page_userList_table(lv_event_t * e) 
// {
//   lv_obj_t * table = lv_event_get_target(e);
//   uint16_t row, col;
//   lv_table_get_selected_cell(table, &row, &col);
//   fr_user_t* temp = (fr_user_t*)lv_table_get_cell_user_data(table, row, col);
//   if(temp==NULL) return;
//   memcpy(&user_buf0, temp, sizeof(fr_user_t));
//   page_load(PAGE_ID_INFO);
// }

// static void lv_btn_event_cb_page_userList_back(lv_event_t* event)
// {
//   page_load(PAGE_ID_SETTING);
// }

// static void lv_btn_event_cb_page_userInfo_back(lv_event_t* e)
// {
//   page_load(PAGE_ID_LIST);
// }

static lv_obj_t* lv_create_page_recog()
{
  lv_obj_t* page = lv_obj_create(NULL);
  lv_obj_set_size(page, LV_HOR_RES, LV_VER_RES);

  lv_obj_set_style_pad_top(page, 10, 0);
  lv_obj_set_style_pad_bottom(page, 10, 0);
  lv_obj_set_style_pad_left(page, 10, 0);
  lv_obj_set_style_pad_right(page, 10, 0);

	lvo_cam = lv_canvas_create(page);
  lv_canvas_set_buffer(lvo_cam, cam_buff, BSP_LCD_H_RES, BSP_LCD_V_RES, LV_COLOR_FORMAT_RGB565);
  assert(lvo_cam);
  lv_obj_align(lvo_cam, LV_ALIGN_TOP_MID, 0, 0);


  // lvl_recog_info = lv_label_create(page);
  // lv_obj_set_size(lvl_recog_info, lv_pct(80), lv_pct(10));
  // lv_label_set_text_fmt(lvl_recog_info, "[ID]  %-2s  [姓名]  %-5s", "??", "?????");
  // lv_obj_align_to(lvl_recog_info, lvo_cam, LV_ALIGN_OUT_BOTTOM_MID, 0, 10); // 显示到 lvo_cam 正下方
  // lv_obj_add_style(lvl_recog_info, &lvs_default, 0);

  lv_obj_t* btn = lv_btn_create(page);
  lv_obj_set_size(btn, lv_pct(80), lv_pct(15));
  lv_obj_align(btn, LV_ALIGN_BOTTOM_MID, 0, 0);
  lv_obj_add_style(btn, &lvs_default, 0);
  // lv_obj_add_event_cb(btn, lv_btn_event_cb_page_recog_back, LV_EVENT_CLICKED, NULL);
  lv_obj_t* label_btn = lv_label_create(btn);
  lv_label_set_text(label_btn, "setting");
  lv_obj_center(label_btn);

  return page;
}

// static lv_obj_t* lv_create_page_setting()
// {
//   bsp_display_lock(0);//!

//   lv_obj_t* page = lv_obj_create(NULL);
//   lv_obj_set_size(page, LV_HOR_RES, LV_VER_RES);
//   lv_obj_set_style_pad_top(page, 10, 0);
//   lv_obj_set_style_pad_bottom(page, 10, 0);
//   lv_obj_set_style_pad_left(page, 10, 0);
//   lv_obj_set_style_pad_right(page, 10, 0);

//   lv_obj_t* btn;
//   lv_obj_t* label;

//   btn = lv_btn_create(page);
//   lv_obj_add_style(btn, &lvs_default, 0);
//   lv_obj_set_size(btn, lv_pct(80), lv_pct(15));
//   lv_obj_add_event_cb(btn, lv_btn_event_cb_page_setting_faceAdd, LV_EVENT_CLICKED, NULL);
//   lv_obj_align(btn, LV_ALIGN_TOP_MID, 0, 0);
//   label = lv_label_create(btn);
//   lv_label_set_text(label, "添加用户");
//   lv_obj_center(label);
  
//   btn = lv_btn_create(page);
//   lv_obj_add_style(btn, &lvs_default, 0);
//   lv_obj_set_size(btn, lv_pct(80), lv_pct(15));
//   lv_obj_add_event_cb(btn, lv_btn_event_cb_page_setting_faceDel, LV_EVENT_CLICKED, NULL);
//   lv_obj_align_to(btn, lv_obj_get_child(page, 0), LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
//   label = lv_label_create(btn);
//   lv_label_set_text(label, "删除用户");
//   lv_obj_center(label);
  
//   btn = lv_btn_create(page);
//   lv_obj_add_style(btn, &lvs_default, 0);
//   lv_obj_set_size(btn, lv_pct(80), lv_pct(15));
//   lv_obj_add_event_cb(btn, lv_btn_event_cb_page_setting_userList, LV_EVENT_CLICKED, NULL);
//   lv_obj_align_to(btn, lv_obj_get_child(page, 1), LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
//   label = lv_label_create(btn);
//   lv_label_set_text(label, "用户列表");
//   lv_obj_center(label);
  
//   btn = lv_btn_create(page);
//   lv_obj_add_style(btn, &lvs_default, 0);
//   lv_obj_set_size(btn, lv_pct(80), lv_pct(15));
//   lv_obj_add_event_cb(btn, lv_btn_event_cb_page_setting_back, LV_EVENT_CLICKED, NULL);
//   lv_obj_align(btn, LV_ALIGN_BOTTOM_MID, 0, 0);
//   label = lv_label_create(btn);
//   lv_label_set_text(label, "返回");
//   lv_obj_center(label);
  
//   bsp_display_unlock();//!
//   return page;
// }

// static lv_obj_t* lv_create_page_http()
// {
//   bsp_display_lock(0);//!

// 	//创建-屏幕
// 	lv_obj_t* page = lv_create_page_base();

//   //创建-功能容器
//   lv_obj_t* cont = lv_obj_create(page);
//   lv_obj_set_size(cont, lv_pct(80), lv_pct(80));
//   lv_obj_align(cont, LV_ALIGN_TOP_MID, 0, 0);
//   //创建-旋转器
// 	lv_obj_t * spinner = lv_spinner_create(cont, 500, 100);
// 	lv_obj_set_size(spinner, 100, 100);
// 	lv_obj_center(spinner);
//   //创建-标签
//   lv_obj_t* label = lv_label_create(page);
//   lv_obj_add_style(label, &lvs_default, 0);
//   lv_label_set_text(label, "等待手机端信息...");
//   lv_obj_align_to(label, spinner, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

//   //创建-返回按钮
//   lv_obj_t* btn = lv_btn_create(page);
//   lv_obj_add_style(btn, &lvs_default, 0);
//   lv_obj_set_size(btn, lv_pct(80), lv_pct(15));
//   lv_obj_add_event_cb(btn, lv_btn_event_cb_page_setting_back, LV_EVENT_CLICKED, NULL);
//   lv_obj_align(btn, LV_ALIGN_BOTTOM_MID, 0, 0);
//   label = lv_label_create(btn);
//   lv_label_set_text(label, "返回");
//   lv_obj_center(label);
  
//   bsp_display_unlock();//!
//   return page;  
// }

// static lv_obj_t* lv_create_page_faceAdd()
// {
//   bsp_display_lock(0);//!

// 	//创建屏幕
// 	lv_obj_t* page = lv_obj_create(NULL);
// 	lv_obj_set_size(page, LV_HOR_RES, LV_VER_RES);

// 	//设置-屏幕布局
// 	lv_obj_set_flex_flow(page, LV_FLEX_FLOW_COLUMN);
// 	lv_obj_set_flex_align(page, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
//   //设置边距
//   lv_obj_set_style_pad_top(page, 10, 0);
//   lv_obj_set_style_pad_bottom(page, 10, 0);
//   lv_obj_set_style_pad_left(page, 10, 0);
//   lv_obj_set_style_pad_right(page, 10, 0);

//   //创建摄像头画面和图像转换器
// 	lvo_cam = lv_img_create(page);
// 	lv_img_set_src(lvo_cam, &lvid_cam);

//   //设置宽度和高度
// 	lv_obj_set_width(lvo_cam, lv_pct(80));
//   lv_obj_set_height(lvo_cam, lv_pct(80));
//   //设置弧度
// 	lv_obj_add_style(lvo_cam, &lvs_default, 0);

// 	lv_obj_t* lvb_info = lv_btn_create(page);
//   lv_obj_set_width(lvb_info, lv_pct(80));
// 	lv_obj_t* lvl_info = lv_label_create(lvb_info);
//   lv_obj_add_style(lvl_info, &lvs_default, 0);
// 	lv_label_set_text(lvl_info, "录入中...");
// 	lv_obj_align(lvl_info, LV_ALIGN_CENTER, 0, 0);

//   bsp_display_unlock();//!
//   return page;
// }

// static lv_obj_t* lv_create_page_userList()
// {
// 	bsp_display_lock(0);//!

//   lv_obj_t* page = lv_obj_create(NULL);
//   lv_obj_set_size(page, LV_HOR_RES, LV_VER_RES);
//   lv_obj_set_style_pad_top(page, 10, 0);
//   lv_obj_set_style_pad_bottom(page, 10, 0);
//   lv_obj_set_style_pad_left(page, 10, 0);
//   lv_obj_set_style_pad_right(page, 10, 0);

//   int cnt = fr_user_get_cnt();
//   int table_row = cnt+1;

//   const lv_coord_t table_w =  LV_HOR_RES*0.8-20;
//   const lv_coord_t table_h = LV_VER_RES*0.8-20;
//   const lv_coord_t col0_w = 42;
//   const lv_coord_t col1_w = table_w-col0_w-5;
  
//   lv_obj_t* table = lv_table_create(page);
//   lv_obj_add_style(table, &lvs_default, 0);
//   lv_obj_set_size(table,table_w,table_h);
//   lv_obj_align(table, LV_ALIGN_TOP_MID, 0, 0);
//   lv_obj_add_event_cb(table, lv_btn_event_cb_page_userList_table, LV_EVENT_LONG_PRESSED, NULL);
//   // lv_obj_set_size(table, lv_pct(80), lv_pct(80)); //如果使用百分比设置大小,则无法设置col宽度
//   /**
//    * lv_pct(80) == 8272
//    * lv_obj_get_width(table) == 0
//    */
//   lv_table_set_col_cnt(table, 2);
//   lv_table_set_row_cnt(table, FR_USER_MAX);
//   lv_table_set_cell_value(table, 0, 0, "ID");
//   lv_table_set_cell_value(table, 0, 1, "姓名");
//   lv_table_set_col_width(table, 0, col0_w);
//   lv_table_set_col_width(table, 1, col1_w);

//   char id_buf[4];
//   int data_index = 0;
//   for(int table_index = 1; table_index < table_row; table_index++)
//   {
//     data_index = table_index-1;
//     sprintf(id_buf, "%d", fr_user_get_face_id(data_index));
//     lv_table_set_cell_value(table, table_index, 0, id_buf);
//     lv_table_set_cell_value(table, table_index, 1, fr_user_get_name(data_index));
//     fr_user_t* temp = (fr_user_t*)lv_mem_alloc(sizeof(fr_user_t));
//     memcpy(temp, fr_user_get_ptr(data_index), sizeof(fr_user_t));
//     lv_table_set_cell_user_data(table, table_index, 1, temp);
//   }

//   lv_obj_t* btn = lv_btn_create(page);
//   lv_obj_set_size(btn, lv_pct(80), lv_pct(15));
//   lv_obj_add_style(btn, &lvs_default, 0);
//   lv_obj_add_event_cb(btn, lv_btn_event_cb_page_userList_back, LV_EVENT_CLICKED, NULL);
//   lv_obj_align(btn, LV_ALIGN_BOTTOM_MID, 0, 0);
//   lv_obj_t* label_btn = lv_label_create(btn);
//   lv_label_set_text(label_btn, "返回");
//   lv_obj_add_style(label_btn, &lvs_default, 0);
//   lv_obj_center(label_btn);

//   bsp_display_unlock();//!
//   return page;
// }

// static lv_obj_t* lv_create_page_userInfo()
// {
//   bsp_display_lock(0);//!

//   lv_obj_t* page = lv_create_page_base();
//   //创建-信息标签
//   lv_obj_t* label0;
//   lv_obj_t* label1;
//   lv_obj_t* btn;

//   lv_obj_t* list = lv_list_create(page);
//   lv_obj_add_style(list, &lvs_default, 0);
//   lv_obj_set_size(list, lv_pct(80), lv_pct(80));
//   lv_obj_align(list, LV_ALIGN_TOP_MID, 0, 0);

//   char buf[64];
//   sprintf(buf, "姓名: %s", user_buf0.name);
//   lv_list_add_btn(list, NULL, buf);
//   sprintf(buf, "性别: %s", user_buf0.gender);
//   lv_list_add_btn(list, NULL, buf);
//   sprintf(buf, "年龄: %d", user_buf0.age);
//   lv_list_add_btn(list, NULL, buf);
//   sprintf(buf, "工号: %s", user_buf0.employee_id);
//   lv_list_add_btn(list, NULL, buf);
//   sprintf(buf, "职位: %s", user_buf0.position);
//   lv_list_add_btn(list, NULL, buf);

//   //返回按钮
//   btn = lv_btn_create(page);
//   lv_obj_add_style(btn, &lvs_default, 0);
//   lv_obj_set_size(btn, lv_pct(80), lv_pct(15));
//   lv_obj_add_event_cb(btn, lv_btn_event_cb_page_userInfo_back, LV_EVENT_CLICKED, NULL);
//   lv_obj_align(btn, LV_ALIGN_BOTTOM_MID, 0, 0);
//   label1 = lv_label_create(btn);
//   lv_label_set_text(label1, "返回");
//   lv_obj_center(label1);

//   bsp_display_unlock();//!
//   return page;
// }

static void bep_recog()
{
  static TickType_t recog_last_tick = 0;

  //*采集摄像头数据
  camera_fb_t* frame = NULL;
  frame = esp_camera_fb_get(); //!
  if(frame==NULL) {
    vTaskDelay(1);
    return;
  }

  //*人脸检测
  bsp_display_lock(0);//!
  //一级人脸检测
  std::list<dl::detect::result_t> &detect_candidates = detector.infer(
    (uint16_t *)frame->buf, {(int)frame->height, (int)frame->width, 3});
  //二级人脸检测
  std::list<dl::detect::result_t> &detect_results = detector2.infer(
    (uint16_t *)frame->buf, {(int)frame->height, (int)frame->width, 3}, detect_candidates);
    
  if(detect_results.size())
  {
    //绘制检测结果
    draw_detection_result((uint16_t *)frame->buf, frame->height, frame->width, detect_results);
    
    if(xTaskGetTickCount()-recog_last_tick>FR_RECOG_INTERVAL) //识别间隔大于1秒钟才��别
    {
      //识别图像,获取结果
      face_info_t recognize_result = recognizer.recognize(
        (uint16_t *)frame->buf, {(int)frame->height, (int)frame->width, 3}, detect_results.front().keypoint);
      //分析识别结果
      if (recognize_result.id > 0) {
        ESP_LOGI("RECOGNIZE", "相似度: %f, 匹配ID: %d", recognize_result.similarity, recognize_result.id);
        //更新GUI信息
        const char* name = recognizer.get_enrolled_ids().at(recognize_result.id-1).name.c_str();
        lv_label_set_text_fmt(lvl_recog_info, "[ID]  %-2d  [Name]  %-5s", recognize_result.id, name);
      } else {
        ESP_LOGE("RECOGNIZE", "相似度: %f, 匹配ID: %d", recognize_result.similarity, recognize_result.id);
        //更新GUI信息
        lv_label_set_text_fmt(lvl_recog_info, "[ID]  %-2s  [Name]  %-5s", "??", "?????");
      }
      ///更新最后识别时间
      recog_last_tick = xTaskGetTickCount();    
    }
  }
  memcpy((void*)cam_buff, frame->buf, frame->len);
  lv_draw_sw_rgb565_swap(cam_buff, cam_buff_size);
  if(osque_frame_out){
    xQueueSend(osque_frame_out, &frame, 0);
  }else{
    esp_camera_fb_return(frame);//!
  }
  bsp_display_unlock();//!
  vTaskDelay(1);
}

// static void bep_http()
// {
//   if(osque_http_req) {
//     if( xQueueReceive(osque_http_req, &user_buf0, 0)==pdTRUE ) {
// 			ESP_LOGI(TAG, "接收到数据:[name:%s][age:%d][gender:%s][employee_id:%s][position:%s]", 
// 				user_buf0.name, user_buf0.age, user_buf0.gender, user_buf0.employee_id, user_buf0.position);
//       page_load(PAGE_ID_ENROLL);
//     }
//   }
// }

// static void bep_faceAdd()
// {
//   static uint8_t frame_cnt = 0;
//   //*采集摄像头数据
//   camera_fb_t* frame = NULL;
//   frame = esp_camera_fb_get(); //!

//   if(frame==NULL) {
//     vTaskDelay(1);
//     return;
//   }

//   //*人脸录入
//   bsp_display_lock(0);//!
//   //一级人脸检测
//   std::list<dl::detect::result_t> &detect_candidates = detector.infer(
//     (uint16_t *)frame->buf, {(int)frame->height, (int)frame->width, 3});
//   //二级人脸检测
//   std::list<dl::detect::result_t> &detect_results = detector2.infer(
//     (uint16_t *)frame->buf, {(int)frame->height, (int)frame->width, 3}, detect_candidates);

//   if(detect_results.size())
//   {
//     //绘制检测结果
//     draw_detection_result((uint16_t *)frame->buf, frame->height, frame->width, detect_results);
//     if(frame_cnt>30)
//     {
//       //录入到dl框架
//       int id = recognizer.enroll_id(
//         (uint16_t *)frame->buf, {(int)frame->height, (int)frame->width, 3}, detect_results.front().keypoint, user_buf0.name, true);

//       if(id>0)
//       {
//         ESP_LOGW("ENROLL", "ID:%d 已录入, 已注册ID总数:%d", id, recognizer.get_enrolled_id_num());
//         user_buf0.face_id = id;
//         fr_user_register(&user_buf0);
//         fr_user_print();
//         fr_user_save();
//         memset(&user_buf0, 0, sizeof(fr_user_t));  
//         page_load(PAGE_ID_SETTING);
//       }
//       else
//       {

//       }
//       frame_cnt=0;
//     }
//     else
//     {
//       frame_cnt++;
//     }
//   }

//   memcpy((void*)cam_buff, frame->buf, cam_buff_size);
//   if(osque_frame_out){
//     xQueueSend(osque_frame_out, &frame, 0);
//   }else{
//     esp_camera_fb_return(frame);//!
//   }
//   lv_img_set_src(lvo_cam, &lvid_cam);

//   bsp_display_unlock();//!
//   vTaskDelay(1);
// }

static inline void bsp_handler()
{
  static int bep = 0;
  int temp = 0;

  if(xQueueReceive(osque_bep, &temp, 0)==pdTRUE) {
    bep = temp; 
  }
  
  switch(bep)
  {
    case PAGE_ID_RECOG : bep_recog();     vTaskDelay(1);break;
    // case PAGE_ID_HTTP  : bep_http();      vTaskDelay(1);break;
    // case PAGE_ID_ENROLL: bep_faceAdd();   vTaskDelay(1);break;
          default       :                  vTaskDelay(1);break;
  }
}

void ostk_fr_main(void *arg)
{
  while(1){
    bsp_handler();  
  }
}

void fr_main_init(QueueHandle_t frame_out, QueueHandle_t http_req)
{
  //*创建队列
  osque_frame_out = frame_out;
  osque_http_req = http_req;
  // assert(osque_frame_out!=NULL);
  // assert(osque_http_req!=NULL);

  //*创建OS相关对象
  osque_bep = xQueueCreate(1, sizeof(int));
  assert(osque_bep!=NULL);

  //*LVGL缓冲区和页面初始化
  cam_buff = (uint8_t *)heap_caps_malloc(cam_buff_size, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
  assert(cam_buff!=NULL);
	//Style初始化
	lv_style_init(&lvs_default);
	lv_style_set_radius(&lvs_default, 5);
  //设置字体
  lv_style_set_text_font(&lvs_default, &lv_font_montserrat_12);

  //*AI识别器初始化
  //设置人脸识别数据存储段落
  recognizer.set_partition(ESP_PARTITION_TYPE_DATA,ESP_PARTITION_SUBTYPE_ANY,"fr");
  //从 Flash 中读取已注册的人脸特征
  recognizer.set_ids_from_flash();  

  //*加载���面
  page_load(PAGE_ID_RECOG);
}