#ifndef MISC_H
#define MISC_H 1

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
/* misc.h  edg
Version identification:
$Id$
*/

#ifdef __cplusplus
#include "rpc.h"     /* needed to define "RPCSpot" */


extern int ShowFacetNum(RPCSpot *spot, lsList cmdList, long userOptionWord);
extern int Man(RPCSpot *spot, lsList cmdList, long userOptionWord);
extern int Profile(RPCSpot *spot, lsList cmdList, long userOptionWord);
extern int PrintFacet(RPCSpot *spot, lsList cmdList, long userOptionWord);
extern int RpcOpenFacet(RPCSpot *spot, lsList cmdList, long userOptionWord);
extern int RpcLoadStar(RPCSpot *spot, lsList cmdList, long userOptionWord);
extern int RpcLoadStarPerm(RPCSpot *spot, lsList cmdList, long userOptionWord);
extern int ERFilterDesign(RPCSpot *spot, lsList cmdList, long userOptionWord);
extern int WFilterDesign(RPCSpot *spot, lsList cmdList, long userOptionWord);
extern int ptOpenFacet( char* Name, char* View, char* Facet);

/* FIXME: The following functions are not in misc.c */
extern int RpcPlot();
extern int RpcCompileStar();
extern int RpcParamCheck();
#else
extern int ShowFacetNum();
extern int Man();
extern int Profile();
extern int PrintFacet();
extern int RpcOpenFacet();
extern int RpcLoadStar();
extern int RpcLoadStarPerm();
extern int ERFilterDesign();
extern int WFilterDesign();
extern int ptOpenFacet();

/* FIXME: The following functions are not in misc.c */
extern int RpcPlot();
extern int RpcCompileStar();
extern int RpcParamCheck();
#endif /* __cplusplus */
#endif /* MISC_H */

