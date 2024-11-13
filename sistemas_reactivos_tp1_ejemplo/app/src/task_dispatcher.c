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
	ao_all_t* ao_all = (ao_all_t*) argument;
	
	ao_t* ao_ui = (ao_t*) ao_all->ui;
	ao_t* ao_red_led = (ao_t*) ao_all->red;
	ao_t* ao_blue_led = (ao_t*) ao_all->blue;
	ao_t* ao_yellow_led = (ao_t*) ao_all->yellow;

	/*
	WARNING:
		* The reason the event_data.button_event value now correctly stays as 1 instead of changing to 96 is due to the sequence in which the event_ptr->event_data.target_h fields are assigned within the dispatcher task. Here’s a breakdown of the previous issue and the fix:
		* Previous Issue
		* Assignment After xQueueReceive Call: In the original version, event_ptr->event_data.target_h queue handles (queue_red_h, queue_yellow_h, and queue_blue_h) were assigned after receiving event_ptr from the queue in xQueueReceive.
		* Data Corruption Due to Overwriting: After xQueueReceive, if the event_ptr fields were overwritten (e.g., if event_data.target_h was changed), this would unintentionally overwrite other parts of event_ptr’s data, including event_data.button_event. This is likely due to the memory layout in event_ptr, where assigning the queue_*_h fields caused memory overwrites affecting button_event.
		* The Fix:
		* Assigning target_h Fields Outside the xQueueReceive Loop: By assigning the event_ptr->event_data.target_h queue handles (queue_red_h, queue_yellow_h, and queue_blue_h) before entering the while loop that receives event_ptr, target_h remains consistent without overwriting button_event.
		* Result: Since button_event is no longer being altered by late assignments to target_h, it correctly retains its value as 1 throughout processing. This ensures data integrity within event_ptr and prevents corruption of the button_event value after receiving it in the dispatcher.
		* Summary of Why It Works Now
		* Consistent Assignment Order: Assigning target_h fields before the while loop prevents unintended modifications to button_event.
		* Data Integrity Maintained: With target_h fields pre-assigned, button_event no longer gets overwritten by subsequent assignments, preserving its value through xQueueSend and xQueueReceive operations.
		* This fix ensures that event_ptr holds correct data as it flows through the dispatcher and UI event processing, and maintains separation between button_event and the target_h assignments within the ao_event_t structure.
	*/

	ao_event_t* event_ptr;

	/*
	NOTE:
	    It is preferable to assign the target queue handles (`target_h.queue_red_h`, `target_h.queue_yellow_h`,
	    and `target_h.queue_blue_h`) in the button task before sending `event_ptr` to the queue, rather than
	    assigning them in the dispatcher task. Assigning these fields within the dispatcher task after receiving
	    `event_ptr` can lead to data corruption issues, as modifying `event_ptr->event_data.target_h` post-`xQueueReceive`
	    has shown to overwrite other critical data, such as `event_data.button_event`.

	    By setting these fields in the button task, we ensure that `target_h` holds the correct queue handles when
	    `event_ptr` is first populated, preserving the integrity of other data fields and preventing unintended
	    overwrites. This approach maintains consistent data handling across tasks, improving data stability and
	    preventing potential memory errors associated with late assignments.

	event_ptr->event_data.target_h.queue_red_h = ao_red_led->event_queue_h;
	event_ptr->event_data.target_h.queue_yellow_h = ao_yellow_led->event_queue_h;
	event_ptr->event_data.target_h.queue_blue_h = ao_blue_led->event_queue_h;
	*/

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
		if (xQueueReceive(ao_ui->event_queue_h, &event_ptr, 0) == pdPASS) {
            LOGGER_INFO("Dispatcher task: Address of event_ptr after receiving=%p", (void*)event_ptr);
            LOGGER_INFO("Dispatcher task: event_data.button_event=%d", event_ptr->event_data.button_event);
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
            LOGGER_INFO("Dispatcher task: queue_red_h handle:%p", (void*)event_ptr->event_data.target_h.queue_red_h);
            LOGGER_INFO("Dispatcher task: queue_yellow_h handle:%p", (void*)event_ptr->event_data.target_h.queue_yellow_h);
            LOGGER_INFO("Dispatcher task: queue_blue_h handle:%p", (void*)event_ptr->event_data.target_h.queue_blue_h);

			ao_ui->callback_process_event(event_ptr); // ui process event handle needs button event and LED queue handles.	
			event_ptr->callback_free(event_ptr);
		}

		if (xQueueReceive(ao_red_led->event_queue_h, &event_ptr, 0) == pdPASS) {
			LOGGER_INFO("LED Red Event received");
			ao_red_led->callback_process_event(event_ptr);
			event_ptr->callback_free(event_ptr);
			LOGGER_INFO("Callback free ok.");
		}

		if (xQueueReceive(ao_yellow_led->event_queue_h, &event_ptr, 0) == pdPASS) {
			LOGGER_INFO("LED Yellow Event received");
			ao_yellow_led->callback_process_event(event_ptr);
			event_ptr->callback_free(event_ptr);
			LOGGER_INFO("Callback free ok.");
		}

		if (xQueueReceive(ao_blue_led->event_queue_h, &event_ptr, 0) == pdPASS) {
			LOGGER_INFO("LED Blue Event received");
			ao_blue_led->callback_process_event(event_ptr);
			event_ptr->callback_free(event_ptr);
			LOGGER_INFO("Callback free ok.");
		}
		vTaskDelay(pdMS_TO_TICKS(10)); // Adjust polling rate if necessary
	}
#endif
}

