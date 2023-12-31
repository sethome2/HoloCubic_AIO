#ifndef APP_CLOCK_H
#define APP_CLOCK_H

#include "lvgl.h"
#include "sys/app_controller.h"
#include "common.h"
#include "time.hpp"
#include "driver/rgb_led.h"

#define CLOCK_APP_NAME "clock"

extern const lv_img_dsc_t clock_ico;

static lv_obj_t *main_scr = NULL;
static lv_obj_t *time_label = NULL;
static lv_obj_t *date_label = NULL;
static lv_obj_t *weekday_label = NULL;

static lv_style_t scr_style;
static lv_style_t time_text_style;
static lv_style_t date_text_style;

static int clock_init(AppController *sys)
{
    // 初始化GUI
    lv_obj_t *act_obj = lv_scr_act(); // 获取当前活动页
    lv_obj_clean(act_obj);            // 清空此前页面

    lv_style_init(&scr_style);
    lv_style_set_bg_color(&scr_style, lv_color_hex(0x000000));

    lv_style_init(&time_text_style);
    lv_style_set_text_font(&time_text_style, &lv_font_montserrat_48);
    lv_style_set_text_color(&time_text_style, lv_color_white());
    lv_style_set_text_letter_space(&time_text_style, 2);
    lv_style_set_text_line_space(&time_text_style, 2);
    lv_style_set_text_opa(&time_text_style, LV_OPA_COVER);
    lv_style_set_text_align(&time_text_style, LV_TEXT_ALIGN_CENTER);

    lv_style_init(&date_text_style);
    lv_style_set_text_font(&date_text_style, &lv_font_montserrat_28);
    lv_style_set_text_color(&date_text_style, lv_color_white());
    lv_style_set_text_letter_space(&date_text_style, 2);
    lv_style_set_text_line_space(&date_text_style, 2);
    lv_style_set_text_opa(&date_text_style, LV_OPA_COVER);
    lv_style_set_text_align(&date_text_style, LV_TEXT_ALIGN_CENTER);

    if (NULL != time_label)
    {
        lv_obj_del(time_label);
        time_label = NULL;
    }
    if (NULL != main_scr)
    {
        lv_obj_del(main_scr);
        main_scr = NULL;
    }

    main_scr = lv_obj_create(NULL);
    lv_obj_add_style(main_scr, &scr_style, LV_STATE_DEFAULT);

    time_label = lv_label_create(main_scr);
    lv_obj_add_style(time_label, &time_text_style, LV_STATE_DEFAULT);
    lv_obj_align(time_label, LV_ALIGN_CENTER, 0, -25);
    lv_label_set_recolor(time_label, true); // 先得使能文本重绘色功能

    date_label = lv_label_create(main_scr);
    lv_obj_add_style(date_label, &date_text_style, LV_STATE_DEFAULT);
    lv_obj_align(date_label, LV_ALIGN_CENTER, 0, 25);
    lv_label_set_recolor(date_label, true); // 先得使能文本重绘色功能

    weekday_label = lv_label_create(main_scr);
    lv_obj_add_style(weekday_label, &date_text_style, LV_STATE_DEFAULT);
    lv_obj_align(weekday_label, LV_ALIGN_CENTER, 0, 65);
    lv_label_set_recolor(weekday_label, true); // 先得使能文本重绘色功能

    lv_scr_load(main_scr);

    sys->send_to(CLOCK_APP_NAME, CTRL_NAME,
                 APP_MESSAGE_WIFI_CONN, NULL, NULL);
    
    rgb_stop();
    rgb.setBrightness(0.08);
    return 0;
}

static void clock_process(AppController *sys,
                          const ImuAction *act_info)
{
    static uint8_t last_sec = 0;

    if (RETURN == act_info->active)
    {
        sys->app_exit(); // 退出APP
        return;
    }

    lv_obj_t *act_obj = lv_scr_act(); // 获取当前活动页

    sys->send_to(CLOCK_APP_NAME, CTRL_NAME,
                 APP_MESSAGE_WIFI_ALIVE, NULL, NULL);

    tm now = SystemTime::instance().time();
    char time_str[32] = {0};
    sprintf(time_str, "%02d:%02d:%02d", now.tm_hour, now.tm_min, now.tm_sec);
    lv_label_set_text(time_label, time_str);

    char date_str[32] = {0};
    sprintf(date_str, "20%2d-%02d-%02d", now.tm_year - 100, now.tm_mon + 1, now.tm_mday);
    lv_label_set_text(date_label, date_str);

    char weekday_str[32] = {0};
    switch (now.tm_wday)
    {
    case 0:
        sprintf(weekday_str, "Sunday");
        break;
    case 1:
        sprintf(weekday_str, "Monday");
        break;
    case 2:
        sprintf(weekday_str, "Tuesday");
        break;
    case 3:
        sprintf(weekday_str, "Wednesday");
        break;
    case 4:
        sprintf(weekday_str, "Thursday");
        break;
    case 5:
        sprintf(weekday_str, "Friday");
        break;
    case 6:
        sprintf(weekday_str, "Saturday");
        break;
    default:
        break;
    }
    lv_label_set_text(weekday_label, weekday_str);

    if (last_sec != now.tm_sec)
    {
        last_sec = now.tm_sec;

        // 闪烁LED
        if (last_sec % 2 == 0)
            rgb.setRGB(255, 255, 255);
        else
            rgb.setRGB(0, 0, 0);
    }
}

static void clock_background_task(AppController *sys,
                                  const ImuAction *act_info)
{
}

static int clock_exit_callback(void *param)
{
    // 释放资源
    if (NULL != time_label)
    {
        lv_obj_del(time_label);
        time_label = NULL;
    }
    if (NULL != date_label)
    {
        lv_obj_del(date_label);
        date_label = NULL;
    }
    if (NULL != weekday_label)
    {
        lv_obj_del(weekday_label);
        weekday_label = NULL;
    }
    if (NULL != main_scr)
    {
        lv_obj_del(main_scr);
        main_scr = NULL;
    }
    return 0;
}

static void clock_message_handle(const char *from, const char *to,
                                 APP_MESSAGE_TYPE type, void *message,
                                 void *ext_info)
{
    // 目前主要是wifi开关类事件（用于功耗控制）
    switch (type)
    {
    case APP_MESSAGE_WIFI_CONN:
    {
        // todo
    }
    break;
    case APP_MESSAGE_WIFI_AP:
    {
        // todo
    }
    break;
    case APP_MESSAGE_WIFI_ALIVE:
    {
        // wifi心跳维持的响应 可以不做任何处理
    }
    break;
    case APP_MESSAGE_GET_PARAM:
    {
        char *param_key = (char *)message;
    }
    break;
    case APP_MESSAGE_SET_PARAM:
    {
        char *param_key = (char *)message;
        char *param_val = (char *)ext_info;
    }
    break;
    default:
        break;
    }
}

APP_OBJ clock_app = {CLOCK_APP_NAME, &clock_ico, "Author sethome\nVersion 1.0.0\n",
                     clock_init, clock_process, clock_background_task,
                     clock_exit_callback, clock_message_handle};

#endif