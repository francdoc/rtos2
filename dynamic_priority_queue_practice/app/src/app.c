#include "main.h"
#include "cmsis_os.h"
#include "logger.h"
#include "dwt.h"
#include "board.h"

#include "app.h"

#define LED_AO_TASK_PRIORITY (tskIDLE_PRIORITY + 1)
#define UI_AO_TASK_PRIORITY (tskIDLE_PRIORITY + 1)
#define BUTTON_TASK_PRIORITY (tskIDLE_PRIORITY + 1)

static ao_t ao_ui;
static pao_t pao_leds;

static system_t system = {
		.ui = &ao_ui,
		.leds = &pao_leds,
};

void app_init(void)
{
	LOGGER_INFO("app init");
	cycle_counter_init();
}
