/* $Id$ */

/* Copyright (C) 1988 Regents of the University of California
 * All rights reserved.
 */
/*
 * GetUserPath.c -- Find the home directory for given users.
 * 	The path to the home directory for each user is placed in
 *	a hash table (using the OCT st package), for quick home
 *	directory retrieval.
 */

#include <stdio.h>
#include <pwd.h>
#include <string.h>
#include "st.h"

char		*lagerAllocText();

char *
GetUserPath(user)
	/*
	 * GetUserPath -- return a user's home directory.
	 * Arguments:
	 *	user	The user's name
	 * Returned Value:
	 *		A string containing the path to the user's home
	 *		directory, or NULL, if the user does not exist.
	 */
	char	*user;
{
	static st_table	*user_table = NULL;
	static int	more_users = 1;
	struct passwd	*ent;
	char		*puser;
	char		*home;

	if (!user_table) {
		/*
		 * Create the hash table for storing user home directories.
		 */
		user_table = st_init_table(strcmp, st_strhash);
		setpwent();
	}

	if (st_lookup(user_table, user, &home) != 0) {
		return (home);
	}

	if (!more_users) return (NULL);

	while (ent = getpwent()) {
		/* Save the password data in the hash table. */
		puser = lagerAllocText(ent->pw_name);
		home = lagerAllocText(ent->pw_dir);
		st_insert(user_table, puser, home);
		if (strcmp(user, puser) == 0)
			return (home);
	}

	/* No more password file entries! */
	endpwent();
	more_users = 0;

	return (NULL);
}
