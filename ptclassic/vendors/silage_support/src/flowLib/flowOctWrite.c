/* $Id$ */

/* Copyright (C) 1990 Regents of the University of California
 * All rights reserved.
 */

/*
 * Routines to translate a "C Flowgraph Structure" into an Oct database
 * Two modes a supported :
 * a) "new", which creates a complete new database version.
 *    Existing leaf-nodes are reused however (since those are not to be
 *    overwritten).
 * b) "annotate", which updates the attributes of the database.
 */

#include "flowgraph.h"
#include "oct.h"
#include "oh.h"
#include "st.h"
#include "flowOct.h"

#define FLOWVIEW "flowgraph"

/* Global Functions */
int strcmp();

/* local variables */
static int WriteLeafCells;
static st_table *GraphTable;

int
WriteOctGraph(FlowGraph, ForcedFlag)
GraphPointer FlowGraph;
int ForcedFlag;
{
    int fatal;

    WriteLeafCells = ForcedFlag;

    GraphTable = st_init_table(strcmp, st_strhash);

    fatal = WriteOctSubgraph(FlowGraph);

    st_free_table(GraphTable);

    return (fatal);
}

int
WriteOctSubgraph(FlowGraph)
GraphPointer FlowGraph;
{

    octObject prop, instance, master, term, fterm, net, prop2, graph;
    octObject bag, instancebag, source, dest;
    octObject precedencebag, precedenceinbag, precedenceoutbag;
    octGenerator termGenerator, netGenerator;

    EdgePointer Edge;
    NodePointer Node;
    ListPointer Ptr, SetAttribute();

    int i, LeafNodeFlag, Mode;
    char *Path, *GetOctPath();
    
    /* Check if graph has been dumped already */
    if (st_lookup(GraphTable, FlowGraph->Name, NULL) != 0)
	return (TRUE);

    /* Check if this node is a leafnode */
    LeafNodeFlag = !strcmp(FlowGraph->Class, LEAF_NODE);

    Path = GetOctPath("flowgraph.oct", FlowGraph->Name, FLOWVIEW, "contents");

    if (LeafNodeFlag && !WriteLeafCells  && Path != NULL) {
	    /* Existing LeafNode - Do NOT touch */
        st_insert(GraphTable, FlowGraph->Name, Intern(Path));
	return (TRUE);
    }
    if (Path != NULL) { /* Create a new version */
	FlowVersion(FlowGraph);
    }

    ASSERT(ohOpenFacet(&graph, FlowGraph->Name, FLOWVIEW, "contents", "w"));

    st_insert(GraphTable, FlowGraph->Name, FlowGraph->Name); 

    /* Provide the necessary properties */
    ASSERT(ohCreatePropStr(&graph, &prop, "EDITSTYLE", "SYMBOLIC"));
    ASSERT(ohCreatePropStr(&graph, &prop, "VIEWTYPE", FLOWVIEW));
    ASSERT(ohCreatePropStr(&graph, &prop, "CELLCLASS", FlowGraph->Class));
    ASSERT(ohCreatePropStr(&graph, &prop, "TECHNOLOGY", "scmos"));

    /* Dump Graph Arguments, Attributes and Model */
    if (UpdateArgumentList(&graph, "FORMAL_PARAMETERS",
	                   FlowGraph->Arguments) == 0)
	return (0);

    if (UpdateArgumentList(&graph, "ATTRIBUTES", FlowGraph->Attributes) == 0)
	return (0);

    if (UpdateArgumentList(&graph, "MODEL", FlowGraph->Model) == 0)
	return (0);

    /* Create Formal Terminals */
    for (Ptr = FlowGraph->InEdges; Ptr != NULL; Ptr = Ptr->Next) {
	Edge = (EdgePointer) Ptr->Entry;
	ASSERT(ohCreateTerm(&graph, &term, Edge->Name));
	ASSERT(ohCreatePropStr(&term, &prop, "DIRECTION", "IN"));
    }

    for (Ptr = FlowGraph->OutEdges; Ptr != NULL; Ptr = Ptr->Next) {
	Edge = (EdgePointer) Ptr->Entry;
	ASSERT(ohCreateTerm(&graph, &term, Edge->Name));
	ASSERT(ohCreatePropStr(&term, &prop, "DIRECTION", "OUT"));
    }

    /* Create the edges */

    for (Edge = FlowGraph->EdgeList; Edge != NULL; Edge = Edge->Next) {
	ASSERT(ohCreateNet(&graph, &net, Edge->Name));

	ASSERT(ohCreatePropStr(&net, &prop, "CLASS", Edge->Class));

	/* Add Attributes and Arguments */
	if (UpdateArgumentList(&net, "ATTRIBUTES", Edge->Attributes) == 0)
	    return (FALSE);

        if (UpdateArgumentList(&net, "PARAMETERS",
	                       Edge->Arguments) == 0)
	    return (FALSE);
    }

    /* Connect the formal terminals to the edges */

    for (Ptr = FlowGraph->InEdges; Ptr != NULL; Ptr = Ptr->Next) {
	Edge = (EdgePointer) Ptr->Entry;
	ASSERT(ohGetByNetName(&graph, &net, Edge->Name));
	ASSERT(ohGetByTermName(&graph, &term, Edge->Name));
	ASSERT(octAttach(&net, &term));
    }
    for (Ptr = FlowGraph->OutEdges; Ptr != NULL; Ptr = Ptr->Next) {
	Edge = (EdgePointer) Ptr->Entry;
	ASSERT(ohGetByNetName(&graph, &net, Edge->Name));
	ASSERT(ohGetByTermName(&graph, &term, Edge->Name));
	ASSERT(octAttach(&net, &term));
    }

    /* That's it for a leaf node */
    if (LeafNodeFlag) {
        ASSERT(octFlushFacet(&graph));
	return (TRUE);
    }


    /* Dump Nodes */
    ASSERT (ohCreateBag(&graph, &instancebag, "INSTANCES"));

    for (Node = FlowGraph->NodeList; Node != NULL; Node = Node->Next) {
	/* Create the master first */
	if (WriteOctSubgraph(Node->Master) == FALSE)
	    return (FALSE);

	/* Instantiate the master */
        st_lookup(GraphTable, Node->Master->Name, &Path);
	if (Path == NULL) {
	    ASSERT(ohCreateInst(&instancebag, &instance, Node->Name,
	        Node->Master->Name, FLOWVIEW));	
	}
	else {
	    ASSERT(ohCreateInst(&instancebag, &instance, Node->Name,
	         Path, FLOWVIEW));	
	}

	ASSERT(ohCreatePropStr(&instance, &prop, "CLASS", Node->Class));

	ASSERT(ohOpenMaster(&master, &instance, "contents", "r"));

	/* Define the direction of the terminals */
	ASSERT(octInitGenContents(&instance, OCT_TERM_MASK, &termGenerator));

	while (octGenerate(&termGenerator, &term) == OCT_OK) {
	    ASSERT(ohGetTerminal(&master, term.contents.term.formalExternalId, 
				 &fterm));
	    ASSERT(ohGetByPropName(&fterm, &prop, "DIRECTION"));
	    ASSERT(ohCreatePropStr(&term, &prop2, "DIRECTION", 
				   prop.contents.prop.value.string));
	}

	/* Add Attributes and Arguments */
	if (UpdateArgumentList(&instance, "ATTRIBUTES", Node->Attributes) == 0)
	    return (FALSE);

        if (UpdateArgumentList(&instance, "ACTUAL_PARAMETERS",
	                       Node->Arguments) == 0)
	    return (FALSE);

	/* Connect the terminals to the edges */
        for (i = 0, Ptr = Node->InEdges; Ptr != NULL; i++, Ptr = Ptr->Next) {
	    if (Ptr->Type == NullNode)
		continue;
	    Edge = (EdgePointer) Ptr->Entry;
	    ASSERT(ohGetByNetName(&graph, &net, Edge->Name));
	    if (GetNthTerminal(&instance, &term, i, "IN") == 0) {
		printmsg("OctWrite",
		    "Number of input edges in node %s exceeds template %s\n",
		    Node->Name, Node->Master->Name);
	        return (FALSE);
	    }
	    ASSERT(octAttach(&net, &term));
        }
        for (i = 0, Ptr = Node->OutEdges; Ptr != NULL; i++, Ptr = Ptr->Next) {
	    if (Ptr->Type == NullNode)
		continue;
	    Edge = (EdgePointer) Ptr->Entry;
	    ASSERT(ohGetByNetName(&graph, &net, Edge->Name));
	    if (GetNthTerminal(&instance, &term, i, "OUT") == 0) {
		printmsg("OctWrite",
		    "Number of output edges in node %s exceeds template %s\n",
		    Node->Name, Node->Master->Name);
	        return (FALSE);
	    }
	    ASSERT(octAttach(&net, &term));
        }
    }

    /* Create the precedence constraints */
    ASSERT(ohCreateBag(&graph, &precedencebag, "PRECEDENCES"));
    ASSERT(ohCreateBag(&graph, &precedenceinbag, "PRECEDENCE_IN"));
    ASSERT(ohCreateBag(&graph, &precedenceoutbag, "PRECEDENCE_OUT"));

    for (Edge = FlowGraph->ControlList; Edge != NULL; Edge = Edge->Next) {
	Edge->Arguments = SetAttribute("name", CharNode, Edge->Name,
					Edge->Arguments);
	for (Ptr = Edge->InNodes; Ptr != NULL; Ptr = Ptr->Next) {
	    if (Ptr->Type == NodeNode) {
	        Node = (NodePointer) Ptr->Entry;
	        ASSERT(ohGetByInstName(&instancebag, &source, Node->Name)); 
	    }
	    else {
	        source = precedenceinbag;
	    }
	    if (Ptr == Edge->InNodes) {
	        ASSERT(ohCreateBag(&source, &bag, "PRECEDES"));
	    }
	    else {
		ASSERT(octAttach(&source, &bag));
	    }
	}
	for (Ptr = Edge->OutNodes; Ptr != NULL; Ptr = Ptr->Next) {
	    if (Ptr->Type == NodeNode) {
	        Node = (NodePointer) Ptr->Entry;
	        ASSERT(ohGetByInstName(&instancebag, &dest, Node->Name)); 
	    }
	    else {
		dest = precedenceoutbag;
	    }
	    ASSERT(octAttach(&bag, &dest));
	}

	/* Attach the arguments */
        if (UpdateArgumentList(&bag, "ARGUMENTS", Edge->Arguments) == 0)
	    return (FALSE);

	ASSERT(octAttach(&precedencebag, &bag));
    }

    ASSERT(octFlushFacet(&graph));

    return (TRUE);
}

int
AnnotateOctGraph(FlowGraph, ForcedFlag)
GraphPointer FlowGraph;
int ForcedFlag;
{
    int fatal;

    WriteLeafCells = ForcedFlag;

    GraphTable = st_init_table(strcmp, st_strhash);

    fatal = AnnotateOctSubgraph(FlowGraph);

    st_free_table(GraphTable);

    return (fatal);
}

int
AnnotateOctSubgraph(FlowGraph)
GraphPointer FlowGraph;
{
    octObject graph, net, bag, instance;

    EdgePointer Edge;
    NodePointer Node;

    /* Check if graph has been dumped already */
    if (st_lookup(GraphTable, FlowGraph->Name, NULL) != 0)
	return (TRUE);

    /* Open the Oct View for the graph */
    ASSERT (ohOpenFacet(&graph, FlowGraph->Name, FLOWVIEW, "contents", "a"));

    st_insert(GraphTable, FlowGraph->Name, FlowGraph);

    /* Run over the attributes of the graph, edges and nodes and see if any 
     * have changed.
     */
    if (UpdateArgumentList(&graph, "ATTRIBUTES", FlowGraph->Attributes) == 0)
	return (FALSE);

    /* That's it for a leaf node */
    if (!strcmp(FlowGraph->Class, LEAF_NODE)) {
        ASSERT(octFlushFacet(&graph));
	return (TRUE);
    }

    /* Edges */
    for (Edge = FlowGraph->EdgeList; Edge != NULL; Edge = Edge->Next) {
	ASSERT_WHY(ohGetByNetName(&graph, &net, Edge->Name),
		   "Could Not Locate Net");
	if (UpdateArgumentList(&net, "ATTRIBUTES", Edge->Attributes) == 0)
	    return (FALSE);
    }

    /* Nodes */
    ASSERT_WHY (ohGetByBagName(&graph, &bag, "INSTANCES"), "no INSTANCES bag");

    for (Node = FlowGraph->NodeList; Node != NULL; Node = Node->Next) {
	ASSERT_WHY(ohGetByInstName(&bag, &instance, Node->Name),
		   "Could Not Locate Instance");
	if (UpdateArgumentList(&instance, "ATTRIBUTES", Node->Attributes) == 0)
	    return (FALSE);

	/* Annotate the master nodes as well */
	if (AnnotateOctSubgraph(Node->Master) == 0)
	    return (FALSE);
    }

    ASSERT(octFlushFacet(&graph));

    return (TRUE);
}

int
UpdateArgumentList(container, Class, List)
octObject *container;
char *Class;
ListPointer List;
{
    double *DoublePointer;

    octObject bag, prop;

    if (List == NULL)
	return (TRUE);

    ASSERT(ohGetOrCreateBag(container, &bag, Class));

    for (; List != NULL; List = List->Next) {
	switch (List->Type) {
	case (IntNode) :
	    ASSERT(ohCreateIprop(&bag, &prop, List->Label, (int) List->Entry));
	    break;
	case (RealNode) :
	    DoublePointer = (double *) List->Entry;
	    ASSERT(ohCreateRprop(&bag, &prop, List->Label, *DoublePointer));
	    break;
	case (CharNode) :
	    ASSERT(ohCreateSprop(&bag, &prop, List->Label,
				      (char *) List->Entry));
	    break;
	case (EdgeNode) :
	    ASSERT(ohCreateSprop(&bag, &prop, List->Label,
					 ((EdgePointer) List->Entry)->Name));
	    break;
	case (NodeNode) :
	    ASSERT(ohCreateSprop(&bag, &prop, List->Label,
					 ((NodePointer) List->Entry)->Name));
	    break;
	case (GraphNode) :
	    ASSERT(ohCreateSprop(&bag, &prop, List->Label,
					 ((GraphPointer) List->Entry)->Name));
	    break;
	case (ListNode) :
	    UpdateArgumentList(&bag, List->Label, (ListPointer) List->Entry);
	    break;
	default :
	    break;
	}
    }
    return (TRUE);
}
