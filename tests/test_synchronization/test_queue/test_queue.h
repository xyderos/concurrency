#include <check.h>
#include <limits.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#include "../../../src/synchronization/queue/queue.h"

Suite *queue_suite(void);

START_TEST(TEST_QUEUE)
{
	queue_t *q = queue_init();
	queue_enqueue(q, 0);
	queue_enqueue(q, 1);
	queue_enqueue(q, 2);
	queue_enqueue(q, 3);
	queue_dequeue(q);
	queue_dequeue(q);

	queue_destroy(q);
}
END_TEST
