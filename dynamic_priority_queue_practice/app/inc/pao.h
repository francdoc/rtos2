#ifndef INC_PAO_H_
#define INC_PAO_H_

#define MAX_SIZE 10

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

#endif /* INC_PAO_H_ */
