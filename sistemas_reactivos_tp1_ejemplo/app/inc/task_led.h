#ifndef TASK_LED_H_
#define TASK_LED_H_

/********************** CPP guard ********************************************/
#ifdef __cplusplus
extern "C" {
#endif

/********************** inclusions *******************************************/
#include "app.h"

/********************** macros ***********************************************/

/********************** typedef **********************************************/
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

/********************** external data declaration ****************************/

/********************** external functions declaration ***********************/

void task_led(void* argument);

/********************** End of CPP guard *************************************/
#ifdef __cplusplus
}
#endif

#endif /* TASK_LED_H_ */
/********************** end of file ******************************************/

