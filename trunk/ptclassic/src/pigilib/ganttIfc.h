#ifndef GANTTIFC_H
#define GANTTIFC_H 1
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
/*  ganttIfc.h  edg
Version identification:
$Id$
*/

#ifdef __cplusplus
#include "rpc.h"
extern int CreateFrames();
extern int Frame(char *stars[]);
extern int ClearFrames();
extern int DestroyFrames();
/* following is not used */
/* extern void FindAndMarkError( octObject *facetP, char *name ); */

extern boolean FindNameSet(/* octObject *facetPtr, char *name */);
/* FIXME:  These functions are not in ganttIfc.h */
extern void FrameSetFacet();

extern int GanttMan();
extern void GanttErr();

/* Functions below should only be called by GGI */
extern boolean GanttRun();  /* FIXME: Not in ganttIfc.h */
extern boolean GanttInit(); /* FIXME: Not in ganttIfc.h */
extern void FindClear();
#else
extern void FrameSetFacet();
extern int CreateFrames();
extern int Frame();
extern int ClearFrames();
extern int DestroyFrames();
extern int GanttMan();
extern void GanttErr();
/* extern void FindAndMarkError(); */
/* Called by POct */
extern boolean FindNameSet();

/* Functions below should only be called by GGI */
extern boolean GanttRun();
extern boolean GanttInit();
extern void FindClear();
extern int RpcFindName();
void FindAndMark(/* octObject facetP, char *name, int pattern */);
#endif /* __cplusplus */

typedef struct RgbValue_s {
    unsigned short red;
    unsigned short green;
    unsigned short blue;
} RgbValue;

#endif /* GANTTIFC_H */
