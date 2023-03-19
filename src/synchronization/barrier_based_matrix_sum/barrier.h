#pragma once

// barrier based matrix sum, a thread is waiting for the rest to finish up and then will finalize the result
int barrier_based_matrix_sum(void);
