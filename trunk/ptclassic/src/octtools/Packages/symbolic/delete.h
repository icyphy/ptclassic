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
 * Deleting a set of objects in Symbolic
 *
 * David Harrison
 * University of California,  Berkeley
 * 1989, 1990
 *
 * New delete command implemented using new group net maintenance
 * library.  This file contains the exported definitions of the
 * package.
 */

#ifndef _DELETE_H_
#define _DELETE_H_

#include "ansi.h"

/*
 * Delete a set of objects
 */

typedef struct sym_del_item_func_defn {
    void (*func)		/* function to call */
      ARGS((octObject *item, char *data));
    char *data;			/* User data        */
} symDelItemFunc;

extern void symDeleteObjects
  ARGS((octObject *delSetBag, symDelItemFunc *del_func));

#endif /* _DELETE_H_ */
