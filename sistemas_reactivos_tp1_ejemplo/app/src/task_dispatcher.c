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
	    	// NOTE 1: loop that checks in a cyclic way 4 queues. Another way is to use a global queue and pass it on to each active object, check RobAxt implementation for TP2 (they do not create 4 queues, just 1 flagged with a True/False condition).
	    	// NOTE 2: in here you could make a call to a function_pointer that dispatches the messages according to AO ID. And this function_pointer can be associated to the corresponding AO data structure.
	    	// 		 And the event message has to contain the event and this AO data structure.

	    	// SUMMARY:
	    	// 			- make a task that receives a message.
	    	// 			- inside the received message there is: an event and also a pointer to an AO structure (this message was sent by a generic AO send function that needs a specific AO structure as input).
	    	// 			- from this function pointer you make a process message callback (in the case of UI process message callback, inside that function you define the specific AO structure as input for the generic AO send function).
	    	// 			- which inside it can do for example another queue send or execute some kind of action.

	        // Check each AO queue for events
	        if (xQueueReceive(ui_queue, &event_ptr, 0) == pdPASS) {
				LOGGER_INFO("UI Event received");
				LOGGER_INFO("Going to execute UI event process callback");
				event_ptr->callback_process_event(event_ptr->event_data.button_event);
				// handle_ui_event(event_ptr->event_data.button_event); // NOTE: commented out to try function pointer concept

				/* NOTE:
				* To implement function pointers for handling all events, it's important to standardize the interface
				* for these function pointers. This means designing a unified data structure that can consistently
				* pass function pointers and the associated data across different active objects (AOs).
				*
				* By creating a standard interface, we ensure compatibility across various event processing functions,
				* allowing each AO to define its own event handler while still adhering to a common function signature.
				*
				* The function pointer void (*callback_process_event)(button_event_t) is compatible with 
				* void handle_ui_event(button_event_t event) but is incompatible with void handle_led_event(ao_id_t led_id, led_event_t event).
				* 
				* To work around this limitation, we need to pass a generic data structure to callback_process_event. This data structure
				* should contain all necessary information, such as the AO ID and specific event data, allowing each handler to interpret
				* the data according to its requirements. Inside each handle function (e.g., UI or LED), we can then extract and process 
				* the components of this data structure as needed. 
				*
				* This approach enables a unified event handling interface that supports both single-parameter and multi-parameter functions, 
				* making it flexible enough to accommodate various event signatures.
				*/
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

