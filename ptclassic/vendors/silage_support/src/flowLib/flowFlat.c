/* $Id$ */

/* Copyright (C) 1990 Regents of the University of California
 * All rights reserved.
 */

/*
 * Routines to flatten a flowgraph (expanding "func"s).
 */

#include "flowgraph.h"

extern NodePointer NodeAppend(), NodeListAppend();
extern EdgePointer EdgeListAppend(), RemoveFromEdgeList(), GetNthGraphEdge();
extern ListPointer new_list(), ListAppend();
extern GraphPointer CopyGraph();
extern char *Intern();
extern char *UniqueEdgeName(), *UniqueNodeName(), *UniqueControlName();

static NodePointer NodeList;

FlattenGraph(Graph, FullFlat)
GraphPointer Graph;
int FullFlat;
{
    NodePointer Node, Next;
    GraphPointer SubGraph;
    char *Function, *GetFunction();

    /* Flatten the subgraphs */
    if (FullFlat) {
        for (Node = Graph->NodeList; Node != NULL; Node = Node->Next)
	    if (IsHierarchyNode(Node))
                FlattenGraph(Node->Master, FullFlat);
    }

    /* Expand the functions */
    NodeList = NULL;
    for (Node = Graph->NodeList; Node != NULL; Node = Next) {
	Next = Node->Next;
	Function = GetFunction(Node);
	if (Function == NULL || strcmp(Function, "func")) {
	    NodeList = NodeAppend(Node, NodeList);
	    continue;
	}

	SubGraph = CopyGraph(Node->Master);
	FlattenNode(Graph, Node, SubGraph);

	NodeList = NodeListAppend(SubGraph->NodeList, NodeList);
	Graph->EdgeList = EdgeListAppend(SubGraph->EdgeList, Graph->EdgeList);
	Graph->ControlList = EdgeListAppend(SubGraph->ControlList, 
			     Graph->ControlList);

	free_graph(SubGraph);
	ClearNode(Node);
    }

    Graph->NodeList = NodeList;
}

FlattenNode(Graph, Node, SubGraph)
GraphPointer Graph;
NodePointer  Node;
GraphPointer SubGraph;
{
    ListPointer L, LL, K, KK, Ptr, Next, NNext;
    EdgePointer E, EE;
    NodePointer N;
    char NewName[128];
    int Count;

    /* Relabel internal nodes and edges (to insure unique names) */
    for (N = SubGraph->NodeList; N != NULL; N = N->Next)
	N->Name = UniqueNodeName(N, NodeList, Node->Name);

    for (E = SubGraph->EdgeList; E != NULL; E = E->Next)
	E->Name = UniqueEdgeName(E, Graph->EdgeList, Node->Name);

    for (E = SubGraph->ControlList; E != NULL; E = E->Next) 
	E->Name = UniqueControlName(E, Graph->ControlList, Node->Name);

    /* Disconnect the arcs and flatten */

    /* Input edges */
    for (K = Node->InEdges, KK = SubGraph->InEdges; K != NULL;
	 K = Next, KK = NNext) {
	Next = K->Next;
	NNext = KK->Next;
	E = (EdgePointer) K->Entry;
	EE = (EdgePointer) KK->Entry;
	
	/* Remove Node from OutNodes of E */
	DisconnectEdgeFromNode(E, Node, NodeNode);

	/* Remove SubGraph from InEdges of EE */
	DisconnectNodeFromEdge(SubGraph, GraphNode, EE);

	/* Connect Fanout of EE to E */
	ReplaceEdgeInFanout(EE, E);

	/* remove edge from edgelist */
	SubGraph->EdgeList = RemoveFromEdgeList(EE, SubGraph->EdgeList);

	ClearEdge(EE);
    }

    /* Output Edges */
    for (K = Node->OutEdges, KK = SubGraph->OutEdges; K != NULL;
	 K = Next, KK = NNext) {
	Next = K->Next;
	NNext = KK->Next;

	E = (EdgePointer) K->Entry;
	EE = (EdgePointer) KK->Entry;

	if ((K->Type == NullNode) || (E == NULL)) {
	    /* Some output edges of the sub-graph might not be connected
	       at the next hierarchy level -- jr 5/11/92 */
	    DisconnectEdgeFromNode(EE, SubGraph, GraphNode);
	    continue;
	}
	
	/* Remove Node from InNodes of E */
	DisconnectNodeFromEdge(Node, NodeNode, E);

	/* Remove SubGraph from OutEdges of EE */
	DisconnectEdgeFromNode(EE, SubGraph, GraphNode);

	/* Connect Fanin of EE to E */
	while ((Ptr = EE->InNodes) != NULL) {
	    pointer EntrySave = Ptr->Entry;
	    EntryType TypeSave = Ptr->Type;

		/* can't rely on Ptr->Entry and Ptr->Type being valid
		 * upon exit of DisconnectNodeFromEdge since DNFE calls
		 * free_list (which indirectly frees Ptr)
		 *					[erc/6-22-92]
		 */

	    DisconnectNodeFromEdge(Ptr->Entry, Ptr->Type, EE);
	    ConnectNodeToEdge(EntrySave, TypeSave, E);
	}

	/* Connect Fanout of EE to E */
	ReplaceEdgeInFanout(EE, E);

	/* remove edge from edgelist */
	if (IsMemberOfEdgeList(EE, SubGraph->EdgeList))
	    SubGraph->EdgeList = RemoveFromEdgeList(EE, SubGraph->EdgeList);
        else
	    Graph->EdgeList = RemoveFromEdgeList(EE, Graph->EdgeList);

	ClearEdge(EE);
    }

    /* Input Control Edges */
    for (Count = 0, K = Node->InControl; K != NULL; K = Next, Count++) {
	Next = K->Next;
	E = (EdgePointer) K->Entry;

	/* Try to find control edge in sub-graph with the same name */
	EE = GetNthGraphEdge(SubGraph, Count, IN, "control");
	
	/* Remove Node from OutNodes of E */
	DisconnectEdgeFromNode(E, Node, NodeNode);

	/* Remove SubGraph from InEdges of EE */
	if (EE != NULL) {
	    /* DisconnectNodeFromEdge(SubGraph, GraphNode, EE); */

	    ReplaceEdgeInFanout(EE, E);
	}
	else { /* Non-directed precedence - Add to ALL nodes in the subgraph */
	    for (N = SubGraph->NodeList; N != NULL; N = N->Next)
		ConnectEdgeToNode(E, N, NodeNode);
	}
    }

    /* Eliminate all remaining input control edges in subgraph  */
    for (KK = SubGraph->InControl; KK != NULL; KK = Next) {
	Next = KK->Next;
	EE = (EdgePointer) KK->Entry;
	EliminateEdge(SubGraph, EE);
    }

    /* Output Control Edges */
    for (Count = 0, K = Node->OutControl; K != NULL; K = Next, Count++) {
	Next = K->Next;
	E = (EdgePointer) K->Entry;

	/* Try to find control edge in sub-graph with the same name */
	EE = GetNthGraphEdge(SubGraph, Count, OUT, "control");

	if (E == NULL) {
	    /* Some output edges of the sub-graph might not be connected
	       at the next hierarchy level -- jr 5/11/92 */
	    DisconnectEdgeFromNode(EE, SubGraph, GraphNode); 
	    continue;
	}
	
	/* Remove Node from InNodes of E */
	DisconnectNodeFromEdge(Node, NodeNode, E);

	/* Remove SubGraph from OutNodes of EE */
	if (EE != NULL) {
	    /* DisconnectEdgeFromNode(EE, SubGraph, GraphNode); */

	    /* Connect Fanin of EE to E */
	    for (Ptr = EE->InNodes; Ptr != NULL; Ptr = NNext) {
	        NNext = Ptr->Next;
		ReplaceOutputEdge(Ptr->Entry, Ptr->Type, EE, E);
	    }

	    for (Ptr = EE->OutNodes; Ptr != NULL; Ptr = NNext) {
	        NNext = Ptr->Next;
		if (Ptr->Type != GraphNode)
	            ReplaceInputEdge(Ptr->Entry, Ptr->Type, EE, E);
	    }
	}

	else { /* Non-directed precedence - Add to ALL nodes in the subgraph */
	    for (N = SubGraph->NodeList; N != NULL; N = N->Next)
		ConnectNodeToEdge(N, NodeNode, E);
	}
    }

    /* Eliminate all remaining outpu control edges in subgraph  */
    for (KK = SubGraph->OutControl; KK != NULL; KK = Next) {
	Next = KK->Next;
	EE = (EdgePointer) KK->Entry;
	EliminateEdge(SubGraph, EE);
    }
    

    /* free the Node and SubGraph */
    ClearList(SubGraph->InEdges);
    ClearList(SubGraph->OutEdges);
    ClearList(SubGraph->InControl);
    ClearList(SubGraph->OutControl);
    ClearList(SubGraph->Parents);
}


ReplaceEdgeReference(List, OldEdge, NewEdge)
ListPointer List;
EdgePointer OldEdge, NewEdge;
{
    for (; List != NULL; List = List->Next)
	if (List->Entry == (pointer) OldEdge) {
	    List->Entry = (pointer) NewEdge;
	    return;
        }
}
