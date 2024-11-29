#ifndef INC_AO_H_
#define INC_AO_H_

#include "app.h"
#include <stdbool.h>

#define MAX_SIZE 10

typedef void* ao_event_t;
typedef void (*ao_process_event_t)(ao_event_t* ao_event);
typedef void (*ao_msg_callback_t)(ao_event_t* ao_event);

typedef struct {
	uint8_t ao_event_size;
	ao_process_event_t ao_process_event;
	QueueHandle_t event_queue_h;
	uint8_t ao_id;
 } ao_t;

typedef struct {
	ao_msg_callback_t ao_msg_callback;
	ao_event_t ao_event;
} ao_msg_t;

typedef void* pao_event_t;

typedef enum {
	LOW,
	MEDIUM,
	HIGH
} priority_t;

typedef struct {
	priority_t priority;
	pao_event_t data_t;
} queue_data_t;

typedef struct {
	int size;
	queue_data_t elements[MAX_SIZE];
	SemaphoreHandle_t mtx;
} PrioQueueHandle_t;

typedef struct {
	void (*pao_process_event_t)(pao_event_t*);
	PrioQueueHandle_t pevent_queue_h;
} pao_t;

typedef struct {
	pao_event_t pevent;
	void (*pao_msg_callback_t)(void*);
	pao_t pao;
} pao_msg_t;

typedef struct {
	ao_t *ui;
	pao_t *leds;
} system_t;

void ao_task(void* parameters);

void init_ao(ao_t* ao,
		ao_process_event_t ao_process_event,
		uint8_t ao_task_priority,
		const char* task_name);


bool ao_send(ao_t* ao,
		ao_msg_callback_t ao_msg_callback,
		ao_event_t ao_event);

#endif /* INC_AO_H_ */
