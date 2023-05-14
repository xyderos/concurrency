#pragma once

void fifo_barbershop_init(void);

void *fifo_barber(void *);

void *fifo_customer(void *);

void fifo_barbershop_destroy(void);
