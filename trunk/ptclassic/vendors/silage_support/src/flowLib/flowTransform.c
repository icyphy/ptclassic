/* $Id$ */

/* Copyright (C) 1990 Regents of the University of California
 * All rights reserved.
 */


/* Support Routines for flowgraph transformations */

#include "flowgraph.h"

/* Macro Definitions */
#define HasFanout(x) (x->OutEdges != NULL && \
      ((EdgePointer) x->OutEdges->Entry)->OutNodes != NULL)

/* global functions used */
extern ListPointer ListAdd(), ListAppend(), 
		   RemoveEntryFromList(), SetAttribute();
extern NodePointer CopyNode();
extern EdgePointer SpliceEdge(),
		   SplitEdge(), DuplicateEdge(), RemoveFromEdgeList();

Commutativity(Node)
NodePointer Node;
{
    int Count;
    ListPointer Ptr;
    pointer Tmp;

    /* Count the # if input edges */
    for (Count = 0, Ptr = Node->InEdges; Ptr != NULL; Ptr = Ptr->Next, Count++);

    if (Count != 2)
	return;

    Tmp = Node->InEdges->Entry;
    Node->InEdges->Entry = Node->InEdges->Next->Entry;
    Node->InEdges->Next->Entry = Tmp;
}


SwapNodes(Graph, NodeA, NodeB)
GraphPointer Graph;
NodePointer NodeA, NodeB;
{
    /* Move NodeA from the fanin to the fanout of NodeB */

    /* This assumes that NodeA is a one-input one-output node */
    /* NodeB should have only one output as well              */

    EdgePointer OutEdge, InEdge;
    NodePointer NodeC;

    /* Step 1 : Remove Node B from the fanout of Node A */
    OutEdge = (EdgePointer) NodeA->OutEdges->Entry;
    InEdge = (EdgePointer) NodeA->InEdges->Entry;
    ReplaceInputEdge(NodeB, NodeNode, OutEdge, InEdge);
    if (HasFanout(NodeA)) {
	NodeC = CopyNode(Graph, NodeA, TRUE);
    }
    else {
       EliminateEdge(Graph, OutEdge);
       DisconnectNode(NodeA, Graph, FALSE); 
       NodeC = NodeA;
    }

    /* Step 2 : move node to fanout */
    OutEdge = (EdgePointer) NodeB->OutEdges->Entry;
    InEdge = SpliceEdge(Graph, OutEdge, FALSE);
    ConnectEdgeToNode(OutEdge, NodeC, NodeNode);
    ConnectNodeToEdge(NodeC, NodeNode, InEdge);
}

EliminateFromFanin(Graph, NodeA, NodeB)
GraphPointer Graph;
NodePointer NodeA, NodeB;
{
    EdgePointer InEdge, OutEdge;

    /* Step 1 : Remove Node A from the fanin of Node B */
    /*          This boils down to shortcircuiting A   */
    /* Step 2 : Remove NodeA if not needed anymore     */

    OutEdge = (EdgePointer) NodeA->OutEdges->Entry;
    InEdge = (EdgePointer) NodeA->InEdges->Entry;
    ReplaceInputEdge(NodeB, NodeNode, OutEdge, InEdge);
    if (HasFanout(NodeA) == FALSE) {
	EliminateEdge(Graph, OutEdge);
	EliminateNode(Graph, NodeA);
    }
}

ReplaceInputEdge(Node, NodeType, EdgeA, EdgeB)
pointer Node;
EntryType NodeType;
EdgePointer EdgeA;
EdgePointer EdgeB;
{
    /* Replaces input EdgeA by EdgeB */
    /* This has to preserve the ordering of the input edges */

    ListPointer Ptr;

    if (NodeType == NodeNode) 
	Ptr = (IsControlEdge(EdgeA)) ? 
	    ((NodePointer) Node)->InControl : ((NodePointer) Node)->InEdges;
    else 
	Ptr = (IsControlEdge(EdgeA)) ? 
	    ((GraphPointer) Node)->OutControl : ((GraphPointer) Node)->OutEdges;

    for (; Ptr != NULL; Ptr = Ptr->Next) {
	if (Ptr->Entry == (pointer) EdgeA) {
	    if (EdgeB == NULL)
		Ptr->Type = NullNode;
	    Ptr->Entry = (pointer) EdgeB;
	}
    }

    if (EdgeB != NULL)
        EdgeB->OutNodes = ListAppend(new_list(NodeType, Node), EdgeB->OutNodes);
    EdgeA->OutNodes = RemoveEntryFromList(NodeType, Node, EdgeA->OutNodes);
}

ReplaceEdgeInFanout(Edge1, Edge2)
EdgePointer Edge1, Edge2;
{
    ListPointer Ptr, Next;

    for (Ptr = Edge1->OutNodes; Ptr != NULL; Ptr = Next) {
	Next = Ptr->Next;
	ReplaceInputEdge(Ptr->Entry, Ptr->Type, Edge1, Edge2);
    }
}

ReplaceOutputEdge(Node, NodeType, EdgeA, EdgeB)
pointer Node;
EntryType NodeType;
EdgePointer EdgeA;
EdgePointer EdgeB;
{
    /* Replaces output EdgeA by EdgeB */
    /* This has to preserve the ordering of the edges */

    ListPointer Ptr;

    if (NodeType == NodeNode) 
	Ptr = (IsControlEdge(EdgeA)) ? 
	    ((NodePointer) Node)->OutControl : ((NodePointer) Node)->OutEdges;
    else 
	Ptr = (IsControlEdge(EdgeA)) ? 
	    ((GraphPointer) Node)->InControl : ((GraphPointer) Node)->InEdges;

    for (; Ptr != NULL; Ptr = Ptr->Next) {
	if (Ptr->Entry == (pointer) EdgeA) {
	    if (EdgeB == NULL)
		Ptr->Type = NullNode;
	    Ptr->Entry = (pointer) EdgeB;
	}
    }
    if (EdgeB != NULL)
        EdgeB->InNodes = ListAdd(new_list(NodeType, Node), EdgeB->InNodes);

    EdgeA->InNodes = RemoveEntryFromList(NodeType, Node, EdgeA->InNodes);
}

ShortCircuitNode(Graph, Node)
GraphPointer Graph;
NodePointer Node;
{
    /* Effectively eliminates a node from the graph by shortcircuiting its
     * input and output. This only works for a one input - one output node
     */
    EdgePointer InEdge, OutEdge;

    InEdge = (EdgePointer) Node->InEdges->Entry;
    OutEdge = (EdgePointer) Node->OutEdges->Entry; 
    EliminateNode(Graph, Node);
    MergeEdges(Graph, InEdge, OutEdge);
}

EdgePointer
SpliceEdge(Graph, Edge, RootConnect)
GraphPointer Graph;
EdgePointer Edge;
{
    EdgePointer NewEdge;
    ListPointer Ptr;
    char NewName[WORDLENGTH];

    /* Split an edge into two edges */

    sprintf(NewName, "%s#1", Edge->Name);
    NewEdge = DuplicateEdge(Graph, Edge, NewName);

    /* Rearrange the connections */

    while ((Ptr = Edge->OutNodes) != NULL) 
	ReplaceInputEdge(Ptr->Entry, Ptr->Type, Edge, NewEdge);

    if (RootConnect) {
        ConnectEdgeToNode(Edge, Graph, GraphNode);
        ConnectNodeToEdge(Graph, GraphNode, NewEdge);
    }
    return (NewEdge);
}

EdgePointer
SplitEdge(Graph, Edge, RootConnect)
GraphPointer Graph;
EdgePointer Edge;
{
    EdgePointer NewEdge;

    NewEdge = SpliceEdge(Graph, Edge, RootConnect);

    NewEdge->Attributes = SetAttribute("equivalent", EdgeNode,
    		  (pointer) Edge, NewEdge->Attributes);

    return (NewEdge);
}

MergeEdges(Graph, Edge1, Edge2)
GraphPointer Graph;
EdgePointer Edge1, Edge2;
{

    /* Rearrange the connections */
    ListPointer Ptr, CopyList();
    EdgePointer Tmp;
    pointer Entry;
    extern char *Intern();

    /* if Edge2 is an input or output edge - preserve that edge */
    if (IsOutputEdge(Edge2) || IsInputEdge(Edge2)) {
	Tmp = Edge1;
	Edge1 = Edge2;
	Edge2 = Tmp;
    }

    while ((Ptr = Edge2->OutNodes) != NULL) {
	ReplaceInputEdge(Ptr->Entry, Ptr->Type, Edge2, Edge1);
    }

    while ((Ptr = Edge2->InNodes) != NULL) {
        ReplaceOutputEdge(Ptr->Entry, Ptr->Type, Edge2, Edge1);
    }

    /* Copy attributes from 2 to 1 (if not already defined) */
    for (Ptr = Edge2->Attributes; Ptr != NULL; Ptr = Ptr->Next) {
	if (GetAttribute(Edge1->Attributes, Ptr->Label) == NULL) {
	    if (Ptr->Type == ListNode)
		Entry = (pointer)CopyList(Ptr->Entry);
            else
		Entry = Ptr->Entry;
	    Edge1->Attributes = 
	    SetAttribute(Ptr->Label, Ptr->Type, Entry, Edge1->Attributes);
        }
    }
    for (Ptr = Edge2->Arguments; Ptr != NULL; Ptr = Ptr->Next) {
	if (GetAttribute(Edge1->Arguments, Ptr->Label) == NULL) {
	    if (Ptr->Type == ListNode)
		Entry = (pointer)CopyList(Ptr->Entry);
            else
		Entry = Ptr->Entry;
	    Edge1->Arguments = 
	    SetAttribute(Ptr->Label, Ptr->Type, Entry, Edge1->Arguments);
        }
    }

    Graph->EdgeList = RemoveFromEdgeList(Edge2, Graph->EdgeList);
    ClearEdge(Edge2);
}
