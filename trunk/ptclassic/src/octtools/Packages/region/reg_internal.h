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
 * VEM Region Package Internal definitions
 *
 * David Harrison
 * University of California,  Berkeley
 * 1987, 1988
 *
 * This file contains important internal definitions for
 * the region package.
 */

#include "copyright.h"
#include "port.h"
#include "utility.h"

#define MAX_ERR_SIZE	2048
extern char _reg_error[];
extern char *_reg_mem;

/* Error handling code */
extern int _reg_depth;
extern char *_reg_func_name;
extern void _reg_err_handler();

#define REG_PRE(name) \
if (_reg_depth++ == 0) {		\
    _reg_func_name = name;		\
    errPushHandler(_reg_err_handler);	\
}

#define REG_POST \
if (--_reg_depth == 0) {		\
   errPopHandler();			\
}

#define REG_RETURN(val)		REG_POST return (val)

#define REG_OA(call) \
if ((call) < OCT_OK) {			        \
    (void) reg_fault(REG_OCT, "assertion");	\
}

extern regStatus reg_error();
  /* Writes an error into a buffer */

extern char *reg_fault();
  /* Fatal error */

#define REG_ALLOC(type)	\
  ((_reg_mem = malloc(sizeof(type))) ? (type *) _reg_mem : (type *) reg_fault(REG_MEM))

#define REG_ARYALLOC(type, num) \
  ((_reg_mem = malloc((unsigned) (sizeof(type) * (num)))) ? \
   (type *) _reg_mem : (type *) reg_fault(REG_MEM))

#define REG_REALLOC(type, ptr, newcount) \
((_reg_mem = (char *) (REALLOC(type, (ptr), (newcount)))) ? \
 (type *) _reg_mem : (type *) reg_fault(REG_MEM))

#ifdef NEVER
#define REG_REALLOC(type, ptr, newcount) \
  ((_reg_mem = realloc((char *) ptr, (unsigned) (sizeof(type) * (newcount)))) ? \
   (type *) _reg_mem : (type *) reg_fault(REG_MEM))
#endif
#define REG_FREE(ptr)	free((char *) ptr)

#define REG2STR		(void) sprintf

#define NIL(type)	((type *) 0)

#define	REGMAXINT	2147483647
#define REGMININT	(-REGMAXINT)+1

/* Maximum and minimum macros */

#define	REGMAX(x, y)	((x) > (y) ? (x) : (y))
#define REGMIN(x, y)	((x) < (y) ? (x) : (y))

#define REGBOXNORM(box, temp) \
temp = REGMAX((box).lowerLeft.x, (box).upperRight.x);             \
(box).lowerLeft.x = REGMIN((box).lowerLeft.x, (box).upperRight.x); \
(box).upperRight.x = temp;					\
temp = REGMAX((box).lowerLeft.y, (box).upperRight.y);             \
(box).lowerLeft.y = REGMIN((box).lowerLeft.y, (box).upperRight.y); \
(box).upperRight.y = temp;

#define REGBOXINTERSECT(b1, b2) \
	(((b1).upperRight.x >= (b2).lowerLeft.x) && \
	 ((b2).upperRight.x >= (b1).lowerLeft.x) && \
	 ((b1).upperRight.y >= (b2).lowerLeft.y) && \
	 ((b2).upperRight.y >= (b1).lowerLeft.y))

#define REGBOXINSIDE(b1, b2) \
	(((b1).lowerLeft.x >= (b2).lowerLeft.x) && \
	 ((b1).lowerLeft.y >= (b2).lowerLeft.y) && \
	 ((b1).upperRight.x <= (b2).upperRight.x) && \
	 ((b1).upperRight.y <= (b2).upperRight.y))

#define REGBOXKDINTR(octbox, kdbox) \
	(((octbox).upperRight.x >= (kdbox)[KD_LEFT]) && \
	 ((kdbox)[KD_RIGHT] >= (octbox).lowerLeft.x) && \
	 ((octbox).upperRight.y >= (kdbox)[KD_BOTTOM]) && \
	 ((kdbox)[KD_TOP] >= (octbox).lowerLeft.y))

#define octXId	int32

#define REGION_CHANGE_BAG_NAME	";;; RegionCLBag ;;;"
