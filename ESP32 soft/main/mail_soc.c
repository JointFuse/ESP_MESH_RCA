/*
 *	    Developer:  Mayskiy Alexandr Igorevich
 *	         mail:  JointFuse@yandex.ru
 *  First version:  18.10.2021
 */
#include "mail_soc.h"

static const char* MAIL_TAG = "mail_soc";

enum Action_t{
    default_action,
    send_good,
    send_fail
};

void mail_soc_mesh_send(InMessage_t* to_send)
{
    esp_err_t send_state = ESP_OK;

     mesh_data_t message = {
        .data = (uint8_t*)(to_send),
        .size = sizeof(InMessage_t),
        .proto = MESH_PROTO_BIN,
        .tos = MESH_TOS_P2P
     };

    send_state = esp_mesh_send(
        (mesh_addr_t*)(to_send->recipient),
        &message,
        MESH_DATA_P2P,
        NULL, 0
    );
    
    ESP_ERROR_CHECK_WITHOUT_ABORT(send_state);
}

void mail_soc_check_message()
{
    static InMessage_t message_buf;

    if (receive_queue_get(&message_buf) == ESP_OK) {
        if (message_buf.type == GETDEVICES) {
            int routing_table_size = 0;
            mesh_addr_t routing_table[CONFIG_MESH_ROUTE_TABLE_SIZE + 1];
            esp_mesh_get_routing_table(
                routing_table,
                CONFIG_MESH_ROUTE_TABLE_SIZE,
                &routing_table_size
            );

            int i;
            for (i = 0; i < 6; ++i)
                routing_table[routing_table_size].addr[i] = 0;
            
            ++routing_table_size;
            send_queue_add((OutMessage_t*)(&routing_table[0]), routing_table_size);
        }
        else
            mail_soc_mesh_send(&message_buf);
    }
}

void mail_soc_task(void* param)
{
    EventBits_t uxBits;
    ESP_LOGI(MAIL_TAG, "<started>");

    while (1) {
        uxBits = xEventGroupGetBits(mail_soc_event_group);
        
        if (uxBits & MAIL_SOC_STOP) {
            xEventGroupClearBits(mail_soc_event_group, 
                                 MAIL_SOC_RUN | MAIL_SOC_STOP);
            vTaskDelete(NULL);
            return;
        }

        mail_soc_check_message();
        vTaskDelay(MAIL_SOC_TASK_DELAY);
    }
}

esp_err_t mail_socket_init()
{
    if (mail_soc_event_group != NULL) {
        EventBits_t uxBits = xEventGroupWaitBits(
            mail_soc_event_group,
            MAIL_SOC_INIT,
            false, false,
            portMAX_DELAY
        );

        if (!(uxBits & MAIL_SOC_INIT))
            xEventGroupSetBits(mail_soc_event_group,
                               MAIL_SOC_INIT);
        return ESP_OK;
    }

    mail_soc_event_group = xEventGroupCreate();

    if (mail_soc_event_group == NULL)
        return ESP_FAIL;
    
    xEventGroupSetBits(mail_soc_event_group,
                       MAIL_SOC_INIT);
    
    ESP_ERROR_CHECK(message_queue_init());
    ESP_LOGI(MAIL_TAG, "<initialized>");

    return ESP_OK;
}

esp_err_t mail_socket_start()
{
    if (mail_soc_event_group == NULL)
        return ESP_FAIL;

    EventBits_t uxBits = xEventGroupWaitBits(
        mail_soc_event_group,
        MAIL_SOC_INIT,
        false, false,
        portMAX_DELAY
    );

    if (!(uxBits & MAIL_SOC_INIT))
        return ESP_FAIL;

    TaskHandle_t mail_soc_handle;
    xTaskCreate(
        mail_soc_task,
        "mail_soc_task",
        MAIL_SOC_STACK_SIZE,
        NULL,
        MAIL_SOC_PRIORITY,
        &mail_soc_handle
    );

    if (mail_soc_handle == NULL)
        return ESP_FAIL;

    xEventGroupSetBits(mail_soc_event_group,
                       MAIL_SOC_RUN);

    return ESP_OK;
}

void mail_socket_stop()
{
    xEventGroupSetBits(mail_soc_event_group,
                       MAIL_SOC_STOP);
}