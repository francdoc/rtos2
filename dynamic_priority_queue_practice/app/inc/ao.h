#ifndef INC_AO_H_
#define INC_AO_H_

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

void init_ao(ao_t* ao,
		ao_process_event_t* ao_process_event,
		uint8_t ao_task_priority,
		const char* task_name);

#endif /* INC_AO_H_ */
