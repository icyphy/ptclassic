/* $Id$ */

/* Copyright (C) 1990 Regents of the University of California
 * All rights reserved.
 */

/*
 * Routines to parse the flowgraph input and perform some simple 
 * transformations to bring the graph in a simple format
 */

#include "flowgraph.h"
#include "st.h"

/* global functions */
extern ListPointer ListAdd(), ListAppend(), SetAttribute();
extern GraphPointer GraphListAppend();
extern char *Intern();
extern int strcmp();

/* local variables */
static st_table *GraphTable;
static st_table *NodeTable;
static st_table *EdgeTable;

GraphPointer
GraphInputInit()
{
    /* Initializes Table first */
    GraphTable = st_init_table(strcmp, st_strhash);
}

GraphPointer
GraphInput(FlowGraphName, WarnFlag)
char *FlowGraphName;
int WarnFlag;
{
    GraphPointer GraphInputWithCheck();

    return (GraphInputWithCheck(FlowGraphName, WarnFlag));
}

GraphPointer
GraphInputWithInit(FlowGraphName, WarnFlag)
char *FlowGraphName;
int WarnFlag;
{
    GraphPointer GraphInputWithCheck();
    
    GraphInputInit();
    return (GraphInputWithCheck(FlowGraphName, WarnFlag));
}

GraphPointer 
GraphInputWithCheck(FlowGraphName, WarnFlag)
char *FlowGraphName;
int WarnFlag;
{
    GraphPointer Graph, GraphList, Root, 
		 HierarchicalGraphParser(), FindGraphRoot();

    /* Checks first if this graph has not been inputted yet */
    if (st_lookup(GraphTable, FlowGraphName, &Graph) != 0)
	return (Graph);

    GraphList = HierarchicalGraphParser(FlowGraphName);

    if (GraphList == NULL)
	return (NULL);

    /* Determine the root */
    Root = FindGraphRoot(GraphList);

    /*
     * Bring ArgumentLists and AttributeLists in the appropriate formats
     * Using Labeled lists (Label entry)
     */

    for (Graph = GraphList; Graph != NULL; Graph = Graph->Next)
        TransformArgumentLists(Graph);

    /* Fill in default values */
    for (Graph = GraphList; Graph != NULL; Graph = Graph->Next)
        FillDefaultEntries(Graph);

    /*
     * Resolve Node and Edge References : replace name references by pointers
     */

    fatal = FALSE;
    for (Graph = GraphList; Graph != NULL; Graph = Graph->Next) {
        ResolveNodeReferences(Graph);
        ResolveEdgeReferences(Graph);
    }
    if (fatal == TRUE)
        exit (NULL);

    /* Check Network consistency */
    for (Graph = GraphList; Graph != NULL; Graph = Graph->Next)
        CheckConnectivity(Graph);
    if (fatal == TRUE)
        exit (NULL);

    /*
     * Perform Type checks
     */
    /* Is not necessary - should be correct */
    /* FlowTypes(Root, WarnFlag); */

    if (fatal == TRUE)
        exit(NULL);

    return (Root);
}  

GraphPointer
HierarchicalGraphParser(FlowGraphName)
char *FlowGraphName;
{
    GraphPointer GraphList, Graph, GraphParser();
    NodePointer Node;
    char *MasterName;

    GraphList = GraphParser(FlowGraphName, NULL);

    if (GraphList == NULL)
	return (NULL);

    for (Graph = GraphList; Graph != NULL; Graph = Graph->Next) {
	for (Node = Graph->NodeList; Node != NULL; Node = Node->Next) {
	    MasterName = (char *) Node->Master;

	    if (st_lookup(GraphTable, MasterName, &(Node->Master)) == 0) {
		/* Try to find the master using the hyper-search mechanism */
		if (((GraphList = GraphParser(MasterName, GraphList)) 
			    == NULL) ||
		    (st_lookup(GraphTable, MasterName, &(Node->Master)) == 0)) {
		    printmsg("Parser", "Could Not Locate Master Graph %s\n",
			     MasterName);
		    return (NULL);
	        }
	    }

	    Node->Master->Parents = ListAdd(
	        new_list(GraphNode, (pointer) Graph), Node->Master->Parents);
	}
    }
    return (GraphList);
}

GraphPointer
GraphParser(FlowGraphName, GraphList)
char *FlowGraphName;
GraphPointer GraphList;
{

    GraphPointer NewList, Graph, BuildGraphHierarchy(), Parser() ;

    char *Path, *GetPath(), FileName[WORDLENGTH];

    /* Locate position of File */
    sprintf(FileName, "%s.afl", FlowGraphName);

    Path = GetPath("flowgraph.afl", FileName);
    if (Path == NULL) {
        printmsg ("Parser", 
		"Reference to Undefined SubGraph (%s)\n", FlowGraphName);
	return (NULL);
    }

    /* Parse FlowGraphFile and return list of all subgraphs in the graph */
    NewList = Parser(Path);

    if (NewList == NULL || fatal == TRUE)	/* Parse error */
	return (NULL);

    for (Graph = NewList; Graph != NULL; Graph = Graph->Next)
	st_insert(GraphTable, Graph->Name, (pointer) Graph);

    GraphList = GraphListAppend(NewList, GraphList);

    return (GraphList);
}

GraphPointer
FindGraphRoot(GraphList)
GraphPointer GraphList;
{

    GraphPointer Root, Graph;

    /* Locate the Root Graph */
    Root = NULL;
    for (Graph = GraphList; Graph; Graph = Graph->Next) {
	if (Graph->Parents != NULL)
	    continue;
	if (Root == NULL)
	    Root = Graph;
	else {
	    printmsg("Parser", "Multiple Root Graphs (%s and %s)\n",
		Root->Name, Graph->Name);
	    return (NULL);
	}
    }
    return (Root);
}

TransformArgumentLists(Graph)
GraphPointer Graph;
{
    EdgePointer Edge;
    NodePointer Node;

    if (Graph->Arguments != NULL) {
        ListCompact(Graph->Arguments);
        if (Graph->Arguments == NULL) {
                printmsg("parser",
                "fatal Error in Arguments of Graph %s.\n", Graph->Name);
                exit (-1);
        }
    }
    if (Graph->Attributes != NULL) {
        ListCompact(Graph->Attributes);
        if (Graph->Attributes == NULL) {
                printmsg("parser",
                "fatal Error in Attributes of Graph %s.\n", Graph->Name);
                exit (-1);
        }
    }
    if (Graph->Model != NULL) {
        ListCompact(Graph->Model);
        if (Graph->Model == NULL) {
                printmsg("parser",
                "fatal Error in Model of Graph %s.\n", Graph->Name);
                exit (-1);
        }
    }

    for (Edge = Graph->EdgeList; Edge != NULL; Edge = Edge->Next) {
	if (Edge->Arguments != NULL) {
	    ListCompact(Edge->Arguments);
	    if (Edge->Arguments == NULL) {
		printmsg("parser", 
		"fatal Error in Arguments of Edge %s in Graph %s.\n",
		Edge->Name, Graph->Name);
		exit (-1);
	    }
	}
	if (Edge->Attributes != NULL) {
	    ListCompact(Edge->Attributes);
	    if (Edge->Attributes == NULL) {
		printmsg("parser", 
		"fatal Error in Attributes of Edge %s in Graph %s.\n",
		Edge->Name, Graph->Name);
		exit (-1);
	    }
	}
    }
    for (Edge = Graph->ControlList; Edge != NULL; Edge = Edge->Next) {
	if (Edge->Arguments != NULL) {
	    ListCompact(Edge->Arguments);
	    if (Edge->Arguments == NULL) {
		printmsg("parser", 
		"fatal Error in Arguments of Edge %s in Graph %s.\n",
		Edge->Name, Graph->Name);
		exit (-1);
	    }
	}
	if (Edge->Attributes != NULL) {
	    ListCompact(Edge->Attributes);
	    if (Edge->Attributes == NULL) {
		printmsg("parser", 
		"fatal Error in Attributes of Edge %s in Graph %s.\n",
		Edge->Name, Graph->Name);
		exit (-1);
	    }
	}
    }
    for (Node = Graph->NodeList; Node != NULL; Node = Node->Next) {
	if (Node->Attributes != NULL) {
	    ListCompact(Node->Attributes);
	    if (Node->Attributes == NULL) {
		printmsg("parser", 
		"fatal Error in Attributes of Node %s in Graph %s.\n",
		Node->Name, Graph->Name);
		exit (-1);
	    }
	}
	if (Node->Arguments != NULL) {
	    ListCompact(Node->Arguments);
	    if (Node->Arguments == NULL) {
		printmsg("parser", 
		"fatal Error in Arguments of Node %s in Graph %s.\n",
		Node->Name, Graph->Name);
		exit (-1);
	    }
	}
    }
}

ListCompact(Field)
ListPointer Field;
{
    ListPointer Ptr;
    char *UpperToLower();

    if (Field == NULL || Field->Type != ListNode)
	return;
    Ptr = (ListPointer) Field->Entry;
    if (Ptr->Type == CharNode && Ptr->Next != NULL && Ptr->Next->Next == NULL) {
	/* This is a label with extra data */
	Field->Label = Intern((char *) Ptr->Entry);
	Field->Entry = Ptr->Next->Entry;
	Field->Type = Ptr->Next->Type;
	if (Field->Type == ListNode)
	    ListCompact(Field->Entry);
	free_list(Ptr->Next);
        free_list(Ptr);
    }
    ListCompact (Field->Next);
}

/*
 * Routines to resolve references to Nodes or Edges
 * In order to speed up this operations for Large Graphs, we use a hash table.
 */

ResolveNodeReferences(Graph)
GraphPointer Graph;
{
    EdgePointer Edge;
    NodePointer Node;

    NodeTable = st_init_table(strcmp, st_strhash);

    for (Node = Graph->NodeList; Node != NULL; Node = Node->Next)
	st_insert(NodeTable, Node->Name, (pointer) Node);

    for (Edge = Graph->EdgeList; Edge != NULL; Edge = Edge->Next)
	ResolveNodeReferencesInEdge(Edge, Graph);

    for (Edge = Graph->ControlList; Edge != NULL; Edge = Edge->Next)
	ResolveNodeReferencesInEdge(Edge, Graph);

    st_free_table(NodeTable);

    return (fatal);
}

ResolveNodeReferencesInEdge(Edge, Graph)
EdgePointer Edge;
GraphPointer Graph;
{
    ListPointer Ptr;

    for (Ptr = Edge->InNodes; Ptr; Ptr = Ptr->Next)
	ResolveNodeReference(Ptr, Edge, Graph, "IN_NODES");
    for (Ptr = Edge->OutNodes; Ptr; Ptr = Ptr->Next)
	ResolveNodeReference(Ptr, Edge, Graph, "OUT_NODES");

}

ResolveNodeReference(NodePtr, Edge, Graph, ListName)
ListPointer NodePtr;
EdgePointer Edge;
GraphPointer Graph;
char *ListName;
{
    NodePointer Node;

    if (NodePtr->Type != CharNode) {
        printmsg("Parser", 
            "Illegal elements in %s list of Edge %s in Graph %s.\n",
	    ListName, Edge->Name, Graph->Name);
	fatal = TRUE;
	return;
    }

    if (!strcmp( (char *) NodePtr->Entry, "parent") ||
      	!strcmp( (char *) NodePtr->Entry, Graph->Name) ||
	!strcmp( (char *) NodePtr->Entry, "PARENT")) {
	NodePtr->Type = GraphNode;
	NodePtr->Entry = (pointer) Graph;
	return;
    }
    if (st_lookup(NodeTable, (char *) NodePtr->Entry, &Node) == 0) {
	printmsg("Parser",
	    "Reference to non-existing node (%s).\n", (char *) NodePtr->Entry);
	printmsg(NULL,
	    "(%s List of Edge %s in Graph %s)\n\n", 
	    ListName, Edge->Name, Graph->Name);
	fatal = TRUE;
	return;
    }

    NodePtr->Type = NodeNode;
    NodePtr->Entry = (pointer) Node;
}

ResolveEdgeReferences(Graph)
GraphPointer Graph;
{
    EdgePointer Edge, EquivalentEdge;
    NodePointer Node;
    ListPointer Ptr;
    char *Equivalence;

    EdgeTable = st_init_table(strcmp, st_strhash);

    for (Edge = Graph->EdgeList; Edge != NULL; Edge = Edge->Next)
	st_insert(EdgeTable, Edge->Name, (pointer) Edge);
    for (Edge = Graph->ControlList; Edge != NULL; Edge = Edge->Next)
	st_insert(EdgeTable, Edge->Name, (pointer) Edge);

    for (Node = Graph->NodeList; Node != NULL; Node = Node->Next)
	ResolveEdgeReferencesInNode(Node, Graph);

    for (Ptr = Graph->InEdges; Ptr != NULL; Ptr = Ptr->Next)
	ResolveEdgeReference(Ptr, NULL, Graph, "IN_EDGES");

    for (Ptr = Graph->OutEdges; Ptr != NULL; Ptr = Ptr->Next)
	ResolveEdgeReference(Ptr, NULL, Graph, "OUT_EDGES");

    for (Ptr = Graph->InControl; Ptr != NULL; Ptr = Ptr->Next)
	ResolveEdgeReference(Ptr, NULL, Graph, "IN_CONTROL");

    for (Ptr = Graph->OutControl; Ptr != NULL; Ptr = Ptr->Next)
	ResolveEdgeReference(Ptr, NULL, Graph, "OUT_CONTROL");

    for (Edge = Graph->EdgeList; Edge != NULL; Edge = Edge->Next) {
	Equivalence = (char *) GetAttribute(Edge->Attributes, "equivalent");
	if (Equivalence != NULL) { 
	    st_lookup(EdgeTable, Equivalence, &EquivalentEdge);
	    Edge->Attributes = SetAttribute("equivalent", EdgeNode,
	        EquivalentEdge, Edge->Attributes);
	}
    }

    st_free_table(EdgeTable);

    return (fatal);
}


ResolveEdgeReferencesInNode(Node, Graph)
NodePointer Node;
GraphPointer Graph;
{
    ListPointer Ptr;

    for (Ptr = Node->InEdges; Ptr; Ptr = Ptr->Next)
	ResolveEdgeReference(Ptr, Node, Graph, "IN_EDGES");
    for (Ptr = Node->OutEdges; Ptr; Ptr = Ptr->Next)
	ResolveEdgeReference(Ptr, Node, Graph, "OUT_EDGES");
    for (Ptr = Node->InControl; Ptr; Ptr = Ptr->Next)
	ResolveEdgeReference(Ptr, Node, Graph, "IN_CONTROL");
    for (Ptr = Node->OutControl; Ptr; Ptr = Ptr->Next)
	ResolveEdgeReference(Ptr, Node, Graph, "OUT_CONTROL");
}

ResolveEdgeReference(EdgePtr, Node, Graph, ListName)
ListPointer EdgePtr;
NodePointer Node;
GraphPointer Graph;
char *ListName;
{
    EdgePointer Edge;

    if (EdgePtr->Type == NullNode) 
	return;

    if (EdgePtr->Type != CharNode) {
	printmsg("Parser", 
	    "Illegal elements in %s list of Node %s in Graph %s.\n",
	    ListName, (Node == NULL) ? Graph->Name : Node->Name, Graph->Name);
	fatal = TRUE;
	return;
    }

    if (st_lookup(EdgeTable, (char *) EdgePtr->Entry, &Edge) == 0) {
	printmsg("Parser",
	    "Reference to non-existing edge (%s).\n", (char *) EdgePtr->Entry);
	printmsg(NULL,
	    "(%s List of Node %s in Graph %s)\n\n", 
	    ListName, (Node == NULL) ? Graph->Name : Node->Name, Graph->Name);
	fatal = TRUE;
	return;
    }

    EdgePtr->Type = EdgeNode;
    EdgePtr->Entry = (pointer) Edge;
}

FillDefaultEntries(Graph)
GraphPointer Graph;
{
    EdgePointer Edge;

    if (Graph->Class == NULL) 
	Graph->Class = (Graph->NodeList == NULL) ?
	    Intern(LEAF_NODE) : Intern(HIERARCHY_NODE);

    for (Edge = Graph->EdgeList; Edge != NULL; Edge = Edge->Next)
	if (Edge->Class == NULL)
	    Edge->Class = Intern(DATA_EDGE);
    for (Edge = Graph->ControlList; Edge != NULL; Edge = Edge->Next)
	if (Edge->Class == NULL)
	    Edge->Class = Intern(CONTROL_EDGE);
}

CheckConnectivity(Graph)
GraphPointer Graph;
{
    NodePointer Node;
    EdgePointer Edge;
    ListPointer Ptr, Ptr2;

    /* 
     * Check if the defined flowgraph network  is consistent (sources and
     * destinations of edges and nodes
     */

    /* Check for the consistency of the Nodes */

    for (Node = Graph->NodeList; Node != NULL; Node = Node->Next) {
	for (Ptr = Node->InEdges; Ptr != NULL; Ptr = Ptr->Next) {
	    if (Ptr->Type != EdgeNode) 
		continue;
	    Edge = (EdgePointer) Ptr->Entry;
	    for (Ptr2 = Edge->OutNodes; Ptr2 != NULL; Ptr2 = Ptr2->Next)
		if (Ptr2->Type == NodeNode && (NodePointer) Ptr2->Entry == Node)
		    break;
	    if (Ptr2 == NULL) {
		printmsg("Parser",
		"Node %s does not appear in out-list of Edge %s\n",
		Node->Name, Edge->Name);
		fatal = TRUE;
	    }
	}
	for (Ptr = Node->OutEdges; Ptr != NULL; Ptr = Ptr->Next) {
	    if (Ptr->Type != EdgeNode) 
		continue;
	    Edge = (EdgePointer) Ptr->Entry;
	    for (Ptr2 = Edge->InNodes; Ptr2 != NULL; Ptr2 = Ptr2->Next)
		if (Ptr2->Type == NodeNode && (NodePointer) Ptr2->Entry == Node)
		    break;
	    if (Ptr2 == NULL) {
		printmsg("Parser",
		"Node %s does not appear in in-list of Edge %s\n",
		Node->Name, Edge->Name);
		fatal = TRUE;
	    }
	}
	for (Ptr = Node->InControl; Ptr != NULL; Ptr = Ptr->Next) {
	    if (Ptr->Type != EdgeNode) 
		continue;
	    Edge = (EdgePointer) Ptr->Entry;
	    for (Ptr2 = Edge->OutNodes; Ptr2 != NULL; Ptr2 = Ptr2->Next)
		if (Ptr2->Type == NodeNode && (NodePointer) Ptr2->Entry == Node)
		    break;
	    if (Ptr2 == NULL) {
		printmsg("Parser",
		"Node %s does not appear in in-control-list of Edge %s\n",
		Node->Name, Edge->Name);
		fatal = TRUE;
	    }
	}
	for (Ptr = Node->OutControl; Ptr != NULL; Ptr = Ptr->Next) {
	    if (Ptr->Type != EdgeNode) 
		continue;
	    Edge = (EdgePointer) Ptr->Entry;
	    for (Ptr2 = Edge->InNodes; Ptr2 != NULL; Ptr2 = Ptr2->Next)
		if (Ptr2->Type == NodeNode && (NodePointer) Ptr2->Entry == Node)
		    break;
	    if (Ptr2 == NULL) {
		printmsg("Parser",
		"Node %s does not appear in out-control-list of Edge %s\n",
		Node->Name, Edge->Name);
		fatal = TRUE;
	    }
	}
    }

    /* Check for the consistency of the edges */
    for (Edge = Graph->EdgeList; Edge != NULL; Edge = Edge->Next) {
	for (Ptr = Edge->InNodes; Ptr != NULL; Ptr = Ptr->Next) {
	    if (Ptr->Type != NodeNode) 
		continue;
	    Node = (NodePointer) Ptr->Entry;
	    for (Ptr2 = Node->OutEdges; Ptr2 != NULL; Ptr2 = Ptr2->Next)
		if (Ptr2->Type == EdgeNode && (EdgePointer) Ptr2->Entry == Edge)
		    break;
	    if (Ptr2 == NULL) {
		printmsg("Parser",
		"Edge %s does not appear in out-list of Node %s\n",
		Edge->Name, Node->Name);
		fatal = TRUE;
	    }
	}
	for (Ptr = Edge->OutNodes; Ptr != NULL; Ptr = Ptr->Next) {
	    if (Ptr->Type != NodeNode) 
		continue;
	    Node = (NodePointer) Ptr->Entry;
	    for (Ptr2 = Node->InEdges; Ptr2 != NULL; Ptr2 = Ptr2->Next)
		if (Ptr2->Type == EdgeNode && (EdgePointer) Ptr2->Entry == Edge)
		    break;
	    if (Ptr2 == NULL) {
		printmsg("Parser",
		"Edge %s does not appear in in-list of Node %s\n",
		Edge->Name, Node->Name);
		fatal = TRUE;
	    }
	}
    }

    /* Check for the consistency of the control-edges */
    for (Edge = Graph->ControlList; Edge != NULL; Edge = Edge->Next) {
	for (Ptr = Edge->InNodes; Ptr != NULL; Ptr = Ptr->Next) {
	    if (Ptr->Type != NodeNode) 
		continue;
	    Node = (NodePointer) Ptr->Entry;
	    for (Ptr2 = Node->OutControl; Ptr2 != NULL; Ptr2 = Ptr2->Next)
		if (Ptr2->Type == EdgeNode && (EdgePointer) Ptr2->Entry == Edge)
		    break;
	    if (Ptr2 == NULL) {
		printmsg("Parser",
		"Edge %s does not appear in out-control-list of Node %s\n",
		Edge->Name, Node->Name);
		fatal = TRUE;
	    }
	}
	for (Ptr = Edge->OutNodes; Ptr != NULL; Ptr = Ptr->Next) {
	    if (Ptr->Type != NodeNode) 
		continue;
	    Node = (NodePointer) Ptr->Entry;
	    for (Ptr2 = Node->InControl; Ptr2 != NULL; Ptr2 = Ptr2->Next)
		if (Ptr2->Type == EdgeNode && (EdgePointer) Ptr2->Entry == Edge)
		    break;
	    if (Ptr2 == NULL) {
		printmsg("Parser",
		"Edge %s does not appear in in-control-list of Node %s\n",
		Edge->Name, Node->Name);
		fatal = TRUE;
	    }
	}
    }
}

