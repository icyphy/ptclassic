/* $Id$ */

#include "sfg.h"
#include "dag.h"

char *Intern();
ListPointer new_list(), ListAppend();
ListPointer SetAttribute();
pointer GetAttribute();

static ListPointer CycleEdge[MAXINDEXLEVEL];
extern int indexlevel;
extern int maxindexlevel;

/**************************************************************************
  InitCycleEdge() -- Initializes CycleEdge[] at indexlevel 
****************************************************************************/
InitCycleEdge()
{
    CycleEdge[indexlevel] = NULL;
}

/**************************************************************************
   ClearCycleEdge() -- Clears Cycle Edges from indexlevel 0 to
   maxindexlevel.
****************************************************************************/
ClearCycleEdge()
{
    register int i;

    for (i = 0; i<= maxindexlevel; i++)
	ClearList(CycleEdge[i]);
}

/**************************************************************************
   IsCycleEdge() -- Checks to see if edge was a delay cycle edge from
   lower level hierarchy.
****************************************************************************/
IsCycleEdge(edge)
EdgePointer edge;
{
    ListPointer p1;
    int i;

    for (i = indexlevel+1; i<= maxindexlevel; i++) {
	for (p1 = CycleEdge[i]; p1 != NULL; p1 = p1->Next) {
	    if (edge == (EdgePointer) p1->Entry)
		return(TRUE);
	}
    }

    return(FALSE);
}

/**************************************************************************
   AddCycleEdge() -- Adds Cycle edge "e" to CycleEdge[] at current indexlevel
****************************************************************************/
AddCycleEdge(e)
EdgePointer e;
{
    CycleEdge[indexlevel] = ListAppend(new_list(EdgeNode, e),
                                                CycleEdge[indexlevel]);
}

/**************************************************************************
   RemoveCycleEdge() -- Removes Cycle edge "e" to CycleEdge[] at current 
   indexlevel
****************************************************************************/
RemoveCycleEdge(e)
EdgePointer e;
{
    ListPointer RemoveEntryFromList();

    CycleEdge[indexlevel] = RemoveEntryFromList(EdgeNode, (pointer)e,
                                CycleEdge[indexlevel]);
}

/**************************************************************************
   PromoteCycleEdge() -- Process leftover edges in CycleEdge[], and put in 
   EdgeList or ControlList
****************************************************************************/
PromoteCycleEdge()
{
    ListPointer ptr;
    extern EdgePointer ControlList, EdgeList;
    EdgePointer EdgeListAppend();

    for (ptr = CycleEdge[indexlevel]; ptr != NULL; ptr = ptr->Next) {
        printmsg(NULL, "Promoting Cycle Edge %s\n", EP(ptr)->Name);
        if (strcmp(EP(ptr)->Class, "control") == 0)
            ControlList = EdgeListAppend(EP(ptr), ControlList);
        else
            EdgeList = EdgeListAppend(EP(ptr), EdgeList);
    }
}

