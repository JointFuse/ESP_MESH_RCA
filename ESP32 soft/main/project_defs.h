#ifndef project_defs_h
#define project_defs_h

#include "esp_system.h"
#include "esp_mesh.h"
#include "esp_err.h"

#define ASIO_MESSAGE_SIZE		1024
#define MESSAGE_SIZE			sizeof(InMessage_t)

#define OUTPUT_QUEUE_LENGTH		CONFIG_MESH_ROUTE_TABLE_SIZE + 10
#define INPUT_QUEUE_LENGTH		50
#define OUTPUT_QUEUE_ITEM_SIZE	sizeof(OutMessage_t)
#define INPUT_QUEUE_ITEM_SIZE	MESSAGE_SIZE
#define WAIT_TO_ADD_TIME		10 / portTICK_PERIOD_MS
#define WAIT_TO_GET_TIME		1 / portTICK_PERIOD_MS

#define MAIL_SOC_STACK_SIZE		4096
#define MAIL_SOC_PRIORITY		1
#define MAIL_SOC_TASK_DELAY		5 / portTICK_PERIOD_MS
#define MAIL_SOC_SEND_BLOCK		20 / portTICK_PERIOD_MS

#define MESH_MAIL_STACK_SIZE	4096
#define MESH_MAIL_PRIORITY		2
#define MESH_MAIL_TASK_DELAY	25 / portTICK_PERIOD_MS
#define MESH_RECEIVE_TIMEOUT	2 / portTICK_PERIOD_MS
#define MESH_SENDTIME_UPADAPT	1.5
#define MESH_SENDTIME_DOWNADAPT	0.8
#define MESH_TIME_ADAPT_BORDER	2
#define MESH_BAD_SEND_BORDER	3

#define MCPWM_MODULE_STACK_SIZE	4096
#define MCPWM_MODULE_PRIORITY	3
#define MCPWM_MODULE_TASK_DELAY	20 / portTICK_PERIOD_MS
#define MCPWM_MODULE_WORK_TIME	120 / portTICK_PERIOD_MS

#define MESH_RX_SIZE			64

#define GPIO_PWM0A_OUT			4
#define MCPWM_MODULE_FREQUENCY	500

#define LED_INDICATE_OFF_T		1000 / portTICK_PERIOD_MS
#define LED_INDICATE_ON_T		100 / portTICK_PERIOD_MS
#define GPIO_GREEN_LED			19
#define GPIO_RED_LED			18
#define GPIO_LED_PULL			((1ULL<<GPIO_GREEN_LED) | (1ULL<<GPIO_RED_LED))

#define GPIO_GND_IO				14
#define GPIO_IO0_IO				26
#define GPIO_RXD0_IO			32
#define GPIO_PROTECT_PULL		((1ULL<<GPIO_GND_IO) | (1ULL<<GPIO_IO0_IO) | (1ULL<<GPIO_RXD0_IO))

#define TURNON		'o'
#define SETLEVEL	's'
#define GETDEVICES	'g'

typedef struct {
	char type;
	uint8_t recipient[6];
	int  value;
} InMessage_t;

typedef struct {
	mesh_addr_t device;
} OutMessage_t;

#endif