/*
 * Copyright(C) 2011-2016 Pedro H. Penna <pedrohenriquepenna@gmail.com>
 *
 * This file is part of Nanvix.
 *
 * Nanvix is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * Nanvix is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Nanvix. If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stropts.h>
#include <unistd.h>
#include <string.h>

#include <sys/utsname.h>

#include <nanvix/accounts.h>
#include <nanvix/config.h>
#include <dev/tty.h>

#if (MULTIUSER == 1)

int main()
{
	int file;         /* Passwords file.  */
	struct account a; /* Working account. */

	/* Open passwords file. */
	if ((file = open("/etc/passwords", O_RDONLY)) == -1)
	{
		fprintf(stderr, "cannot open password file\n");
		return (0);
	}

	/* Search in the  passwords file. */
	while (read(file, &a, sizeof(struct account)))
	{
		account_decrypt(a.name, USERNAME_MAX, KERNEL_HASH);

		printf("user name : %s \nuid : %d \ngid : %d \n",a.name,a.uid,a.gid); 
		
	}

	/* House keeping. */
	close(file);

	return (EXIT_SUCCESS);
}

#endif
