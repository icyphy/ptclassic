/* $Id$ */

/*
 * Copyright (C) 1990 Regents of the University of California
 * All rights reserved.
 */

/*
 * Transforming Conditionals into the basic set 
 */

#include "flowgraph.h"

/* global functions */
extern EdgePointer DuplicateEdge();
extern NodePointer CreateNewNode();
extern GraphPointer GraphInput();
extern char *GetFunction(), *Intern();

/* static variables */
static GraphPointer GEGraph, EQGraph, NOTGraph;

Conditionals(Graph)
GraphPointer Graph;
{
    printmsg(NULL, "transforming conditionals ...\n");

    GEGraph = GraphInput("cond_ge", FALSE);
    EQGraph = GraphInput("cond_e", FALSE);
    NOTGraph = GraphInput("inv", FALSE);

    ConditionalsAux(Graph);
}

ConditionalsAux(Graph)
GraphPointer Graph;
{
    char *Function;
    NodePointer Node;

    for (Node = Graph->NodeList; Node != NULL; Node = Node->Next) {
	if (IsHierarchyNode(Node)) 
	    ConditionalsAux(Node->Master);
	Function = GetFunction(Node);

	if (!strcmp(Function, "<")) {
	    Node->Master = GEGraph;
	    InsertNotNode(Graph,Node);
	}
	else if (!strcmp(Function, ">")) {
	    Commutativity(Node);
	    Node->Master = GEGraph;
	    InsertNotNode(Graph,Node);
	}
	else if (!strcmp(Function, "!=")) {
	    Node->Master = EQGraph;
	    InsertNotNode(Graph,Node);
	}
	else if (!strcmp(Function, "<=")) {
	    Commutativity(Node);
	    Node->Master = GEGraph;
	}
    }
}

InsertNotNode(Graph, Node)
GraphPointer Graph;
NodePointer Node;
{
    EdgePointer OutEdge, NewEdge;
    NodePointer NotNode;
    char NewName[WORDLENGTH];

    OutEdge = (EdgePointer) Node->OutEdges->Entry;
    DisconnectNodeFromEdge(Node, NodeNode, OutEdge);
    sprintf(NewName, "%sinv", OutEdge->Name);
    NewEdge = DuplicateEdge(Graph, OutEdge, NewName);
    ConnectNodeToEdge(Node, NodeNode, NewEdge);
    NotNode = CreateNewNode(Graph, UniqueName("invnode"),
	Intern("data"), NOTGraph);
    ConnectEdgeToNode(NewEdge, NotNode, NodeNode);
    ConnectNodeToEdge(NotNode, NodeNode, OutEdge);
}
