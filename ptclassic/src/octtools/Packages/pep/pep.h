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
#ifndef PEP_H
#define PEP_H

/* This structure contains a bounding-box description AND the list
 * of free sides. Sides are indicated by tap direction constants.
 * A side bit set means that that side is free. A bit reset means
 * that on that side there is something.
 */
typedef struct pepBox {
  octBox bb;
  unsigned int freeSides;
} pepBox;

/* This structure contains the data regarding a branch resistance: 
 * first node, second node, value and name */

typedef struct pepRes {
    char *name;
    double value;
    char *node1;
    char *node2;
} pepRes;

#define PEP_PKG_NAME "pep"

EXTERN octPoint pepCenterTerm
  ARGS( (octObject *term, double coord) );

EXTERN double pepCapacitance
  ARGS( (octObject *obj ));

EXTERN double pepCrossCapacitance
  ARGS( (octObject *obj1, octObject *obj2 ));

EXTERN double pepResistance
  ARGS( (octObject *source, octObject* target ));

EXTERN pepRes *pepBranchResistance
  ARGS( (octObject *net, int *numberOfBranches ));

EXTERN double pepPathRes
  ARGS( (octObject *path, octObject* t1, octObject* t2 ));

EXTERN double pepTermCap
  ARGS((octObject  *term));

#endif
