/* $Id$ */

/* Copyright (C) 1988 Regents of the University of California
 * All rights reserved.
 */
/*
 * Expand file names.
 */
#include "port.h"
#include "LagerMessage.h"
#include "utility.h"

char			*GetUserPath();
char			*getenv(), *sprintf();

static char		*relative = "";
static char		relative_holder[1024];
static char		Message[1024];

char *
ExpandFileName(name)
	char	*name;
{
	char	*home;
	char	*LUNextBuffer();

	home = LUNextBuffer();

	if (*name == '/') {
		strcpy(home, name);
		return (home);
	}

	if (name[0] == '.' && relative[0] != '\000') {
		strcpy(home, relative);
		if (name[1] == '\000') {
			return (home);
		} else if (name[1] == '/') {
			if (name[2] == '\000')
				return (home);
			else
				return (strcat(home, &name[1]));
		} else {
			return (strcat(strcat(home, "/"), name));
		}
	}

	if (*name == '~') {
		/* Do the tilde expansion. */
		return (util_tilde_expand(name));
	}

	/*
	 * The file is relative to the path relative[].
	 */
	if (relative[0] == '\000') {
		strcpy(home, name);
		return (home);
	}

	sprintf(home, "%s/%s", relative, name);
	return (home);
}

SetRelativePath(name)
	char	*name;
{
	if (name) {
		strcpy(relative_holder, name);
		relative = relative_holder;
	} else {
		relative = "";
	}
}
