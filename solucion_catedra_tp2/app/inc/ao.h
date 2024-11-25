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

#ifndef AO_H_
#define AO_H_

/********************** CPP guard ********************************************/
#ifdef __cplusplus
extern "C" {
#endif

/********************** inclusions *******************************************/

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "main.h"
#include "cmsis_os.h"

/********************** macros ***********************************************/

/********************** typedef **********************************************/

typedef struct ao_msg_s ao_msg_t;
typedef struct ao_event_s ao_event_t;
typedef struct ao_handle_s ao_handle_t;

typedef void (*ao_handler_t)(ao_handle_t* hao, void* pargs, ao_event_t* hevent);

struct ao_handle_s
{
    void* pargs;
    ao_handler_t handler;
};

struct ao_event_s
{
    int event;
    int value;
};

typedef void (*ao_callback_t)(ao_event_t* hevent);

struct ao_msg_s
{
    ao_callback_t callback;
    ao_event_t* hevent;
    ao_handle_t* hao;
};

/********************** external data declaration ****************************/

/********************** external functions declaration ***********************/

bool ao_send(ao_handle_t* hao, ao_event_t* pevent, ao_callback_t callback);

void ao_init(ao_handle_t* hao, ao_handler_t handler, void* pargs);

void ao_core_init(void);

void ao_core_tick(void);

/********************** End of CPP guard *************************************/
#ifdef __cplusplus
}
#endif

#endif /* AO_H_ */
/********************** end of file ******************************************/

