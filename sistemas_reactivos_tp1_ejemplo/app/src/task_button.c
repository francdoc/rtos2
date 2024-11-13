// task_button.c

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
    uint32_t button_counter = 0;

    LOGGER_INFO("Button task initialized");

#ifdef MULTIPLE_TASK_MULTIPLE_AO
	button_task_params_t *params = (button_task_params_t*) argument;

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
                LOGGER_INFO("Button event duration: %lu ms", button_counter);
                xQueueSend(params->ui_queue_h, &event, (TickType_t) 0);
                LOGGER_INFO("Sent event to UI queue: %d", event);
            }

            button_counter = 0;
        }
        vTaskDelay(pdMS_TO_TICKS(BUTTON_PERIOD_MS_));
    }
#endif

#ifdef SINGLE_TASK_SINGLE_AO
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
                LOGGER_INFO("Button event duration: %lu ms", button_counter);

				ao_event_t *msg = (ao_event_t*) memory_pool_block_get(&memory_pool);
				if (msg != NULL)
				{
                    // Log the contents of the message after assignment
                    LOGGER_INFO("Message prepared: recipient=%d, event_data.button_event=%d, callback_free=%p",
                                msg->recipient, msg->event_data.button_event, (void*)msg->callback_free);
					msg->recipient = AO_ID_UI;
					msg->event_data.button_event = event;
					msg->callback_free = memory_pool_block_free;
					xQueueSend(dispatcher_queue, &msg, 0);
                    LOGGER_INFO("Message dispatched");
				}
	            button_counter = 0;
			}
		}
		vTaskDelay(pdMS_TO_TICKS(BUTTON_PERIOD_MS_));
    }
#endif

#ifdef SINGLE_TASK_MULTIPLE_AO
	ao_all_t* ao_all = (ao_all_t*) argument;

	ao_t* ao_ui = (ao_t*) ao_all->ui;
	ao_t* ao_red_led = (ao_t*) ao_all->red;
	ao_t* ao_blue_led = (ao_t*) ao_all->blue;
	ao_t* ao_yellow_led = (ao_t*) ao_all->yellow;

    while (true) {
		GPIO_PinState button_state = HAL_GPIO_ReadPin(BUTTON_PORT, BUTTON_PIN);

		if (button_state == GPIO_PIN_SET) {
			button_counter += BUTTON_PERIOD_MS_;
		} else {
			button_event_t event = BUTTON_TYPE_NONE;

			if (button_counter >= LONG_BUTTON_LOWER_LIMIT_MS) {
				event = BUTTON_TYPE_LONG;
			} else if (button_counter >= SHORT_BUTTON_UPPER_LIMIT_MS) {
				event = BUTTON_TYPE_SHORT;
			} else if (button_counter >= PULSE_BUTTON_LOWER_LIMIT_MS) {
				event = BUTTON_TYPE_PULSE;
			}

			if (event != BUTTON_TYPE_NONE) {
				ao_event_t* msg = (ao_event_t*) memory_pool_block_get(&memory_pool);

				if (msg != NULL) {
					msg->callback_free = memory_pool_block_free;
					msg->event_data.target_h.queue_red_h = ao_red_led->event_queue_h;
					msg->event_data.target_h.queue_yellow_h = ao_yellow_led->event_queue_h;
					msg->event_data.target_h.queue_blue_h = ao_blue_led->event_queue_h;
					msg->event_data.button_event = event; // WARNING: we assign this at the end to avoid corruption
					LOGGER_INFO("Button event duration: %lu ms", button_counter);
					LOGGER_INFO("Button task: event_data.button_event before sending=%d", msg->event_data.button_event);
					LOGGER_INFO("Button task: Address of msg (event_ptr) before sending=%p", (void*)msg);
					xQueueSend(ao_ui->event_queue_h, &msg, 0);
					LOGGER_INFO("Button task: event_data.button_event after sending=%d", msg->event_data.button_event);
					LOGGER_INFO("Button task: Address of msg (event_ptr) after sending=%p", (void*)msg);
				}
			}
			button_counter = 0;
		}
		vTaskDelay(pdMS_TO_TICKS(BUTTON_PERIOD_MS_));
	}
#endif
}


