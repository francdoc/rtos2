// app.c

#include "main.h"
#include "cmsis_os.h"
#include "logger.h"
#include "dwt.h"
#include "board.h"

#include "task_button.h"
#include "task_led.h"
#include "task_ui.h"
#include "task_dispatcher.h"
#include "app.h"

// #define MULTIPLE_TASK_MULTIPLE_AO
#define SINGLE_TASK_MULTIPLE_AO

#ifdef SINGLE_TASK_MULTIPLE_AO
#include "memory_pool.h"
QueueHandle_t dispatcher_queue;
memory_pool_t memory_pool;
static uint8_t memblock[MEMORY_POOL_SIZE(10,sizeof(ao_event_t))];
#endif

// NOTE: now I will not make the queue handles extern (global).
//		 I can encapsulate them within the app.c file, keeping them private to that file. Instead of using extern declarations in app.h.
// 		 So that we can manage all the queue handles through active_object_t instances without exposing them externally.

#ifdef MULTIPLE_TASK_MULTIPLE_AO
static active_object_t ui_ao;

static active_object_t led_red_ao;
static active_object_t led_blue_ao;
static active_object_t led_yellow_ao;

static button_task_params_t button_task_params;
static ui_task_params_t ui_task_params;
static led_task_params_t led_red_params;
static led_task_params_t led_blue_params;
static led_task_params_t led_yellow_params;
#endif

void app_init(void)
{
#ifdef SINGLE_TASK_MULTIPLE_AO
	memory_pool_init(&memory_pool, memblock, 10, sizeof(ao_event_t));
	dispatcher_queue = xQueueCreate(10, sizeof(ao_event_t));
	configASSERT(dispatcher_queue != NULL);
#endif

#ifdef MULTIPLE_TASK_MULTIPLE_AO
	ui_ao.event_queue_h = xQueueCreate(1, sizeof(button_event_t));

    configASSERT(ui_ao.event_queue_h != NULL);

    vQueueAddToRegistry(ui_ao.event_queue_h , "UI Queue");

    led_red_ao.event_queue_h = xQueueCreate(1, sizeof(led_event_t));
    led_blue_ao.event_queue_h = xQueueCreate(1, sizeof(led_event_t));
    led_yellow_ao.event_queue_h = xQueueCreate(1, sizeof(led_event_t));

    configASSERT(led_red_ao.event_queue_h != NULL);
    configASSERT(led_blue_ao.event_queue_h != NULL);
    configASSERT(led_yellow_ao.event_queue_h != NULL);

    vQueueAddToRegistry(led_red_ao.event_queue_h, "LED Red Queue");
    vQueueAddToRegistry(led_blue_ao.event_queue_h, "LED Blue Queue");
    vQueueAddToRegistry(led_yellow_ao.event_queue_h, "LED Yellow Queue");

    // NOTE: There are generally two main approaches for the UI active object to access the queue handles of each LED active object:
    // 		 1) Using global handles.
    //       2) Passing each LED active object to the UI active object, allowing private access to these handles.

    // In this case we choose option 2).

    // NOTE: The task parameter structures (button_task_params, ui_task_params, led_red_params, etc.)
    //       are declared as global variables rather than local variables within app_init().
    //       This prevents potential hard faults or undefined behavior caused by stack-allocated
    //       variables going out of scope once app_init() completes. Since tasks run asynchronously
    //       and continuously access these structures, they require globally persistent storage.

	button_task_params.ui_queue_h = ui_ao.event_queue_h;

	ui_task_params.ui_ao = ui_ao;
	ui_task_params.led_red_queue_h = led_red_ao.event_queue_h;
	ui_task_params.led_blue_queue_h = led_blue_ao.event_queue_h;
	ui_task_params.led_yellow_queue_h = led_yellow_ao.event_queue_h;

	led_red_params.led_ao = led_red_ao;
	led_blue_params.led_ao = led_blue_ao;
	led_yellow_params.led_ao = led_yellow_ao;

    xTaskCreate(task_button, "Button Task", 256, (void*)&button_task_params, tskIDLE_PRIORITY, NULL);

    xTaskCreate(task_ui, "UI Task", 256, (void*)&ui_task_params, tskIDLE_PRIORITY, NULL);

    xTaskCreate(task_led, "LED Red Task", 256, (void*)&led_red_params, tskIDLE_PRIORITY, NULL);
    xTaskCreate(task_led, "LED Blue Task", 256, (void*)&led_blue_params, tskIDLE_PRIORITY, NULL);
    xTaskCreate(task_led, "LED Yellow Task", 256, (void*)&led_yellow_params, tskIDLE_PRIORITY, NULL);
#endif

#ifdef SINGLE_TASK_MULTIPLE_AO
    xTaskCreate(task_button, "Button Task", 256, NULL, tskIDLE_PRIORITY, NULL); // now queue handle for button task is 1 and it is global
    xTaskCreate(task_dispatcher, "Dispatcher Task", 256, NULL, tskIDLE_PRIORITY, NULL);
#endif

    LOGGER_INFO("App initialized");

    cycle_counter_init();
}

/********************** end of file ******************************************/
