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
 * General definitions for VEM and its subsystems
 * David Harrison,  September, 1985-1988
 * UC Berkeley
 */

#ifndef GENERAL
#define GENERAL

#include "port.h"		/* System wide portability */
#include <math.h>		/* Mathematics routines    */
#include "str.h"		/* String definitions      */
#include "errors.h"		/* VEM error handling      */
#include "utility.h"		/* System wide utilities   */

typedef char *Pointer;		/* Pointer to anything   */
typedef int vemStatus;		/* Status codes          */

/* General data structure macros */

#define VEMALLOCNAME	malloc
#define VEMFREENAME	free
/* Use the REALLOC macro defined in utility.h */ 
#define VEMREALLOCNAME	REALLOC

#define VEMALLOC(type)	(type *) VEMALLOCNAME(sizeof(type))
#define VEMARRAYALLOC(type, count) \
	(type *) VEMALLOCNAME((unsigned)(sizeof(type) * (count)))


#ifdef NEVER
/* If VEMREALLOCNAME is realloc, then convert the 3 arg macro to a 2
 * arg call.
 */
#define VEMREALLOC(type, ptr, newcount) \
	(type *) VEMREALLOCNAME((char *) ptr, \
				(unsigned) (sizeof(type) * (newcount)))
#else
/* Substitute in the REALLOC call here, see Packages/utility/utility.h */
#define VEMREALLOC(type, ptr, newcount) \
             VEMREALLOCNAME(type, ptr, newcount)

#endif

#define VEMFREE(ptr)	VEMFREENAME((char *) (ptr)) 

#define VEMSTRCOPY(s)	STRMOVE(VEMALLOCNAME((unsigned) (STRLEN(s) + 1)), (s))

#define VEMMAXSTR	255

/* Box intersection.  Both boxes are assumed to be (struct octBox) */

#define VEMBOXINTERSECT(b1, b2) \
	(((b1).upperRight.x >= (b2).lowerLeft.x) && \
	 ((b2).upperRight.x >= (b1).lowerLeft.x) && \
	 ((b1).upperRight.y >= (b2).lowerLeft.y) && \
	 ((b2).upperRight.y >= (b1).lowerLeft.y))

/* Box inclusion.  Checks to see if b1 is inside b2 (both struct octBox) */

#define VEMBOXINSIDE(b1, b2) \
	(((b1).lowerLeft.x >= (b2).lowerLeft.x) && \
	 ((b1).lowerLeft.y >= (b2).lowerLeft.y) && \
	 ((b1).upperRight.x <= (b2).upperRight.x) && \
	 ((b1).upperRight.y <= (b2).upperRight.y))

/* 
 * This macro takes a box and insures the lower left is really the lower
 * left and the upper right is really the upper right.  'box' should
 * be a (struct octBox) and temp should be (octCoord).
 */

#define VEMBOXNORM(box, temp) \
temp = VEMMAX((box).lowerLeft.x, (box).upperRight.x);             \
(box).lowerLeft.x = VEMMIN((box).lowerLeft.x, (box).upperRight.x); \
(box).upperRight.x = temp;					\
temp = VEMMAX((box).lowerLeft.y, (box).upperRight.y);             \
(box).lowerLeft.y = VEMMIN((box).lowerLeft.y, (box).upperRight.y); \
(box).upperRight.y = temp;


/* Maximum and minimum integers assuming integers are 32 bits */

#define	VEMMAXINT	2147483647
#define VEMMININT	(-VEMMAXINT)+1

/* Maximum and minimum macros */

#define	VEMMAX(x, y)	((x) > (y) ? (x) : (y))
#define VEMMIN(x, y)	((x) < (y) ? (x) : (y))

/* Square a number */

#define VEMSQUARE(x)	((x) * (x))

/* Absolute value macro */

#define VEMABS(x)	((x) >= 0 ? (x) : -(x))

/* Computing distance.  Both arguments are octPoint */

#define VEMDIST(a, b) \
((int) hypot((double) ((a).x-(b).x), (double) ((a).y-(b).y)))

/* Assertion for debugging */

#define VEM_CHECK(cond, name) \
((cond) ? 1 : vemFail((name), __FILE__, __LINE__))

#define VEM_CKRET(cond, msg) \
if (!VEM_CHECK(cond, msg)) return

#define VEM_CKRVAL(cond, msg, code) \
if (!VEM_CHECK(cond, msg)) return code

#define VEM_OCTCK(octcall) \
((octcall) < OCT_OK ? vemOctFail(__FILE__, __LINE__) : 1)

#define VEM_OCTCKRET(octcall) \
if (!VEM_OCTCK(octcall)) return

#define VEM_OCTCKRVAL(octcall, code) \
if (!VEM_OCTCK(octcall)) return code

/* VE status codes.  Negative codes are fatal.  Positive are non-fatal. */

#define VEM_OK		0

#define VEM_FALSE	1	/* Predicate failure value       */
#define VEM_NOMORE	2	/* Generator is exhausted 	 */
#define VEM_DUPLICATE	3	/* Same name used more than once */
#define VEM_ZEROGRID	4	/* Indicates zero grid line      */
#define VEM_ARGMOD	5	/* Command modified argument list */
#define VEM_ARGRESP	6	/* Like 5 but redisplay arg list  */
#define VEM_ARGBREAK	7	/* Breaks into a string argument */
#define VEM_NOSELECT	8	/* No selection made             */
#define VEM_SELPLANE	9	/* Selection on its own plane    */
#define VEM_NEWITEM	10	/* New item created              */
#define VEM_TOOSMALL	11	/* Value is too small            */
#define VEM_INTERRUPT	12	/* VEM has been interrupted      */
#define VEM_NOMATCH	13	/* Failed to match items         */

#define VEM_BADSTATE	-1	/* State of routine is inappropriate */
#define VEM_CORRUPT	-2	/* Data structure corruption         */
#define VEM_NOROOM	-3	/* Allocation of new space failed    */
#define VEM_NOFILE	-4	/* File or directory not found       */
#define VEM_CANTFIND	-5	/* Requested item can't be found     */
#define VEM_RESOURCE	-6	/* Can't allocate a new resource     */
#define VEM_NOIMPL	-7	/* Feature is not implemented        */
#define VEM_NOTSUFF	-8	/* Not enough data provided          */
#define VEM_TYPECONF	-9	/* Type conflict                     */
#define VEM_NOTUNIQ	-10	/* Requested item is not unique      */
#define VEM_ACCESS	-11	/* Access rights are not sufficient  */
#define VEM_NOTINIT	-12	/* No initialization has been done   */
#define VEM_INUSE	-13	/* Item requested is in use          */
#define VEM_BADARG	-14	/* Bad argument to routine           */
#define VEM_OCTFAIL	-15	/* Oct call failed                   */

#define VEM_ABORT	-99	/* Terminate program                 */

/*
 * Routines exported by vem.c (main program)
 */
extern int vemSyncEvents();
   /* Makes sure VEM is up to date with respect to events */

extern int vemHandleEvent();
   /* Handles an event */

#endif
