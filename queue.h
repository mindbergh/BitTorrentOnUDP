#ifndef _QUEUE_H
#define _QUEUE_H



#include "sha.h"
#include "bt_parse.h"


typedef struct node_s {
	void *data;
	struct node_s next;
} node_t;

typedef struct queue_s {
    node_t *head;
    node_t *tail;
    int n;
} queue_t;




#endif /* _QUEUE_H */