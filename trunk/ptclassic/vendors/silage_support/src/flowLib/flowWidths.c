/* $Id$ */

/* Copyright (C) 1990 Regents of the University of California
 * All rights reserved.
 */

/*
 * Routines to set the widths on nodes and edges
 */

#include "flowgraph.h"

SetEdgeWidths(Graph)
GraphPointer Graph;
{
    EdgePointer Edge;
    NodePointer Node;
    ListPointer SetAttribute();
    char *Class, *Type, *Intern();
    int a, b;

    for (Edge = Graph->EdgeList; Edge != NULL; Edge = Edge->Next) {
	Type = (char *) GetAttribute(Edge->Arguments, "type");
	if (ParseType(Type, &Class, &a, &b) != FALSE) 
            Edge->Attributes = SetAttribute("width", IntNode, (pointer) a,
					    Edge->Attributes);
    }

    for (Node = Graph->NodeList; Node != NULL; Node = Node->Next)
	if (IsHierarchyNode(Node))
	    SetEdgeWidths(Node->Master);
}

SetNodeWidths(Graph)
GraphPointer Graph;
{
    NodePointer Node;
    EdgePointer Edge;
    ListPointer SetAttribute();
    char *Class, *Type, *Intern();
    int a, b;

    for (Node = Graph->NodeList; Node != NULL; Node = Node->Next) {
	if (IsHierarchyNode(Node) || Node->OutEdges == NULL ||
	    GetAttribute(Node->Attributes, "N") != NULL)
	    continue;
	if (Node->OutEdges->Type != EdgeNode)
	    continue;
        Edge = (EdgePointer) Node->OutEdges->Entry;
	Type = (char *) GetAttribute(Edge->Arguments, "type");
        if (ParseType(Type, &Class, &a, &b) != FALSE)
            Node->Attributes = SetAttribute("N", IntNode, (pointer) a,
					    Node->Attributes);
    }
    for (Node = Graph->NodeList; Node != NULL; Node = Node->Next) 
	SetNodeWidths(Node->Master);
}
