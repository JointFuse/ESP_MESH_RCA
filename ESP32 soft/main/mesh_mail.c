/*
 *	    Developer:  Mayskiy Alexandr Igorevich
 *	         mail:  JointFuse@yandex.ru
 *  First version:  20.10.2021
 */
#include "mesh_mail.h"

static const char* MESH_MAIL_TAG = "mesh_mail";
static uint8_t rx_buf[MESH_RX_SIZE] = { 0, };

void mesh_message_soc()
{
    mesh_addr_t sender;
    mesh_data_t message = {
        .data = rx_buf,
        .size = MESH_RX_SIZE
    };
    int receive_flag = 0;
    esp_err_t receive_error;
    receive_error = esp_mesh_recv(
        &sender, &message,
        MESH_RECEIVE_TIMEOUT,
        &receive_flag,
        NULL, 0
    );

    if (receive_error) {
        ESP_ERROR_CHECK_WITHOUT_ABORT(receive_error);
        return;
    }
    /*
    if (sizeof(InMessage_t) != message.size) {
        ESP_LOGW(MESH_MAIL_TAG, "<wrong data> expected size:%d; got:%d [data: %d ]",
                 sizeof(InMessage_t), message.size, *(message.data));
        return;
    }
    */
    InMessage_t* command = (InMessage_t*)(message.data);
    //ESP_LOGI(MESH_MAIL_TAG, "<got command>type:%c", command->type);
    switch (command->type) {
    case TURNON:
        mcpwm_module_push();
        break;
    case SETLEVEL:
        mcpwm_module_set_duty(command->value);
        break;
    default:
        break;
    }
}

void mesh_mail_task(void* param)
{
    mesh_rx_pending_t pending_info;

	while (1) {
        esp_err_t check_error;
        check_error = esp_mesh_get_rx_pending(&pending_info);

        if (!check_error && pending_info.toSelf)
            mesh_message_soc();
        else if (check_error)
            ESP_ERROR_CHECK_WITHOUT_ABORT(check_error);

        vTaskDelay(MESH_MAIL_TASK_DELAY);
	}
}

esp_err_t mesh_mail_start()
{
    TaskHandle_t mesh_mail_handle;
    xTaskCreate(
        mesh_mail_task,
        "mesh_mail_task",
        MESH_MAIL_STACK_SIZE,
        NULL,
        MESH_MAIL_PRIORITY,
        &mesh_mail_handle
    );

    if (mesh_mail_handle == NULL)
        return ESP_FAIL;

    ESP_LOGI(MESH_MAIL_TAG, "<started sucessfully>");
    return ESP_OK;
}