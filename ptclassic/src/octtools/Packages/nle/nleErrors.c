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
 *  FILE:nleErrors.c
 *
 *  DESCRIPTION: error handling routines
 *
 *
 *  CREATION DATE: xx/xx/xx        AUTHOR:
 *
 *  FUNCTIONS:
 */
#include "copyright.h"
#include "port.h"
#include "errtrap.h"
#include "oct.h"
#include "nleCommon.h"
#include "nle.h"

char nleErrorStringInternal[1024] = "no error";

void
nleFatalError(es)
char *es;            /* error string */
{
    errRaise(NLE_PKG_NAME, -1, es);
    return;
}

void
nleWarning(es)
char *es;            /* error string */
{
    (void) sprintf(nleErrorStringInternal, "NLE: Warning: %s", es);
    (void) fprintf(stderr, "%s\n", nleErrorStringInternal);
    return;
}

char *
nleErrorString()
{
    return(nleErrorStringInternal);
}
