/* $Id$ */

/* Copyright (C) 1990 Regents of the University of California
 * All rights reserved.
 */

/* 
 * Routines to define the types of the edges in a flowgraph. This routine
 * checks the types at the same time.
 * At this point in time, I am still using the built-in type checking.
 * A future version should use the more involved type propagation,
 * described in the OCT document.
 */

#include "flowgraph.h"

#define MAX_NR_OF_EDGES 20

struct DataType {
    EdgePointer edge;
    char *class;
    int a;
    int b;
};

static struct DataType InList[MAX_NR_OF_EDGES];
static int NrInEdges;
static char *CurrentNode, *CurrentGraph;
static int PrintWarning;

extern char *GetType(), *GetFunction(), *GetAttribute();


FlowTypes(Graph, WarnFlag)
GraphPointer Graph;
int WarnFlag;
{
    fatal = FALSE;

    PrintWarning = WarnFlag;

    ForwardDrive(Graph);
    PropagateDelays(Graph); 
    BackwardDrive(Graph);

    UnresolvedEdges(Graph);

    return (fatal);
}

ForwardDrive(Graph)
GraphPointer Graph;
{

    NodePointer *NodeOrder, *TopologicalOrdering();
    int NrOfNodes, i;

    /* Level the graph using an ASAP schedule */
    NodeOrder = TopologicalOrdering(Graph, &NrOfNodes);

    for (i = NrOfNodes - 1;  i >= 0; i--)
	if (DeduceType(NodeOrder[i], Graph) == TRUE)
	    break;

    cfree(NodeOrder);
    return (fatal);
	
}

BackwardDrive(Graph)
GraphPointer Graph;
{
    NodePointer *NodeOrder, *TopologicalOrdering();
    int NrOfNodes, i;

    /* Level the graph using an ALAP schedule */
    NodeOrder = TopologicalOrdering(Graph, &NrOfNodes);

    for (i = 0; i < NrOfNodes; i++)
	if (ImposeType(NodeOrder[i], Graph) == TRUE)
	    break;

    cfree(NodeOrder);
    return (fatal);
}

UnresolvedEdges(Graph)
GraphPointer Graph;
{
    EdgePointer Edge;
    NodePointer Node;
    int a, b;
    char *Class;

    for (Edge = Graph->EdgeList; Edge != NULL; Edge = Edge->Next) {
	if (ParseType(GetType(Edge), &Class, &a, &b) == FALSE && PrintWarning)
	{
	   printmsg("type-check", "Type of edge %s (graph %s) is unresolved.\n",
		Edge->Name, Graph->Name);
	}
    }

    for (Node = Graph->NodeList; Node != NULL; Node = Node->Next)
	if (IsHierarchyNode(Node))
            UnresolvedEdges(Node->Master);
}

DeduceType(Node, Graph)
NodePointer Node;
GraphPointer Graph;
{
    /* Try to determine the types of the output edges of a node given the
     * types of the input edges.
     * This is clearly dependent upon the functionality of the node.
     * We have therefore created a list of all the commonly implemented nodes.
     * This list should be made extendable (or file driven) in a next version
     */

    ListPointer Ptr1, Ptr2;
    EdgePointer Edge1, Edge2;
    GraphPointer SubGraph;

    if (IsHierarchyNode(Node)) {
	    /* Hierarchical Node - go to subgraphs */
	    SubGraph = Node->Master;

	    /* copy types of Node InEdges to types of Graph InEdges */
	    for (Ptr1 = Node->InEdges, Ptr2 = SubGraph->InEdges;
		 Ptr1 != NULL && Ptr2 != NULL;
		 Ptr1 = Ptr1->Next, Ptr2 = Ptr2->Next) {
		if (Ptr1->Type == NullNode || Ptr2->Type == NullNode)
		    continue;
		Edge1 = (EdgePointer) Ptr1->Entry;
		Edge2 = (EdgePointer) Ptr2->Entry;
		if (MatchEdges(Edge1, Node->Name, Edge2, SubGraph->Name)
		    == FALSE) {
		    fatal = TRUE;
		    return (fatal);
		}
	    }

	    if (ForwardDrive(SubGraph) == TRUE)
		return (TRUE);

	    /* copy types of Graph OutEdges to types of Node OutEdges */
	    for (Ptr1 = SubGraph->OutEdges, Ptr2 = Node->OutEdges;
		 Ptr1 != NULL && Ptr2 != NULL;
		 Ptr1 = Ptr1->Next, Ptr2 = Ptr2->Next) {
		if (Ptr1->Type == NullNode || Ptr2->Type == NullNode)
		    continue;
		Edge1 = (EdgePointer) Ptr1->Entry;
		Edge2 = (EdgePointer) Ptr2->Entry;
		if (MatchEdges(Edge1, SubGraph->Name, Edge2, Node->Name)
		    == FALSE) {
		    fatal = TRUE;
		    return (fatal);
		}
	    }
    }
    else 
	DeduceAtomType(Node, Graph);

    return (fatal);
}

ImposeType(Node, Graph)
NodePointer Node;
GraphPointer Graph;
{
    /* Try to determine the types of the input edges of a node given the
     * types of the output edges.
     * This is clearly dependent upon the functionality of the node.
     * We have therefore created a list of all the commonly implemented nodes.
     * This list should be made extendable (or file driven) in a next version
     */

    ListPointer Ptr1, Ptr2;
    EdgePointer Edge1, Edge2;
    GraphPointer SubGraph;

    if (IsHierarchyNode(Node)) {
	     /* Hierarchical Node - go to subgraphs */
	    SubGraph = Node->Master;

	    /* copy types of Node OutNodes to types of Graph OutNodes */
	    for (Ptr1 = Node->OutEdges, Ptr2 = SubGraph->OutEdges;
		 Ptr1 != NULL && Ptr2 != NULL;
		 Ptr1 = Ptr1->Next, Ptr2 = Ptr2->Next) {
		if (Ptr1->Type == NullNode || Ptr2->Type == NullNode)
		    continue;
		Edge1 = (EdgePointer) Ptr1->Entry;
		Edge2 = (EdgePointer) Ptr2->Entry;
		if (MatchEdges(Edge1, Node->Name, Edge2, SubGraph->Name)
		    == FALSE) {
		    fatal = TRUE;
		    return (fatal);
		}
	    }

	    if (BackwardDrive(SubGraph) == TRUE)
		return (TRUE);

	    /* copy types of Graph InEdges to types of Node InEdges */
	    for (Ptr1 = SubGraph->InEdges, Ptr2 = Node->InEdges;
		 Ptr1 != NULL && Ptr2 != NULL;
		 Ptr1 = Ptr1->Next, Ptr2 = Ptr2->Next) {
		if (Ptr1->Type == NullNode || Ptr2->Type == NullNode)
		    continue;
		Edge1 = (EdgePointer) Ptr1->Entry;
		Edge2 = (EdgePointer) Ptr2->Entry;
		if (MatchEdges(Edge1, SubGraph->Name, Edge2, Node->Name)
		    == FALSE) {
		    fatal = TRUE;
		    return (fatal);
		}
	    }
    }
    else 
	ImposeAtomType(Node, Graph);

    return (fatal);
}

DeduceAtomType(Node, Graph)
NodePointer Node;
GraphPointer Graph;
{
    
    EdgePointer OutEdge;
    char *class, *Function, *Type;
    int a, b;

    InputTypes(Node);
    Function = GetFunction(Node);
    OutEdge = (Node->OutEdges != NULL)  ? 
	(EdgePointer) Node->OutEdges->Entry : NULL;
    CurrentNode = Node->Name;
    CurrentGraph = Graph->Name;

    /* Conditional operations require inputs of the same types but produce a
       bool */
    if (!strcmp(Function, "==") ||
        !strcmp(Function, "!=") ||
        !strcmp(Function, ">=") ||
        !strcmp(Function, ">")  ||
        !strcmp(Function, "<=") ||
        !strcmp(Function, "<")) {
	GetFirstDefinedType(InList, 0, NrInEdges-1, &class, &a, &b);
	CheckTypes(InList, 0, NrInEdges - 1, class, a, b);
	SetEdgeType(OutEdge, "fix", 1, 0);
    }
    /* For (+,-,/,++,--, &, |, ^, !, =), the output has the
       same type as the input edges */
    else if (!strcmp(Function, "+") ||
	     !strcmp(Function, "-") ||
	     !strcmp(Function, "/") ||
	     !strcmp(Function, "++") ||
	     !strcmp(Function, "--") ||
	     !strcmp(Function, "&") ||
	     !strcmp(Function, "|") ||
	     !strcmp(Function, "!") ||
	     !strcmp(Function, "^") ||
	     !strcmp(Function, "=")) {

	GetFirstDefinedType(InList, 0, NrInEdges-1, &class, &a, &b);
	CheckTypes(InList, 0, NrInEdges - 1, class, a, b);
	SetEdgeType(OutEdge, class, a, b);
    }

    /* Multiply definition : wl = wl1 + wl2, dc = dc1 + dc2 */
    else if (!strcmp(Function, "*")) {
	/* if (HasDefinedType(InList, 0) && HasDefinedType(InList, 1))
	    SetEdgeType(OutEdge, InList[0].class, 
		InList[0].a + InList[1].a, InList[0].b + InList[1].b); */
    }

    /* in a shift, the output has the same type as the first argument.
     * The second argument has to be an integer
     */
    else if (!strcmp(Function, ">>") ||
	!strcmp(Function, "<<")) {
	CheckTypes(InList, 1, 1, "fix", UNDEFINED, 0);
	GetFirstDefinedType(InList, 0, 0, &class, &a, &b);
	SetEdgeType(OutEdge, class, a, b);
    }

    /* The address of a write has to be an integer */
    else if (!strcmp(Function, "write")) {
	CheckTypes(InList, 1, 1, "fix", UNDEFINED, 0);
    }
    /* same for a read - nothing more can be said */
    else if (!strcmp(Function, "read")) {
	CheckTypes(InList, 0, 0, "fix", UNDEFINED, 0);
    }
    /* The type of a cast node can be found from the cast argument */
    else if (!strcmp(Function, "cast")) {
	Type = GetAttribute(Node->Arguments, "type");
	if (Type != NULL) {
	    ParseType(Type, &class, &a, &b);
	    SetEdgeType(OutEdge, class, a, b);
	}
    }
    else if (!strcmp (Function, "bit")) {
	CheckTypes(InList, 0, 0, "fix", UNDEFINED, 0);
	SetEdgeType(OutEdge, "fix", 1, 0);
    }
    else if (!strcmp (Function, "bitselect")) {
	CheckTypes(InList, 0, 0, "fix", UNDEFINED, 0);
	a = (int) GetAttribute(Node->Arguments, "width");
	if (a != 0)
	    SetEdgeType(OutEdge, "fix", a, 0);
    }
    else if (!strcmp (Function, "bitmerge")) {
	/* Inputs have to be integers */
	/* The merged vector is <wl1+wl2, 0> */
	CheckTypes(InList, 0, NrInEdges - 1, "fix", UNDEFINED, 0);
	if (HasDefinedType(InList, 0) && HasDefinedType(InList, 1))
	    SetEdgeType(OutEdge, InList[0].class, 
		InList[0].a + InList[1].a, 0);
    }
    else if (!strcmp (Function, "mux") ||
	     !strcmp (Function, "exit")) {
	CheckTypes(InList, 0, 0, "fix", 1, 0);
	GetFirstDefinedType(InList, 1, NrInEdges-1, &class, &a, &b);
	CheckTypes(InList, 1, NrInEdges - 1, class, a, b);
	SetEdgeType(OutEdge, class, a, b);
    }

    else {
	/* nothing can be said about constant nodes and user nodes */
	/* don't know what to say here */
    }
    return (fatal);
}

ImposeAtomType(Node, Graph)
NodePointer Node;
GraphPointer Graph;
{
    int DefinedOutput;
    int a, b;
    char *class, *Function;
    EdgePointer OutEdge;
    
    InputTypes(Node);
    Function = GetFunction(Node);
    OutEdge = (Node->OutEdges != NULL)  ? 
	(EdgePointer) Node->OutEdges->Entry : NULL;
    DefinedOutput = (OutEdge == NULL) ? 
	FALSE : ParseType(GetType(OutEdge), &class, &a, &b);
    CurrentNode = Node->Name;
    CurrentGraph = Graph->Name;

    /* Besides the fact that all inputs have to have the same types, we
       can say nothing more about the type of a conditional */
    if (!strcmp(Function, "==") ||
        !strcmp(Function, "!=") ||
        !strcmp(Function, ">=") ||
        !strcmp(Function, ">")  ||
        !strcmp(Function, "<=") ||
        !strcmp(Function, "<")) {
	GetFirstDefinedType(InList, 0, NrInEdges-1, &class, &a, &b);
	SetEdgeListType(InList, 0, NrInEdges-1, class, a, b);
    }
    /* For (+,-,/,++,--, &, |, ^, !, =), the output has the
       same type as the input edges */
    else if (!strcmp(Function, "+") ||
	     !strcmp(Function, "-") ||
	     !strcmp(Function, "/") ||
	     !strcmp(Function, "++") ||
	     !strcmp(Function, "--") ||
	     !strcmp(Function, "&") ||
	     !strcmp(Function, "|") ||
	     !strcmp(Function, "!") ||
	     !strcmp(Function, "^") ||
	     !strcmp(Function, "=")) {

	if (DefinedOutput == FALSE)
	    GetFirstDefinedType(InList, 0, NrInEdges-1, &class, &a, &b);
	SetEdgeListType(InList, 0, NrInEdges-1, class, a, b);
    }

    /* Let's not try to make assumptions about the inputs to a multiply
       at this point in time
     */
    else if (!strcmp(Function, "*")) {
    }

    /* in a shift, the output has the same type as the first argument.
     */
    else if (!strcmp(Function, ">>") ||
	!strcmp(Function, "<<")) {
	if (DefinedOutput == TRUE)
	    SetEdgeListType(InList, 0, 0, class, a, b);
    }

    else if (!strcmp (Function, "mux") ||
	     !strcmp (Function, "exit")) {
	if (DefinedOutput == FALSE)
	    GetFirstDefinedType(InList, 1, NrInEdges-1, &class, &a, &b);
	SetEdgeListType(InList, 1, NrInEdges-1, class, a, b);
    }

    /* Nothing can be said concerning read, write, constant, merge, select,
       cast and user defined nodes */
    return (fatal);
}

PropagateDelays(Graph)
GraphPointer Graph;
{
    /*
     * Propagates the results of the forward drive through the delays to
     * trigger the backward drive
     */
    NodePointer Node;
    EdgePointer OutEdge;
    char *class;
    int a, b;

    CurrentGraph = Graph->Name;
    
    for (Node = Graph->NodeList; Node != NULL; Node = Node->Next) {
	if (!IsDelayNode(Node))
	    continue;
	CurrentNode = Node->Name;
	InputTypes(Node);
	/* The delay-edge has to be integer */
	CheckTypes(InList, 1, 1, "fix", UNDEFINED, 0);
	OutEdge = (EdgePointer) Node->OutEdges->Entry;
	if (ParseType(GetType(OutEdge), &class, &a, &b) == FALSE)
	    continue;
    }

    for (Node = Graph->NodeList; Node != NULL; Node = Node->Next)
        if (IsHierarchyNode(Node))
            PropagateDelays(Node->Master);
}

CheckTypes(EdgeList, MinRange, MaxRange, Class, a, b)
struct DataType EdgeList[];
int MinRange, MaxRange;
char *Class;
int a, b;
{
    int i;
    if (Class == NULL)
	return;
    
    for (i = MinRange; i <= MaxRange && i < NrInEdges; i++) {
	if (EdgeList[i].edge == NULL || EdgeList[i].class == NULL)
	    continue;
	if (strcmp(EdgeList[i].class, Class) ||
	    (EdgeList[i].a != UNDEFINED && a != UNDEFINED &&
	     EdgeList[i].a != a) ||
	    (EdgeList[i].b != UNDEFINED && b != UNDEFINED && 
	     EdgeList[i].b != b)) {
	    fatal = TRUE;
	    printmsg("type-check",
		"Incompatible input types for node %s (graph %s)\n",
		CurrentNode, CurrentGraph);
	}
    }
}

GetFirstDefinedType(EdgeList, MinRange, MaxRange, Class, a, b)
struct DataType EdgeList[];
int MinRange, MaxRange;
char **Class;
int *a, *b;
{
    int i;

    *Class = NULL;
    *a = -1;
    *b = -1;

    for (i = MinRange; i <= MaxRange; i++) {
	if (EdgeList[i].class != NULL &&
	    EdgeList[i].a >= 0 &&
	    EdgeList[i].b >= 0)
	    break;
    }
    if (i <= MaxRange) {
	*Class = EdgeList[i].class;
	*a = EdgeList[i].a;
	*b = EdgeList[i].b;
    }
}

InputTypes(Node)
NodePointer Node;
{
    ListPointer Ptr;
    EdgePointer Edge;
    char *Type;
    int i;

    /* Create a list of the types of the input edges of the node */

    NrInEdges = 0;

    for (Ptr = Node->InEdges; Ptr != NULL; Ptr = Ptr->Next) {
	if (Ptr->Type == EdgeNode) {
	    Edge = (EdgePointer) Ptr->Entry;
	    InList[NrInEdges].edge = Edge;
	    Type = GetType(Edge);
	    ParseType(Type, &(InList[NrInEdges].class),
	        &(InList[NrInEdges].a), &(InList[NrInEdges].b));
	}
	else {
	    InList[NrInEdges].class = NULL;
	    InList[NrInEdges].edge = NULL;
	}

	if (++NrInEdges > MAX_NR_OF_EDGES) {
	    printmsg("InputOutputTypes",
		"Increase MAX_NR_OF_EDGES (currently set to 20)\n");
	    exit (-1);
	}
    }
    for (i = NrInEdges; i < 2; i++) {
	InList[i].edge = NULL;
	InList[i].class = NULL;
    }
}

SetEdgeType(Edge, Class, a, b)
EdgePointer Edge;
char *Class;
int a;
int b;
{
    char *OldClass, *Intern();
    char aString[64], bString[64], EdgeType[64];
    int Olda, Oldb;


    ParseType(GetType(Edge), &OldClass, &Olda, &Oldb);

    if (Class == NULL)
	return;
    if (OldClass != NULL && strcmp(OldClass, Class) ||
	a >= 0 && Olda >= 0 && a != Olda ||
	b >= 0 && Oldb >= 0 && b != Oldb) {
	fatal = TRUE;
	if (PrintWarning) 
	    printmsg("type-check",
	    "Incompatible type definitions for node %s (graph %s)\n",
	    CurrentNode, CurrentGraph);
	return;
    }
    if (a < 0) a = Olda;
    if (b < 0) b = Oldb;
    if (a < 0)
	sprintf(aString, "*");
    else
	sprintf(aString, "%d", a);
    if (b < 0)
	sprintf(bString, "*");
    else
	sprintf(bString, "%d", b);

    sprintf (EdgeType, "%s<%s,%s>", Class, aString, bString);
    SetType(Edge, EdgeType);
}

SetEdgeListType(EdgeList, MinRange, MaxRange, Class, a, b)
struct DataType EdgeList[];
int MinRange, MaxRange;
char *Class;
int a, b;
{
    int i;

	
    for (i = MinRange; i <= MaxRange; i++)
	if (EdgeList[i].edge != NULL)
	    SetEdgeType(EdgeList[i].edge, Class, a, b);
}

int
HasDefinedType(EdgeList, Index)
struct DataType EdgeList[];
int Index;
{
    return (EdgeList[Index].class == NULL || 
	    EdgeList[Index].a < 0 ||
	    EdgeList[Index].b < 0 ? FALSE : TRUE);
}

int
MatchEdges(Edge1, Graph1, Edge2, Graph2)
EdgePointer Edge1, Edge2;
char *Graph1, *Graph2;
{
    char *Type1, *Type2;

    Type1 = GetType(Edge1);
    Type2 = GetType(Edge2);

    if (Type1 == NULL && Type2 == NULL)
	return (TRUE);
    else if (Type1 == NULL) {
	SetType(Edge1, Type2);
	return (TRUE);
    }
    else if (Type2 == NULL) {
	SetType(Edge2, Type1);
	return (TRUE);
    }
    else if (AreIdenticalTypes(Type1, Type2))
	return (TRUE);

    if (PrintWarning)
        printmsg("type-check",
        "Incompatible types between edges %s (Graph %s) and %s (Graph %s)\n",
	Edge1->Name, Graph1, Edge2->Name, Graph2);
    /* return (FALSE); */
    return (TRUE);
}

int
AreIdenticalTypes(Type1, Type2)
char *Type1, *Type2;
{
    char *class1, *class2;
    int a1, a2, b1, b2;

    ParseType(Type1, &class1, &a1, &b1);
    ParseType(Type2, &class2, &a2, &b2);
    return (class1 == class2 && a1 == a2 && b1 == b2);
}

int
ParseType(String, Type, a, b)
char *String, **Type;
int *a, *b;
{
    char Str1[16], Str2[16], Str3[16];
    char *Ptr1, *Ptr2;
    long int strtol();

    *a = *b = -1;
    *Type = NULL;
    if (String == NULL)
	return (FALSE);

    *Str2 = *Str3 = '\0';

    for (Ptr1 = String, Ptr2 = Str1; *Ptr1 != '<' && *Ptr1 != '\0';
	 Ptr1++, Ptr2++) *Ptr2 = *Ptr1;
    *Ptr2 = '\0';
    if (*Ptr1 != '\0') {
	for (++Ptr1, Ptr2 = Str2; *Ptr1 != ',' && *Ptr1 != '>' && *Ptr1 != '\0';
	    Ptr1++, Ptr2++) *Ptr2 = *Ptr1;
	*Ptr2 = '\0';
    }
    if (*Ptr1 != '\0' && *Ptr1 != '>') {
	for (++Ptr1, Ptr2 = Str3; *Ptr1 != '>' && *Ptr1 != '\0';
	    Ptr1++, Ptr2++) *Ptr2 = *Ptr1;
	*Ptr2 = '\0';
    }

    if (!strcmp(Str1, "UndefType")) {
	*Type = Intern("fix");
	*a = -1;
	*b = -1;
	return (FALSE);
    }
    if (!strcmp(Str1, "bool")) {
	*Type = Intern("fix");
	*a = 1;
	*b = 0;
	return (TRUE);
    }
    if (!strcmp(Str1, "int"))
	    *Type = Intern("fix");
    else
	    *Type = Intern(Str1);

    if (*Str2 != '\0') {
	*a = (int) strtol(Str2, &Ptr1, 10);
	if (Ptr1 == Str2)
	    *a = -1;
    }

    if (*Str3 != '\0') {
	*b = (int) strtol(Str3, &Ptr1, 10);
	if (Ptr1 == Str3)
	    *b = -1;
    }
    else if (*a >= 0)
	*b = 0;

    return (*Type == NULL || *a < 0 || *b < 0 ? FALSE : TRUE);

}

char *
ConstructType(Class, W, D)
char *Class;
int W;
int D;
{
    char Type[WORDLENGTH];

    sprintf(Type, "%s<%d,%d>", Class, W, D);
    return(Intern(Type));
}
