/* $Id$ */

/* Copyright (C) 1990 Regents of the University of California
 * All rights reserved.
 */

/* Support Routines for Node structures */

#include "flowgraph.h"

/* global functions */
extern NodePointer new_node();
extern NodePointer NodeListAppend();
extern NodePointer SelectiveNodeListAppend();
extern NodePointer CreateNewNode();
extern ListPointer CopyListInDepth();
extern char *UniqueName();

NodePointer
CreateNewNode(Graph, Name, Class, Master)
GraphPointer Graph;
char *Name;
char *Class;
GraphPointer Master;
{
    NodePointer Node;

    Node = new_node();
    Node->Name = Name;
    Node->Class = Class;
    Node->Master = Master;
    Graph->NodeList = NodeListAppend(Node, Graph->NodeList);
    return (Node);
}

NodePointer
CopyNode(Graph, Node, NewName)
GraphPointer Graph;
NodePointer Node;
int NewName;
{
    NodePointer NewNode;

    NewNode = CreateNewNode(Graph, 
        (NewName) ? UniqueName(Node->Name) : Node->Name,
	Node->Class, Node->Master);
    NewNode->Arguments = CopyListInDepth(Node->Arguments);
    NewNode->Attributes = CopyListInDepth(Node->Attributes);
    return (NewNode);
}

NodePointer
NodeListAppend(Node, NodeList)
NodePointer Node;
NodePointer NodeList;
{
    NodePointer Ptr;

    if (NodeList == NULL)
	return (Node);

    for (Ptr = NodeList; Ptr->Next != NULL; Ptr = Ptr->Next);
    Ptr->Next = Node;
    return (NodeList);
}

NodePointer
SelectiveNodeListAppend(Node, NodeList)
NodePointer Node;
NodePointer NodeList;
{
    NodePointer Ptr;

    if (NodeList == NULL)
	return (Node);

    for (Ptr = NodeList; Ptr != NULL; Ptr = Ptr->Next)
	if (Ptr == Node)
	    return (NodeList);

    for (Ptr = NodeList; Ptr->Next != NULL; Ptr = Ptr->Next);
    Ptr->Next = Node;
    Node->Next = NULL;
    return (NodeList);
}

NodePointer
NodeAppend(Node, NodeList)
NodePointer Node;
NodePointer NodeList;
{
    NodePointer Ptr;

    if (Node != NULL)
	Node->Next = NULL;

    if (NodeList == NULL)
	return (Node);

    for (Ptr = NodeList; Ptr->Next != NULL; Ptr = Ptr->Next);
    Ptr->Next = Node;
    return (NodeList);
}

NodePointer
RemoveFromNodeList(Node, NodeList)
NodePointer Node;
NodePointer NodeList;
{
    NodePointer Ptr, Last;

    for (Ptr = NodeList, Last = NULL; Ptr != NULL; Last = Ptr, Ptr = Ptr->Next)
	if (Ptr == Node)
	    break;
    if (Ptr == NULL)
	return (NodeList);
    else if (Last == NULL)
	return (Ptr->Next);
    Last->Next = Node->Next;
    return (NodeList);
}

ClearNode(Node)
NodePointer Node;
{
    ClearList(Node->InEdges);
    ClearList(Node->OutEdges);
    ClearList(Node->InControl);
    ClearList(Node->OutControl);
    ClearList(Node->Arguments);
    ClearList(Node->Attributes);
    free_node(Node);
}

EliminateNode(Graph, Node)
GraphPointer Graph;
NodePointer Node;
{
    DisconnectNode(Node, Graph, FALSE);
    Graph->NodeList = RemoveFromNodeList(Node, Graph->NodeList);
    ClearNode(Node);
}

DisconnectNode(Node, Graph, RootConnect)
NodePointer Node;
GraphPointer Graph;
{
    /* 
     * Replace all references to Node in the In and OutEdges of Node by
     * references to the parent graph. (if RootConnect)
     * else just leave them unconnected
     */

    EdgePointer InEdge, OutEdge;
    ListPointer Ptr, Next;

    for (Ptr = Node->InEdges; Ptr; Ptr = Next) {
	Next = Ptr->Next;
	if (Ptr->Type != EdgeNode)
	    continue;
        InEdge = (EdgePointer) Ptr->Entry; 
        DisconnectEdgeFromNode(InEdge, Node, NodeNode);
	if (RootConnect)
            ConnectEdgeToNode(InEdge, Graph, GraphNode);
    }

    for (Ptr = Node->InControl; Ptr; Ptr = Next) {
	Next = Ptr->Next;
	if (Ptr->Type != EdgeNode)
	    continue;
        InEdge = (EdgePointer) Ptr->Entry; 
        DisconnectEdgeFromNode(InEdge, Node, NodeNode);
	if (RootConnect)
            ConnectEdgeToNode(InEdge, Graph, GraphNode);
    }

    for (Ptr = Node->OutEdges; Ptr; Ptr = Next) {
	Next = Ptr->Next;
	if (Ptr->Type != EdgeNode)
	    continue;
        OutEdge = (EdgePointer) Ptr->Entry; 
	DisconnectNodeFromEdge(Node, NodeNode, OutEdge);
	if (RootConnect)
            ConnectNodeToEdge(Graph, GraphNode, OutEdge);
    }

    for (Ptr = Node->OutControl; Ptr; Ptr = Next) {
	Next = Ptr->Next;
	if (Ptr->Type != EdgeNode)
	    continue;
        OutEdge = (EdgePointer) Ptr->Entry; 
	DisconnectNodeFromEdge(Node, NodeNode, OutEdge);
	if (RootConnect)
            ConnectNodeToEdge(Graph, GraphNode, OutEdge);
    }
}

EdgePointer
NthNodeEdge(Node, N, Direction)
NodePointer Node;
int N;
int Direction;
{
    ListPointer Ptr;
    int i;

    for (Ptr = (Direction == IN) ? Node->InEdges : Node->OutEdges, i = 0;
        Ptr != NULL && i != N; i++, Ptr = Ptr->Next);
    return ((Ptr != NULL) ? (EdgePointer) Ptr->Entry : NULL);
}

int
IsNodeInGraph(Node, Graph)
NodePointer Node;
GraphPointer Graph;
{
    NodePointer N;

    for (N = Graph->NodeList; N != NULL; N = N->Next)
	if (N == Node)
	    return (TRUE);
    return (FALSE);
}

PrintNodeList(l)
NodePointer l;
{
    NodePointer ptr;

    for (ptr = l; ptr != NULL; ptr = ptr->Next) {
	if (ptr->Name != NULL)
	    printf("Node: %s", ptr->Name);
	else
	    printf("Node: NULL");
        printf(" -> ");
    }
    printf("\n");
}
