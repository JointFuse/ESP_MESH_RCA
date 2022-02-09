/*
 *	    Developer:  Mayskiy Alexandr Igorevich
 *	         mail:  JointFuse@yandex.ru
 *  First version:  26.10.2021
 */
#include "mcpwm_module.h"

static const char* MCPWM_MODULE_TAG = "mcpwm_module";
/* Timer things */
static StaticTimer_t timer_buffer;
static TimerHandle_t mcpwm_timer;
/* Global variables */
static bool initialized = false;
static bool timer_manage = false;
static bool pushed = false;
static float mcpwm_duty = 0.0;

void mcpwm_timer_callback(TimerHandle_t timer)
{
    if (timer_manage) {
        pushed = false;
        timer_manage = false;
    }
}

void mcpwm_module_push()
{
    timer_manage = false;
    if (!pushed)
        pushed = true;
    xTimerReset(mcpwm_timer, portMAX_DELAY);
    timer_manage = true;
}

void mcpwm_module_set_duty(int duty)
{
    if (initialized)
        mcpwm_duty = duty;
}

void mcpwm_module_task(void* param)
{
	while (1) {
        if (pushed) {
            mcpwm_set_signal_high(MCPWM_UNIT_0,
                                  MCPWM_TIMER_0,
                                  MCPWM_GEN_A);
            mcpwm_set_duty(MCPWM_UNIT_0,
                           MCPWM_TIMER_0,
                           MCPWM_GEN_A,
                           mcpwm_duty);
            mcpwm_set_duty_type(MCPWM_UNIT_0,
                                MCPWM_TIMER_0,
                                MCPWM_GEN_A,
                                MCPWM_DUTY_MODE_0);
        }
        else
            mcpwm_set_signal_low(MCPWM_UNIT_0,
                                 MCPWM_TIMER_0,
                                 MCPWM_GEN_A);

        vTaskDelay(MCPWM_MODULE_TASK_DELAY);
	}
}

esp_err_t mcpwm_module_init()
{
    esp_err_t init_error;
    init_error = mcpwm_gpio_init(
        MCPWM_UNIT_0,
        MCPWM0A,
        GPIO_PWM0A_OUT
    );
    
    if (init_error)
        return init_error;

    mcpwm_config_t mcpwm_settings = {
        .frequency = MCPWM_MODULE_FREQUENCY,
        .cmpr_a = 0,
        .cmpr_b = 0,
        .duty_mode = MCPWM_DUTY_MODE_0,
        .counter_mode = MCPWM_UP_COUNTER
    };

    init_error = mcpwm_init(
        MCPWM_UNIT_0,
        MCPWM_TIMER_0,
        &mcpwm_settings
    );

    if (!init_error)
        initialized = true;

    TickType_t timer_period = MCPWM_MODULE_WORK_TIME;
    mcpwm_timer = xTimerCreateStatic(
        "mcpwm_timer",
        timer_period,
        pdFALSE,
        NULL,
        mcpwm_timer_callback,
        &timer_buffer
    );

    if (!mcpwm_timer)
        return ESP_FAIL;

    return init_error;
}

esp_err_t mcpwm_module_start()
{
    if (!initialized)
        return ESP_FAIL;

    TaskHandle_t mcpwm_module_handle;
    xTaskCreate(
        mcpwm_module_task,
        "mcpwm_module_task",
        MCPWM_MODULE_STACK_SIZE,
        NULL,
        MCPWM_MODULE_PRIORITY,
        &mcpwm_module_handle
    );

    if (mcpwm_module_handle == NULL)
        return ESP_FAIL;

    ESP_LOGI(MCPWM_MODULE_TAG, "<started>");
    return ESP_OK;
}