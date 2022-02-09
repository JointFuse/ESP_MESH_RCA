/*
 *	    Developer:  Mayskiy Alexandr Igorevich
 *	         mail:  JointFuse@yandex.ru
 *  First version:  26.10.2021
 */
#include "led_indicate.h"

static const char* LED_INDICATE_TAG = "led_indicate";
/* Timer things */
static StaticTimer_t led_indicate_on_timer_buffer;
static TimerHandle_t led_indicate_on_timer;
static StaticTimer_t led_indicate_off_timer_buffer;
static TimerHandle_t led_indicate_off_timer;
/* Global variables */
static bool blink_indicate = false;
static int indicate_color = 0;

void indicate_on_timer_callback(TimerHandle_t timer)
{
    if (blink_indicate) {
        gpio_set_level(indicate_color, 1);
        xTimerReset(led_indicate_off_timer, portMAX_DELAY);
    }
}

void indicate_off_timer_callback(TimerHandle_t timer)
{
    gpio_set_level(indicate_color, 0);
    xTimerReset(led_indicate_on_timer, portMAX_DELAY);
}

void led_indicate_red_blink()
{
    blink_indicate = true;
    indicate_color = GPIO_RED_LED;
    gpio_set_level(GPIO_GREEN_LED, 0);
    gpio_set_level(GPIO_RED_LED, 1);
    xTimerReset(led_indicate_off_timer, portMAX_DELAY);
}

void led_indicate_green_blink()
{
    blink_indicate = true;
    indicate_color = GPIO_GREEN_LED;
    gpio_set_level(GPIO_RED_LED, 0);
    gpio_set_level(GPIO_GREEN_LED, 1);
    xTimerReset(led_indicate_off_timer, portMAX_DELAY);
}

void led_indicate_red()
{
    blink_indicate = false;
    gpio_set_level(GPIO_GREEN_LED, 0);
    gpio_set_level(GPIO_RED_LED, 1);
}

void led_indicate_green()
{
    blink_indicate = false;
    gpio_set_level(GPIO_RED_LED, 0);
    gpio_set_level(GPIO_GREEN_LED, 1);
}

esp_err_t led_indicate_init()
{
    TickType_t timer_period = LED_INDICATE_OFF_T;
    led_indicate_on_timer = xTimerCreateStatic(
        "led_indicate_on_timer",
        timer_period,
        pdFALSE,
        NULL,
        indicate_on_timer_callback,
        &led_indicate_on_timer_buffer
    );

    if (!led_indicate_on_timer)
        return ESP_FAIL;

    timer_period = LED_INDICATE_ON_T;
    led_indicate_off_timer = xTimerCreateStatic(
        "led_indicate_off_timer",
        timer_period,
        pdFALSE,
        NULL,
        indicate_off_timer_callback,
        &led_indicate_off_timer_buffer
    );

    if (!led_indicate_off_timer)
        return ESP_FAIL;

    /* configure LED indication pins */
    gpio_config_t gpio_settings = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = GPIO_LED_PULL
    };
    gpio_config(&gpio_settings);
    gpio_set_level(GPIO_RED_LED, 0);
    gpio_set_level(GPIO_GREEN_LED, 0);

    ESP_LOGI(LED_INDICATE_TAG, "<ready>");
    return ESP_OK;
}