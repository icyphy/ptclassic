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
/* edit.h  edg
Version identification:
$Id$
*/

#ifdef __cplusplus

/* Define lsList and RPCSpot data types, respectively */
#include "list.h"
#include "rpc.h"

extern int RpcFindName(RPCSpot *spot, lsList cmdList, long userOptionWord);
extern int EditFormalParams(octObject *galFacetPtr);
extern int EditParams(RPCSpot *spot, lsList cmdList, long userOptionWord);
extern int EditPragmas(RPCSpot *spot, lsList cmdList, long userOptionWord);
extern boolean OpenPaletteInit();
extern int RpcOpenPalette(RPCSpot *spot, lsList cmdList, long userOptionWord);
extern int RpcEditComment(RPCSpot *spot, lsList cmdList, long userOptionWord);
extern int RpcEditDomain(RPCSpot *spot, lsList cmdList, long userOptionWord);
extern int RpcEditSeed(RPCSpot *spot, lsList cmdList, long userOptionWord);
extern int RpcEditTarget(RPCSpot *spot, lsList cmdList, long userOptionWord);

#else

extern int RpcFindName();
extern int EditFormalParams();
extern int EditParams();
extern int EditPragmas();
extern boolean OpenPaletteInit();
extern int RpcOpenPalette();
extern int RpcEditComment();
extern int RpcEditDomain();
extern int RpcEditSeed();
extern int RpcEditTarget();

#endif
