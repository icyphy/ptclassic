/* $Id$ */

/*
 * Copyright (C) 1990 Regents of the University of California
 * All rights reserved.
 */

/*
 * Routines to expand a multiplication into an add-shift 
 */
#include "flowgraph.h"

#define ZERO_BOUND (1.E-12)

/* external functions */
extern GraphPointer GraphInput();
extern NodePointer CreateNewNode(), RemoveFromNodeList();
extern EdgePointer CreateNewEdge(), RemoveFromEdgeList();
extern ListPointer SetAttribute();
extern char *GetFunction(), *Intern(), *GetType();

/* local variables */
static GraphPointer AddGraph;
static GraphPointer SubtractGraph;
static GraphPointer LeftShiftGraph;
static GraphPointer RightShiftGraph;
static GraphPointer NegateGraph;

static char *EdgeType;

extern int debugFlag;

TransformMultNodes(Graph, MaxNonZeroBits)
GraphPointer Graph;
int MaxNonZeroBits;
{
    printmsg(NULL, "transforming multiplications into add/shifts ...\n");

    /* Get some sub-graphs which will be needed */
    AddGraph = GraphInput("add", FALSE);
    SubtractGraph = GraphInput("minus", FALSE);
    LeftShiftGraph = GraphInput("shift_left", FALSE);
    RightShiftGraph = GraphInput("shift_right", FALSE);
    NegateGraph = GraphInput("negate", FALSE);

    TransformMultNodesAux(Graph, MaxNonZeroBits);
    TransformCastNodes(Graph, TRUE);
    SetEdgeWidths(Graph);
}

TransformMultNodesAux(Graph, MaxNonZeroBits)
GraphPointer Graph;
int MaxNonZeroBits;
{
    NodePointer Node, Next;
    EdgePointer Edge, InEdge, OutEdge;
    ListPointer Ptr, Next2;
    char *Function;
    char *Type;
    float Coefficient;

    for (Node = Graph->NodeList; Node != NULL; Node = Next) {
        Next = Node->Next;

	if (IsHierarchyNode(Node)) {
	    TransformMultNodesAux(Node->Master);
	    continue;
	}

	Function = GetFunction(Node);

	if (strcmp(Function, "*"))
	    continue;

	/* Check first if this is a multiplication by a constant */
	if (GetNumAttribute(Node->Arguments, "coef", &Coefficient) == FALSE) {
	   /* check if one of the edges is a constant edge */
	    for (Ptr = Node->InEdges; Ptr != NULL; Ptr = Next2) {
		Next2 = Ptr->Next;
	        if (Ptr->Type != EdgeNode)
		    continue;
		Edge = (EdgePointer) Ptr->Entry;
		if (!strcmp(Edge->Class, "constant")) {
		    GetNumAttribute(Edge->Attributes, "value", &Coefficient);	
		    DisconnectEdgeFromNode(Edge, Node, NodeNode);
		    Type = GetType(Edge);
		    break;
		}
	    }
	    if (Ptr == NULL)
		continue;
	}
	else {
	    /* Use the type of the other edge - this is a hack */
	    if (Node->InEdges == NULL || Node->InEdges->Type != EdgeNode)
		continue;
	    Edge = (EdgePointer) Node->InEdges->Entry;
	    Type = GetType(Edge);
	}

	InEdge = (EdgePointer) Node->InEdges->Entry;
	OutEdge = (EdgePointer) Node->OutEdges->Entry;
	SetType(OutEdge, GetType(InEdge));

	DisconnectNode(Node, Graph, FALSE);
	Graph->NodeList = RemoveFromNodeList(Node, Graph->NodeList);
	ClearNode(Node);

	ExpandMultiplicationNode(Graph, InEdge, OutEdge, Coefficient,
	    Type, MaxNonZeroBits);
    }
}

ExpandMultiplicationNode(Graph, InEdge, OutEdge, Coefficient, CoefType, 
    MaxNonZeroBits)
GraphPointer Graph;
EdgePointer InEdge, OutEdge;
float Coefficient;
char *CoefType;
int MaxNonZeroBits;
{
    int *Binary, Width, Fract, Sign, LSign, RSign;
    EdgePointer LEdge, REdge, OEdge, Tmp;
    NodePointer Node;
    ListPointer Ptr, Next;
    EdgePointer CreateAddShiftGraph(); 
    int i, *FixToBinary(), *BinaryToCSD();
    char Class[8];

    /* Check first if coefficient is different from zero */
    if (-ZERO_BOUND < Coefficient && Coefficient < ZERO_BOUND) {
	OutEdge->Class = Intern("constant");
        OutEdge->Attributes = SetAttribute("value",   
            CharNode, Intern("0.0"), OutEdge->Attributes);
	return;
    }

    /* Check on the Type */
    ParseType(CoefType, Class, &Width, &Fract);


    /* Translate into CSD Notation */
    Binary = FixToBinary(Coefficient, Width, Fract);

    if (Binary == NULL) { /* zero coefficient */
	OutEdge->Class = Intern("constant");
        OutEdge->Attributes = SetAttribute("value",   
            CharNode, Intern("0.0"), OutEdge->Attributes);
	return;
    }
    Binary = BinaryToCSD(Binary, &Width, &Fract, MaxNonZeroBits);

    if (debugFlag == TRUE) {
	fprintf(stdout, "Coefficient %f - Type  %s (%d, %d)\n\t",
	    Coefficient, CoefType, Width, Fract);
	for(i = Width - 1; i >= 0; i--)
	    fprintf(stdout, "%d ", Binary[i]);
	fprintf(stdout,"\n\n");
    }

    /* Get type for internal nodes */
    EdgeType = GetType(InEdge);

    /* Translate IntegerPart and Fract Part subgraphs */
    LEdge = 
	CreateAddShiftGraph(Graph, Binary, Fract, Width, 1, 1, &LSign, InEdge);
    REdge = 
	CreateAddShiftGraph(Graph, Binary, Fract-1, Width,-1,0, &RSign, InEdge);

    if (LEdge != NULL && REdge != NULL) {
	Node = CreateNewNode(Graph, UniqueName("multnode"),
	    Intern("data"), (LSign != RSign) ? SubtractGraph : AddGraph);

	if (LSign == -1) {
	    Tmp = LEdge;
	    LEdge = REdge;
	    REdge = Tmp;
	}
	ConnectEdgeToNode(LEdge, Node, NodeNode);
	ConnectEdgeToNode(REdge, Node, NodeNode);

	OEdge = CreateNewEdge(Graph, UniqueName("multedge"), Intern("data"));
	SetType(OEdge, EdgeType);
	ConnectNodeToEdge(Node, NodeNode, OEdge);
	Sign = (LSign == -1 && RSign == -1) ? -1 : 1;
    }
    else if (LEdge) {
	OEdge = LEdge;
	Sign = LSign;
    }
    else {
	OEdge = REdge;
	Sign = RSign;
    }

    /* Perform final negation if necessary */
    if (Sign == -1) {
	Node = CreateNewNode(Graph, UniqueName("multnode"),
	    Intern("data"), NegateGraph);
	ConnectEdgeToNode(OEdge, Node, NodeNode);
	OEdge = CreateNewEdge(Graph, UniqueName("multedge"), Intern("data"));
	SetType(OEdge, EdgeType);
	ConnectNodeToEdge(Node, NodeNode, OEdge);
    }

    /* Rearrange the connections - merge OEdge and OutEdge */

    MergeEdges(Graph, OutEdge, OEdge);

    cfree(Binary);
}

EdgePointer
CreateAddShiftGraph(Graph, Binary, Pointer, Length, Step, Offset, Sign, InEdge)
GraphPointer Graph;
int* Binary;
int Pointer, Length, Step, Offset;
int *Sign;
EdgePointer InEdge;
{
    int i, Shift;
    int LSign, RSign;
    EdgePointer OutEdge, LEdge, REdge;
    NodePointer Node;

    for (i = Pointer; Binary[i] == 0 && i >= 0 && i < Length; i += Step);
    if (i < 0 || i >= Length) /* Finished */
	return (NULL);

    Shift = i - Pointer + Step - Offset; 

    LSign = Binary[i];

    REdge = CreateAddShiftGraph(Graph, Binary, i + Step, Length, Step, 0, 
	&RSign, InEdge);

    /* Compose the new subgraph */

    if (REdge != NULL) {
	Node = CreateNewNode(Graph, UniqueName("multnode"), 
	    Intern("data"), (LSign != RSign) ? SubtractGraph : AddGraph);

	if (LSign == 1 || RSign == -1) {
	    LEdge = InEdge;
	}
	else {
	    LEdge = REdge;
	    REdge = InEdge;
	}
	ConnectEdgeToNode(LEdge, Node, NodeNode);
	ConnectEdgeToNode(REdge, Node, NodeNode);

	OutEdge = CreateNewEdge(Graph, UniqueName("multedge"), Intern("data"));
	SetType(OutEdge, EdgeType);
	ConnectNodeToEdge(Node, NodeNode, OutEdge);
	*Sign = (LSign == -1 && RSign == -1) ? -1 : 1;
    }
    else {
	OutEdge = InEdge;
	*Sign = LSign;
    }

    if (Shift != 0) {
	Node = CreateNewNode(Graph, UniqueName("multnode"), 
	    Intern("data"), 
	    (Shift > 0) ? LeftShiftGraph : RightShiftGraph);
	Node->Arguments = SetAttribute("shift", 
	    IntNode, (Shift < 0) ? - Shift : Shift, Node->Arguments);
	ConnectEdgeToNode(OutEdge, Node, NodeNode);
	ConnectEdgeToNode(NULL, Node, NodeNode);
	OutEdge = CreateNewEdge(Graph, UniqueName("multedge"), Intern("data"));
	SetType(OutEdge, EdgeType);
	ConnectNodeToEdge(Node, NodeNode, OutEdge);
    }
    return (OutEdge);
}
