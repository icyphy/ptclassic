#ifndef _EDIT_H
#define _EDIT_H 1

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
/* edit.h  edg
Version identification:
@(#)edit.h	1.19	11/23/95
*/

/* Do the right thing for sol2 boolean defs.  compat.h must be included
 * first so sys/types.h is included correctly.
 */
#include "sol2compat.h"

/* Define data structures lsList, octObject, and RPCSpot, respectively */
#include "list.h"
#include "oct.h"
#include "rpc.h"

extern int RpcFindName ARGS((RPCSpot *spot, lsList cmdList,
			     long userOptionWord));
extern int EditFormalParams ARGS((octObject *galFacetPtr));
extern int EditParams ARGS((RPCSpot *spot, lsList cmdList,
			    long userOptionWord));
extern int EditPragmas ARGS((RPCSpot *spot, lsList cmdList,
			     long userOptionWord));
extern boolean OpenPaletteInit();
extern int RpcOpenPalette ARGS((RPCSpot *spot, lsList cmdList,
				long userOptionWord));
extern int RpcEditComment ARGS((RPCSpot *spot, lsList cmdList,
				long userOptionWord));
extern int RpcEditDomain ARGS((RPCSpot *spot, lsList cmdList,
			       long userOptionWord));
extern int RpcEditSeed ARGS((RPCSpot *spot, lsList cmdList,
			     long userOptionWord));
extern int RpcEditTarget ARGS((RPCSpot *spot, lsList cmdList,
			       long userOptionWord));
extern int RpcGetObjectNames ARGS((RPCSpot *spot, lsList cmdList,
				   long userOptionWord));
extern void getSelectedObjNames ARGS((int* numObjs, char *nameList[]));
extern void clearSelectedObjNames();
extern int getFacetContents ARGS((char* facetName, int* numObjs,
				  const char* fname));

#endif   /* _EDIT_H */
