/*
 *	    Developer:  Mayskiy Alexandr Igorevich
 *	         mail:  JointFuse@yandex.ru
 *  First version:  26.10.2021
 */
#ifndef mcpwm_module_h
#define mcpwm_module_h

#include "freertos\FreeRTOS.h"
#include "freertos\task.h"
#include "freertos\timers.h"
#include "esp_log.h"
#include "esp_err.h"
#include "driver\mcpwm.h"
#include "soc\mcpwm_periph.h"
#include "project_defs.h"

extern esp_err_t mcpwm_module_start();
extern esp_err_t mcpwm_module_init();
extern void mcpwm_module_push();
extern void mcpwm_module_set_duty(int duty);

#endif