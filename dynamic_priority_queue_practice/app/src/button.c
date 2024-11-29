#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "main.h"
#include "cmsis_os.h"
#include "board.h"
#include "logger.h"
#include "dwt.h"

#include "ao.h"
#include "button.h"

/********************** macros and definitions *******************************/

#define BUTTON_PERIOD_MS_               (50)
#define BUTTON_PULSE_TIMEOUT_           (200)
#define BUTTON_SHORT_TIMEOUT_           (1000)
#define BUTTON_LONG_TIMEOUT_            (2000)

/********************** internal data declaration ****************************/

/********************** internal functions declaration ***********************/

/********************** internal data definition *****************************/

static struct
{
    uint32_t counter;
} button;

/********************** external data definition *****************************/

/********************** internal functions definition ************************/

static button_type_t button_process_state_(bool value)
{
  button_type_t ret = BUTTON_TYPE_NONE;
  if(value)
  {
    button.counter += BUTTON_PERIOD_MS_;
  }
  else
  {
    if(BUTTON_LONG_TIMEOUT_ <= button.counter)
    {
      ret = BUTTON_TYPE_LONG;
      LOGGER_INFO("button long detected");
    }
    else if(BUTTON_SHORT_TIMEOUT_ <= button.counter)
    {
      ret = BUTTON_TYPE_SHORT;
      LOGGER_INFO("button short detected");
    }
    else if(BUTTON_PULSE_TIMEOUT_ <= button.counter)
    {
      ret = BUTTON_TYPE_PULSE;
      LOGGER_INFO("button pulse detected");
    }
    button.counter = 0;
  }
  return ret;
}

static void callback_(ao_event_t* ao_event)
{
  vPortFree((void*)ao_event);
}

static void send_(ao_t* ao, button_event_t button_event)
{
  if(NULL != button_event)
  {
    if(ao_send(&ao, callback_, button_event))
    {
       LOGGER_INFO("send button msg: OK");
       return;
    }
    else
    {
      vPortFree((void*)hevent);
    }
  }
  LOGGER_INFO("send button msg: error");
}

void button_init(void)
{
  button.counter = 0;
}

void button_task(void* argument)
{
	system_t *system = (system_t*) argument;
	ao_t *ui_ao = (ao_t*) system->ui;

	button_init();

	button_event_t button_event;
	button_event.ao_leds = system->leds;

	while(true) {
		GPIO_PinState button_state;
		button_state = HAL_GPIO_ReadPin(BUTTON_PORT, BUTTON_PIN);
		button_event.type = button_process_state(button_state);
		if (button_event.type != BUTTON_TYPE_NONE)
		{
			send_(ui_ao, &button_event);
		}
	}
}

/********************** end of file ******************************************/
