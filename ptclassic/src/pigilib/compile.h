#ifndef COMPILE_H
#define COMPILE_H 1
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
/* compile.h  edg
Version identification:
$Id$
*/

#ifdef __cplusplus

#include "rpc.h"     /* needed to define "RPCSpot" */

extern boolean CompileFacet(octObject *facetPtr);
extern boolean EssClear();
extern int RpcCompileFacet(RPCSpot *spot, lsList cmdList, long userOptionWord);
extern void CompileInit();
extern void CompileEnd();
extern boolean CompileGal(octObject *galFacetPtr);
extern boolean CompileGalStandalone(octObject *galFacetPtr);
extern boolean CompileGalInst(octObject *galInstPtr, octObject *parentFacetPtr);
#else
extern boolean CompileFacet();
extern boolean EssClear();
extern int RpcCompileFacet();
extern void CompileInit();
extern void CompileEnd();
extern boolean CompileGal();
extern boolean CompileGalStandalone(/* octObject *galFacetPtr */);
extern boolean CompileGalInst(/* octObject *galInstPtr,
			        octObject *parentFacetPtr */); 
extern int RpcRunAllDemos();
#endif /* __cplusplus */
#endif /* COMPILE_H */
