/* $Id$ */

/*
 * Copyright (C) 1990 Regents of the University of California
 * All rights reserved.
 */

/*
 * some routines to help in the simplae flowgraph transformations
 */

#include "flowgraph.h"

/* definitions */
#define EPSILON (1.0E-20)

/* global functions */
extern char *Intern();
extern ListPointer SetAttribute(), ListSelectiveAdd(), new_list();
extern ListPointer RemoveAttribute();

int
IsZeroEdge(Edge)
EdgePointer Edge;
{
    float Value;

    /* Check if this is a constant edge with value 0 */
    if (Edge == NULL || strcmp(Edge->Class, "constant") ||
        GetNumAttribute(Edge->Attributes, "value", &Value) == FALSE)
	return (FALSE);
    return ((Value < EPSILON && Value > -EPSILON) ? TRUE : FALSE);
}

int
IsOneEdge(Edge)
EdgePointer Edge;
{
    float Value;

    /* Check if this is a constant edge with value 1 */
    if (Edge == NULL || strcmp(Edge->Class, "constant") ||
        GetNumAttribute(Edge->Attributes, "value", &Value) == FALSE)
	return (FALSE);
    return ((Value - 1.0 < EPSILON && Value - 1.0 > -EPSILON) ? TRUE : FALSE);
}

BypassIdentityNode(Graph, Node, Edge)
GraphPointer Graph;
NodePointer Node;
EdgePointer Edge;
{
    /* This function is almost identical to the ShortCiurcuitNode function
     * in flowLib. However, here I can choose the Edge I would like to
     * use as new input edge.
     */
    EdgePointer OutEdge;
    
    OutEdge = (EdgePointer) Node->OutEdges->Entry;
    ReplaceEdgeInFanout(OutEdge, Edge);
    EliminateEdge(Graph, OutEdge);
    EliminateNode(Graph, Node);
}
    
PerformGraphCleanUp(Graph)
GraphPointer Graph;
{
    /* Make sure that we don't have any spurious old nodes or edges still
     * hanging around
     */

    NodePointer Node, NextNode;
    EdgePointer Edge, NextEdge;

    for (Node = Graph->NodeList; Node != NULL; Node = NextNode) {
	NextNode = Node->Next;

	if (IsHierarchyNode(Node)) {
            PerformGraphCleanUp(Node->Master);
            continue;
        }

	if (Node->OutEdges == NULL && Node->OutControl == NULL && 
	    !IsOutputNode(Node))
	    EliminateNode(Graph, Node);
    }

    for (Edge = Graph->EdgeList; Edge != NULL; Edge = NextEdge) {
	NextEdge = Edge->Next;
	if (Edge->OutNodes == NULL &&
	   (!strcmp(Edge->Class, "data") || !strcmp(Edge->Class, "constant")))
	    EliminateEdge(Graph, Edge);
    }
}	

EliminateAllocation(Graph)
GraphPointer Graph;
{
    /* 
     * Remove Previous Allocations
     */
    Graph->Attributes = RemoveAttribute(Graph->Attributes, "allocate");
    Graph->Attributes = RemoveAttribute(Graph->Attributes, "registers");
    Graph->Attributes = RemoveAttribute(Graph->Attributes, "exu_min_bound");
    Graph->Attributes = RemoveAttribute(Graph->Attributes, "exu_cost");
    EliminateAllocationAux(Graph);
}

EliminateAllocationAux(Graph)
GraphPointer Graph;
{
    NodePointer Node;

    for (Node = Graph->NodeList; Node != NULL; Node = Node->Next) {
	if (IsHierarchyNode(Node)) {
            EliminateAllocationAux(Node->Master);
            continue;
        }
	Node->Attributes = RemoveAttribute(Node->Attributes, "exu_class");
	Node->Attributes = RemoveAttribute(Node->Attributes, "duration");
	Node->Attributes = RemoveAttribute(Node->Attributes, "delay");
    }
}	


MarkIteratorEdges(Graph)
GraphPointer Graph;
{
    NodePointer Node;
    EdgePointer Edge;
    char *Iterator;

    /* Find the iterator edge (if present) and mark */
    for (Node = Graph->NodeList; Node != NULL; Node = Node->Next) {
	if (IsHierarchyNode(Node) == FALSE)
	    continue;

	MarkIteratorEdges(Node->Master);

	if (strcmp(GetFunction(Node), "iteration"))
	    continue;

	Iterator = (char *) GetAttribute(Node->Arguments, "index");

	if (Iterator == NULL)
	    continue;

	for (Edge = Node->Master->EdgeList; Edge != NULL; Edge = Edge->Next) {
	    if (Edge->Name == Iterator) {
		Edge->Class = Intern("iterator");
		break;
	    }
	}
    }
}

MarkIOEdges(Graph)
GraphPointer Graph;
{
    char *InputEdge, *OutputEdge;
    ListPointer Ptr;
    EdgePointer Edge;
    NodePointer Node;

    /* Traverse the hierarchy */
    for (Node = Graph->NodeList; Node != NULL; Node = Node->Next) 
	if (IsHierarchyNode(Node))
	    MarkIOEdges(Node->Master);

    /* mark the class of IO edges with "input" or "output" */

    InputEdge = Intern("input");
    OutputEdge = Intern("output");

    for (Node = Graph->NodeList; Node != NULL; Node = Node->Next) {
	if (IsInputNode(Node)) {
	    if (Node->OutEdges != NULL) {
	        Edge = (EdgePointer) Node->OutEdges->Entry;
	        Edge->Class = InputEdge;
	    }
	}
	else if (IsOutputNode(Node)) {
	    if (Node->InEdges != NULL) {
		Edge = (EdgePointer) Node->InEdges->Entry;
	        Edge->Class = OutputEdge;
	    }
	}
    }
	
    for (Ptr = Graph->InEdges; Ptr != NULL; Ptr = Ptr->Next) {
	Edge = (EdgePointer) Ptr->Entry; 
	Edge->Class = InputEdge;
    }
    for (Ptr = Graph->OutEdges; Ptr != NULL; Ptr = Ptr->Next) {
	Edge = (EdgePointer) Ptr->Entry; 
	Edge->Class = OutputEdge;
    }
}

MergeTypes(Edge, RefEdge)
EdgePointer Edge, RefEdge;
{
    char *Type1, *Type2;
    ListPointer Ptr, Next, Cast1, Cast2;

    Type1 = (char *) GetAttribute(Edge->Arguments, "type");
    Cast1 = (ListPointer) GetAttribute(Edge->Arguments, "casted");

    Type2 = (char *) GetAttribute(RefEdge->Arguments, "type");
    Cast2 = (ListPointer) GetAttribute(RefEdge->Arguments, "casted");

    if (Type1 != Type2) {
	Ptr = new_list(CharNode, Type1);
	Cast2 = ListSelectiveAdd(Ptr, Cast2);
    }

    for (Ptr = Cast1; Ptr != NULL; Ptr = Next) {
	Next = Ptr->Next;
	Type1 = (char *) Ptr->Entry;
	if (Type1 != Type2) {
	    Ptr->Next = NULL;
	    Cast2 = ListSelectiveAdd(Ptr, Cast2);
	}
    }
    RefEdge->Arguments = SetAttribute("casted", ListNode, Cast2,
	RefEdge->Arguments);
}

char *
Int2Asc(Number)
int Number;
{
    char Buffer[30];

    sprintf(Buffer, "%d", Number);
    return (Intern(Buffer));
}
