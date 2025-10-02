#include <nanvix/sem.h>

struct semaphore semtab[SEM_MAX];

struct semaphore semIDLE(){
	struct semaphore rv;
	rv.isUsed = 0;
	rv.key = 0;
	rv.counter = 0;
	rv.waitingHead = NULL;
	return rv;
}	

/**
 * @brief Initializes the semaphore system.
 */
void sem_init(void){
	for (int i = 0; i < SEM_MAX; i++){
		struct semaphore* sem = &semtab[i];
		sem->isUsed = 0;
		sem->key = 0;
		sem->counter = 0;
		sem->waitingHead = NULL;
	}
}
