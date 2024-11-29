#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "main.h"
#include "cmsis_os.h"
#include "board.h"
#include "logger.h"
#include "dwt.h"

#include "app.h"
#include "button.h"
#include "led.h"
#include "ui.h"

void ui_process_event(ao_event_t* ao_event) {
	/*Adjusting ui_process_event to accept a pointer matches the type expected by ao_process_event_t.*/
	button_event_t* button_event = (button_event_t*) (*ao_event);

	int priority = 0;
	led_color_t led_color = LED_COLOR_NONE; // Initialize to avoid uninitialized use

	switch(button_event->type) {
	case BUTTON_TYPE_PULSE:
		LOGGER_INFO("PULSE");
		priority = 3;
		led_color = LED_COLOR_RED;
		break;
	case BUTTON_TYPE_SHORT:
		LOGGER_INFO("SHORT");
		priority = 2;
		led_color = LED_COLOR_GREEN;
		break;
	case BUTTON_TYPE_LONG:
		LOGGER_INFO("LONG");
		priority = 1;
		led_color = LED_COLOR_BLUE;
		break;
	default:
		LOGGER_INFO("Unknown button event type: %d", button_event->type);
		return; // Exit if the event type is unknown
	}
	if (button_event->type != BUTTON_TYPE_NONE) {
		LOGGER_INFO("Priority: %d, LED Color: %d", priority, led_color);
	}
}
