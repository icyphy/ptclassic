/* $Id$ */

/*
 * DirectoryHash.c -- Routines for maintaining tables of directory contents.
 */

#include "port.h"
#include "st.h"
#include "lisplike.h"
#include "LagerMessage.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

static char	Message[1024];
extern lnode	*all_lager_files;

void
DHReadDirectory(dir_table, directory_name)
	st_table	*dir_table;
	char		*directory_name;
{
	DIR		*dp;
	struct dirent	*de;
	char		*key;
	st_table	**table;

	if (dir_table == (st_table *) 0) return;

	key = (char *) lagerAllocText(directory_name);
	if (st_find_or_add(dir_table, key, (char ***) &table) == 0) {
		/* New table entry. */
		*table = st_init_table(strcmp, st_strhash);
	} else {
		/* The table exists already. */
		free(key);
		return;
	}

	if (chdir(directory_name) < 0) return;

	if (!(dp = opendir(directory_name))) return;
	while ((de = readdir(dp)) != (struct dirent *) NULL) {
		struct stat	sbuf;

		if (stat(de->d_name, &sbuf) < 0) continue;
		if (sbuf.st_mode & S_IFDIR) continue; /* Skip directories. */

		st_insert(*table, lagerAllocText(de->d_name), (char *) 0);
	}

	closedir(dp);
}

DHReplaceDirectory(dir_table, directory_name)
	st_table	*dir_table;
	char		*directory_name;
{
	if (!dir_table) return;

	if (st_is_member(dir_table, directory_name)) {
		DHDeleteDirectory(dir_table, directory_name);
	}

	DHReadDirectory(dir_table, directory_name);
}

int
DHValidFile(dir_table, dir, file)
	st_table	*dir_table;
	char		*dir, *file;
{
	st_table	*table;

	if (!dir_table) return (0);
	if (*dir != '/') {
		/* Load in relative directories. */
		DHReadDirectory(dir_table, dir);
	}
	if (st_lookup(dir_table, dir, (char **) &table) == 0) return (0);
	if (st_is_member(table, file) == 0) return (0);
	return (1);
}

DHDeleteDirectory(dir_table, directory_name)
	st_table	*dir_table;
	char		*directory_name;
{
	st_table	*table;
	st_generator	*gen;
	char		*name = directory_name;

	if (!dir_table) return;

	if (st_delete(dir_table, &name, (char **) &table) == 0) {
		/* Nothing deleted. */
		return;
	}

	free(name);
	gen = st_init_gen(table);
	while (st_gen(gen, &name, (char **)0) != 0) {
		free(name);
	}

	st_free_table(table);
}

DHFreeDirectories(dir_table)
	st_table	*dir_table;
{
	st_table	*table;
	st_generator	*d_gen, *f_gen;
	char		*name;

	if (!dir_table) return;

	st_foreach_item(dir_table, d_gen, &name, (char **) &table) {
		free(name);
		st_foreach_item(table, f_gen, &name, (char **)0) {
			free(name);
		}
		st_free_table(table);
	}
	st_free_table(dir_table);
}

void
DHReadTable(dir_table, fp)
	st_table	*dir_table;
	FILE		*fp;
{
	st_table	*table = (st_table *) 0;
	char		line[MAXNAMLEN];
	char		name[MAXNAMLEN];

	if (!dir_table) return;

	while (fgets(line, MAXNAMLEN, fp) != (char *) 0) {
		switch (line[0]) {
		      case ' ':
		      case '\t':
			/* A file entry. */
			if (!table) break;
			sscanf(line, "%s", name);
			st_insert(table, lagerAllocText(name), (char *) 0);
			break;
		      case '/':
			/* Create a new directory entry. */
			sscanf(line, "%s", name);
			if (st_is_member(dir_table, name) == 1) {
				/* Skip this directory. */
				table = (st_table *) NULL;
				break;
			}

			table = st_init_table(strcmp, st_strhash);
			st_add_direct(dir_table, lagerAllocText(name),
				      (char *) table);
			break;
		}
	}
}

int
DHDumpTable(dir_table, fp)
	st_table	*dir_table;
	FILE		*fp;
{
	st_generator	*dir_gen, *file_gen;
	char		*dir, *file;
	st_table	*table;

	if (!dir_table) return;

	dir_gen = st_init_gen(dir_table);
	while (st_gen(dir_gen, &dir, (char **) &table)) {
		fprintf(fp, "%s\n", dir);
		file_gen = st_init_gen(table);
		while (st_gen(file_gen, &file, (char **) 0)) {
			fprintf(fp, "\t%s\n", file);
		}
		st_free_gen(file_gen);
	}
	st_free_gen(dir_gen);
}

DHAddLagerFilesToTable(dir_table)
	st_table	*dir_table;
{
	lnode		*all;

	for (all = all_lager_files; all; all = CDR_P(all)) {
		DHAddLagerFileToTable(dir_table, CAR_P(all));
	}
}

DHAddLagerFileToTable(dir_table, root)
	st_table	*dir_table;
	lnode		*root;
{
	lnode		*ln, *path, *element, *expanded_name;
	char		*filename, *name;

	if (!root || !dir_table) return;

	ln = CAR_P(root);
	filename = THE_TEXT(ln);

	while (root = CDR_P(root)) {
		path = car_p(root);
		element = car_p(path);
		if (!IS_TEXT(element)) {
			sprintf(Message,
	"WARNING: found a non-string indicator in the lager file `%s'.\n",
				filename);
			lagerMessage(Message, RED_ALERT);
		}

		while (path = CDR_P(path)) {
			element = CAR_P(path);
			if (IS_TEXT(element)) {
				if (ExpandDirectoryNode(&element) == 0) {
					continue;
				}
				path->value.cons.a = element;
			}

			expanded_name = cdr_p(element);
			if (IS_DOTDOT(expanded_name)) {
				expanded_name = CAR_P(expanded_name);
			}
			name = THE_TEXT(expanded_name);
			if (*name != '/') continue; /* No relative paths. */
			DHReadDirectory(dir_table, THE_TEXT(expanded_name));
		}
	}
}
