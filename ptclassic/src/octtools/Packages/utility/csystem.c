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
/* LINTLIBRARY */
#include "copyright.h"
#include "port.h"
#include <sys/wait.h>
#include "utility.h"

int
util_csystem(s)
char *s;
{
    register SIGNAL_FN (*istat)(), (*qstat)();
#ifdef WAIT_TAKES_INT_STAR
    int status;    
#else
    union wait status;
#endif
    int pid, w, retval;

    if ((pid = vfork()) == 0) {
	(void) execl("/bin/csh", "csh", "-f", "-c", s, 0);
	(void) _exit(127);
    }

    /* Have the parent ignore interrupt and quit signals */
    istat = signal(SIGINT, SIG_IGN);
    qstat = signal(SIGQUIT, SIG_IGN);

    while ((w = wait(&status)) != pid && w != -1)
	    ;
    if (w == -1) {		/* check for no children ?? */
	retval = -1;
    } else {
#ifdef WAIT_TAKES_INT_STAR
	retval = status;
#else
	retval = status.w_status;
#endif
    }

    /* Restore interrupt and quit signal handlers */
    (void) signal(SIGINT, istat);
    (void) signal(SIGQUIT, qstat);
    return retval;
}
