/* $Id$ */

/*
 * Copyright (C) 1990 Regents of the University of California
 * All rights reserved.
 */

/*
 * DeadCode Elimination
 */
#include "flowgraph.h"

extern ListPointer ListSelectiveAdd(), ListAdd(), CopyList();
extern EdgePointer RemoveFromEdgeList(), GetNthGraphEdge(), GetNthEdge();

DeadCodeElimination(Graph)
GraphPointer Graph;
{
    printmsg(NULL, "dead code elimination ...\n");

    DeadCodeEliminationAux(NULL, Graph);
}

DeadCodeEliminationAux(Parentnode, Graph)
NodePointer  Parentnode;
GraphPointer Graph;
{
    NodePointer Node;
    EdgePointer Edge;
    ListPointer Ptr, DeadEdges, DeadNodes;

    for (Node = Graph->NodeList; Node != NULL; Node = Node->Next) {
	if (IsHierarchyNode(Node)) 
	    DeadCodeEliminationAux(Node, Node->Master);
    }

    /* find all dead edges : edges with no output nodes, 
     * which are not output edges 
     */
    DeadEdges = NULL;

    for (Edge = Graph->EdgeList; Edge != NULL; Edge = Edge->Next) {
	if (Edge->OutNodes == NULL && strcmp(Edge->Class, "break")) {
	    DeadEdges = ListAdd(new_list(EdgeNode, (pointer) Edge), DeadEdges);
        }
    }

    for (Ptr = DeadEdges; Ptr != NULL; Ptr = Ptr->Next) 
	RemoveDeadEdgeTree(Parentnode, Graph, (EdgePointer) Ptr->Entry);

    ClearList(DeadEdges);

    DeadNodes = NULL;

    for (Node = Graph->NodeList; Node != NULL; Node = Node->Next) {
	if (Node->OutEdges == NULL && Node->OutControl == NULL &&
	    !IsOutputNode(Node) && !IsHierarchyNode(Node)) {
	    DeadNodes = ListAdd(new_list(NodeNode, (pointer) Node), DeadNodes);
        }
    }

    for (Ptr = DeadNodes; Ptr != NULL; Ptr = Ptr->Next)
	RemoveDeadNodeTree(Parentnode, Graph, (NodePointer) Ptr->Entry);

    ClearList(DeadNodes);
}

RemoveDeadEdgeTree(Parentnode, Graph, Edge)
NodePointer  Parentnode;
GraphPointer Graph;
EdgePointer  Edge;
{
    ListPointer InputList, Ptr;
    NodePointer Node;

    InputList = CopyList(Edge->InNodes);

    EliminateEdgeThruHierarchy(Parentnode, Graph, Edge);

    for (Ptr = InputList; Ptr != NULL; Ptr = Ptr->Next) {
	if (Ptr->Type != NodeNode)
	    continue;
	Node = (NodePointer) Ptr->Entry;
	if (Node->OutEdges == NULL && Node->OutControl == NULL && 
	    !IsHierarchyNode(Node))
	    RemoveDeadNodeTree(Parentnode, Graph, Node);
    }
    ClearList(InputList);
}

RemoveDeadNodeTree(Parentnode, Graph, Node)
NodePointer Parentnode;
GraphPointer Graph;
NodePointer Node;
{
    ListPointer InputList, Ptr;
    EdgePointer Edge;

    /* create fanin list */
    InputList = NULL;
    for (Ptr = Node->InEdges; Ptr != NULL; Ptr = Ptr->Next) {
	if (Ptr->Type != EdgeNode)
	    continue;
	InputList = ListSelectiveAdd(new_list(EdgeNode, Ptr->Entry), InputList);
    }

    EliminateNode(Graph, Node);

    for (Ptr = InputList; Ptr != NULL; Ptr = Ptr->Next) {
	Edge = (EdgePointer) Ptr->Entry;
	if (Edge->OutNodes == NULL && !strcmp(Edge->Class, "data"))
	    RemoveDeadEdgeTree(Parentnode, Graph, Edge);
    }
    ClearList(InputList);
}

EliminateEdgeThruHierarchy(Parentnode, Graph, Edge)
NodePointer Parentnode;
GraphPointer Graph;
EdgePointer Edge;
{
    DisconnectEdgeThruHierarchy(Parentnode, Graph, Edge);
    if (IsControlEdge(Edge))
        Graph->ControlList = RemoveFromEdgeList(Edge, Graph->ControlList);
    else
        Graph->EdgeList = RemoveFromEdgeList(Edge, Graph->EdgeList);
    ClearEdge(Edge);
}

DisconnectEdgeThruHierarchy(Parentnode, Graph, Edge)
NodePointer Parentnode;
GraphPointer Graph;
EdgePointer Edge;
{
    NodePointer InNode, OutNode;
    EdgePointer EE;
    ListPointer Ptr, Next;

    for (Ptr = Edge->InNodes; Ptr; Ptr = Next) {
	Next = Ptr->Next;
	if (Ptr->Type != NodeNode && Ptr->Type != GraphNode)
	    continue;
        InNode = (NodePointer) Ptr->Entry; 
        if (Ptr->Type == NodeNode) {    /* sean 07/21/92 */
	    if (IsHierarchyNode(InNode)) {
	        EE = GetNthGraphEdge(InNode->Master, 
		     NodeEdgeNumber(InNode, Edge, Ptr, OUT) , OUT, Edge->Class);
	        DisconnectEdgeFromNode(EE, InNode->Master, GraphNode);
	    }
	}
        else {
            EE = GetNthEdge(Parentnode, IN, 
                 GraphEdgeNumber(Graph, Edge, Ptr, IN), Edge->Class);
            DisconnectEdgeFromNode(EE, Parentnode, NodeNode);
        }
        DisconnectNodeFromEdge(InNode, Ptr->Type, Edge);
    }


    for (Ptr = Edge->OutNodes; Ptr; Ptr = Next) {
	Next = Ptr->Next;
	if (Ptr->Type != NodeNode && Ptr->Type != GraphNode)
	    continue;
        OutNode = (NodePointer) Ptr->Entry; 
        if (Ptr->Type == NodeNode) {    /* sean 07/21/92 */
	    if (IsHierarchyNode(OutNode)) {
	        EE = GetNthGraphEdge(InNode->Master, 
		    NodeEdgeNumber(InNode, Edge, Ptr, IN) , IN, Edge->Class);
	        DisconnectNodeFromEdge(OutNode->Master, GraphNode, EE);
	    }
        }
        else {
            EE = GetNthEdge(Parentnode, OUT, 
                 GraphEdgeNumber(Graph, Edge, Ptr, OUT), Edge->Class);
            DisconnectNodeFromEdge(Parentnode, NodeNode, EE);
        }
	DisconnectEdgeFromNode(Edge, OutNode, Ptr->Type);
    }
}
