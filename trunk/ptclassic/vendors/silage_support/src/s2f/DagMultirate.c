/* $Id$ */

/*
 * Copyright (C) 1990 Regents of the University of California
 * All rights reserved.
 */

/*
 * Routines to cluster processes into iterations.
 */ 

#include "dag.h"
#include "st.h"
#include "math.h"

typedef enum {false, true} bool;

/* external functions */
GraphPointer GraphInput();
NodePointer *TopologicalOrderingWithDelay();
NodePointer RemoveFromNodeList(), CreateNewNode();
NodePointer NodeAppend(), NodeListAppend(), NewNode();
EdgePointer DuplicateEdge(), RemoveFromEdgeList(), NewEdge(), CreateNewEdge();
EdgePointer SelectiveEdgeListAppend(), EdgeListAppend();
EdgePointer GetNthEdge(), GetNthGraphEdge();
ListPointer ListSelectiveAdd(), SetAttribute(), ListAppend(), RemoveFromList();
ListPointer RemoveAttribute(), RemoveEntryFromList(), CopyList();
char *UniqueName(), *Intern(), *MakeRange(), *EdgeName(), *IndexEdgesName();
char *Int2Asc(), ConstIndexName(), *GetType();
pointer GetAttribute();

/* constants */
#define NOT_CONSIDERED -1

/* local variables */
#define MAXPROCESS 50
static int processlist[MAXPROCESS];
static float rate[MAXPROCESS];
static int product = 1;
static int NrOfProcess = 1;
static st_table *ReferTable;
extern int strcmp();

ClusterProcess(Graph)
GraphPointer Graph;
{
    ReferTable = st_init_table(strcmp, st_strhash);
    FindProcess(Graph);
    st_free_table(ReferTable);
    if (!HasAttribute(Graph->Attributes, "HasMultirate")) return;
    printmsg(NULL, "clustering processes in hierarchical subgraphs ...\n");
    RevealProcess(Graph);
    AddTransfers(Graph);
    ClusterProcessAux(Graph);
}

/*
 * Traverse Graph bottom up and label subgraphs if they contain a multirate
 * node as their descendants.
 */

FindProcess(Graph)
GraphPointer Graph;
{
    GraphPointer SubGraph;
    NodePointer Node;
    int HasMultirate;

    HasMultirate = FALSE;
 
    for (Node = Graph->NodeList; Node != NULL; Node = Node->Next) {
        if (IsHierarchyNode(Node)) {
	    SubGraph = (GraphPointer)Node->Master;
	    if (st_lookup(ReferTable, SubGraph->Name, &SubGraph) == 0) {
		st_insert(ReferTable, SubGraph->Name, SubGraph);
	        FindProcess(SubGraph);
	    }
	    if (HasAttribute(SubGraph->Attributes, "HasMultirate"))
	        HasMultirate = TRUE;
	} else if (IsMultirateNode(Node))
		HasMultirate = TRUE;
    }

    if (HasMultirate) {
        if (IsIteration(Graph))
	    printmsg(NULL,"Multirate operator inside an iteration %s\n",
					Graph->Name);
        Graph->Attributes = SetAttribute("HasMultirate", IntNode, (char *)1,
				Graph->Attributes);
    }
}

/*
 * Expand Graph until all multirate nodes are shown.  We do this by proceeding
 * top down and expand all hierarchical node whose subgraph has Multirate.
 */

RevealProcess(Graph)
GraphPointer Graph;
{
    NodePointer Node, Next, NodeList;
    EdgePointer EdgeList, ControlList;
    GraphPointer SubGraph, CopyGraph();


    for (Node = Graph->NodeList; Node != NULL; Node = Node->Next) {
	if (IsHierarchyNode(Node) && HasAttribute(Node->Master->Attributes,
					"HasMultirate"))
	    RevealProcess(Node->Master);
    }

    /* Expand the functions */
    NodeList = NULL;
    EdgeList = Graph->EdgeList;
    ControlList = Graph->ControlList;

    for (Node = Graph->NodeList; Node != NULL; Node = Next) {
        Next = Node->Next;
	if (!IsHierarchyNode(Node) || !HasAttribute(Node->Master->Attributes,
				"HasMultirate")) {
            NodeList = NodeAppend(Node, NodeList);
            continue;
        }

        SubGraph = CopyGraph(Node->Master);
        FlattenNode(Node, SubGraph);

        NodeList = NodeListAppend(SubGraph->NodeList, NodeList);
        EdgeList = EdgeListAppend(SubGraph->EdgeList, EdgeList);
        ControlList = EdgeListAppend(SubGraph->ControlList, ControlList);

        free_graph(SubGraph);
        ClearNode(Node);
    }

    Graph->NodeList = NodeList;
    Graph->EdgeList = EdgeList;
    Graph->ControlList = ControlList;
}

static GraphPointer EqualGraph;

AddTransfers(Graph)
GraphPointer Graph;
{
    NodePointer EqualNode, InNode, OutNode;
    EdgePointer Edge, EqualEdge;
    ListPointer Ptr;

    EqualGraph = GraphInput("equal", FALSE);

    for (Edge = Graph->EdgeList; Edge != NULL; Edge = Edge->Next) {
	for (Ptr = Edge->InNodes; Ptr != NULL; Ptr = Ptr->Next) {
	    if (Ptr->Type != NodeNode) continue;
	    InNode = NP(Ptr);
	    if (IsMultirateNode(InNode)) break;
	}
	if (Ptr == NULL) continue;
	for (Ptr = Edge->OutNodes; Ptr != NULL; Ptr = Ptr->Next) {
	    if (Ptr->Type != NodeNode) continue;
	    OutNode = NP(Ptr);
	    if (IsMultirateNode(OutNode)) break;
	}
	if (Ptr == NULL) continue;
        EqualNode = CreateNewNode(Graph, UniqueName("transfer"), 
			Intern("data"), EqualGraph);
        EqualEdge = CreateNewEdge(Graph, UniqueName("transfer"), 
				Intern("data"));
        EqualEdge->Arguments = SetAttribute("type", CharNode, 
				GetType(Edge), EqualEdge->Arguments);
	DisconnectNodeFromEdge(InNode, NodeNode, Edge);
	ConnectNodeToEdge(InNode, NodeNode, EqualEdge);
	ConnectEdgeToNode(EqualEdge, EqualNode, NodeNode);
	ConnectNodeToEdge(EqualNode, NodeNode, Edge);
    }
}

ClusterProcessAux(Graph)
GraphPointer Graph;
{
    NodePointer *NodeOrder;
    int NrOfNodes;

    /* first go down in the hierachy and perform clustering on the loop
     * and iteration sub-graphs
     */
    

    /* Perform the clustering operation */
    /* 0) Remove constant nodes and constant cast nodes
       1) Add equal nodes to Graph->OutEdges that comes from a MultirateNode 
       1) Topological ordering
       2) Perform an extremely simple scheduling, merging nodes if
	  they are of same process
       3) Reorganize the graph
       4) For each Multirate iteration, increment the dimension of its
          I/O edges
     */
    NodeOrder = TopologicalOrderingWithDelay(Graph, &NrOfNodes);

    DetermineNodeProcess(NodeOrder, NrOfNodes);
 
    DetermineProcessRate();

    CreateProcessClusters(Graph);

    MatchIOorder(Graph);

    IncrementEdgeDimension(Graph);

    RemoveMultirateNode(Graph);

/*    ModifyDelaysInProcess(Graph);  */

    cfree(NodeOrder);
}

AddDummyEndProcess(Graph)
GraphPointer Graph;
{
    ListPointer Ptr, Ptr2;
    NodePointer Node;
    EdgePointer e, Edge;

    for (Ptr = Graph->OutEdges; Ptr != NULL; Ptr = Ptr->Next) {
	if (Ptr->Type != EdgeNode)
	    continue;
	Edge = EP(Ptr);
        for (Ptr2 = Edge->InNodes; Ptr2 != NULL; Ptr2 = Ptr2->Next) {
	    if (Ptr2->Type != NodeNode || !IsMultirateNode(NP(Ptr2)))
		continue;
        /* Add an ASSIGN node in between output of multirate node and Edge */
	    Node = NewNode(ASSIGN, "data");
	    Graph->NodeList = NodeListAppend(Node, Graph->NodeList);
	    e = NewEdge(NULL, "data", GetType(Edge));
	    Graph->EdgeList = EdgeListAppend(e, Graph->EdgeList);
	    DisconnectNodeFromEdge(NP(Ptr2), NodeNode, Edge);
	    ConnectNodeToEdge(NP(Ptr2), NodeNode, e);
	    ConnectEdgeToNode(e, Node, NodeNode);
	    ConnectNodeToEdge(Node, NodeNode, Edge);
	}
    }
}

DetermineNodeProcess(NodeOrder, NrOfNodes)
NodePointer *NodeOrder;
int NrOfNodes;
{
    int i, j;
    int Process, scale, MyProcess, Multirate;
    char *Function, *GetFunction();
    float InputRate;
    NodePointer Node, FanInNode;
    ListPointer Ptr1, Ptr2;
    EdgePointer Edge;

    /* Process 0 is the rate of the input samples.  It is normalized to 1 */
    rate[0] = 1.0;

    /* Label the nodes */
    for (i = NrOfNodes - 1; i >= 0; i--) {
	Node = NodeOrder[i];
	MyProcess = (IsMultirateNode(Node)) ? TRUE : FALSE;

        /* Run over the fanin and select the highest number */
	Process = 0;
	InputRate = 0.0;
        Multirate = FALSE;
    
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
                if (FanInNode->Token > Process) {
		    Process = FanInNode->Token;
 		    InputRate = rate[FanInNode->Token];
		    Multirate = IsMultirateNode(FanInNode) ? TRUE : FALSE;
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
                if (FanInNode->Token > Process) {
		    Process = FanInNode->Token;
 		    InputRate = rate[FanInNode->Token];
		    Multirate = IsMultirateNode(FanInNode) ? TRUE : FALSE;
	        }
	    }
	}

        if (MyProcess == TRUE) {
            if (NrOfProcess == MAXPROCESS) {
                printmsg("DetermineNodeProcess",
                "Maximum Number of Processes exceeded ");
                printmsg("Increase MAXPROCESS");
                exit(-1);
            }
            Node->Token = NrOfProcess;
	    if (InputRate == 0.0)
		InputRate = 1.0;
	    Function = GetFunction(Node);
	    scale = (int)GetAttribute(Node->Arguments, "Scale");
	    if (!strcmp(Function, UPSAMPLE) || 
		!strcmp(Function, INTERPOLATE) ||
		!strcmp(Function, TIMEMUX)) {
	        rate[Node->Token] = InputRate * scale;
            }
	    else if (!strcmp(Function, DOWNSAMPLE) ||
                     !strcmp(Function, DECIMATE) || 
		     !strcmp(Function, TIMEDEMUX)) {
	        rate[Node->Token] = InputRate / scale;
		product = product * scale;
	    }
            processlist[NrOfProcess++] = TRUE;
            continue;
        }

	if (Process == 0 || Multirate == TRUE) {
	    /* find first available process, larger then Process */
	    for (j = Process + 1; Process < NrOfProcess; j++)
		if (processlist[j] == FALSE)
	 	    break;
            if (j == NrOfProcess) {
	        if (NrOfProcess == MAXPROCESS) {
		    printmsg("DetermineNodeProcess",
		         "Maximum Number of Processes exceeded");
		    printmsg("Increase MAXPROCESS");
    		    exit (-1);
	        }
                Node->Token = NrOfProcess;
		processlist[NrOfProcess++] = FALSE;
	    }
	    else
                Node->Token = j;
	    rate[Node->Token] = (InputRate)? InputRate : 1.0;
	}
	else { /* merge data classes */
	    Node->Token = Process;
	}
    }
}

DetermineProcessRate()
{
    int i, j, cd, procrate;

/* Normalize to get all integer rates */

    for (i = 0; i < NrOfProcess; i++) {
	rate[i] = rate[i] * product;
    }

/* Find gcd of all integer rates */

    cd = (int) rate[0];
    for (i = 1; i < NrOfProcess; i++) {
	procrate = (int) rate[i];
	cd = gcd (cd, procrate);
    }

/* Normalize to get lowest all integer rates */

    printmsg(NULL, "Rate of processes...\n");
    j = 0;
    for (i = 0; i < NrOfProcess; i++) {
	rate[i] = rate[i] / cd;
        if (i == 0)
            printmsg(NULL, "Input rate:  %d\n", (int)rate[i]);
        else if (processlist[i] == TRUE)  /* Multirate Node */
            printmsg(NULL, "Rate change from %d to %d\n", (int)rate[i-1],
							(int)rate[i]);
	else
            printmsg(NULL, "Process %d:  %d\n", j++, (int)rate[i]);
    }
}

gcd (u, v)
int u, v;
{
    if (v == 0) 
        return(u);
    else
	return(gcd(v, u % v));
}

CreateProcessClusters(Graph)
GraphPointer Graph;
{
    /* Merge data nodes of the same class together in a subgraph */

    int i;
    ListPointer Ptr, Ptr2;
    GraphPointer SubGraph;
    NodePointer FuncNode, Node, Source, Dest, Next;
    EdgePointer Edge, NewEdge, NextEdge;
    GraphPointer GraphInputWithInit();
	
    if (NrOfProcess <= 1)
	return;
    
    for (i = 1; i < NrOfProcess; i++) {
        if (processlist[i] == TRUE || rate[i] == 1) /* Multirate Node or rate 1 */
	    continue;

	/* Create a new node of class "func" */
	SubGraph = GraphInputWithInit("iteration", FALSE);

	SubGraph->Name = UniqueName("subgraph");

	FuncNode = CreateNewNode(Graph, SubGraph->Name, Intern("Process"),
	    SubGraph);
	FuncNode->Token = -1;
        FuncNode->Arguments = SetAttribute("index", CharNode, Intern("i"), 
							FuncNode->Arguments);
        FuncNode->Arguments = SetAttribute("min", IntNode, 0, 
							FuncNode->Arguments);
        FuncNode->Arguments = SetAttribute("max", IntNode, (int)(rate[i]-1), 
							FuncNode->Arguments);
        FuncNode->Attributes = SetAttribute("Dependency", CharNode, 
				Intern("parallel"), FuncNode->Attributes);

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

MatchIOorder(Graph)
GraphPointer Graph;
{
    NodePointer Node;

    for (Node = Graph->NodeList; Node != NULL; Node = Node->Next) {
        if (IsTimeMuxNode(Node))
	    MatchInputOrder(Node);
    }
}

MatchInputOrder(Node)
NodePointer Node;
{
    EdgePointer InEdge;
    NodePointer InNode;
    GraphPointer Subgraph;
    ListPointer Ptr, Ptr1, OutList;

/*  May need to be alot more general */
    InEdge = EP(Node->InEdges);
    InNode = NP(InEdge->InNodes);
    if (IsProcessNode(InNode)) {
        InNode->OutEdges = CopyList(Node->InEdges);
	Subgraph = InNode->Master;
	OutList = Subgraph->OutEdges;
	Subgraph->OutEdges = (ListPointer) NULL;
	for (Ptr = InNode->OutEdges; Ptr != NULL; Ptr = Ptr->Next) {
	    for (Ptr1 = OutList; Ptr1 != NULL; Ptr1 = Ptr1->Next) {
		if (strcmp(EP(Ptr1)->Name, EP(Ptr)->Name) == 0)
		    Subgraph->OutEdges = ListAppend(new_list(EdgeNode,
				Ptr1->Entry), Subgraph->OutEdges);
	    }
	}
	ClearList(OutList);
    }
}

IsMultirateNode(node)
NodePointer node;
{
    char *Function, *GetFunction();

    if ((Function = GetFunction(node)) == NULL)
        return (FALSE);
    if (!strcmp(Function, UPSAMPLE)) return(TRUE);
    if (!strcmp(Function, DOWNSAMPLE)) return(TRUE);
    if (!strcmp(Function, INTERPOLATE)) return(TRUE);
    if (!strcmp(Function, DECIMATE)) return(TRUE);
    if (!strcmp(Function, TIMEMUX)) return(TRUE);
    if (!strcmp(Function, TIMEDEMUX)) return(TRUE);

    return(FALSE);
}

IsTimeMuxNode(node)
NodePointer node;
{
    char *Function, *GetFunction();

    if ((Function = GetFunction(node)) == NULL)
        return (FALSE);
    if (!strcmp(Function, TIMEMUX)) return(TRUE);
    return(FALSE);
}

IsTimeDeMuxNode(node)
NodePointer node;
{
    char *Function, *GetFunction();

    if ((Function = GetFunction(node)) == NULL)
        return (FALSE);
    if (!strcmp(Function, TIMEDEMUX)) return(TRUE);
    return(FALSE);
}

HasTimeMuxOutNode(node)
NodePointer node;
{
    ListPointer Ptr1, Ptr2;
    EdgePointer Edge;
    NodePointer OutNode;

    for (Ptr1 = node->OutEdges; Ptr1 != NULL; Ptr1 = Ptr1->Next) {
        if (Ptr1->Type != EdgeNode) continue;
	Edge = (EdgePointer) Ptr1->Entry;
	for (Ptr2 = Edge->OutNodes; Ptr2 != NULL; Ptr2 = Ptr2->Next) {
            if (Ptr2->Type != NodeNode) continue;
	    OutNode = (NodePointer) Ptr2->Entry;
	    if (IsTimeMuxNode(OutNode)) return(TRUE);
	}
    }

    for (Ptr1 = node->OutControl; Ptr1 != NULL; Ptr1 = Ptr1->Next) {
        if (Ptr1->Type != EdgeNode) continue;
	Edge = (EdgePointer) Ptr1->Entry;
	for (Ptr2 = Edge->OutNodes; Ptr2 != NULL; Ptr2 = Ptr2->Next) {
            if (Ptr2->Type != NodeNode) continue;
	    OutNode = (NodePointer) Ptr2->Entry;
	    if (IsTimeMuxNode(OutNode)) return(TRUE);
	}
    }
    return(FALSE);
}

HasTimeDeMuxInNode(node)
NodePointer node;
{
    ListPointer Ptr1, Ptr2;
    EdgePointer Edge;
    NodePointer InNode;

    for (Ptr1 = node->InEdges; Ptr1 != NULL; Ptr1 = Ptr1->Next) {
        if (Ptr1->Type != EdgeNode) continue;
	Edge = (EdgePointer) Ptr1->Entry;
	for (Ptr2 = Edge->InNodes; Ptr2 != NULL; Ptr2 = Ptr2->Next) {
            if (Ptr2->Type != NodeNode) continue;
	    InNode = (NodePointer) Ptr2->Entry;
	    if (IsTimeDeMuxNode(InNode)) return(TRUE);
	}
    }

    for (Ptr1 = node->InControl; Ptr1 != NULL; Ptr1 = Ptr1->Next) {
        if (Ptr1->Type != EdgeNode) continue;
	Edge = (EdgePointer) Ptr1->Entry;
	for (Ptr2 = Edge->InNodes; Ptr2 != NULL; Ptr2 = Ptr2->Next) {
            if (Ptr2->Type != NodeNode) continue;
	    InNode = (NodePointer) Ptr2->Entry;
	    if (IsTimeDeMuxNode(InNode)) return(TRUE);
	}
    }
    return(FALSE);
}

IsMultirateIterNode(Node)
NodePointer Node;
{
    ListPointer Ptr1, Ptr2;
    EdgePointer Edge;
    NodePointer InNode, OutNode;

    if (!IsIterationNode(Node)) return(FALSE);

    for (Ptr1 = Node->InEdges; Ptr1 != NULL; Ptr1 = Ptr1->Next) {
        if (Ptr1->Type != EdgeNode) continue;
	Edge = (EdgePointer) Ptr1->Entry;
	for (Ptr2 = Edge->InNodes; Ptr2 != NULL; Ptr2 = Ptr2->Next) {
            if (Ptr2->Type != NodeNode) continue;
	    InNode = (NodePointer) Ptr2->Entry;
	    if (IsMultirateNode(InNode)) return(TRUE);
	}
    }

    for (Ptr1 = Node->InControl; Ptr1 != NULL; Ptr1 = Ptr1->Next) {
        if (Ptr1->Type != EdgeNode) continue;
	Edge = (EdgePointer) Ptr1->Entry;
	for (Ptr2 = Edge->InNodes; Ptr2 != NULL; Ptr2 = Ptr2->Next) {
            if (Ptr2->Type != NodeNode) continue;
	    InNode = (NodePointer) Ptr2->Entry;
	    if (IsMultirateNode(InNode)) return(TRUE);
	}
    }

    for (Ptr1 = Node->OutEdges; Ptr1 != NULL; Ptr1 = Ptr1->Next) {
        if (Ptr1->Type != EdgeNode) continue;
	Edge = (EdgePointer) Ptr1->Entry;
	for (Ptr2 = Edge->OutNodes; Ptr2 != NULL; Ptr2 = Ptr2->Next) {
            if (Ptr2->Type != NodeNode) continue;
	    OutNode = (NodePointer) Ptr2->Entry;
	    if (IsMultirateNode(OutNode)) return(TRUE);
	}
    }

    for (Ptr1 = Node->OutControl; Ptr1 != NULL; Ptr1 = Ptr1->Next) {
        if (Ptr1->Type != EdgeNode) continue;
	Edge = (EdgePointer) Ptr1->Entry;
	for (Ptr2 = Edge->OutNodes; Ptr2 != NULL; Ptr2 = Ptr2->Next) {
            if (Ptr2->Type != NodeNode) continue;
	    OutNode = (NodePointer) Ptr2->Entry;
	    if (IsMultirateNode(OutNode)) return(TRUE);
	}
    }

    return(FALSE);
}

ModifyDelaysInProcess(Graph)
GraphPointer Graph;
{
    NodePointer Node;

    ReferTable = st_init_table(strcmp, st_strhash);

    for (Node = Graph->NodeList; Node != NULL; Node = Node->Next) {
        if (!IsProcessNode(Node)) continue;
	SampleToLoopDelay(Node->Master);
    }
    st_free_table(ReferTable);
}

SampleToLoopDelay(Graph)
GraphPointer Graph;
{
    NodePointer Node;
    GraphPointer SubGraph, NewGraph();
    char *Function, *GetFunction();

    for (Node = Graph->NodeList; Node != NULL; Node = Node->Next) {
        if (IsHierarchy(Node)) {
            SubGraph = (GraphPointer)Node->Master;
            if (st_lookup(ReferTable, SubGraph->Name, NULL) == 0) {
                st_insert(ReferTable, SubGraph->Name, 0);
                SampleToLoopDelay(SubGraph);
            }
        }

        Function = GetFunction(Node);
 
        if (strcmp(Function, "@"))
	    continue;
        
  	Node->Master = NewGraph(LPDELAY);
        /* Modify name of output edge from @ to # */
	if (Node->OutEdges != NULL)
	    SubstituteChar(EP(Node->OutEdges), '@', '#');
        else
	    SubstituteChar(EP(Node->OutControl), '@', '#');
    }
}

SubstituteChar(Edge, oldc, newc)
EdgePointer Edge;
char oldc, newc;
{
    char *p1, *p2;
    char str[STRSIZE];

    for(p1 = Edge->Name, p2 = str; *p1 != NULL; p1++, p2++) {
	if (*p1 == oldc)
	    *p2 = newc;
	else
	    *p2 = *p1;
    }
    *p2 = '\0';
    Edge->Name = Intern(str);
}

IncrementEdgeDimension(Graph)
GraphPointer Graph;
{
    NodePointer Node;
    EdgePointer Edge;
    ListPointer Ptr, PtrNext;
    int id, lb, ub;
    NodePointer *NodeOrder;
    int NrOfNodes;

    /* Save the Process number of Multirate nodes */
    for (Node = Graph->NodeList; Node != NULL; Node = Node->Next) {
        if (IsMultirateNode(Node))
	    Node->Attributes = SetAttribute("ProcessID", IntNode, (pointer)
				Node->Token, Node->Attributes);
    }

    /* Sort the nodes so changes are consistent */
    NodeOrder = TopologicalOrderingWithDelay(Graph, &NrOfNodes);
    StoreInPlace(Graph, NodeOrder, NrOfNodes);

    /* Increment Edge Dimension */
    for (Node = Graph->NodeList; Node != NULL; Node = Node->Next) {
        if (IsProcessNode(Node)) {
            ub = (int)GetAttribute(Node->Arguments, "max");
	    IncDimDataInEdge(Node, Graph, ub);
	    IncDimDataOutEdge(Node, Graph, ub);
        } else if (IsMultirateNode(Node)) {
	    id = (int)GetAttribute(Node->Attributes, "ProcessID");
            lb = rate[id - 1] - 1;
            if (lb != 0)
	        IncDimDataInEdge(Node, Graph, lb);
            ub = rate[id] - 1;
            if (ub != 0)
	        IncDimDataOutEdge(Node, Graph, ub);
	}
    }

    /* Save the Process number of Multirate nodes */
    for (Node = Graph->NodeList; Node != NULL; Node = Node->Next) {
        if (IsMultirateNode(Node))
	    Node->Attributes = RemoveAttribute(Node->Attributes, "ProcessID");
    }
}

IncDimDataInEdge(Node, Graph, ub)
NodePointer Node;
GraphPointer Graph;
int ub;
{
    ListPointer Ptr1, Ptr1Next, Ptr2;
    EdgePointer Edge, SubEdge, GetNthGraphEdge();
    NodePointer InNode;
    int Cnt;
    char *range;

    for (Ptr1 = Node->InEdges; Ptr1 != NULL; Ptr1 = Ptr1Next) {
        Ptr1Next = Ptr1->Next;
        if (Ptr1->Type != EdgeNode) continue;
	Edge = EP(Ptr1);

        /* Move Edge from Node->InEdges to Node->InControl */

	Node->InEdges = RemoveEntryFromList(EdgeNode, Edge, Node->InEdges);
	Node->InControl = ListAppend(new_list(EdgeNode, Edge), 
							Node->InControl);
	for (Ptr2 = Edge->InNodes; Ptr2 != NULL; Ptr2 = Ptr2->Next) {
	    if (Ptr2->Type != NodeNode && Ptr2->Type != GraphNode) continue;
	    if (Ptr2->Type == NodeNode) {
		InNode = NP(Ptr2);
		Cnt = NodeIONumber(InNode, Edge, OUT);
		InNode->OutEdges = RemoveEntryFromList(EdgeNode, Edge, 
							InNode->OutEdges);
	        InNode->OutControl = ListAppend(new_list(EdgeNode, Edge), 
						InNode->OutControl);
		if (IsProcessNode(InNode)) {
                    SubEdge = GetNthGraphEdge(InNode->Master, Cnt, OUT, "data");
                    IncDimDataOutSubEdge(SubEdge, InNode->Master, ub);
                }
	    } else {
		Graph->InEdges = RemoveEntryFromList(EdgeNode, Edge, 
							Graph->InEdges);
                Graph->InControl = ListAppend(new_list(EdgeNode, Edge),
                                                        Graph->InControl);
            }
        }

        /* Change Edge class to "control", move from EdgeList to ControlList, 
           add index range, and change name */

    	Edge->Class = Intern("control");
    	Graph->EdgeList = RemoveFromEdgeList(Edge, Graph->EdgeList);
    	Edge->Next = NULL;
    	Graph->ControlList = EdgeListAppend(Edge, Graph->ControlList);
    	range = MakeRange(0,ub);
    	Edge->Arguments = SetAttribute("index0", CharNode, range,
						Edge->Arguments);
    	Edge->Name = EdgeName(Edge);

        /* Determine which Subgraph Edge is affected, and modify Subgraph */

        if (IsProcessNode(Node)) {
            SubEdge = GetNthGraphEdge(Node->Master, 0, IN, "data");
            IncDimDataInSubEdge(SubEdge, Node->Master, ub);
        }
    }
}

IncDimDataOutEdge(Node, Graph, ub)
NodePointer Node;
GraphPointer Graph;
int ub;
{
    ListPointer Ptr1, Ptr1Next, Ptr2;
    EdgePointer Edge, SubEdge, GetNthGraphEdge();
    NodePointer OutNode;
    char *range;
    int Cnt;

    for (Ptr1 = Node->OutEdges; Ptr1 != NULL; Ptr1 = Ptr1Next) {
        Ptr1Next = Ptr1->Next;
        if (Ptr1->Type != EdgeNode) continue;
	Edge = EP(Ptr1);

        /* Move Edge from Node->OutEdges to Node->OutControl */

	Node->OutEdges = RemoveEntryFromList(EdgeNode, Edge, Node->OutEdges);
	Node->OutControl = ListAppend(new_list(EdgeNode, Edge), 
							Node->OutControl);
	for (Ptr2 = Edge->OutNodes; Ptr2 != NULL; Ptr2 = Ptr2->Next) {
	    if (Ptr2->Type != NodeNode && Ptr2->Type != GraphNode) continue;
	    if (Ptr2->Type == NodeNode) {
                OutNode = NP(Ptr2);
		Cnt = NodeIONumber(OutNode, Edge, IN);
		OutNode->InEdges = RemoveEntryFromList(EdgeNode, Edge, 
							OutNode->InEdges);
	        OutNode->InControl = ListAppend(new_list(EdgeNode, Edge), 
						OutNode->InControl);
		if (IsProcessNode(OutNode)) {
                    SubEdge = GetNthGraphEdge(OutNode->Master, Cnt, IN, "data");
                    IncDimDataInSubEdge(SubEdge, OutNode->Master, ub);
		}
	    } else {
		Graph->OutEdges = RemoveEntryFromList(EdgeNode, Edge, 
							Graph->OutEdges);
                Graph->OutControl = ListAppend(new_list(EdgeNode, Edge),
                                                        Graph->OutControl);
            }
        }

        /* Change Edge class to "control", move from EdgeList to ControlList, 
           add index range, and change name */

    	Edge->Class = Intern("control");
    	Graph->EdgeList = RemoveFromEdgeList(Edge, Graph->EdgeList);
    	Edge->Next = NULL;
    	Graph->ControlList = EdgeListAppend(Edge, Graph->ControlList);
    	range = MakeRange(0,ub);
    	Edge->Arguments = SetAttribute("index0", CharNode, range,
						Edge->Arguments);
    	Edge->Name = EdgeName(Edge);

        /* Determine which Subgraph Edge is affected, and modify Subgraph */

        if (IsProcessNode(Node)) {
            SubEdge = GetNthGraphEdge(Node->Master, 0, OUT, "data");
            IncDimDataOutSubEdge(SubEdge, Node->Master, ub);
        }
    }
}

IncDimDataInSubEdge(Edge, Graph, ub)
EdgePointer Edge;
GraphPointer Graph;
int ub;
{
    char *range;
    EdgePointer ArrayEdge, IterEdge;
    NodePointer Node;

    /* Disconect  Edge from Graph->InEdges */
    DisconnectNodeFromEdge(Graph, GraphNode, Edge);

    /* Create new Array edge, add to Controllist, Connect to Graph */
    ArrayEdge = NewEdge(Edge->Name, "control", GetType(Edge));
    range = MakeRange(0, ub);
    ArrayEdge->Arguments = SetAttribute("index0", CharNode, range,
			   ArrayEdge->Arguments);
    ArrayEdge->Name = EdgeName(ArrayEdge);
    Graph->ControlList = EdgeListAppend(ArrayEdge, Graph->ControlList);
    ConnectNodeToEdge(Graph, GraphNode, ArrayEdge);

    /* Create Iter edge if not present, add to Edgelist */
    IterEdge = NewEdge("i", "data", "int");
    Graph->EdgeList = SelectiveEdgeListAppend(&IterEdge, Graph->EdgeList);
    ConnectNodeToEdge(Graph, GraphNode, IterEdge);

    /* Create READ node, and make connections */
    Node = NewNode(READ, "data");
    Graph->NodeList = NodeListAppend(Node, Graph->NodeList);
    Node->Arguments = SetAttribute("array_name", CharNode, Intern(Edge->Name),
                                   Node->Arguments);
    ConnectEdgeToNode(ArrayEdge, Node, NodeNode);
    ConnectEdgeToNode(IterEdge, Node, NodeNode);
    ConnectNodeToEdge(Node, NodeNode, Edge); 

    /* Change name of Edge, needed Read node connected */
    Edge->Name = Intern(IndexEdgesName(Node));
}

IncDimDataOutSubEdge(Edge, Graph, ub)
EdgePointer Edge;
GraphPointer Graph;
int ub;
{
    char *range, *TmpEdgeName();
    EdgePointer ArrayEdge, IterEdge;
    NodePointer Node;

    /* Disconect  Edge from Graph->OutEdges */
    DisconnectEdgeFromNode(Edge, Graph, GraphNode);

    /* Create new Array edge, add to Controllist, Connect to Graph */
    ArrayEdge = NewEdge(Edge->Name, "control", GetType(Edge));
    range = MakeRange(0, ub);
    ArrayEdge->Arguments = SetAttribute("index0", CharNode, range,
			   ArrayEdge->Arguments);
    ArrayEdge->Name = EdgeName(ArrayEdge);
    Graph->ControlList = EdgeListAppend(ArrayEdge, Graph->ControlList);
    ConnectEdgeToNode(ArrayEdge, Graph, GraphNode);

    /* Create Iter edge if not present, add to Edgelist */
    IterEdge = NewEdge("i", "data", "int");
    Graph->EdgeList = SelectiveEdgeListAppend(&IterEdge, Graph->EdgeList);
    ConnectNodeToEdge(Graph, GraphNode, IterEdge);

    /* Create WRITE node, and make connections */
    Node = NewNode(WRITE, "data");
    Graph->NodeList = NodeListAppend(Node, Graph->NodeList);
    Node->Arguments = SetAttribute("array_name", CharNode, Intern(Edge->Name),
                                   Node->Arguments);
    ConnectEdgeToNode(Edge, Node, NodeNode);
    ConnectEdgeToNode(IterEdge, Node, NodeNode);
    ConnectNodeToEdge(Node, NodeNode, ArrayEdge); 

    /* Change name of Edge */
    Edge->Name = TmpEdgeName();
}

static GraphPointer AddGraph;
static GraphPointer MultGraph;
static GraphPointer ReadGraph;
static GraphPointer WriteGraph;

RemoveMultirateNode(Graph)
GraphPointer Graph;
{
    AddGraph = GraphInput("add", FALSE);
    MultGraph = GraphInput("multiply", FALSE);
    ReadGraph = GraphInput("read", FALSE);
    WriteGraph = GraphInput("write", FALSE);

    OptimizeDataTransfer(Graph);
}

OptimizeDataTransfer(Graph)
GraphPointer Graph;
{
    NodePointer Node, NextNode;

    for (Node = Graph->NodeList; Node != NULL; Node = NextNode) {
	NextNode = Node->Next;
	if (!IsMultirateNode(Node)) continue;
	if (IsTimeMuxNode(Node))
	    OptimizeTimeMuxTransfer(Graph, Node);
	else if (IsTimeDeMuxNode(Node))
	    OptimizeTimeDeMuxTransfer(Graph, Node);
	else
	    printmsg(NULL, "OptimizeDataTransfer not implemented\n");
    }
}

OptimizeTimeMuxTransfer(Graph, Node)
GraphPointer Graph;
NodePointer Node;
{
    int scale;
    EdgePointer InEdge, OutEdge;
    NodePointer InNode, OutNode;
    ListPointer Ptr, PtrNext, Ptr1;

    scale = (int)GetAttribute(Node->Arguments, "Scale");
    OutEdge = EP(Node->OutControl);
    /* Input edges are data, implement writes immediately */
    if (Node->InControl == NULL) {
	TransformTimeMuxNode(Graph, Node);
	return;
    }
    InEdge = EP(Node->InControl);
    InNode = NP(InEdge->InNodes);
    for (Ptr = Node->InControl; Ptr != NULL; Ptr = PtrNext) {
	PtrNext = Ptr->Next;
	if (Ptr->Type != EdgeNode) continue;
	InEdge = EP(Ptr);
        EliminateEdge(Graph, InEdge);
    }
    OptimizeTimeMuxTransferInSubgraph(InNode, OutEdge, scale);
    EliminateNode(Graph, Node);
    ConnectNodeToEdge(InNode, NodeNode, OutEdge);
}

OptimizeTimeMuxTransferInSubgraph(InNode, OutEdge, scale)
NodePointer InNode;
EdgePointer OutEdge;
int scale;
{
    int offset;
    char *ArrayName, EdgeType;
    EdgePointer SubEdge,SubOutEdge,IndexEdge, TmpEdge, ScaleEdge, OffsetEdge;
    NodePointer WriteNode, MultNode, AddNode;
    GraphPointer Subgraph;
    ListPointer Ptr, PtrNext;
    bool IsWriteNode();
    
    Subgraph = InNode->Master;
    SubOutEdge = DuplicateEdge(Subgraph, OutEdge, OutEdge->Name);
    /* Traverse all output arrays of InNode, process the write nodes,
       eliminate the output arrays, connect to OutEdge */
    for (Ptr = Subgraph->OutControl, offset = 0; Ptr != NULL; 
			Ptr = PtrNext, offset++) {
	PtrNext = Ptr->Next;
	if (Ptr->Type != EdgeNode) continue;
	SubEdge = EP(Ptr);
        WriteNode = NP(SubEdge->InNodes);
        if (!IsWriteNode(WriteNode)) 
	    Error("OptimizeTimeMuxTransferInSubgraph: Not Write Node\n");
	ArrayName = Intern(GetArrayName(SubOutEdge));
	WriteNode->Arguments = SetAttribute("array_name", CharNode, 
					ArrayName, WriteNode->Arguments);
        /* Locate and Change index edge */
        IndexEdge = GetNthEdge(WriteNode, IN, 1, "data");
        DisconnectEdgeFromNode(IndexEdge, WriteNode, NodeNode);
        MultNode = CreateNewNode(Subgraph, UniqueName("multnode"), 
			Intern("data"), MultGraph);
        ScaleEdge = CreateNewEdge(Subgraph, UniqueName("scale"), 
				Intern("constant"));
        ScaleEdge->Arguments = SetAttribute("type", CharNode, 
				GetType(IndexEdge), ScaleEdge->Arguments);
        ScaleEdge->Attributes = SetAttribute("value", CharNode, Int2Asc(scale),
				ScaleEdge->Attributes);
        TmpEdge = CreateNewEdge(Subgraph,UniqueName("index"),Intern("data"));
        TmpEdge->Arguments = SetAttribute("type", CharNode, 
				GetType(IndexEdge), TmpEdge->Arguments);
        ConnectEdgeToNode(IndexEdge, MultNode, NodeNode);
        ConnectEdgeToNode(ScaleEdge, MultNode, NodeNode);
        ConnectNodeToEdge(MultNode, NodeNode, TmpEdge);
        if (offset > 0) {
            OffsetEdge = CreateNewEdge(Subgraph, UniqueName("offset"),
							Intern("constant"));
            OffsetEdge->Arguments = SetAttribute("type", CharNode, 
				GetType(IndexEdge), OffsetEdge->Arguments);
            OffsetEdge->Attributes = SetAttribute("value", CharNode, 
				Int2Asc(offset), OffsetEdge->Attributes);
            AddNode = CreateNewNode(Subgraph, UniqueName("addnode"), 
					Intern("data"), AddGraph);
            ConnectEdgeToNode(TmpEdge, AddNode, NodeNode);
            ConnectEdgeToNode(OffsetEdge, AddNode, NodeNode);
	    TmpEdge = CreateNewEdge(Subgraph, UniqueName("index"),
					Intern("data"));
            TmpEdge->Arguments = SetAttribute("type", CharNode, 
				GetType(IndexEdge), TmpEdge->Arguments);
            ConnectNodeToEdge(AddNode, NodeNode, TmpEdge);
        }
        SetNthEdge(WriteNode, TmpEdge, IN, 1);
        EliminateEdge(Subgraph, SubEdge);
        ConnectNodeToEdge(WriteNode, NodeNode, SubOutEdge);
    }
    ConnectEdgeToNode(SubOutEdge, Subgraph, GraphNode);
}

OptimizeTimeDeMuxTransfer(Graph, Node)
GraphPointer Graph;
NodePointer Node;
{
    int scale, cnt;
    EdgePointer InEdge, OutEdge;
    NodePointer InNode, OutNode;
    ListPointer Ptr, PtrNext, Ptr1;

    scale = (int)GetAttribute(Node->Arguments, "Scale");
    InEdge = EP(Node->InControl);
    /* Input edges are data, implement writes immediately */
    if (Node->OutControl == NULL) {
	TransformTimeDeMuxNode(Graph, Node);
	return;
    }
    /* Traverse output arrays to see which incidents on a process */
    for (Ptr = Node->OutControl, cnt=0; Ptr != NULL; Ptr = PtrNext, cnt++) {
	PtrNext = Ptr->Next;
	if (Ptr->Type != EdgeNode) continue;
        OutEdge = EP(Ptr);
        for (Ptr1 = OutEdge->OutNodes; Ptr1 != NULL; Ptr1 = Ptr1->Next) {
	     if (Ptr1->Type != NodeNode) continue;
             OutNode = NP(Ptr1);
             OptimizeTimeDeMuxTransferInSubgraph(OutNode, InEdge, scale, cnt);
             ConnectEdgeToNode(InEdge, OutNode, NodeNode);
	}
        EliminateEdge(Graph, OutEdge);
    }

    EliminateNode(Graph, Node);
}

TransformTimeMuxNode(Graph, Node)
GraphPointer Graph;
NodePointer Node;
{
    int cnt;
    EdgePointer InEdge, OutEdge;
    NodePointer WriteNode;
    ListPointer Ptr, PtrNext;

    /* Expects Input Edge of node to be scalar */
    OutEdge = EP(Node->OutControl);
    DisconnectNodeFromEdge(Node, NodeNode, OutEdge);
    for (Ptr = Node->InEdges, cnt=0; Ptr != NULL; Ptr = PtrNext, cnt++) {
	PtrNext = Ptr->Next;
	if (Ptr->Type != EdgeNode) continue;
	InEdge = EP(Ptr);
	DisconnectEdgeFromNode(InEdge, Node, NodeNode);
        WriteNode = CreateNewNode(Graph, UniqueName("writenode"), 
			Intern("data"), WriteGraph);
        WriteNode->Arguments = SetAttribute("array_name", CharNode, 
			GetArrayName(OutEdge), WriteNode->Arguments);
        WriteNode->Arguments = SetAttribute("index0", CharNode, 
			Int2Asc(cnt), WriteNode->Arguments);
        ConnectEdgeToNode(InEdge, WriteNode, NodeNode);
        ConnectNodeToEdge(WriteNode, NodeNode, OutEdge);
    }
    EliminateNode(Graph, Node);
}

TransformTimeDeMuxNode(Graph, Node)
GraphPointer Graph;
NodePointer Node;
{
    int cnt;
    EdgePointer InEdge, OutEdge;
    NodePointer ReadNode;
    ListPointer Ptr, PtrNext;

    /* Expects Output Edge of node to be scalar */
    InEdge = EP(Node->InControl);
    DisconnectEdgeFromNode(InEdge, Node, NodeNode);
    for (Ptr = Node->OutEdges, cnt=0; Ptr != NULL; Ptr = PtrNext, cnt++) {
	PtrNext = Ptr->Next;
	if (Ptr->Type != EdgeNode) continue;
	OutEdge = EP(Ptr);
	DisconnectNodeFromEdge(Node, NodeNode, OutEdge);
        ReadNode = CreateNewNode(Graph, UniqueName("readnode"), 
			Intern("data"), ReadGraph);
        ReadNode->Arguments = SetAttribute("array_name", CharNode, 
			GetArrayName(InEdge), ReadNode->Arguments);
        ReadNode->Arguments = SetAttribute("index0", CharNode, 
			Int2Asc(cnt), ReadNode->Arguments);
        ConnectNodeToEdge(ReadNode, NodeNode, OutEdge);
        OutEdge->Name = Intern(IndexEdgesName(ReadNode));
        ConnectEdgeToNode(InEdge, ReadNode, NodeNode);
    }
    EliminateNode(Graph, Node);
}

OptimizeTimeDeMuxTransferInSubgraph(OutNode, InEdge, scale, cnt)
NodePointer OutNode;
EdgePointer InEdge;
int scale, cnt;
{
    int offset;
    char *ArrayName, EdgeType;
    EdgePointer SubEdge,SubInEdge,IndexEdge, TmpEdge, ScaleEdge, OffsetEdge;
    EdgePointer ReadOutEdge;
    NodePointer ReadNode, MultNode, AddNode;
    GraphPointer Subgraph;
    ListPointer Ptr, PtrNext;
    bool IsReadNode();
    
    Subgraph = OutNode->Master;
    SubInEdge = DuplicateEdge(Subgraph, InEdge, InEdge->Name);
    /* Traverse all input arrays of OutNode, process the read nodes,
       eliminate the input arrays, connect to InEdge */
    for (Ptr = Subgraph->InControl; Ptr != NULL; Ptr = PtrNext) {
	PtrNext = Ptr->Next;
	if (Ptr->Type != EdgeNode) continue;
	SubEdge = EP(Ptr);
        ReadNode = NP(SubEdge->OutNodes);
        if (!IsReadNode(ReadNode)) 
	    Error("OptimizeTimeDeMuxTransferInSubgraph: Not Read Node\n");
	ArrayName = Intern(GetArrayName(SubInEdge));
	ReadNode->Arguments = SetAttribute("array_name", CharNode, 
					ArrayName, ReadNode->Arguments);
        /* Locate and Change index edge */
        IndexEdge = GetNthEdge(ReadNode, IN, 0, "data");
        DisconnectEdgeFromNode(IndexEdge, ReadNode, NodeNode);
        MultNode = CreateNewNode(Subgraph, UniqueName("multnode"), 
			Intern("data"), MultGraph);
        ScaleEdge = CreateNewEdge(Subgraph, UniqueName("scale"), 
				Intern("constant"));
        ScaleEdge->Arguments = SetAttribute("type", CharNode, 
				GetType(IndexEdge), ScaleEdge->Arguments);
        ScaleEdge->Attributes = SetAttribute("value", CharNode, Int2Asc(scale),
				ScaleEdge->Attributes);
        TmpEdge = CreateNewEdge(Subgraph,UniqueName("index"),Intern("data"));
        TmpEdge->Arguments = SetAttribute("type", CharNode, 
				GetType(IndexEdge), TmpEdge->Arguments);
        ConnectEdgeToNode(IndexEdge, MultNode, NodeNode);
        ConnectEdgeToNode(ScaleEdge, MultNode, NodeNode);
        ConnectNodeToEdge(MultNode, NodeNode, TmpEdge);
        if (cnt > 0) {
            OffsetEdge = CreateNewEdge(Subgraph, UniqueName("offset"),
							Intern("constant"));
            OffsetEdge->Arguments = SetAttribute("type", CharNode, 
				GetType(IndexEdge), OffsetEdge->Arguments);
            OffsetEdge->Attributes = SetAttribute("value", CharNode, 
				Int2Asc(cnt), OffsetEdge->Attributes);
            AddNode = CreateNewNode(Subgraph, UniqueName("addnode"), 
					Intern("data"), AddGraph);
            ConnectEdgeToNode(TmpEdge, AddNode, NodeNode);
            ConnectEdgeToNode(OffsetEdge, AddNode, NodeNode);
	    TmpEdge = CreateNewEdge(Subgraph, UniqueName("index"),
					Intern("data"));
            TmpEdge->Arguments = SetAttribute("type", CharNode, 
				GetType(IndexEdge), TmpEdge->Arguments);
            ConnectNodeToEdge(AddNode, NodeNode, TmpEdge);
        }
        SetNthEdge(ReadNode, TmpEdge, IN, 0);
        EliminateEdge(Subgraph, SubEdge);
        ConnectEdgeToNode(SubInEdge, ReadNode, NodeNode);
        ReadOutEdge = EP(ReadNode->OutEdges);
        ReadOutEdge->Name = Intern(IndexEdgesName(ReadNode));
    }
    ConnectNodeToEdge(Subgraph, GraphNode, SubInEdge);
}
