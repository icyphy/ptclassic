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
 * Jump Terminal and Net Maintenence
 *
 * David Harrison
 * University of California, Berkeley
 * 1990
 *
 * This file contains definitions for functions that export
 * jump terminal and net policy.
 */

#ifndef _JUMP_H_
#define _JUMP_H_

#include "ansi.h"
#include "oct.h"

#define SYM_MAX_MSG	2048

typedef struct sym_jumpname_defn {
    void (*func)		/* Function to call */
      ARGS((octObject *term, char jump_name[SYM_MAX_MSG], char *data));
    char *data;			/* User data passed to function */
} symJumpNameFunc;

extern void symSetJumpNameFunc
  ARGS((symJumpNameFunc *new, symJumpNameFunc *old));
  /* Sets the global/local net naming function */

extern int symJumpTermP
  ARGS((octObject *obj));
  /* Returns non-zero value if given terminal is a jump terminal */

extern int symJumpTermDeepP
  ARGS((octObject *aterm));
  /* Examines interface master to determine jump term predicate */

extern int symJumpNetP
  ARGS((octObject *net));
  /* Returns non-zero status if net has any jump terminals on it */

extern void symAddToJTBag
  ARGS((octObject *term));
  /* Adds a terminal to the jump terminals bag for the facet */

extern void symCreateJumpNet
  ARGS((octObject *term, octObject *net));
  /* Creates a new jump terminal net */

#endif /* _JUMP_H_ */
