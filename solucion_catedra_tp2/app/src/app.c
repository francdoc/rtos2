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

#include <button.h>
#include "main.h"
#include "cmsis_os.h"
#include "logger.h"
#include "dwt.h"
#include "board.h"

#include "common.h"
#include "resources.h"
#include "ao.h"
#include "ao_led.h"
#include "ao_ui.h"

/********************** macros and definitions *******************************/

#define TASK_PERIOD_MS_           (50)

/********************** internal data declaration ****************************/

static ao_event_t reset_event_ = {.event = SYSEVENT_RESET, .value = 0};

/********************** internal functions declaration ***********************/

static void task_app_(void* argument)
{
  while(true)
  {
    button_loop();
    ao_core_tick();
    vTaskDelay((TickType_t)(TASK_PERIOD_MS_ / portTICK_PERIOD_MS));
  }
}

/********************** internal data definition *****************************/

/********************** external data declaration *****************************/

/********************** external functions definition ************************/

void app_init(void)
{
#if 1
  /* Only for test */
  HAL_GPIO_WritePin(LED_RED_PORT, LED_RED_PIN, GPIO_PIN_SET);
  HAL_GPIO_WritePin(LED_GREEN_PORT, LED_GREEN_PIN, GPIO_PIN_SET);
  HAL_GPIO_WritePin(LED_BLUE_PORT, LED_BLUE_PIN, GPIO_PIN_SET);
#endif

  button_init();
  ao_core_init();

  BaseType_t status;
  status = xTaskCreate(task_app_, "task_app", 128, NULL, tskIDLE_PRIORITY, NULL);
  while (pdPASS != status)
  {
    // error
  }

  ao_led_red_config.color = COLOR_RED;
  ao_init(&ao_led_red, ao_led_handler, &ao_led_red_config);

  ao_led_green_config.color = COLOR_GREEN;
  ao_init(&ao_led_green, ao_led_handler, &ao_led_green_config);

  ao_led_blue_config.color = COLOR_BLUE;
  ao_init(&ao_led_blue, ao_led_handler, &ao_led_blue_config);

  ao_init(&ao_ui, ao_ui_handler, NULL);

  ao_send(&ao_ui, &reset_event_, NULL);

  LOGGER_INFO("app init");

  cycle_counter_init();
}

/********************** end of file ******************************************/
