#pragma once

#include <pthread.h>

typedef struct queue_node {
	struct queue_node *next;
	int value;
	char pad[sizeof(struct queue_node *) - sizeof(int)];
} queue_node_t;

typedef struct queue {
	queue_node_t *head, *tail;
	pthread_mutex_t head_lock, tail_lock;
} queue_t;

queue_t *queue_init(void);

void queue_enqueue(queue_t *const, const int);

int queue_dequeue(queue_t *const);

void queue_destroy(queue_t *);
