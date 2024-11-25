/*
 * ao.h
 *
 *  Created on: Nov 25, 2024
 *      Author: franco
 */

#ifndef INC_AO_H_
#define INC_AO_H_

typedef void* event_data_t;

typedef struct {
	void (*callback_process_event)(ao_event_t*);
	QueueHandle_t event_queue_h;
	uint8_t id;
} ao_t;

#endif /* INC_AO_H_ */
