/* $Id$ */

/*
 * Copyright (C) 1990 Regents of the University of California
 * All rights reserved.
 */

/* 
 * Routines to initialize loop delay variables
 */

#include "flowgraph.h"

#define INIT_EDGE_NR 2

/* Global Functions */
extern GraphPointer GraphInput();
extern EdgePointer CreateNewEdge(), GetNthEdge();
extern ListPointer SetAttribute(), ListAppend();
extern NodePointer CreateNewNode();
extern char *UniqueName(), *Intern(), *GetFunction(), *Int2Asc();

/* Local Functions and Variables */
static GraphPointer equalGraph;

InitializeDelays(Graph)
GraphPointer Graph;
{
    /*
     * This routine initializes loop delay nodes
     */

    printmsg(NULL, "initializing delay nodes ...\n");

    equalGraph = GraphInput("equal", FALSE);

    InitializeDelaysAux(NULL, NULL, Graph);
}


InitializeDelaysAux(Top, Master, Graph)
GraphPointer Top;
NodePointer Master;
GraphPointer Graph;
{

    NodePointer Node, TransferNode;
    EdgePointer OutEdge, InitEdge, ValueEdge;
    char *InitEdgeType, EdgeName[WORDLENGTH];
    char *Value;

    /* Descend in the hierarchy */
    for (Node = Graph->NodeList; Node != NULL; Node = Node->Next) {
	/* Traverse the hierarchy */
        if (IsHierarchyNode(Node)) {
    	    InitializeDelaysAux(Graph, Node, Node->Master);
	    continue;
	}

	if (Top == NULL || !IsLpDelayNode(Node))
	    continue;

	if ((InitEdge = GetNthEdge(Node, IN, INIT_EDGE_NR, "data")) != NULL) {
	    if (InitEdge->InNodes != NULL && 
					InitEdge->InNodes->Type == GraphNode)
		continue;    /* no problems here - initialization already
				happens in upper graph */
	    /* Initialization is an expression - move this code out of the
	       loop onto the next hierarchy level */
	    PerformCodeMovement(InitEdge, Master, Top);
	}
	else {
	    /* Implicit initializer - move upwards and include a register
	       transfer operation */
	    if ((Value = (char *) GetAttribute(Node->Arguments, "init")) ==NULL)
		continue;
	    
	    OutEdge = GetNthEdge(Node, OUT, 0, "data");
	    InitEdgeType = (char *) GetAttribute(OutEdge->Arguments, "type");

	    sprintf(EdgeName, "%s_%s", Graph->Name, OutEdge->Name);
	    InitEdge = CreateNewEdge(Graph, Intern(EdgeName), Intern("data"));
	    InitEdge->Arguments = SetAttribute("type", CharNode, InitEdgeType,
		InitEdge->Arguments);
	    ConnectNodeToEdge(Graph, GraphNode, InitEdge);
	    SetNthEdge(Node, InitEdge, IN, INIT_EDGE_NR);

	    TransferNode = CreateNewNode(Top, UniqueName("initnode"),
	        Intern("iteration_init"), equalGraph);
	    ValueEdge = CreateNewEdge(Top, UniqueName("initedge"),
	        Intern("constant"));
	    ValueEdge->Attributes = SetAttribute("value", CharNode, 
	        Value, ValueEdge->Attributes);
	    ValueEdge->Arguments = SetAttribute("type", CharNode, InitEdgeType,
		ValueEdge->Arguments);
	    InitEdge = CreateNewEdge(Top, Intern(EdgeName), Intern("data"));
	    InitEdge->Arguments = SetAttribute("type", CharNode, InitEdgeType,
		InitEdge->Arguments);
	    ConnectEdgeToNode(ValueEdge, TransferNode, NodeNode);
	    ConnectNodeToEdge(TransferNode, NodeNode, InitEdge);
	    ConnectEdgeToNode(InitEdge, Master, NodeNode);
	}
    }
}
