/*
 * $Id$
 * Program to check if the entered graph is compatible with the current
 * HYPER version (2.0)
 */

#include "dag.h"
#include "st.h"

/* Global Functions */
char *GetFunction();

int
CheckGraphValidity(Graph)
GraphPointer Graph;
{
    /* Only 1 dimensional vectors are supported at present */

    if (CheckArrayDimensions(Graph) == FALSE)
	return (FALSE);

    /* Delays of Arrays are not supported */

    /* A number of operations are not supported (yet) */
    if (CheckOperations(Graph) == FALSE)
	return (FALSE);

    return (TRUE);
}

int
CheckArrayDimensions(Graph)
GraphPointer Graph;
{
    NodePointer Node;
    char *Function;

    for (Node = Graph->NodeList; Node != NULL; Node = Node->Next) {
	if (IsHierarchyNode(Node)) {
	    if (CheckArrayDimensions(Node->Master) == FALSE)
		return (FALSE);
	    continue;
	}
	Function = GetFunction(Node);
	if (!strcmp(Function, "read")) {
	    if (Node->InEdges != NULL && Node->InEdges->Next != NULL) {
		printmsg(NULL, 
		    "Multi-Dimensional Arrays are Not Supported Yet\n");
		    return (FALSE);
	    }
	}
	else if (!strcmp(Function, "write")) {
	    if (Node->InEdges->Next != NULL &&
		Node->InEdges->Next->Next != NULL) {
		printmsg(NULL, 
		    "Multi-Dimensional Arrays are Not Supported Yet\n");
		return (FALSE);
	    }
	}
    }
}

int
CheckOperations(Graph)
GraphPointer Graph;
{
    NodePointer Node;
    char *Function;

    for (Node = Graph->NodeList; Node != NULL; Node = Node->Next) {
	if (IsHierarchyNode(Node)) {
	    if (CheckArrayDimensions(Node->Master) == FALSE)
		return (FALSE);
	    continue;
	}
	Function = GetFunction(Node);
	if (!strcmp(Function, "/")) {
	    printmsg(NULL, "Function '/' not Supported in Present Version\n");
	    return (FALSE);
	}
    }
    return (TRUE);
}
