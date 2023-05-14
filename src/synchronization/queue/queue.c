#include <stdio.h>
#include <stdlib.h>

#include "queue.h"

queue_t *
queue_init(void)
{
	queue_t *to_return = malloc(sizeof(queue_t));
	queue_node_t *tmp = malloc(sizeof(queue_node_t));

	if (!tmp) {
		return NULL;
	}

	tmp->next = NULL;
	to_return->head = to_return->tail = tmp;

	pthread_mutex_init(&to_return->head_lock, NULL);
	pthread_mutex_init(&to_return->tail_lock, NULL);

	return to_return;
}

void
queue_enqueue(queue_t *const q, const int value)
{
	queue_node_t *to_be_inserted = malloc(sizeof(queue_node_t));
	if (!to_be_inserted) {
		return;
	}

	to_be_inserted->value = value;
	to_be_inserted->next = NULL;

	pthread_mutex_lock(&q->tail_lock);

	q->tail->next = to_be_inserted;
	q->tail = to_be_inserted;

	pthread_mutex_unlock(&q->tail_lock);
}

int
queue_dequeue(queue_t *const q)
{
	queue_node_t *tmp = NULL, *new_head = NULL;

	pthread_mutex_lock(&q->head_lock);

	tmp = q->head;
	new_head = tmp->next;

	if (!new_head) {
		pthread_mutex_unlock(&q->head_lock);
		return -1;
	}

	new_head = new_head->next;
	q->head = new_head;

	pthread_mutex_unlock(&q->head_lock);
	free(tmp);

	return 0;
}

void
queue_destroy(queue_t *q)
{
	pthread_mutex_destroy(&q->head_lock);
	pthread_mutex_destroy(&q->tail_lock);
	free(q);
}
