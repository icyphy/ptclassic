/* $Id$ */

/* err.c -- version 1.4 (IMEC)		last updated: 8/11/88 */
static char *SccsId = "@(#)err.c	1.4 (IMEC)	88/08/11";

#include <stdio.h>
#include "bits.h"

void Crash (c)
char *c;
{
   fprintf (stderr, "Inconsistency in : %s.\n", c);
   exit (1);
}
