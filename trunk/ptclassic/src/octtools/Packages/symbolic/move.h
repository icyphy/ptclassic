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
 * Move Objects in Symbolic
 *
 * David Harrison
 * University of California,  Berkeley
 * 1987, 1989, 1990
 *
 * Move a set of objects without perturbing connectivity (drags
 * connection end-points).  This file contains exported functionality
 * from move.c.
 */

#ifndef _MOVE_H_
#define _MOVE_H_

#include "ansi.h"
#include "oct.h"

typedef struct sym_move_item_func_defn {
    void (*func)		/* Function to call */
      ARGS((octObject *item, int after_p, char *data));
    char *data;			/* Extra data       */
} symMoveItemFunc;

extern void symMoveObjects
  ARGS((octObject *set, struct octTransform *transform,
	symMoveItemFunc *move_func));
  /* Moves a set of objects without disturbing connectivity */

#endif /* _MOVE_H_ */
