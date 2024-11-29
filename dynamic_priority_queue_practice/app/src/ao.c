#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "main.h"
#include "cmsis_os.h"
#include "board.h"
#include "logger.h"
#include "dwt.h"

#include "ao.h"

#define MESSAGE_TIMEOUT_MS_      (100)
#define QUEUE_LENGTH_            (10)

static char *QUEUE_ID_1 = "Queue_id_1";
static char *QUEUE_ID_2 = "Queue_id_2";
static char *QUEUE_ID_3 = "Queue_id_3";
static char *QUEUE_ID_4 = "Queue_id_4";
static char *QUEUE_ID_DEF = "Queue_id_default";

char *get_queue_name(uint8_t id) {
	char *buffer[4] = {0};
	if (id > 4) {
		return QUEUE_ID_DEF;
	}

	buffer[0] = QUEUE_ID_1;
	buffer[1] = QUEUE_ID_2;
	buffer[2] = QUEUE_ID_3;
	buffer[3] = QUEUE_ID_4;

	return buffer[id-1];
}

void ao_task(void* parameters){
	ao_t *ao = (ao_t*) parameters;

	ao_msg_t ao_msg;

	while (pdPASS == xQueueReceive(ao->event_queue_h, &ao_msg, (TickType_t)(MESSAGE_TIMEOUT_MS_/ portTICK_PERIOD_MS))) {
		ao->ao_process_event(ao_msg.ao_event);
		if(ao_msg.ao_msg_callback){
			ao_msg.ao_msg_callback(ao_msg.ao_event);
		}
	}
}

bool ao_send(ao_t* ao,
		ao_msg_callback_t ao_msg_callback,
		ao_event_t ao_event)
{
  ao_msg_t ao_msg;
  ao_msg.ao_msg_callback = ao_msg_callback;
  ao_msg.ao_event = ao_event;
  return (pdPASS == xQueueSendToBack(ao->event_queue_h, (void*)&ao_msg, 0));
}

void init_ao(ao_t* ao,
		ao_process_event_t ao_process_event,
		uint8_t ao_task_priority,
		const char* task_name)
{
	ao->event_queue_h = xQueueCreate(QUEUE_LENGTH_, ao->ao_event_size);
	vQueueAddToRegistry(ao->event_queue_h, get_queue_name(ao->ao_id));
	configASSERT(NULL != ao->event_queue_h);

	ao->ao_process_event = ao_process_event;

	BaseType_t status;
	status = xTaskCreate(ao_task, task_name, configMINIMAL_STACK_SIZE, ao, ao_task_priority, NULL);
	configASSERT(pdPASS == status);
}
