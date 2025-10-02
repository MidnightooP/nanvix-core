/**
 * @file nanvix/sem.h
 *
 * @brief Semaphores system
 */

#include <nanvix/const.h>

#ifndef NANVIX_SEM_H_
#define NANVIX_SEM_H_

#define SEM_MAX 10

struct semaphore
{
	int isUsed;
	unsigned key;
	int counter;
	struct process** waitingHead;
};

EXTERN struct semaphore semtab[SEM_MAX];

EXTERN void sem_init(void);

#endif
