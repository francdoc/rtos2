#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "main.h"
#include "cmsis_os.h"
#include "board.h"
#include "logger.h"
#include "dwt.h"

#include "app.h"

#define TASK_PERIOD_MS_           (5)
#define BUTTON_PERIOD_MS_         (TASK_PERIOD_MS_)

#define PULSE_BUTTON_LOWER_LIMIT_MS (200)
#define SHORT_BUTTON_UPPER_LIMIT_MS (1000)
#define LONG_BUTTON_LOWER_LIMIT_MS (2000)

void task_button(void* argument)
{
    uint32_t button_counter = 0;

    while (true)
    {
        GPIO_PinState button_state = HAL_GPIO_ReadPin(BUTTON_PORT, BUTTON_PIN);
        if (button_state == GPIO_PIN_SET)
        {
            button_counter += BUTTON_PERIOD_MS_;
        }
        else
        {
            int event = 0;
            if (button_counter >= LONG_BUTTON_LOWER_LIMIT_MS)
                event = BUTTON_TYPE_LONG; // to led_blue_queue
            else if (button_counter >= SHORT_BUTTON_UPPER_LIMIT_MS && button_counter <= LONG_BUTTON_LOWER_LIMIT_MS)
                event = BUTTON_TYPE_SHORT;  // to led_yellow_queue
            else if (button_counter >= PULSE_BUTTON_LOWER_LIMIT_MS && button_counter <= SHORT_BUTTON_UPPER_LIMIT_MS)
                event = BUTTON_TYPE_PULSE; // to led_red_queue

            if (event)
                xQueueSend(ui_queue, &event, ( TickType_t ) 0);

            button_counter = 0;
        }
        vTaskDelay(pdMS_TO_TICKS(BUTTON_PERIOD_MS_));
    }
}

/********************** end of file ******************************************/
