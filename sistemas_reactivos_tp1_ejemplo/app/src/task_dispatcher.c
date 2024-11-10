// task_dispatcher.c

#include "app.h"
#include "logger.h"

void task_dispatcher(void *argument) {
#ifdef SINGLE_TASK_MULTIPLE_AO
	ao_event_t* event_ptr;
	while (true) {
		if (xQueueReceive(dispatcher_queue, &event_ptr, portMAX_DELAY) == pdPASS) {
			switch (event_ptr->recipient) {
			case AO_ID_UI:
				handle_ui_event(event_ptr->event_data.button_event);
				break;
			case AO_ID_LED_RED:
			case AO_ID_LED_YELLOW:
			case AO_ID_LED_BLUE:
				handle_led_event(event_ptr->recipient, event_ptr->event_data.led_event);
				break;
			default:
				LOGGER_INFO("Unknown AO ID");
				break;
			}
			event_ptr->callback_free(event_ptr);
		}
	}
#endif
}

