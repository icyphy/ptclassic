/* Version Identification:
 * $Id$
 */
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
 * Internal Symbolic Definitions
 *
 * David Harrison
 * University of California, Berkeley
 * 1989
 *
 * These definitions are private to the symbolic package.
 */

#ifndef _SYM_INTERNAL_
#define _SYM_INTERNAL_

#include "copyright.h"
#include "port.h"
#include "ansi.h"
#include "oct.h"
#include "errtrap.h"

/*
 * Policy specific names
 */

#define OCT_CONTACT_NAME	"CONNECTORS"
#define OCT_CONTACT_PROP_NAME	"CELLTYPE"
#define OCT_CONTACT_TYPE_NAME	"CONTACT"
#define OCT_INSTANCE_NAME	"INSTANCES"
#define OCT_JOINGROUP_NAME	"JOINEDTERMS"
#define OCT_JOIN_NAME		"JOINED"
#define OCT_JUMPTERM_NAME	"JUMPTERMS"
#define OCT_GLOBAL_NAME		"GLOBALNET"
#define OCT_LOCAL_NAME		"LOCALNET"
#define OCT_CONN_INST_NAME	"*"

/*
 * Utility macros
 */

#define POINT_IN_BOX(p,b)	((p)->x >= (b)->lowerLeft.x && \
				 (p)->x <= (b)->upperRight.x && \
				 (p)->y >= (b)->lowerLeft.y && \
				 (p)->y <= (b)->upperRight.y)

#define DIST(a, b) \
((int) hypot((double) ((a).x-(b).x), (double) ((a).y-(b).y)))

#define DISTP(a, b) \
((int) hypot((double) ((a)->x-(b)->x), (double) ((a)->y-(b)->y)))

#define LA(x, y)	if ((x) < (y)) (y) = (x)
#define GA(x, y)	if ((x) > (y)) (y) = (x)

extern int symVerbose;
extern char *sym_str_zero;

#define STRZERO	sym_str_zero
#define STRCMP(a, b)	\
((a) ? ((b) ? strcmp(a, b) : strcmp(a, STRZERO)) \
  : ((b) ? strcmp(STRZERO, b) : 1))

#define SYMBOXINTERSECT(b1, b2) \
	(((b1).upperRight.x >= (b2).lowerLeft.x) && \
	 ((b2).upperRight.x >= (b1).lowerLeft.x) && \
	 ((b1).upperRight.y >= (b2).lowerLeft.y) && \
	 ((b2).upperRight.y >= (b1).lowerLeft.y))



extern char *sym_pkg_name;

#define SYMCK(oct_call) \
if ((oct_call) < OCT_OK)   \
   errRaise(sym_pkg_name, SYM_OCTFAIL, "Oct function failed:\n  %s\n", octErrorString())

#define OCT_CONTACT_INSTNAME	"*"

#define JUMPTERMS

typedef enum sym_severity_defn { SYM_INFO, SYM_WARN, SYM_ERR } symSeverity;

#ifdef lint

/*ARGSUSED*/
/*VARARGS1*/
void symMsg(sev, fmt)
symSeverity sev;
char *fmt;
{
    /* Definition left empty */
}

#else

extern void symMsg
  ARGS((symSeverity sev, char *fmt, ...));

#endif /* lint */

/*
 * Utility routines
 */

extern int symLessAttach
  ARGS((octObject *obj, int contents_p, octObjectMask mask, int count));

/*
 * Warnings and messages
 */

typedef struct sym_warn_func_defn {
    void (*func)		/* Function to call */
      ARGS((symSeverity severity, char *message, char *data));
    char *data;			/* User data        */
} symWarnFunc;

extern void symSetWarnFunc
  ARGS((symWarnFunc *new, symWarnFunc *old));
  /* Sets the warning/message function for symbolic */

/* Possible fatal faults */
#define SYM_OCTFAIL	0	/* Oct call failed              */
#define SYM_NONET	1	/* Object has no associated net */
#define SYM_MAPFAIL	2	/* Group map table failure      */
#define SYM_BADOBJ	3	/* Bad object type              */
#define SYM_BADTERM	4	/* Formal/actual mismatch       */
#define SYM_NOINTR	5	/* No interface facet           */
#define SYM_NOMASTER	6	/* No master for instance       */
#define SYM_PERM	7	/* Permission failure           */
#define SYM_BADPOL	8	/* Cell has bad symbolic policy */
#define SYM_MULNET	9	/* Too many nets                */
#define SYM_BADBAG	10	/* Bad policy bag maintenence   */
#define SYM_BADSEG	11	/* Bad symbolic segment         */
#define SYM_BADFUNC	12	/* Bad user function            */
#define SYM_BADIMPL     13      /* Bad terminal implementation  */
#endif /* _SYM_INTERNAL_ */
