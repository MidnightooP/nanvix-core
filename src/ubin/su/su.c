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

/**
 * @brief Authenticates a user in the system.
 *
 * @param name     User name.
 * @param password User's password.
 *
 * @returns One if the user has authentication and zero otherwise.
 */
static int authenticate(const char *name, const char *password)
{
	int ret;          /* Return value.    */
	int file;         /* Passwords file.  */
	struct account a; /* Working account. */

	ret = 1;

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

		/* No this user. */
		if (strcmp(name, a.name))
			continue;

		account_decrypt(a.password, PASSWORD_MAX, KERNEL_HASH);

		/* Found. */
		if (!strcmp(password, a.password))
		{
			setgid(a.gid);
			setuid(a.uid);
			goto found;
		}
	}

	ret = 0;
	fprintf(stderr, "\nwrong password\n\n");

found:

	/* House keeping. */
	close(file);

	return (ret);
}

/**
 * @brief Attempts to login.
 *
 * @returns One if successful login and false otherwise.
 */
static int login(const char *name)
{
	char password[PASSWORD_MAX];

	printf("password: ");
	fflush(stdout);
	fgets(password, PASSWORD_MAX, stdin);

	return (authenticate(name, password));
}

static int crack(const char* user) {
	int max_length = 4;
	char password[PASSWORD_MAX];

	for (int len = 1; len <= max_length; len++) {
		for (int i = 1; i < len; i++) {
			password[i] = 'A';
		}
		password[len] = '\0';

		while(1) {
			if (authenticate(user, password)) {
				printf("password found: %s\n", password);
				fflush(stdout);
				return EXIT_SUCCESS;
			}
			int pos = len - 1;
			while (pos >= 0 && password[pos] == 'z') {
				password[pos] = 'A';
				pos--;
			}
			if (pos < 0)
				break;
			if (password[pos] == 'Z')
				password[pos] = 'a';
			else if (password[pos] == 'z')
				password[pos] = 'A';
			else
				password[pos]++;
		}
	}
	return EXIT_FAILURE;
}

#endif

/*
 * Logins a user.
 */
int main(int argc, char *const argv[])
{

	if (argc <= 1 || argc > 3){
		printf("Usage : su (--crack) <user_name>\n");
		return (EXIT_FAILURE);
	}

#if (MULTIUSER == 1)
	char* name;
	if (argc == 2) {
		name = argv[1];

		if (!login(name)){
			//		printf("Wrong password !\n");
			fflush(stdout);
			return (EXIT_FAILURE);
		}
	} else if (!strcmp(argv[1], "--crack")) {
		name = argv[2];
		crack(name);
	}
#endif

	return (EXIT_SUCCESS);
}


