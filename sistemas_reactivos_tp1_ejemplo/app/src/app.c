#include "main.h"
#include "cmsis_os.h"
#include "logger.h"
#include "dwt.h"
#include "board.h"

#include "task_button.h"
#include "task_led.h"
#include "task_ui.h"
#include "app.h"
#include "cmsis_os.h"
#include "logger.h"

QueueHandle_t ui_queue;

QueueHandle_t led_red_queue;
QueueHandle_t led_blue_queue;
QueueHandle_t led_yellow_queue;

void app_init(void)
{
    ui_queue = xQueueCreate(1, sizeof(int));
    led_red_queue = xQueueCreate(1, sizeof(int));
    led_blue_queue = xQueueCreate(1, sizeof(int));
    led_yellow_queue = xQueueCreate(1, sizeof(int));

    configASSERT(ui_queue != NULL);
    configASSERT(led_red_queue != NULL);
    configASSERT(led_blue_queue != NULL);
    configASSERT(led_yellow_queue != NULL);

    vQueueAddToRegistry(ui_queue, "UI Queue");
    vQueueAddToRegistry(led_red_queue, "LED Red Queue");
    vQueueAddToRegistry(led_blue_queue, "LED Blue Queue");
    vQueueAddToRegistry(led_yellow_queue, "LED Yellow Queue");

    xTaskCreate(task_button, "Button Task", 128, NULL, tskIDLE_PRIORITY, NULL);
    xTaskCreate(task_ui, "UI Task", 128, NULL, tskIDLE_PRIORITY, NULL);
    xTaskCreate(task_led, "LED Red Task", 128, (void*)led_red_queue, tskIDLE_PRIORITY, NULL);
    xTaskCreate(task_led, "LED Blue Task", 128, (void*)led_blue_queue, tskIDLE_PRIORITY, NULL);
    xTaskCreate(task_led, "LED Yellow Task", 128, (void*)led_yellow_queue, tskIDLE_PRIORITY, NULL);

    LOGGER_INFO("App initialized");

    cycle_counter_init();
}

/********************** end of file ******************************************/
