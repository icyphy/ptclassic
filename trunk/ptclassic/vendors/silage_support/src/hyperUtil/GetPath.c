/* $Id$ */

/* Copyright (C) 1988 Regents of the University of California
 * All rights reserved.
 *
 *
 * GetPath.c --
 *
 *
 *  2/3/89  Added capability to expand paths with environment variables
 *         in them.  Bob Reese
 */


#include "port.h"
#include "st.h"
#include <sys/file.h>
#include "lisplike.h"
#include "LagerMessage.h"

#define TRUE		1
#define FALSE		0
#define	HASH		TRUE
#define	NOHASH		FALSE
#define MAXPATHLEN	300
#define	NUM_BUFFERS	5
#define	GP_OK		0
#define	GP_NOPATH	1
#define	GP_NOTINPATH	2
#define	DEFAULT_LAGER_FILES	"hyper:~/hyper:$HYPER/lib/hyper"

char		*rindex(), *index();
char		*getenv(), *sprintf(), *getcwd();

extern 		int debugFlag;

int		GP_crash_on_error = FALSE;
int		GP_return_NULL = TRUE;
char		*QuietGetPath();
char		*GetUserPath();
char		*LUNextBuffer();
char		*ExpandFileName();

static int	use_expanded_path = TRUE;
static lnode	*found_path = NULL;
static char	path_raw[MAXPATHLEN];
static char	path_expanded[MAXPATHLEN];
static char	*path_to_use;
static int	GetPath_error;
static char	Message[1024];
lnode		*all_lager_files;
st_table	*gp_dir_table = (st_table *) NULL;

char *
GetPath(indicator, filename)
	char	*indicator, *filename;
{
	char	*r_value;

	use_expanded_path = TRUE;
	r_value = QuietGetPath(indicator, filename);

	if (r_value == NULL) {
		/*
		sprintf(Message, "FATAL ERROR: The file `%s' was not found\n",
					filename);
		lagerMessage(Message, RED_ALERT);
		*/
		if (GP_crash_on_error) {
			exit(-1);
		}
		if (GP_return_NULL) {
			return(NULL);
		} else {
			/* Just return the filename that was given. */
			return (filename);
		}
	}

	return (r_value);
}

char *
GetOctPath(indicator, cell, view, facet)
	char	*indicator;
	char	*cell;
	char	*view;
	char	*facet;
{
	char	full_cell_name[200];
	char	*result;

	/* OCT will expand tilde. */
	use_expanded_path = FALSE;

	sprintf(full_cell_name, "%s/%s/%s;", cell, view, facet);
	result = QuietGetPath(indicator, full_cell_name);
	if (result == NULL) {
		/*
		sprintf(Message, "FATAL ERROR: The file `%s' was not found\n",
					full_cell_name);
		lagerMessage(Message, RED_ALERT);
		*/
		if (GP_crash_on_error) {
			exit(-1);
		}
		return (NULL);
	}

	/* Remove the view and facet from the path. */
	result[strlen(result) - strlen(full_cell_name) + strlen(cell)] = NULL;
	return (result);
}

char *
QuietGetPath(indicator, filename)
/*
 * Arguments:
 *	indicator	A string defining which path in the lager file
 *			should be searched.
 *	filename	A string defining the name of the file to look
 *			for along the indicated path.
 *
 * Return Value:
 *			GetPath() returns a pointer to a string which is
 *			normally the full path to the file including the
 *			file name.  If the file was not found, NULL is
 *			returned.
 *
 * The lager file:
 *			The GetPath routine looks for its path database in
 *			the file lager in the current directory or, if not
 *			found, in the user's home directory.  The file
 *			can contain comments, denoted by a semicolon, and
 *			terminated by an end of line character.  Each path
 *			entry has the following format:
 *				(<indicator> dir1 dir2 . . .)
 *			The list is terminated by a closing parenthesis,
 *			and any white space, including a carriage return,
 *			is ignored.  A comment can be put in at the end
 *			of any line.
 */
	char	*indicator, *filename;
{
	int	status;
	lnode	*lager_ref;
	lnode	*lager_root;

	*path_raw = NULL;
	found_path = NULL;

	if (*filename == '/') {
		strcpy(path_raw, filename);
		strcpy(path_expanded, filename);
		found_path = lager_root;
		return (path_expanded);
	}

	if (*filename == '~') {
		strcpy(path_expanded, ExpandFileName(filename));
		strcpy(path_raw, filename);
		found_path = lager_root;
		return (path_expanded);
	}

	GetPath_error = 0;

	for (lager_ref = all_lager_files; lager_ref && gp_dir_table;
				lager_ref = CDR_P(lager_ref)) {
		lager_root = CAR_P(lager_ref);
		if ((status = SearchIn(CDR_P(lager_root), indicator,
				       filename, THE_TEXT(CAR_P(lager_root)),
				       HASH)) == GP_OK) {
			return (path_to_use);
		} else {
			GetPath_error |= status;
		}
	}

	/* Check directly in the file system, if not in the hash tables. */
	for (lager_ref = all_lager_files; lager_ref;
				lager_ref = CDR_P(lager_ref)) {
		lager_root = CAR_P(lager_ref);
		if ((status = SearchIn(CDR_P(lager_root), indicator,
				       filename, THE_TEXT(CAR_P(lager_root)),
				       NOHASH))	== GP_OK) {
			return (path_to_use);
		} else {
			GetPath_error |= status;
		}
	}

	if (GetPath_error == GP_NOPATH) {
		/* None of the files had the requested path indicator. */
		sprintf(Message,
			"The path indicator `%s' not found in a hyper file.\n",
			indicator);
		lagerMessage(Message, RED_ALERT);
		if (!GP_crash_on_error || GP_return_NULL) {
			return(NULL);
		}
		exit(-1);
	}

	*path_raw = NULL;
	return (NULL);
}

char *
ExpandEnvironment(name)
char *name;
{
        char	*env_name;
	char	*new_name;
	char	*dollar, *remainder;
	int	name_len;

	if ((dollar = index(name, '$')) == NULL) return (name);

	remainder = index(dollar + 1, '/');
	if (remainder) *remainder = 0;

	new_name = getenv(dollar + 1);
	if (!new_name) {
		sprintf(Message, "GetPath: FATAL ERROR: Cannot expand %s.",
				dollar + 1);
		lagerMessage(Message, RED_ALERT);
		sprintf(Message,
			"  Check your hyper file(s).\n");
		lagerMessage(Message, RED_ALERT);
		if (!GP_crash_on_error) {
			return(NULL);
		}
		exit(-1);
	}

	env_name = LUNextBuffer();

	if (dollar != name) {
		*dollar = 0;
		(void) strcpy(env_name, name);
		*dollar = '$';
	} else {
		env_name[0] = '\000';
	}

	(void) strcat(env_name, new_name);

	if (remainder) {
		*remainder = '/';
		(void) strcat(env_name, remainder);
	}

	return(ExpandEnvironment(env_name));
}

char *
ExpandDirectory(name)
	char	*name;
{
	char	*new_name;
	char	*lagerAllocText();

	if (!name) return ((char *) NULL);

	new_name = ExpandFileName(name);

	if (!new_name) {
		sprintf(Message,
			"GetPath: FATAL ERROR: Cannot expand %s.\n",
			name);
		lagerMessage(Message, RED_ALERT);
		sprintf(Message,
			"GetPath: FATAL ERROR: Check your hyper file(s).\n");
		lagerMessage(Message, RED_ALERT);
		if (!GP_crash_on_error) {
			return(NULL);
		}
		exit(-1);
	}

	return (lagerAllocText(new_name));
}

int
ExpandDirectoryNode(ln)
	lnode	**ln;
{
	char	*name;
	lnode	*create_dotdot();
	lnode	*create_string();
	char	*exp_env;
	lnode	*exp_dir;

	if (!IS_TEXT(*ln)) return;

	name = THE_TEXT(*ln);
	if (!name) return;

	exp_env = ExpandEnvironment(name);
	if (!exp_env) {
		sprintf(Message, "GetPath: FATAL ERROR: Cannot expand %s.\n",
			name);
		lagerMessage(Message, RED_ALERT);
		sprintf(Message,
			"GetPath: FATAL ERROR: Check your hyper file(s).\n");
		lagerMessage(Message, RED_ALERT);
		if (!GP_crash_on_error) {
			return(0);
		}
		exit(-1);
	}

	if (exp_env != name) {
		/* Save the expanded name in place of the original name. */
		THE_TEXT(*ln) = lagerAllocText(exp_env);
		free(name);
		name = THE_TEXT(*ln);
	}

	exp_dir = create_string(lagerAllocText(ExpandFileName(name)),
			LN_SYMBOL);

	/* Copy the lager file position info to the new node. */
	exp_dir->line_no = (*ln)->line_no;
	exp_dir->char_no = (*ln)->char_no;

	/* Replace the existing string node with a dotted pair. */
	*ln = create_dotdot(*ln, exp_dir, LN_DOTDOT);

	return(1);
}

ReadPath()
/*
 * ReadPath -- Set up the lager file database.
 *	The lager files referenced in the environment variable LAGERFILE
 *	will be read in, defining the search paths for the LagerIV tools.
 */
{
	FILE	*fp;
	char	*lager_file_list;
	char	*colon, *lager_file, *full_lager_file;
	lnode	*lager_root, *name_node;
	char	*hash_file;
	char	*message;

	if (gp_dir_table) {
		DHFreeDirectories(gp_dir_table);
		gp_dir_table = (st_table *) NULL;
	}

	/* Relative paths will have this path prepended. */
	SetRelativePath(getcwd(LUNextBuffer(), MAXPATHLEN));

	/* Get the search path for the lager files. */
	if (!getenv("HYPER")) putenv("HYPER=~hyper");
	if ((lager_file_list = getenv("HYPERFILES")) == NULL) {
		lager_file_list = DEFAULT_LAGER_FILES;
	}

	all_lager_files = (lnode *) NULL;

	for (lager_file = lager_file_list; lager_file; ) {
		if ((colon = strchr(lager_file, ':')) != (char *) NULL) {
			*colon = '\000';
		}

		full_lager_file =ExpandFileName(ExpandEnvironment(lager_file));

		/* Read in the lager file parse tree. */
		if ((fp = fopen(full_lager_file, "r")) == (FILE *) NULL) {
			lager_root = (lnode *) NULL;
		} else if ((lager_root = read_lisp(fp)) == (lnode *) NULL) {
			fclose(fp);
		} else {
			fclose(fp);
			all_lager_files = create_dotdot(create_dotdot(
				create_string(lagerAllocText(full_lager_file),
				LN_SYMBOL), lager_root, LN_DOTDOT),
				all_lager_files, LN_DOTDOT);

		}

		/* Process a directory hash file, if found. */
		hash_file = (char *) malloc(strlen(full_lager_file) + 7);
		sprintf(hash_file, "%s.files", full_lager_file);

		if ((fp = fopen(hash_file, "r")) != (FILE *) NULL) {
			if (!gp_dir_table) {
				gp_dir_table=st_init_table(strcmp, st_strhash);
			}
			DHReadTable(gp_dir_table, fp);
			fclose(fp);
		}

		free(hash_file);

		/* Find the next lager file. */
		if (colon) {
			*colon = ':';
			lager_file = colon + 1;
		} else {
			lager_file = (char *) NULL;
		}
	}

	/*
	 * The preceding loop prepended entries onto the list of lager
	 * files read in, so the order of the list must be reversed.
	 */
	all_lager_files = ReverseCons(all_lager_files);

	message = LUNextBuffer();

	if (!all_lager_files) {
		sprintf(message, "No 'hyper' file was found.  Searched for");
		sprintf(message, "%s the following files: %s\n", message,
				lager_file_list);
		lagerMessage(message, RED_ALERT);
		lagerMessage("Check the environment variable HYPERFILES\n",
				RED_ALERT);
		if (!GP_crash_on_error) {
			return(NULL);
		}
		exit (-1);
	}

	lagerMessage("The CAD tools will use the paths found in the",
			GREEN_ALERT);
	lagerMessage(" following hyper file(s) (in the order shown):",
			GREEN_ALERT);

	for (lager_root = all_lager_files; lager_root;
			lager_root = CDR_P(lager_root)) {
		sprintf(message, "\t\t%s",
				THE_TEXT(car_p(CAR_P(lager_root))));
		lagerMessage(message, GREEN_ALERT);
	}
}

/**************************pbt***********************
 * Modified by Brian C. Richards (7/11/89) to maintain the
 * unexpanded file names.
 */
int
SearchIn (all_paths, indicator, filename, path_name, hash)
	lnode	*all_paths;
	char	*indicator, *filename, *path_name;
	int	hash;	/* 1 => Use hash table; 0 => Use access(). */
{
	lnode	*ind_path, *path_element;
	lnode	*given_name, *expanded_name;
	int	GPerr = GP_NOTINPATH;
	char	*name;

	while (all_paths) {
		/* Search for a list starting with the indicator string. */
		ind_path = car_p(all_paths);
		path_element = car_p(ind_path);
		if (!IS_TEXT(path_element)) {
			sprintf(Message,
	"WARNING: found a non-string indicator in the hyper file `%s'.\n",
					path_name);
			lagerMessage(Message, RED_ALERT);
		}
		if (!strcmp(THE_TEXT(path_element), indicator)) break;

		all_paths = cdr_p(all_paths);
	}

	if (!all_paths) {
		return(GP_NOPATH);
	}

	/* Search for the file along the given path. */
	ind_path = cdr_p(ind_path);
	while (ind_path) {
		path_element = CAR_P(ind_path);
		if (IS_TEXT(path_element)) {
			if (ExpandDirectoryNode(&path_element) == 0) {
				return(GPerr);
			}
			ind_path->value.cons.a = path_element;
		}

		given_name = car_p(path_element);
		expanded_name = cdr_p(path_element);
		if (IS_DOTDOT(expanded_name)) {
			expanded_name = car_p(expanded_name);
		}

		name = THE_TEXT(expanded_name);
		(void) sprintf(path_expanded, "%s/%s", name, filename);

		if ((!hash && access(path_expanded, F_OK) == 0) ||
		    (hash && DHValidFile(gp_dir_table, name, filename))) {
			/* Found the file! */
			DHReadDirectory(gp_dir_table, THE_TEXT(expanded_name));
			GPerr = GP_OK;
			if (!found_path) {
				found_path = path_element;
				(void) sprintf(path_raw, "%s/%s",
					       THE_TEXT(given_name),filename);

				if (use_expanded_path) {
					path_to_use = path_expanded;
				} else {
					path_to_use = path_raw;
				}

				sprintf(Message,
				"Using '%s' found through the path in '%s'\n",
						path_raw, path_name);
				lagerMessage(Message, GREEN_ALERT);
				return(GPerr);
			} 
		}
		ind_path = cdr_p(ind_path);
	}

	return (GPerr);
}

char *
GPLastPathRaw()
{
	lnode	*raw;

	if (found_path) return (path_raw);

	return ((char *) NULL);
}

char *
GPLastPathExpanded()
{
	lnode	*expanded;

	if (found_path) return (path_expanded);

	return ((char *) NULL);
}

void
GPInit (crash_on_error, return_NULL)
	int crash_on_error, return_NULL;

{
	GP_crash_on_error	= crash_on_error;
	GP_return_NULL		= return_NULL;
}

char *
LUNextBuffer()
{
	static char	buf[NUM_BUFFERS][MAXPATHLEN];
	static int	buf_index = 0;

	if (buf_index >= NUM_BUFFERS) buf_index = 0;

	return (&buf[buf_index++][0]);
}

/* added for hyper */

lagerMessage(string, priority )
char *string;
int   priority;
{
    switch( priority ) {
      case RED_ALERT:
        printmsg(NULL, string );
        printmsg(NULL, "\n");
        exit (-1);
      case YELLOW_ALERT:
        printmsg(NULL, string );
        printmsg(NULL, "\n");
        break;
      case GREEN_ALERT:
	if (debugFlag == TRUE) {
            printmsg(NULL, string );
            printmsg(NULL, "\n");
	}
        break;
      default:
        printmsg(NULL, string );
        printmsg(NULL, "\n");
        break;
    }
}	
