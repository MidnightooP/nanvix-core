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

static int set_new_pwd(const char *name, const char *password)
{
	int file;         /* Passwords file.  */
	struct account a[10]; /* Working account. */
	int i = 0;

	/* Open passwords file. */
	if ((file = open("/etc/passwords", O_RDWR)) == -1)
	{
		fprintf(stderr, "cannot open password file\n");
		return (0);
	}

	/* Search in the  passwords file. */
	while (read(file, &(a[i]), sizeof(struct account)))
	{
		account_decrypt(a[i].name, USERNAME_MAX, KERNEL_HASH);
		account_decrypt(a[i].password, PASSWORD_MAX, KERNEL_HASH);
		
		/* If it is trhe right user, change password */
		if (!strcmp(name, a[i].name))
			strcpy(a[i].password,password);
		
		i++;
	}

	/* House keeping. */
	close(file);

	
	/* Copy users table (a) in /etc/passwords. */
	
	int nb_user = i;
	int file_wr;

	/* Open passwords file. */
	if ((file_wr = open("/etc/passwords", O_RDWR)) == -1)
	{
		fprintf(stderr, "cannot open password file in writing\n");
		return (0);
	}

	for(int j = 0; j < nb_user; j++){
		account_encrypt(a[j].name, USERNAME_MAX, KERNEL_HASH);
		account_encrypt(a[j].password, PASSWORD_MAX, KERNEL_HASH);

		write(file_wr, &(a[j]), sizeof(struct account));
	}

	/* House keeping. */
	close(file_wr);

	return (1);
}

static int new_pwd(const char* name){
	char password_1[PASSWORD_MAX];
	char password_2[PASSWORD_MAX];

	int to_print = 0;

	do{
	printf("new password: ");
	fflush(stdout);
	fgets(password_1, PASSWORD_MAX, stdin);

	printf("new password (verif): ");
	fflush(stdout);
	fgets(password_2, PASSWORD_MAX, stdin);
 
	if (to_print)
		printf("The two passwords don't match.\n");
	to_print = 1;

	}while (strcmp(password_1, password_2));

	return set_new_pwd(name, password_1);
}

#endif

/*
 * Logins a user.
 */
int main(int argc, char *const argv[])
{

	if (argc != 2){
		printf("Usage : passwd <user_name>\n");
		return (EXIT_FAILURE);
	}

	char* name = argv[1];
	
#if (MULTIUSER == 1)
	if (!login(name))
		return (EXIT_FAILURE);
	
	new_pwd(name);

#endif

	return (EXIT_SUCCESS);
}


