/* $Id$ */
#include "dag.h"
#include "st.h"

/* Global Functions */
extern EdgePointer new_edge(), EdgeListAppend(), RemoveFromEdgeList();
extern ListPointer new_list(), ListAppend(), ListAdd(), SetAttribute();
extern char *UniqueName(), *Intern(), *GetFunction();
extern NodePointer RemoveFromNodeList();

static st_table *ReferTable;
extern int strcmp();
extern int castFlag;

int
TransformGraph(Graph)
GraphPointer Graph;
{
    /* Initializes Table */
    GraphInputInit();

    FlattenGraph(Graph, TRUE);

    /* Sub-sets Silage to be consistent with the current set of HYPER tools */
    if (CheckGraphValidity(Graph) == FALSE) {
	printmsg(NULL, 
	    "Please Contact HYPER Support (hyper@zabriskie.berkeley.edu)\n");
	exit (-1);
    }

    RemoveConstantNodes(Graph);
    RemoveEqualNodes(Graph, TRUE);
    MarkIteratorEdges(Graph);

    RemoveIdentities(Graph);
    CollapseNegateNodes(Graph);
    Conditionals(Graph);

    /* Loops and delays */
    TransformIONodes(Graph);
    TransformIteratorNodes(Graph); 
    InitializeDelays(Graph);

    if (debugFlag == TRUE) {
       DumpFlowGraph(Graph, FALSE);
       exit (-1);
    }

    /* Resolve undefined edges and types before proceeding */
    if (FlowTypes(Graph, TRUE) == TRUE) { 
       DumpFlowGraph(Graph, FALSE);
       exit (-1);
    }
    
    /* Remaining Transforms.. */
    TransformCastNodes(Graph, castFlag);
    ManifestExpressions(Graph);
    CollapseConstantEdges(Graph);
    CommonSubExpression(Graph);
    DeadCodeElimination(Graph);

    PerformGraphCleanUp(Graph);

    ClusterNodes(Graph);

    return (TRUE);

}

RemoveAllConstNodes(root)
GraphPointer root;
{
    ReferTable = st_init_table(strcmp, st_strhash);
    RemoveHierConstantNodes(root);
    st_free_table(ReferTable);
}

RemoveHierConstantNodes(Graph)
GraphPointer Graph;
{
    NodePointer Node, Next;
    EdgePointer Edge;
    GraphPointer SubGraph;
    char *Value;
    char *Function;

    for (Node = Graph->NodeList; Node != NULL; Node = Next) {
        Next = Node->Next;

	if (IsHierarchy(Node)) {
	    SubGraph = (GraphPointer)Node->Master;
            if (st_lookup(ReferTable, SubGraph->Name, NULL) == 0) {
                st_insert(ReferTable, SubGraph->Name, 0);
     		RemoveHierConstantNodes(SubGraph);
            }
        } 

        Function = GetFunction(Node);
        /* constant node */
        if (strcmp(Function, "constant"))
            continue;

        Graph->NodeList = RemoveFromNodeList(Node, Graph->NodeList);

        Edge = EP(Node->OutEdges);
        Edge->Class = Intern("constant");
        Value = (char *)GetAttribute(Node->Arguments, "value");
        Edge->Attributes=SetAttribute("value",CharNode,Value,Edge->Attributes);
        ClearList(Edge->InNodes);
        Edge->InNodes = NULL;
        ClearNode(Node);
    }
}

RemoveAllConstCastNodes(root)
GraphPointer root;
{
    ReferTable = st_init_table(strcmp, st_strhash);
    RemoveConstCastNodes(root);
    st_free_table(ReferTable);
}

RemoveConstCastNodes(Graph)
GraphPointer Graph;
{
    NodePointer Node, Next;
    GraphPointer SubGraph;
    EdgePointer InEdge, OutEdge;

    char *Function;
    char *OutputType;

    for (Node = Graph->NodeList; Node != NULL; Node = Next) {
        Next = Node->Next;

        if (IsHierarchyNode(Node)) {
            SubGraph = (GraphPointer)Node->Master;
            if (st_lookup(ReferTable, SubGraph->Name, NULL) == 0) {
                st_insert(ReferTable, SubGraph->Name, 0);
                RemoveConstCastNodes(Node->Master);
            }
        }

        Function = GetFunction(Node);

        /* cast node */
        if (strcmp(Function, "cast"))
            continue;

        InEdge = EP(Node->InEdges);
	if (!IsConstant(InEdge))
	    continue;
        OutEdge = EP(Node->OutEdges);

        OutputType = (char *) GetAttribute(OutEdge->Arguments, "type");
        InEdge->Arguments = SetAttribute("type", CharNode, (pointer)OutputType,
				InEdge->Arguments);
        /* Remove the node */
        ShortCircuitNode(Graph, Node);
    }
}

RemoveAllCastNodes(root)
GraphPointer root;
{
    ReferTable = st_init_table(strcmp, st_strhash);
    RemoveCastNodes(root);
    st_free_table(ReferTable);
}

RemoveCastNodes(Graph)
GraphPointer Graph;
{
    NodePointer Node, Next;
    GraphPointer SubGraph;
    EdgePointer InEdge, OutEdge;

    char *Function;
    char *InputType, *OutputType;
    int w1, w2, d1, d2;

    for (Node = Graph->NodeList; Node != NULL; Node = Next) {
        Next = Node->Next;

        if (IsHierarchyNode(Node)) {
            SubGraph = (GraphPointer)Node->Master;
            if (st_lookup(ReferTable, SubGraph->Name, NULL) == 0) {
                st_insert(ReferTable, SubGraph->Name, 0);
                RemoveCastNodes(Node->Master);
            }
        }

        Function = GetFunction(Node);

        /* cast node */
        if (strcmp(Function, "cast"))
            continue;

        InEdge = EP(Node->InEdges);
        OutEdge = EP(Node->OutEdges);

        InputType = (char *) GetAttribute(InEdge->Arguments, "type");
        sscanf(InputType, "fix<%d,%d>", &w1, &d1);
        OutputType = (char *) GetAttribute(OutEdge->Arguments, "type");
        sscanf(OutputType, "fix<%d,%d>", &w2, &d2);

        if (w1 == w2 && d1 == d2) {
            /* Remove the node */
            ShortCircuitNode(Graph, Node);
	}
    }
}

/* Here we used DisconnectNode() without connecting the Node->InEdges, OutEdges
 * to the parent Graph.  The true effect is we don't want these edges to end
 * up in the input and output parameters of the graph for code generation.
 * It is not clear, however, what is the effects of having them unconnected */

RemoveAllDelayNodes(root)
GraphPointer root;
{
    ReferTable = st_init_table(strcmp, st_strhash);
    RemoveDelayNodes(root);
    st_free_table(ReferTable);
}

RemoveDelayNodes(Graph)
GraphPointer Graph;
{
    NodePointer Node, RemoveFromNodeList();
    GraphPointer SubGraph;

    for (Node = Graph->NodeList; Node != NULL; Node = Node->Next) {
   	if (IsHierarchy(Node)) {
            SubGraph = (GraphPointer)Node->Master;
            if (st_lookup(ReferTable, SubGraph->Name, NULL) == 0) {
                st_insert(ReferTable, SubGraph->Name, 0);
                RemoveDelayNodes(SubGraph);
            }
        }

    	if (IsDelayNode(Node)) {
	    DisconnectNode(Node, Graph, FALSE);  /* don't connect to Graph */
	    Graph->NodeList = RemoveFromNodeList(Node, Graph->NodeList);
	}
    }
}

RemoveDelaysInGraph(Graph)
GraphPointer Graph;
{
    NodePointer Node, RemoveFromNodeList();

    for (Node = Graph->NodeList; Node != NULL; Node = Node->Next) {
        if (IsDelayNode(Node)) {
            DisconnectNode(Node, Graph, FALSE);  /* don't connect to Graph */
            Graph->NodeList = RemoveFromNodeList(Node, Graph->NodeList);
        }
    }
}

RemoveLpDelaysInGraph(Graph)
GraphPointer Graph;
{
    NodePointer Node, RemoveFromNodeList();

    for (Node = Graph->NodeList; Node != NULL; Node = Node->Next) {
        if (IsLpDelayNode(Node)) {
            DisconnectNode(Node, Graph, FALSE);  /* don't connect to Graph */
            Graph->NodeList = RemoveFromNodeList(Node, Graph->NodeList);
        }
    }
}

ListPointer
RemoveEdgeFromList(Edge, List)
EdgePointer Edge;
ListPointer List;
{
    ListPointer Ptr, Last, val;

    for (Ptr = List, Last = NULL; Ptr != NULL; Last = Ptr, Ptr = Ptr->Next)
        if (EP(Ptr) == Edge)
            break;
    if (Ptr == NULL)
        return (List);
    else if (Last == NULL)
        val = Ptr->Next;
    else {
        Last->Next = Ptr->Next;
        val = List;
    }
    free_list(Ptr);
    return (val);
}

EliminateNodeAndConnections(Graph, Node)
GraphPointer Graph;
NodePointer Node;
{
    /*
     * Eliminate all I/O edges if they only interface with this node
     */

    EdgePointer InEdge, OutEdge;
    ListPointer Ptr, Next;

    for (Ptr = Node->InEdges; Ptr; Ptr = Next) {
        Next = Ptr->Next;
        if (Ptr->Type != EdgeNode)
            continue;
        InEdge = (EdgePointer) Ptr->Entry;
	if (ListCount(InEdge->OutNodes) == 1)
	    EliminateEdge(Graph, InEdge);
    }

    for (Ptr = Node->InControl; Ptr; Ptr = Next) {
        Next = Ptr->Next;
        if (Ptr->Type != EdgeNode)
            continue;
        InEdge = (EdgePointer) Ptr->Entry;
	if (ListCount(InEdge->OutNodes) == 1)
	    EliminateEdge(Graph, InEdge);
   }

    for (Ptr = Node->OutEdges; Ptr; Ptr = Next) {
        Next = Ptr->Next;
        if (Ptr->Type != EdgeNode)
            continue;
        OutEdge = (EdgePointer) Ptr->Entry;
	if (ListCount(OutEdge->InNodes) == 1)
	    EliminateEdge(Graph, OutEdge);
    }

    for (Ptr = Node->OutControl; Ptr; Ptr = Next) {
        Next = Ptr->Next;
        if (Ptr->Type != EdgeNode)
            continue;
        OutEdge = (EdgePointer) Ptr->Entry;
	if (ListCount(OutEdge->InNodes) == 1)
	    EliminateEdge(Graph, OutEdge);
    }

    EliminateNode(Graph, Node);
}

/**************************************************************************
  AddIONodes() -- Removes IO edges from the Root and replace with Input and
  Output nodes.
****************************************************************************/
static GraphPointer InputGraph, OutputGraph;

AddIONodes(Graph)
GraphPointer Graph;
{
    GraphPointer GraphInputWithCheck();

    InputGraph = GraphInputWithCheck(INPUT, FALSE);
    OutputGraph = GraphInputWithCheck(OUTPUT, FALSE);

    AddInputNodes(Graph, Graph->InEdges);
    AddInputNodes(Graph, Graph->InControl);
    AddOutputNodes(Graph, Graph->OutEdges);
    AddOutputNodes(Graph, Graph->OutControl);
}

AddInputNodes(Graph, InList)
GraphPointer Graph;
ListPointer InList;
{
    ListPointer Ptr, PtrNext;
    EdgePointer Edge;
    NodePointer Node, CreateNewNode();
    char *TmpNodeName();

    for (Ptr = InList; Ptr != NULL; Ptr = PtrNext) {
        PtrNext = Ptr->Next;
        if (Ptr->Type != EdgeNode) continue;
        Edge = EP(Ptr);
        Node = CreateNewNode(Graph, TmpNodeName(), Intern("data"), InputGraph);
        DisconnectNodeFromEdge(Graph, GraphNode, Edge);
        ConnectNodeToEdge(Node, NodeNode, Edge);
    }
}

AddOutputNodes(Graph, OutList)
GraphPointer Graph;
ListPointer OutList;
{
    ListPointer Ptr, PtrNext;
    EdgePointer Edge;
    NodePointer Node, CreateNewNode();
    char *TmpNodeName();

    for (Ptr = OutList; Ptr != NULL; Ptr = PtrNext) {
        PtrNext = Ptr->Next;
        if (Ptr->Type != EdgeNode) continue;
        Edge = EP(Ptr);
        Node = CreateNewNode(Graph, TmpNodeName(), Intern("data"), OutputGraph);
        DisconnectEdgeFromNode(Edge, Graph, GraphNode);
        ConnectEdgeToNode(Edge, Node, NodeNode);
    }
}

/*
 * RemoveIONodes() -- Removes Input and Output nodes from the Root Graph and
 * make them IO edges to the Root Graph.
 */
RemoveIONodes(Graph)
GraphPointer Graph;
{
     RemoveInputNodes(Graph);
     RemoveOutputNodes(Graph);
}

RemoveInputNodes(Graph)
GraphPointer Graph;
{
    NodePointer Node, NodeNext, RemoveFromNodeList();

    for (Node = Graph->NodeList; Node != NULL; Node = NodeNext) {
        NodeNext = Node->Next;
        if (IsInputNode(Node)) {
            DisconnectNode(Node, Graph, TRUE);
            Graph->NodeList = RemoveFromNodeList(Node, Graph->NodeList);
            ClearNode(Node);
        }
    }
}

RemoveOutputNodes(Graph)
GraphPointer Graph;
{
    NodePointer Node, NodeNext, RemoveFromNodeList();

    for (Node = Graph->NodeList; Node != NULL; Node = NodeNext) {
        NodeNext = Node->Next;
        if (IsOutputNode(Node)) {
            DisconnectNode(Node, Graph, TRUE);
            Graph->NodeList = RemoveFromNodeList(Node, Graph->NodeList);
            ClearNode(Node);
        }
    }
}
