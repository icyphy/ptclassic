/* $Id$ */

#include "codegen.h"    /* for RootGraph structure, has dag.h, flowgraph.h  */
#include "st.h"

#define MAX(A,B) ((A) > (B) ? (A) : (B))
#define MIN(A,B) ((A) < (B) ? (A) : (B))

pointer GetAttribute();
ListPointer SetAttribute(), InheritAttribute(), ListAppend(), new_list();
char *BaseName(), *DelayName(), *GetFunction(), *Intern(), *EdgeName();
char *DeclName();
bool IsArray(), IsFixedType(), IsIntType(), IsBoolType(), IsHierarchy();
bool IsFuncNode(), IsDelayOut();

extern int ForcedFlag;
extern int interactiveFlag;
extern FILE *CFD, *LogFile;
extern int strcmp();
extern GraphPointer Root;
extern char *DumpFile;
extern char FlowGraphName[];

/*
 * Simulation Control Flags
 */

int HighLevelFlag = 2;
int FFTFlag = 0;
int LinearFlag = 1;
int MagnitudeFlag = 0;

ListPointer ListOfParams;
ListPointer ListOfFixedLeafs; 
ListPointer ListOfInputs = NULL;
ListPointer ListOfDisplays = NULL;
int indexlevel;
int NrOfCycles = 1;
st_table *Edgetable[MAXINDEXLEVEL];

static st_table *GraphTable;

FlowToC(l_flag,pl_flag,highlevel,bittrue,tmp_thor,sdf_flag)
bool l_flag;
bool pl_flag;
bool highlevel;
bool bittrue;
bool tmp_thor;
bool sdf_flag;
{
    printmsg(NULL, "\n\n\t\t\t*** STARTING CODE GENERATION ***\n\n");
    /* input commands first */
    NrOfCycles = (interactiveFlag == TRUE) ? ReadInteractive() : ReadComFile();
    printmsg(NULL, "REMOVING CONSTANT NODES...\n\n");
    RemoveAllConstNodes(Root);
    ExpandFuncNodesWithLpDelay(Root, NULL, NULL);
    ListOfFixedLeafs = NULL; 
    BuildRootGraph();
    EnforceArrayName(Root);
    AddEqualNodes(Root);
    EnforceEdgeName(Root);
    InheritAttr(Root);
    printmsg(NULL, "ADDING DELAY ATTRIBUTES TO GRAPH...\n\n");
    DecorateDelays();
    EnforceBoolType(Root);
    printmsg(NULL, "GENERATING INITIALIZATION CODE...\n\n");
/*    DumpFlowGraph(Root, FALSE);  */
    GenCode(l_flag,pl_flag,highlevel,bittrue,tmp_thor,sdf_flag); 
}

BuildRootGraph()
{
    GraphTable = st_init_table(strcmp, st_strhash);
    BuildGraphList(Root, Root, 1);
    PrintGraphList();
    st_free_table(GraphTable);
}

BuildGraphList(graph, ParentFuncGraph, num)
    GraphPointer graph;
    GraphPointer ParentFuncGraph;
    int num;
{
    NodePointer Node, RemoveFromNodeList();
    pointer NewGraphExt(), NewEdgeExt(), NewNodeExt();
    EdgePointer Edge;
    GraphPointer FuncGraph;
    int min, max, newnum;
    ListPointer entry;
    char *value;
    bool IsFunc();

    graph->Extension = NewGraphExt();
    graph->Next = NULL;
    GraphAppend(graph);

    if (IsFunc(graph))
	LabelIOedges(graph);
    for(Edge = graph->EdgeList; Edge != NULL; Edge = Edge->Next) {
	Edge->Extension = NewEdgeExt();
	EE(Edge)->graph = graph;
        if (IsFixedType(Edge) && HasAttribute(Edge->Attributes, "value")) {
	    value = (char*)GetAttribute(Edge->Attributes, "value");
	    if (!isnumber(value)) continue;
	    entry = new_list(EdgeNode, (pointer)Edge );
	    ListOfFixedLeafs = ListAppend(entry, ListOfFixedLeafs);
	}
    }
    for(Edge = graph->ControlList; Edge != NULL; Edge = Edge->Next) {
	Edge->Extension = NewEdgeExt();
	EE(Edge)->graph = graph;
    }
    for(Node = graph->NodeList; Node != NULL; Node = Node->Next) {
	Node->Extension = NewNodeExt();
	newnum = num;
        if (IsHierarchy(Node)) {
	    if (IsFuncNode(Node)) {
 	       	entry = new_list(NodeNode, (pointer) Node);
		entry->Label = (char *)num;
	       	GE(ParentFuncGraph)->ListOfFuncApps = ListAppend(entry, 
					GE(ParentFuncGraph)->ListOfFuncApps);
		if (num > 1)
		    Node->Attributes = SetAttribute("NrOfApps", IntNode, 
						(char *)num, Node->Attributes);
		FuncGraph = Node->Master;
	    }
	    else if (IsLoopNode(Node)) {
		FuncGraph = ParentFuncGraph;
	        if (IsIterationNode(Node) && !IsProcessNode(Node)) {
    		    min = (int)GetAttribute(Node->Arguments, "min");
    		    max = (int)GetAttribute(Node->Arguments, "max");
		    newnum = newnum*(max-min+1);
		}
	    }
	    if (st_lookup(GraphTable, Node->Master->Name, NULL) == 0) {
		st_insert(GraphTable, Intern(Node->Master->Name), 0);
		BuildGraphList(Node->Master, FuncGraph, newnum);
	    }
	}
    }
}

GraphAppend(graph)
GraphPointer graph;
{
    register GraphPointer p;
 
    if (graph == Root) return;
    for (p = Root; p->Next != NULL; p = p->Next);
    p->Next = graph;
    return;
}

PrintGraphList()
{
    register ListPointer ptr;
    register GraphPointer graph, Graph;
    register char *str;

    printmsg(NULL, "ListOfFixedLeafs:\n");
    for (ptr = ListOfFixedLeafs; ptr != NULL; ptr = ptr->Next) {
        str =(char *)GetAttribute(EP(ptr)->Attributes,"value");
	    printmsg(NULL, "%s, ", str);
    }
    printmsg(NULL, "\n\nList of Functions:\n");
    for (graph = Root; graph != NULL; graph = graph->Next) {
	printmsg(NULL, "\nFunction Name: %s\n", graph->Name);
	printmsg(NULL, "List of Func Apps: ");
	for (ptr = GE(graph)->ListOfFuncApps; ptr != NULL; ptr = ptr->Next){
	    Graph = NP(ptr)->Master;
	    printmsg(NULL, "%s ", Graph->Name);
	    if ((int)ptr->Label > 1)
	        printmsg(NULL, "(%d),", (int)ptr->Label);
	    else
	        printmsg(NULL, ",");
        }
	printmsg(NULL, "\n");
	if (GE(graph)->HasDelay)
        printmsg(NULL, "Delay EXISTS.\n\n");
    }
}

LabelIOedges(graph)
GraphPointer graph;
{
    LabelInedges(graph->InEdges);
    LabelInedges(graph->InControl);
    LabelOutedges(graph->OutEdges);
    LabelOutedges(graph->OutControl);
}

LabelInedges(List)
ListPointer List;
{
    register ListPointer ptr;

    for (ptr = List; ptr != NULL; ptr = ptr->Next)
	EP(ptr)->Attributes = SetAttribute("input", IntNode, (char *)1, 
							EP(ptr)->Attributes);
}

LabelOutedges(List)
ListPointer List;
{
    register ListPointer ptr;

    for (ptr = List; ptr != NULL; ptr = ptr->Next)
	EP(ptr)->Attributes = SetAttribute("output", IntNode, (char *)1, 
							EP(ptr)->Attributes);
}

InheritAttr(Graph)
GraphPointer Graph;
{
    GraphPointer graph;
    NodePointer Node;

    for (graph = Graph; graph != NULL; graph = graph->Next) {
        if (!IsFunc(graph)) continue;
        for (Node = graph->NodeList; Node != NULL; Node = Node->Next) {
            if (IsLoopNode(Node)) {
                InheritInOutAttr(Node, Node->Master);
                EnforceNameInheritance(Node, Node->Master);
            }
        }
    }
}

EnforceNameInheritance(TopNode, SubGraph)
NodePointer TopNode;
GraphPointer SubGraph;
{
    /* Input and Output data edges */
    EnforceDataEdgeNameInheritance(TopNode->InEdges, SubGraph->InEdges);
    EnforceDataEdgeNameInheritance(TopNode->OutEdges, SubGraph->OutEdges);
    /* Input and Output control edges */
    EnforceArrayEdgeNameInheritance(TopNode, SubGraph);
}

EnforceDataEdgeNameInheritance(List1, List2)
ListPointer List1, List2;
{
    ListPointer K, KK;
    EdgePointer E, SE, GetNthGraphEdge();

    for (K = List1, KK = List2; K != NULL; K = K->Next, KK = KK->Next) {
        if (K->Type != EdgeNode || KK->Type != EdgeNode) continue;
        E = EP(K);
        SE = EP(KK);
         /* Check for same name, else inherit name */
        if (strcmp(E->Name, SE->Name))
            SE->Name = Intern(E->Name);
    }
}

EnforceArrayEdgeNameInheritance(Node, SubGraph)
NodePointer Node;
GraphPointer SubGraph;
{
    int Count;
    ListPointer K, KK;
    EdgePointer E, SE, GetNthGraphEdge();

    for (Count = 0, K = Node->InControl; K != NULL; K = K->Next, Count++) {
        if (K->Type != EdgeNode) continue;
        E = EP(K);
        SE = GetNthGraphEdge(SubGraph, Count, IN, "control");

        /* Check that both are array edges */
        if (!IsArray(E) && !IsArray(SE)) continue;

        if ((IsArray(E) && !IsArray(SE)) || (!IsArray(E) && IsArray(SE)))
            Warning("EnforceArrayEdgeNameInheritance Error");

         /* Check for same name, else inherit name */
        if (strcmp(Intern(BaseName(E)), Intern(BaseName(SE)))) {
            SE->Name = Intern(BaseName(E));
            SE->Name = EdgeName(SE);
        }
    }

    for (Count = 0, K = Node->OutControl; K != NULL; K = K->Next, Count++) {
        if (K->Type != EdgeNode) continue;
        E = EP(K);
        SE = GetNthGraphEdge(SubGraph, Count, OUT, "control");

        /* Check that both are array edges */
        if ((IsArray(E) && !IsArray(SE)) || (!IsArray(E) && IsArray(SE)))
            Warning("EnforceArrayEdgeNameInheritance Error");

         /* Check for same name, else inherit name */
        if (strcmp(Intern(BaseName(E)), Intern(BaseName(SE)))) {
            SE->Name = Intern(BaseName(E));
            SE->Name = EdgeName(SE);
        }
    }
}

InheritInOutAttr(Node, Graph)
NodePointer Node;
GraphPointer Graph;
{
    InheritIOAttr(Node->InEdges, Graph->InEdges);
    InheritIOAttr(Node->InControl, Graph->InControl);
    InheritIOAttr(Node->OutEdges, Graph->OutEdges);
    InheritIOAttr(Node->OutControl, Graph->OutControl);
}

InheritIOAttr(srclist, destlist)
ListPointer srclist, destlist;
{
    register ListPointer p1, p2;

    for (p1 = srclist,p2 = destlist; p1 != NULL && p2 != NULL; 
					p1 = p1->Next,p2 = p2->Next) {
        if (p1->Type != EdgeNode || p2->Type != EdgeNode) continue;
	InheritIOAttrAux(EP(p1), EP(p2));
    }
}

InheritIOAttrAux(SrcEdge, DestEdge)
EdgePointer SrcEdge;
EdgePointer DestEdge;
{
    DestEdge->Attributes = InheritAttribute(SrcEdge->Attributes,
                                        DestEdge->Attributes, "input");
    DestEdge->Attributes = InheritAttribute(SrcEdge->Attributes,
                                        DestEdge->Attributes, "output");
    InheritMaxIndexAttr(SrcEdge, DestEdge);
}

InheritMaxIndexAttr(SrcEdge, DestEdge)
EdgePointer SrcEdge;
EdgePointer DestEdge;
{
    char label[STRSIZE];
    int i = 0;

    sprintf(label, "maxindex%d", i);
    while (HasAttribute(SrcEdge->Attributes, label)) {
        DestEdge->Attributes = InheritAttribute(SrcEdge->Attributes,
                                                DestEdge->Attributes, label);
        sprintf(label, "maxindex%d", ++i);
    }
}

EnforceEdgeName(Graph)
GraphPointer Graph;
{
    GraphPointer graph;

    for (graph = Graph; graph != NULL; graph = graph->Next)
	EnforceEdgeNameAux(graph);
}

EnforceEdgeNameAux(Graph)
GraphPointer Graph;
{
    EdgePointer Edge;

    for (Edge = Graph->EdgeList; Edge != NULL; Edge = Edge->Next) {
	if (IsSampDelayIO(Edge) || IsLoopDelayIO(Edge)) continue;
	EnforceLegalName(Edge);
    }		
}

static GraphPointer EqualGraph;

AddEqualNodes(Graph)
GraphPointer Graph;
{
    GraphPointer graph, GraphInputWithCheck();

    EqualGraph = GraphInputWithCheck("equal", FALSE);

    for (graph = Graph; graph != NULL; graph = graph->Next) {
	AddEqualNodesToInEdge(graph->InEdges, graph);
	AddEqualNodesToInEdge(graph->InControl, graph);
	AddEqualNodesToOutEdge(graph->OutEdges, graph);
    }
}

AddEqualNodesToInEdge(Edgelist, Graph)
ListPointer Edgelist;
GraphPointer Graph;
{
    ListPointer K, KK;
    EdgePointer e, Edge, EdgeList, CreateNewEdge(), EdgeListAppend();
    NodePointer n, OutNode, CreateNewNode(), NodeAppend();
    char *TmpNodeName(), *UniqueEdgeName(), *GetType();

    for (K = Edgelist; K != NULL; K = K->Next) {
        Edge = EP(K);
        for (KK = Edge->OutNodes; KK != NULL; KK = KK->Next) {
            if (KK->Type != NodeNode) continue;
            OutNode = NP(KK);
            if (!IsDelayNode(OutNode) ||
                NodeIONumber(OutNode, Edge, IN) != 0) continue;
            n = CreateNewNode(Graph, TmpNodeName(), Intern("data"),
                        EqualGraph);
            n->Extension = NewNodeExt();
            ConnectEdgeToNode(Edge, n, NodeNode);
	    EdgeList = (IsControlEdge(Edge))?Graph->ControlList:Graph->EdgeList;
            e = CreateNewEdge (Graph, UniqueEdgeName(Edge, EdgeList,
		n->Name), Intern("data"));
            e->Arguments = SetAttribute("type", CharNode, GetType(Edge),
                                             e->Arguments);
            e->Extension = NewEdgeExt();
            EE(e)->graph = Graph;
            ConnectNodeToEdge(n, NodeNode, e);
            ReplaceInputEdge(OutNode, NodeNode, Edge, e);
        }
    }
}

/*
 * Add Equal nodes before output edges if innode is READ (so an assignment 
 * statement will be executed) or DELAY (so the delay routine won't overwrite
 * the output edge`s name.
 */
AddEqualNodesToOutEdge(Edgelist, Graph)
ListPointer Edgelist;
GraphPointer Graph;
{
    ListPointer K, KK, RemoveAttribute();
    EdgePointer e, Edge, EdgeList, CreateNewEdge(), EdgeListAppend();
    NodePointer n, InNode, OutNode, CreateNewNode(), NodeAppend();
    char *name, *IndexEdgesName(),*TmpNodeName(),*UniqueEdgeName(),*GetType();
    int test;

    for (K = Edgelist; K != NULL; K = K->Next) {
	test = FALSE;
        Edge = EP(K);
        InNode = NP(Edge->InNodes);
        if ((strcmp(InNode->Master->Name, READ) == 0) || (IsDelayNode(InNode)))
	    test = TRUE;
        for (KK = Edge->OutNodes; KK != NULL; KK = KK->Next) {
            if (KK->Type != NodeNode) continue;
	    OutNode = NP(KK);
	    if (IsDelayNode(OutNode)) 
		test = TRUE;
        }
	if (test == TRUE) {
            n = CreateNewNode(Graph, TmpNodeName(), Intern("data"),
                        EqualGraph);
            n->Extension = NewNodeExt();
            e = CreateNewEdge (Graph, Edge->Name, Intern("data"));
            e->Arguments = SetAttribute("type", CharNode, GetType(Edge),
                                             e->Arguments);
            e->Extension = NewEdgeExt();
            EE(e)->graph = Graph;
	    if (strcmp(InNode->Master->Name, READ) == 0)
                Edge->Name = IndexEdgesName(InNode);
            else {
	        EdgeList = (IsControlEdge(Edge)) ? Graph->ControlList:
							Graph->EdgeList;
		Edge->Name = UniqueEdgeName(Edge, EdgeList, n->Name);
	    }		
	    ReplaceInputEdge(Graph, GraphNode, Edge, e);
            ConnectEdgeToNode(Edge, n, NodeNode);
            ConnectNodeToEdge(n, NodeNode, e);
            if (HasAttribute(Edge->Attributes, "output")) {
                e->Attributes = SetAttribute("output", IntNode, (char *)1,
                                                e->Attributes);
                Edge->Attributes = RemoveAttribute(Edge->Attributes, "output");
            }
	    /* Note that if Graph is a DoBody Loop, we may have to propagate
	       Maxlpdelay attribute from Edge to e, so that it can be propagated
	       to the outer hierarchy in TransferDelayAttr. 7/24/91     */
        }
    }
}

EnforceBoolType(Graph)
GraphPointer Graph;
{
    GraphPointer graph;
    EdgePointer edge;
    char *Class;
    int a, b;
    bool IsBoolType();

    for (graph = Graph; graph != NULL; graph = graph->Next) {
	for (edge = graph->EdgeList; edge != NULL; edge = edge->Next) {
	    ParseType(GetType(edge), &Class, &a, &b);
	    if (a == 1 && b == 0)
	        SetType(edge, "bool");
	}
    }
}

EnforceArrayName(Graph)
GraphPointer Graph;
{
    GraphPointer graph;
    NodePointer node;
    EdgePointer edge;
    char *arrayname;
    char *IndexEdgesCodeName(), *UniqueName(), *BaseName();

    for (graph = Graph; graph != NULL; graph = graph->Next) {
	for (node = graph->NodeList; node != NULL; node = node->Next) {
	    if (strcmp(node->Master->Name, READ) == 0 ||
		strcmp(node->Master->Name, WRITE) == 0) {
		if (node->OutControl == NULL) {
		    edge = EP(node->OutEdges);
		    arrayname = (char *)GetAttribute(node->Arguments, 
								"array_name");
                    if (strcmp(arrayname, BaseName(edge)) == 0)
	                edge->Name = UniqueName("readout");
		    edge->Attributes = SetAttribute("readout", NodeNode, node,
					edge->Attributes);
		} else {
		    edge = EP(node->OutControl);	    
	            edge->Name = Intern(IndexEdgesCodeName(node));
		}
	    }
 	}
    }
}

/**************************************************************************
  IndexEdgesCodeName() -- Given a node, returns the full name of the output 
  edge expression, i.e. with brackets.  Implements Names of delay index edges
  too.
****************************************************************************/
char *IndexEdgesCodeName(n)
    NodePointer n;
{
    static char str[4*STRSIZE];
    char *name, *value, *p1, *p2, *ConstIndexName();
    char *IndexEdgeCodeName();
    ListPointer ptr, IndexEdges;
    EdgePointer edge;
    NodePointer Innode;

    IndexEdges = n->InEdges;	 /* READ node, index edges starts at 1st edge */
    if (strcmp(n->Master->Name, WRITE) == 0)	  
	IndexEdges = n->InEdges->Next;  /* WRITE node, index edges is 2 */
    if (IndexEdges == NULL)
	return(ConstIndexName(n));
    p1 = str;
    name = (char *)GetAttribute(n->Arguments, "array_name");
    for (p2 = name; *p2 != NULL;)
	*p1++ = *p2++;
    for (ptr = IndexEdges; ptr != NULL; ptr = ptr->Next) {
	edge = EP(ptr);
	name = IndexEdgeCodeName(edge);
        if (edge->InNodes != NULL && edge->InNodes->Type == NodeNode) {
            Innode = NP(edge->InNodes);
/* copy input edge names, except if from a constant node, then copy value */
	    if (!strcmp(Innode->Master->Name, CONST)) {
	        value = (char *)GetAttribute(Innode->Arguments, "value");
	        if (isnumber(value))
		    name = value;
	    }
	}
	*p1++ = '[';
	for (p2 = name; *p2 != NULL;)
	    *p1++ = *p2++;
	*p1++ = ']';
    }
    *p1 = '\0';
    return(str);
}

/**************************************************************************
    IndexEdgeCodeName() : print out the name of the index edge.  Can be a
    delayed IO edge.
****************************************************************************/
char *
IndexEdgeCodeName(edge)
EdgePointer edge;
{
    static char str[STRSIZE];
    static char value[STRSIZE];
    char *name, *p1, *p2, *DelayName();
    bool IsDelayIO();
    int delaysize;
    char *delay;

    if (IsDelayIO(edge)) {
        p1 = str;
        *p1++ = 'p'; *p1++ = 'S'; *p1++ = 'T'; *p1++ = '-'; *p1++ = '>';
        name = DelayName(edge);
        for (p2 = name; *p2 != NULL;)
	    *p1++ = *p2++;
        if (HasAttribute(edge->Attributes, "Maxdelay")) {
            *p1++='['; *p1++='p'; *p1++='S'; *p1++='T'; *p1++='-'; *p1++='>';
            for (p2 = name; *p2 != NULL;)
	        *p1++ = *p2++;
            *p1++='_'; *p1++='C'; *p1++=']';
	} else if (HasAttribute(edge->Attributes, "Maxloopdelay")) {
            *p1++='[';
            for (p2 = name; *p2 != NULL;)
	        *p1++ = *p2++;
            *p1++='_'; *p1++='C'; *p1++=']';
	} else if (HasAttribute(edge->Attributes, "delay")) {
            delay = (char *)GetAttribute(edge->Attributes, "delay");
            delaysize = (int)GetAttribute(edge->Attributes, "delaysize");
            *p1++='['; *p1++='a'; *p1++='t'; *p1++=' '; *p1++='(';
	    for (p2 = delay; *p2 != NULL;)
		*p1++ = *p2++;
	    *p1++=','; *p1++=' ';
            *p1++='p'; *p1++='S'; *p1++='T'; *p1++='-'; *p1++='>';
            for (p2 = name; *p2 != NULL;)
	        *p1++ = *p2++;
            *p1++ = '_'; *p1++ = 'C'; *p1++ = ','; *p1++ = ' ';
	    itoa(delaysize, value);
	    for (p2 = delay; *p2 != NULL;)
		*p1++ = *p2++;
            *p1++ = ')'; *p1++ = ']';
	} else if (HasAttribute(edge->Attributes, "loopdelay")) {
            delay = (char *)GetAttribute(edge->Attributes, "loopdelay");
            delaysize = (int)GetAttribute(edge->Attributes, "loopdelaysize");
            *p1++='['; *p1++='a'; *p1++='t'; *p1++=' '; *p1++='(';
	    for (p2 = delay; *p2 != NULL;)
		*p1++ = *p2++;
	    *p1++=','; *p1++=' ';
            for (p2 = name; *p2 != NULL;)
	        *p1++ = *p2++;
            *p1++ = '_'; *p1++ = 'C'; *p1++ = ','; *p1++ = ' ';
	    itoa(delaysize, value);
	    for (p2 = delay; *p2 != NULL;)
		*p1++ = *p2++;
            *p1++ = ')'; *p1++ = ']';
        }
	*p1 = '\0';
	return(str);
    } else
	return(edge->Name);
}

GenCode(l_flag,pl_flag,highlevel,bittrue,tmp_thor,sdf_flag)
bool l_flag;
bool pl_flag;
bool highlevel;
bool bittrue;
bool tmp_thor;
bool sdf_flag;
{
    /* Initialize Edgetable */
    indexlevel = 0;
    Edgetable[indexlevel] = st_init_table(strcmp, st_strhash);

    if (pl_flag == true) GenPtHeading(highlevel,bittrue,tmp_thor,sdf_flag);
    else GenHeading();
    fprintf (CFD, "/* Delay structure definition goes here...*/\n\n");
    GenDelayStruct(Root,pl_flag);  /*  Process all hierarchy  */
    if(!pl_flag) GenInputFileDecls();
    if(!pl_flag) GenDisplayFileDecls(Root);  /*  Process all hierarchy  */
    if(!pl_flag) fprintf (CFD, ";\n\n");
    if (GE(Root)->HasDelay) {
	if(bittrue) fprintf(CFD,"static ");
	GenDelayStructName(Root);
	fprintf (CFD, " SigTab;\n\n");
    }
    GenConstInputDecls();
    GenFixedLeafDecls();
    GenFixedLeafInits(pl_flag);
    GenDelayStructInits(Root,pl_flag);  /*  Process all hierarchy  */
    if(!pl_flag) GenFuncOpenFile();
    if(!pl_flag) GenFuncCreateFile ();
    if(!bittrue) GenReadInput(pl_flag);
    if(pl_flag) GenReadConstInput();

    if(pl_flag)
    {
    	GenFunctions(pl_flag,bittrue);  /*  Process all hierarchy  */
	fprintf(CFD,"} // end code \n");
	GenPtSetup(bittrue,pl_flag);
	GenPtGo(bittrue,tmp_thor);
	fprintf(CFD,"} // end defstar \n");
    }
    else
    {
    	GenMain(pl_flag);
    	GenWrapUp();
    	if (l_flag) {
        	printmsg(NULL, "REDUCING LOCAL VARIABLES...\n\n");
		ReduceLocalVars(Root);
    	}
    	printmsg(NULL, "GENERATING FUNCTION DEFINITION CODE...\n\n");
    	GenFunctions(pl_flag,bittrue);  /*  Process all hierarchy  */
    }
    st_free_table(Edgetable[indexlevel]);
}

GenHeading ()
{
   long ct;
   time (&ct);
   fprintf (CFD, "/*\n");
   fprintf (CFD, " * Author : Phu Hoang\n");
   fprintf (CFD, " * Date : %s", ctime (&ct));
   fprintf (CFD, " */\n\n");
   fprintf (CFD, "#include <stdio.h>\n");
   fprintf (CFD, "#include <math.h>\n");
   fprintf (CFD, "#include <sys/types.h>\n");
   fprintf (CFD, "#include <sys/times.h>\n\n");
   fprintf (CFD, "#ifdef HIGHLEVEL\n");
   fprintf (CFD, "#include \"highlevel.h\"\n");
   fprintf (CFD, "#endif\n");
   fprintf (CFD, "#ifdef BITTRUE\n");
   fprintf (CFD, "#include \"bittrue.h\"\n");
   fprintf (CFD, "#endif\n");
   fprintf (CFD, "#ifdef BITFAST\n");
   fprintf (CFD, "#include \"bitfast.h\"\n");
   fprintf (CFD, "#endif\n\n");
   fprintf (CFD, "#define at(d, i, m)  ((i+d)%%m)\n");
   fprintf (CFD, "#define _Min(a,b)  ((a) < (b) ? (a) : (b))\n");
   fprintf (CFD, "#define _Max(a,b)  ((a) > (b) ? (a) : (b))\n\n");
   fprintf (CFD, "int CycleCount;\n\n");
}

GenPtHeading (highlevel,bittrue,tmp_thor,sdf_flag)
bool highlevel;
bool bittrue;
bool tmp_thor;
bool sdf_flag;
{
   long ct;
   time (&ct);
   fprintf (CFD, "defstar { \n");
   fprintf (CFD, "\t name { %s } \n", Root->Name);
if(sdf_flag) 
   fprintf (CFD, "\t domain { SDF } \n");
else
   fprintf (CFD, "\t domain { Silage } \n");
   fprintf (CFD, "\t desc { Automatically Generated Silage code for Galaxy %s } \n",Root->Name);
   fprintf (CFD, "\t author { *** } \n");
   fprintf (CFD, "\t copyright { \n");
   fprintf (CFD, "Copyright (c) 1990, 1991, 1992 The Regents of the University of California. \n");
   fprintf (CFD, "All rights reserved. \n");
   fprintf (CFD, "See the file ~ptolemy/copyright for copyright notice, \n");
   fprintf (CFD, "limitation of liability, and disclaimer of warranty provisions. \n \t } \n");
   GenPtInputHeaders(bittrue,tmp_thor);
   GenPtOutputHeaders(bittrue,tmp_thor);
   fprintf (CFD, "\tccinclude { <stdio.h>, <math.h>, <sys/types.h>, <sys/times.h>, \"Fix.h\" } \n");
   fprintf (CFD, "\thinclude { \"bittrue.h\" } \n");
   if(bittrue) GenPtProtectedVars();
   fprintf (CFD, "\tcode { \n");
   fprintf (CFD, "#define at(d, i, m)  ((i+d)%%m)\n");
   fprintf (CFD, "#define _Min(a,b)  ((a) < (b) ? (a) : (b))\n");
   fprintf (CFD, "#define _Max(a,b)  ((a) > (b) ? (a) : (b))\n");
   fprintf (CFD, "\n");
}

GenPtInputHeaders(bittrue,tmp_thor)
bool bittrue;
bool tmp_thor;
{
/* need to generate the input name type etc stuff for all inputs */
   FILE *fp;
   char name[100];
   char inputName[100];
   char prec[10];
   char typ[10];
   char arrSz[10];
   strcpy(name,Root->Name);
   strcat(name,"Prec.inputs");
   fp = fopen(name,"r");
   if(fp == NULL) 
   {
   fprintf(stderr,"input information file not found.\n");
   exit(-1);
   }
   while (fscanf(fp,"%s",inputName) == 1)
   {
   fscanf(fp,"%s",typ);
if(strcmp(typ,"bool") == 0);
else fscanf(fp,"%s",prec);
if(strcmp(typ,"fixArray")==0) fscanf(fp,"%s",arrSz);
if(strcmp(typ,"constArray")==0) fscanf(fp,"%s",arrSz);
   fprintf (CFD, "\tinput { \n");
   fprintf (CFD, "\t\tname { ");
   fprintf (CFD, "%s",inputName);
   fprintf (CFD, " }\n");
   fprintf (CFD, "\t\ttype {");
if(tmp_thor)
   fprintf (CFD, " int "); /* NEED TO GET THIS TOO LATER */
else 
   fprintf (CFD, " fix "); /* NEED TO GET THIS TOO LATER */
   fprintf (CFD, "}\n");
   fprintf (CFD, "\t} \n");
   if(bittrue) GenPtState(inputName,prec);
   if(tmp_thor) GenPtState(inputName,prec);
   }
   fclose(fp);
}

GenPtState(name,prec)
char* name;
char* prec;
{
/* need to generate the state with the precision per port for use with
the .pl file like anyother fixedpoint star */ 
   fprintf (CFD, "\tdefstate { \n");
   fprintf (CFD, "\t\tname { ");
   fprintf (CFD, "%sPrecision",name);
   fprintf (CFD, " }\n");
   fprintf (CFD, "\t\ttype {");
   fprintf (CFD, " string "); 
   fprintf (CFD, "}\n");
   fprintf (CFD, "\t\tdefault {");
   fprintf (CFD, " \"%s\" ",prec); 
   fprintf (CFD, "}\n");
   fprintf (CFD, "\t\tdesc {");
   fprintf (CFD, " Precision for porthole \"%s\": left_BP.right_BP",name); 
   fprintf (CFD, "}\n");
   fprintf (CFD, "\t} \n");
}

GenPtOutputHeaders(bittrue,tmp_thor)
bool bittrue;
bool tmp_thor;
{
/* need to generate the output name type etc stuff for all outputs */
   FILE *fq;
   char name[100];
   char outputName[100];
   char prec[10];
   char typ[10];
   char arrSz[10];
   strcat(outputName,"");
   strcpy(name,Root->Name);
   strcat(name,"Prec.outputs");
   fq = fopen(name,"r");

   if(fq == NULL) 
   {
   fprintf(stderr,"output information file not found.\n");
   exit(-1);
   }
   while( (fscanf(fq,"%s",outputName) == 1 ) )
   {
   fscanf(fq,"%s",typ);
if( strcmp(typ,"bool") == 0);
else fscanf(fq,"%s",prec);
if( strcmp(typ,"fixArray") == 0) fscanf(fq,"%s",arrSz);
if( strcmp(typ,"constArray") == 0) fscanf(fq,"%s",arrSz);
   fprintf (CFD, "\toutput { \n");
   fprintf (CFD, "\t\tname { ");
   fprintf (CFD, "%s",outputName);
   fprintf (CFD, " }\n");
   fprintf (CFD, "\t\ttype {");
if(tmp_thor)
   fprintf (CFD, " int "); /* NEED TO GET THIS TOO LATER */
else 
   fprintf (CFD, " fix "); /* NEED TO GET THIS TOO LATER */
   fprintf (CFD, "}\n");
   fprintf (CFD, "\t} \n");
   if(tmp_thor) GenPtState(outputName,prec);
   if(bittrue) GenPtState(outputName,prec);
   }
   fclose(fq);
}

GenPtProtectedVars()
{
/* to generate the protected variables needed for using Fixed SDF fns */
   FILE *fq;
   char fileName[100];
   char portName[100];
   char prec[10], typ[10], arrSz[10];

   strcpy(portName,"");
   strcpy(fileName,Root->Name);
   strcat(fileName,"Prec.outputs");
   fq = fopen(fileName,"r");
/* outputs */
   if(fq == NULL) 
   {
   	fprintf(stderr,"output information file not found.\n");
   	exit(-1);
   }

   fprintf (CFD, "\tprotected { \n");

   while( (fscanf(fq,"%s",portName) == 1 ) )
   {
   	fscanf(fq,"%s",typ);
   	if( strcmp(typ,"bool") == 0);
   	else fscanf(fq,"%s",prec);
	if( strcmp(typ,"fixArray") == 0) fscanf(fq,"%s",arrSz);
	if( strcmp(typ,"constArray") == 0) fscanf(fq,"%s",arrSz);
if( (strcmp(typ,"constArray") == 0) || (strcmp(typ,"bool") == 0));
else
{
  /* fprintf (CFD, "\t// Fixed Point information for port %s \n",portName); */
   fprintf (CFD, "\t\tconst char* %s_P; \n",portName); 	/* precision read in */
   fprintf (CFD, "\t\tint %s_IntBits; \n",portName); 	/* int part */
   fprintf (CFD, "\t\tint %s_Len; \n",portName); 	/* word length */
   fprintf (CFD, "\t\tint %s_FracBits; \n",portName); 	/* fractional length */
}
   }
   fclose(fq);
/* inputs */

   strcpy(portName,"");
   strcpy(fileName,Root->Name);
   strcat(fileName,"Prec.inputs");
   fq = fopen(fileName,"r");

   if(fq == NULL) 
   {
   	fprintf(stderr,"input information file not found.\n");
   	exit(-1);
   }

   while( (fscanf(fq,"%s",portName) == 1 ) )
   {
   	fscanf(fq,"%s",typ);
   	if( strcmp(typ,"bool") == 0);
   	else fscanf(fq,"%s",prec);
	if( strcmp(typ,"fixArray") == 0) fscanf(fq,"%s",arrSz);
	if( strcmp(typ,"constArray") == 0) fscanf(fq,"%s",arrSz);
if( (strcmp(typ,"constArray") == 0) || (strcmp(typ,"bool") == 0));
else
{
  /* fprintf (CFD, "\t// Fixed Point information for port %s \n",portName); */
   fprintf (CFD, "\t\tconst char* %s_P; \n",portName); 	/* precision read in */
   fprintf (CFD, "\t\tint %s_IntBits; \n",portName); 	/* int part */
   fprintf (CFD, "\t\tint %s_Len; \n",portName); 	/* word length */
   fprintf (CFD, "\t\tint %s_FracBits; \n",portName); 	/* fractional length */
}
   }
   fclose(fq);
   fprintf (CFD, "\t} \n");
}

GenInputFileDecls()
{
    fprintf(CFD,"/* Declaring Input and Output File Descriptors */\n");
    fprintf (CFD, "FILE");
    GenInputFileDeclsOfEdge(ListOfInputs);
}

GenInputFileDeclsOfEdge(List)
ListPointer List;
{
    register ListPointer ptr;
    register EdgePointer edge;

    for (ptr = List; ptr != NULL; ptr = ptr->Next) {
        edge = EP(ptr);
        fprintf(CFD, " *fd_");
        GenEdgeName(edge);
        if (ptr->Next != NULL)
            fprintf(CFD, ",");
    }
}

GenDisplayFileDecls(Graph)
GraphPointer Graph;
{
    if (Graph == NULL)
        return;
    fprintf(CFD, ", *dfd_dump");
}

GenConstInputDecls()
{
    fprintf(CFD, "/* Declaring Inputs that are constant*/\n");
    GenConstInputDeclOfEdge(ListOfInputs);
    GenConstArrayInputDeclOfEdge(ListOfInputs);
    fprintf(CFD, "\n");
}

GenConstInputDeclOfEdge(List)
ListPointer List;
{
    register ListPointer ptr;
    register EdgePointer edge;

    for(ptr = List; ptr != NULL; ptr = ptr->Next) {
        edge = EP(ptr);
        if (!HasAttribute(edge->Attributes, "IsConstant")) continue;
        if(IsArray(edge)) continue;
	if (IsDeclaredEdge(edge)) continue;
	if (IsFixedType(edge))
            fprintf(CFD, "Sig_Type ");
	else
            fprintf(CFD, "int ");
	GenEdgeDecl(edge);
	st_insert(Edgetable[indexlevel], Intern(edge->Name), 0);
        fprintf(CFD, ";\n");
    }
}

GenConstArrayInputDeclOfEdge(List)
ListPointer List;
{
    register ListPointer ptr;
    register EdgePointer edge;
    ArrayDefinitionList adl, adlNext, AddToControlListDecl(), conlist = NULL;

    for(ptr = List; ptr != NULL; ptr = ptr->Next) {
        edge = EP(ptr);
        if (!HasAttribute(edge->Attributes, "IsConstant")) continue;
        if(!IsArray(edge)) continue;
	if (IsDeclaredEdge(edge)) continue;
	conlist = AddToControlListDecl(edge, conlist);
    }
    for (adl = conlist; adl != NULL; adl = adlNext) {
	    adlNext = adl->Next;
	    if (IsFixedType(edge))
                fprintf(CFD, "Sig_Type ");
	    else
                fprintf(CFD, "int ");
            GenConEdgeDecl(adl);
            fprintf(CFD, ";\n");
	    st_insert(Edgetable[indexlevel], DeclName(adl->edge), 0);
            free(adl);
    }
}

GenFixedLeafDecls()
{
    register ListPointer ptr;
    register EdgePointer edge;
  
    if (ListOfFixedLeafs == NULL) return;
    fprintf(CFD,"/* Declaring FixedPoint Constants as globals */\n");
    fprintf(CFD,"static Sig_Type ");
/* change for ptolemy : static added */
    for(ptr = ListOfFixedLeafs; ptr != NULL; ptr = ptr->Next) {
        edge = EP(ptr);
        GenEdgeName(edge);
        st_insert(Edgetable[indexlevel], DeclName(edge), 0);
        if (ptr->Next != NULL)
	    fprintf(CFD,", ");
    }
    fprintf(CFD,";\n\n");	
}

GenFixedLeafInits(pl_flag)
bool pl_flag;
{
    register ListPointer ptr;
    register EdgePointer edge;
    register char *value;

    fprintf(CFD,"/* Initialize the FixedPoint globals */\n");
    if(pl_flag == true) fprintf(CFD,"void \t ");
    fprintf(CFD,"InitFixedLeafs ()\n{\n");

    for(ptr = ListOfFixedLeafs; ptr != NULL; ptr = ptr->Next) {
        edge = EP(ptr);
        value = (char *)GetAttribute(edge->Attributes, "value");
        fprintf(CFD, "    Float2Fix (%s, ", value);
        GenEdgeName(edge);
        GenFixedType(edge);
	    fprintf(CFD,");\n");
    }
    fprintf(CFD,"}\n\n");
}

GenNodeName(node)
NodePointer node;
{
    fprintf(CFD, "c_%s", node->Name);
}

GenEdgeName(edge)
EdgePointer edge;
{
    char *value, *arrayname;
    NodePointer Node;

    if (IsArray(edge)) {
        fprintf(CFD, "%s", BaseName(edge));
    }
    else if (HasAttribute(edge->Attributes, "readout")) {
	Node = (NodePointer)GetAttribute(edge->Attributes, "readout");
	arrayname = (char *)GetAttribute(Node->Arguments, "array_name");
	fprintf(CFD, "%s", arrayname);
    }
    else if (IsDelayOut(edge)) {
	    fprintf(CFD, "%s", DelayName(edge));
    }
    else if (IsIntType(edge) && HasAttribute(edge->Attributes, "value")) {
	    value =  (char *)GetAttribute(edge->Attributes, "value");
	    if (isnumber(value))
	        fprintf(CFD, "%s", value);
	    else
            fprintf(CFD, "%s", edge->Name);
    }
    else
        fprintf(CFD, "%s", edge->Name);
}

GenFixedType(edge)
EdgePointer edge;
{
    register char *type;
    int w, d;

    type = (char *)GetAttribute(edge->Arguments, "type");
    sscanf(type, "fix<%d,%d>", &w, &d);
    fprintf(CFD, ", %d, %d", w, d);
}

GenFuncOpenFile ()
{
    fprintf (CFD, "FILE *OpenFile (FileName)\n");
    fprintf (CFD, "   char *FileName;\n");
    fprintf (CFD, "{\n");
    fprintf (CFD, "   FILE *FD;\n");
    fprintf (CFD, "   FD = fopen (FileName, \"r\");\n");
    fprintf (CFD, "   if (FD == NULL) {\n");
    fprintf (CFD, "      printmsg(NULL, ");
    fprintf (CFD, "\"Can't open file : %%s\\n\", FileName);\n");
    fprintf (CFD, "      exit (-1);\n");
    fprintf (CFD, "   }\n");
    fprintf (CFD, "   return (FD);\n");
    fprintf (CFD, "}\n\n");
}

GenFuncCreateFile ()
{
    fprintf (CFD, "FILE *CreateFile (FileName) \n");
    fprintf (CFD, "   char *FileName;\n");
    fprintf (CFD, "{\n");
    fprintf (CFD, "   FILE *FD;\n");
    fprintf (CFD, "   FD = fopen (FileName, \"r\");\n");
    fprintf (CFD, "   if (FD != NULL) {\n");
    fprintf (CFD, "      printmsg(NULL, ");
    fprintf (CFD, "\"File exists, not removed : %%s\\n\", FileName);\n");
    fprintf (CFD, "      exit (-1);\n");
    fprintf (CFD, "   }\n");
    fprintf (CFD, "   FD = fopen (FileName, \"w\");\n");
    fprintf (CFD, "   if (FD == NULL) {\n");
    fprintf (CFD, "      printmsg(NULL, ");
    fprintf (CFD, "\"Can't open file : %%s\\n\", FileName);\n");
    fprintf (CFD, "      exit (-1);\n");
    fprintf (CFD, "   }\n");
    fprintf (CFD, "   return (FD);\n");
    fprintf (CFD, "}\n\n");
}

GenReadInput(pl_flag)
bool pl_flag;
{
    register ListPointer ptr;

    fprintf(CFD, "/* Routines to read in inputs */\n");
    for (ptr = ListOfInputs; ptr != NULL; ptr = ptr->Next) {
	if (IsArray(EP(ptr))) 
	{
	    if(pl_flag) GenReadInputEdge(EP(ptr),pl_flag);
	    GenReadInputControl(EP(ptr),pl_flag);
	}
	else
	    GenReadInputEdge(EP(ptr),pl_flag);
    }
}

/* to read in constant inputs from files in ptolemy, ONLY ARRAYS  */
GenReadConstInput()
{
    register ListPointer ptr;
    register EdgePointer edge;
    register char *file;

    	fprintf(CFD, "/* Routines to read in constant inputs */\n");

	for (ptr = ListOfInputs; ptr != NULL; ptr = ptr->Next) 
	{
        edge = EP(ptr);
        if ((int)GetAttribute(edge->Attributes, "IsConstant")) 
	{
	if (IsArray(edge) ) 
	{
	    fprintf(CFD,"void ReadConstArray_");
            GenEdgeName(edge);
	    fprintf(CFD,"(char* FileName)\n{\n");
	    fprintf(CFD,"\tFILE *fp;\n");
	    fprintf(CFD,"\tfloat v;\n");
	    fprintf(CFD,"\tint i=0;\n\n");
	    fprintf(CFD,"\tfp = fopen(FileName,\"r\");\n");
	    fprintf(CFD,"\tif(fp == NULL) fprintf(stderr,");
	    fprintf(CFD,"\"Cant open file %%s\\n\",FileName);\n\n");

	    fprintf(CFD,"\twhile(fscanf(fp,\"%%f\",&v) == 1)\n\t{\n");
	    fprintf(CFD,"\t\tFloat2Fix(v,");
            GenEdgeName(edge);
	    fprintf(CFD,"[i]");
	    GenFixedType(edge);
	    fprintf(CFD,");\n\t\ti++;\n\t}\n\n");

	    fprintf(CFD,"\tfclose(fp);\n");
	    fprintf(CFD,"}\n\n");
	}
	else
	fprintf(stderr,"can read only arrays of constant inputs so far \n");
	}
    	} /* for */
}

GenReadInputControl(edge,pl_flag)
EdgePointer edge;
bool pl_flag;
{
/* this is to generate the read for array inputs */
    int i = 0;
    char label[STRSIZE], value[STRSIZE];
    char *lowstr, *highstr, *strtok();
    int low, high;

    if(pl_flag) fprintf(CFD,"void \t");
    fprintf(CFD, "ReadArray_");
    GenEdgeName(edge);
    if(pl_flag) fprintf(CFD,"(");
    else fprintf(CFD," (pIn)\n");
    if(pl_flag) fprintf(CFD, "Sig_Type pIn");
    else fprintf(CFD, "Sig_Type pIn");
/* generate dimension of signals */
    GenEdgeMaxDim(edge);
    if(pl_flag)	
    {
    fprintf(CFD, ",float d");
    GenEdgeMaxDim(edge);
    fprintf(CFD,")\n{\n");
    }
    if(!pl_flag) fprintf(CFD, ";\n{\n");

/* generate local parameter declarations */
    fprintf(CFD, "    int ");
    i = 0;
    sprintf(label, "index%d", i);
    while(1) {
        fprintf(CFD, "i_%d", i++);
        sprintf(label, "index%d", i);
	    if (HasAttribute(edge->Arguments, label))
	        fprintf(CFD, ", ");
	    else
	        break;
    }
    fprintf(CFD, ";\n");

/* generate loops to read input */
    i = 0;
    sprintf(label, "index%d", i);
    while(HasAttribute(edge->Arguments, label)) {
        strcpy(value, (char *)GetAttribute(edge->Arguments, label));
        if (isnumber(value))
	        high = 1;
	    else {
	        lowstr = strtok(value, "..");
	        highstr = strtok(NULL, "..");
	        if (isnumber(highstr))
	            high = atoi(highstr) + 1;
	        else 
		        Error("GenReadInputControl() : Unknown dimension");
	    }
        fprintf(CFD, "    for (i_%d=0; i_%d<%d; i_%d++)\n",i,i,high,i);
        i++;
        sprintf(label, "index%d", i);
    }

/* generate function call to read input */
    if(!pl_flag)
    {
    fprintf(CFD, "\tif (Read_");
    GenEdgeName(edge);
    fprintf(CFD, " (&pIn");
    i = 0;
    sprintf(label, "index%d", i);
    while(HasAttribute(edge->Arguments, label)) {
        fprintf(CFD, "[i_%d]", i++);
        sprintf(label, "index%d", i);
    }
    fprintf(CFD, ") < 0)\n");
    fprintf(CFD, "\t    WrapUp();\n");
    fprintf(CFD, "    return(0);\n}\n\n");
    }
    else if(pl_flag)
    {
    fprintf(CFD, "\tRead_");
    GenEdgeName(edge);
    fprintf(CFD, " (&pIn");
    i = 0;
    sprintf(label, "index%d", i);
    while(HasAttribute(edge->Arguments, label)) {
        fprintf(CFD, "[i_%d]", i++);
        sprintf(label, "index%d", i);
    }
    /* actual input value instead of a scanf */
    fprintf(CFD,",&d");
    i = 0;
    sprintf(label, "index%d", i);
    while(HasAttribute(edge->Arguments, label)) {
        fprintf(CFD, "[i_%d]", i++);
        sprintf(label, "index%d", i);
    }
    fprintf(CFD, "); \n}\n\n");
    } /* else */

/* generate the routine called above */
    if(!pl_flag) GenReadInputEdge(edge,pl_flag);
}

GenReadInputEdge(edge,pl_flag)
EdgePointer edge;
bool pl_flag;
{
    char *Infile;

    if(pl_flag) fprintf(CFD,"void \t");
    fprintf(CFD,"Read_");
    GenEdgeName(edge);
    if(pl_flag) fprintf(CFD,"(");
    else fprintf(CFD," (pIn)\n");
    if (IsFixedType(edge))
    {
	    if(pl_flag) fprintf(CFD, "Sig_Type* pIn,");
	    else fprintf(CFD, "Sig_Type *pIn;\n");
    }
    else 
    {
    	if(pl_flag) fprintf(CFD, "int* pIn,");
    	else fprintf(CFD, "int *pIn;\n");
    }
    if(pl_flag) 
    {
    if(IsArray(edge)) fprintf(CFD, "float* d");
    else fprintf(CFD, "float d");
    fprintf(CFD,")");
    }

    fprintf(CFD, "{\n");

    if(!pl_flag)
    {
    fprintf(CFD, "    float d;\n");
    fprintf(CFD, "    if (fscanf (fd_");
    GenEdgeName(edge);
    fprintf(CFD, ", \"%%f \", &d) != 1) {\n");
    fprintf(CFD, "\tprintmsg(NULL, \"Reach end of ");
    Infile = (char *)GetAttribute(edge->Attributes, "InFile");
    fprintf(CFD, "file : %s\\n\");\n", Infile);
    fprintf(CFD, "\tWrapUp();\n");
    fprintf(CFD, "    } else\n");
    }
    if (IsFixedType(edge)) {
	    if(IsArray(edge)) fprintf(CFD, "\tFloat2Fix (d[0], pIn[0]");
	    else
	    fprintf(CFD, "\tFloat2Fix (d, pIn[0]");
	    GenFixedType(edge);
	    fprintf(CFD, ");\n");
    }
    else
	    fprintf(CFD, "\tpIn[0] = d;\n");
    if(!pl_flag) fprintf(CFD, "    return(0);\n");
    fprintf(CFD, "}\n\n");
}

GenMain(pl_flag)
bool pl_flag;
{
    extern int NrOfCycles;

    fprintf(CFD, "main (argc, argv)\n");
    fprintf(CFD, "int argc;\nchar *argv[];\n{\n");
    fprintf(CFD, "    Sig_Type fdum");
    fprintf(CFD, ";\n");
    fprintf(CFD, "    int idum");
    fprintf(CFD, ";\n");
    fprintf(CFD, "    int MaxCycles;\n");
    fprintf(CFD, "    if ((argc == 1) ||\n");
    fprintf(CFD, "\t(sscanf (argv[1], \"%%d\",");
    fprintf(CFD, " &MaxCycles) != 1) ||\n");
    fprintf(CFD, "\t(MaxCycles < 0))\n");
    fprintf(CFD, "\tMaxCycles = %d;\n", NrOfCycles);
    fprintf(CFD, "    InitFixedLeafs ();\n");
	if (GE(Root)->HasDelay == true) {
        fprintf(CFD, "\n/*  Delay Initialization goes here... */\n");
        fprintf(CFD, "    Init_%s (&SigTab);\n", Root->Name);
    }
    fprintf(CFD, "\n/* Creating Display files */\n");
    GenOpenDisplayFiles(Root);  /*  Process all hierarchy  */
    GenOpenInputFiles();
    GenMainBody(pl_flag);
    fprintf(CFD, "\n    WrapUp();\n");
    fprintf(CFD, "}\n\n");
}

GenPtSetup(bittrue,pl_flag)
bool bittrue;
bool pl_flag;
{
   FILE *fq; 	char in_fileName[100]; 		char portName[100];
   FILE *fp; 	char out_fileName[100]; 
   char typ[10], prec[10];
   int arrSz;

   strcpy(portName,"");
   strcpy(in_fileName,Root->Name); 	strcpy(out_fileName,Root->Name);
   strcat(in_fileName,"Prec.inputs");	strcat(out_fileName,"Prec.outputs");
   fq = fopen(in_fileName,"r");		 fp = fopen(out_fileName,"r");

   if(fq == NULL) { fprintf(stderr,"input file not found.\n"); exit(-1); }
   if(fp == NULL) { fprintf(stderr,"output file not found.\n"); exit(-1); }


    fprintf(CFD, "\tsetup { \n");
    fprintf(CFD, "\t\tInitFixedLeafs ();");
    fprintf(CFD, "/*  Initializing Fixed quantities */\n");
/* read constant inputs - into array */
    GenConstInputs(pl_flag); 

	if (GE(Root)->HasDelay == true) 
        {
        fprintf(CFD, "\n\t\t/*  Delay Initialization goes here... */\n");
        fprintf(CFD, "\t\tInit_%s (&SigTab);\n", Root->Name);
        }

	fprintf(CFD, "\n\t\t/* Fixed Point Precision Initialization */\n");
	while( (fscanf(fq,"%s",portName) == 1 ) )
	{
   	fscanf(fq,"%s",typ);
	if(strcmp(typ,"bool") == 0);
	else fscanf(fq,"%s",prec);
	if( (strcmp(typ,"fixArray")==0) || (strcmp(typ,"constArray")==0) )
		 fscanf(fq,"%d",&arrSz);
	else arrSz =1;
if(bittrue)
{
if( (strcmp(typ,"constArray")==0) || (strcmp(typ,"bool")==0) );
else
{
   	fprintf (CFD, "\t// Fixed Point variables for port %s \n",portName);
	fprintf(CFD,"\t\t%s_P = %sPrecision;\n",portName,portName);
    	fprintf(CFD,"\t\t%s_IntBits = Fix::get_intBits (%s_P);\n",portName,portName);
    	fprintf(CFD,"\t\t%s_Len = Fix::get_length (%s_P);\n",portName,portName);
    	fprintf(CFD,"\t\t%s_FracBits = %s_Len - %s_IntBits;\n",portName,portName,portName);
}
}
	if( (strcmp(typ,"fixArray")==0) && arrSz >1)
	{
	fprintf(CFD,"\t\t// set parameters to store past samples\n");
	fprintf(CFD,"\t\t%s.setSDFParams(%d,%d);\n\n",portName,arrSz,arrSz);
	}

	}
	while( (fscanf(fp,"%s",portName) == 1 ) )
	{
   	fscanf(fp,"%s",typ);
	if(strcmp(typ,"bool") == 0);
	else fscanf(fp,"%s",prec);
	if( (strcmp(typ,"fixArray")==0) || (strcmp(typ,"constArray")==0) )
			 fscanf(fp,"%d",&arrSz);
	else arrSz =1;
if(bittrue)
{
if( (strcmp(typ,"constArray")==0) || (strcmp(typ,"bool")==0) );
else
{
   	fprintf (CFD, "\t// Fixed Point variables for port %s \n",portName);
	fprintf(CFD,"\t\t%s_P = %sPrecision;\n",portName,portName);
    	fprintf(CFD,"\t\t%s_IntBits = Fix::get_intBits (%s_P);\n",portName,portName);
    	fprintf(CFD,"\t\t%s_Len = Fix::get_length (%s_P);\n",portName,portName);
    	fprintf(CFD,"\t\t%s_FracBits = %s_Len - %s_IntBits;\n",portName,portName,portName);
}
}
	if( (strcmp(typ,"fixArray")==0) && (arrSz >1))
	{
	fprintf(CFD,"\t\t// set parameters to store past samples\n");
	fprintf(CFD,"\t\t%s.setSDFParams(%d,%d);\n\n",portName,arrSz,arrSz);
	}

	}
    fprintf(CFD, "\n \t } // setup \n\n");
   fclose(fq);
   fclose(fp);
}

GenPtGo(bittrue,tmp_thor)
bool bittrue;
bool tmp_thor;
{
   bool comma = false;
   FILE *fp;
   FILE *fq;
   char fInName[100], fOutName[100], inputName[100], outputName[100];
   char inputTyp[10], inputPrec[10];
   int inArrSz;
   char outputTyp[10], outputPrec[10];
   int outArrSz;
   struct listOfIO {
       	char name[100];
       	char typ[10];
       	char prec[10];
       	int arrSz;
   };
   struct listOfIO inputList[10];	/* max num of inputs */ 
   struct listOfIO outputList[10];	/* max num of outputs */ 
   int numIn, numOut, numberIn, numberOut, k, cnt;

   fprintf(CFD, "\tgo { \n");

   numIn = 0;
   numOut = 0;

   strcpy(fInName,Root->Name); 		strcat(fInName,"Prec.inputs");
   strcpy(fOutName,Root->Name); 	strcat(fOutName,"Prec.outputs");
   fp = fopen(fInName,"r");		fq = fopen(fOutName,"r");
   if (fp == NULL) {fprintf(stderr,"error opening input file\n"); exit(-1); }
   if (fq == NULL) {fprintf(stderr,"error opening output file\n"); exit(-1); }

   while( fscanf(fp,"%s",inputName) == 1)
   {
   fscanf(fp,"%s",inputTyp);
   if(strcmp(inputTyp,"bool") == 0) strcpy(inputPrec,"2.0");
   else fscanf(fp,"%s",inputPrec);
   if( (strcmp(inputTyp,"fixArray")==0) || (strcmp(inputTyp,"constArray")==0) )
			fscanf(fp,"%d",&inArrSz);
   else inArrSz=1;

if( strcmp(inputTyp,"constArray")==0);
else
{
   strcpy(inputList[numIn].name,inputName);
   strcpy(inputList[numIn].typ,inputTyp);
   strcpy(inputList[numIn].prec,inputPrec);
   inputList[numIn].arrSz=inArrSz;
   numIn++;
}
   } /* while */
   numberIn = numIn; 

for(cnt=0;cnt<numberIn;cnt++)
{
   	if(tmp_thor)
   	{
		fprintf(CFD,"\t\tfloat %s_%s = float(%s%%0);\n",Root->Name,
				inputList[cnt].name,inputList[cnt].name);
   	} /* tmp_thor */

   	if(!bittrue && !tmp_thor)
   	/*if(!bittrue)*/
   	{
   	fprintf(CFD,"\t\tEnvelope env_%s;\n",inputList[cnt].name);
   	fprintf(CFD,"\t\tconst FloatingPt* f_%s;\n",inputList[cnt].name);
	if(strcmp(inputList[cnt].typ,"bool")!=0)
	{
  	fprintf(CFD,"\t\tfloat %s_%s", Root->Name,inputList[cnt].name);
	if(inputList[cnt].arrSz >1) fprintf(CFD,"[%d]",inputList[cnt].arrSz);
	fprintf(CFD,";\n");
	}
	else
  	fprintf(CFD,"\t\tint %s_%s;\n", Root->Name,inputList[cnt].name);

	fprintf(CFD,"\n\t\tfor(int %s_i=0; %s_i< %d; %s_i++)\n",
                inputList[cnt].name,inputList[cnt].name,inputList[cnt].arrSz,
                inputList[cnt].name);
        fprintf(CFD,"\t\t{\n");


   	fprintf(CFD,"\t\t(%s%%%s_i).getMessage(env_%s);\n",inputList[cnt].name, 
			inputList[cnt].name, inputList[cnt].name);

   	fprintf(CFD,"\t\tf_%s = (const FloatingPt*) env_%s.myData();\n",
		inputList[cnt].name, inputList[cnt].name);
  	if(strcmp(inputList[cnt].typ,"bool")==0)
  	{
		fprintf(CFD,"\t\t%s_%s = int(f_%s->data);\n",
			Root->Name,inputList[cnt].name,
			inputList[cnt].name);
  	}
  	else
   	{
  		fprintf(CFD,"\t\t%s_%s", Root->Name,inputList[cnt].name,
			inputList[cnt].name);
	if(inputList[cnt].arrSz >1) 
	fprintf(CFD,"[%d-%s_i]",(inputList[cnt].arrSz-1),inputList[cnt].name);
 		fprintf(CFD," = f_%s->data;\n", inputList[cnt].name);
   	}
   	}  /* not bittrue */

   	else if(bittrue)
   	{
   	fprintf(CFD,"\t\tfloat %s_%s;\n",Root->Name,inputList[cnt].name); 
   	if(strcmp(inputList[cnt].typ,"bool")!=0)
	{
	fprintf(CFD,"\t\tSig_Type F_%s_%s",Root->Name,inputList[cnt].name);
	if(inputList[cnt].arrSz >1) fprintf(CFD,"[%d]",inputList[cnt].arrSz);
	fprintf(CFD,";\n");
	}
	else
	{
		fprintf(CFD,"\t\tint F_%s_%s;\n",Root->Name,
					inputList[cnt].name);
	}

	/* if(inputList[cnt].arrSz > 1) */
	fprintf(CFD,"\n\t\tfor(int %s_i=0; %s_i< %d; %s_i++)\n",
		inputList[cnt].name,inputList[cnt].name,inputList[cnt].arrSz,
		inputList[cnt].name);
	fprintf(CFD,"\t\t{\n");

   	fprintf(CFD,"\t\t%s_%s = double(%s%%%s_i);\n",Root->Name,
		inputList[cnt].name,inputList[cnt].name,inputList[cnt].name); 
	
  	if(strcmp(inputList[cnt].typ,"bool")==0)
  	{
		fprintf(CFD,"\t\tF_%s_%s = int(%s_%s);\n",Root->Name,
		inputList[cnt].name,Root->Name,inputList[cnt].name);
  	}
  	else
   	{
   	fprintf(CFD,"\t\tFloat2Fix(%s_%s,F_%s_%s", Root->Name,
		inputList[cnt].name,Root->Name,inputList[cnt].name);
	if(inputList[cnt].arrSz >1) 
	fprintf(CFD,"[%d-%s_i]",(inputList[cnt].arrSz-1),inputList[cnt].name);
	fprintf(CFD,",%s_Len,%s_FracBits);\n", inputList[cnt].name,
				inputList[cnt].name); 
   	}
	} /*bittrue */
	fprintf(CFD,"\t\t} // input %s \n\n",inputList[cnt].name);
} /* for */

   while( fscanf(fq,"%s",outputName) == 1)
   {
   fscanf(fq,"%s",outputTyp);
   if(strcmp(outputTyp,"bool") == 0) strcpy(outputPrec,"2.0");
   else fscanf(fq,"%s",outputPrec);
   if((strcmp(outputTyp,"fixArray")==0) || (strcmp(outputTyp,"constArray")==0) )
			 fscanf(fq,"%d",&outArrSz);
   else outArrSz=1;
   
if(strcmp(outputTyp,"constArray")==0);
else
{
   strcpy(outputList[numOut].name,outputName);
   strcpy(outputList[numOut].typ,outputTyp);
   strcpy(outputList[numOut].prec,outputPrec);
   outputList[numOut].arrSz = outArrSz;
   numOut++;
}
   } /* while */
   numberOut = numOut; 

for(cnt=0;cnt<numberOut;cnt++)
{
   if(!bittrue || tmp_thor)
   {
   if(strcmp(outputList[cnt].typ,"bool")==0) 
   	fprintf(CFD, "\t\tint %s_%s; \n", Root->Name,outputList[cnt].name);
   else
	{
   	fprintf(CFD, "\t\tfloat %s_%s", Root->Name,outputList[cnt].name);
	if(outputList[cnt].arrSz >1) fprintf(CFD,"[%d]",outputList[cnt].arrSz);
  	fprintf(CFD,";\n");
	}
   }
   else if(bittrue)
   {
   if(strcmp(outputList[cnt].typ,"bool")==0) 
   	fprintf(CFD, "\t\tint F_%s_%s; \n", Root->Name,outputList[cnt].name);
   else
	{
  	fprintf(CFD,"\t\tSig_Type F_%s_%s",Root->Name,outputList[cnt].name);
	if(outputList[cnt].arrSz >1) fprintf(CFD,"[%d]",outputList[cnt].arrSz);
  	fprintf(CFD,";\n");
	}
   }
} /* for */

   fprintf(CFD, "\n\t\tSim_%s (", Root->Name);
	if (GE(Root)->HasDelay) {
	fprintf(CFD, "&SigTab");
	if(numberIn>0) fprintf(CFD,",");
	if(numberIn==0 && numberOut>0) fprintf(CFD,",");
        }
   if(!bittrue || tmp_thor)
   {
   	for( k=0; k< (numberIn-1) ; k++)
   	{ 
	if(inputList[k].arrSz ==1) fprintf(CFD,"&");
	fprintf(CFD, "%s_%s, ",Root->Name,inputList[k].name); 
	}
	if(inputList[numberIn-1].arrSz ==1) fprintf(CFD,"&");
   	fprintf(CFD, "%s_%s",Root->Name,inputList[numberIn-1].name); 

   	if(numberOut>0) fprintf(CFD,", ");

   	for( k=0; k< (numberOut-1) ; k++)
   	{ 
	if(outputList[k].arrSz ==1) fprintf(CFD,"&");
	fprintf(CFD, "%s_%s, ",Root->Name,outputList[k].name); 
	}
	if(outputList[numberOut-1].arrSz ==1) fprintf(CFD,"&");
   	fprintf(CFD, "%s_%s ",Root->Name,outputList[numberOut-1].name); 
   }
   else if (bittrue)
   {
   	for( k=0; k< (numberIn-1) ; k++)
   	{ 
	if(inputList[k].arrSz ==1) fprintf(CFD,"&");
	fprintf(CFD, "F_%s_%s, ",Root->Name,inputList[k].name); 
	}
	if(inputList[numberIn-1].arrSz ==1) fprintf(CFD,"&");
   	fprintf(CFD, "F_%s_%s",Root->Name,inputList[numberIn-1].name); 

   	if(numberOut>0) fprintf(CFD,", ");

   	for( k=0; k< (numberOut-1) ; k++)
   	{ 
	if(outputList[k].arrSz ==1) fprintf(CFD,"&");
	fprintf(CFD, "F_%s_%s, ",Root->Name,outputList[k].name); 
	}
	if(outputList[numberOut-1].arrSz ==1) fprintf(CFD,"&");
   	fprintf(CFD, "F_%s_%s ",Root->Name,outputList[numberOut-1].name); 
   }

    fprintf(CFD, ");\n\n");

   	for( k=0; k<numberOut; k++)
   	{
		if(tmp_thor)
		{
		fprintf(CFD,"\t\t%s%%0 << int(%s_%s); \n",outputList[k].name,
			Root->Name,outputList[k].name); 
		} /* tmp_thor*/

   		if(!bittrue && !tmp_thor)
		{
			if(strcmp(outputList[k].typ,"bool")==0)
			{
	fprintf(CFD,"\t\tFloatingPt* t_%s = new FloatingPt (float(%s_%s))",
			outputList[k].name, Root->Name,outputList[k].name);
			if(outputList[k].arrSz > 1) fprintf(CFD,"[%s_i]",
				outputList[k].name);
			fprintf(CFD,";\n");
			fprintf(CFD,"\t\tEnvelope env_%s(*t_%s);\n", 
				outputList[k].name,outputList[k].name);
			fprintf(CFD,"\t\t%s%%0 << env_%s;\n",
				outputList[k].name,outputList[k].name);
			}
			else
			{
			fprintf(CFD,"\t\tfor(int %s_i=0; %s_i < %d; %s_i++)\n",
                                outputList[k].name,outputList[k].name,
                                outputList[k].arrSz,outputList[k].name);
                        fprintf(CFD,"\t\t{\n");

		fprintf(CFD,"\t\tFloatingPt* t_%s = new FloatingPt (%s_%s",
			outputList[k].name,Root->Name,outputList[k].name);
		if(outputList[k].arrSz > 1) fprintf(CFD,"[%s_i]",
				outputList[k].name);
			fprintf(CFD,");\n");
		fprintf(CFD,"\t\tEnvelope env_%s(*t_%s);\n", outputList[k].name,
				outputList[k].name);
		fprintf(CFD,"\t\t%s%%%s_i << env_%s;\n",outputList[k].name,
					outputList[k].name,outputList[k].name);
			
	fprintf(CFD,"\t\t} // output %s \n\n",outputList[k].name);
			}
		} /* !bittrue */
   		else if(bittrue)
   		{
			if(strcmp(outputList[k].typ,"bool")==0)
			{
   			fprintf(CFD,"\t\tfloat %s_%s = float(F_%s_%s);\n",
				Root->Name,outputList[k].name,Root->Name,
				outputList[k].name);
		fprintf(CFD,"\t\tEnvelope env_%s(*t_%s);\n",outputList[k].name,
			outputList[k].name);
          	fprintf(CFD,"\t\t%s%%%0 << env_%s;\n",outputList[k].name,
			outputList[k].name);
			}
			else
			{
   			fprintf(CFD,"\t\tfloat %s_%s;\n",Root->Name,
					outputList[k].name);
   			fprintf(CFD,"\t\tSig_Type %s_tp;\n\n",outputList[k].name);

		/*	if(outputList[k].arrSz > 1) */
			fprintf(CFD,"\t\tfor(int %s_i=0; %s_i < %d; %s_i++)\n",
				outputList[k].name,outputList[k].name,
				outputList[k].arrSz,outputList[k].name);
			fprintf(CFD,"\t\t{\n");

   			fprintf(CFD,"\t\t%s_tp = F_%s_%s",outputList[k].name,
				Root->Name,outputList[k].name);
			if(outputList[k].arrSz > 1) fprintf(CFD,"[%s_i]",
				outputList[k].name);
			fprintf(CFD,";\n");
   			fprintf(CFD,"\t\tfix_2_float (%s_tp,%s_Len, %s_FracBits,&%s_%s);\n",
				outputList[k].name,outputList[k].name,
				outputList[k].name,Root->Name,
				outputList[k].name);
   		fprintf(CFD,"\t\tFix t_%s(%s_Len, %s_IntBits,(double)%s_%s);\n",
		outputList[k].name, outputList[k].name,outputList[k].name,
		Root->Name, outputList[k].name);
          	fprintf(CFD,"\t\t%s%%%s_i << t_%s;\n",outputList[k].name,
			outputList[k].name, outputList[k].name);

		fprintf(CFD,"\t\t} // output %s \n\n",outputList[k].name);
			} /* else */
   		} /* bittrue */
   } /* for */

    fprintf(CFD, "\t} // go \n");
    fclose(fp); fclose(fq);
}

GenWrapUp()
{
    fprintf(CFD, "WrapUp()\n{\n");
    GenCloseInputFiles();
    fprintf(CFD, "\n/* Closing Display files */\n");
    GenCloseDisplayFiles(Root); /*  Process all hierarchy  */
    GenPerformPostProcessing(Root); 
    fprintf(CFD, "   exit(0);\n}\n\n");
}

GenGlobalFixPars(List)
ListPointer List;
{
    register ListPointer ptr;
    register EdgePointer edge;

    for (ptr = List; ptr != NULL; ptr = ptr->Next) {
        edge = EP(ptr);
        if (IsFixedType(edge)) {
            fprintf(CFD, ", ");
	        GenEdgeDecl(edge);
	    }
    }
}

GenGlobalFixConPars(List)
ListPointer List;
{
    register ListPointer ptr;
    register EdgePointer edge;
    ArrayDefinitionList adl, adlNext, AddToControlListDecl(), conlist = NULL;

    for (ptr = List; ptr != NULL; ptr = ptr->Next) {
	edge = EP(ptr);
	if (IsFixedType(edge)) 
	    conlist = AddToControlListDecl(edge, conlist);
    }
    for (adl = conlist; adl != NULL; adl = adlNext) {
  	    adlNext = adl->Next;
	    fprintf (CFD, ", ");
	    GenConEdgeDecl(adl);
	    free(adl);
    }
}

GenGlobalIntConPars(List)
ListPointer List;
{
    register ListPointer ptr;
    register EdgePointer edge;
    ArrayDefinitionList adl, adlNext, AddToControlListDecl(), conlist = NULL;

    for (ptr = List; ptr != NULL; ptr = ptr->Next) {
	edge = EP(ptr);
	if (!IsFixedType(edge)) 
	    conlist = AddToControlListDecl(edge, conlist);
    }
    for (adl = conlist; adl != NULL; adl = adlNext) {
	    adlNext = adl->Next;
	    fprintf (CFD, ", ");
	    GenConEdgeDecl(adl);
	    free(adl);
    }
}

GenGlobalIntPars(List)
ListPointer List;
{
    register ListPointer ptr;
    register EdgePointer edge;

    for (ptr = List; ptr != NULL; ptr = ptr->Next) {
        edge = EP(ptr);
        if (!IsFixedType(edge)) {
            fprintf(CFD, ", ");
	        GenEdgeDecl(edge);
	    }
    }
}

GenEdgeDecl(edge)
EdgePointer edge;
{
    int maxdelay;

    GenEdgeName(edge);
    if (HasAttribute(edge->Attributes, "Maxdelay")) {
        maxdelay = (int)GetAttribute(edge->Attributes, "Maxdelay");
        fprintf (CFD, "[%d]", maxdelay + 1);
    } else if (HasAttribute(edge->Attributes,"Maxloopdelay")) {
        maxdelay = (int)GetAttribute(edge->Attributes,"Maxloopdelay");
        fprintf (CFD, "[%d]", maxdelay + 1);
    }
    GenEdgeMaxDim(edge);
}

GenEdgeMaxDim(edge)
EdgePointer edge;
{
    char label[STRSIZE], value[STRSIZE], *lowstr, *highstr;
    int high, i = 0;

    sprintf(label, "index%d", i);
    while(HasAttribute(edge->Arguments, label)) {
        strcpy(value, (char *)GetAttribute(edge->Arguments, label));
        if (isnumber(value))
	        high = atoi(value) + 1;
	    else {
	        lowstr = strtok(value, "..");
	        highstr = strtok(NULL, "..");
	        high = atoi(highstr) + 1;
	    }
        fprintf(CFD, "[%d]", high);
        i++;
        sprintf(label, "index%d", i);
    }
}

GenOpenDisplayFiles(Graph)
GraphPointer Graph;
{

    if (Graph == NULL)
         return;
    fprintf(CFD,"    dfd_dump = CreateFile(\"%s\");\n", DumpFile);
}

GenMainBody(pl_flag)
bool pl_flag;
{
    GenConstInputs(pl_flag);
    fprintf(CFD, "\n/* Simulation for # cycles = MaxCycles */\n");
    fprintf(CFD, "    for (CycleCount = 1;");
    fprintf(CFD, " CycleCount < MaxCycles+1; CycleCount++) {\n");
    fprintf (CFD, "      ");
    GenProcessCall();
    fprintf(CFD, "    }\n");
}

GenOpenInputFiles()
{
    fprintf(CFD, "\n/* Opening Input files */\n");
    GenOpenInputFilesOfEdge(ListOfInputs);
}

GenOpenInputFilesOfEdge(List)
ListPointer List;
{
    register ListPointer ptr;
    register EdgePointer edge, edgeOfSIF;
    EdgePointer FindEdgeOfSIF();
    register char *file;

    for(ptr = List; ptr != NULL; ptr = ptr->Next) {
        edge = EP(ptr);
        fprintf(CFD, "    fd_");
        GenEdgeName(edge);
	    fprintf(CFD, " = ");
        if ((file = (char *)GetAttribute(edge->Attributes, "InFile")) == NULL)
	    Error("GenOpenInputFilesOfEdge: No file for input edge.");
        edgeOfSIF = FindEdgeOfSIF(file);
	    if (edgeOfSIF == edge)
	        fprintf(CFD, "OpenFile (\"%s\")", file);
        else {
	        fprintf(CFD, "fd_");
	        GenEdgeName(edgeOfSIF);
	    }
	    fprintf(CFD, ";\n");
    }
}

EdgePointer FindEdgeOfSIF(filename)
char *filename;
{
    register EdgePointer edgeOfSIF;
    EdgePointer FindEdgeOfSIFinEdge();
    
    edgeOfSIF = FindEdgeOfSIFinEdge(filename, ListOfInputs);
    if (edgeOfSIF != NULL) return(edgeOfSIF);
}

EdgePointer FindEdgeOfSIFinEdge(filename, List)
char *filename;
ListPointer List;
{
    register ListPointer ptr;
    register EdgePointer edge;
    char *file;

    for(ptr = List; ptr != NULL; ptr = ptr->Next) {
        edge = EP(ptr);
        if((file = (char *)GetAttribute(edge->Attributes, "InFile")) == NULL){
     	    printmsg(NULL, "FindEdgeOfSIFinEdge() -- No InFile for edge %s",
			edge->Name);
	        exit(-1);
        }
        if(!strcmp(file, filename)) return(edge);
    }
    return(NULL);
}

GenConstInputs(pl_flag)
bool pl_flag;
{
    if(pl_flag) fprintf(CFD, "\n\t\t/* Reading Inputs that are constant*/\n");
    else fprintf(CFD, "\n/* Reading Inputs that are constant*/\n");
    GenConstInputsOfEdge(ListOfInputs,pl_flag);
}

GenConstInputsOfEdge(List,pl_flag)
ListPointer List;
bool pl_flag;
{
    register ListPointer ptr;
    register EdgePointer edge;
    register char *file;

    for(ptr = List; ptr != NULL; ptr = ptr->Next) {
        edge = EP(ptr);
        if ((int)GetAttribute(edge->Attributes, "IsConstant")) {
            if(IsArray(edge))
		{
	    	if(pl_flag) fprintf(CFD,"\t\tReadConstArray_");
                else fprintf(CFD,"    ReadArray_");
		}
	    else
                fprintf(CFD,"    Read_");
            GenEdgeName(edge);
            if(IsArray(edge))
                fprintf(CFD,"(");
	    else
                fprintf(CFD,"(&");

	    if(pl_flag) fprintf(CFD,"\"%s.",Root->Name);
            GenEdgeName(edge);
	    if(pl_flag) fprintf(CFD,"\"");
            fprintf(CFD, ");\n");
        }
    }
}

GenProcessCall()
{
    bool GenAddressOfParams(), comma = false;

    fprintf(CFD, "\t/* Calling main simulation routine */\n");
    fprintf(CFD, "\tSim_%s (", Root->Name);
	if (GE(Root)->HasDelay) {
	fprintf(CFD, "&SigTab");
        comma = true;
    }
    fprintf(CFD, ");\n");
}

GenCloseInputFiles()
{
    fprintf(CFD, "\n/* Closing Input files */\n");
    GenCloseInputFilesOfEdge(ListOfInputs);
}

GenCloseInputFilesOfEdge(List)
ListPointer List;
{
    register ListPointer ptr;
    register EdgePointer edge;

    for(ptr = List; ptr != NULL; ptr = ptr->Next) {
        edge = EP(ptr);
        fprintf(CFD, "    fclose (fd_");
        GenEdgeName(edge);
        fprintf(CFD, ");\n");
    }
}

GenCloseDisplayFiles(Graph)
GraphPointer Graph;
{

    if (Graph == NULL)
         return;
    fprintf(CFD, "    fclose(dfd_dump);\n");
}

GenPerformPostProcessing(Graph)
GraphPointer Graph;
{
    int size;

    size = CalculateDisplayBufferSize(Graph);
    fprintf(CFD, "\n/* Performing Postprocessing */\n");
    fprintf(CFD, "#ifdef HIGHLEVEL\n");
    fprintf(CFD, "    f2cPost(\"%s\", %d*CycleCount, %d, %d, %d, 0);\n",
        FlowGraphName, size, FFTFlag, LinearFlag, MagnitudeFlag);
    fprintf(CFD, "#endif\n");
    fprintf(CFD, "#ifdef BITTRUE\n");
    fprintf(CFD, "    f2cPost(\"%s\", %d*CycleCount, %d, %d, %d, 1);\n",
        FlowGraphName, size, FFTFlag, LinearFlag, MagnitudeFlag);
    fprintf(CFD, "#endif\n");
}

CalculateDisplayBufferSize(Graph)
GraphPointer Graph;
{
    NodePointer Node;
    EdgePointer Edge;
    int max, min, value;
    int maxsize = 0;
    
    for (Edge = Graph->EdgeList; Edge != NULL; Edge = Edge->Next) {
	if (HasAttribute(Edge->Attributes, "IsDisplay"))
	    maxsize = MAX(maxsize,1);
    }

    for (Edge = Graph->ControlList; Edge != NULL; Edge = Edge->Next) {
	if (HasAttribute(Edge->Attributes, "IsDisplay"))
	    maxsize = MAX(maxsize,1);
    }

    for (Node = Graph->NodeList; Node != NULL; Node = Node->Next) {
	if (IsFuncNode(Node)) {
	    value = CalculateDisplayBufferSize(Node->Master);
	    maxsize = MAX(maxsize, value);
	} else if (IsIterationNode(Node)) {
	    max = (int)GetAttribute(Node->Arguments, "max");
	    min = (int)GetAttribute(Node->Arguments, "min");
	    value = (max-min+1)*CalculateDisplayBufferSize(Node->Master);
	    maxsize = MAX(maxsize, value);
	}
    }
    return(maxsize);
}
