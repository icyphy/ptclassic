#ifndef lint
static char SccsId[]="$Id$";
#endif /*lint*/
/* Copyright (c) 1990-1993 The Regents of the University of California.
 * All rights reserved.
 * 
 * Permission is hereby granted, without written agreement and without
 * license or royalty fees, to use, copy, modify, and distribute this
 * software and its documentation for any purpose, provided that the above
 * copyright notice and the following two paragraphs appear in all copies
 * of this software.
 * 
 * IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY 
 * FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES 
 * ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF 
 * THE UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF 
 * SUCH DAMAGE.
 * 
 * THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE
 * PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE UNIVERSITY OF
 * CALIFORNIA HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
 * ENHANCEMENTS, OR MODIFICATIONS.
 * 
 */
#include "port.h"
#include "errtrap.h"

/*ARGSUSED*/
int
main(argc, argv)
int argc;
char *argv[];
{
    char *pkg, *msg;
    int code;
    void testFunc(), benignHandler();

    errProgramName(argv[0]);

    ERR_IGNORE(testFunc());
    if (errStatus(&pkg, &code, &msg)) {
	(void) fprintf(stderr, "Ignored error %d from `%s' package: %s\n",
		    code, pkg, msg);
    }

    errPushHandler(benignHandler);
    testFunc();
    /* shouldn't be reached */
    return 0;
}

void testFunc()
{
    errRaise("test", 0, "formatted message: (1)=%d; (hello)=%s", 1, "hello");
}

void benignHandler(pkgName, code, message)
char *pkgName;
int code;
char *message;
{
    (void) fprintf(stderr, "Error %d from `%s' package: %s\n",
		    code, pkgName, message);
    exit(0);
}
