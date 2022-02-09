/*
 *	    Developer:  Mayskiy Alexandr Igorevich
 *	         mail:  JointFuse@yandex.ru
 *  First version:  20.10.2021
 */
#ifndef mesh_mail_h
#define mesh_mail_h

#include "freertos\FreeRTOS.h"
#include "freertos\task.h"
#include "esp_log.h"
#include "esp_err.h"
#include "esp_mesh.h"
#include "project_defs.h"
#include "mcpwm_module.h"

extern esp_err_t mesh_mail_start();

#endif