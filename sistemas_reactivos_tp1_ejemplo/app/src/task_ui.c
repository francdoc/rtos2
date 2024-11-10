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

#ifdef MULTIPLE_TASK_MULTIPLE_AO
void task_ui(void* argument)
{
	ui_task_params_t *params = (ui_task_params_t*) argument;

    button_event_t event;

    LOGGER_INFO("UI task initialized");

    while (true)
    {
    	if (xQueueReceive(params->ui_ao.event_queue_h, &event, portMAX_DELAY) == pdPASS)
        {
            led_event_t led_event = LED_NONE;

            LOGGER_INFO("UI task received an event...");

            switch (event)
            {
                case BUTTON_TYPE_PULSE:
                    led_event = LED_RED_BLINK;
                    xQueueSend(params->led_red_queue_h, &led_event, (TickType_t)0);
                    LOGGER_INFO("Sent LED event: RED_BLINK");
                    break;
                case BUTTON_TYPE_SHORT:
                    led_event = LED_YELLOW_BLINK;
                    xQueueSend(params->led_yellow_queue_h, &led_event, (TickType_t)0);
                    LOGGER_INFO("Sent LED event: YELLOW_BLINK");
                    break;
                case BUTTON_TYPE_LONG:
                    led_event = LED_BLUE_BLINK;
                    xQueueSend(params->led_blue_queue_h, &led_event, (TickType_t)0);
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
#endif

#ifdef SINGLE_TASK_MULTIPLE_AO
void handle_ui_event(button_event_t event) {
	ao_event_t* msg = (ao_event_t*) memory_pool_block_get(&memory_pool);
    if (msg != NULL) {
        switch (event) {
            case BUTTON_TYPE_PULSE:
                msg->recipient = AO_ID_LED_RED;
                msg->event_data.led_event = LED_RED_ON;
                break;
            case BUTTON_TYPE_SHORT:
                msg->recipient = AO_ID_LED_YELLOW;
                msg->event_data.led_event = LED_YELLOW_ON;
                break;
            case BUTTON_TYPE_LONG:
                msg->recipient = AO_ID_LED_BLUE;
                msg->event_data.led_event = LED_BLUE_ON;
                break;
            default:
                return;
        }
        msg->callback_free = memory_pool_block_free;
        xQueueSend(dispatcher_queue, &msg, 0);
    }
}
#endif

