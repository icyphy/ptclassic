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
#include "copyright.h"
#include "port.h"

#ifdef __STDC__
#include "stdarg.h"
#else
#include <varargs.h>
#endif

#include "uprintf.h"

#define Fprintf		(void) fprintf
#define Printf		(void) printf

#ifdef __STDC__

/* ANSI version */
void error(char *func_name, char *file_name, int line_num, char *format, ...)
/*
 * error(func, file, line, format, ... )
 */
{
    va_list ap;
    char buf[1024];

    va_start(ap, format);

    Fprintf(stderr, "Error: function %s, file %s, line %d:\n  %s\n",
	    func_name, file_name, line_num,
	    uprintf(buf, format, &ap));

    va_end(ap);
}

#else

/* Normal version */
/*VARARGS1*/
void error(va_alist)
va_dcl
/*
 * error(func, file, line, format, ... )
 */
{
    char *func_name, *file_name, *format;
    va_list ap;
    char buf[1024];
    int line_num;

    va_start(ap);

    func_name = va_arg(ap, char *);
    file_name = va_arg(ap, char *);
    line_num = va_arg(ap, int);
    format = va_arg(ap, char *);

    Fprintf(stderr, "Error: function %s, file %s, line %d:\n  %s\n",
	    func_name, file_name, line_num,
	    uprintf(buf, format, &ap));

    va_end(ap);
}

#endif

#define PI 3.14159

int
main()
/* Tests error routine */
{
    Fprintf(stderr, "Begin processing\n");
    error("main", __FILE__, __LINE__, "value of pi is %f\n", PI);
    return 0;
}
