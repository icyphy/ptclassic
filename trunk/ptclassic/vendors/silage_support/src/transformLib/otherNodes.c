/* $Id$ */

/*
 * Copyright (C) 1990 Regents of the University of California
 * All rights reserved.
 */

/*
 * Contains some simple graph transformations :
 * removal of cast nodes and break nodes
 */

#include "flowgraph.h"

/* global functions */
extern char *GetFunction(), *Intern(), *UniqueName();
extern ListPointer SetAttribute(), new_list(), ListSelectiveAdd();
extern ListPointer RemoveAttribute();
extern NodePointer RemoveFromNodeList(), CreateNewNode();
extern EdgePointer DuplicateEdge();
extern GraphPointer GraphInput();

static GraphPointer LeftShiftGraph;
static GraphPointer RightShiftGraph;
static GraphPointer equalGraph;

RemoveEqualNodes(Graph, HierarchyFlag)
GraphPointer Graph;
int HierarchyFlag;
{
    NodePointer Node, Next;
    EdgePointer InEdge, OutEdge;
    char *Function;

    for (Node = Graph->NodeList; Node != NULL; Node = Next) {
        Next = Node->Next;

	if (IsHierarchyNode(Node) && HierarchyFlag == TRUE) {
	    RemoveEqualNodes(Node->Master);
	    continue;
	}

        Function = GetFunction(Node);

        /* equal node */
        if (strcmp(Function, "="))
    	    continue;

	/* Equal nodes are sometimes essential and should not be
	   removed ! */
	InEdge = (EdgePointer) Node->InEdges->Entry;
	OutEdge = (EdgePointer) Node->OutEdges->Entry;

	if (IsInputEdge(InEdge) || IsOutputEdge(OutEdge))
	    continue;

        /* Remove the node */
	ShortCircuitNode(Graph, Node);
    }
}

IntroduceEqualNodes(Graph, HierarchyFlag)
GraphPointer Graph;
int HierarchyFlag;
{
    NodePointer Node, EqualNode;
    EdgePointer Edge, EqualEdge;
    ListPointer Ptr, Next;

    if (HierarchyFlag == TRUE) {
        for (Node = Graph->NodeList; Node != NULL; Node = Node->Next) {
	    if (IsHierarchyNode(Node) && HierarchyFlag == TRUE) 
	        IntroduceEqualNodes(Node->Master, TRUE);
	}
    }

    for (Edge = Graph->EdgeList; Edge != NULL; Edge = Edge->Next) {
	if (!IsConstantEdge(Edge))
	    continue;

	EqualNode = NULL;
	EqualEdge = NULL;

	for (Ptr = Edge->OutNodes; Ptr != NULL; Ptr = Next) {
	    Next = Ptr->Next;
	    if (Ptr->Type != NodeNode || !IsHierarchyNode(Ptr->Entry)) 
		continue;
	    Node = (NodePointer) Ptr->Entry;
	    if (EqualNode == NULL) {
		EqualNode = CreateNewNode(Graph, UniqueName("initnode"),
			Intern("iteration_init"), GraphInput("equal", FALSE));
		EqualEdge = DuplicateEdge(Graph, Edge, UniqueName("initedge"));
		EqualEdge->Class = Intern("data");
		EqualEdge->Attributes = RemoveAttribute(EqualEdge->Attributes, 
		    "value");
		ConnectEdgeToNode(Edge, EqualNode, NodeNode);
		ConnectNodeToEdge(EqualNode, NodeNode, EqualEdge);
	    }
	    ReplaceInputEdge(Node, NodeNode, Edge, EqualEdge);
	}
    }
}

TransformCastNodes(Graph, castFlag)
GraphPointer Graph;
int castFlag;
{
    TransformCastNodesAux(Graph, castFlag);
}

TransformCastNodesAux(Graph, castFlag)
GraphPointer Graph;
int castFlag;
{
    NodePointer Node, Next;
    EdgePointer InEdge, OutEdge;
    ListPointer CastList, Entry;
    int aIn, bIn, aOut, bOut;

    char *Function;
    char *InputType, *OutputType;
    char *InClass, *OutClass;
    
    for (Node = Graph->NodeList; Node != NULL; Node = Next) {
        Next = Node->Next;

	if (IsHierarchyNode(Node)) {
	    TransformCastNodesAux(Node->Master, castFlag);
	    continue;
	}

        Function = GetFunction(Node);

        /* cast node */
        if (strcmp(Function, "cast"))
    	    continue;

        InEdge = (EdgePointer) Node->InEdges->Entry;
        OutEdge = (EdgePointer) Node->OutEdges->Entry; 

	InputType = (char *) GetAttribute(InEdge->Arguments, "type");
	OutputType = (char *) GetAttribute(OutEdge->Arguments, "type");

	ParseType(InputType, &InClass, &aIn, &bIn);
	ParseType(OutputType, &OutClass, &aOut, &bOut);

	if (aIn == aOut && bIn == bOut) {
	    /* Remove the node */
	    ShortCircuitNode(Graph, Node);
	    continue;
	}
	else if (castFlag == TRUE) { /* Cast node stay around as dummy nodes */
	    Node->Class = Intern("dummy");
	    continue;
	}

	if (IsOutputEdge(OutEdge))
	    InEdge->Name = OutEdge->Name;
	ReplaceEdgeInFanout(OutEdge, InEdge);
	InEdge->Arguments = SetAttribute("type", CharNode, OutputType,
					InEdge->Arguments);
	EliminateEdge(Graph, OutEdge);
	EliminateNode(Graph, Node);
    }
}

TransformIONodes(Graph)
GraphPointer Graph;
{
    equalGraph = GraphInput("equal", FALSE);
    TransformOutputNodesAux(Graph);
}

TransformOutputNodesAux(Graph)
GraphPointer Graph;
{
    NodePointer Node, Next, TransferNode;
    EdgePointer InEdge, MatchEdge;

    char *Name;
    
    for (Node = Graph->NodeList; Node != NULL; Node = Next) {
        Next = Node->Next;

	if (IsHierarchyNode(Node)) {
	    TransformOutputNodesAux(Node->Master);
	    continue;
	}

	if (IsOutputNode(Node)) {
	    Node->Class = Intern("dummy");

	    /* Add a transfer operation if needed */
	    if (Node->InEdges == NULL)
		continue;

            InEdge = (EdgePointer) Node->InEdges->Entry;
	    if (InEdge->InNodes->Type == GraphNode || 
	        IsHierarchyNode((NodePointer) InEdge->InNodes->Entry)) {
	        /* Introduce a transfer operation to the output register */
	        Name = InEdge->Name;
	        InEdge->Name = UniqueName(Name);
	        MatchEdge = DuplicateEdge(Graph, InEdge, Name);
	        TransferNode = CreateNewNode(Graph, UniqueName("output"), 
		    Intern("data"), equalGraph);
	        DisconnectEdgeFromNode(InEdge, Node, NodeNode);
	        ConnectEdgeToNode(InEdge, TransferNode, NodeNode);
	        ConnectNodeToEdge(TransferNode, NodeNode, MatchEdge);
	        ConnectEdgeToNode(MatchEdge, Node, NodeNode);
	    }
	}
	else if (IsInputNode(Node)) {
	    /* Check for array inputs */
	    if (Node->OutEdges != NULL || Node->OutControl == NULL)
		continue;
	    
	    /* This is temporary - Have to decide how to enter arrays !! */
	    Node->Class = Intern("dummy");
	}
    }
}

static char *Plus, *Minus, *Multiply, *Divide, *ShiftLeft, *ShiftRight;

RemoveIdentities(Graph)
GraphPointer Graph;
{
    printmsg(NULL, "removal of algebraic identities ...\n");
    Plus = Intern("+");
    Minus = Intern("-");
    Multiply = Intern("*");
    Divide = Intern("/");
    ShiftLeft = Intern("<<");
    ShiftRight = Intern(">>");

    RemoveIdentitiesAux(Graph);
}

RemoveIdentitiesAux(Graph)
GraphPointer Graph;
{
    /* 
     * Eliminate identity operations, such as a + 0, a * 1
     */
    NodePointer Node, Next;
    EdgePointer Edge1, Edge2;
    char *Function;

    for (Node = Graph->NodeList; Node != NULL; Node = Next) {
        Next = Node->Next;

	if (IsHierarchyNode(Node)) {
	    RemoveIdentitiesAux(Node->Master);
	    continue;
	}

        Function = GetFunction(Node);

	if (Node->InEdges == NULL)
	    continue;

	Edge1 = (EdgePointer) Node->InEdges->Entry;
	Edge2 = (Node->InEdges->Next != NULL) ? 
	    (EdgePointer) Node->InEdges->Next->Entry : NULL;

	if (Function == Plus) {
	    if (IsZeroEdge(Edge1))
		BypassIdentityNode(Graph, Node, Edge2);
	    else if (IsZeroEdge(Edge2))
		BypassIdentityNode(Graph, Node, Edge1);
	}
	else if (Function == Minus) {
	    if (IsZeroEdge(Edge2))
		BypassIdentityNode(Graph, Node, Edge1);
	}
	else if (Function == Multiply) {
	    if (IsOneEdge(Edge1))
		BypassIdentityNode(Graph, Node, Edge2);
	    else if (IsOneEdge(Edge2))
		BypassIdentityNode(Graph, Node, Edge1);
	}
	else if (Function == Divide) {
	    if (IsOneEdge(Edge2))
		BypassIdentityNode(Graph, Node, Edge1);
	}
	else if (Function == ShiftLeft || Function == ShiftRight) {
	    if (IsZeroEdge(Edge2))
		BypassIdentityNode(Graph, Node, Edge1);
	}
    }
}
