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
 * Copying a set of objects in symbolic
 *
 * David Harrison
 * University of California, Berkeley
 * 1990
 *
 * Exported definitions for the copy primitives.
 */

#ifndef _COPY_H_
#define _COPY_H_

#include "ansi.h"
#include "contact.h"
#include "oct.h"

/*
 * Copy a set of objects
 */

typedef struct sym_copy_item_defn {
    void (*func)		/* Function to call */
      ARGS((octObject *old, octObject *new, char *data));
    char *data;			/* User supplied data */
} symCopyItemFunc;

extern void symCopyObjects
  ARGS((octObject *facet, octObject *set, struct octTransform *transform,
	octObject *result, symConFunc *confun, symCopyItemFunc *ci));


#endif /* _COPY_H_ */
