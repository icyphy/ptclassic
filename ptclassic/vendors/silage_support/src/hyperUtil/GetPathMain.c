/* $Id$ */

/* Copyright (C) 1988 Regents of the University of California
 * All rights reserved.
 */
/*
 * GetPathMain.c
 *	This program is the main routine for the command getpath,
 *	which can be used from a shell to find the location of a
 *	Lager program or database.
 */

#include "port.h"
#include "st.h"

char log_file[1024];
FILE *log_fp = stderr;
char Message[1024];
int	verbose_option = 0;
int 	debugFlag = 0;
int     RpcFlag = 0;
int	create_table = 0;
char	*indicator = NULL;
char	*filename = NULL;
char	*path_specified = NULL;
extern	void	ParseCommandLine();
extern st_table	*gp_dir_table;


main(argc, argv)
	int	argc;
	char	*argv[];
{
	char	*return_value, *QuietGetPath();

	ParseCommandLine(argc, argv);
	GPInit (1, 1);

	if (create_table && indicator) {
		/* Modify the list of lager files. */
		char	*env;

		env = (char *) malloc(strlen(indicator) + 12);
		(void) sprintf(env, "LAGERFILES=%s", indicator);
		putenv(env);
	}

	/* Read the getpath database. */
	ReadPath(path_specified);

	if (create_table) {
		DHFreeDirectories(gp_dir_table);
		gp_dir_table = st_init_table(strcmp, st_strhash);
		DHAddLagerFilesToTable(gp_dir_table);
		DHDumpTable(gp_dir_table, stdout);
		exit (0);
	}

	return_value = QuietGetPath(indicator, filename);

	if (return_value) printf("%s\n", return_value);
	else printf("NULL\n");

	exit (0);
}

Usage()
{
	printmsg(NULL, "Usage: Getpath [-v] <identifier> <file>\n");
	printmsg(NULL, "	returns a full path or 'NULL' if not found\n");
	printmsg(NULL, "Usage: Getpath -c <hyperfile>[{:<hyperfile>}]\n");
	printmsg(NULL, "	Send a table of directorys to stdout.\n");
	printmsg(NULL, "	The lager files can be given explicitly.\n");
	printmsg(NULL, "\nOption:\t-v	verbose\n\n");
	exit(-1);
}

void
ParseCommandLine(argc, argv)

	int	argc;
	char	**argv;
{
	while (*++argv) {
		if (argv[0][0] == '-') {
			switch (argv[0][1]) {
			      case 'v':
				verbose_option = 1;
				break;
			      case 'd':
				debugFlag = 1;
				break;
			      case 'c':
			      case 'h':
				create_table = 1;
				break;
			}
		} else {
			if (!indicator) {
				indicator = *argv;
			} else if (!filename) {
				filename = *argv;
			} else {
				Usage();
			}
		}
	}
	if ((!indicator || !filename) && !create_table) Usage();
}

myMessage(Message)
char *Message;
{
}
