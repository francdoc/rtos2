#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "main.h"
#include "cmsis_os.h"
#include "board.h"
#include "logger.h"
#include "dwt.h"

#include "app.h"

void task_ui(void* argument)
{
    int event;
    while (true)
    {
        if (xQueueReceive(ui_queue, &event, portMAX_DELAY) == pdPASS)
        {
            switch (event)
            {
                case BUTTON_TYPE_PULSE:
                    xQueueSend(led_red_queue, &event, ( TickType_t ) 0);
                    break;
                case BUTTON_TYPE_SHORT:
                    xQueueSend(led_yellow_queue, &event, ( TickType_t ) 0);
                    break;
                case BUTTON_TYPE_LONG:
                    xQueueSend(led_blue_queue, &event, ( TickType_t ) 0);
                    break;
                default:
                    break;
            }
        }
    }
}
