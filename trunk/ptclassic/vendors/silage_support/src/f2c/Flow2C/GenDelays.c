/*   $Id$ */

#include "codegen.h"   /* for RootGraph structure, has dag.h, flowgraph.h  */
#include "st.h"

pointer GetAttribute();
ListPointer SetAttribute(), ListAppend(), new_list();
char *BaseName(), *Intern();
bool IsArray(), IsFixedType(), IsIntType();
bool IsInput(), IsOutput();
bool IsLoop(), IsFunc(), IsSampDelayNode(), IsLpDelayNode();

extern FILE *CFD, *LogFile;
extern int strcmp();
extern GraphPointer Root;

static st_table *GraphTable;
static int InitStruct = FALSE;

DecorateDelays()
{
    GraphPointer Graph;

    for (Graph = Root; Graph != NULL; Graph = Graph->Next) {
        AddDelayAttribute(Graph);
    }
    GraphTable = st_init_table(strcmp, st_strhash);
    BuildDelayExist(Root);
    st_free_table(GraphTable);
}

BuildDelayExist(Graph)
GraphPointer Graph;
{
    NodePointer Node;
   
    for (Node = Graph->NodeList; Node != NULL; Node = Node->Next) {
	if (IsHierarchy(Node)) {
            if (st_lookup(GraphTable, Node->Master->Name, NULL) == 0) {
                st_insert(GraphTable, Intern(Node->Master->Name), 0);
		BuildDelayExist(Node->Master);
	    }
	    if (GE(Node->Master)->HasDelay)
                GE(Graph)->HasDelay = true;
	}
    }
    if (GE(Graph)->ListOfDelays != NULL)
        GE(Graph)->HasDelay = true;
}

AddDelayAttribute(Graph)
GraphPointer Graph;
{
    NodePointer Node;

    for (Node = Graph->NodeList; Node != NULL; Node = Node->Next) {
/*	if (IsFunc(Graph) && IsSampDelayNode(Node)) {   */
	if (IsFunc(Graph) && IsDelayNode(Node)) {
	    AddDelayAttr(Node, Graph, Graph);
	}
	else if (IsLoopNode(Node)) {
	    AddDelayAttrInLoop(Node, Graph);
	    AddLoopDelayAttr(Node, Graph);
	}
    }
}

AddDelayAttrInLoop(IterNode, Graph)
NodePointer IterNode;
GraphPointer Graph;
{
    NodePointer Node;

    for (Node = IterNode->Master->NodeList; Node != NULL; Node = Node->Next) {
	if (IsSampDelayNode(Node)) {
	    AddDelayAttr(Node, Graph, Graph);
	}
	else if (IsLoopNode(Node)) {
	    AddDelayAttrInLoop(Node, Graph);
	}
    }
}

AddLoopDelayAttr(IterNode, Graph)
NodePointer IterNode;
GraphPointer Graph;
{
    NodePointer Node;

    for (Node = IterNode->Master->NodeList; Node != NULL; Node = Node->Next) {
	if (IsLpDelayNode(Node)) {
	    AddDelayAttr(Node, IterNode->Master, Graph);
	}
	else if (IsLoopNode(Node)) {
	    AddLoopDelayAttr(Node, Graph);
	}
    }
    TransferDelayAttr(IterNode->Master, IterNode);
}

AddDelayAttr(Node, CurrentGraph, MasterGraph)
NodePointer Node;
GraphPointer CurrentGraph, MasterGraph;
{
    NodePointer PrevNode, NextNode;
    EdgePointer PrimeInEdge, OutEdge;
    int delay, Maxdelay, IsLoop;
    char str[STRSIZE], *delayLb, *MaxdelayLb, *delaysizeLb;
    char c, name[STRSIZE];
    ListPointer entry, OutNodes, InNodes, *ListOfDelays, *LocalListOfDelays;

    if (IsLpDelayNode(Node)) {
	IsLoop = TRUE;
        c = '#';
	delayLb = "loopdelay";
        delaysizeLb = "loopdelaysize";
 	MaxdelayLb = "Maxloopdelay";
	ListOfDelays = &(GE(MasterGraph)->ListOfLoopDelays);
	LocalListOfDelays = &(GE(CurrentGraph)->ListOfLoopDelays);
    } else {
        IsLoop = FALSE;
        c = '@';
	delayLb = "delay";
        delaysizeLb = "delaysize";
	MaxdelayLb = "Maxdelay";
	ListOfDelays = &(GE(MasterGraph)->ListOfDelays);
    }

    OutEdge = EP(Node->OutEdges);
    if (HasAttribute(OutEdge->Attributes, delayLb)) return;
/* trace back to primary input edge of delay line */
    PrimeInEdge = EP(Node->InEdges);
    InNodes = PrimeInEdge->InNodes;
    while (InNodes != NULL && InNodes->Type == NodeNode) {
	PrevNode = NP(InNodes);
        if ((!IsLoop && IsSampDelayNode(PrevNode)) || 
	     			(IsLoop && IsLpDelayNode(PrevNode))) {
	    PrimeInEdge = EP(PrevNode->InEdges);
            InNodes = PrimeInEdge->InNodes;
	}
	else {
	    InNodes = InNodes->Next;
	}
    }
/* trace forward to determine size of delay line */
    OutNodes = PrimeInEdge->OutNodes;
    Maxdelay = 0;
    while (OutNodes != NULL && OutNodes->Type == NodeNode) {
        NextNode = NP(OutNodes);
	if ((!IsLoop && IsSampDelayNode(NextNode)) ||
	     			(IsLoop && IsLpDelayNode(NextNode))) {
	    OutEdge = EP(NextNode->OutEdges);
  	    OutNodes = OutEdge->OutNodes;
	    Maxdelay += GetDelayCnt(NextNode, UPPER);
	}
	else {
	    OutNodes = OutNodes->Next;
	}
    }
/* Add "Maxdelay" Attribute to PrimeInEdge, and add to ListOfDelays */
    PrimeInEdge->Attributes = SetAttribute(MaxdelayLb, IntNode,
	        (pointer) Maxdelay, PrimeInEdge->Attributes);
    entry = new_list(EdgeNode, (pointer) PrimeInEdge);
    *ListOfDelays=ListAppend(entry, *ListOfDelays);
    if (IsLoop) {
        entry = new_list(EdgeNode, (pointer) PrimeInEdge);
        *LocalListOfDelays=ListAppend(entry, *LocalListOfDelays);
    }
/* Add "delaysize" and "delay" Attribute to Successor OutEdge */
    OutNodes = PrimeInEdge->OutNodes;
    delay = 0;
    while (OutNodes != NULL && OutNodes->Type == NodeNode) {
	NextNode = NP(OutNodes);
	if ((!IsLoop && IsSampDelayNode(NextNode)) ||
	     			(IsLoop && IsLpDelayNode(NextNode))) {
            OutEdge = EP(NextNode->OutEdges);
            OutNodes = (ListPointer) OutEdge->OutNodes;
	    if (NextNode->InEdges->Next == NULL || NextNode->InEdges->Next->
				Type == NullNode) { /* constant delay */
	        delay += GetDelayCnt(NextNode, UPPER);
	        sprintf(str, "%d", delay);
	    } else   /* variable delay, return name */
		sprintf(str, "%s", EP(NextNode->InEdges->Next)->Name);
	    OutEdge->Attributes = SetAttribute(delayLb, CharNode,
			            Intern(str), OutEdge->Attributes);
	    OutEdge->Attributes = SetAttribute(delaysizeLb, IntNode,
			            Maxdelay+1, OutEdge->Attributes);
/* Change name of OutEdge to be PrimeInEdge->Name@delay if not already */
	    sprintf(name, "%s%c%s", PrimeInEdge->Name, c, str);
	    if (strcmp(name, OutEdge->Name)) /* name is different */
		OutEdge->Name = Intern(name);
	}
	else {
	    OutNodes = OutNodes->Next;
	}
    }
}

GenDelayStruct(Graph)
GraphPointer Graph;
{

    if (Graph == NULL) return;
        GenDelayStruct(Graph->Next);
    if (IsFunc(Graph) && GE(Graph)->HasDelay) {
 	fprintf(CFD, "typedef struct {\n");
	GenDelayDef(GE(Graph)->ListOfDelays);
	GenDelayHierarchy(GE(Graph)->ListOfFuncApps);
	fprintf(CFD, "} ");
	GenDelayStructName(Graph);
	fprintf(CFD, ";\n\n");
    }
}

GenDeclDelayVars(Graph)
GraphPointer Graph;
{
    ListPointer LOD;
    register ListPointer ptr;
    register EdgePointer Edge;
    char *DeclName();
    bool IsDeclaredEdge();
    extern int indexlevel;
    extern st_table *Edgetable[];

    LOD = GE(Graph)->ListOfLoopDelays;
    for (ptr = LOD; ptr != NULL; ptr = ptr->Next) {
        Edge = EP(ptr);
        if (IsDeclaredEdge(Edge)) continue;
        if (!HasAttribute(Edge->Attributes, "Maxloopdelay")) {
            Error("GenDeclDelayVars() : No Maxloopdelay Attr.\n");
        }
        if (IsIntType(Edge))
            fprintf(CFD, "    int ");
        else
            fprintf(CFD, "    Sig_Type ");
        GenEdgeDecl(Edge);
        fprintf(CFD, ";\n");
        fprintf(CFD, "    int ");
        GenEdgeName(Edge);
        fprintf(CFD, "_C;\n");
        st_insert(Edgetable[indexlevel], DeclName(Edge), 0);
    }

    if (LOD != NULL) {
	Indent();
	fprintf(CFD, "Sig_Type c0;\n\n");
    }
}

GenDelayDef(LOD)
ListPointer LOD;
{
    register ListPointer ptr;
    register EdgePointer Edge;
    char *DeclName();
    bool IsDeclaredEdge();
    extern int indexlevel;
    extern st_table *Edgetable[];

    for (ptr = LOD; ptr != NULL; ptr = ptr->Next) {
	Edge = EP(ptr);
	if (!HasAttribute(Edge->Attributes, "Maxdelay")) {
	    Error("GenDelayDef() : No Maxdelay Attr.\n");
        }
	if (IsIntType(Edge))
	    fprintf(CFD, "    int ");
        else
	    fprintf(CFD, "    Sig_Type ");
	GenEdgeDecl(Edge);
	fprintf(CFD, ";\n");
	fprintf(CFD, "    int ");
	GenEdgeName(Edge);
	fprintf(CFD, "_C;\n");
    }
}

GenDelayHierarchy(LOFA)
ListPointer LOFA;
{
    register ListPointer ptr;
    register GraphPointer Graph;
    register NodePointer Node;

    for (ptr = LOFA; ptr != NULL; ptr = ptr->Next) {
        Node = NP(ptr);
	Graph = Node->Master;
        if (GE(Graph)->HasDelay) {
	    Indent();
	    GenDelayStructName(Graph);
	    fprintf(CFD, " ");
	    GenDelayInstanceName(Graph, Node);
	    if ((int)ptr->Label > 1)
		fprintf(CFD, "[%d]", (int)ptr->Label);
	    fprintf(CFD, ";\n");
	}
    }
}

GenDelayInstanceName(Graph, Node)
GraphPointer Graph;
NodePointer Node;
{
    fprintf( CFD, "%s_%s", Graph->Name, Node->Name);
}

GenDelayStructName(Graph)
GraphPointer Graph;
{
    fprintf(CFD, "%s_ST", Graph->Name);
}

GenDelayStructInits(GraphList,pl_flag)
GraphPointer GraphList;
bool pl_flag;
{
    GraphPointer Graph,tmp;

    fprintf (CFD, "/* Initializing Delay structure */\n\n");
    if(!pl_flag)
    {
    for (Graph = GraphList; Graph != NULL; Graph = Graph->Next) {
	if (IsFunc(Graph) && GE(Graph)->HasDelay) {
	    fprintf (CFD, "Init_%s (pST)\n    ", Graph->Name);
            GenDelayStructName(Graph);
	    fprintf(CFD, " *pST;\n");

     	    fprintf(CFD, "{\n");
            if ((GE(Graph)->ListOfDelays != NULL) || 
			(GE(Graph)->ListOfFuncApps != NULL)) {
		Indent();
		fprintf(CFD, "Sig_Type c0;\n");
		Indent();
		fprintf(CFD, "int i;\n\n");
    	    }
	    GenInitDelayPointers(GE(Graph)->ListOfDelays);
            GenInitDelayHierarchy(GE(Graph)->ListOfFuncApps);
            GenInitDelayValues(GE(Graph)->ListOfDelays);
	    fprintf(CFD, "}\n\n");
        }
    }
    } /* if !pl_flag */
    if(pl_flag)
    {
    tmp = (GraphPointer) malloc (sizeof(Graph));
    tmp = NULL;

    while(tmp != Root)
    {
    for (Graph = GraphList; Graph->Next != tmp; Graph = Graph->Next);
	tmp = Graph;
	if (IsFunc(Graph) && GE(Graph)->HasDelay) {
     	    fprintf (CFD, "void Init_%s ( ", Graph->Name);
            GenDelayStructName(Graph);
	    fprintf(CFD, "* pST)\n");
	    
     	    fprintf(CFD, "{\n");
            if ((GE(Graph)->ListOfDelays != NULL) || 
			(GE(Graph)->ListOfFuncApps != NULL)) {
		Indent();
		fprintf(CFD, "Sig_Type c0;\n");
		Indent();
		fprintf(CFD, "int i;\n\n");
    	    }
	    GenInitDelayPointers(GE(Graph)->ListOfDelays);
            GenInitDelayHierarchy(GE(Graph)->ListOfFuncApps);
            GenInitDelayValues(GE(Graph)->ListOfDelays);
	    fprintf(CFD, "}\n\n");
        }
    } /* while */
    } /* if pl_flag */
}

GenInitDelayVars(Graph)
GraphPointer Graph;
{
    ListPointer ptr;
    EdgePointer Edge;

    for (ptr = GE(Graph)->ListOfLoopDelays; ptr != NULL; ptr = ptr->Next) {
	Edge = EP(ptr);
        if (!HasAttribute(Edge->Attributes, "Maxloopdelay")) {
            Error("GenInitDelayVars() : No Maxloopdelay Attr.\n");
        }
        Indent();
	GenEdgeName(Edge);
	fprintf(CFD, "_C = 0;\n");
    }
    GenInitDelayValues(GE(Graph)->ListOfLoopDelays);
}

GenInitDelayPointers(LOD)
ListPointer LOD;
{
    register ListPointer ptr;
    register EdgePointer Edge;

    for (ptr = LOD; ptr != NULL; ptr = ptr->Next) {
	Edge = EP(ptr);
	if (!HasAttribute(Edge->Attributes, "Maxdelay")) {
	    Error("GenInitDelayPointers() : No Maxdelay Attr.\n");
        }
	Indent();
	fprintf(CFD, "pST->");
	GenEdgeName(Edge);
	fprintf(CFD, "_C = 0;\n");
    }
}

GenInitDelayHierarchy(LOFA)
ListPointer LOFA;
{
    register ListPointer ptr;
    register GraphPointer Graph;
    register NodePointer Node;

    for (ptr = LOFA; ptr != NULL; ptr = ptr->Next) {
        Node = NP(ptr);
        Graph = Node->Master;
	if (GE(Graph)->HasDelay) {
	    Indent();
	    if ((int)ptr->Label > 1)
		fprintf(CFD, "for (i=0; i<%d; i++)\n\t", (int)ptr->Label);
	    fprintf(CFD, "Init_%s (&(pST->", Graph->Name);
	    GenDelayInstanceName(Graph, Node);
	    if ((int)ptr->Label > 1)
		fprintf(CFD, "[i]");
	    fprintf(CFD, "));\n");
	}
    }
}

GenInitDelayValues(LOD)
ListPointer LOD;
{
    register ListPointer ptr, OutNodes;
    register EdgePointer Edge, OutEdge, InitEdge;
    register NodePointer NextNode;
    EdgePointer GetNthEdge();
    extern int InitStruct;
    int Maxdelay;
    char *init;

/* for each delay line, convert initial values to FixedPoint, and initialize */
    for (ptr = LOD; ptr != NULL; ptr = ptr->Next) {
	Edge = EP(ptr);
        OutNodes = Edge->OutNodes;
        while (OutNodes != NULL && OutNodes->Type == NodeNode) {
            NextNode = NP(OutNodes);
	    if (IsSampDelayNode(NextNode) || IsLpDelayNode(NextNode)) {
  		if (NextNode->InEdges->Next != NULL && NextNode->InEdges->
			Next->Type != NullNode) { /* Initialize in a loop */
		     if (HasAttribute(Edge->Attributes, "Maxdelay"))
	                 Maxdelay = (int)GetAttribute(Edge->Attributes, 
								"Maxdelay");
		     else
	                 Maxdelay = (int)GetAttribute(Edge->Attributes, 
							"Maxloopdelay");
	             init = (char *)GetAttribute(NextNode->Arguments, "init");
		     if (init == NULL) init = Intern("0");
                     OutEdge = EP(NextNode->OutEdges);
		     if (IsFixedType(OutEdge)) {
	                 Indent();
	                 fprintf(CFD, "Float2Fix (%s, c0", init);
	                 GenFixedType(OutEdge);
	                 fprintf(CFD, ");\n");
		     }
	             Indent();
	             fprintf(CFD, "for (i = 1; i < %d; i++)\n", Maxdelay);
	             Indent(); Indent();
		     if (IsIntType(OutEdge)) {
		         InitStruct = TRUE;
		         GenDelayEdgeRef(OutEdge);
		         InitStruct = FALSE;
			 fprintf(CFD, " = %s;\n", init);
		     } else {
	                 fprintf(CFD, "FixAssign (c0, ");
		         InitStruct = TRUE;
	                 GenDelayEdgeRef(OutEdge);
		         InitStruct = FALSE;
	                 fprintf(CFD, ");\n");
		     }
		     OutNodes = OutNodes->Next;
	        } else {
	             OutEdge = EP(NextNode->OutEdges);
                     OutNodes = (ListPointer) OutEdge->OutNodes;
                     InitEdge = GetNthEdge (NextNode, IN, 2, "data");
                     if (InitEdge == NULL) {
                       init = (char *)GetAttribute(NextNode->Arguments, "init");
                       if (init == NULL) init = Intern("0");
                       Indent();
                       if (IsIntType(OutEdge)) {
                         GenDelayEdgeRef(OutEdge);
                         fprintf(CFD, " = %s;\n", init);
                       } else {
                         fprintf(CFD, "Float2Fix (%s, c0", init);
                         GenFixedType(OutEdge);
                         fprintf(CFD, ");\n");
                         Indent();
                         fprintf(CFD, "FixAssign (c0, ");
                         GenDelayEdgeRef(OutEdge);
                         fprintf(CFD, ");\n");
                       }
                     } else {
                       Indent();
                       fprintf(CFD, "FixAssign (");
		       if (InitEdge->InNodes != NULL && InitEdge->InNodes->
			 Type == NodeNode && IsReadNode(NP(InitEdge->InNodes)))
			   GenSingleNode(NP(InitEdge->InNodes));
                       GenOperand(InitEdge);
                       fprintf(CFD, ", ");
                       GenDelayEdgeRef(OutEdge);
                       fprintf(CFD, ");\n");
                     }
                }
	    }
	    else {
		OutNodes = OutNodes->Next;
	    }
	}
    }
}

GenDelayEdgeRef(edge)
EdgePointer edge;
{
    if (IsArray(edge)) {
	GenIndexedEdgeRef(edge);
	GenDelayRef(edge);
	GenEdgeDim(edge);
    }
    else {
	GenSingleEdgeRef(edge);
	GenDelayRef(edge);
    }
}

/*
 * This routine is also called in GenInitDelayValues to initialize delay
 * variables.  When it does this, it use the common index "i", which we
 * have to test here.  Using the edge's own name is too complicated, cuz
 * we would have to declare the name too and I am too lazy to do it.
 */
GenDelayRef(edge)
EdgePointer edge;
{
    int delaysize;
    char *delay;
    extern int InitStruct;

    if (HasAttribute(edge->Attributes, "delay")) {
        delay = (char *)GetAttribute(edge->Attributes, "delay");
        delaysize = (int)GetAttribute(edge->Attributes, "delaysize");
	if (InitStruct == TRUE)
            fprintf(CFD, "[at (i, pST->"); /* major kluge!!!! */
	else
            fprintf(CFD, "[at (%s, pST->", delay);
        GenEdgeName(edge);
        fprintf(CFD, "_C, %d)]", delaysize);
    } else if (HasAttribute(edge->Attributes, "loopdelay")) {
        delay = (char *)GetAttribute(edge->Attributes, "loopdelay");
        delaysize = (int)GetAttribute(edge->Attributes, "loopdelaysize");
        fprintf(CFD, "[at (%s, ", delay);
        GenEdgeName(edge);
        fprintf(CFD, "_C, %d)]", delaysize);
    }
}

GenIncrementDelays(LOD)
ListPointer LOD;
{
    register ListPointer ptr;
    register EdgePointer Edge;
    register int maxdelay;

    for (ptr = LOD; ptr != NULL; ptr = ptr->Next) {
        Edge = EP(ptr);
        if (!HasAttribute(Edge->Attributes, "Maxdelay")) {
            Error("GenIncrementDelays() : No Maxdelay Attr.\n");
        }
	maxdelay = (int)GetAttribute(Edge->Attributes,"Maxdelay");
	Indent();
	fprintf(CFD, "if (--pST->");
	GenEdgeName(Edge);
	fprintf(CFD, "_C<0) pST->");
	GenEdgeName(Edge);
	fprintf(CFD, "_C = %d;\n", maxdelay);
    }
}

GenIncrementLoopDelays(Graph)
GraphPointer Graph;
{
    GenIncrementLoopDelaysInEdge(Graph->EdgeList);
    GenIncrementLoopDelaysInEdge(Graph->ControlList);
}

GenIncrementLoopDelaysInEdge(List)
EdgePointer List;
{
    char str[STRSIZE];
    register EdgePointer Edge;
    register int maxloopdelay;
    int IsProcess;

    for (Edge = List; Edge != NULL; Edge = Edge->Next) {
        if (HasAttribute(Edge->Attributes, "Maxdelay")) {
	    IsProcess = TRUE;
	    sprintf(str, "Maxdelay");
        } else if (HasAttribute(Edge->Attributes, "Maxloopdelay")) {
	    IsProcess = FALSE;
	    sprintf(str, "Maxloopdelay");
	} else
	    continue;
	maxloopdelay = (int)GetAttribute(Edge->Attributes, str);
	Indent();
	fprintf(CFD, "if (--");
	if (IsProcess)
	    fprintf(CFD, "pST->");
	GenEdgeName(Edge);
	fprintf(CFD, "_C<0) ");
	if (IsProcess)
	    fprintf(CFD, "pST->");
	GenEdgeName(Edge);
	fprintf(CFD, "_C = %d;\n", maxloopdelay);
    }
}

GenDecrementLoopDelays(Graph, IsProcess)
GraphPointer Graph;
int IsProcess;
{
    GenDecrementLoopDelaysInEdge(Graph->EdgeList, IsProcess);
    GenDecrementLoopDelaysInEdge(Graph->ControlList, IsProcess);
}

GenDecrementLoopDelaysInEdge(List, IsProcess)
EdgePointer List;
int IsProcess;
{
    char str[STRSIZE];
    register EdgePointer Edge;
    register int maxloopdelay;

    if (IsProcess)
	sprintf(str, "Maxdelay");
    else
	sprintf(str, "Maxloopdelay");

    for (Edge = List; Edge != NULL; Edge = Edge->Next) {
        if (HasAttribute(Edge->Attributes, str)) {
	    maxloopdelay = (int)GetAttribute(Edge->Attributes, str);
	    Indent();
	    if (IsProcess)
	        fprintf(CFD, "pST->");
	    GenEdgeName(Edge);
	    fprintf(CFD, "_C = (");
	    if (IsProcess)
	        fprintf(CFD, "pST->");
	    GenEdgeName(Edge);
	    fprintf(CFD, "_C+1)%%");
	    fprintf(CFD, "%d;\n", maxloopdelay+1);
        }
    }
}

TransferDelayAttr(Graph, Node)
GraphPointer Graph;
NodePointer Node;
{
    ListPointer ptr1, ptr2;
    EdgePointer Gedge, Nedge;
    register int maxloopdelay;

    for (ptr1 = Graph->OutEdges, ptr2 = Node->OutEdges; ptr1 != NULL;
				 ptr1 = ptr1->Next, ptr2 = ptr2->Next) {
	if (ptr1->Type != EdgeNode || ptr2->Type != EdgeNode) continue;
	Gedge = EP(ptr1);
	Nedge = EP(ptr2);
/* Usually, all OutEdges of Iterators are LoopDelay edges.  But for DoLoops,
   in addition to any LoopDelay edges, we also have those boolean test edges. */
        if (HasAttribute(Gedge->Attributes, "Maxloopdelay")) {
	    maxloopdelay = (int)GetAttribute(Gedge->Attributes,"Maxloopdelay");
            Nedge->Attributes = SetAttribute("Maxloopdelay", IntNode, (pointer)
	 	maxloopdelay, Nedge->Attributes);
        }
    }
}

char *DelayName(edge)
EdgePointer edge;
{
    static char str[STRSIZE];
    char *p1, *p2;
    int i;

    for(p1 = str, p2 = edge->Name, i = 0; *p2 != NULL; p1++, p2++, i++) {
        if (*p2 == '@' || *p2 == '#' || i == STRSIZE) break;
        *p1 = *p2;
    }
    *p1 = '\0';
    return(str);
}
