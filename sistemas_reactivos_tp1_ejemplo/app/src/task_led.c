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

// LED blink duration in milliseconds
#define BLINK_DELAY_MS (1000)

void task_led(void* argument)
{
    QueueHandle_t led_queue = (QueueHandle_t)argument;
    int event;

    while (true)
    {
        // Receive event from queue
        if (xQueueReceive(led_queue, &event, portMAX_DELAY) == pdPASS)
        {
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
}
