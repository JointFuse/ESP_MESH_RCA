#ifndef smartconfig_main_h
#define smartconfig_main_h

#include <string.h>
#include <stdlib.h>
#include "freertos\FreeRTOS.h"
#include "freertos\task.h"
#include "freertos\event_groups.h"
#include "esp_wifi.h"
#include "esp_wpa2.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_smartconfig.h"
#include "led_indicate.h"

static const int CONNECTED_BIT = BIT0;
static const int ESPTOUCH_DONE_BIT = BIT1;
static const int ESPTOUCH_GOTCONF = BIT2;

extern void smartconfig_get_router_settings(EventGroupHandle_t** event_ptr);

#endif