#include "app.h"

#include "queue_p.h"

#include <string.h>

#define QUEUE_P_LENGTH 10

void queue_create(queue_p_t *queue)
{
    if (queue) {
        queue->head = NULL;
        queue->tail = NULL;
        queue->current_length = 0;
        queue->queue_mutex = xSemaphoreCreateMutex();
        vQueueAddToRegistry(queue->queue_mutex, "Mutex Handle");
    }
}

void queue_destroy(queue_p_t *queue)
{
    if (queue) {
        // Protect shared resource (mutex)
        xSemaphoreTake(queue->queue_mutex, portMAX_DELAY);
        {
            node_t *next = queue->head ? queue->head->next : NULL; // Ensure head is valid before accessing next
            node_t *current = queue->head;

            while (current) {
                vPortFree(current);
                current = next;
                next = current ? current->next : NULL; // Prevent dereferencing NULL
            }
        }
        xSemaphoreGive(queue->queue_mutex);

        // Destroy the mutex
        vSemaphoreDelete(queue->queue_mutex);

        // Reset the queue's fields to indicate it is cleared
        queue->head = NULL;
        queue->tail = NULL;
        queue->current_length = 0;
    }
}

// Removes the element with the highest priority from the list
bool_t queue_pop(queue_p_t* queue, void* data)
{
    if(!queue || !data) {
    	return false;
    }

    bool_t ret = false;

    xSemaphoreTake(queue->queue_mutex, portMAX_DELAY);
    {
        if(!queue_is_empty(queue)) {
            memcpy(data, &(queue->head->data), sizeof(int));
            node_t* temp = queue->head;
            (queue->head) = (queue->head)->next;
            queue->current_length--;
            vPortFree(temp);
            ret = true;
        }
    }
    xSemaphoreGive(queue->queue_mutex);

    return ret;
}

// Function to push according to priority (p)
bool_t queue_push(queue_p_t* queue, void* data, int p)
{
    bool_t ret = false;

    if (queue == NULL || data == NULL) {
        return false; // Handle invalid inputs gracefully
    }

    xSemaphoreTake(queue->queue_mutex, portMAX_DELAY);
    {
        if (queue->current_length < QUEUE_P_LENGTH) {
            node_t* start = queue->head;

            // Create a new node
            node_t* temp = (node_t*)pvPortMalloc(sizeof(node_t));
            if (temp == NULL) {
                xSemaphoreGive(queue->queue_mutex);
                return false; // Handle memory allocation failure
            }

            // Copy data into the new node
            temp->data = data;
            temp->priority = p;
            temp->next = NULL;

            queue->current_length++;

            if (queue_is_empty(queue)) {
                // If the queue is empty, set the new node as the head
                queue->head = temp;
            } else if (queue->head->priority < p) {
                // Special case: The new node has higher priority than the current head
                temp->next = queue->head;
                queue->head = temp;
            } else {
                // Traverse the list to find the correct position
                while (start->next != NULL && start->next->priority >= p) {
                    start = start->next;
                }

                // Insert the new node at the correct position
                temp->next = start->next;
                start->next = temp;
            }
            ret = true;
        }
    }
    xSemaphoreGive(queue->queue_mutex);

    return ret;
}

// Function to check is list is empty
bool_t queue_is_empty(queue_p_t* queue) {
	return (queue->head) == NULL;
}
