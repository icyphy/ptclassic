/* $Id$ */

/* Copyright (C) 1990 Regents of the University of California
 * All rights reserved.
 */

/* 
 * Creates a new copy of the flowgraph in the memory
 */

#include "flowgraph.h"
#include "st.h"

/* global functions */
int strcmp();

/* local functions */
static ListPointer GCopyList();
static EdgePointer GCopyEdge();
static NodePointer GCopyNode();

/* local variables */
static st_table *NodeTable;

GraphPointer
CopyGraph(Graph)
GraphPointer Graph;
{
    GraphPointer CopyGraphAux(), GraphCopy;

    NodeTable = st_init_table(strcmp, st_strhash);
    GraphCopy = CopyGraphAux(Graph);
    st_free_table(NodeTable);

    return (GraphCopy);
}

GraphPointer
CopyGraphAux(Graph)
GraphPointer Graph;
{

    EdgePointer GCopyEdgeList();
    NodePointer GCopyNodeList();
    GraphPointer NewGraph, new_graph(), GraphListAppend();

    if (Graph == NULL)
	return (NULL);

    /* Check if graph already has been copied */ 
    if (st_lookup(NodeTable, Graph->Name, &NewGraph) != 0)
	return (NewGraph);

    NewGraph = new_graph();
    st_insert(NodeTable, Graph->Name, NewGraph);
    NewGraph->Name = Graph->Name; 
    NewGraph->Class = Graph->Class;
    NewGraph->InEdges = GCopyList(Graph->InEdges);
    NewGraph->OutEdges = GCopyList(Graph->OutEdges);
    NewGraph->InControl = GCopyList(Graph->InControl);
    NewGraph->OutControl = GCopyList(Graph->OutControl);
    NewGraph->NodeList = GCopyNodeList(Graph->NodeList);
    NewGraph->EdgeList = GCopyEdgeList(Graph->EdgeList);
    NewGraph->ControlList = GCopyEdgeList(Graph->ControlList);
    NewGraph->Arguments = GCopyList(Graph->Arguments);
    NewGraph->Attributes = GCopyList(Graph->Attributes);
    NewGraph->Model = GCopyList(Graph->Model);

    ResolveNodeReferences(NewGraph);
    ResolveEdgeReferences(NewGraph);

    return (NewGraph);
}

NodePointer
GCopyNodeList(NodeList)
NodePointer NodeList;
{
    NodePointer NewList, LastNode, NewNode, Ptr;

    NewList = NULL;
    LastNode = NULL;

    for (Ptr = NodeList; Ptr != NULL; Ptr = Ptr->Next) {
	NewNode = GCopyNode(Ptr);
	if (LastNode != NULL)
	    LastNode->Next = NewNode;
	else
	    NewList = NewNode;
	LastNode = NewNode;
    }
    return (NewList);
}

static NodePointer
GCopyNode(Node)
NodePointer Node;
{
    NodePointer NewNode, new_node();
    GraphPointer CopyGraphAux();

    NewNode = new_node();
    NewNode->Name = Node->Name;
    NewNode->Class = Node->Class;
    NewNode->Master = CopyGraphAux(Node->Master);
    NewNode->Arguments = GCopyList(Node->Arguments);
    NewNode->Attributes = GCopyList(Node->Attributes);
    NewNode->InEdges = GCopyList(Node->InEdges);
    NewNode->OutEdges = GCopyList(Node->OutEdges);
    NewNode->InControl = GCopyList(Node->InControl);
    NewNode->OutControl = GCopyList(Node->OutControl);
    return (NewNode);
}

EdgePointer
GCopyEdgeList(EdgeList)
EdgePointer EdgeList;
{
    EdgePointer NewList, LastEdge, NewEdge, Ptr;

    NewList = NULL;
    LastEdge = NULL;

    for (Ptr = EdgeList; Ptr != NULL; Ptr = Ptr->Next) {
	NewEdge = GCopyEdge(Ptr);
	if (LastEdge != NULL)
	    LastEdge->Next = NewEdge;
	else
	    NewList = NewEdge;
	LastEdge = NewEdge;
    }
    return (NewList);
}

static EdgePointer
GCopyEdge(Edge)
EdgePointer Edge;
{
    EdgePointer NewEdge, new_edge();

    NewEdge = new_edge();
    NewEdge->Name = Edge->Name;
    NewEdge->Class = Edge->Class;
    NewEdge->Arguments = GCopyList(Edge->Arguments);
    NewEdge->Attributes = GCopyList(Edge->Attributes);
    NewEdge->InNodes = GCopyList(Edge->InNodes);
    NewEdge->OutNodes = GCopyList(Edge->OutNodes);
    return (NewEdge);
}

static ListPointer
GCopyList(List)
ListPointer List;
{
    ListPointer Ptr, NewList, NewLast, NewPtr, new_list();
    pointer Entry;
    EntryType Type;

    NewList = NULL;
    NewLast = NULL;
    for (Ptr = List; Ptr != NULL; Ptr = Ptr->Next) {
	switch (Ptr->Type) {
	case (EdgeNode) :
	    Type = CharNode;
	    Entry = (pointer) ((EdgePointer) Ptr->Entry)->Name;
	    break;
	case (NodeNode) :
	    Type = CharNode;
	    Entry = (pointer) ((NodePointer) Ptr->Entry)->Name;
	    break;
	case (GraphNode) :
	    Type = CharNode;
	    Entry = (pointer) ((GraphPointer) Ptr->Entry)->Name;
	    break;
	case (ListNode) :
	    Type = ListNode;
	    Entry = (pointer) GCopyList((ListPointer) Ptr->Entry);
	    break;
	default :
	    Type = Ptr->Type;
	    Entry = Ptr->Entry;
	}
	NewPtr = new_list(Type, Entry);
	NewPtr->Label = Ptr->Label;
	if (NewLast != NULL) 
	    NewLast->Next = NewPtr;
	else
	    NewList = NewPtr;
	NewLast = NewPtr;
    }
    return (NewList);
}
