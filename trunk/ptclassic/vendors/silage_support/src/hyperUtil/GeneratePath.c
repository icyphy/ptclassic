/* $Id$ */

/*
 * GeneratePath.c -- Generate the path elements in a sequence of lager files.
 *
 * Author:
 *	Paul Tjahjadi
 *	Brian C. Richards, August 7, 1989:
 *		Rewritten and separated from GetPath.c
 *
 * Routines:
 *	InitPathGenerator(key);
 *	PathGenerator();
 *	PathGeneratorRaw();
 */

#include "port.h"
#include "lisplike.h"

#define	FALSE 0
#define TRUE (!FALSE)

/*
 * The following are lager file parse tree roots for the library, user home,
 * the current working directory, and an optional user-specified directory.
 */
extern lnode	*all_lager_files;	/* From GetPath.c */
static lnode	*current_file_node;
static int	gen_source;
static char	ind[200];
static lnode	*path_list = (lnode *) NULL;

int
InitPathGenerator(indicator)
/*
 * InitPathGenerator() -- Specify an indicator for paths to generate
 *			  in a sequence of lager files.
 * Arguments:
 *	indicator	A string defining which paths in the lager file
 *			should be searched.
 *
 * Return Value:
 *			GetPath() returns TRUE when the generator could
 *			be initialized.
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
	char	*indicator;
{
	current_file_node = all_lager_files;
	(void) strcpy(ind, indicator);
	path_list = (lnode *) NULL;

	return (TRUE);
}

/*
 * PathGenerator() -- Return the next element in the given search path.
 *
 * Arguments:
 *	NONE.
 *
 * Return Value:
 *	A character pointer to the next string in the given path is
 *	returned, with environment variables substituted, and '~'
 *	expanded.  This string is owned by libLagerUtil.a, and must
 *	NOT be free()ed.  If there are no more strings in the given
 *	search path, then (char *)NULL is returned.
 */
char *
PathGenerator()
{
	lnode	*path_element;
	lnode	*expanded_path;
	lnode	*root;

	while ((path_list = CDR_P(path_list)) == (lnode *) NULL) {
		/* Check if there are more places to look. */
		if (!current_file_node) break;

		/* Scan the next lager file. */
		root = cdr_p(CAR_P(current_file_node));
		current_file_node = CDR_P(current_file_node);

		while (root) {
			path_list = CAR_P(root);
			path_element = CAR_P(path_list);
			if (IS_TEXT(path_element) &&
					!strcmp(THE_TEXT(path_element), ind)) {
				/* Found the desired path! */
				break;
			}

			root = CDR_P(root);
		}
	}

	if (!path_list) return (char *) NULL;

	path_element = CAR_P(path_list);
	if (IS_TEXT(path_element)) {
		ExpandDirectoryNode(&path_element);
		path_list->value.cons.a = path_element;
	}

	expanded_path = CDR_P(path_element);
	if (IS_DOTDOT(expanded_path)) expanded_path = CAR_P(expanded_path);

	if (!IS_TEXT(expanded_path)) return ((char *) NULL);

	return (THE_TEXT(expanded_path));
}

/*
 * PathGeneratorRaw() -- Return the next element in the given search path.
 *
 * Arguments:
 *	NONE.
 *
 * Return Value:
 *	A character pointer to the next string in the given path is
 *	returned.  This string may begin with a '~', but will otherwise
 *	be expanded.  The string is owned by libLagerUtil.a, and must
 *	NOT be free()ed.  If there are no more strings in the given
 *	search path, then (char *)NULL is returned.
 */
char *
PathGeneratorRaw()
{
	lnode	*path_element;
	lnode	*raw_path;
	lnode	*root;

	while ((path_list = CDR_P(path_list)) == (lnode *) NULL) {
		/* Check if there are more places to look. */
		if (!current_file_node) break;

		/* Scan the next lager file. */
		root = cdr_p(CAR_P(current_file_node));
		current_file_node = CDR_P(current_file_node);

		while (root) {
			path_list = CAR_P(root);
			path_element = CAR_P(path_list);
			if (IS_TEXT(path_element) &&
					!strcmp(THE_TEXT(path_element), ind)) {
				/* Found the desired path! */
				break;
			}

			root = CDR_P(root);
		}
	}

	if (!path_list) return (char *) NULL;

	path_element = CAR_P(path_list);
	if (IS_TEXT(path_element)) {
		ExpandDirectoryNode(&path_element);
		path_list->value.cons.a = path_element;
	}

	raw_path = CAR_P(path_element);

	if (!IS_TEXT(raw_path)) return ((char *) NULL);

	return (THE_TEXT(raw_path));
}
