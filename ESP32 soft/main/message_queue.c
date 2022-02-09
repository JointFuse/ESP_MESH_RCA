/*
 *	    Developer:  Mayskiy Alexandr Igorevich
 *	         mail:  JointFuse@yandex.ru
 *  First version:  18.10.2021
 */
#include "message_queue.h"

static StaticQueue_t input_queue_buffer;
static StaticQueue_t output_queue_buffer;
static uint8_t input_queue_storage[INPUT_QUEUE_LENGTH * INPUT_QUEUE_ITEM_SIZE];
static uint8_t output_queue_storage[OUTPUT_QUEUE_LENGTH * OUTPUT_QUEUE_ITEM_SIZE];
static QueueHandle_t input_queue = NULL;
static QueueHandle_t output_queue = NULL;
static bool output_accessibility = false;

esp_err_t message_queue_init()
{
	if (input_queue == NULL) {
		input_queue = xQueueCreateStatic(
			INPUT_QUEUE_LENGTH,
			INPUT_QUEUE_ITEM_SIZE,
			&(input_queue_storage[0]),
			&input_queue_buffer
		);
	}
	if (output_queue == NULL) {
		output_queue = xQueueCreateStatic(
			OUTPUT_QUEUE_LENGTH,
			OUTPUT_QUEUE_ITEM_SIZE,
			&(output_queue_storage[0]),
			&output_queue_buffer
		);
		output_accessibility = true;
	}
	if (input_queue == NULL ||
		output_queue == NULL)
		return ESP_FAIL;

	return ESP_OK;
}

esp_err_t receive_queue_add(InMessage_t* message)
{
	if (!xQueueSendToBack(input_queue, message, WAIT_TO_ADD_TIME)) {
		return ESP_FAIL;
	}

	return ESP_OK;
}

esp_err_t receive_queue_get(InMessage_t* buffer)
{
	if (!xQueueReceive(input_queue, buffer, WAIT_TO_GET_TIME)) {
		return ESP_FAIL;
	}

	return ESP_OK;
}

esp_err_t send_queue_add(OutMessage_t* message, int count)
{
	if (output_queue == NULL)
		return ESP_FAIL;

	while (!output_accessibility)
		/* wait */;
	output_accessibility = false;

	int i;
	for (i = 0; i < count; ++i)
		if (!xQueueSendToBack(output_queue, &message[i], WAIT_TO_ADD_TIME)) {
			output_accessibility = true;
			return ESP_FAIL;
		}

	output_accessibility = true;
	return ESP_OK;
}

esp_err_t send_queue_get(OutMessage_t* buffer)
{
	if (output_queue == NULL)
		return ESP_FAIL;
	
	while (!output_accessibility)
		/* wait */;
	output_accessibility = false;

	if (!xQueueReceive(output_queue, buffer, WAIT_TO_GET_TIME)) {
		output_accessibility = true;
		return ESP_FAIL;
	}

	output_accessibility = true;
	return ESP_OK;
}