/* $Id$ */

/* util.c -- version 1.2 (IMEC)		last updated: 6/22/88 */
static char *SccsId = "@(#)util.c	1.2 (IMEC)	88/06/22";

#include <stdio.h>
#include "silage.h"

char *tmalloc (s)
   int s;
{
   char *a, *malloc ();
   a = malloc (s);
   if (a == NULL)
      fprintf (stdout, "Can't allocate enough memory.\n");
   else
      return (a);
}

void Punt(msg)
    char *msg;
{
    fprintf(stdout,"%s\nCompilation terminated.\n",msg);
    exit(1);
}

NodePosition(pnode)
NodeType	*pnode		;
{
	fprintf(stdout,"FILE %s, LINE %d, (CHARACTER %d)\n", FileFrom(pnode),
	Line(pnode),CharPos(pnode));
}
