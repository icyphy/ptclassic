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

FlowToC(l_flag)
bool l_flag;
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
    GenCode(l_flag); 
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

GenCode(l_flag)
bool l_flag;
{

    /* Initialize Edgetable */
    indexlevel = 0;
    Edgetable[indexlevel] = st_init_table(strcmp, st_strhash);

    GenHeading();
    fprintf (CFD, "/* Delay structure definition goes here...*/\n\n");
    GenDelayStruct(Root);  /*  Process all hierarchy  */
    GenInputFileDecls();
    GenDisplayFileDecls(Root);  /*  Process all hierarchy  */
    fprintf (CFD, ";\n\n");
    if (GE(Root)->HasDelay) {
	GenDelayStructName(Root);
	fprintf (CFD, " SigTab;\n\n");
    }
    GenConstInputDecls();
    GenFixedLeafDecls();
    GenFixedLeafInits();
    GenDelayStructInits(Root);  /*  Process all hierarchy  */
    GenFuncOpenFile();
    GenFuncCreateFile ();
    GenReadInput();
    GenMain();
    GenWrapUp();
    if (l_flag) {
        printmsg(NULL, "REDUCING LOCAL VARIABLES...\n\n");
	ReduceLocalVars(Root);
    }
    printmsg(NULL, "GENERATING FUNCTION DEFINITION CODE...\n\n");
    GenFunctions();  /*  Process all hierarchy  */
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
    fprintf(CFD,"Sig_Type ");
    for(ptr = ListOfFixedLeafs; ptr != NULL; ptr = ptr->Next) {
        edge = EP(ptr);
        GenEdgeName(edge);
        st_insert(Edgetable[indexlevel], DeclName(edge), 0);
        if (ptr->Next != NULL)
	    fprintf(CFD,", ");
    }
    fprintf(CFD,";\n\n");	
}

GenFixedLeafInits()
{
    register ListPointer ptr;
    register EdgePointer edge;
    register char *value;

    fprintf(CFD,"/* Initialize the FixedPoint globals */\n");
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

GenReadInput()
{
    register ListPointer ptr;

    fprintf(CFD, "/* Routines to read in inputs */\n");
    for (ptr = ListOfInputs; ptr != NULL; ptr = ptr->Next) {
	if (IsArray(EP(ptr))) 
	    GenReadInputControl(EP(ptr));
	else
	    GenReadInputEdge(EP(ptr));
    }
}

GenReadInputControl(edge)
EdgePointer edge;
{
    int i = 0;
    char label[STRSIZE], value[STRSIZE];
    char *lowstr, *highstr, *strtok();
    int low, high;

    fprintf(CFD, "ReadArray_");
    GenEdgeName(edge);
    fprintf(CFD, " (pIn)\n");
    fprintf(CFD, "Sig_Type pIn");
/* generate dimension of signals */
    GenEdgeMaxDim(edge);
    fprintf(CFD, ";\n{\n");

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

/* generate the routine called above */
    GenReadInputEdge(edge);
}

GenReadInputEdge(edge)
EdgePointer edge;
{
    char *Infile;

    fprintf(CFD,"Read_");
    GenEdgeName(edge);
    fprintf(CFD," (pIn)\n");
    if (IsFixedType(edge))
	    fprintf(CFD, "Sig_Type *pIn;\n");
    else 
    	fprintf(CFD, "int *pIn;\n");
    fprintf(CFD, "{\n");
    fprintf(CFD, "    float d;\n");
    fprintf(CFD, "    if (fscanf (fd_");
    GenEdgeName(edge);
    fprintf(CFD, ", \"%%f \", &d) != 1) {\n");
    fprintf(CFD, "\tprintmsg(NULL, \"Reach end of ");
    Infile = (char *)GetAttribute(edge->Attributes, "InFile");
    fprintf(CFD, "file : %s\\n\");\n", Infile);
    fprintf(CFD, "\tWrapUp();\n");
    fprintf(CFD, "    } else\n");
    if (IsFixedType(edge)) {
	    fprintf(CFD, "\tFloat2Fix (d, pIn[0]");
	    GenFixedType(edge);
	    fprintf(CFD, ");\n");
    }
    else
	    fprintf(CFD, "\tpIn[0] = d;\n");
    fprintf(CFD, "    return(0);\n");
    fprintf(CFD, "}\n\n");
}

GenMain()
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
    GenMainBody();
    fprintf(CFD, "\n    WrapUp();\n");
    fprintf(CFD, "}\n\n");
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

GenMainBody()
{
    GenConstInputs();
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

GenConstInputs()
{
    fprintf(CFD, "\n/* Reading Inputs that are constant*/\n");
    GenConstInputsOfEdge(ListOfInputs);
}

GenConstInputsOfEdge(List)
ListPointer List;
{
    register ListPointer ptr;
    register EdgePointer edge;
    register char *file;

    for(ptr = List; ptr != NULL; ptr = ptr->Next) {
        edge = EP(ptr);
        if ((int)GetAttribute(edge->Attributes, "IsConstant")) {
            if(IsArray(edge))
                fprintf(CFD,"    ReadArray_");
	        else
                fprintf(CFD,"    Read_");
            GenEdgeName(edge);
            if(IsArray(edge))
                fprintf(CFD,"(");
	        else
                fprintf(CFD,"(&");
            GenEdgeName(edge);
            fprintf(CFD, ");\n");
        }
    }
}

GenProcessCall()
{
    bool GenAddressOfParams(), comma = false;

    fprintf(CFD, "\n/* Calling main simulation routine */\n");
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
