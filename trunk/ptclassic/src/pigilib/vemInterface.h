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
/* vemInterface.h  edg
Version identification:
$Id$
*/
#ifdef __cplusplus
#ifndef OCT_H
/* Do the right thing for sol2 boolean defs.  oct.h must be included
 * first so sys/types.h is included correctly.  See octtools/include/port.h
 */
#include "oct.h"
#endif

extern void PrintCon();
extern void PrintConLog();
extern void PrintErr();
extern void PrintDebug();
extern void PrintDebugSet();
extern boolean PrintDebugGet();
extern void ViInit();
extern void ViTerm();
extern char *ViGetName();
extern boolean MyCopyFacet();
extern boolean ViKillBuf(octObject *facetPtr);
extern boolean ViGetErrWindows();
extern void ViSetErrWindows(boolean state);

#else /* __cplusplus */

extern void PrintCon();
extern void PrintConLog();
extern void PrintErr();
extern void PrintDebug();
extern void PrintDebugSet();
extern boolean PrintDebugGet();
extern void ViInit();
extern void ViTerm();
extern char *ViGetName();
extern boolean MyCopyFacet();
extern boolean ViKillBuf(/* octObject *facetPtr */);
extern boolean ViGetErrWindows();
extern void ViSetErrWindows(/* boolean state */);
#define ViDone() \
    ViTerm(); \
    vemPrompt(); \
    return (RPC_OK);
#endif /* __cplusplus */
