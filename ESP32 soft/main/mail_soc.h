/*
 *	    Developer:  Mayskiy Alexandr Igorevich
 *	         mail:  JointFuse@yandex.ru
 *  First version:  18.10.2021
 */
#ifndef mail_soc_h
#define mail_soc_h

#include "freertos\FreeRTOS.h"
#include "freertos\task.h"
#include "freertos\event_groups.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_err.h"
#include "message_queue.h"
#include "myesp_mesh.h"
#include "project_defs.h"

static EventGroupHandle_t mail_soc_event_group;
static const int MAIL_SOC_RUN = BIT0;
static const int MAIL_SOC_STOP = BIT1;
static const int MAIL_SOC_INIT = BIT2;
static const int MAIL_SOC_ON = BIT0 | BIT2;

extern esp_err_t mail_socket_init();
extern esp_err_t mail_socket_start();
extern void mail_socket_stop();

#endif