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
 * Terminal Information Header
 *
 * David Harrison
 * University of California,  Berkeley
 * 1987
 *
 * This file contains definitions for information returned
 * by the terminal information module.
 */

#ifndef _TERMINFO_H_
#define _TERMINFO_H_

#include "ansi.h"
#include "oct.h"

typedef struct term_dummy_defn {
    int dummy;
} term_dummy;

typedef term_dummy *symTermGen;


/* 
 * Information about segment terminals returned by seg_term()
 */

#define SEG_AT_CONNECTOR	1 /* Segment AT is a connector */
#define SEG_AT_MULTI		2 /* Segment AT is multi-layer */

typedef struct seg_term_info {
    short flags;		/* Terminal flags        */
    octObject term;		/* Terminal itself       */
    struct octBox term_bb;	/* Terminal bounding box */
} segTermInfo;

extern void symInitTerm
  ARGS((octObject *a_term, symTermGen *t_gen));
  /* Initializes generation of terminal implementations */

extern int symNextTerm
  ARGS((symTermGen gen, octObject *lyr, octObject *geo));
  /* Returns next implementation geometry in sequence */

extern void symEndTerm
  ARGS((symTermGen gen));
  /* Ends sequence of terminal generation */

extern void symSegTerms
  ARGS((octObject *segment, struct octPoint *points, segTermInfo info[2]));
  /* Produces information about segment endpoints */


#endif /* _TERMINFO_H_ */
