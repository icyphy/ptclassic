#ifndef ICON_H
#define ICON_H 1
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
/* icon.h  edg
Version identification:
$Id$
*/

#ifdef __cplusplus
#include "rpc.h"     /* needed to define "RPCSpot" */

extern int RpcMkStar(RPCSpot *spot, lsList cmdList, long userOptionWord);
extern int MkStar(char *name, char *domain, char *dir, char *palette);
extern int RpcMkSchemIcon(RPCSpot *spot, lsList cmdList, long userOptionWord);
extern int RpcLookInside(RPCSpot *spot, lsList cmdList, long userOptionWord);
extern int RpcEditIcon(RPCSpot *spot, lsList cmdList, long userOptionWord);
extern int IconFileToSourceFile(char *iconFile, char *sourceFile, char *domain);
extern int AbsPath(char *path, char *abs_path);
extern int ptMkStar(char* name, char* domain, char* dir, char* palette);
                     /* used by POct - SetMkSchemIcon */
extern int GetTildePath(octObject *facetPtr, char *tPath);
#else
extern int RpcMkStar();
extern int MkStar();
extern int RpcMkSchemIcon();
extern int RpcLookInside();
extern int RpcShowName();
extern int RpcEditIcon();
extern int IconFileToSourceFile();
extern int AbsPath(/* char *path, char *abs_path */);
extern int GetTildePath();  /* used by POct - SetMkSchemIcon */
#endif /* __cplusplus */

#endif /* ICON_H */
