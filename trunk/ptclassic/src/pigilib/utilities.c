/* utilities.c  eal
Version identification:
$Id$
*/


/* Includes */
#include "rpc.h"
#include "exec.h"

/* Invoke a standard facet to plot a signal */
int 
RpcPlotSignal(spot, cmdList, userOptionWord) /* ARGSUSED */
RPCSpot *spot;
lsList cmdList;
long userOptionWord;
{
	return RunUniverse(
		"$PTOLEMY/src/domains/sdf/utilities/plot", 100000);
}

/* Invoke a standard facet to plot a signal */
int 
RpcPlotCxSignal(spot, cmdList, userOptionWord) /* ARGSUSED */
RPCSpot *spot;
lsList cmdList;
long userOptionWord;
{
	return RunUniverse(
		"$PTOLEMY/src/domains/sdf/utilities/plotCx", 100000);
}


/* Invoke a standard facet to plot a DFT */
int 
RpcDFT(spot, cmdList, userOptionWord) /* ARGSUSED */
RPCSpot *spot;
lsList cmdList;
long userOptionWord;
{
	return RunUniverse("$PTOLEMY/src/domains/sdf/utilities/dft",1);
}

/* Invoke a standard facet to plot a DFT */
int 
RpcDFTCx(spot, cmdList, userOptionWord) /* ARGSUSED */
RPCSpot *spot;
lsList cmdList;
long userOptionWord;
{
	return RunUniverse("$PTOLEMY/src/domains/sdf/utilities/dftCx",1);
}
