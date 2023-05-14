#include <sys/time.h>

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include "linked_list.h"

list_t *
list_init(void)
{
	list_t *to_return = malloc(sizeof(list_t));

	to_return->head = NULL;

	pthread_mutex_init(&to_return->lock, NULL);

	return to_return;
}

void
list_insert(list_t *const lst, const int key)
{
	list_node_t *to_be_inserted = malloc(sizeof(list_node_t));
	if (!to_be_inserted) {
		return;
	}
	to_be_inserted->key = key;

	pthread_mutex_lock(&lst->lock);

	to_be_inserted->next = lst->head;
	lst->head = to_be_inserted;

	pthread_mutex_unlock(&lst->lock);
}

int
list_lookup(list_t *const lst, const int key)
{
	list_node_t *iterator = NULL;

	int rv = -1;

	pthread_mutex_lock(&lst->lock);

	iterator = lst->head;

	while (iterator) {
		if (iterator->key == key) {
			rv = 0;
			break;
		}
		iterator = iterator->next;
	}
	pthread_mutex_unlock(&lst->lock);

	return rv;
}

void
list_free(list_t *const lst)
{
	list_node_t *iterator = NULL, *tmp = NULL;

	pthread_mutex_lock(&lst->lock);

	iterator = lst->head;

	while (iterator) {

		tmp = iterator;
		iterator = iterator->next;

		free(tmp);
	}
	pthread_mutex_unlock(&lst->lock);

	free(lst);
}
