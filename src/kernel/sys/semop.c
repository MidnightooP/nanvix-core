#include <nanvix/const.h>
#include <nanvix/sem.h>
#include <nanvix/hal.h>
#include <nanvix/pm.h>

void up(struct semaphore *sem) {
	disable_interrupts();	
	sem->counter ++;
	enable_interrupts();
	if (sem->counter <= 0)
		wakeupone(sem->waitingHead);
}

void down(struct semaphore *sem) {
	disable_interrupts();	
	sem->counter--;
	enable_interrupts();
	if (sem->counter < 0)
		sleep(sem->waitingHead, curr_proc->priority);
}

/*
 * @brief ask/free abs(@op) resources to the semaphore @semid
 */
PUBLIC int sys_semop(int semid, int op){
	if (semid >= SEM_MAX || semid < 0)
		return -1;

	struct semaphore *sem = &semtab[semid];

	if (!sem->isUsed || op == 0)
		return -1;

	/* CS */
	if(op < 0)
		down(sem);
	else
		up(sem);
	/* END CS */

	return 0;
}

