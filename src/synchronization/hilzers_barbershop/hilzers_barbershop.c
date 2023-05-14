#include <sys/queue.h>

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "hilzers_barbershop.h"
static sem_t *cust_num_mutex, *sofaq_mutex, *chairq_mutex, *cust_arrive,
    *cust_on_sofa, *cust_leave_sofa, *payment, *receipt, *cashier_mutex;
static int max_capacity = 8, customer_num = 0;

struct entry {
	sem_t *cust_sem;
	int index;
	STAILQ_ENTRY(entry) entries;
};
STAILQ_HEAD(stailhead, entry);
static struct stailhead wait_sofaq_head, wait_chairq_head;

void
hilzers_barbershop_init(void)
{
	STAILQ_INIT(&wait_sofaq_head);
	STAILQ_INIT(&wait_chairq_head);
	cust_num_mutex = malloc(sizeof(sem_t));
	sem_init(cust_num_mutex, 0, 1);
	sofaq_mutex = malloc(sizeof(sem_t));
	sem_init(sofaq_mutex, 0, 1);
	chairq_mutex = malloc(sizeof(sem_t));
	sem_init(chairq_mutex, 0, 1);
	cust_arrive = malloc(sizeof(sem_t));
	sem_init(cust_arrive, 0, 0);
	cust_on_sofa = malloc(sizeof(sem_t));
	sem_init(cust_on_sofa, 0, 0);
	cust_leave_sofa = malloc(sizeof(sem_t));
	sem_init(cust_leave_sofa, 0, 0);
	payment = malloc(sizeof(sem_t));
	sem_init(payment, 0, 0);
	receipt = malloc(sizeof(sem_t));
	sem_init(receipt, 0, 0);
	cashier_mutex = malloc(sizeof(sem_t));
	sem_init(cashier_mutex, 0, 1);
}

void
hilzers_barbershop_destroy(void)
{
	sem_destroy(cust_num_mutex);
	free(cust_num_mutex);
	sem_destroy(sofaq_mutex);
	free(sofaq_mutex);
	sem_destroy(chairq_mutex);
	free(chairq_mutex);
	sem_destroy(cust_arrive);
	free(cust_arrive);
	sem_destroy(cust_on_sofa);
	free(cust_on_sofa);
	sem_destroy(cust_leave_sofa);
	free(cust_leave_sofa);
	sem_destroy(payment);
	free(payment);
	sem_destroy(receipt);
	free(receipt);
	sem_destroy(cashier_mutex);
	free(cashier_mutex);
}

void *
hilzers_barber(void *arg)
{
	int index = *(int *)arg;
	struct entry *e = NULL;

	while (1) {
		sem_wait(cust_on_sofa);
		sem_wait(chairq_mutex);

		e = STAILQ_FIRST(&wait_chairq_head);
		STAILQ_REMOVE_HEAD(&wait_chairq_head, entries);

		sem_post(chairq_mutex);
		sem_post(e->cust_sem);

		printf("Barber %d cuts customer %d's hair.\n", index, e->index);

		sem_wait(payment);
		sem_wait(cashier_mutex);

		printf("Barber %d accepts payment.\n", index);

		sem_post(cashier_mutex);
		sem_post(receipt);
	}
	return NULL;
}

static struct entry *
init_entry(int index)
{
	sem_t *cust_sem = NULL;
	struct entry *e = NULL;

	cust_sem = malloc(sizeof(sem_t));

	sem_init(cust_sem, 0, 0);

	e = malloc(sizeof(struct entry));
	e->cust_sem = cust_sem;
	e->index = index;

	return e;
}

static void
free_entry(struct entry *e)
{
	sem_destroy(e->cust_sem);

	free(e->cust_sem);
	free(e);
}

void *
hilzers_customer(void *arg)
{
	int index = *(int *)arg;
	char name[BUFSIZ];
	struct entry *e = NULL;

	sem_wait(cust_num_mutex);

	if (customer_num == max_capacity) {

		sem_post(cust_num_mutex);

		printf("Customer %d balks.\n", index);

		pthread_exit(NULL);
	}

	customer_num++;

	printf("Customer %d arrives.\n", index);

	sem_post(cust_num_mutex);
	sem_wait(sofaq_mutex);

	(void)sprintf(name, "/cust_sofa_%d", index);

	e = init_entry(index);

	STAILQ_INSERT_TAIL(&wait_sofaq_head, e, entries);

	sem_post(sofaq_mutex);

	sem_post(cust_arrive);
	sem_wait(e->cust_sem);

	free_entry(e);
	printf("Customer %d sits on sofa.\n", index);

	sem_wait(chairq_mutex);
	(void)sprintf(name, "/cust_chair_%d", index);

	e = init_entry(index);

	STAILQ_INSERT_TAIL(&wait_chairq_head, e, entries);

	sem_post(chairq_mutex);
	sem_post(cust_on_sofa);
	sem_wait(e->cust_sem); // get chair

	free_entry(e);

	sem_post(cust_leave_sofa);

	printf("Customer %d gets hair cut.\n", index);

	sem_post(payment);
	sem_wait(receipt);
	sem_wait(cust_num_mutex);

	customer_num--;

	printf("Customer %d leaves.\n", index);

	sem_post(cust_num_mutex);

	return NULL;
}

void *
usher(void *arg)
{
	int index = *(int *)arg;
	struct entry *e = NULL;

	while (1) {

		sem_wait(cust_arrive);
		sem_wait(sofaq_mutex);

		e = STAILQ_FIRST(&wait_sofaq_head);
		STAILQ_REMOVE_HEAD(&wait_sofaq_head, entries);

		sem_post(sofaq_mutex);
		sem_post(e->cust_sem);

		printf("Usher %d guides customer %d to sofa.\n", index,
		    e->index);

		sem_wait(cust_leave_sofa);
	}
	return NULL;
}
