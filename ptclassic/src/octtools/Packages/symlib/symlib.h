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
#ifndef SYMLIB_H
#define SYMLIB_H

#define SYMLIB_PKG_NAME "symlib"

#define SYM_NOT_FOUND		 2
#define SYM_OK			 1
#define SYM_ERROR		-1
#define SYM_ALREADY_EXISTS	-2
#define SYM_NO_EXIST		-3

#define SYM_INSTANCES	0
#define SYM_CONNECTORS	1

#define SYM_USE_EXISTING		1
#define SYM_CREATE			2
#define SYM_USE_EXISTING_OR_CREATE	3

#define SYM_HORIZONTAL		1
#define SYM_VERTICAL		2

typedef struct symSegment {
    octObject layer;
    struct octPoint pt1;
    struct octPoint pt2;
    octCoord width;
    int direction;
} symSegment;


EXTERN void symOpenFacet
  ARGS((octObject *facet, char *mode, char *technology));
EXTERN void symCreateFormalTerm
  ARGS((octObject *fct, octObject *term, char *name));
EXTERN void symImplementFormalTerm
  ARGS((octObject *ft, octObject *at));
EXTERN void symCreateNet
  ARGS((octObject *fct, octObject *net));
EXTERN void symNameObject
  ARGS((octObject *object, char *name));
EXTERN void symAddTermToNet
  ARGS((octObject *term, octObject *net));
EXTERN void symCreateInstance
  ARGS((octObject *facet, octObject *inst));
EXTERN void symReplaceInstance
  ARGS((octObject *old, octObject *newObj));
EXTERN void symCreateSegment
  ARGS((octObject *layer, struct octPoint *pt1, octObject *term1, int type1, struct octPoint *pt2, octObject *term2, int type2, octCoord width, int dir));

typedef void (*MNF)
  ARGS((octObject *net1, octObject *net2, octObject *keep, octObject *merge));
EXTERN void symSetMergeFunction
  ARGS((MNF func));

EXTERN void symGetNet
  ARGS((octObject *term, octObject *net));
EXTERN void symRoute
  ARGS((symSegment *array, int count));

EXTERN void symInitInstanceGen
  ARGS((octObject *facet, int type, octGenerator *gen));
EXTERN void symInitPropertyGen
  ARGS((octObject *object, octGenerator *gen));
EXTERN octStatus symGetProperty
  ARGS((octObject *object, octObject *prop, char *name));
EXTERN int symConnectorp
  ARGS((octObject *object));


/* 
 * Added by Andrea Casotto on June 5 1989 
 * Procedural interface to add constraints for sparcs
 */

/* min and max can be undefined */
#define SYM_PUT_CONST_UNDEFINED	-1


/* Values good for 'side' */
#define SYM_PUT_CONST_TOP	0
#define SYM_PUT_CONST_BOTTOM	1
#define SYM_PUT_CONST_LEFT	2
#define SYM_PUT_CONST_RIGHT	3

EXTERN int symNormalConstraint
  ARGS((int direction, octObject *oInst1, octObject *oInst2, int min, int max));
EXTERN int symSideConstraint
  ARGS((octObject *oInst, int side, int min, int max));
EXTERN int symActiveConstraint
  ARGS(( int direction, octObject *inst1, octObject *inst2, octObject *inst3, octObject *inst4 ));
EXTERN int symActiveSideConstraint
  ARGS( (int side, octObject *inst1, octObject *inst2 ));
EXTERN int symCleanAllConstraints
  ARGS((octObject *facet));

#endif /* SYMLIB_H */


