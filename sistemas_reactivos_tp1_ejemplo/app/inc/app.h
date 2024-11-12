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
extern QueueHandle_t ui_queue;
extern QueueHandle_t led_red_queue;
extern QueueHandle_t led_blue_queue;
extern QueueHandle_t led_yellow_queue;

/********************** external functions declaration ***********************/
void app_init(void);

#ifdef __cplusplus
}
#endif

#endif /* APP_H_ */

