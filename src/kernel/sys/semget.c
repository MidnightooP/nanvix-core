#include <nanvix/const.h>
#include <nanvix/sem.h>

PUBLIC int sys_semget(unsigned key) {
	for (int i = 0; i < SEM_MAX; i++) {
		if (semtab[i].isUsed && semtab[i].key == key)
			return i;
	}

	int i = 0;
	while (semtab[i].isUsed && i < SEM_MAX)
		i++;

	if (i >= SEM_MAX)
		return -1;
	
	(&semtab[i])->key = key;
	(&semtab[i])->isUsed = 1;
	return i;
}
