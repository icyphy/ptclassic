#ifndef _VEMINTERFACE_H
#define _VEMINTERFACE_H 1

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

#include "pthreadsCompat.h"	/* Must be first for hpux10 */

/* Do the right thing for sol2 boolean defs.  
 * Under HPUX10, pthreads.h must be included first, so we do it in 
 *   pthreadsCompat.h
 * oct.h must be included second so sys/types.h is included correctly.
 *   See octtools/include/port.h 
 */
#include "oct.h"
#include "rpc.h"

extern void ViSetErrWindows ARGS((boolean state));
extern boolean ViGetErrWindows();
extern void PrintCon ARGS((const char *s));
extern void PrintConLog ARGS((const char *s));
extern void PrintErr ARGS((const char *s));
extern void PrintErrNoTag ARGS((const char *s));
extern void PrintDebug ARGS((const char *s));
extern void PrintDebugSet ARGS((boolean state));
extern boolean PrintDebugGet();
extern void ViInit ARGS((char *name));
extern void ViTerm();
extern char *ViGetName();
extern boolean ViKillBuf ARGS((octObject *facetPtr));
extern vemStatus dmMultiTextTrim ARGS((char *name, int nItems,
				       dmTextItem items[]));

#define ViDone()	ViTerm(); vemPrompt(); return (RPC_OK)

#endif  /* _VEMINTERFACE_H */
