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

/********************** external functions declaration ***********************/
void app_init(void);

#ifdef __cplusplus
}
#endif

#endif /* APP_H_ */

