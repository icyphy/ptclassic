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
#include "copyright.h"
#include "errtrap.h"	/* pulls in setjmp.h, which we need here */

#ifdef DEFINE_ATTACHE_EXTERN
#define ATTACHE_EXTERN
#else
#define ATTACHE_EXTERN extern
#endif

#define RET_OK		0	/* successful completion */
#define RET_FAIL	1	/* unsuccessful completion */
#define RET_TURKEY	2	/* bad command-line options */

#define OCT_ASSERT(exp, mess)	\
    if (exp < OCT_OK) errRaise("oct", 0, mess, octErrorString());

ATTACHE_EXTERN octStatus protStat;
ATTACHE_EXTERN char *protPkg, *protMesg;
ATTACHE_EXTERN int protCode;
#define OCT_PROTECT(exp)	ERR_IGNORE(protStat = (exp))
#define OCT_VPROTECT(exp)	OCT_PROTECT(((exp), OCT_OK))
#define DID_OCT_FAIL()		\
    ((protStat < OCT_OK) ?						\
	(protMesg = octErrorString(), 1) :				\
	errStatus(&protPkg, &protCode, &protMesg))

struct maskEntry {
    int codeChar;
    octObjectMask mask;
};

ATTACHE_EXTERN struct maskEntry maskTable[]
#ifdef DEFINE_ATTACHE_EXTERN
    = {
	{	'f',	OCT_FACET_MASK	},
	{	'i',	OCT_INSTANCE_MASK	},
	{	't',	OCT_TERM_MASK	},
	{	'n',	OCT_NET_MASK	},
	{	'p',	OCT_PROP_MASK	},
	{	'b',	OCT_BAG_MASK	},
	{	'l',	OCT_LAYER_MASK	},
	{	'g',	OCT_GEO_MASK	},
	{	'a',	OCT_ALL_MASK	},
	{	'\0',	0		}
    }
#endif
;

ATTACHE_EXTERN st_table *nameTable;

/*	Current state	*/

typedef struct {
    octId ID;
    int genPos;
    int genDirection;
} histElement;

#define CONTENTS	0
#define CONTAINERS	1

struct state {
    octObject currentFacet;	/* the facet we're in */
    octObject currentObject;	/* the object being displayed */
    histElement *historyList;	/* history of objects we've moved to */
    int historyLength;		/* number of IDs in above list */
    octObjectMask genMask;	/* mask of objects types to generate */
    int genDirection;		/* generate contents or containers? */
    int genPos;			/* gen iter of first obj on screen (1-based) */
    jmp_buf jmpBuffer;		/* context to return to on error */
    int changeFlag;		/* has this facet been modified? */
};
ATTACHE_EXTERN struct state *currentState;/* context of editing in current facet */

ATTACHE_EXTERN int nObjs;		/* number of objects that fit on the screen */
ATTACHE_EXTERN int quitAll		/* are we exiting the whole thing? */
#ifdef DEFINE_ATTACHE_EXTERN
    = 0
#endif
;
