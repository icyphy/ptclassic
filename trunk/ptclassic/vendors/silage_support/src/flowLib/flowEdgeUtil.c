/* $Id$ */

/* Copyright (C) 1990 Regents of the University of California
 * All rights reserved.
 */

/* Support Routines for Edge structures */

#include "flowgraph.h"

/* global functions used */

extern EdgePointer new_edge();
extern EdgePointer SelectiveEdgeListAppend(), EdgeListAppend();
extern EdgePointer RemoveFromEdgeList();
extern ListPointer new_list(), ListAppend(), CopyListInDepth();
extern ListPointer SetAttribute();
extern char *UniqueName();

EdgePointer
CreateNewEdge(Graph, Name, Class)
GraphPointer Graph;
char *Name;
char *Class;
{
    EdgePointer Edge;

    Edge = new_edge();
    Edge->Name = Name;
    Edge->Class = Class;
    if (!strcmp(Edge->Class, "control"))
        Graph->ControlList = EdgeListAppend(Edge, Graph->ControlList);
    else
        Graph->EdgeList = EdgeListAppend(Edge, Graph->EdgeList);
    return (Edge);
}

EdgePointer
CopyEdge(Edge, Name)
EdgePointer Edge;
char *Name;
{
    EdgePointer NewEdge;
    char *Intern();

    NewEdge = new_edge();
    NewEdge->Name = Intern(Name);
    NewEdge->Class = Edge->Class;
    NewEdge->Arguments = CopyListInDepth(Edge->Arguments);
    NewEdge->Attributes = CopyListInDepth(Edge->Attributes);
    NewEdge->Token = Edge->Token;

    return (NewEdge);
}

EdgePointer
DuplicateEdge(Graph, Edge, Name)
GraphPointer Graph;
EdgePointer Edge;
char *Name;
{
    EdgePointer NewEdge;

    NewEdge = CopyEdge(Edge,Name);
    if (!strcmp(Edge->Class, "control"))
        Graph->ControlList = SelectiveEdgeListAppend(&NewEdge, 
	    Graph->ControlList);
    else
        Graph->EdgeList = SelectiveEdgeListAppend(&NewEdge, Graph->EdgeList);

    return (NewEdge);
}

EdgePointer
EdgeListAppend(Edge, EdgeList)
EdgePointer Edge;
EdgePointer EdgeList;
{
    EdgePointer Ptr;

    if (EdgeList == NULL)
	return (Edge);

    for (Ptr = EdgeList; Ptr->Next != NULL; Ptr = Ptr->Next);
    Ptr->Next = Edge;
    return (EdgeList);
}
EdgePointer
SelectiveEdgeListAppend(Edge, EdgeList)
EdgePointer *Edge;
EdgePointer EdgeList;
{
    EdgePointer Ptr;

    if (EdgeList == NULL)
	return (*Edge);

    for (Ptr = EdgeList; Ptr->Next != NULL && (*Edge)->Name != Ptr->Name;
	 Ptr = Ptr->Next);

    if (Ptr->Name == (*Edge)->Name) {
	ClearEdge(*Edge);
	*Edge = Ptr;
	return (EdgeList);
    }

    Ptr->Next = *Edge;
    return (EdgeList);
}

AddControlEdge(Graph, Source, Dest, Function, Value)
GraphPointer Graph;
NodePointer Source, Dest;
char *Function;
int Value;
{
    AddControlEdgeDeLuxe(Graph, Source, NodeNode, Dest, NodeNode, 
	Function, Value);
}

AddControlEdgeDeLuxe(Graph, Source, SourceType, Dest, DestType, Function, Value)
GraphPointer Graph;
NodePointer Source, Dest;
EntryType SourceType, DestType;
char *Function;
int Value;
{
    EdgePointer Edge;
    ListPointer Entry;
    GraphPointer Tmp;

    Edge = new_edge();
    Edge->Name = UniqueName("cedge");
    Edge->Class = Intern(CONTROL_EDGE);

    Graph->ControlList = EdgeListAppend(Edge, Graph->ControlList);

    Entry = new_list(SourceType, (pointer) Source);
    Edge->InNodes = ListAppend(Entry, Edge->InNodes);

    Entry = new_list(DestType, (pointer) Dest);
    Edge->OutNodes = ListAppend(Entry, Edge->OutNodes);

    Entry = new_list(EdgeNode, (pointer) Edge);
    if (SourceType == NodeNode)
        Source->OutControl = ListAppend(Entry, Source->OutControl);
    else {
	Tmp = (GraphPointer) Source;
        Tmp->InControl = ListAppend(Entry, Tmp->InControl);
    }

    Entry = new_list(EdgeNode, (pointer) Edge);
    if (DestType == NodeNode)
        Dest->InControl = ListAppend(Entry, Dest->InControl);
    else {
	Tmp = (GraphPointer) Dest;
        Tmp->OutControl = ListAppend(Entry, Tmp->OutControl);
    }

    Edge->Arguments = SetAttribute ("function", CharNode, (pointer) Function,
    			    Edge->Arguments);
    Edge->Arguments = SetAttribute ("value", IntNode, (pointer) Value,
    			    Edge->Arguments);
    Edge->Arguments = SetAttribute ("type", IntNode, 
	(pointer) Intern("num<1,0>"), Edge->Arguments);
}

EdgePointer
RemoveFromEdgeList(Edge, EdgeList)
EdgePointer Edge;
EdgePointer EdgeList;
{
    EdgePointer Ptr, Last;

    for (Ptr = EdgeList, Last = NULL; Ptr != NULL; Last = Ptr, Ptr = Ptr->Next)
	if (Ptr == Edge)
	    break;
    if (Ptr == NULL)
	return (EdgeList);
    else if (Last == NULL)
	return (Ptr->Next);
    Last->Next = Edge->Next;
    return (EdgeList);
}

ClearEdge(Edge)
EdgePointer Edge;
{
    ClearList(Edge->InNodes);
    ClearList(Edge->OutNodes);
    ClearList(Edge->Arguments);
    ClearList(Edge->Attributes);
    free_edge(Edge);
}

EliminateEdge(Graph, Edge)
GraphPointer Graph;
EdgePointer Edge;
{
    DisconnectEdge(Edge);
    if (IsControlEdge(Edge))
        Graph->ControlList = RemoveFromEdgeList(Edge, Graph->ControlList);
    else
        Graph->EdgeList = RemoveFromEdgeList(Edge, Graph->EdgeList);
    ClearEdge(Edge);
}

DisconnectEdge(Edge)
EdgePointer Edge;
{
    NodePointer InNode, OutNode;
    ListPointer Ptr, Next;

    for (Ptr = Edge->InNodes; Ptr; Ptr = Next) {
	Next = Ptr->Next;
	if (Ptr->Type != NodeNode && Ptr->Type != GraphNode)
	    continue;
        InNode = (NodePointer) Ptr->Entry; 
        DisconnectNodeFromEdge(InNode, Ptr->Type, Edge);
    }


    for (Ptr = Edge->OutNodes; Ptr; Ptr = Next) {
	Next = Ptr->Next;
	if (Ptr->Type != NodeNode && Ptr->Type != GraphNode)
	    continue;
        OutNode = (NodePointer) Ptr->Entry; 
	DisconnectEdgeFromNode(Edge, OutNode, Ptr->Type);
    }
}


IsMemberOfEdgeList(Edge, EdgeList)
EdgePointer Edge;
EdgePointer EdgeList;
{
    EdgePointer ptr;

    for (ptr = EdgeList; ptr != NULL; ptr = ptr->Next)
	if (ptr == Edge) return (TRUE);
    return FALSE;
}


PrintEdgeList(l)
EdgePointer l;
{
    EdgePointer ptr;

    for (ptr = l; ptr != NULL; ptr = ptr->Next) {
	if (ptr->Name != NULL)
	    printf("Edge: %s", ptr->Name);
	else
	    printf("Edge: NULL");
        printf(" -> ");
    }
    printf("\n");
}


NodePointer
GetEdgeInNode(Edge)
EdgePointer Edge;
/* Get the fanin node of Edge. */
{
    ListPointer node_l, edge_l;
    NodePointer node;
    EdgePointer edge;
    GraphPointer graph;
    int is_control;
    int index;

    if (!(node_l = Edge->InNodes)) return NULL;
    is_control = IsControlEdge(Edge);
    if (node_l->Type == GraphNode) {
        graph = (GraphPointer)(node_l->Entry);
        node = ParentNode(graph);
	edge_l = (is_control) ? (graph->InControl) : (graph->InEdges);
        index = GetIndexOfListEntry(edge_l, Edge);
	edge_l = (is_control) ? (node->InControl) : (node->InEdges);
        LookupList(edge_l, index, &edge, NULL);
        if (!edge) return NULL;
        return (GetEdgeInNode(edge));
    }
    else {
        if (!(node = (NodePointer)(node_l->Entry))) return NULL;
        if (IsHierarchyNode(node)) {
	    graph = node->Master;
	    edge_l = (is_control) ? (node->OutControl) : (node->OutEdges);
	    index = GetIndexOfListEntry(edge_l, Edge);
	    edge_l = (is_control) ? (graph->OutControl) : (graph->OutEdges);
	    LookupList(edge_l, index, &edge, NULL);
	    if (!edge) return NULL;
	    return (GetEdgeInNode(edge));
        }	
	else return node;
    }
}


NodePointer
GetEdgeInNodeWithLoop(Edge)
EdgePointer Edge;
/* Get the fanin node of Edge. */
{
    ListPointer node_l, edge_l;
    NodePointer node;
    EdgePointer edge;
    GraphPointer graph;
    int is_control;
    int index;

    if (!(node_l = Edge->InNodes)) return NULL;
    is_control = IsControlEdge(Edge);
    if (node_l->Type == GraphNode) {
        graph = (GraphPointer)(node_l->Entry);
	if (IsLoop(graph)) return NULL;
        node = ParentNode(graph);
	edge_l = (is_control) ? (graph->InControl) : (graph->InEdges);
        index = GetIndexOfListEntry(edge_l, Edge);
	edge_l = (is_control) ? (node->InControl) : (node->InEdges);
        LookupList(edge_l, index, &edge, NULL);
        if (!edge) return NULL;
        return (GetEdgeInNodeWithLoop(edge));
    }
    else {
        if (!(node = (NodePointer)(node_l->Entry))) return NULL;
        if (IsHierarchyNode(node) && (!IsLoop(node->Master))) {
	    graph = node->Master;
	    edge_l = (is_control) ? (node->OutControl) : (node->OutEdges);
	    index = GetIndexOfListEntry(edge_l, Edge);
	    edge_l = (is_control) ? (graph->OutControl) : (graph->OutEdges);
	    LookupList(edge_l, index, &edge, NULL);
	    if (!edge) return NULL;
	    return (GetEdgeInNodeWithLoop(edge));
        }	
	else return node;
    }
}

int
NumberOfOutNodes(Edge)
EdgePointer Edge;
{
    int count = 0;
    ListPointer Ptr;

    for (Ptr = Edge->OutNodes; Ptr != NULL; Ptr = Ptr->Next)
	count++;

    return(count);
}
