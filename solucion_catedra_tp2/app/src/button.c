/*
 * Copyright (c) 2024 Sebastian Bedin <sebabedin@gmail.com>.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * @author : Sebastian Bedin <sebabedin@gmail.com>
 */

/********************** inclusions *******************************************/

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "main.h"
#include "cmsis_os.h"
#include "board.h"
#include "logger.h"
#include "dwt.h"

#include "ao.h"
#include "common.h"
#include "resources.h"

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

static void callback_(ao_event_t* hevent)
{
  vPortFree((void*)hevent);
}

static void send_(int event, int value)
{
  ao_event_t* hevent = (ao_event_t*)pvPortMalloc(sizeof(ao_event_t));
  if(NULL != hevent)
  {
    hevent->event = event;
    hevent->value = value;
    if(ao_send(&ao_ui, hevent, callback_))
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

/********************** external functions definition ************************/

void button_init(void)
{
  button.counter = 0;
}

void button_loop(void)
{
  GPIO_PinState button_state = HAL_GPIO_ReadPin(BTN_PORT, BTN_PIN);
  button_type_t button_type = button_process_state_(button_state);
  if(BUTTON_TYPE_NONE != button_type)
  {
    LOGGER_INFO("new button event");
    send_(SYSEVENT_BUTTON, button_type);
  }
}

/********************** end of file ******************************************/
