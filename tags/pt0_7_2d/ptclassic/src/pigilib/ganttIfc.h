#ifndef _GANTTIFC_H
#define _GANTTIFC_H 1

/* 
Copyright (c) 1990-1996 The Regents of the University of California.
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

/*  ganttIfc.h  edg
Version identification:
@(#)ganttIfc.h	1.17	11/26/95
Function prototypes by Brian Evans.
*/

/* Do the right thing for sol2 boolean defs.  compat.h must be included
 * first so sys/types.h is included correctly.
 */
#include "sol2compat.h"

/* Define the octObject data structure */
#include "oct.h"

typedef struct RgbValue_s {
    unsigned short red;
    unsigned short green;
    unsigned short blue;
} RgbValue;

extern char* incr ARGS((const char *s));
extern boolean FrameStarCall ARGS((char* name, RgbValue color, int usePattern));
extern void FindClear();
extern boolean FindNameSet ARGS((octObject* facetPtr, char *name));
extern void FindAndMark ARGS((octObject* facetP, const char *name,
			      int pattern));
extern int CreateFrames ARGS((RgbValue colors[], int n));
extern int ClearFrames();
extern int DestroyFrames();
extern int Frame ARGS((char *stars[]));
extern void FindAndMarkColor ARGS((octObject* facetP, const char *name,
				   int pattern, const char *Color));

#endif /* _GANTTIFC_H */
