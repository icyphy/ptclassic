#ifndef lint
static char SccsId[]="$Id$";
#endif /*lint*/
/*
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the
above copyright notice and the following two paragraphs appear in all
copies of this software.

IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY
FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES
ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF
THE UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF
SUCH DAMAGE.

THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE
PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE UNIVERSITY OF
CALIFORNIA HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
ENHANCEMENTS, OR MODIFICATIONS.

						PT_COPYRIGHT_VERSION_2
						COPYRIGHTENDKEY
*/
/*
 * Measurements
 *
 * David Harrison
 * University of California, Berkeley
 * 1988
 *
 * This routine produces statistics on VEM usage.  For each session,
 * it records the user and machine, date, and the command count for each
 * command in the file /cad/lib/vem/measurefile.
 */

#include "general.h"
#include <pwd.h>
#include <time.h>
#include "st.h"
#define MEASURE_FILE	"~cad/lib/vem/measurefile"

#ifdef MF
static st_table *cmd_table = (st_table *) 0;
#endif

void m_command(uid)
STR uid;			/* Unique command identifier */
/*
 * Increments the cound for the command with unique identifier `uid'.
 */
{
#ifdef MF
    long *count;

    if (!cmd_table) cmd_table = st_init_table(strcmp, st_strhash);
    if (st_lookup(cmd_table, uid, (char **) &count)) {
	(*count)++;
    } else {
	count = VEMALLOC(long);
	*count = 1;
	st_insert(cmd_table, uid, (char *) count);
    }
#endif /* MF */
}

#ifdef MF
static enum st_retval wr_cmd(key, value, arg)
char *key, *value, *arg;
/*
 * Writes out info on a command.  The command is given as
 * `key', number of times invoked is `value', and file
 * is `arg'.
 */
{
    FILE *m_file = (FILE *) arg;
    int *count = (int *) value;

    (void) fprintf(m_file, "%s %d\n", key, *count);
    return ST_CONTINUE;
}
#endif /* MF */

void m_flush()
/*
 * Flushes information to measurement file if writable.
 */
{
#ifdef MF
    FILE *m_file;
    char host_name[1024], *t_str;
    int uid, euid;
    long clock;
    struct passwd *pw_ent;
    
    if (m_file = fopen(util_file_expand(MEASURE_FILE), "a")) {
	/* Write header information */
	uid = getuid();
	euid = geteuid();
	if (pw_ent = getpwuid(uid)) {
	    (void) fprintf(m_file, "----- %s", pw_ent->pw_name);
	}
	if ((euid != uid) && (pw_ent = getpwuid(euid))) {
	    (void) fprintf(m_file, " (as %s)", pw_ent->pw_name);
	}
	if (gethostname(host_name, 1024) == 0) {
	    (void) fprintf(m_file, " at %s", host_name);
	}
	(void) time(&clock);
	t_str = ctime(&clock);
	(void) fprintf(m_file, ", %s", t_str);
	/* Write command information */
	st_foreach(cmd_table, wr_cmd, (char *) m_file);
	(void) fclose(m_file);
    }
#endif /* MF */
}
