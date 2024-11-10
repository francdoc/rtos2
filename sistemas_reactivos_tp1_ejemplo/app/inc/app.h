#ifndef APP_H_
#define APP_H_

#ifdef __cplusplus
extern "C" {
#endif

/********************** inclusions *******************************************/
#include "cmsis_os.h"

#include "task_button.h"
#include "task_ui.h"
#include "task_led.h"

#include "memory_pool.h"

//#define MULTIPLE_TASK_MULTIPLE_AO
#define SINGLE_TASK_MULTIPLE_AO

/********************** external data declaration ****************************/

typedef struct {
	QueueHandle_t ui_queue_h; // queue for msg dispatching
} button_task_params_t;

typedef struct {
	QueueHandle_t event_queue_h; // queue for event recv
} active_object_t;

typedef struct {
    active_object_t ui_ao;
    QueueHandle_t led_red_queue_h;  // queue for msg dispatching
    QueueHandle_t led_blue_queue_h;
    QueueHandle_t led_yellow_queue_h;
} ui_task_params_t;

typedef struct {
	active_object_t led_ao;
} led_task_params_t;

typedef enum
{
  LED_NONE,
  LED_RED_ON,
  LED_RED_OFF,
  LED_RED_BLINK,
  LED_YELLOW_ON,
  LED_YELLOW_OFF,
  LED_YELLOW_BLINK,
  LED_BLUE_ON,
  LED_BLUE_OFF,
  LED_BLUE_BLINK,
} led_event_t;

#ifdef SINGLE_TASK_MULTIPLE_AO
// Identificadores de AO
typedef enum {
	AO_ID_UI,
	AO_ID_LED_RED,
	AO_ID_LED_YELLOW,
	AO_ID_LED_BLUE,
} ao_id_t;

// Estructura de mensaje para comunicación entre AOs
typedef struct {
	ao_id_t recipient;
	union {
		button_event_t button_event;
		led_event_t led_event;
	} event_data;
	void (*callback_free)(void*); // callback to free memory
} ao_event_t;

extern memory_pool_t memory_pool; // global memory pool for single-task configuration
extern QueueHandle_t dispatcher_queue; // global message queue for single-task configuration

void handle_led_event(ao_id_t led_id, led_event_t event);
void memory_pool_block_free(void *pblock);

#endif

/********************** external functions declaration ***********************/
void app_init(void);

#ifdef __cplusplus
}
#endif

#endif /* APP_H_ */

