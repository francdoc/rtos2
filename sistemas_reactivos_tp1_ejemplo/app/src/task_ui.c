// task_ui.c

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "main.h"
#include "cmsis_os.h"
#include "board.h"
#include "logger.h"
#include "dwt.h"

#include "app.h"
#include "task_led.h"

void task_ui(void* argument)
{
	ui_task_params_t *params = (ui_task_params_t*) argument;

    QueueHandle_t ui_recv_queue_h = params->ui_ao.event_queue_h;

    QueueHandle_t led_red_queue = params->led_red_queue_h;
    QueueHandle_t led_yellow_queue = params->led_yellow_queue_h;
    QueueHandle_t led_blue_queue = params->led_blue_queue_h;

    button_event_t event;

    LOGGER_INFO("UI task initialized");

    while (true)
    {
        if (xQueueReceive(ui_recv_queue_h, &event, portMAX_DELAY) == pdPASS)
        {
            led_event_t led_event = LED_NONE;

            switch (event)
            {
                case BUTTON_TYPE_PULSE:
                    led_event = LED_RED_BLINK;
                    xQueueSend(led_red_queue, &led_event, (TickType_t)0);
                    LOGGER_INFO("Sent LED event: RED_BLINK");
                    break;
                case BUTTON_TYPE_SHORT:
                    led_event = LED_YELLOW_BLINK;
                    xQueueSend(led_yellow_queue, &led_event, (TickType_t)0);
                    LOGGER_INFO("Sent LED event: YELLOW_BLINK");
                    break;
                case BUTTON_TYPE_LONG:
                    led_event = LED_BLUE_BLINK;
                    xQueueSend(led_blue_queue, &led_event, (TickType_t)0);
                    LOGGER_INFO("Sent LED event: BLUE_BLINK");
                    break;
                default:
                    LOGGER_INFO("Received unknown event, no LED action sent");
                    break;
            }
            vTaskDelay(pdMS_TO_TICKS(5));
        }
    }
}

