#include <check.h>
#include <limits.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#include "../../../src/synchronization/cigarette_smokers/cigarette_smokers.h"

Suite *cigarette_smokers_suite(void);

START_TEST(TEST_CIGARETTE_SMOKERS)
{
	pthread_t agents[3];
	pthread_t smokers[3];

	int types[] = { 0, 1, 2 };

	smokers_init();

	for (int i = 0; i < 3; i++) {
		pthread_create(&agents[i], NULL, agent, &types[i]);
		pthread_create(&smokers[i], NULL, smoker, &types[i]);
	}

	for (int i = 0; i < 3; i++) {
		pthread_join(agents[i], NULL);
		pthread_join(smokers[i], NULL);
	}
	smokers_destroy();
}
END_TEST
