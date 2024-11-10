// task_dispatcher.c

#include "app.h"
#include "logger.h"

void task_dispatcher(void *argument) {
#ifdef SINGLE_TASK_SINGLE_AO
	ao_event_t* event_ptr;
	while (true) {
		if (xQueueReceive(dispatcher_queue, &event_ptr, portMAX_DELAY) == pdPASS) {
			 LOGGER_INFO("Event received: recipient=%d, event_type=%d",

					 event_ptr->recipient,
			                        event_ptr->recipient == AO_ID_UI ? event_ptr->event_data.button_event : event_ptr->event_data.led_event);

			switch (event_ptr->recipient) {
			case AO_ID_UI:
                LOGGER_INFO("Dispatching to UI handler with button_event=%d", event_ptr->event_data.button_event);
				handle_ui_event(event_ptr->event_data.button_event);
				break;
			case AO_ID_LED_RED:
			case AO_ID_LED_YELLOW:
			case AO_ID_LED_BLUE:
                LOGGER_INFO("Dispatching to LED handler: recipient=%d, led_event=%d", event_ptr->recipient, event_ptr->event_data.led_event);
				handle_led_event(event_ptr->recipient, event_ptr->event_data.led_event);
				break;
			default:
                LOGGER_INFO("Unknown AO ID: %d", event_ptr->recipient);
				break;
			}
            LOGGER_INFO("Freeing event memory with callback");
			event_ptr->callback_free(event_ptr);
		}
	}
#endif

#ifdef SINGLE_TASK_MULTIPLE_AO
	 ao_event_t* event_ptr;
	    while (true) {
	        // Check each AO queue for events
	        if (xQueueReceive(ui_queue, &event_ptr, 0) == pdPASS) {
	            LOGGER_INFO("UI Event received");
	            handle_ui_event(event_ptr->event_data.button_event);
	            event_ptr->callback_free(event_ptr);
	        }

	        if (xQueueReceive(led_red_queue, &event_ptr, 0) == pdPASS) {
	            LOGGER_INFO("LED Red Event received");
	            handle_led_event(AO_ID_LED_RED, event_ptr->event_data.led_event);
	            event_ptr->callback_free(event_ptr);
	        }

	        if (xQueueReceive(led_yellow_queue, &event_ptr, 0) == pdPASS) {
	            LOGGER_INFO("LED Yellow Event received");
	            handle_led_event(AO_ID_LED_YELLOW, event_ptr->event_data.led_event);
	            event_ptr->callback_free(event_ptr);
	        }

	        if (xQueueReceive(led_blue_queue, &event_ptr, 0) == pdPASS) {
	            LOGGER_INFO("LED Blue Event received");
	            handle_led_event(AO_ID_LED_BLUE, event_ptr->event_data.led_event);
	            event_ptr->callback_free(event_ptr);
	        }

	        vTaskDelay(pdMS_TO_TICKS(10)); // Adjust polling rate if necessary
	    }
#endif
}

