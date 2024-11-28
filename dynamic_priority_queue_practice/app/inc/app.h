/*
 * app.h
 *
 *  Created on: Nov 25, 2024
 *      Author: franco
 */

#ifndef INC_APP_H_
#define INC_APP_H_

#include "ao.h"
#include "pao.h"

typedef struct {
	ao_t *ui;
	pao_t *leds;
} system_t;

void app_init(void);

#endif /* INC_APP_H_ */
