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
 * Formal Terminal Maintenence
 *
 * David Harrison
 * University of California, Berkeley
 * 1988
 *
 * This file contains an implementation of the symbolic formal
 * terminal command.
 */

#ifndef _FORMAL_H_
#define _FORMAL_H_

#include "ansi.h"

typedef struct sym_term_func_defn {
    void (*func)		/* Function to call */
      ARGS((octObject *fterm, octObject *obj, int detach, char *data));
    char *data;			/* User data        */
} symTermFunc;

extern void symNewFormal
  ARGS((octObject *facet, octObject *fterm, octObject *implBag,
	char *termtype, char *termdir, symTermFunc *term_func));
  /* Creates a new symbolic formal terminal */

#define SYM_TERM_TYPE	"TERMTYPE"
#define SYM_TERM_DIR	"DIRECTION"

extern int symGetTermProp
  ARGS((octObject *term, octObject *prop));
  /* Finds property annotations on formal terminals */

#endif /* _FORMAL_H_ */
