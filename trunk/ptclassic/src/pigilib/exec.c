/* exec.c  edg
Version identification:
$Id$
*/


/* Includes */
#include <stdio.h>
#include <strings.h>
#include "local.h"
#include "rpc.h"
#include "vemInterface.h"
#include "compile.h"
#include "octIfc.h"
#include "err.h"
#include "util.h"

/* The vars below store the state of the last run command for re-runs */
static int lastIterate;
static octObject lastFacet = {OCT_UNDEFINED_OBJECT};

static boolean
AskIterate(facetPtr, nPtr)
octObject *facetPtr;
int *nPtr;
{
    static dmTextItem item = {"Number of iterations", 1, 20, NULL, NULL};
    char buf[64];
    int oldN;

    if ((oldN = GetIterateProp(facetPtr)) == -1) {
	oldN = 10;
    }
    item.value = sprintf(buf, "%d", oldN);
    ERR_IF2(dmMultiText("Iterations", 1, &item) != VEM_OK, "Aborted entry");
    ERR_IF2((*nPtr = atoi(item.value)) <= 0,
	"Invalid entry, number must be > 0");
    if (*nPtr != oldN) {
	/* set prop only if it changes, to avoid creation of OctChangeRecord */
	SetIterateProp(facetPtr, *nPtr);
    }
    lastIterate = *nPtr;
    return (TRUE);
}

static boolean
Run(facetPtr)
octObject *facetPtr;
{
    int n;

    lastFacet = *facetPtr;
    ERR_IF1(!AskIterate(facetPtr, &n));
    ERR_IF1(!CompileFacet(facetPtr));
    ERR_IF1(!KcRun(n));
    return (TRUE);
}
    
/* 11/7/89 4/24/88
*/
int 
RpcRun(spot, cmdList, userOptionWord)
RPCSpot *spot;
lsList cmdList;
long userOptionWord;
{
    int n;
    octObject facet;

    ViInit("run");
    ErrClear();
    FindClear();
    /* get current facet */
    facet.objectId = spot->facet;
    if (octGetById(&facet) != OCT_OK) {
	PrintErr(octErrorString());
    	ViDone();
    }
    if (!IsUnivFacet(&facet)) {
	PrintErr("Schematic is not a universe");
        ViDone();
    }

    if(! KcSetKBDomain(DEFAULT_DOMAIN)) {
        PrintErr("Failed to set default domain.");
        ViDone();
    }

    if (!Run(&facet)) {
	PrintErr(ErrGet());
        ViDone();
    }
    ViDone();
}

static boolean
ReRun()
{
    ERR_IF1(!CompileFacet(&lastFacet));
    ERR_IF1(!KcRun(lastIterate));
    return (TRUE);
}

/* 12/29/89
*/
int 
RpcReRun(spot, cmdList, userOptionWord)
RPCSpot *spot;
lsList cmdList;
long userOptionWord;
{
    ViInit("re-run");
    ErrClear();

    if(! KcSetKBDomain(DEFAULT_DOMAIN)) {
        PrintErr("Failed to set default domain.");
        ViDone();
    }

    if (lastFacet.type == OCT_UNDEFINED_OBJECT
	|| spot->facet != lastFacet.objectId) {
	PrintErr("Cannot re-run this facet, use 'run'");
        ViDone();
    }
    if (!ReRun()) {
	PrintErr(ErrGet());
        ViDone();
    }
    ViDone();
}

/* mark stars involved in errors detected by kernel */
void
PigiErrorMark(objName)
char *objName;
{
    FindAndMarkError(&lastFacet, objName);
}
