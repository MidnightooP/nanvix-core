#include <nanvix/const.h>
#include <nanvix/sem.h>
#include <sys/sem.h>
#include <nanvix/klib.h>

PUBLIC int sys_semctl(int semid, int cmd, int val) {
	if (semid >= SEM_MAX || semid < 0)
		return -1;

	struct semaphore *sem = &semtab[semid];

	switch(cmd) {
		case GETVAL:
			return sem->counter;
		case SETVAL:
			sem->counter = val;
			return 0;
		case IPC_RMID:
			sem->isUsed = 0;
			wakeup(curr_proc->chain);
			return 0;
		default:
			return -1;
	}
}
