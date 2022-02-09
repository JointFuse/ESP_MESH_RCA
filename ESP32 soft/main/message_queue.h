/*
 *	    Developer:  Mayskiy Alexandr Igorevich
 *	         mail:  JointFuse@yandex.ru
 *  First version:  18.10.2021
 */
#ifndef message_queue_h
#define message_queue_h

#include "freertos\FreeRTOS.h"
#include "freertos\queue.h"
#include "esp_err.h"
#include "project_defs.h"

extern esp_err_t message_queue_init();
extern esp_err_t receive_queue_get(InMessage_t* buffer);
extern esp_err_t receive_queue_add(InMessage_t* message);
extern esp_err_t send_queue_get(OutMessage_t* buffer);
extern esp_err_t send_queue_add(OutMessage_t* message, int count);

#endif