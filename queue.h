#ifndef _QUEUE_H
#define _QUEUE_H


#include <stdlib.h>
#include "sha.h"
#include "bt_parse.h"


typedef struct node_s {
	void *data;
	struct node_s* next;
} node_t;

typedef struct queue_s {
    node_t *head;
    node_t *tail;
    int n;
} queue_t;

queue_t *queue_init(void);
void enqueue(queue_t* queue, void *data);
void *dequeue(queue_t* queue);
void free_queue(queue_t* queue);



#endif /* _QUEUE_H */