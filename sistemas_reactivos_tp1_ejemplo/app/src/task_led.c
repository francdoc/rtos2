// task_led.c

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "main.h"
#include "cmsis_os.h"
#include "board.h"
#include "logger.h"
#include "dwt.h"

#include "app.h"
#include "task_led.h"

#define BLINK_DELAY_MS (1000)

void task_led(void* argument)
{
    LOGGER_INFO("LED task initialized");

#ifdef MULTIPLE_TASK_MULTIPLE_AO
    led_task_params_t *params = (led_task_params_t*) argument;
    led_event_t event;

    while (true)
    {
        // Receive event from queue
        if (xQueueReceive(params->led_ao.event_queue_h, &event, portMAX_DELAY) == pdPASS)
        {
            LOGGER_INFO("Event received in LED task: %d", event);  // Log received event

            // LED action based on event type
            // NOTE 1: Using global queue handles, while convenient, can lead to potential issues in concurrent systems.
            //         A better approach would be to encapsulate the queue handle in an "active object" structure,
            //         allowing us to access the specific memory region holding the queue handle, reducing the risk of conflicts in a multithreaded environment.

            // NOTE 2: The events handled in the switch statement should be LED-related events, not button events.
            //         This decoupling helps to separate button handling from LED actions.

            switch (event)
            {
                case LED_RED_BLINK:
                    LOGGER_INFO("LED set to RED blink");
                    HAL_GPIO_WritePin(LED_RED_PORT, LED_RED_PIN, GPIO_PIN_SET);
                    vTaskDelay(pdMS_TO_TICKS(BLINK_DELAY_MS));
                    HAL_GPIO_WritePin(LED_RED_PORT, LED_RED_PIN, GPIO_PIN_RESET);
                    vTaskDelay(pdMS_TO_TICKS(BLINK_DELAY_MS));
                    break;

                case LED_YELLOW_BLINK:
                    LOGGER_INFO("LED set to YELLOW blink");
                    HAL_GPIO_WritePin(LED_YELLOW_PORT, LED_YELLOW_PIN, GPIO_PIN_SET);
                    vTaskDelay(pdMS_TO_TICKS(BLINK_DELAY_MS));
                    HAL_GPIO_WritePin(LED_YELLOW_PORT, LED_YELLOW_PIN, GPIO_PIN_RESET);
                    vTaskDelay(pdMS_TO_TICKS(BLINK_DELAY_MS));
                    break;

                case LED_BLUE_BLINK:
                    LOGGER_INFO("LED set to BLUE blink");
                    HAL_GPIO_WritePin(LED_BLUE_PORT, LED_BLUE_PIN, GPIO_PIN_SET);
                    vTaskDelay(pdMS_TO_TICKS(BLINK_DELAY_MS));
                    HAL_GPIO_WritePin(LED_BLUE_PORT, LED_BLUE_PIN, GPIO_PIN_RESET);
                    vTaskDelay(pdMS_TO_TICKS(BLINK_DELAY_MS));
                    break;

                default:
                    LOGGER_INFO("Invalid event, no LED action");
                    break;
            }
        }
    }
#endif
}

#ifdef SINGLE_TASK_SINGLE_AO
void handle_led_event(ao_id_t led_id, led_event_t event) {
    GPIO_TypeDef* port;
    uint16_t pin;

    switch (led_id) {
        case AO_ID_LED_RED:
            port = LED_RED_PORT;
            pin = LED_RED_PIN;
            break;
        case AO_ID_LED_YELLOW:
            port = LED_YELLOW_PORT;
            pin = LED_YELLOW_PIN;
            break;
        case AO_ID_LED_BLUE:
            port = LED_BLUE_PORT;
            pin = LED_BLUE_PIN;
            break;
        default:
            return;
    }

    if (event == LED_RED_ON || event == LED_YELLOW_ON || event == LED_BLUE_ON) {
        HAL_GPIO_WritePin(port, pin, GPIO_PIN_SET);
        vTaskDelay(pdMS_TO_TICKS(1000));
        HAL_GPIO_WritePin(port, pin, GPIO_PIN_RESET);
    }
}
#endif

#ifdef SINGLE_TASK_MULTIPLE_AO

void handle_led_event(ao_id_t led_id, led_event_t event) {
    GPIO_TypeDef* port;
    uint16_t pin;

    switch (led_id) {
        case AO_ID_LED_RED:
            port = LED_RED_PORT;
            pin = LED_RED_PIN;
            break;
        case AO_ID_LED_YELLOW:
            port = LED_YELLOW_PORT;
            pin = LED_YELLOW_PIN;
            break;
        case AO_ID_LED_BLUE:
            port = LED_BLUE_PORT;
            pin = LED_BLUE_PIN;
            break;
        default:
            return;
    }

    if (event == LED_RED_ON || event == LED_YELLOW_ON || event == LED_BLUE_ON) {
        HAL_GPIO_WritePin(port, pin, GPIO_PIN_SET);
        vTaskDelay(pdMS_TO_TICKS(1000));
        HAL_GPIO_WritePin(port, pin, GPIO_PIN_RESET);
    }
}
#endif

