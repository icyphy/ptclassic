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
 * Symbolic Instances
 *
 * David Harrison 
 * University of California, Berkeley
 * 1990
 *
 * This file contains routines for creating and manipulating symbolic
 * instances.  It includes routines for creating new instances,
 * reconnecting existing instances, and replacing an instance
 * with a new master.
 */

#ifndef _INST_H_
#define _INST_H_

#include "ansi.h"
#include "oct.h"
#include "segments.h"
#include "delete.h"

typedef struct sym_lyr_alt_defn {
    int (*lyr_alt_func)		/* Alternate implementation function */
      ARGS((octObject *t1, octObject *t2, int num_alt, octObject lyrs[],
	    char *data));
    char *data;			/* User data */
} symLyrAltFunc;

extern void symSetLyrAltFunc
  ARGS((symLyrAltFunc *new, symLyrAltFunc *old));

extern int symInstanceP
  ARGS((octObject *obj));
  /* Returns a non-zero status if object is real instance or instance term */

extern void symInstance
  ARGS((octObject *fct, octObject *inst));
  /* Creates a new symbolic instance or connector - no terminal maintenence */

extern void symReconnect
  ARGS((octObject *inst, symConFunc *confun, symDelItemFunc *del_func));
  /* Reconnects and existing instance by examining abutting elements */

extern int symDirChanges
  ARGS((octObject *term, int num));
  /* Returns non-zero if there are atleast `num' changes in directions */

#endif /* _INST_H_ */
