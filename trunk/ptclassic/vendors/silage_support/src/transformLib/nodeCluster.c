/* $Id$ */

/*
 * Copyright (C) 1990 Regents of the University of California
 * All rights reserved.
 */

/*
 * Routines to cluster nodes into subfunctions. This generates the 
 * block-structure used in the HYPER methodology.
 * Primitive nodes can never be mixed with hierachical nodes.
 */ 

#include "flowgraph.h"

/* external functions */
NodePointer *TopologicalOrdering();
NodePointer RemoveFromNodeList(), CreateNewNode(), NodeAppend();
EdgePointer DuplicateEdge(), RemoveFromEdgeList();
GraphPointer GraphInputWithInit(), GraphInput();
ListPointer ListSelectiveAdd();
char *UniqueName();

/* constants */
#define NOT_CONSIDERED -1

/* local variables */
#define MAXHIERARCHYLEVEL 50
static int classlist[MAXHIERARCHYLEVEL];
static int NrOfClasses = 0;
static GraphPointer equalGraph;

ClusterNodes(Graph)
GraphPointer Graph;
{
    printmsg(NULL, "clustering nodes in hierarchical subgraphs ...\n");

    equalGraph = GraphInput("equal", FALSE);

    ClusterNodesAux(Graph);
}

ClusterNodesAux(Graph)
GraphPointer Graph;
{
    NodePointer Node;
    NodePointer *NodeOrder;
    int NrOfNodes, HasHierarchy;

    /* first go down in the hierachy and perform clustering on the loop
     * and iteration sub-graphs
     */
    
    HasHierarchy = FALSE;

    for (Node = Graph->NodeList; Node != NULL; Node = Node->Next) {
	if (IsHierarchyNode(Node)) {
	    ClusterNodesAux(Node->Master);
	    HasHierarchy = TRUE;
	}
    }
    if (HasHierarchy == FALSE) /* Flat graph - no clustering needed */
	return;

    /* Perform the clustering operation */
    /* 1) Topological ordering
       2) Perform an extremely simple scheduling, merging nodes if
	  they are of a compatible type
       3) Reorganize the graph
     */

    NodeOrder = TopologicalOrdering(Graph, &NrOfNodes);

    DetermineNodeClasses(NodeOrder, NrOfNodes);

    DetermineDelayClasses(Graph);

    CreateNodeClusters(Graph);

    cfree(NodeOrder);
}

DetermineNodeClasses(NodeOrder, NrOfNodes)
NodePointer *NodeOrder;
int NrOfNodes;
{
    int i;
    NodePointer Node;

    NrOfClasses = 0;

    /* Label the nodes */
    for (i = NrOfNodes - 1; i >= 0; i--) {
	Node = NodeOrder[i];
	Node->Token = DetermineNodeClass(Node);
    }
}

int 
DetermineNodeClass(Node)
NodePointer Node;
{
    int MyClass, LoopClass, Maximum;
    NodePointer FanInNode;
    ListPointer Ptr1, Ptr2;
    EdgePointer Edge;

    MyClass = (IsHierarchyNode(Node)) ? TRUE : FALSE;

    /* if the current class is a loop - find the first available class */
    if (MyClass == TRUE) {
        if (NrOfClasses == MAXHIERARCHYLEVEL) {
	    printmsg("DetermineNodeClasses", 
		"Maximum Number of Hierarchy Classes exceeded");
	    printmsg("Increase MAXHIERARCHYLEVEL");
	    exit (-1);
	}
	classlist[NrOfClasses] = TRUE;
	return (NrOfClasses++);
    }

    /* current node is a leaf-node */
    /* Run over the fanin and select the highest number */
    Maximum = -1;
    LoopClass = FALSE;

    for (Ptr1 = Node->InEdges; Ptr1 != NULL; Ptr1 = Ptr1->Next) {
        if (Ptr1->Type != EdgeNode)
            continue;
        Edge = (EdgePointer) Ptr1->Entry;
        for (Ptr2 = Edge->InNodes; Ptr2 != NULL; Ptr2 = Ptr2->Next) {
            if (Ptr2->Type == GraphNode)
                continue;
            FanInNode = (NodePointer) Ptr2->Entry;
	    if (FanInNode->Token == NOT_CONSIDERED)
                continue;
	    if (FanInNode->Token > Maximum) {
	        Maximum = FanInNode->Token;
	        LoopClass = (IsHierarchyNode(FanInNode)) ? TRUE : FALSE;
	    }
	}
    }
    for (Ptr1 = Node->InControl; Ptr1 != NULL; Ptr1 = Ptr1->Next) {
        if (Ptr1->Type != EdgeNode)
            continue;
        Edge = (EdgePointer) Ptr1->Entry;
        for (Ptr2 = Edge->InNodes; Ptr2 != NULL; Ptr2 = Ptr2->Next) {
            if (Ptr2->Type == GraphNode)
                continue;
            FanInNode = (NodePointer) Ptr2->Entry;
            if (FanInNode->Token == NOT_CONSIDERED)
                continue;
	    if (FanInNode->Token > Maximum) {
	        Maximum = FanInNode->Token;
	        LoopClass = (IsHierarchyNode(FanInNode)) ? TRUE : FALSE;
	    }
        }
    }
    return ((Maximum == -1 || LoopClass == TRUE) ?
	/* Find first available dataclass, larger then maximum */
	/* else - Merge Data Classes */
	FindFirstAvailableClass(Maximum + 1) : Maximum);
}

int
FindFirstAvailableClass(Start)
int Start;
{
    int j;

    for (j = Start; j < NrOfClasses; j++)
        if (classlist[j] == FALSE)
            break;
    if (j == NrOfClasses) {
        if (NrOfClasses == MAXHIERARCHYLEVEL) {
            printmsg("DetermineNodeClasses",
                     "Maximum Number of Hierarchy Classes exceeded");
            printmsg("Increase MAXHIERARCHYLEVEL");
            exit (-1);
        }
        classlist[NrOfClasses] = FALSE;
	return(NrOfClasses++);
     }
     else {
        return (j);
     }
}

DetermineDelayClasses(Graph)
GraphPointer Graph;
{
    /* Push local delays down in the hierarchy */
    int Src, Dst, i;
    char *Name;
    NodePointer Node, Source, Dest, TransferNode;
    EdgePointer Edge, InEdge, OutEdge, TempEdge;
    ListPointer Ptr, Ptr2;

    for (Node = Graph->NodeList; Node != NULL; Node = Node->Next) 
	if (IsDelayNode(Node))
	    Node->Token = NOT_CONSIDERED;
    
    for (Node = Graph->NodeList; Node != NULL; Node = Node->Next) {
	if (!IsDelayNode(Node))
	    continue;
	/* Determine the classes of the fanin(out) of the delay nodes */
	Src = -1;
	Dst = NrOfClasses+1;
	for (Ptr = Node->InEdges; Ptr != NULL; Ptr = Ptr->Next) {
	    if (Ptr->Type != EdgeNode)
	        continue;
	    Edge = (EdgePointer) Ptr->Entry;
	    for (Ptr2 = Edge->InNodes; Ptr2 != NULL; Ptr2 = Ptr2->Next) {
		if (Ptr2->Type != NodeNode)
		    continue;
		Source = (NodePointer) Ptr2->Entry;
		if (Source->Token > Src)
		    Src = Source->Token;
	    }
	}
	for (Ptr = Node->InControl; Ptr != NULL; Ptr = Ptr->Next) {
	    if (Ptr->Type != EdgeNode)
	        continue;
	    Edge = (EdgePointer) Ptr->Entry;
	    for (Ptr2 = Edge->InNodes; Ptr2 != NULL; Ptr2 = Ptr2->Next) {
		if (Ptr2->Type != NodeNode)
		    continue;
		Source = (NodePointer) Ptr2->Entry;
		if (Source->Token > Src)
		    Src = Source->Token;
	    }
	}
	for (Ptr = Node->OutEdges; Ptr != NULL; Ptr = Ptr->Next) {
	    if (Ptr->Type != EdgeNode)
	        continue;
	    Edge = (EdgePointer) Ptr->Entry;
	    for (Ptr2 = Edge->OutNodes; Ptr2 != NULL; Ptr2 = Ptr2->Next) {
		if (Ptr2->Type != NodeNode)
		    continue;
		Dest = (NodePointer) Ptr2->Entry;
		if (Dest->Token < Dst)
		    Dst = Dest->Token;
	    }
	}
	for (Ptr = Node->OutControl; Ptr != NULL; Ptr = Ptr->Next) {
	    if (Ptr->Type != EdgeNode)
	        continue;
	    Edge = (EdgePointer) Ptr->Entry;
	    for (Ptr2 = Edge->OutNodes; Ptr2 != NULL; Ptr2 = Ptr2->Next) {
		if (Ptr2->Type != NodeNode)
		    continue;
		Dest = (NodePointer) Ptr2->Entry;
		if (Dest->Token < Dst)
		    Dst = Dest->Token;
	    }
	}
	classlist[NrOfClasses] = FALSE;
	if (Src == Dst && classlist[Src] != TRUE && Src != NOT_CONSIDERED) {
	    Node->Token = Src;
	}
	else {
	    /* Delay cannot be removed - it has to stick to this
	       hierarchy level.
	       It is therefore necessary to introduce a transfer operation 
               (when needed)
	     */
	    InEdge = (EdgePointer) Node->InEdges->Entry;
            OutEdge = (EdgePointer) Node->OutEdges->Entry;
	    if (WouldGenerateCycle(InEdge, OutEdge) == TRUE) {
		/* Insert transfer node */
		TransferNode = CreateNewNode(Graph, UniqueName("transfer"),
                     Intern("data"), equalGraph);
		Name = InEdge->Name;
		InEdge->Name = UniqueName("transfer");
                TempEdge = DuplicateEdge(Graph, InEdge, Name);
    		while ((Ptr = InEdge->OutNodes) != NULL) 
		    ReplaceInputEdge(Ptr->Entry, Ptr->Type, InEdge, TempEdge);

		ConnectEdgeToNode(InEdge, TransferNode, NodeNode);
		ConnectNodeToEdge(TransferNode, NodeNode, TempEdge);
		TransferNode->Token = DetermineNodeClass(TransferNode);
		if (TransferNode->Token == Dst)
		    Node->Token = TransferNode->Token;
	    }
	}
    }
}

int
WouldGenerateCycle(InEdge, OutEdge)
EdgePointer InEdge;
EdgePointer OutEdge;
{
    /* This routine checks if the removal of the delay would create a cycle */
 
    ListPointer Ptr;
    NodePointer Source, Dest;

    if (InEdge->InNodes != NULL &&
        InEdge->InNodes->Type == NodeNode &&
        IsDelayNode((NodePointer) InEdge->InNodes->Entry) ||
        (InEdge->InNodes->Type != NodeNode ))
	return (TRUE);
 
    Source = (NodePointer) InEdge->InNodes->Entry;
 
    for (Ptr = OutEdge->OutNodes; Ptr != NULL; Ptr = Ptr->Next) {
        if (Ptr->Type != NodeNode)
            continue;
        Dest = (NodePointer) Ptr->Entry;
        if (IsDelayNode(Dest))
            continue;
 
        if (IsNodeInFanout(Source, Dest))
            return (TRUE);
    }
    return (FALSE);
}

CreateNodeClusters(Graph)
GraphPointer Graph;
{
    /* Merge data nodes of the same class together in a subgraph */

    int i;
    ListPointer Ptr, Ptr2;
    GraphPointer SubGraph;
    NodePointer FuncNode, Node, Source, Dest, Next;
    EdgePointer Edge, NewEdge, NextEdge;
	
    if (NrOfClasses <= 1)
	return;
    
    for (i = 0; i < NrOfClasses; i++) {
	if (classlist[i] == TRUE)   /* loop - is already hierarchical */
	    continue;
	/* Create a new node of class "func" */
	SubGraph = GraphInputWithInit("func", FALSE);

	SubGraph->Name = UniqueName("subgraph");

	FuncNode = CreateNewNode(Graph, SubGraph->Name, Intern("Hierarchy"),
	    SubGraph);
	FuncNode->Token = -1;

	for (Node = Graph->NodeList; Node != NULL; Node = Next) {
	    Next = Node->Next;
	    if (Node->Token != i)
		continue;
	    Graph->NodeList = RemoveFromNodeList(Node, Graph->NodeList);
	    SubGraph->NodeList = NodeAppend(Node, SubGraph->NodeList);

	    for (Ptr = Node->InEdges; Ptr != NULL; Ptr = Ptr->Next) {
		if (Ptr->Type != EdgeNode)
		    continue;
		Edge = (EdgePointer) Ptr->Entry;
		NewEdge = DuplicateEdge(SubGraph, Edge, Edge->Name);
		ReplaceEdgeToNodeConnection(Edge, NewEdge, Node);

		/* check if this edge crosses the boundary */
		for (Ptr2 = Edge->InNodes; Ptr2 != NULL; Ptr2 = Ptr2->Next) {
		    Source = (NodePointer) Ptr2->Entry;
		    if (Ptr2->Type == GraphNode ||
			Source != FuncNode && Source->Token != Node->Token)
			break;
		}
		if (Ptr2 != NULL) {
		    ConnectEdgeToNode(Edge, FuncNode, NodeNode);
		    ConnectNodeToEdge(SubGraph, GraphNode, NewEdge);
		}
	    }

	    for (Ptr = Node->InControl; Ptr != NULL; Ptr = Ptr->Next) {
		if (Ptr->Type != EdgeNode)
		    continue;
		Edge = (EdgePointer) Ptr->Entry;
		NewEdge = DuplicateEdge(SubGraph, Edge, Edge->Name);
		ReplaceEdgeToNodeConnection(Edge, NewEdge, Node);

		/* check if this edge crosses the boundary */
		for (Ptr2 = Edge->InNodes; Ptr2 != NULL; Ptr2 = Ptr2->Next) {
		    Source = (NodePointer) Ptr2->Entry;
		    if (Ptr2->Type == GraphNode ||
			Source != FuncNode && Source->Token != Node->Token)
			break;
		}
		if (Ptr2 != NULL) {
		    ConnectEdgeToNode(Edge, FuncNode, NodeNode);
		    ConnectNodeToEdge(SubGraph, GraphNode, NewEdge);
		}
	    }

	    for (Ptr = Node->OutEdges; Ptr != NULL; Ptr = Ptr->Next) {
		if (Ptr->Type != EdgeNode)
		    continue;
		Edge = (EdgePointer) Ptr->Entry;
		NewEdge = DuplicateEdge(SubGraph, Edge, Edge->Name);
		ReplaceNodeToEdgeConnection(Node, Edge, NewEdge);

		/* check if this edge crosses the boundary */
		for (Ptr2 = Edge->OutNodes; Ptr2 != NULL; Ptr2 = Ptr2->Next) {
		    Dest = (NodePointer) Ptr2->Entry;
		    if (Ptr2->Type == GraphNode ||
			Dest != FuncNode && Dest->Token != Node->Token)
			break;
		}
		if (Ptr2 != NULL) {
		    ConnectEdgeToNode(NewEdge, SubGraph, GraphNode);
		    ConnectNodeToEdge(FuncNode, NodeNode, Edge);
		}
	    }

	    for (Ptr = Node->OutControl; Ptr != NULL; Ptr = Ptr->Next) {
		if (Ptr->Type != EdgeNode)
		    continue;
		Edge = (EdgePointer) Ptr->Entry;
		NewEdge = DuplicateEdge(SubGraph, Edge, Edge->Name);
		ReplaceNodeToEdgeConnection(Node, Edge, NewEdge);

		/* check if this edge crosses the boundary */
		for (Ptr2 = Edge->OutNodes; Ptr2 != NULL; Ptr2 = Ptr2->Next) {
		    Dest = (NodePointer) Ptr2->Entry;
		    if (Ptr2->Type == GraphNode ||
			Dest != FuncNode && Dest->Token != Node->Token)
			break;
		}
		if (Ptr2 != NULL) {
		    ConnectEdgeToNode(NewEdge, SubGraph, GraphNode);
		    ConnectNodeToEdge(FuncNode, NodeNode, Edge);
		}
	    }
	}

    }

    /* Clean up all edges which are obsolete */
    for (Edge = Graph->EdgeList; Edge != NULL; Edge = NextEdge) {
	NextEdge = Edge->Next;
	if (Edge->InNodes == NULL && Edge->OutNodes == NULL) {
	    Graph->EdgeList = RemoveFromEdgeList(Edge, Graph->EdgeList);
	    ClearEdge(Edge);
	}
    }
}

ReplaceEdgeToNodeConnection(Edge, NewEdge, Node)
EdgePointer Edge, NewEdge;
NodePointer Node;
{
    ListPointer Ptr, Last;

    for (Ptr = (IsControlEdge(Edge)) ? Node->InControl : Node->InEdges;
	Ptr != NULL; Ptr = Ptr->Next) {
	if (Ptr->Entry == (pointer) Edge) {
	    Ptr->Entry = (pointer) NewEdge;
	    break;
	}
    }

    for (Last = NULL, Ptr = Edge->OutNodes; Ptr != NULL; 
	 Last = Ptr, Ptr = Ptr->Next) 
	if (Ptr->Entry == (pointer) Node)
	    break;
    if (Last == NULL)
	Edge->OutNodes = Ptr->Next;
    else
	Last->Next = Ptr->Next;


    NewEdge->OutNodes = ListSelectiveAdd(Ptr, NewEdge->OutNodes);
}

ReplaceNodeToEdgeConnection(Node, Edge, NewEdge)
EdgePointer Edge, NewEdge;
NodePointer Node;
{
    ListPointer Ptr, Last;

    for (Ptr = (IsControlEdge(Edge)) ? Node->OutControl : Node->OutEdges;
	Ptr != NULL; Ptr = Ptr->Next) {
	if (Ptr->Entry == (pointer) Edge) {
	    Ptr->Entry = (pointer) NewEdge;
	    break;
	}
    }

    for (Last = NULL, Ptr = Edge->InNodes; Ptr != NULL; 
	 Last = Ptr, Ptr = Ptr->Next) 
	if (Ptr->Entry == (pointer) Node)
	    break;
    if (Last == NULL)
	Edge->InNodes = Ptr->Next;
    else
	Last->Next = Ptr->Next;


    NewEdge->InNodes = ListSelectiveAdd(Ptr, NewEdge->InNodes);
}

