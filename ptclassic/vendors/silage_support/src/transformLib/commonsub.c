/* $Id$ */

/*
 * Copyright (C) 1990 Regents of the University of California
 * All rights reserved.
 */

/*
 * Common Subexpression Elimination 
 */

#include "flowgraph.h"

/* global functions */
extern ListPointer ListSelectiveAdd(), ListAdd(), CopyList();
extern NodePointer *TopologicalOrdering();

CommonSubExpression(Graph)
GraphPointer Graph;
{
    printmsg(NULL, "common subexpression elimination ...\n");

    CommonSubExpressionAux(Graph);
}

CommonSubExpressionAux(Graph)
GraphPointer Graph;
{
    NodePointer Node, EquivalentNode, *NodeOrder;
    NodePointer SearchForIdenticalNode();
    EdgePointer OutEdge;
    ListPointer Ptr, Next;
    int NrOfNodes, i;

    for (Node = Graph->NodeList; Node != NULL; Node = Node->Next) {
	if (IsHierarchyNode(Node)) 
	    CommonSubExpressionAux(Node->Master);

	/* Perform name ordering on the edges - if a node has commutativity,
	 * order the edges in alphabetical order
	 */
	if (GetAttribute(Node->Master->Attributes, "commutativity") != NULL)
	    OrderInputEdges(Node);
    }

    /* eliminate common sub-expressions :
     * 1) Topological ordering (make sure we move from in to output)
     * 2) On a node by node base, it is checked if any node is equivalent
     *    (same type, same inputs) to the current node. If yes, the current
     *    node is eliminated.
     */

    NodeOrder = TopologicalOrdering(Graph, &NrOfNodes);

    for (i = NrOfNodes - 1; i >= 0; i--) {
	Node = NodeOrder[i];

	if ((EquivalentNode = SearchForIdenticalNode(Node)) == NULL)
	    continue;

	/* replace node by EquivalentNode */
	OutEdge = (EdgePointer) Node->OutEdges->Entry;
	EliminateNode(Graph, Node);

	/* get rid of the output-edge */
	ReplaceEdgeInFanout(OutEdge,
			    (EdgePointer) EquivalentNode->OutEdges->Entry);

	MergeTypes(OutEdge, (EdgePointer) EquivalentNode->OutEdges->Entry);

	EliminateEdge(Graph, OutEdge);
    }

    cfree(NodeOrder);
}

NodePointer
SearchForIdenticalNode(Node)
NodePointer Node;
{
    ListPointer Ptr;
    NodePointer Candidate;
    EdgePointer Edge;

    /* Locate a node which is completely equivalent (same type, same inputs) */
    if (Node->InEdges == NULL)
	return (NULL);

    for (Ptr = Node->InEdges; Ptr != NULL; Ptr = Ptr->Next) {
	if (Ptr->Type == EdgeNode)
	    break;
    }
    if (Ptr == NULL)
	return (NULL);

    Edge = (EdgePointer) Ptr->Entry;
	
    for (Ptr = Edge->OutNodes; Ptr != NULL; Ptr= Ptr->Next) {
	if (Ptr->Type != NodeNode)
	    continue;
	Candidate = (NodePointer) Ptr->Entry;
	/* Check if Candidate Node is Equivalent to Node */
	if (Candidate != Node && Candidate->Master == Node->Master &&
	    HaveIdenticalInputEdges(Node, Candidate) &&
	    HaveMatchingArguments(Node, Candidate))
	    return (Candidate);
    }
    return (NULL);
}

int
HaveIdenticalInputEdges(Node1, Node2)
NodePointer Node1, Node2;
{
    ListPointer Ptr1, Ptr2;

    for (Ptr1 = Node1->InEdges, Ptr2 = Node2->InEdges;
	 Ptr1 != NULL && Ptr2 != NULL; 
	 Ptr1 = Ptr1->Next, Ptr2 = Ptr2->Next) {
	if (Ptr1->Entry != Ptr2->Entry)
	    return (FALSE);
    }
    return ( (Ptr1 == NULL && Ptr2 == NULL) ? TRUE : FALSE);
}	

int
HaveMatchingArguments(Node1, Node2)
NodePointer Node1, Node2;
{
    ListPointer Ptr1, Ptr2;

    for (Ptr1 = Node1->Arguments; Ptr1 != NULL; Ptr1 = Ptr1->Next) {
	for (Ptr2 = Node2->Arguments; Ptr2 != NULL; Ptr2 = Ptr2->Next) {
	    if (Ptr2->Label == Ptr2->Label) {
	        if (Ptr1->Type != Ptr2->Type || Ptr1->Entry != Ptr2->Entry)
		    return (FALSE);
	        else 
		    break;
	    }
	}
    }
    return (TRUE);
}

OrderInputEdges(Node)
NodePointer Node;
{
    EdgePointer Edge1, Edge2;

    if (Node->InEdges->Next == NULL)
	return;

    Edge1 = (EdgePointer) Node->InEdges->Entry;

    Edge2 = (EdgePointer) Node->InEdges->Next->Entry;

    if (strcmp(Edge2->Name, Edge1->Name) < 0)
	Commutativity(Node);
}
