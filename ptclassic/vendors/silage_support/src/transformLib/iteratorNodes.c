/* $Id$ */

/*
 * Copyright (C) 1990 Regents of the University of California
 * All rights reserved.
 */

/* routines to provide the extra node srequired to implement iterator
 * functions (when they have not been unrolled)
 */

#include "flowgraph.h"
#include <math.h>

/* definitions */
#define DEFAULT_ITERATIONS 1 /* Default number of iterations of a do body */

/* Global Functions */
extern GraphPointer GraphInput();
extern EdgePointer CreateNewEdge();
extern EdgePointer DuplicateEdge();
extern ListPointer SetAttribute();
extern NodePointer CreateNewNode();
extern char *UniqueName(), *Intern(), *GetFunction(), *Int2Asc(), *GetType();

/* Local Functions and Variables */
static GraphPointer LpDelayGraph, AddGraph, GEGraph, LTGraph;
static doFlag = FALSE;

TransformIteratorNodes(Graph)
GraphPointer Graph;
{
    /*
     * This routine transforms iterator nodes
     */

    printmsg(NULL, "transforming iterator and do nodes ...\n");
    AddGraph = GraphInput("add", FALSE);
    LpDelayGraph = GraphInput("lpdelay", FALSE);
    GEGraph = GraphInput("cond_ge", FALSE);
    LTGraph = GraphInput("cond_l", FALSE);

    TransformIteratorNodesAux(Graph);
    TransformDoNodesAux(Graph);
    if (doFlag == TRUE)
	FlattenGraph(Graph, TRUE);
}


TransformIteratorNodesAux(Graph)
GraphPointer Graph;
{

    GraphPointer SubGraph;
    NodePointer Node, DelayNode, AddNode, TestNode;
    EdgePointer IterEdge, NextEdge, StatusEdge, StepEdge, EndEdge;
    char *Iterator, NextName[WORDLENGTH];
    char *IterEdgeType, Buffer[20];
    int Min, Max, Step;

    /* Descend in the hierarchy */
    for (Node = Graph->NodeList; Node != NULL; Node = Node->Next) {
        if (IsHierarchyNode(Node) == FALSE)
	    continue;

    	TransformIteratorNodesAux(Node->Master);

	if (strcmp(GetFunction(Node), "iteration"))
	    continue;

        Iterator = (char *) GetAttribute(Node->Arguments, "index");
	Min = (int) GetAttribute(Node->Arguments, "min");
	Max = (int) GetAttribute(Node->Arguments, "max");
	Step = (int) GetAttribute(Node->Arguments, "step");
	if (Step == 0)
	    Step = (Max >= Min) ? 1 : -1;
	
	SubGraph = Node->Master;

	DelayNode = CreateNewNode(SubGraph, UniqueName("iternode"),
	        Intern("iteration_control"), LpDelayGraph);
	DelayNode->Arguments = SetAttribute("init", CharNode,
	    Int2Asc((Step > 0) ? Min : Max), DelayNode->Arguments);
	AddNode = CreateNewNode(SubGraph, UniqueName("iternode"),
	    Intern("iteration_control"), AddGraph);
	TestNode = CreateNewNode(SubGraph, UniqueName("iternode"),
	    Intern("iteration_control"), (Step > 0) ? GEGraph : LTGraph);

	for (IterEdge = SubGraph->EdgeList; IterEdge != NULL; 
	     IterEdge = IterEdge->Next) 
	    if (IterEdge->Name == Iterator) 
		break;

	/* Disconnect IterEdge from Graph */
	if (IsMemberOfList(SubGraph, GraphNode, IterEdge->InNodes))
	    DisconnectNodeFromEdge(SubGraph, GraphNode, IterEdge);

	IterEdgeType = GetType(IterEdge);

	sprintf(NextName, "%sNext", Iterator);
	NextEdge = CreateNewEdge(SubGraph, Intern(NextName), Intern("data"));
	NextEdge->Arguments = SetAttribute("type", CharNode, IterEdgeType,
	    NextEdge->Arguments);

	EndEdge = CreateNewEdge(SubGraph, UniqueName("iteredge"),
	    Intern("constant"));
	EndEdge->Attributes = SetAttribute("value", CharNode, 
	    Int2Asc((Step > 0) ? Max : Min + 1), EndEdge->Attributes);
	SetType(EndEdge, IterEdgeType);
	StepEdge = CreateNewEdge(SubGraph, UniqueName("iteredge"),
	    Intern("constant"));
	StepEdge->Attributes = SetAttribute("value", CharNode, 
	    Int2Asc(Step), StepEdge->Attributes);
	SetType(StepEdge, IterEdgeType);
	StatusEdge = CreateNewEdge(SubGraph, UniqueName("iteredge"),
	    Intern("break"));
	SetType(StatusEdge, Intern("bool"));

	ConnectNodeToEdge(AddNode, NodeNode, NextEdge);
	ConnectEdgeToNode(NextEdge, DelayNode, NodeNode);
	ConnectNodeToEdge(DelayNode, NodeNode, IterEdge);
	ConnectEdgeToNode(IterEdge, AddNode, NodeNode);
	ConnectEdgeToNode(StepEdge, AddNode, NodeNode);
	ConnectEdgeToNode(IterEdge, TestNode, NodeNode);
	ConnectEdgeToNode(EndEdge, TestNode, NodeNode);
	ConnectNodeToEdge(TestNode, NodeNode, StatusEdge);

	/* Add a control edge to incorporate the controller delay
	   This is not enabled at present
	 */
	/*
	AddControlEdgeDeLuxe(SubGraph, TestNode, NodeNode, SubGraph,
	    GraphNode, ">=", ControllerDelay());
	*/
    }
}

int
DeduceEdgeType(Edge, invalue)
EdgePointer Edge;
int invalue;
{
    NodePointer OutNode;
    ListPointer Ptr, Ptr1;
    EdgePointer InEdge, OutEdge;
    int const, outvalue, value;
    char *EdgeType, *InEdgeType, *Function;

    outvalue = invalue;
    for (Ptr = Edge->OutNodes; Ptr != NULL; Ptr = Ptr->Next) {
	if (Ptr->Type != NodeNode) continue;
	OutNode = (NodePointer)Ptr->Entry;
	const = 1;
	for (Ptr1 = OutNode->InEdges; Ptr1 != NULL; Ptr1 = Ptr1->Next) {
	    if (Ptr1->Type != EdgeNode) continue;
	    InEdge = (EdgePointer) Ptr1->Entry;
	    if (IsConstantEdge(InEdge)) {
		const = atoi((char *)GetAttribute(InEdge->Attributes, "value"));
		break;
	    }
	}
	Function = GetFunction(OutNode);
	if (strcmp(Function, "*") == 0)
	    invalue *= const;
	else if (strcmp(Function, "/") == 0)
	    invalue /= const;
	else if (strcmp(Function, "+") == 0)
	    invalue += const;
	else if (strcmp(Function, "-") == 0)
	    invalue -= const;
	else
	    continue;

	for (Ptr1 = OutNode->OutEdges; Ptr1 != NULL; Ptr1 = Ptr1->Next) {
	    if (Ptr1->Type != EdgeNode) continue;
	    OutEdge = (EdgePointer) Ptr1->Entry;
	    value = DeduceEdgeType(OutEdge, invalue);
	    outvalue = (value > outvalue) ? value : outvalue;
	}
    }
    return(outvalue);
}

int
ControllerDelay()
{
    /* This routine returns the number of pipeline stages present in the
       controller - As this is architecture and clocking dependent, this
       information should be flexible and not hardcoded.
     */
     return (0);
}

TransformDoNodesAux(Graph)
GraphPointer Graph;
{
    NodePointer Node, N;
    EdgePointer InEdge;
    GraphPointer LoopGraph;
    char *Function;

    /* Descend in the hierarchy */
    for (Node = Graph->NodeList; Node != NULL; Node = Node->Next) {
        if (IsHierarchyNode(Node) == FALSE)
	    continue;

    	TransformDoNodesAux(Node->Master);

	if (!strcmp(GetFunction(Node), "do")) {
	    SetFunction(Node, "func");
	    continue;
	}
	if (strcmp(GetFunction(Node), "dobody"))
	    continue;

	SetFunction(Node, "do");
	doFlag = TRUE;

	/* Eliminate the exit node and replace by connection to controller */
	LoopGraph = Node->Master;
	for (N = LoopGraph->NodeList; N != NULL; N = N->Next) {
	    Function = GetFunction(N);
	    if (!strcmp(Function, "exit"))
		break;
	}
	if (N != NULL) {
	    InEdge = (EdgePointer) N->InEdges->Entry;
	    InEdge->Class = Intern("break");
	    EliminateNode(LoopGraph, N);
	}

	/* Fill in the average number of iterations */
	Node->Arguments = SetAttribute("avg", IntNode,
	    DEFAULT_ITERATIONS, Node->Arguments);
    }
}

ResolveIteratorTypes(Graph)
GraphPointer Graph;
{
    NodePointer Node;
    GraphPointer SubGraph;
    EdgePointer IterEdge;
    char *Iterator, *IterEdgeType;
    char Buffer[WORDLENGTH];
    int Max;

    for (Node = Graph->NodeList; Node != NULL; Node = Node->Next) {
        if (IsHierarchyNode(Node) == FALSE)
	    continue;

/*
    	TransformIteratorNodesAux(Node->Master);
*/
        ResolveIteratorTypes(Node->Master);

	if (strcmp(GetFunction(Node), "iteration"))
	    continue;

        Iterator = (char *) GetAttribute(Node->Arguments, "index");
	Max = (int) GetAttribute(Node->Arguments, "max");
	
	SubGraph = Node->Master;

	for (IterEdge = SubGraph->EdgeList; IterEdge != NULL; 
	     IterEdge = IterEdge->Next) 
	    if (IterEdge->Name == Iterator) 
		break;

	/* Traverse IterEdge through nodes to determine max value */
	Max = DeduceEdgeType(IterEdge, Max);
	sprintf(Buffer, "int<%d>", ((int) (log2((double) Max))) + 2);
	IterEdgeType = Intern(Buffer);
	SetType(IterEdge, IterEdgeType);
    }
}
