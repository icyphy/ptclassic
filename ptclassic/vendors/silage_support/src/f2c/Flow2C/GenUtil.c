/* $Id$ */
#include "codegen.h"
#include "st.h"

char *tmalloc (s)
   int s;
{
   char *a, *malloc ();
   a = malloc (s);
   if (a == NULL)
      fprintf (stderr, "Can't allocate enough memory.\n");
   else
      return (a);
}

pointer NewGraphExt()
{
    eGraphPointer Ext;

    Ext = (eGraphPointer) malloc (sizeof (eGraph));
    Ext->visit = UNVISITED;
    Ext->HasDelay = false;
    Ext->ListOfFuncApps = NULL;
    Ext->ListOfDelays = NULL;
    Ext->ListOfLoopDelays = NULL;
    Ext->NumberOfProcesses = -1;
    return((pointer)Ext);
}

pointer NewNodeExt()
{
    eNodePointer Ext;

    Ext = (eNodePointer) malloc (sizeof (eNode));
    Ext->visit = UNVISITED;
    return((pointer)Ext);
}

pointer NewEdgeExt()
{
    eEdgePointer Ext;

    Ext = (eEdgePointer) malloc (sizeof (eEdge));
    Ext->visit = UNVISITED;
    return((pointer)Ext);
}

char *RemoveVersion(name)
char *name;
{
    char str[STRSIZE], *p1, *p2;
    char *Intern();

    for (p1 = name, p2 = str; *p1 != '\0' && *p1 != '#'; p1++, p2++)
	*p2 = *p1;
    *p2 = '\0';
    return(Intern(str));
}

int 
GetVersion(name)
char *name;
{
    char *Ptr, *rindex();

    Ptr = rindex(name, '#');

    if (Ptr == NULL)
	return (-1);
    else
	return(atoi(++Ptr));
}

EnforceLegalName(Edge)
EdgePointer Edge;
{
    char *p1;
    char *UniqueName();

    for (p1=Edge->Name;*p1 != '\0' && *p1 != '#' && *p1 != '@';p1++);
    if (*p1 != '\0')
	Edge->Name = UniqueName("initedge");
}

#define NOT_CONSIDERED -1

TopologicalOrder(Graph)
GraphPointer Graph;
{
    /* Performs a topological ordering on the graph */
    /* in a hierarchical fashion		    */ 

    int NrOfNodes, i;
    NodePointer Node, *OrderList;
    int Counter;

    if (Graph == NULL) return;

    /* Clear all the nodes */
    /* Count them at the same time */

    NrOfNodes = 0;

    for (Node = Graph->NodeList; Node != NULL; Node = Node->Next) {
	    if (IsDelayNode(Node))
	        NE(Node)->visit = NOT_CONSIDERED;
	    else {
	        NrOfNodes++;
	        NE(Node)->visit = UNVISITED;
	    }
    }

    /* Reserve pointer list */
    OrderList = (NodePointer *) calloc (NrOfNodes, sizeof(NodePointer));

    /* Clear the counters    */
    Counter = 0;

    /* Perform the ordering */
    for (Node = Graph->NodeList; Node != NULL; Node = Node->Next)
	    OrderVertex(Node, &Counter, OrderList);

    StoreInPlace(Graph, OrderList, NrOfNodes);
    free(OrderList);
}

OrderVertex(Node, Counter, OrderList)
NodePointer Node;
int *Counter;
NodePointer *OrderList;
{
    ListPointer Ptr1, Ptr2;
    EdgePointer Edge;

    if (NE(Node)->visit != UNVISITED)
	return;

    NE(Node)->visit = VISITED;

    /* Run over the precedence fanout */
    for (Ptr1 = Node->OutEdges; Ptr1 != NULL; Ptr1 = Ptr1->Next) {
	if (Ptr1->Type != EdgeNode) continue;
	Edge = EP(Ptr1);
	for (Ptr2 = Edge->OutNodes; Ptr2 != NULL; Ptr2 = Ptr2->Next)
	    if (Ptr2->Type == NodeNode)
		OrderVertex(NP(Ptr2), Counter, OrderList);
    }

    /* Run over the control fanout */
    for (Ptr1 = Node->OutControl; Ptr1 != NULL; Ptr1 = Ptr1->Next) {
	if (Ptr1->Type != EdgeNode) continue;
	Edge = EP(Ptr1);
	for (Ptr2 = Edge->OutNodes; Ptr2 != NULL; Ptr2 = Ptr2->Next)
	    if (Ptr2->Type == NodeNode)
		OrderVertex(NP(Ptr2), Counter, OrderList);
    }

    /* Number the node */
    OrderList[(*Counter)++] = Node;
}

bool IsReadOut(edge)
EdgePointer edge;
{
    NodePointer Innode;

    if (edge->InNodes != NULL && edge->InNodes->Type == NodeNode) {
        Innode = NP(edge->InNodes);
        if (!strcmp(Innode->Master->Name, READ)) return(true);
    }
    return(false);
}

bool IsInput(edge)
EdgePointer edge;
{
    GraphPointer graph;
    register ListPointer ptr;

    graph = EE(edge)->graph;
    for (ptr = graph->InEdges; ptr != NULL; ptr = ptr->Next) {
        if (edge == EP(ptr)) return(true);
    }
    for (ptr = graph->InControl; ptr != NULL; ptr = ptr->Next) {
        if (edge == EP(ptr)) return(true);
    }
    return(false);
}	

bool IsFuncInput(edge)
EdgePointer edge;
{
    if (HasAttribute(edge->Attributes, "input"))
	return(true);
    else
	return(false);
}	

bool IsOutput(edge)
EdgePointer edge;
{
    GraphPointer graph;
    register ListPointer ptr;

    graph = EE(edge)->graph;
    for (ptr = graph->OutEdges; ptr != NULL; ptr = ptr->Next) {
	if (edge == EP(ptr)) return(true);
    }
    for (ptr = graph->OutControl; ptr != NULL; ptr = ptr->Next) {
	if (edge == EP(ptr)) return(true);
    }
    return(false);
}

bool IsFuncOutput(edge)
EdgePointer edge;
{
    if (HasAttribute(edge->Attributes, "output"))
	return(true);
    else
	return(false);
}

bool IsLoopDelayIO(edge)
EdgePointer edge;
{
    ListPointer K;
    NodePointer Node;
    bool IsLpDelayNode();

    for (K = edge->InNodes; K != NULL; K = K->Next) {
	if (K->Type != NodeNode) continue;
	Node = NP(K);
	if (IsLpDelayNode(Node) && NodeIONumber(Node, edge, OUT) ==0)
	    return(true);
    }
    for (K = edge->OutNodes; K != NULL; K = K->Next) {
	if (K->Type != NodeNode) continue;
	Node = NP(K);
	if (IsLpDelayNode(Node) && NodeIONumber(Node, edge, IN) == 0)
	    return(true);
    }
    return(false);
}

bool IsDelayOut(edge)
EdgePointer edge;
{
    if (HasAttribute(edge->Attributes, "delay") ||
            HasAttribute(edge->Attributes, "loopdelay"))
                return(true);
        return(false);
}

bool IsDelayIO(edge)
EdgePointer edge;
{

    if (HasAttribute(edge->Attributes, "Maxdelay") ||
		HasAttribute(edge->Attributes, "delay"))
	return(true);
    return(false);
}

bool IsSampDelayIO(edge)
EdgePointer edge;
{
    ListPointer K;
    NodePointer Node;
    bool IsSampDelayNode();

    for (K = edge->InNodes; K != NULL; K = K->Next) {
	if (K->Type != NodeNode) continue;
	Node = NP(K);
	if (IsSampDelayNode(Node) && NodeIONumber(Node, edge, OUT) == 0)
	    return(true);
    }
    for (K = edge->OutNodes; K != NULL; K = K->Next) {
	if (K->Type != NodeNode) continue;
	Node = NP(K);
	if (IsSampDelayNode(Node) && NodeIONumber(Node, edge, IN) == 0)
	    return(true);
    }
    return(false);
}

bool IsDeclaredName(Name)
char *Name;
{
    extern int indexlevel;
    extern st_table *Edgetable[];
    int i;

    for (i = indexlevel; i >= 0; i--) {
        if (st_lookup(Edgetable[i], Name, NULL) != 0)
            return(true);
    }
    return(false);
}

bool IsDeclaredEdge(Edge)
EdgePointer Edge;
{
    extern int indexlevel;
    extern st_table *Edgetable[];
    char *DeclName();
    int i;

    if (IsReadOut(Edge) || IsDelayIO(Edge)) return(true);

    for (i = indexlevel; i >= 0; i--) {
        if (st_lookup(Edgetable[i], DeclName(Edge), NULL) != 0)
            return(true);
    }
    return(false);
}

ReduceLocalVars(Graph)
GraphPointer Graph;
{
    NodePointer Node;
    EdgePointer input, output, InputSearch();
    bool Reducible();

    if (Graph == NULL) return;
    for (Node = Graph->NodeList; Node != NULL; Node = Node->Next) {
	if (Reducible(Node) && ((input = InputSearch(Node)) != NULL)) {
	    output = EP(Node->OutEdges);
	    output->Name = input->Name;
	}
    }
    ReduceLocalVars(Graph->Next);
}

/*
 * Node is reducible if NOT a Read node, have only 1 output which is not
 * an output or delayout edge
 */
bool Reducible(node)
NodePointer node;
{
    EdgePointer outedge;

    if (IsReadNode(node) || IsDelayNode(node) || 
		node->OutEdges->Next != NULL) return(false);
    outedge = EP(node->OutEdges);   
    if (IsOutput(outedge)) return(false);
    return(true);
}

/*
 * Candidate input edge to propagate name must NOT be a Read outedge, and
 * must only have 1 outnode.
 */
EdgePointer InputSearch(node)
NodePointer node;
{
    ListPointer ptr;
    EdgePointer inedge;
    bool IsReadOut();

    for (ptr = node->InEdges; ptr != NULL; ptr = ptr->Next) {
	inedge = EP(ptr);
        if (!IsReadOut(inedge) && inedge->OutNodes->Next == NULL)
	    return(inedge);
    }
    return(NULL);
}

ExpandFuncNodesWithLpDelay(Graph, parent_node, parent_graph)
GraphPointer Graph;
NodePointer parent_node;
GraphPointer parent_graph;
{
    NodePointer Node, NodeNext;

    for (Node = Graph->NodeList; Node != NULL; Node = NodeNext) {
	NodeNext = Node->Next;
	if (IsHierarchyNode(Node))
	    ExpandFuncNodesWithLpDelay(Node->Master, Node, Graph);
        if (IsLpDelayNode(Node)) {
	    if (parent_node != NULL && IsFuncNode(parent_node))
	        ExpandFuncCall(parent_node, parent_graph);
	}
    }
}

ExpandFuncCall(Node, parent_graph)
NodePointer Node;
GraphPointer parent_graph;
{
    GraphPointer SubGraph, CopyGraph();
    NodePointer NodeListAppend();
    EdgePointer EdgeListAppend();

    /* Get Subgraph */
    SubGraph = CopyGraph(Node->Master);
    CopyEdgeName(SubGraph, Node);
    FlattenFuncNode(Node, SubGraph, parent_graph);

    parent_graph->NodeList = NodeListAppend(SubGraph->NodeList, 
						parent_graph->NodeList);
    parent_graph->EdgeList = EdgeListAppend(SubGraph->EdgeList, 
						parent_graph->EdgeList);
    parent_graph->ControlList = EdgeListAppend(SubGraph->ControlList, 
						parent_graph->ControlList);

    free_graph(SubGraph);
    EliminateNode(parent_graph, Node);
}

/* Match name of input and output arrays of subgraph to that of node, so
   FlattenNode do not connect node control edges to ALL subnodes */

CopyEdgeName(Subgraph, node)
GraphPointer Subgraph;
NodePointer node;
{
    ListPointer K;
    EdgePointer NodeEdge, SubGraphEdge, GetNthGraphEdge();
    char *GetArrayName(), *EdgeName();
    int Count;
    bool IsArray();

    for (K = node->InControl, Count = 0; K != NULL ; K = K->Next, Count++) {
        if (K->Type != EdgeNode) continue;
        NodeEdge = EP(K);
        if (!IsArray(NodeEdge)) continue;
        SubGraphEdge = GetNthGraphEdge(Subgraph, Count, IN, "control");
        if (SubGraphEdge == NULL)
            printmsg("CopyEdgeName:","No corresponding Subgraph Edge to %s\n",
                                        NodeEdge->Name);
        SubGraphEdge->Name = GetArrayName(NodeEdge);
        SubGraphEdge->Name = EdgeName(SubGraphEdge);
    }

    for (K = node->OutControl, Count = 0; K != NULL ; K = K->Next, Count++) {
        if (K->Type != EdgeNode) continue;
        NodeEdge = EP(K);
        if (!IsArray(NodeEdge)) continue;
        SubGraphEdge = GetNthGraphEdge(Subgraph, Count, OUT, "control");
        if (SubGraphEdge == NULL)
            printmsg("CopyEdgeName:","No corresponding Subgraph Edge to %s\n",
                                        NodeEdge->Name);
        SubGraphEdge->Name = GetArrayName(NodeEdge);
        SubGraphEdge->Name = EdgeName(SubGraphEdge);
    }
}

FlattenFuncNode(Node, SubGraph, parent_graph)
NodePointer Node;
GraphPointer SubGraph;
GraphPointer parent_graph;
{
    ListPointer L, LL, K, KK, Ptr, Next, NNext;
    EdgePointer E, E1, RemoveFromEdgeList();
    NodePointer N;
    char NewName[128];
    char *UniqueNodeName(), *UniqueEdgeName(), *UniqueControlName();
    int Count;

    /* Relabel internal nodes and edges (to insure unique names) */
    for (N = SubGraph->NodeList; N != NULL; N = N->Next)
        N->Name = UniqueNodeName(N, parent_graph->NodeList, Node->Name);

    for (E = SubGraph->EdgeList; E != NULL; E = E->Next)
        E->Name = UniqueEdgeName(E, parent_graph->EdgeList, Node->Name);

    for (E = SubGraph->ControlList; E != NULL; E = E->Next)
        E->Name = UniqueControlName(E, parent_graph->ControlList, Node->Name);

    /* Disconnect the arcs and flatten */

    /* Input edges */
    for (K = Node->InEdges, KK = SubGraph->InEdges; K != NULL;
         K = Next, KK = NNext) {
        Next = K->Next;
        NNext = KK->Next;
        E = (EdgePointer) K->Entry;
        E1 = (EdgePointer) KK->Entry;

        /* Remove Node from OutNodes of E */
        DisconnectEdgeFromNode(E, Node, NodeNode);

        /* Remove SubGraph from InEdges of E1 */
        DisconnectNodeFromEdge(SubGraph, GraphNode, E1);

        /* Connect Fanout of E1 to E */
        ReplaceEdgeInFanout(E1, E);

        /* remove edge from edgelist */
        SubGraph->EdgeList = RemoveFromEdgeList(E1, SubGraph->EdgeList);

        ClearEdge(E1);
    }

    /* Output Edges */
    for (K = Node->OutEdges, KK = SubGraph->OutEdges; K != NULL;
         K = Next, KK = NNext) {
        Next = K->Next;
        NNext = KK->Next;
        E = (EdgePointer) K->Entry;
        E1 = (EdgePointer) KK->Entry;

        /* Remove Node from InNodes of E */
        DisconnectNodeFromEdge(Node, NodeNode, E);

        /* Remove SubGraph from OutEdges of E1 */
        DisconnectEdgeFromNode(E1, SubGraph, GraphNode);

        /* Connect Fanin of E1 to E */
        while ((Ptr = E1->InNodes) != NULL) {
            DisconnectNodeFromEdge(Ptr->Entry, Ptr->Type, E1);
            ConnectNodeToEdge(Ptr->Entry, Ptr->Type, E);
        }

        /* Connect Fanout of E1 to E */
        ReplaceEdgeInFanout(E1, E);

        /* remove edge from edgelist */
        SubGraph->EdgeList = RemoveFromEdgeList(E1, SubGraph->EdgeList);

        ClearEdge(E1);
    }

    /* Input Control Edges */
    for (Count = 0, K = Node->InControl; K != NULL; K = Next, Count++) {
        Next = K->Next;
        E = (EdgePointer) K->Entry;

        /* Try to find control edge in sub-graph with the same name */
        E1 = GetNthGraphEdge(SubGraph, Count, IN, "control");

        /* Remove Node from OutNodes of E */
        DisconnectEdgeFromNode(E, Node, NodeNode);

        /* Remove SubGraph from InEdges of E1 */
        if (E1 != NULL) {
            /* DisconnectNodeFromEdge(SubGraph, GraphNode, E1); */

            ReplaceEdgeInFanout(E1, E);
        }
        else { /* Non-directed precedence - Add to ALL nodes in the subgraph */
            for (N = SubGraph->NodeList; N != NULL; N = N->Next)
                ConnectEdgeToNode(E, N, NodeNode);
        }
    }

    /* Eliminate all remaining input control edges in subgraph  */
    for (KK = SubGraph->InControl; KK != NULL; KK = Next) {
        Next = KK->Next;
        E1 = (EdgePointer) KK->Entry;
        EliminateEdge(SubGraph, E1);
    }

    /* Output Control Edges */
    for (Count = 0, K = Node->OutControl; K != NULL; K = Next, Count++) {
        Next = K->Next;
        E = (EdgePointer) K->Entry;

        /* Try to find control edge in sub-graph with the same name */
        E1 = GetNthGraphEdge(SubGraph, Count, OUT, "control");

        /* Remove Node from InNodes of E */
        DisconnectNodeFromEdge(Node, NodeNode, E);

        /* Remove SubGraph from OutNodes of E1 */
        if (E1 != NULL) {
            /* DisconnectEdgeFromNode(E1, SubGraph, GraphNode); */

            /* Connect Fanin of E1 to E */
            for (Ptr = E1->InNodes; Ptr != NULL; Ptr = NNext) {
                NNext = Ptr->Next;
                ReplaceOutputEdge(Ptr->Entry, Ptr->Type, E1, E);
            }

            for (Ptr = E1->OutNodes; Ptr != NULL; Ptr = NNext) {
                NNext = Ptr->Next;
                if (Ptr->Type != GraphNode)
                    ReplaceInputEdge(Ptr->Entry, Ptr->Type, E1, E);
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
        E1 = (EdgePointer) KK->Entry;
        EliminateEdge(SubGraph, E1);
    }


    /* free the Node and SubGraph */
    ClearList(SubGraph->InEdges);
    ClearList(SubGraph->OutEdges);
    ClearList(SubGraph->InControl);
    ClearList(SubGraph->OutControl);
    ClearList(SubGraph->Parents);
}
