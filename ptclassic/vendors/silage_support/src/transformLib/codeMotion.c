/* $Id$ */

/*
 * Copyright (C) 1990 Regents of the University of California
 * All rights reserved.
 */

/*
 * Routines to move pieces of codes between graphs
 * Starting from and edge called Rootedge
 */

#include "flowgraph.h"
#include "st.h"

/* global functions */
extern EdgePointer GetNthEdge(), DuplicateEdge();
extern NodePointer RemoveFromNodeList(), SelectiveNodeListAppend();
extern char *UniqueEdgeName(), *UniqueNodeName(), strcmp();

/* modified by _sean_ 07/17
static st_table *EdgeTable;
*/

PerformCodeMovement(RootEdge, TopNode, TopGraph)
EdgePointer RootEdge;
NodePointer TopNode;
GraphPointer TopGraph;
{
    st_table *EdgeTable;

    EdgeTable = st_init_table(strcmp, st_strhash);

    PerformCodeMovementAux(RootEdge, TopNode, TopGraph, EdgeTable);

    st_free_table(EdgeTable);
}

PerformCodeMovementAux(RootEdge, TopNode, TopGraph, EdgeTable)
EdgePointer RootEdge;
NodePointer TopNode;
GraphPointer TopGraph;
st_table *EdgeTable;
{
    GraphPointer SubGraph;
    EdgePointer MatchEdge;
    NodePointer Node;
    ListPointer Ptr, Ptr2, Next, Next2;
    int HasFanout;

    if (st_lookup(EdgeTable, RootEdge->Name, NULL) != 0)
	return;

    SubGraph = TopNode->Master;

    /* Create a duplicate edge in the TopGraph */
    if (RootEdge->InNodes && RootEdge->InNodes->Type == GraphNode) {
	/* Find the matching edge in the topgraph */
	MatchEdge = GetNthEdge(TopNode, IN, 
	    GraphIONumber(SubGraph, RootEdge, IN), RootEdge->Class);
    }
    else {
	/* Create a duplicate Edge in the topGraph */
        MatchEdge = DuplicateEdge(TopGraph, RootEdge, 
	    UniqueEdgeName(RootEdge, TopGraph->EdgeList, TopNode->Name));
    }

    st_insert(EdgeTable, RootEdge->Name, NULL);

    /* Move the fanout of RootEdge to MatchEdge */
    for (Ptr = RootEdge->OutNodes; Ptr != NULL; Ptr = Next) {
	Next = Ptr->Next;
	if (Ptr->Type != NodeNode)
	    continue;
        Node = (NodePointer) Ptr->Entry;

	if (IsNodeInGraph(Node, SubGraph) == FALSE) 
	     ReplaceInputEdge(Node, NodeNode, RootEdge, MatchEdge);
    }

    /* Move the fanin of RootEdge to TopGraph */
    for (Ptr = RootEdge->InNodes; Ptr != NULL; Ptr = Next) {
	Next = Ptr->Next;
	if (Ptr->Type != NodeNode)
	    continue;
        Node = (NodePointer) Ptr->Entry;

        Node->Name = UniqueNodeName(Node, TopGraph->NodeList, TopNode->Name);
	SubGraph->NodeList = RemoveFromNodeList(Node, SubGraph->NodeList);
	TopGraph->NodeList = SelectiveNodeListAppend(Node, TopGraph->NodeList);

	ReplaceNodeToEdgeConnection(Node, RootEdge, MatchEdge);

	for (Ptr2 = Node->InEdges; Ptr2 != NULL; Ptr2 = Next2) {
	    Next2 = Ptr2->Next;
	    if (Ptr2->Type == EdgeNode)
	        PerformCodeMovementAux((EdgePointer) Ptr2->Entry, TopNode,
		    TopGraph, EdgeTable);
		
	}
	for (Ptr2 = Node->InControl; Ptr2 != NULL; Ptr2 = Next2) {
	    Next2 = Ptr2->Next;
	    if (Ptr2->Type == EdgeNode)
	        PerformCodeMovement((EdgePointer) Ptr2->Entry, TopNode, 
		    TopGraph);
	}
    }

    /* Check if RootEdge has any fanout left - if yes - we have to split
       else the edge can be Removed */

    HasFanout = FALSE;
    for (Ptr = RootEdge->OutNodes; Ptr != NULL; Ptr = Ptr->Next) {
	if (Ptr->Type == NodeNode || Ptr->Type == GraphNode) {
	    HasFanout = TRUE;
	    break;
	}
    }

    if (HasFanout == FALSE) {
	/* Check first if the edge was connected to the graph */
	for (Ptr = RootEdge->InNodes; Ptr != NULL; Ptr = Ptr->Next) {
	    if (Ptr->Type == GraphNode)
		break;
	}
	if (Ptr != NULL) {
	    DisconnectEdgeFromNode(MatchEdge, TopNode, NodeNode);
	    DisconnectNodeFromEdge(SubGraph, GraphNode, RootEdge);
	}
        EliminateEdge(SubGraph, RootEdge);
    }
    else {
	for (Ptr = RootEdge->InNodes; Ptr != NULL; Ptr = Ptr->Next) {
	    if (Ptr->Type == GraphNode)
		break;
	}
	if (Ptr == NULL && !IsConstantEdge(RootEdge)) {
	    ConnectEdgeToNode(MatchEdge, TopNode, NodeNode);
	    ConnectNodeToEdge(SubGraph, GraphNode, RootEdge);
	}
    }
}
