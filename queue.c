#include "queue.h"
#include <stdio.h>

queue_t *queue_init(void) {
    queue_t* queue = (queue_t*)malloc(sizeof(queue_t));
    queue->head = NULL;
    queue->tail = NULL;
    queue->n = 0;
    return queue;
}

void enqueue(queue_t* queue, void *data) {
    node_t* node = (node_t *)malloc(sizeof(node_t));
    node->next = NULL;
    node->data = data;

    if (queue->tail != NULL) {
        queue->tail->next = node;
    }

    queue->tail = node;

    if (queue->head == NULL) {
        queue->head = queue->tail;
    }

    queue->n += 1;
}

void *dequeue(queue_t* queue) {
    node_t* node;

   	if (queue->n == 0)
   		return NULL;

    node = queue->head;
    queue->head = queue->head->next;
    node->next = NULL;

    if (queue->head == NULL) {
        queue->tail = NULL;
    }

    void* data = node->data;

    free(node);
    queue->n -= 1;
    return data;
}

void free_queue(queue_t* queue) {
	while (queue->n != 0)
		dequeue(queue);
    free(queue);
}