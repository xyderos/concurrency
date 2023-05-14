#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>

#include "cigarette_smokers.h"

static sem_t *tobacco_sem, *paper_sem, *match_sem, *agent_sem, *lock;
static int has_tobacco = 0, has_paper = 0, has_match = 0;

void
smokers_init(void)
{
	tobacco_sem = malloc(sizeof(sem_t));
	sem_init(tobacco_sem, 0, 0);
	paper_sem = malloc(sizeof(sem_t));
	sem_init(paper_sem, 0, 0);
	match_sem = malloc(sizeof(sem_t));
	sem_init(match_sem, 0, 0);
	agent_sem = malloc(sizeof(sem_t));
	sem_init(agent_sem, 0, 1);
	lock = malloc(sizeof(sem_t));
	sem_init(lock, 0, 1);
}

void
smokers_destroy(void)
{
	sem_destroy(tobacco_sem);
	free(tobacco_sem);
	sem_destroy(paper_sem);
	free(paper_sem);
	sem_destroy(match_sem);
	free(match_sem);
	sem_destroy(agent_sem);
	free(agent_sem);
	sem_destroy(lock);
	free(lock);
}

static void
tobacco_pusher(void)
{
	if (has_paper) {
		has_paper = 0;
		sem_post(match_sem);
	} else if (has_match) {
		has_match = 0;
		sem_post(paper_sem);
	} else {
		has_tobacco = 1;
	}
}

static void
paper_pusher(void)
{
	if (has_match) {
		has_match = 0;
		sem_post(tobacco_sem);
	} else if (has_tobacco) {
		has_tobacco = 0;
		sem_post(match_sem);
	} else {
		has_paper = 1;
	}
}

static void
match_pusher(void)
{
	if (has_paper) {
		has_paper = 0;
		sem_post(tobacco_sem);
	} else if (has_tobacco) {
		has_tobacco = 0;
		sem_post(paper_sem);
	} else {
		has_match = 1;
	}
}

void *
agent(void *arg)
{
	int type = *(int *)arg;
	sem_wait(agent_sem);
	sem_wait(lock);

	switch (type) {
	case 0:
		tobacco_pusher();
		paper_pusher();
		break;
	case 1:
		paper_pusher();
		match_pusher();
		break;
	case 2:
		tobacco_pusher();
		match_pusher();
	}

	sem_post(lock);

	return NULL;
}

void *
smoker(void *arg)
{
	int type = *(int *)arg;

	switch (type) {
	case 0:
		sem_wait(tobacco_sem);
		printf(
		    "Smoker with tobacco is getting a little bit of cancer.\n");
		sem_post(agent_sem);
		break;
	case 1:
		sem_wait(paper_sem);
		printf(
		    "Smoker with paper is getting a little bit of cancer.\n");
		sem_post(agent_sem);
		break;
	case 2:
		sem_wait(match_sem);
		printf(
		    "Smoker with match is getting a little bit of cancer.\n");
		sem_post(agent_sem);
	}
	return NULL;
}
