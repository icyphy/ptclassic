/* $Id$ */

/*
 * Copyright (C) 1990 Regents of the University of California
 * All rights reserved.
 */

/*
 * Contains some simple graph transformations :
 * removeal of cast nodes, constant nodes and break nodes
 */

#include "flowgraph.h"
#include "st.h"

/* definitions */
#define EPSILON (1.0E-20)

/* global functions */
extern char *GetFunction(), *Intern();
extern ListPointer new_list(), ListAppend(), ListAdd(), SetAttribute();
extern NodePointer RemoveFromNodeList();
extern NodePointer *TopologicalOrdering();

static my_numhash(), my_numcmp();

RemoveConstantNodes(Graph)
GraphPointer Graph;
{
    NodePointer Node, Next;
    EdgePointer Edge;
    ListPointer Entry;
    char *Function;

    /* First Remove all Constant Nodes and replace by constant edges */
    for (Node = Graph->NodeList; Node != NULL; Node = Next) {
        Next = Node->Next;

	if (IsHierarchyNode(Node)) {
	    RemoveConstantNodes(Node->Master);
	    continue;
	}

        Function = GetFunction(Node);

        /* constant node */
        if (strcmp(Function, "constant"))
    	    continue;

        Graph->NodeList = RemoveFromNodeList(Node, Graph->NodeList);

        Edge = (EdgePointer) Node->OutEdges->Entry;
        Edge->Class = Intern("constant");

							/* [erc/7-22-92] */
	Edge->Attributes = SetAttribute("value",
					GetAttributeType(Node->Arguments,
							 "value"),
					GetAttribute(Node->Arguments,
						     "value"),
					Edge->Attributes);

        ClearList(Edge->InNodes);
        Edge->InNodes = NULL;
        ClearNode(Node);
    }
}

ManifestExpressions(Graph)
GraphPointer Graph;
{

    printmsg(NULL, "elimination of manifest expressions ...\n");

    ManifestExpressionsAux(Graph);
}

ManifestExpressionsAux(Graph)
GraphPointer Graph;
{
    int i, NrOfNodes, IntValue;
    NodePointer Node, Next, *NodeOrder;
    EdgePointer Edge;
    ListPointer Ptr, Entry;
    char *Function, ValueString[20];
    float Value, Value1, Value2;

    for (Node = Graph->NodeList; Node != NULL; Node = Next) {
        Next = Node->Next;

	if (IsHierarchyNode(Node)) {
	    ManifestExpressionsAux(Node->Master);
	    continue;
	}
    }

    /* Eliminate manifest expressions */
    NodeOrder = TopologicalOrdering(Graph, &NrOfNodes);

    for (i = NrOfNodes - 1; i >= 0; i--) {
	Node = NodeOrder[i];

	Function = GetFunction(Node);

	/* check if all input Edges are constants */
	for (Ptr = Node->InEdges; Ptr != NULL; Ptr = Ptr->Next) {
	    if (Ptr->Type != EdgeNode)
		continue;
	    if (strcmp(((EdgePointer) Ptr->Entry)->Class, "constant"))
		break;
	}
	if (Ptr != NULL)
	    continue;

	/* Eliminate only simple arithmetic operations */
	if (*Function != '+' && *Function != '-' && *Function != '*' &&
	    *Function != '/' && *Function != '<' && *Function != '>')
	    continue;

	Edge = (EdgePointer) Node->InEdges->Entry;
	GetNumAttribute(Edge->Attributes, "value", &Value1);
	if (Node->InEdges->Next != NULL &&
	    Node->InEdges->Next->Type == EdgeNode) {
	    Edge = (EdgePointer) Node->InEdges->Next->Entry;
	    GetNumAttribute(Edge->Attributes, "value", &Value2);
	}
	else
	    Value2 = 0;

	switch (*Function) {
	case ('+') :
	    Value = Value1 + Value2;
	    break;
	case ('-') :
	    Value = Value1 - Value2;
	    break;
	case ('*') :
	    Value = Value1 * Value2;
	    break;
	case ('/') :
	    Value = Value1 / Value2;
	    break;
	case ('>') :
	    IntValue = (int) GetAttribute(Node->Arguments, "shift");
	    Value = Value1 / power (2, IntValue);
	    break;
	case ('<') :
	    IntValue = (int) GetAttribute(Node->Arguments, "shift");
	    Value = Value1 * power (2, IntValue);
	    break;
	}

	Edge = (EdgePointer) Node->OutEdges->Entry;
        Edge->Class = Intern("constant");

	sprintf(ValueString, "%f", Value);
	Edge->Attributes = SetAttribute("value",	/* [erc/7-22-92] */
					CharNode,
					Intern(ValueString),
					Edge->Attributes);

	EliminateNode(Graph, Node);
    }
}

CollapseConstantEdges(Graph)
GraphPointer Graph;
{
    EdgePointer Edge, NextEdge, RefEdge;
    ListPointer ConstantTable, Ptr;
    NodePointer Node;
    float Value, *FValue;

    for (Node = Graph->NodeList; Node != NULL; Node = Node->Next)
	if (IsHierarchyNode(Node))
	    CollapseConstantEdges(Node->Master);

    /* Collapse constant edges (saves registers) */
    ConstantTable = NULL;

    for (Edge = Graph->EdgeList; Edge != NULL; Edge = NextEdge) {
	NextEdge = Edge->Next;
	if (strcmp(Edge->Class, "constant"))
	    continue;
	if (GetNumAttribute(Edge->Attributes, "value", &Value) == FALSE)
            continue;
	for (Ptr = ConstantTable; Ptr != NULL; Ptr = Ptr->Next) {
	    if (*((float *) Ptr->Label) == Value) {
		RefEdge = (EdgePointer) Ptr->Entry;
		if (AreIdenticalTypes(GetAttribute(RefEdge->Arguments, "type"),
		    GetAttribute(Edge->Arguments, "type")))
		    break;
	    }
	}
	if (Ptr != NULL) {
	    /* constant edge already existing - eliminate new edge */
	    ReplaceEdgeInFanout(Edge, RefEdge);
	    EliminateEdge(Graph, Edge);
	}
	else {
	    FValue = (float *) malloc (sizeof(float));
	    *FValue = Value;
	    Ptr = new_list(EdgeNode, (pointer) Edge);
	    Ptr->Label = (char *) FValue;
	    ConstantTable = ListAdd(Ptr, ConstantTable);
	}
    }
    ClearList(ConstantTable);
}

static int
my_numcmp(key1, key2)
float *key1, *key2;
{
    int sign1, sign2; 

    sign1 = *key1 - *key2 >= EPSILON;
    sign2 = *key2 - *key1 >= EPSILON;
    return ( sign1 ? 1 : sign2 ? -1 : 0);
}

static int
my_numhash(key, modulus)
float *key;
int modulus;
{
    char hashstring[20];

    sprintf(hashstring, "%f", *key);
    return (st_strhash(hashstring, modulus));
}

power (x,n)
int  x,n;
{
	int   i, p;

	p = 1;
	for ( i = 1; i <= n; ++i)
	    p *= x;

	return (p);
}
