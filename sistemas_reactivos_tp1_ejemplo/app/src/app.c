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

#ifdef SINGLE_TASK_SINGLE_AO
#include "memory_pool.h"
QueueHandle_t dispatcher_queue;
memory_pool_t memory_pool;
static uint8_t memblock[MEMORY_POOL_SIZE(10,sizeof(ao_event_t))];
#endif

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

#ifdef SINGLE_TASK_SINGLE_AO
void memory_pool_block_free(void *pblock) {
    memory_pool_block_put(&memory_pool, pblock);
}
#endif

#ifdef SINGLE_TASK_MULTIPLE_AO
void memory_pool_block_free(void *pblock) {
    memory_pool_block_put(&memory_pool, pblock);
}

QueueHandle_t ui_queue;
QueueHandle_t led_red_queue;
QueueHandle_t led_yellow_queue;
QueueHandle_t led_blue_queue;

static ao_t ao_ui;
static ao_t ao_red_led;
static ao_t ao_blue_led;
static ao_t ao_yellow_led;

static ao_all_t ao_all = {
    .ui = &ao_ui,
    .red = &ao_red_led,
    .blue = &ao_blue_led,
    .yellow = &ao_yellow_led
};
#endif

void app_init(void)
{
#ifdef SINGLE_TASK_SINGLE_AO
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

#ifdef SINGLE_TASK_SINGLE_AO
    xTaskCreate(task_button, "Button Task", 256, NULL, tskIDLE_PRIORITY, NULL); // now queue handle for button task is 1 and it is global
    xTaskCreate(task_dispatcher, "Dispatcher Task", 256, NULL, tskIDLE_PRIORITY, NULL);
#endif

#ifdef SINGLE_TASK_MULTIPLE_AO
	memory_pool_init(&memory_pool, memblock, 10, sizeof(ao_event_t));

	ui_queue = xQueueCreate(10, sizeof(ao_event_t*));
	led_red_queue = xQueueCreate(10, sizeof(ao_event_t*));
	led_yellow_queue = xQueueCreate(10, sizeof(ao_event_t*));
	led_blue_queue = xQueueCreate(10, sizeof(ao_event_t*));

	configASSERT(ui_queue && led_red_queue && led_yellow_queue && led_blue_queue);

    /*
    NOTE (General Structure):
        - This initialization assigns each active object (AO) its specific queue handle and event-processing handler.
        - We configure a setup where the button task is aware of the UI active object and its queue handle, allowing 
        it to send events directly to the UI AO.
        - The dispatcher task, in turn, receives a reference to all active objects (`ao_all`), giving it access to 
        each AO's queue for event reception and corresponding function pointer for processing events.
        
        - **Initial Planning (Dependency Analysis)**:
            - Before implementing, we need to perform a **Dependency Analysis** to clarify what each component 
            (like the dispatcher task or each AO) requires from the other. Specifically:
                - Define the dispatcher’s role and the exact data it needs to access to handle events effectively.
                - Determine the requirements of each event-processing handler, including the specific AO data and the 
                type of events it will handle.
            - This planning step clarifies what data structures, function pointers, and parameters are necessary 
            for the task and handler implementation.
        
        - The main structure:
            - Each AO has a unique queue handle for incoming events and a designated event handler (function pointer) 
            for processing those events.
            - The dispatcher cyclically checks all AO queues for messages. When an event is received, it uses the 
            AO's function pointer (`callback_process_event`) to handle the event.

        - Function Pointers:
            - The function pointers allow each AO to have a specific event-processing function, such as `handle_ui_event` 
            for the UI AO and `handle_led_event` for each LED AO.
            - Since the function pointers need to be compatible across AOs, a single, generic data structure (`ao_event_t`) 
            is used to standardize the function signature, enabling easy access to the correct AO and event data.
    */

   ao_all.ui->event_queue_h = ui_queue;
   ao_all.red->event_queue_h = led_red_queue;
   ao_all.yellow->event_queue_h = led_yellow_queue;
   ao_all.blue->event_queue_h = led_blue_queue;

   LOGGER_INFO("Assigned queue handles for each active object:");
   LOGGER_INFO("UI Queue Handle Address: %p", (void*)ao_all.ui->event_queue_h);
   LOGGER_INFO("LED Red Queue Handle Address: %p", (void*)ao_all.red->event_queue_h);
   LOGGER_INFO("LED Yellow Queue Handle Address: %p", (void*)ao_all.yellow->event_queue_h);
   LOGGER_INFO("LED Blue Queue Handle Address: %p", (void*)ao_all.blue->event_queue_h);

   ao_all.ui->callback_process_event = handle_ui_event;
   ao_all.red->callback_process_event = handle_led_event;
   ao_all.yellow->callback_process_event = handle_led_event;
   ao_all.blue->callback_process_event = handle_led_event;
   
   xTaskCreate(task_button, "Button Task", 256, &ao_all, tskIDLE_PRIORITY, NULL);
   xTaskCreate(task_dispatcher, "Dispatcher Task", 256, &ao_all, tskIDLE_PRIORITY, NULL);
#endif

    LOGGER_INFO("App initialized");
    cycle_counter_init();
}

/********************** end of file ******************************************/
