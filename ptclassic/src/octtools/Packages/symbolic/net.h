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
 * Net Manipulation
 * 
 * David Harrison
 * University of California, Berkeley
 * 1989
 *
 * This file contains definitions for using the net merging routines
 * in net.c
 */

#ifndef _NET_H_
#define _NET_H_

#include "ansi.h"
#include "oct.h"

typedef struct sym_merge_func_defn {
    int (*func)			/* Function to call */
      ARGS((octObject *net1, octObject *net2, char *data));
    char *data;			/* User data        */
} symMergeFunc;

extern void symSetMergeFunc
  ARGS((symMergeFunc *new, symMergeFunc *old));
  /* Set the merging function */

extern octObject *symMergeNets
  ARGS((octObject *first, octObject *second));
  /* Merge two nets into one */

extern int symBreakNet
  ARGS((octObject *obj, octObject *net, octObject *new_net, int warn_p));
  /* Removes portion of `net' connected to `obj' */

extern int symLocNet
  ARGS((octObject *obj, octObject *net));
  /* Finds net associated with object */

extern void symCreateNamedNet
  ARGS((octObject *obj, octObject *net));
  /* Creates new machine named net in same facet as `obj' */

extern int symHumanNamedNetP
  ARGS((octObject *net));
  /* Returns non-zero if net is human named */

extern void symNetRemove
  ARGS((octObject *net));
  /* Removes net with clean-up */

#endif /* _NET_H_ */
