#include <sys/resource.h>

#include <asm-generic/errno-base.h>
#include <check.h>
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "driver.h"

// in order to be able to run the program with setting scheduling requests
// you need to interact with the system by setting limits
static int
setup_environment(void)
{
	struct rlimit r;

	r.rlim_max = 50;
	r.rlim_cur = 20;

	return setrlimit(RLIMIT_RTPRIO, &r);
}
int
main(void)
{
	int no_failed = 0;
	Suite *s = NULL;
	SRunner *runner = NULL;

	if (setup_environment() == -1) {
		switch (errno) {
		case EFAULT:
			strerror(errno);
			break;
		case EINVAL:
			strerror(errno);
			break;
		case EPERM:
			fprintf(stderr, "%s \n", strerror(errno));
			break;
		case ESRCH:
			strerror(errno);
			break;
		default:
			break;
		}
	}

	runner = srunner_create(s);
	srunner_add_suite(runner, lifecycle_suite());
	srunner_add_suite(runner, barrier_suite());
	srunner_add_suite(runner, coordinate_suite());
	srunner_add_suite(runner, row_suite());
	srunner_add_suite(runner, quicksort_suite());
	srunner_add_suite(runner, cigarette_smokers_suite());
	srunner_add_suite(runner, dining_savages_suite());
	srunner_add_suite(runner, fifo_barbershop_suite());
	srunner_add_suite(runner, hilzers_barbershop_suite());
	srunner_add_suite(runner, linked_list_suite());
	srunner_add_suite(runner, mutex_no_starve_suite());
	srunner_add_suite(runner, queue_suite());
	srunner_add_suite(runner, random_barbershop_suite());
	srunner_add_suite(runner, locks_suite());

	srunner_run_all(runner, CK_NORMAL);
	no_failed = srunner_ntests_failed(runner);
	srunner_free(runner);

	return (no_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
