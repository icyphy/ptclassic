/* $Id$ */

/*
 * Copyright (C) 1990 Regents of the University of California
 * All rights reserved.
 */

/*
 * Routines to eliminate negate nodes - we either try to collapse them or
 * transform them into subtract nodes
 */

#include "flowgraph.h"

/* global functions */
extern char *GetFunction(), *UniqueName(), *Intern();
extern GraphPointer GraphInput();
extern ListPointer SetAttribute();
extern EdgePointer CreateNewEdge();

/* global variables */
static GraphPointer AddGraph;
static GraphPointer SubtractGraph;

CollapseNegateNodes(Graph)
GraphPointer Graph;
{
    printmsg(NULL, "eliminating negate nodes ...\n");
    AddGraph = GraphInput("add", FALSE);
    SubtractGraph = GraphInput("minus", FALSE);

    ContractNegateNodes(Graph);
}


ContractNegateNodes(Graph)
GraphPointer Graph;
{
    /* 
     * Negate nodes are a pain - see if we can collapse them into a subtraction
     * If this does not work, we will map them into a subtraction from a
     * constant.
     */

    NodePointer Node;

    for (Node = Graph->NodeList; Node != NULL; Node = Node->Next) {

	if (IsHierarchyNode(Node)) {
	    ContractNegateNodes(Node->Master);
	    continue;
	}
    }

    while (ContractNegateNodesAux(Graph) == TRUE);

    /* replace remaining negate nodes by subtract nodes */
    EliminateNegateNodes(Graph);
}

int
ContractNegateNodesAux(Graph)
GraphPointer Graph;
{
    NodePointer Node, Next;
    EdgePointer Edge;
    ListPointer Ptr;
    char *Function;
    int rerun = FALSE;

    for (Node = Graph->NodeList; Node != NULL; Node = Next) {
        Next = Node->Next;

        Function = GetFunction(Node);

        /* negate node */
        if (!strcmp(Function, "negate")) {
	    /* run over the fanout and see if we can mess around */
	    if (EliminateNegateNodePart1(Graph, Node) == TRUE)
		rerun = TRUE;
	}
    }
    for (Node = Graph->NodeList; Node != NULL; Node = Next) {
        Next = Node->Next;

        Function = GetFunction(Node);

        /* negate node */
        if (!strcmp(Function, "negate")) {
	    /* run over the fanin and see if we can collapse with subtract */
	    if (EliminateNegateNodePart2(Graph, Node) == TRUE)
		rerun = TRUE;
	}
    }
    return (rerun);
}
	
int
EliminateNegateNodePart1(Graph, Node)
GraphPointer Graph;
NodePointer Node;
{
    ListPointer Ptr1, Ptr2, Next1, Next2;
    NodePointer OutNode;
    EdgePointer Edge;
    char *Function;
    int rerun = FALSE;

    for (Ptr1 = Node->OutEdges; Ptr1 != NULL; Ptr1 = Next1) {
	Next1 = Ptr1->Next;
        if (Ptr1->Type != EdgeNode)
	    continue;
	Edge = (EdgePointer) Ptr1->Entry;

	for (Ptr2 = Edge->OutNodes; Ptr2 != NULL; Ptr2 = Next2) {
	    Next2 = Ptr2->Next;
	    if (Ptr2->Type != NodeNode)
		continue;
	    OutNode = (NodePointer) Ptr2->Entry;
	    Function = GetFunction(OutNode);

	    if (!strcmp(Function, "+")) {
		/* Change from add to minus */
		/* Eliminate the negate node from its fanin */
		/* and make sure that Edge ends up as the second input edge */
		OutNode->Master = SubtractGraph;
		if (OutNode->InEdges->Entry == (pointer) Edge)
		    Commutativity(OutNode);
		EliminateFromFanin(Graph, Node, OutNode);
	    }
	    else if (!strcmp(Function, "-")) {
		/* The transformation depends upon the position of the
		 * negate node
		 */
		OutNode->Master = AddGraph;
		if (OutNode->InEdges->Entry == (pointer) Edge) {
		    SwapNodes(Graph, Node, OutNode);
		    rerun = TRUE;
		}
		else 
		    EliminateFromFanin(Graph, Node, OutNode);
	    }
	    else if (!strcmp(Function, "negate")) {
		/* Remove both nodes */
		if (OutNode != Node->Next) { /* hack - avoids crash ... */
		    EliminateFromFanin(Graph, Node, OutNode);
		    ShortCircuitNode(Graph, OutNode);
		}
	    }
	    else if (!strcmp(Function, "<<") || !strcmp(Function, ">>")) {
		SwapNodes(Graph, Node, OutNode);
		rerun = TRUE;
	    }
	}
    }
    return (rerun);
}

int
EliminateNegateNodePart2(Graph, Node)
GraphPointer Graph;
NodePointer Node;
{
    ListPointer Ptr1;
    EdgePointer InEdge;
    NodePointer InNode;
    char *Function;

    InEdge = (EdgePointer) Node->InEdges->Entry;

    for (Ptr1 = InEdge->InNodes; Ptr1 != NULL; Ptr1 = Ptr1->Next) {
	if (Ptr1->Type != NodeNode)
	    continue;

	InNode = (NodePointer) Ptr1->Entry;
  	Function = GetFunction(InNode);

	if (!strcmp(Function, "-")) {
	    ShortCircuitNode(Graph, Node);
	    Commutativity(InNode);
	}
    }
    return (FALSE);
}

EliminateNegateNodes(Graph)
GraphPointer Graph;
{
    EdgePointer ZeroEdge, InEdge;
    NodePointer Node;
    char *Function;
    int Value;

    for (Node = Graph->NodeList; Node != NULL; Node = Node->Next) {
        Function = GetFunction(Node);

        /* negate node */
        if (strcmp(Function, "negate"))
	    continue;

	Node->Master = SubtractGraph;

	/* Attach to the "0" edge */
	ZeroEdge = CreateNewEdge(Graph, 
		    UniqueName("const_edge"), Intern("constant"));
	ZeroEdge->Attributes = SetAttribute("value", CharNode, "0",
		    ZeroEdge->Attributes);		/* [erc/7-22-92] */
	InEdge = (EdgePointer)Node->InEdges->Entry;
	SetType(ZeroEdge, GetType(InEdge));	 /* [ingrid/9-1-92] */
	ConnectEdgeToNode(ZeroEdge, Node, NodeNode);
	Commutativity(Node);
    }
}
