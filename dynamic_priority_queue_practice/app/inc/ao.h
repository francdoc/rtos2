#ifndef INC_AO_H_
#define INC_AO_H_

typedef void* ao_event_t;

typedef struct {
	 void (*ao_process_event_t)(ao_event_t*);
	 QueueHandle_t event_queue_h;
 } ao_t;

typedef struct {
	ao_event_t event;
	void (*ao_msg_callback_t)(void*);
	ao_t ao;
} ao_msg_t;

#endif /* INC_AO_H_ */
