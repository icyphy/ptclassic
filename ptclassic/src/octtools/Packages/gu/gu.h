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
 * Geometric Updates
 *
 * David Harrison
 * University of California, Berkeley
 * 1989
 *
 * This package provides an interface above Oct change lists for
 * examining geometric changes.  It automatically collapses the
 * events down to a minimum for quick analysis.
 */

#ifndef GU_INCLUDED
#define GU_INCLUDED

#include "copyright.h"
#include "ansi.h"
#include "oct.h"

extern char *gu_pkg_name;

typedef struct gu_dummy_defn {
    int dummy;			/* Opaque structure */
} gu_dummy;

typedef gu_dummy *gu_marker;

typedef gu_dummy *gu_gen;

typedef enum gu_op_defn { GU_NOOP, GU_ADD, GU_UPDATE, GU_REMOVE } gu_op;

typedef gu_dummy *gu_change;

extern gu_marker guMark
  ARGS((octObject *container, octObjectMask mask));
   /* Changes will be examined from this point on */
extern void guUnmark
  ARGS((gu_marker v_mark));
   /* Removes previously created marker */

extern gu_gen guStart
  ARGS((gu_marker v_mark));
   /* Starts generation of changes for supplied marker */
extern gu_change guNext
  ARGS((gu_gen v_gen));
   /* Generates the next change in the sequence */
extern void guEnd
  ARGS((gu_gen v_gen));
   /* Frees resources consumed by generator */

extern gu_op guOp
  ARGS((gu_change v_change));
   /* Returns change operator for change record */
extern int32 guXid
  ARGS((gu_change v_change));
   /* Returns external identifier of changed object */
extern octObject *guFacet
  ARGS((gu_change v_change));
   /* Returns the facet of the change record (if any) */
extern struct octBox *guOldSize
  ARGS((gu_change v_change));
   /* Returns old size of object (if any) */
extern octObject *guOldObject
  ARGS((gu_change v_change));
   /* Returns old object of change record (if any) */
extern int guOldPoints
  ARGS((gu_change v_change, struct octPoint **points));
   /* Returns old points of object in change record (if any) */

/*
 * Error codes
 */

#define GU_NO_BAG	1
#define GU_NO_LIST	2
#define GU_NO_DEL	3
#define GU_BAD_CL	4
#define GU_BAD_OP	5
#define GU_BAD_ID	6
#define GU_BAD_REC	7
#define GU_NOT_IMPL	8

#endif  /* GU_INCLUDED */
