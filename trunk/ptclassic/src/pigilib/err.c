/* 
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the above
copyright notice and the following two paragraphs appear in all copies
of this software.

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
							COPYRIGHTENDKEY
*/
/* err.c  edg
Version identification:
$Id$
Error handling functions.
*/

/* Includes */
#include <stdio.h>
#include "local.h"

#define ERR_BUF_N 1000

static char errBuf[ERR_BUF_N] = "";
static char *next = errBuf;

void
ErrClear()
{
    next = errBuf;
    *next = EOS;
}

void
ErrAdd(msg)
char *msg;
{
    /* if this is not the first message */
    if (next != errBuf) {
	/* add separator if enough room */
	if (next >= errBuf + ERR_BUF_N - 3) {
	    return;
	}
	*next++ = ':';
	*next++ = ' ';
    }
    while (*msg != EOS && next < errBuf + ERR_BUF_N - 1) {
	*next++ = *msg++;
    }
    *next = EOS;
}

char *
ErrGet()
{
    return(errBuf);
}
