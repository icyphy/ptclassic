/* $Id$ */

/* Copyright (C) 1990 Regents of the University of California
 * All rights reserved.
 */

/*
 * Routines to order the nodes topologically.
 * Furthermore, alap and asap scheduling is performed.
 * I use them right now to drive the type checking.
 */

#include "flowgraph.h"
#include "st.h"

#define NOT_CONSIDERED -1
#define UNVISITED 0
#define VISITED 1

typedef struct SCHEDULE {
    int in_level;
    int out_level;
} SCHEDULE;

int
LevelGraph(Graph, AvailableTime)
GraphPointer Graph;
int AvailableTime;
{
    NodePointer *NodeOrder, *TopologicalOrdering();
    int NrOfNodes, CPath;

    NodeOrder = TopologicalOrdering(Graph, &NrOfNodes);

    AllocateScheduleData(NodeOrder, NrOfNodes);
    LevelToInput(Graph, NodeOrder, NrOfNodes);
    LevelToOutput(Graph, NodeOrder, NrOfNodes, AvailableTime);
    CPath = FlowGetCriticalPath(NodeOrder, NrOfNodes, AvailableTime);
    if (AnnotateScheduleData(NodeOrder, NrOfNodes, AvailableTime) == FALSE)
	return (-1);
    cfree (NodeOrder);
    return (CPath);
}


NodePointer *
TopologicalOrdering(Graph, NrOfNodes)
GraphPointer Graph;
int *NrOfNodes;
{
    /* Performs a topological ordering on the graph */
    /* in a hierarchical fashion		    */ 

    NodePointer Node, *OrderList;
    int Counter;

    /* Clear all the nodes */
    /* Count them at the same time */

    *NrOfNodes = 0;

    for (Node = Graph->NodeList; Node != NULL; Node = Node->Next) {
	if (IsDelayNode(Node))
	    Node->Token = NOT_CONSIDERED;
	else {
	    (*NrOfNodes)++;
	    Node->Token = UNVISITED;
	}
    }

    /* Reserve pointer list */
    OrderList = (NodePointer *) calloc (*NrOfNodes, sizeof(NodePointer));

    /* Clear the counters    */
    Counter = 0;

    /* Perform the ordering */
    for (Node = Graph->NodeList; Node != NULL; Node = Node->Next)
	    OrderNode(Node, &Counter, OrderList);

    return (OrderList);
}

NodePointer *
TopologicalOrderingWithDelay(Graph, NrOfNodes)
GraphPointer Graph;
int *NrOfNodes;
{
    /* Performs a topological ordering on the graph */
    /* in a hierarchical fashion		    */ 

    NodePointer Node, *OrderList;
    int Counter;

    /* Clear the counters    */
    *NrOfNodes = 0;
    Counter = 0;

    /* Count nodes */
    for (Node = Graph->NodeList; Node != NULL; Node = Node->Next)
	(*NrOfNodes)++;

    /* Reserve pointer list */
    OrderList = (NodePointer *) calloc (*NrOfNodes, sizeof(NodePointer));


    for (Node = Graph->NodeList; Node != NULL; Node = Node->Next) {
	if (IsDelayNode(Node)) {
	    Node->Token = NOT_CONSIDERED;
	    OrderList[Counter++] = Node;
        }
	else
	    Node->Token = UNVISITED;
    }


    /* Perform the ordering */
    for (Node = Graph->NodeList; Node != NULL; Node = Node->Next)
	    OrderNode(Node, &Counter, OrderList);

    return (OrderList);
}

StoreInPlace(Graph, NodeOrder, NrOfNodes)
GraphPointer Graph;
NodePointer *NodeOrder;
int NrOfNodes;
{
        int i;

        for (i = NrOfNodes - 1; i > 0; i--)
                NodeOrder[i]->Next = NodeOrder[i-1];

        NodeOrder[0]->Next = (NodePointer) NULL;

        Graph->NodeList = NodeOrder[NrOfNodes-1];
}

OrderNode(Node, Counter, OrderList)
NodePointer Node;
int *Counter;
NodePointer *OrderList;
{
  
    ListPointer Ptr1, Ptr2;
    EdgePointer Edge;

    if (Node->Token != UNVISITED)
	return;

    Node->Token = VISITED;

    /* Run over the precedence fanout */
    for (Ptr1 = Node->OutEdges; Ptr1 != NULL; Ptr1 = Ptr1->Next) {
	if (Ptr1->Type != EdgeNode) continue;
	if (!(Edge = (EdgePointer) Ptr1->Entry)) continue;
	for (Ptr2 = Edge->OutNodes; Ptr2 != NULL; Ptr2 = Ptr2->Next)
	    if (Ptr2->Type == NodeNode)
		OrderNode((NodePointer) Ptr2->Entry, Counter, OrderList);
    }

    /* Run over the control fanout */
    for (Ptr1 = Node->OutControl; Ptr1 != NULL; Ptr1 = Ptr1->Next) {
	if (Ptr1->Type != EdgeNode) continue;
	if (!(Edge = (EdgePointer) Ptr1->Entry)) continue;
	for (Ptr2 = Edge->OutNodes; Ptr2 != NULL; Ptr2 = Ptr2->Next)
	    if (Ptr2->Type == NodeNode)
		OrderNode((NodePointer) Ptr2->Entry, Counter, OrderList);
    }
	    

    /* Number the node */
    OrderList[(*Counter)++] = Node;
}

LevelToInput(Graph, NodeOrder, NrOfNodes)
GraphPointer Graph;
NodePointer *NodeOrder;
int NrOfNodes;
{

    int i, Maximum, Duration;

    NodePointer Node, FanInNode;
    EdgePointer Edge;
    ListPointer Ptr1, Ptr2;

    SCHEDULE *Schedule;

    for (i = NrOfNodes - 1; i >= 0; i--) {
	Node = NodeOrder[i];

	/* Get the maximum cycle time of all fanin nodes */
	Maximum = 0;
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
		Schedule = (SCHEDULE *) FanInNode->Token;
		if ((Duration = (int) GetAttribute(FanInNode->Attributes,
					      "duration")) == 0) 
		    Duration = 1;
		if (Schedule->in_level  + Duration > Maximum)
		    Maximum = Schedule->in_level + Duration;
	    }
	}
	for (Ptr1 = Node->InControl; Ptr1 != NULL; Ptr1 = Ptr1->Next) {
	    if (Ptr1->Type != EdgeNode)
		continue;
	    Edge = (EdgePointer) Ptr1->Entry;
	    Duration = (int) GetAttribute(Edge->Arguments, "value");
	    for (Ptr2 = Edge->InNodes; Ptr2 != NULL; Ptr2 = Ptr2->Next) {
		if (Ptr2->Type == GraphNode)
		    continue;
		FanInNode = (NodePointer) Ptr2->Entry;
		if (FanInNode->Token == NOT_CONSIDERED)
		    continue;
		Schedule = (SCHEDULE *) FanInNode->Token;
		if (Schedule->in_level + Duration > Maximum)
		    Maximum = Schedule->in_level + Duration;
	    }
	}

	Schedule = (SCHEDULE *) Node->Token;
	Schedule->in_level = Maximum;
    }
}

LevelToOutput(Graph, NodeOrder, NrOfNodes, AvailableTime)
GraphPointer Graph;
NodePointer *NodeOrder;
int NrOfNodes;
int AvailableTime;
{

    int i, Minimum, Duration;

    NodePointer Node, FanOutNode;
    EdgePointer Edge;
    ListPointer Ptr1, Ptr2;

    SCHEDULE *Schedule;

    for (i = 0; i < NrOfNodes; i++) {
	Node = NodeOrder[i];

	/* Get the minimum cycle time of all fanout nodes */
	Minimum = AvailableTime - 1;
	for (Ptr1 = Node->OutEdges; Ptr1 != NULL; Ptr1 = Ptr1->Next) {
	    if (Ptr1->Type != EdgeNode)
		continue;
	    Edge = (EdgePointer) Ptr1->Entry;
	    for (Ptr2 = Edge->OutNodes; Ptr2 != NULL; Ptr2 = Ptr2->Next) {
		if (Ptr2->Type == GraphNode)
		    continue;
		FanOutNode = (NodePointer) Ptr2->Entry;
		if (FanOutNode->Token == NOT_CONSIDERED)
		    continue;
		Schedule = (SCHEDULE *) FanOutNode->Token;
		if ((Duration = (int) GetAttribute(FanOutNode->Attributes,
					      "duration")) == 0) 
		    Duration = 1;
		if (Schedule->out_level - Duration < Minimum)
		    Minimum = Schedule->out_level - Duration;
	    }
	}
	for (Ptr1 = Node->OutControl; Ptr1 != NULL; Ptr1 = Ptr1->Next) {
	    if (Ptr1->Type != EdgeNode)
		continue;
	    Edge = (EdgePointer) Ptr1->Entry;
	    for (Ptr2 = Edge->OutNodes; Ptr2 != NULL; Ptr2 = Ptr2->Next) {
		if (Ptr2->Type == GraphNode)
		    continue;
		FanOutNode = (NodePointer) Ptr2->Entry;
		if (FanOutNode->Token == NOT_CONSIDERED)
		    continue;
		Schedule = (SCHEDULE *) FanOutNode->Token;
		Duration = (int) GetAttribute(Edge->Arguments, "value");
		if (Schedule->out_level - Duration < Minimum)
		    Minimum = Schedule->out_level - Duration;
	    }
	}

	Schedule = (SCHEDULE *) Node->Token;
	Schedule->out_level = Minimum;
    }
}

AllocateScheduleData(NodeOrder, NrOfNodes)
NodePointer *NodeOrder;
int NrOfNodes;
{
    SCHEDULE *Schedule;
    int i;

    for ( i = 0; i < NrOfNodes; i++) {
        Schedule = (SCHEDULE *) malloc (sizeof (SCHEDULE));
	Schedule->in_level = Schedule->out_level = 0;
	NodeOrder[i]->Token = (int) Schedule;
    }
}

AnnotateScheduleData(NodeOrder, NrOfNodes, AvailableTime)
NodePointer *NodeOrder;
int NrOfNodes;
int AvailableTime;
{
    SCHEDULE *Schedule;
    ListPointer SetAttribute();
    int i, OK;

    OK = TRUE;

    for (i = 0; i < NrOfNodes; i++) {
	Schedule = (SCHEDULE *) NodeOrder[i]->Token;
	if (AvailableTime != 0  && 
	    (Schedule->in_level >= AvailableTime ||
	     Schedule->out_level < 0))  /* impossible schedule */
	    OK = FALSE;

	NodeOrder[i]->Attributes = SetAttribute("in_level", IntNode, 
	    Schedule->in_level, NodeOrder[i]->Attributes);
	NodeOrder[i]->Attributes = SetAttribute("out_level", IntNode, 
	    Schedule->out_level, NodeOrder[i]->Attributes);
	free(Schedule);
	NodeOrder[i]->Token = 0;
    }
    return (OK);
}

int 
FlowGetCriticalPath(NodeOrder, NrOfNodes, AvailableTime)
NodePointer *NodeOrder;
int NrOfNodes;
int AvailableTime;
{
    SCHEDULE *Schedule;
    int CPath = 0;
    int i, Duration;

    for (i = 0; i < NrOfNodes; i++) {
	Schedule = (SCHEDULE *) NodeOrder[i]->Token;
	if ((Duration = (int) GetAttribute(NodeOrder[i]->Attributes,
				      "duration")) == 0) 
	    Duration = 1;
	if (Schedule->in_level + Duration > CPath)
	    CPath = Schedule->in_level + Duration;
    }
    return (CPath);
}

/******************************************************************************
 * Routine to check if node Source is in the transitive fanout of node Dest
 * This routine can be of exponential complexity and extensive use is
 * not recommended.
 *****************************************************************************/

static st_table *NodeTable;

int
IsNodeInFanout(Source, Dest)
NodePointer Source, Dest;
{
    int status;

    NodeTable = st_init_table(st_ptrcmp, st_ptrhash);

    status = IsNodeInFanoutAux(Source, Dest, FALSE);

    st_free_table(NodeTable);

    return (status);
}

int
IsNodeInFanoutAux(Source, Dest, WeightFlag)
NodePointer Source, Dest;
int WeightFlag;
{
    register ListPointer Ptr, Ptr2;
    NodePointer Node;
    EdgePointer Edge;
    int Duration;

    if (st_is_member(NodeTable, Source))
	return (FALSE);
    else
	st_insert(NodeTable, Source, NULL);

    for (Ptr = Source->OutEdges; Ptr != NULL; Ptr = Ptr->Next) {
	if (Ptr->Type != EdgeNode)
	    continue;

	Edge = (EdgePointer) Ptr->Entry;

        for (Ptr2 = Edge->OutNodes; Ptr2 != NULL; Ptr2 = Ptr2->Next) {
	    if (Ptr2->Type != NodeNode)
	    continue;

	    Node = (NodePointer) Ptr2->Entry;

            if (IsDelayNode(Node))
	        continue;
	    else if (Node == Dest || IsNodeInFanoutAux(Node, Dest, TRUE))
		return (TRUE);
	}
    }

    for (Ptr = Source->OutControl; Ptr != NULL; Ptr = Ptr->Next) {
	if (Ptr->Type != EdgeNode)
	    continue;

	Edge = (EdgePointer) Ptr->Entry;

	Duration = (int) GetAttribute(Edge->Arguments, "value");

	/* Check here on weight */

        for (Ptr2 = Edge->OutNodes; Ptr2 != NULL; Ptr2 = Ptr2->Next) {
	    if (Ptr2->Type != NodeNode)
	    continue;

	    Node = (NodePointer) Ptr2->Entry;

            if (IsDelayNode(Node))
	        continue;
	    else if (Node == Dest || IsNodeInFanoutAux(Node, Dest, 
	       (WeightFlag == TRUE || Duration > 0) ? TRUE : FALSE))
		return (TRUE);
	}
    }

    return (FALSE);
}
