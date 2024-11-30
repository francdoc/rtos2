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

#include <ao.h>

/********************** macros and definitions *******************************/

#define MESSAGE_TIMEOUT_MS_      (100)
#define QUEUE_LENGTH_            (10)
#define QUEUE_ITEM_SIZE_         (sizeof(ao_msg_t))

/********************** internal data declaration ****************************/

/********************** internal functions declaration ***********************/

/********************** internal data definition *****************************/

static QueueHandle_t hqueue_;
static SemaphoreHandle_t hsem_;
static TaskHandle_t htask_;

/********************** external data definition *****************************/

/********************** internal functions definition ************************/

static void task_(void *argument)
{
  ao_msg_t msg;
  while (pdPASS == xQueueReceive(hqueue_, &msg, (TickType_t)(MESSAGE_TIMEOUT_MS_ / portTICK_PERIOD_MS)))
  {
    msg.hao->handler(msg.hao, msg.hao->pargs, msg.hevent);
    if(msg.callback)
    {
      msg.callback(msg.hevent);
    }
  }
  xSemaphoreGive(hsem_);
  vTaskSuspend(NULL);

  while (true)
  {
    // error
  }
}

/********************** external functions definition ************************/

bool ao_send(ao_handle_t* hao, ao_event_t* hevent, ao_callback_t callback)
{
  ao_msg_t msg;
  msg.hao = hao;
  msg.hevent = hevent;
  msg.callback = callback;
  return (pdPASS == xQueueSendToBack(hqueue_, (void*)&msg, 0));
}

void ao_init(ao_handle_t* hao, ao_handler_t handler, void* pargs)
{
  hao->handler = handler;
  hao->pargs = pargs;
}

void ao_core_init(void)
{
  static bool init = false;
  if(init)
  {
    return;
  }
  init = true;

  hqueue_ = xQueueCreate(QUEUE_LENGTH_, QUEUE_ITEM_SIZE_);
  while(NULL == hqueue_)
  {
    // error
  }

  hsem_ = xSemaphoreCreateBinary();
  while(NULL == hsem_)
  {
    // Error
  }
  xSemaphoreTake(hsem_, 0);
}

void ao_core_tick(void)
{
  static bool task_is_active = false;
  if(task_is_active)
  {
    if(pdTRUE == xSemaphoreTake(hsem_, 0))
    {
      vTaskDelete(htask_);
    }
    task_is_active = false;
  }
  else
  {
    if(0 < uxQueueMessagesWaiting(hqueue_))
    {
      BaseType_t status;
      status = xTaskCreate(task_, "task_ao", 128, NULL, tskIDLE_PRIORITY, &htask_);
      while (pdPASS != status)
      {
        // error
      }
      while(NULL == htask_)
      {
        // error
      }
    }
    task_is_active = true;
  }
}

/********************** end of file ******************************************/