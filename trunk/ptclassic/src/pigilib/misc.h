#ifndef _MISC_H
#define _MISC_H 1

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

/* misc.h  edg
Version identification:
@(#)misc.h	1.12	3/21/96
*/

/* Define RPCSpot and lsList data types, respectively */
#include "rpc.h"               /* Must be first so pthreads.h is
                                * included first under hpux10
                                */

#include "list.h"

extern int ManPage ARGS((char *starName));
extern int Man ARGS((RPCSpot *spot, lsList cmdList, long userOptionWord));
extern int ShowFacetNum ARGS((RPCSpot *spot, lsList cmdList,
			      long userOptionWord));
extern int PrintFacet ARGS((RPCSpot *spot, lsList cmdList,
			    long userOptionWord));
extern int ERFilterDesign ARGS((RPCSpot *spot, lsList cmdList,
				long userOptionWord));
extern int WFilterDesign ARGS((RPCSpot *spot, lsList cmdList,
			       long userOptionWord));
extern int Profile ARGS((RPCSpot *spot, lsList cmdList, long userOptionWord));
extern int RpcOpenFacet ARGS((RPCSpot *spot, lsList cmdList,
			      long userOptionWord));
extern int RpcLoadStar ARGS((RPCSpot *spot, lsList cmdList,
			     long userOptionWord));
extern int RpcLoadStarPerm ARGS((RPCSpot *spot, lsList cmdList,
				 long userOptionWord));

#endif /* _MISC_H */
