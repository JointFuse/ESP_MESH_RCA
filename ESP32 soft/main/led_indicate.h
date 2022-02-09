/*
 *	    Developer:  Mayskiy Alexandr Igorevich
 *	         mail:  JointFuse@yandex.ru
 *  First version:  26.10.2021
 */
#ifndef led_indicate_h
#define led_indicate_h

#include "freertos\FreeRTOS.h"
#include "freertos\timers.h"
#include "esp_log.h"
#include "esp_err.h"
#include "driver\gpio.h"
#include "project_defs.h"

extern esp_err_t led_indicate_init();
extern void led_indicate_red_blink();
extern void led_indicate_green_blink();
extern void led_indicate_red();
extern void led_indicate_green();

#endif