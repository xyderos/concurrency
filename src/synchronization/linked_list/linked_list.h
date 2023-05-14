#pragma once

#include <pthread.h>

typedef struct list_node {
	struct list_node *next;
	int key;
	char pad[(sizeof(struct list_node *)) - sizeof(int)];
} list_node_t;

typedef struct list {
	list_node_t *head;
	pthread_mutex_t lock;
} list_t;

list_t *list_init(void);

void list_insert(list_t *const, const int);

int list_lookup(list_t *const, const int);

void list_free(list_t *const);
