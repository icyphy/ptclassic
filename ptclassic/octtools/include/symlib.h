/* Version Identification:
 * @(#)symlib.h	1.3	09/22/98
 */
/*
Copyright (c) 1990-1998 The Regents of the University of California.
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


OCT_EXTERN void symOpenFacet
  ARGS((octObject *facet, char *mode, char *technology));
OCT_EXTERN void symCreateFormalTerm
  ARGS((octObject *fct, octObject *term, char *name));
OCT_EXTERN void symImplementFormalTerm
  ARGS((octObject *ft, octObject *at));
OCT_EXTERN void symCreateNet
  ARGS((octObject *fct, octObject *net));
OCT_EXTERN void symNameObject
  ARGS((octObject *object, char *name));
OCT_EXTERN void symAddTermToNet
  ARGS((octObject *term, octObject *net));
OCT_EXTERN void symCreateInstance
  ARGS((octObject *facet, octObject *inst));
OCT_EXTERN void symReplaceInstance
  ARGS((octObject *old, octObject *newObj));
OCT_EXTERN void symCreateSegment
  ARGS((octObject *layer, struct octPoint *pt1, octObject *term1, int type1, struct octPoint *pt2, octObject *term2, int type2, octCoord width, int dir));

typedef void (*MNF)
  ARGS((octObject *net1, octObject *net2, octObject *keep, octObject *merge));
OCT_EXTERN void symSetMergeFunction
  ARGS((MNF func));

OCT_EXTERN void symGetNet
  ARGS((octObject *term, octObject *net));
OCT_EXTERN void symRoute
  ARGS((symSegment *array, int count));

OCT_EXTERN void symInitInstanceGen
  ARGS((octObject *facet, int type, octGenerator *gen));
OCT_EXTERN void symInitPropertyGen
  ARGS((octObject *object, octGenerator *gen));
OCT_EXTERN octStatus symGetProperty
  ARGS((octObject *object, octObject *prop, char *name));
OCT_EXTERN int symConnectorp
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

OCT_EXTERN int symNormalConstraint
  ARGS((int direction, octObject *oInst1, octObject *oInst2, int min, int max));
OCT_EXTERN int symSideConstraint
  ARGS((octObject *oInst, int side, int min, int max));
OCT_EXTERN int symActiveConstraint
  ARGS(( int direction, octObject *inst1, octObject *inst2, octObject *inst3, octObject *inst4 ));
OCT_EXTERN int symActiveSideConstraint
  ARGS( (int side, octObject *inst1, octObject *inst2 ));
OCT_EXTERN int symCleanAllConstraints
  ARGS((octObject *facet));

#endif /* SYMLIB_H */


