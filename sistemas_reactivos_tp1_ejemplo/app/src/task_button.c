//task_button.c

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "main.h"
#include "cmsis_os.h"
#include "board.h"
#include "logger.h"
#include "dwt.h"

#include "app.h"
#include "task_ui.h"

#define TASK_PERIOD_MS_           (5)
#define BUTTON_PERIOD_MS_         (TASK_PERIOD_MS_)

#define PULSE_BUTTON_LOWER_LIMIT_MS (200)
#define SHORT_BUTTON_UPPER_LIMIT_MS (1000)
#define LONG_BUTTON_LOWER_LIMIT_MS (2000)

void task_button(void* argument)
{
	button_task_params_t *params = (button_task_params_t*) argument;
    uint32_t button_counter = 0;

    LOGGER_INFO("Button task initialized");

    while (true)
    {
        GPIO_PinState button_state = HAL_GPIO_ReadPin(BUTTON_PORT, BUTTON_PIN);
        if (button_state == GPIO_PIN_SET)
        {
            button_counter += BUTTON_PERIOD_MS_;
        }
        else
        {
            button_event_t event = BUTTON_TYPE_NONE;
            if (button_counter >= LONG_BUTTON_LOWER_LIMIT_MS)
                event = BUTTON_TYPE_LONG; // to led_blue_queue
            else if (button_counter >= SHORT_BUTTON_UPPER_LIMIT_MS && button_counter <= LONG_BUTTON_LOWER_LIMIT_MS)
                event = BUTTON_TYPE_SHORT;  // to led_yellow_queue
            else if (button_counter >= PULSE_BUTTON_LOWER_LIMIT_MS && button_counter <= SHORT_BUTTON_UPPER_LIMIT_MS)
                event = BUTTON_TYPE_PULSE; // to led_red_queue

            if (BUTTON_TYPE_NONE != event)
            {
                // Log button duration before sending the event
                LOGGER_INFO("Button event duration: %lu ms", button_counter);
                xQueueSend(params->ui_queue_h, &event, ( TickType_t ) 0);

                // Log the sent event
                LOGGER_INFO("Sent event to UI queue: %d", event);
            }

            button_counter = 0;
        }
        vTaskDelay(pdMS_TO_TICKS(BUTTON_PERIOD_MS_));
    }
}

/********************** end of file ******************************************/

