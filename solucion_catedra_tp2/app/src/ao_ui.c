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

/********************** internal data declaration ****************************/

/********************** internal functions declaration ***********************/

typedef enum
{
  STATE_INIT,
  STATE_STANDBY,
  STATE_RED,
  STATE_GREEN,
  STATE_BLUE,
  STATE_ERROR,
} _state_t;

/********************** internal data definition *****************************/

static _state_t _state = STATE_INIT;

/********************** external data definition *****************************/

/********************** internal functions definition ************************/

static void _callback(ao_event_t* hevent)
{
  vPortFree((void*)hevent);
}

static void _send(ao_handle_t* hao, int event, int value)
{
  ao_event_t* hevent = (ao_event_t*)pvPortMalloc(sizeof(ao_event_t));
  if(NULL != hevent)
  {
    hevent->event = event;
    hevent->value = value;
    if(!(ao_send(hao, hevent, _callback)))
    {
      vPortFree((void*)hevent);
    }
  }
}

_state_t _get_next_state(ao_event_t *hevent)
{
  _state_t state = STATE_ERROR;
  switch (hevent->event)
  {
    case SYSEVENT_RESET:        state = STATE_STANDBY; break;
    case SYSEVENT_BUTTON:
      switch (hevent->value)
      {
        case BUTTON_TYPE_PULSE: state = STATE_RED; break;
        case BUTTON_TYPE_SHORT: state = STATE_GREEN; break;
        case BUTTON_TYPE_LONG:  state = STATE_BLUE; break;
        default: /* Error */ break;
      }
      break;
    default: /* Error */ break;
  }
  return state;
}

void _exit_state(_state_t state)
{
  switch (state)
  {
    case STATE_INIT:    break;
    case STATE_STANDBY: break;
    case STATE_RED:     _send(&ao_led_red, SYSEVENT_LED_OFF, 0); break;
    case STATE_GREEN:   _send(&ao_led_green, SYSEVENT_LED_OFF, 0); break;
    case STATE_BLUE:    _send(&ao_led_blue, SYSEVENT_LED_OFF, 0); break;
    default: /* Error */ break;
  }
}

void _enter_state(_state_t state)
{
  switch (state)
  {
    case STATE_INIT:    break;
    case STATE_STANDBY:
      _send(&ao_led_red, SYSEVENT_LED_OFF, 0);
      _send(&ao_led_green, SYSEVENT_LED_OFF, 0);
      _send(&ao_led_blue, SYSEVENT_LED_OFF, 0);
      break;
    case STATE_RED:     _send(&ao_led_red, SYSEVENT_LED_ON, 0); break;
    case STATE_GREEN:   _send(&ao_led_green, SYSEVENT_LED_ON, 0); break;
    case STATE_BLUE:    _send(&ao_led_blue, SYSEVENT_LED_ON, 0); break;
    default: /* Error */ break;
  }
}

/********************** external functions definition ************************/

void ao_ui_handler(ao_handle_t *hao, void *pargs, ao_event_t *hevent)
{
  _state_t next_state = _get_next_state(hevent);
  if (_state != next_state)
  {
    _exit_state(_state);
    _state = next_state;
    _enter_state(_state);
  }
}

/********************** end of file ******************************************/
