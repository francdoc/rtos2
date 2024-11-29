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

void ui_process_event(ao_event_t ao_event) {
	button_event_t *button_event = (button_event_t*) ao_event;
	led_color_t led_color;
	int priority;

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
		break;
	}
	if (button_event->type != BUTTON_TYPE_NONE) {
		// pass for now...
	}
}
