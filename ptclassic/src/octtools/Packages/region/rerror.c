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
/*LINTLIBRARY*/
/*
 * Region Package Error Handling
 *
 * David Harrison
 * University of California, Berkeley
 * 1988
 *
 * This file contains the error reporting functions for the region
 * package.
 */

#include "copyright.h"
#include "port.h"
#include <varargs.h>
#include "kd.h"
#include "region.h"
#include "reg_internal.h"
#include "errtrap.h"
#include "uprintf.h"

/* External definitions */
char *region_pkg_name = "region";
char _reg_error[MAX_ERR_SIZE];
int _reg_depth = 0;
char *_reg_func_name = "no function";

/*ARGSUSED*/
void _reg_err_handler(pkgName, code, message)
char *pkgName;			/* Name of offending package */
int code;			/* Error code                */
char *message;			/* Error message             */
/*
 * This routine repackages a sub-error message and passes it
 * up to the next level.
 */
{
    /* Undo the depth */
    _reg_depth = 0;
    errPopHandler();
    if (pkgName == region_pkg_name) {
	pkgName = "internal";
    }
    errPass("in %s:\n\t%s (%d)", _reg_func_name, message, code);
    /* NOTREACHABLE */
}

char *regErrorString()
/*
 * Returns a diagnostic if an error occurs when calling a region
 * package routine.
 */
{
    return (char *) _reg_error;
}

/*VARARGS0*/
regStatus reg_error(va_alist)
va_dcl
/*
 * reg_error(status, format, ... )
 *
 * This routine sets the global buffer _reg_error to the result of
 * using `format' and the remaining arguments to build an error
 * message.  The routine returns `status'.
 */
{
    va_list ap;
    regStatus stat;
    char *format;

    va_start(ap);
    stat = va_arg(ap, int);
    format = va_arg(ap, char *);
    (void) uprintf(_reg_error, format, &ap);
    va_end(ap);
    return stat;
}


/*VARARGS1*/
char *reg_fault(va_alist)
va_dcl
/*
 * reg_fault(int fault_type, char *format, ...)
 * Prints out a descriptive error for a major fault and calls the
 * fatal fault handler.  The fault handler normally does not return.
 * The format may be omitted.
 */
{
    va_list ap;
    int fault_type;
    char *format;
    char local_buf[MAX_ERR_SIZE];

    va_start(ap);
    fault_type = va_arg(ap, int);
    format = va_arg(ap, char *);
    if (format) {
	(void) uprintf(local_buf, format, &ap);
    } else {
	local_buf[0] = '\0';
    }

    switch (fault_type) {
    case REG_MEM:
	(void) sprintf(_reg_error, "out of memory");
	break;
    case REG_OCT:
	(void) sprintf(_reg_error, "oct error: %s:\n  %s",
		       local_buf, octErrorString());
	break;
    case REG_UCR:
	(void) sprintf(_reg_error, "bad change record");
	break;
    case REG_BA:
	(void) sprintf(_reg_error, "bad argument: %s", local_buf);
	break;
    case REG_KDE:
	/* Add k-d message onto end */
	(void) sprintf(_reg_error, "kd error: %s\n  %s",
		       local_buf, kd_err_string());
	break;
    case REG_NIF:
	(void) sprintf(_reg_error, "cannot open interface facet:\n  %s",
		       octErrorString());
	break;
    case REG_CON:
	(void) sprintf(_reg_error, "inconsistent facets: %s\n  %s",
		       local_buf, octErrorString());
	break;
    case REG_BDB:
	(void) sprintf(_reg_error, "change list type error: %s\n",
		       local_buf);
	break;
    default:
	(void) sprintf(_reg_error, "unknown error %d", fault_type);
	break;
    }
    errRaise(region_pkg_name, fault_type, _reg_error);
    return (char *) 0;
}


