/* $Id$ */

/* Copyright (C) 1990 Regents of the University of California
 * All rights reserved.
 */

#include "flowgraph.h"

#define CHUNK 100


static GraphPointer freegraph = NULL;
static ListPointer freelist = NULL;
static NodePointer freenode = NULL;
static EdgePointer freeedge = NULL;

get_free_graph()
/* allocates a set of free space equal to CHUNK structures GRAPH */
{
    register int i;

    freegraph = (GraphPointer) malloc (CHUNK * sizeof (GRAPH));
    if (freegraph == NULL) {
	printmsg("malloc","can't malloc for TERM");
	exit (-1);
    }
    for (i = 0; i < CHUNK-1; i++)
        (freegraph + i)->Next = freegraph + i + 1;
    (freegraph + i)->Next = NULL; 
}    

GraphPointer
new_graph()
{
    register GraphPointer graph;

    if (!freegraph)
	get_free_graph();
    graph = freegraph;
    freegraph = freegraph->Next;
    graph->Name = NULL;
    graph->Class = NULL;
    graph->Arguments = NULL;
    graph->Attributes = NULL;
    graph->Model = NULL;
    graph->InEdges = NULL;
    graph->OutEdges = NULL;
    graph->InControl = graph->OutControl = NULL;
    graph->EdgeList = NULL;
    graph->NodeList = NULL;
    graph->ControlList = NULL;
    graph->Parents = NULL;
    graph->Token = 0;
    graph->Extension = NULL;
    graph->Next = NULL;
    return(graph);
}

free_graph(graph)
register GraphPointer graph;
{
    if (!graph)
	return;
    graph->Next = freegraph;
    freegraph = graph;
}

get_free_list()
/* allocates a set of free space equal to CHUNK structures LIST */
{
    register int i;

    freelist = (ListPointer) malloc (CHUNK * sizeof (LIST));
    if (freelist == NULL) {
	printmsg("malloc", "can't malloc for LIST");
	exit (-1);
    }
    for (i = 0; i < CHUNK-1; i++)
        (freelist + i)->Next = freelist + i + 1;
    (freelist + i)->Next = NULL; 
}    

ListPointer
new_list(Type, Entry)
EntryType Type;
pointer Entry;
{
    register ListPointer list;

    if (!freelist)
	get_free_list();
    list = freelist;
    freelist = freelist->Next;
    list->Label = NULL;
    list->Type = Type;
    list->Entry = Entry;
    list->Next = NULL;
    return(list);
}

free_list(list)
register ListPointer list;
{
    if (!list)
	return;
    list->Next = freelist;
    list->Entry = NULL;
    list->Label = NULL;
    freelist = list;
}

get_free_edge()
/* allocates a set of free space equal to CHUNK structures EDGE */
{
    register int i;

    freeedge = (EdgePointer) malloc (CHUNK * sizeof (EDGE));
    if (freeedge == NULL) {
	printmsg("malloc", "can't malloc for LIST");
	exit (-1);
    }
    for (i = 0; i < CHUNK-1; i++) {
        (freeedge + i)->Next = freeedge + i + 1;
    }
    (freeedge + i)->Next = NULL; 
}    

EdgePointer
new_edge()
{
    register EdgePointer edge;

    if (!freeedge)
	get_free_edge();
    edge = freeedge;
    freeedge = freeedge->Next;
    edge->Name = NULL;
    edge->Class = NULL;
    edge->InNodes = edge->OutNodes = NULL;
    edge->Arguments = NULL;
    edge->Attributes = NULL;
    edge->Token = 0;
    edge->Extension = NULL;
    edge->Next = NULL;
    return(edge);
}

free_edge(edge)
register EdgePointer edge;
{
    if (!edge)
	return;
    edge->Next = freeedge;
    freeedge = edge;
}

get_free_node()
/* allocates a set of free space equal to CHUNK structures NODE */
{
    register int i;

    freenode = (NodePointer) malloc (CHUNK * sizeof (NODE));
    if (freenode == NULL) {
	printmsg("malloc", "can't malloc for LIST");
	exit (-1);
    }
    for (i = 0; i < CHUNK-1; i++) {
        (freenode + i)->Next = freenode + i + 1;
    }
    (freenode + i)->Next = NULL; 
}    

NodePointer
new_node()
{
    register NodePointer node;

    if (!freenode)
	get_free_node();
    node = freenode;
    freenode = freenode->Next;
    node->Name = NULL;
    node->Class = NULL;
    node->Master = NULL;
    node->Arguments = NULL;
    node->Attributes = NULL;
    node->InEdges = node->OutEdges = NULL;
    node->InControl = node->OutControl = NULL;
    node->Extension = NULL;
    node->Next = NULL;
    node->Token = 0;
    return(node);
}

free_node(node)
register NodePointer node;
{
    if (!node)
	return;
    node->Next = freenode;
    freenode = node;
}
