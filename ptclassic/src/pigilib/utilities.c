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
/* utilities.c  eal
Version identification:
@(#)utilities.c	1.10	11/3/95
*/


/* Standard includes */
#include "local.h"		/* include compat.h, sol2compat.h, ansi.h */

/* Octtools includes */
#include "oct.h"
#include "list.h"		/* define lsList */
#include "rpc.h"		/* define remote procedure calls */

/* Pigilib includes */
#include "exec.h"		/* define RunUniverse */
#include "utilities.h"

/* Invoke a standard facet to plot a signal */
int 
RpcPlotSignal(spot, cmdList, userOptionWord) /* ARGSUSED */
RPCSpot *spot;
lsList cmdList;
long userOptionWord;
{
	return RunUniverse("$PTOLEMY/src/domains/sdf/utilities/plot");
}

/* Invoke a standard facet to plot a signal */
int 
RpcPlotCxSignal(spot, cmdList, userOptionWord) /* ARGSUSED */
RPCSpot *spot;
lsList cmdList;
long userOptionWord;
{
	return RunUniverse("$PTOLEMY/src/domains/sdf/utilities/plotCx");
}


/* Invoke a standard facet to plot a DFT */
int 
RpcDFT(spot, cmdList, userOptionWord) /* ARGSUSED */
RPCSpot *spot;
lsList cmdList;
long userOptionWord;
{
	return RunUniverse("$PTOLEMY/src/domains/sdf/utilities/dft");
}

/* Invoke a standard facet to plot a DFT */
int 
RpcDFTCx(spot, cmdList, userOptionWord) /* ARGSUSED */
RPCSpot *spot;
lsList cmdList;
long userOptionWord;
{
	return RunUniverse("$PTOLEMY/src/domains/sdf/utilities/dftCx");
}
