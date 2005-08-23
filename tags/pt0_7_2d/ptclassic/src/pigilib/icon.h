#ifndef _ICON_H
#define _ICON_H 1

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
/* icon.h  edg
Version identification:
@(#)icon.h	1.12	1/9/96
*/

/* Do the right thing for sol2 boolean defs.  compat.h must be included
 * first so sys/types.h is included correctly.
 */
#include "sol2compat.h"

/* Define data structures lsList, octObject, and RPCSpot, respectively */
#include "list.h"
#include "oct.h"
#include "rpc.h"

extern boolean LookAtFile ARGS((const char* fileName));
extern boolean MkStar ARGS((char *name, char *domain,
			    char *dir, char *palette));
extern int RpcMkStar ARGS((RPCSpot *spot, lsList cmdList, long userOptionWord));
extern int GetTildePath ARGS((octObject *facetPtr, char *tPath));
extern int AbsPath ARGS((char *path, char *abs_path));
extern int RpcMkSchemIcon ARGS((RPCSpot *spot, lsList cmdList,
				long userOptionWord));
extern int RpcShowName ARGS((RPCSpot *spot, lsList cmdList,
			     long userOptionWord));
extern int RpcTycho ARGS((RPCSpot *spot, lsList cmdList,
			       long userOptionWord));
extern int RpcLookInside ARGS((RPCSpot *spot, lsList cmdList,
			       long userOptionWord));
extern int IconFileToSourceFile ARGS((char *iconFile, char *sourceFile,
				      char *domain));
extern int RpcEditIcon ARGS((RPCSpot *spot, lsList cmdList,
			     long userOptionWord));

#endif /* _ICON_H */
