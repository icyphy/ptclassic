/* $Id$ */

/* Copyright (C) 1990 Regents of the University of California
 * All rights reserved.
 */

/* Support Routines for Graph structures */

#include "flowgraph.h"

ListPointer new_list(), ListAppend();

GraphPointer
GraphListAppend(Graph, GraphList)
GraphPointer Graph;
GraphPointer GraphList;
{
    GraphPointer Ptr;

    if (GraphList == NULL)
	return (Graph);

    for (Ptr = GraphList; Ptr->Next != NULL; Ptr = Ptr->Next);
    Ptr->Next = Graph;
    return (GraphList);
}

DisconnectNodeFromEdge(Node, NodeType, Edge)
NodePointer Node;
EntryType NodeType;
EdgePointer Edge;
{
    register ListPointer L, LL;
    GraphPointer Graph;
    int IsControl;

    if (!Edge) return;    /* Edge might be NULL -- sh 9/28/92 */
    IsControl = IsControlEdge(Edge);

    for (L = Edge->InNodes , LL = NULL; L != NULL; LL = L, L = L->Next)
	if ((pointer) Node == L->Entry)
	    break;
    if (LL == NULL)
	Edge->InNodes = L->Next;
    else
	LL->Next = L->Next;
    free_list(L);

    if (NodeType == GraphNode) {
	Graph = (GraphPointer) Node;
        for (L = (IsControl) ? Graph->InControl : Graph->InEdges , LL = NULL;
	     L != NULL; LL = L, L = L->Next)
	    if ((pointer) Edge == L->Entry)
	        break;
        if (LL == NULL) {
	    if (IsControl)
	        Graph->InControl = L->Next;
	    else
	        Graph->InEdges = L->Next;
	}
        else
	    LL->Next = L->Next;
        free_list(L);
    }
    else {
        for (L = (IsControl) ? Node->OutControl : Node->OutEdges , LL = NULL;
	     L != NULL; LL = L, L = L->Next)
	    if ((pointer) Edge == L->Entry)
	        break;
        if (LL == NULL) { 
	    if (IsControl)
	        Node->OutControl = L->Next;
	    else
	        Node->OutEdges = L->Next;
	}
        else
	    LL->Next = L->Next;
        free_list(L);
    }
}

ConnectNodeToEdge(Node, NodeType, Edge)
NodePointer Node;
EntryType NodeType;
EdgePointer Edge;
{
    register ListPointer L;
    GraphPointer Graph;
    int IsControl;

    if (Edge == NULL) {
	L = new_list(NullNode, (pointer) NULL);
	if (NodeType == GraphNode) {
	    Graph = (GraphPointer) Node;
	    Graph->InEdges = ListAppend(L, Graph->InEdges);
        } else
	    Node->OutEdges = ListAppend(L, Node->OutEdges);
	return;
    }

    IsControl = IsControlEdge(Edge);

    for (L = Edge->InNodes; L != NULL; L = L->Next)
	if ((pointer) Node == L->Entry)
	    break;

    if (L == NULL) {
        L = new_list(NodeType, (pointer) Node);
	Edge->InNodes = ListAppend(L, Edge->InNodes);
    }


    if (NodeType == GraphNode) {
	Graph = (GraphPointer) Node;
	for (L = (IsControl) ? Graph->InControl : Graph->InEdges;
	     L != NULL; L = L->Next)
	    if ((pointer) Edge == L->Entry)
	        break;
        if (L == NULL) {
            L = new_list(EdgeNode, (pointer) Edge);
	    if (IsControl)
	        Graph->InControl = ListAppend(L, Graph->InControl);
	    else
	        Graph->InEdges = ListAppend(L, Graph->InEdges);
        }
    }
    else {
        for (L = (IsControl) ? Node->OutControl : Node->OutEdges;
	     L != NULL; L = L->Next)
	    if ((pointer) Edge == L->Entry)
	        break;
        if (L == NULL) {
            L = new_list(EdgeNode, (pointer) Edge);
	    if (IsControl)
	        Node->OutControl = ListAppend(L, Node->OutControl);
	    else
	        Node->OutEdges = ListAppend(L, Node->OutEdges);
        }
    }
}

DisconnectEdgeFromNode(Edge, Node, NodeType)
NodePointer Node;
EntryType NodeType;
EdgePointer Edge;
{
    register ListPointer L, LL;
    GraphPointer Graph;
    int IsControl;

    if (!Edge) return;    /* Edge might be NULL -- sh 9/28/92 */
    IsControl = IsControlEdge(Edge);

    for (L = Edge->OutNodes , LL = NULL; L != NULL; LL = L, L = L->Next)
	if ((pointer) Node == L->Entry)
	    break;
    if (LL == NULL)
	Edge->OutNodes = L->Next;
    else
	LL->Next = L->Next;
    free_list(L);

    if (NodeType == GraphNode) {
	Graph = (GraphPointer) Node;
        for (L = (IsControl) ? Graph->OutControl : Graph->OutEdges , LL = NULL;
	     L != NULL; LL = L, L = L->Next)
	    if ((pointer) Edge == L->Entry)
	        break;
        if (LL == NULL) {
	    if (IsControl)
	        Graph->OutControl = L->Next;
	    else
	        Graph->OutEdges = L->Next;
	}
        else if (L)
	    LL->Next = L->Next;
        if (L) free_list(L);
	else printmsg("DisconnectEdgeFromNode","G %s & E %s connection error.\n"
		      ,Graph->Name, Edge->Name);
    }
    else {
        for (L = (IsControl) ? Node->InControl : Node->InEdges , LL = NULL;
	     L != NULL; LL = L, L = L->Next)
	    if ((pointer) Edge == L->Entry)
	        break;
        if (LL == NULL) {
	    if (IsControl)
	        Node->InControl = L->Next;
	    else
	        Node->InEdges = L->Next;
	}
        else if (L) 
	    LL->Next = L->Next;
        if (L) free_list(L);
	else printmsg("DisconnectEdgeFromNode","N %s & E %s connection error.\n"
		      ,Node->Name, Edge->Name);
    }

}

ConnectEdgeToNode(Edge, Node, NodeType)
NodePointer Node;
EntryType NodeType;
EdgePointer Edge;
{
    register ListPointer L;
    GraphPointer Graph;
    int IsControl;

    if (Edge == NULL) {
	L = new_list(NullNode, (pointer) NULL);
	if (NodeType == GraphNode) {
	    Graph = (GraphPointer) Node;
	    Graph->OutEdges = ListAppend(L, Graph->OutEdges);
        } else
	    Node->InEdges = ListAppend(L, Node->InEdges);
	return;
    }

    IsControl = IsControlEdge(Edge);

    for (L = Edge->OutNodes; L != NULL; L = L->Next)
	if ((pointer) Node == L->Entry)
	    break;

    if (L == NULL) {
        L = new_list(NodeType, (pointer) Node);
	Edge->OutNodes = ListAppend(L, Edge->OutNodes);
    }

    if (NodeType == GraphNode) {
	Graph = (GraphPointer) Node;
	for (L = (IsControl)  ? Graph->OutControl : Graph->OutEdges;
	     L != NULL; L = L->Next)
	    if ((pointer) Edge == L->Entry)
	        break;
        if (L == NULL) {
            L = new_list(EdgeNode, (pointer) Edge);
	    if (IsControl)
	        Graph->OutControl = ListAppend(L, Graph->OutControl);
	    else
	        Graph->OutEdges = ListAppend(L, Graph->OutEdges);
        }
    }
    else {
        for (L = (IsControl) ? Node->InControl : Node->InEdges;
	     L != NULL; L = L->Next)
	    if ((pointer) Edge == L->Entry)
	        break;
        if (L == NULL) {
            L = new_list(EdgeNode, (pointer) Edge);
	    if (IsControl)
	        Node->InControl = ListAppend(L, Node->InControl);
	    else
	        Node->InEdges = ListAppend(L, Node->InEdges);
        }
    }
}

int
NodeEdgeNumber(Node, Edge, Pointer, Direction)
NodePointer Node;
EdgePointer Edge;
ListPointer Pointer;
int Direction;
{
    /*
     * This is a rather tricky routine : Figures out what input of
     * a node we really are talking about. This is particurly important if
     * one edge connects to multiple input-ports of a node
     */
    int i, j;
    ListPointer Ptr, List;

    /* First count the number of occurrences of Node in the fanin/out of
     * Edge
     */
    i = 0;
    for (Ptr = (Direction == IN) ? Edge->OutNodes : Edge->InNodes;
	Ptr != NULL && Ptr != Pointer; Ptr = Ptr->Next)
	if (Ptr->Type == NodeNode && Ptr->Entry == (pointer) Node)
	    i++;

    /* Find the ith occurrence of Edge in the fanin/out list of Node */

    if (IsControlEdge(Edge))
	List = (Direction == IN) ? Node->InControl : Node->OutControl;
    else
	List = (Direction == IN) ? Node->InEdges : Node->OutEdges;

    for (Ptr = List, j = 0; Ptr != NULL; Ptr = Ptr->Next, j++) 
	if (Ptr->Type == EdgeNode && Ptr->Entry == (pointer) Edge) 
	    if (i-- <= 0)
		return (j);
    return (-1);
}

EdgePointer
GetNthEdge(Node, Direction, N, Class)
NodePointer Node;
int Direction;
int N;
char *Class;
{
    ListPointer Ptr, List;
    int Count;

    if (IsControlClass(Class))
	List = (Direction == IN) ? Node->InControl : Node->OutControl;
    else
	List = (Direction == IN) ? Node->InEdges : Node->OutEdges;

    for (Count = 0, Ptr = List; Ptr != NULL; Count++, Ptr = Ptr->Next) {
	if (Count == N)
	    return ((Ptr->Type == EdgeNode) ? (EdgePointer) Ptr->Entry : NULL);
    }
    return (NULL);
}

SetNthEdge(Node, Edge, Direction, N)
NodePointer Node;
EdgePointer Edge;
int Direction;
int N;
{
    int Count;
    ListPointer Ptr, List;

    if (IsControlEdge(Edge))
	List = (Direction == IN) ? Node->InControl : Node->OutControl;
    else
	List = (Direction == IN) ? Node->InEdges : Node->OutEdges;

    for (Count = 0, Ptr = List; Count < N && Ptr != NULL;
	 Count++, Ptr = Ptr->Next);

    if (Count == N && Ptr != NULL) {
        Ptr->Entry = (pointer) Edge;
	Ptr->Type = EdgeNode;
    }
    else {
        for (; Count < N; Count++)
	    List = ListAppend(new_list(NullNode, NULL), List);
	List = ListAppend(new_list(EdgeNode, Edge), List);

	if (IsControlEdge(Edge)) {
	    if (Direction == IN)
		Node->InControl = List;
	    else
		Node->OutControl = List;
	}
	else {
	    if (Direction == IN)
		Node->InEdges = List;
	    else
		Node->OutEdges = List;
	}
    }

    if (Direction == IN)
        Edge->OutNodes = ListAppend(new_list(NodeNode, Node), Edge->OutNodes);
    else
	Edge->InNodes = ListAppend(new_list(NodeNode, Node), Edge->InNodes);
}

int
GraphEdgeNumber(Graph, Edge, Pointer, Direction)
GraphPointer Graph;
EdgePointer Edge;
ListPointer Pointer;
int Direction;
{
    /*
     * This is a rather tricky routine : Figures out what input of
     * a node we really are talking about. This is particurly important if
     * one edge connects to multiple input-ports of a node
     */
    int i, j;
    ListPointer Ptr, List;

    /* First count the number of occurrences of Node in the fanin/out of
     * Edge
     */
    i = 0;

    for (Ptr = (Direction == OUT) ? Edge->OutNodes : Edge->InNodes;
	Ptr != NULL && Ptr != Pointer; Ptr = Ptr->Next)
	if (Ptr->Type == GraphNode && Ptr->Entry == (pointer) Graph)
	    i++;

    /* Find the ith occurrence of Edge in the fanin/out list of Node */
    if (IsControlEdge(Edge))
	List = (Direction == IN) ? Graph->InControl : Graph->OutControl;
    else
	List = (Direction == IN) ? Graph->InEdges : Graph->OutEdges;

    for (Ptr = List, j = 0; Ptr != NULL; Ptr = Ptr->Next, j++) 
	if (Ptr->Type == EdgeNode && Ptr->Entry == (pointer) Edge) 
	    if (i-- <= 0)
		return (j);
    return (-1);
}

EdgePointer
GetNthGraphEdge(Graph, N, Direction, Class)
GraphPointer Graph;
int N;
int Direction;
char *Class;
{
    ListPointer Ptr, List;
    int i;

    if (IsControlClass(Class))
	List = (Direction == IN) ? Graph->InControl : Graph->OutControl;
    else
	List = (Direction == IN) ? Graph->InEdges : Graph->OutEdges;

    for (Ptr = List, i = 0; Ptr != NULL && i != N; i++, Ptr = Ptr->Next);
    return ((Ptr != NULL) ? (EdgePointer) Ptr->Entry : NULL);
}

SetNthGraphEdge(Graph, Edge, Direction, N)
GraphPointer Graph;
EdgePointer Edge;
int Direction;
int N;
{
    int Count;
    ListPointer Ptr, List;

    if (IsControlEdge(Edge))
	List = (Direction == IN) ? Graph->InControl : Graph->OutControl;
    else
	List = (Direction == IN) ? Graph->InEdges : Graph->OutEdges;

    for (Count = 0, Ptr = List; Count < N && Ptr != NULL;
	 Count++, Ptr = Ptr->Next);

    if (Count == N && Ptr != NULL) {
        Ptr->Entry = (pointer) Edge;
	Ptr->Type = EdgeNode;
    }
    else {
        for (; Count < N; Count++)
	    List = ListAppend(new_list(NullNode, NULL), List);
	List = ListAppend(new_list(EdgeNode, Edge), List);

	if (IsControlEdge(Edge)) {
	    if (Direction == IN)
		Graph->InControl = List;
	    else
		Graph->OutControl = List;
	}
	else {
	    if (Direction == IN)
		Graph->InEdges = List;
	    else
		Graph->OutEdges = List;
	}
    }

    if (Direction == IN)
        Edge->InNodes = ListAppend(new_list(GraphNode, Graph), Edge->InNodes);
    else
	Edge->OutNodes = ListAppend(new_list(GraphNode, Graph), Edge->OutNodes);
}

int
NodeIONumber(Node, Edge, Direction)
NodePointer Node;
EdgePointer Edge;
int Direction;
{
    int Count;
    ListPointer Ptr, List;

    if (IsControlEdge(Edge)) 
	List = (Direction == IN) ? Node->InControl : Node->OutControl;
    else
	List = (Direction == IN) ? Node->InEdges : Node->OutEdges;

    for (Count = 0, Ptr = List; Ptr != NULL; Count++, Ptr = Ptr->Next) {
	if ((EdgePointer) Ptr->Entry == Edge)
	    return (Count);
    }
    return (-1);
}

int
GraphIONumber(Graph, Edge, Direction)
GraphPointer Graph;
EdgePointer Edge;
int Direction;
{
    int Count;
    ListPointer Ptr, List;

    if (IsControlEdge(Edge)) 
	List = (Direction == IN) ? Graph->InControl : Graph->OutControl;
    else
	List = (Direction == IN) ? Graph->InEdges : Graph->OutEdges;

    for (Count = 0, Ptr = List; Ptr != NULL; Count++, Ptr = Ptr->Next) {
	if ((EdgePointer) Ptr->Entry == Edge)
	    return (Count);
    }
    return (-1);
}

ClearGraph(Graph)
GraphPointer Graph;
{
    NodePointer Node, NodeNext;
    EdgePointer Edge, EdgeNext;

    if (Graph == NULL || Graph->Name == NULL) /* Has already been cleared */
	return;

    if (!strcmp(Graph->Class, LEAF_NODE)) return;     /* Can't be removed */

    Graph->Name = NULL;
    
    /* Clear the SubGraphs */

    for (Node = Graph->NodeList; Node != NULL; Node = Node->Next)
	ClearGraph(Node->Master);

    ClearList(Graph->InEdges);
    ClearList(Graph->OutEdges);
    ClearList(Graph->InControl);
    ClearList(Graph->OutControl);
    ClearList(Graph->Parents);
    ClearList(Graph->Arguments);
    ClearList(Graph->Attributes);
    ClearList(Graph->Model);

    for (Node = Graph->NodeList; Node != NULL; Node = NodeNext) {
	NodeNext = Node->Next;
	ClearNode(Node);
    }
    for (Edge = Graph->EdgeList; Edge != NULL; Edge = EdgeNext) {
	EdgeNext = Edge->Next;
	ClearEdge(Edge);
    }
    for (Edge = Graph->ControlList; Edge != NULL; Edge = EdgeNext) {
	EdgeNext = Edge->Next;
	ClearEdge(Edge);
    }
    free_graph(Graph);
}


int
SetupParentList(Graph)
GraphPointer Graph;
/* set up the "Parents" lists for all subgraphs of *Graph. */
{

    NodePointer node;
    void ConnectHierarchyNode();

    for(node = Graph->NodeList; node; node = node->Next) {
	if (IsHierarchy(node)) { 
	   node->Master->Parents = NULL;
	   ConnectHierarchyNode(node->Master, node, Graph);
	   SetupParentList(node->Master);
        }
    }
}

static void
ConnectHierarchyNode(Graph_Son, Node_Parent, Graph_Parent)
GraphPointer Graph_Son, Graph_Parent;
NodePointer  Node_Parent;
/* connect the "Parent" list of *Graph_Son to *Node_Parent & *Graph_Parent */
{
    ListPointer ListCreate(), ListAppend();

    if (Graph_Son->Parents == NULL)  
        Graph_Son->Parents = ListCreate(NULL, GraphNode,(char *)Graph_Parent);
    Graph_Son->Parents=ListAppend(ListCreate(NULL,NodeNode,(char *)Node_Parent),
                                  Graph_Son->Parents);

}

