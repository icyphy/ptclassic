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
