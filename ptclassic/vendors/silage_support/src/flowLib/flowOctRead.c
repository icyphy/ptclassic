/* $Id$ */

/* Copyright (C) 1990 Regents of the University of California
 * All rights reserved.
 */

/*
 * Routines to parse an oct flowgraph
 * and translate it into the "C flowgraph structure"
 * All checking is performed on the c-structure.
 */

#include "flowgraph.h"
#include "oct.h"
#include "oh.h"
#include "st.h"
#include "flowOct.h"

#define FLOWVIEW "flowgraph"

/* global functions */
char *Intern(), *UniqueName(), *myGetName();
GraphPointer new_graph();
ListPointer new_list(), ListAdd(), ListAppend(), RemoveAttribute();
EdgePointer new_edge(), EdgeListAppend();
NodePointer new_node(), NodeListAppend();
int strcmp();

/* Global variables */
st_table *InstanceTable;

GraphPointer
ParseOctGraph(Name)
char *Name;
{
    GraphPointer Graph, ParseOctSubgraph();

    InstanceTable = st_init_table(strcmp, st_strhash);

    Graph  = ParseOctSubgraph(Name);

    st_free_table(InstanceTable);

    return (Graph);
}

ParseOctInit()
{
    InstanceTable = st_init_table(strcmp, st_strhash);
}

ParseOctFree()
{
    st_free_table(InstanceTable);
}


GraphPointer
ParseOctSubgraph(Name)
char *Name;
{
    octObject graph, term, formalterm, bag, prop, instance, master, net;
    octObject source, dest;
    octGenerator termGenerator, bagGenerator, instanceGenerator, netGenerator;

    GraphPointer FlowGraph, GraphListAppend(), ParseOctSubgraph();
    ListPointer Entry, Ptr, new_list();
    ListPointer GetArgumentList(), GetArgumentListAux();
    NodePointer Node;
    EdgePointer Edge;

    char *StrippedName;
    int LeafNodeFlag;
    int32 id;

    /* 
     * Check first if this graph has not been parsed already 
     */
    StrippedName = Intern(RemoveDirectoryTree(Name));

    if (st_lookup(InstanceTable, StrippedName, &FlowGraph) != 0)
	return (FlowGraph);

    /* Create Graph Entry and add to the list */
    FlowGraph = new_graph();
    FlowGraph->Name = StrippedName;
    st_insert(InstanceTable, StrippedName, FlowGraph);

    ASSERT_WHY (ohOpenFacet(&graph, Name, FLOWVIEW, "contents", "r"),
	    "Could not open facet");


    /* check if this if a LEAF NODE */
    LeafNodeFlag = 
	(ohGetByPropName(&graph, &prop, "CELLCLASS") == OCT_OK &&
	!strcmp(prop.contents.prop.value.string, LEAF_NODE)) ?
	TRUE : FALSE;

    FlowGraph->Class = (LeafNodeFlag == TRUE) ? LEAF_NODE : HIERARCHY_NODE;

    /* 
     * check if the graph has arguments, attributes or a model 
     */

    if (ohGetByBagName(&graph, &bag, "FORMAL_PARAMETERS") == OCT_OK)
	FlowGraph->Arguments = GetArgumentList(&graph, "FORMAL_PARAMETERS");

    if (ohGetByBagName(&graph, &bag, "ATTRIBUTES") == OCT_OK)
	FlowGraph->Attributes = GetArgumentList(&graph, "ATTRIBUTES");

    if (ohGetByBagName(&graph, &bag, "MODEL") == OCT_OK) 
	FlowGraph->Model = GetArgumentList(&graph, "MODEL");

    /* scan the formal terminals and declare them as input and output edges */

    ASSERT(octInitGenContents(&graph, OCT_TERM_MASK, &termGenerator));

    while (octGenerate(&termGenerator, &term) == OCT_OK) {
	if (octGenFirstContainer(&term, OCT_NET_MASK, &net) == OCT_OK) 
	    Entry = new_list(CharNode, (pointer) myGetName(&net));
	else
	    Entry = new_list(CharNode, (pointer) myGetName(&term));

        if (IsInputTerminal(&term)) 
	    FlowGraph->InEdges = ListAppend(Entry, FlowGraph->InEdges);
	else 
	    FlowGraph->OutEdges = ListAppend(Entry, FlowGraph->OutEdges);
    }

    /* That's it for a leaf node */
    if (LeafNodeFlag == TRUE)
	goto ReadEdges;

    /* scan over all the instances and declare them as nodes */
    ASSERT_WHY (ohGetByBagName(&graph, &bag, "INSTANCES"),
	    "no INSTANCES bag\n");

    ASSERT(octInitGenContents(&bag, OCT_INSTANCE_MASK, &instanceGenerator));

    while (octGenerate(&instanceGenerator, &instance) == OCT_OK) {
	Node = new_node();
	FlowGraph->NodeList = NodeListAppend(Node, FlowGraph->NodeList);
	Node->Name = myGetName(&instance);

	if (ohGetByPropName(&instance, &prop, "CLASS") == OCT_OK)
	    Node->Class = Intern(prop.contents.prop.value.string);
	else
	    Node->Class = Intern("data");

	/* Get the arguments and the attributes */
	Node->Arguments = GetArgumentList(&instance, "ACTUAL_PARAMETERS");
	Node->Attributes = GetArgumentList(&instance, "ATTRIBUTES");

	/* Parse the underlying graph if necessary */
	Node->Master = ParseOctSubgraph(instance.contents.instance.master);
	if (Node->Master == NULL)
	    return (NULL);

	/* Get the InEdges and the OutEdges */
	/* Those should be ordered in the same order as on the master node */

	ASSERT_WHY(ohOpenFacet(&master, instance.contents.instance.master,
		   FLOWVIEW, "contents", "r"),
		   "Could not open master view\n");

	ASSERT(octInitGenContents(&master, OCT_TERM_MASK, &termGenerator));

        while (octGenerate(&termGenerator, &formalterm) == OCT_OK) {
	    /* 
	     * find instance terminal corresponding with formal terminal.
	     */
	       
	    octExternalId(&formalterm, &id);
	    ASSERT (ohGetTerminal(&instance, id, &term));

	    if (octGenFirstContainer(&term, OCT_NET_MASK, &net) == OCT_OK) 
	        Entry = new_list(CharNode, myGetName(&net));
	    else
		Entry = new_list(NullNode, NULL);   /* net is not connected */

            if (IsInputTerminal(&term)) 
	        Node->InEdges = ListAppend(Entry, Node->InEdges);
	    else 
	        Node->OutEdges = ListAppend(Entry, Node->OutEdges);
        }
    }


    /* scan over all the nets and declare them as edges */
ReadEdges:

    ASSERT(octInitGenContents(&graph, OCT_NET_MASK, &netGenerator));

    while (octGenerate(&netGenerator, &net) == OCT_OK) {
	Edge = new_edge();
	FlowGraph->EdgeList = EdgeListAppend(Edge, FlowGraph->EdgeList);
	Edge->Name = myGetName(&net);
	if (ohGetByPropName(&net, &prop, "CLASS") == OCT_OK)
	    Edge->Class = Intern(prop.contents.prop.value.string);
	else
	    Edge->Class = Intern(DATA_EDGE);

	/* Get the arguments and the attributes */
	Edge->Arguments = GetArgumentList(&net, "PARAMETERS");
	Edge->Attributes = GetArgumentList(&net, "ATTRIBUTES");

	/* Get the InNodes and the OutNodes */
	/* Order is not important           */
	ASSERT(octInitGenContents(&net, OCT_TERM_MASK, &termGenerator));

	while (octGenerate(&termGenerator, &term) == OCT_OK) {
	    if (IsFormalTerminal(&term)) {
		Entry = new_list(CharNode, (pointer) Intern("parent"));
	        if (IsInputTerminal(&term)) 
	            Edge->InNodes = ListAppend(Entry, Edge->OutNodes);
	        else
	            Edge->OutNodes = ListAppend(Entry, Edge->InNodes);
	    }
	    else {
		ASSERT(octGenFirstContainer(&term,OCT_INSTANCE_MASK,&instance));
		Entry = new_list(CharNode, myGetName(&instance));
	        if (IsInputTerminal(&term)) 
	            Edge->OutNodes = ListAppend(Entry, Edge->OutNodes);
	        else
	            Edge->InNodes = ListAppend(Entry, Edge->InNodes);
	    }
	}
    }

    /* 
     * Enter the control edges 
     */

    if (ohGetByBagName(&graph, &bag, "PRECEDENCES") == OCT_OK) {
	ASSERT(octInitGenContents(&bag, OCT_BAG_MASK, &bagGenerator));

	while (octGenerate(&bagGenerator, &bag) == OCT_OK) {
	    Edge = new_edge();
	    Edge->Class = Intern(CONTROL_EDGE);

	    ASSERT(octInitGenContainers(&bag, 
		OCT_INSTANCE_MASK | OCT_BAG_MASK, &instanceGenerator));
	    while (octGenerate(&instanceGenerator, &source) == OCT_OK) {
		if (source.type == OCT_INSTANCE) {
	            Entry = new_list(CharNode, (pointer) myGetName(&source));
	            Edge->InNodes = ListAppend(Entry, Edge->InNodes);
		}
		else if (!strcmp(ohGetName(&source), "PRECEDENCE_IN")) {
	            Entry = new_list(CharNode, (pointer) FlowGraph->Name);
	            Edge->InNodes = ListAppend(Entry, Edge->InNodes);
		}
	    }

	    ASSERT(octInitGenContents(&bag,
		OCT_INSTANCE_MASK | OCT_BAG_MASK, &instanceGenerator));
	    while (octGenerate(&instanceGenerator, &dest) == OCT_OK) {
		if (dest.type == OCT_INSTANCE) {
	            Entry = new_list(CharNode, (pointer) myGetName(&dest));
	            Edge->OutNodes = ListAppend(Entry, Edge->OutNodes);
		}
		else if (!strcmp(ohGetName(&dest), "PRECEDENCE_OUT")) {
		    Entry = new_list(CharNode, (pointer) FlowGraph->Name);
		    Edge->OutNodes = ListAppend(Entry, Edge->OutNodes);
		}
	    }

	    FlowGraph->ControlList = 
		EdgeListAppend(Edge, FlowGraph->ControlList);

	    /* Attach Properties to Edge Argument List */
	    Edge->Arguments = GetArgumentList(&bag, "ARGUMENTS");

	    /* Find the name */
	    if (HasAttribute(Edge->Arguments, "name") == FALSE) {
	        Edge->Name = UniqueName("cedge");
	    }
	    else {
		Edge->Name = (char *) GetAttribute(Edge->Arguments, "name");
		Edge->Arguments = RemoveAttribute(Edge->Arguments, "name");
	    }

	}
    }

    /* Resolve references to nodes and edges */
    ResolveNodeReferences(FlowGraph);
    ResolveEdgeReferences(FlowGraph);

    /* Finally connect the control-edges to the nodes */
    for (Edge = FlowGraph->ControlList; Edge != NULL; Edge = Edge->Next) {
	Entry = new_list(EdgeNode, (pointer) Edge); 
	if (Edge->InNodes->Type == GraphNode) {
	    FlowGraph->InControl = ListAppend(Entry, FlowGraph->InControl);
	}
	else {
	    Node = (NodePointer) Edge->InNodes->Entry;
	    Node->OutControl = ListAppend(Entry, Node->OutControl);
	}
	Entry = new_list(EdgeNode, (pointer) Edge); 
	if (Edge->OutNodes->Type == GraphNode) {
	    FlowGraph->OutControl = ListAppend(Entry, FlowGraph->OutControl);
	}
	else {
	    Node = (NodePointer) Edge->OutNodes->Entry;
	    Node->InControl = ListAppend(Entry, Node->InControl);
	}
    }

    octCloseFacet(&graph);
    return ((fatal == TRUE) ? NULL : FlowGraph);
}

ListPointer
GetArgumentList(container, class)
octObject *container;
char *class;
{
    /* Inputs a list of properties, contained in a bag "class", contained
     * by container.
     */
    
    octObject bag;

    if (ohGetByBagName(container, &bag, class) < OCT_OK)
	return (NULL);

    return (GetArgumentListAux(&bag));
}

ListPointer
GetArgumentListAux(bag)
octObject *bag;
{

    octObject prop, listbag;
    octGenerator propGenerator, bagGenerator;
    ListPointer Entry, ArgumentList;
    double *DoublePointer;

    ASSERT (octInitGenContents(bag, OCT_PROP_MASK, &propGenerator));

    ArgumentList = NULL;

    while (octGenerate(&propGenerator, &prop) == OCT_OK) {
	switch (prop.contents.prop.type) {
	case (OCT_INTEGER) :
	    Entry = new_list(IntNode, 
			     (pointer) prop.contents.prop.value.integer);
	    break;
	case (OCT_REAL)  :
	    DoublePointer = (double *) malloc(sizeof(double));
	    *DoublePointer = prop.contents.prop.value.real;
	    Entry = new_list(RealNode, (pointer) DoublePointer);
	    continue;
	    break;
	case (OCT_STRING) :
	    Entry = new_list(CharNode, 
			     (pointer) prop.contents.prop.value.string);
	    break;
	default :
	    continue;
	}
	Entry->Label = myGetName(&prop);
	ArgumentList = ListAppend(Entry, ArgumentList);
    }

    ASSERT(octInitGenContents(bag, OCT_BAG_MASK, &bagGenerator));

    while (octGenerate(&bagGenerator, &listbag) == OCT_OK) {
	Entry = new_list(ListNode, GetArgumentListAux(&listbag));
	Entry->Label = myGetName(&listbag);
	ArgumentList = ListAppend(Entry, ArgumentList);
    }

    return (ArgumentList);
}
