/*
 * pao.h
 *
 *  Created on: Nov 25, 2024
 *      Author: franco
 */

#ifndef INC_PAO_H_
#define INC_PAO_H_

#define MAX_SIZE 10

typedef struct {
	button_event_t button_event;
	led_event_t led_event;
} event_data_t;

typedef enum {
	LOW,
	MEDIUM,
	HIGH
} priority_t;

typedef struct {
	priority_t priority;
	event_data_t data_t;
} queue_data_t;

typedef struct {
	int size;
	queue_data_t elements[MAX_SIZE];
	SemaphoreHandle_t mtx;
} PrioQueueHandle_t;

typedef struct {
	void (*callback_process_event)(queue_data_t*);
	PrioQueueHandle_t event_queue_h;
} pao_t;

#endif /* INC_PAO_H_ */
