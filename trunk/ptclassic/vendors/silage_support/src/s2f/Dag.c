/* $Id$ */

/*  This version 2.4 (6:00, 8/28/89) is the most current.  
    Uses Jan's new internal data structure.
									*/

/*  dag.c  --  builds and prints a DAG from Silage code  */

#include "sfg.h"
#include "dag.h"
#include "st.h"

extern SemanticGraph SemGraph;

GraphPointer Graphlist = NULL;
int indexlevel = 0;
int maxindexlevel;
EdgePointer EdgeList, ControlList;
NodePointer NodeList;

char *Intern(), *EdgeType(), *GetType(), *GetFunction();
pointer GetAttribute();
GraphPointer new_graph(), GraphListAppend();
ListPointer new_list(), ListAppend(), CheckExistingEdgeToNode();
ListPointer SetAttribute(), RemoveAttribute(), RemoveEntryFromList();
ListPointer SaveSilage();
EdgePointer NewEdge(), CheckEdgetable(), EdgeListAppend(), RemoveFromEdgeList();
NodePointer NewNode(), NodeListAppend();

/**************************************************************************
  ConstructDAG() -- Go through the ordered function definition list in
  SemGraph, and construct the graph for that function definition.  This is
  done by calling ConstructFuncDefs, which returns a nodelist and edgelist
  for the graph.  Input and Output edges are then determined.  Finally, when
  all construction is done, the entire structure is printed, graph by graph.
****************************************************************************/
GraphPointer
ConstructDAG()
{
    extern GraphPointer Graphlist;
    extern st_table *InstanceTable;
    extern int indexlevel, maxindexlevel;
    GraphPointer FindGraphRoot(), Root, FlowGraph;
    ListPointer ObtainIOedges();
    FunctionDefinitionList FDL;

    for (FDL = SemGraph.OrderedFuncDefs; FDL != NULL; FDL = FDL->OrderedNext) {
	EdgeList = (EdgePointer) NULL;
	NodeList = (NodePointer) NULL;
	ControlList = (EdgePointer) NULL;
	FlowGraph = new_graph();
        printmsg(NULL, "Begin Graph %s...\n", FDL->FunctionId.IdName);
	if (indexlevel != 0) {
	    printmsg(NULL,"Warning: incorrect indexlevel = %d\n\n",indexlevel);
 	}
	indexlevel = 0;  /* initialize indexlevel */
	maxindexlevel = 0;
        InitManifestEdge();
        InitCycleEdge();
        /* initialize edgetable for this function definition  */
        InitEdgetable();
	ConstructFuncDefs(FDL, FlowGraph);
	ClearCycleEdge();
	FlowGraph->Name = Intern(FDL->FunctionId.IdName);
	FlowGraph->Class = Intern("MODULE");
        FlowGraph->Model = SetAttribute("model_class", CharNode,
				  Intern("internal"), FlowGraph->Model);
        FlowGraph->Model = SetAttribute("model_name", CharNode,
				  Intern("func"), FlowGraph->Model);
	FlowGraph->EdgeList = EdgeList;
	FlowGraph->NodeList = NodeList;
	FlowGraph->ControlList = ControlList;
        FlowGraph->InEdges = ObtainIOedges(FDL->InputParameters, FlowGraph,
					   EDGETYPE, IN);
        FlowGraph->InControl = ObtainIOedges(FDL->InputParameters, FlowGraph,
					   CONTYPE, IN);
        FlowGraph->OutEdges = ObtainIOedges(FDL->OutputParameters, FlowGraph,
					   EDGETYPE, OUT);
        FlowGraph->OutControl = ObtainIOedges(FDL->OutputParameters, FlowGraph,
					   CONTYPE, OUT);
	Graphlist = GraphListAppend(FlowGraph, Graphlist);
	if (FlowGraph == NULL || st_lookup(InstanceTable, FlowGraph->Name, 
								NULL) != 0) {
	    printmsg(NULL, "User redefining function %s\n", FlowGraph->Name);
            exit(-1);
        }
	else
	    st_insert (InstanceTable, FlowGraph->Name, FlowGraph);
        printmsg(NULL, "End Graph %s...\n", FDL->FunctionId.IdName);
    }

    ResolveSubgraphParents(Graphlist);
    EliminateDeadEdges(Graphlist);
    Root = FindGraphRoot(Graphlist);

    AddIONodes(Root);
    MarkIOEdges(Root);

    return(Root);
}

/**************************************************************************
  ResolveSubgraphParents() -- Puts in Node->Master->Parents all parent
  graphs that call this subgraph Master.  This also relabels control edges.
****************************************************************************/
ResolveSubgraphParents(graphlist)
GraphPointer graphlist;
{
    ListPointer ListAdd();
    GraphPointer graph;
    NodePointer node;
    EdgePointer con;
    char *EdgeName();
    int cnt = 0;

    printf("graphlist: ");
    for (graph = graphlist; graph != NULL; graph = graph->Next, cnt++) {
        printf("%s... ", graph->Name);
	if (cnt >= 5) {
            printf("\n");
	    cnt = 0;
	}
        for (node = graph->NodeList; node != NULL; node = node->Next) {
            node->Master->Parents = ListAdd(
                new_list(GraphNode, (pointer) graph), node->Master->Parents);
	}
        for (con = graph->ControlList; con != NULL; con = con->Next) {
	    con->Name = EdgeName(con);
	}
    }
    printf("\n");
}

/**************************************************************************
  EliminateDeadEdges() -- Removes dead edge outputs from iterations and
  function calls.
****************************************************************************/
EliminateDeadEdges(graphlist)
GraphPointer graphlist;
{
    GraphPointer graph;
    NodePointer node;
    ListPointer ptr;
    EdgePointer edge;

    for (graph = graphlist; graph != NULL; graph = graph->Next) {
	for (node = graph->NodeList; node != NULL; node = node->Next) {
	    if (!IsHierarchyNode(node)) continue;
	    for (ptr = node->OutEdges; ptr != NULL; ptr = ptr->Next) {
		if (ptr->Type != EdgeNode) continue;
		edge = EP(ptr);
		if (edge->OutNodes == NULL) {
		    ptr->Type = NullNode;
		    ptr->Entry = NULL;
		    graph->EdgeList = RemoveFromEdgeList(edge, graph->EdgeList);
		    ClearEdge(edge);
		}
	    }
	    for (ptr = node->OutControl; ptr != NULL; ptr = ptr->Next) {
		if (ptr->Type != EdgeNode) continue;
		edge = EP(ptr);
		if (edge->OutNodes == NULL) {
		    ptr->Type = NullNode;
		    ptr->Entry = NULL;
		    graph->ControlList = RemoveFromEdgeList(edge, 
						graph->ControlList);
		    ClearEdge(edge);
		}
	    }
        }
    }
}

/**************************************************************************
  ConstructFuncDefs() -- Create a hashtable to store all the edges for this
  function definition.  Store all input edges into the table as well as edge
  list.  Then for each assignment statement, call ConstructSDef() to process
  the statement, and for each loop statement, call ConstructIDef() to
  process the loop statement.
****************************************************************************/
ConstructFuncDefs (FDL, parent)
    FunctionDefinitionList FDL;
    GraphPointer parent;
{
    SymbolTable ST;
    EdgePointer e, edge, start, Next;
    ValueDefinitionList vdl;
    char label[STRSIZE], index[STRSIZE];
    NodePointer InNode;
    int i, count;

/* prepare to return nodelist and edgelist for this function definition  */

/* installed edges representing input parameters into the edgetable and
   edgelist.  Marked as "lhs" to show produced  */
    for (ST = FDL->InputParameters; ST != NULL; ST = ST->Next) {
      if (ST->IsArray == FALSE) {     /* installed a data edge */
	e = NewEdge(ST->SymbolId.IdName,"data",EdgeType(ST->SymType));
        e->Attributes = SaveSilage(e->Attributes, ST->SymbolId.IdPos);
	e->Attributes = SetAttribute("lhs", IntNode, (char *)1, e->Attributes);
        EdgeList = EdgeListAppend(e, EdgeList);
      }
      else {		/* installed a control edge for array inputs  */
	e = NewEdge(ST->SymbolId.IdName, "control", EdgeType(ST->SymType));
        e->Attributes = SaveSilage(e->Attributes, ST->SymbolId.IdPos);
        for (i=0; i< ST->SingleOrArray.AS.NumberOfDimensions; i++) {
	  sprintf(label,"index%d",i);
	  sprintf(index,"0..%d",ST->SingleOrArray.AS.DimensionBounds[i]-1);
	  e->Arguments = SetAttribute(label, CharNode, Intern(index),
								e->Arguments);
	  e->Attributes = SetAttribute("lhs", IntNode,(char *)1,e->Attributes);
	}
        ControlList = EdgeListAppend(e, ControlList);
      }
      e->InNodes = ListAppend(new_list(GraphNode,parent), e->InNodes);
    }

/* go through function and installed the rest of the edges and nodes  */
    for( vdl = FDL->ValDefs; vdl != NULL; vdl = vdl->Next) {
        if (vdl->ValDefKind == SingleDef) {
	    ConstructSDef(vdl->ValDef.SDef);
	} 
	else if (vdl->ValDef.IDef->IDKind == ForLoop) {
	    ConstructIDef(vdl->ValDef.IDef);
 	}
	else if (vdl->ValDef.IDef->IDKind == DoLoop) {
	    ConstructDDef(vdl->ValDef.IDef);
	}
	else {
	    printmsg("Sil2Flow", "Unknown Silage construct\n");
	}
    }

/* gather output control edges to match FDL->OutputParameters */

    for(ST = FDL->OutputParameters; ST != NULL; ST = ST->Next) {
        count = 0;
        for(edge = ControlList; edge != NULL; edge = edge->Next) {
 	    if (strcmp(ST->SymbolId.IdName, edge->Name) == 0) {
		count++;
 	    }
	}
	if (count > 1) {
	    e = NewEdge(ST->SymbolId.IdName, "control", EdgeType(ST->SymType));
            e->Attributes = SaveSilage(e->Attributes, ST->SymbolId.IdPos);
            for (i=0;i< ST->SingleOrArray.AS.NumberOfDimensions;i++) {
	       sprintf(label,"index%d",i);
	       sprintf(index,"0..%d",ST->SingleOrArray.AS.DimensionBounds[i]-1);
	       e->Arguments = SetAttribute(label, CharNode, 
						Intern(index), e->Arguments);
	    }
            for(edge = ControlList; edge != NULL; edge = Next) {
		Next = edge->Next;
 		if (strcmp(ST->SymbolId.IdName, edge->Name) == 0) {
		    InNode = NP(edge->InNodes);
		    if (edge->OutNodes == NULL) { /* remove this edge */
			DisconnectNodeFromEdge(InNode, NodeNode, edge);
			ControlList = RemoveFromEdgeList(edge,ControlList);
			ClearEdge(edge);
		    }
		    /* ConnectNodeToControlEdge(InNode, NodeNode, e)  */
	            ConnectOutEdge(InNode, e); 
 		}
	    }
            ControlList = EdgeListAppend(e, ControlList);
	}
    }
}

/**************************************************************************
  ObtainIOedges() -- obtain input and output edges of a graph.  If the
  input or output list is given, edges are searched to match these.  If
  not, edges with NULL innodes and outnodes are searched.
***************************************************************************/
ListPointer ObtainIOedges(param, parent, type, dir)
    SymbolTable param;
    GraphPointer parent;
    int type;
    int dir;
{
    SymbolTable ST;
    EdgePointer edge, start;
    ListPointer ptr, entry, list = NULL;

    if (type == EDGETYPE)
        start = EdgeList;
    else
	start = ControlList;

    if (param != NULL) {
/* Function hierarchy, has input and output parameters  */
        if (dir == IN) {  /* inputs of functions */
            for(ST = param; ST != NULL; ST = ST->Next) {
                for(edge = start; edge != NULL; edge = edge->Next) {
 		    if (strcmp(ST->SymbolId.IdName, edge->Name) == 0) {
			entry = new_list(EdgeNode,edge);
			list = ListAppend(entry, list);
		    }
	        }
            }
	}
        else {  /* outputs of functions */
            for(ST = param; ST != NULL; ST = ST->Next) {
                for(edge = start; edge != NULL; edge = edge->Next) {
 		    if (strcmp(ST->SymbolId.IdName, edge->Name) == 0) { 
			 if(IsControlEdge(edge) && edge->OutNodes != NULL)
			     continue;
			 entry = new_list(EdgeNode,edge);
			 list = ListAppend(entry, list);
      			 edge->OutNodes = ListAppend(new_list(GraphNode,
						parent), edge->OutNodes);
		    }
	        }
	    }
        }
    }
    else {
/* iteration hierarchy, has no input or output parameters.  Thus, these are
   determined by finding edges with NULL innodes                    */
        if (dir == IN) {  /* input of iteration */
            for(edge = start; edge != NULL; edge = edge->Next) {
    	        if(edge->InNodes == NULL) {
		    entry = new_list(EdgeNode,edge);
		    list = ListAppend(entry, list);
		    edge->InNodes = ListAppend(new_list(GraphNode,parent),
				    edge->InNodes);
 	        }
    	    }
        } else {  /* output of iteration */
	    if (type == EDGETYPE) {  /* loop delay outputs  */
/* OutEdges are left hand side edges that enter delay nodes */
	        for (edge = start; edge != NULL; edge = edge->Next) {
		  if (HasAttribute(edge->Attributes, "exit") || 
		      HasAttribute(edge->Attributes, "lhs")) {
		        entry = new_list(EdgeNode, edge);
		        list = ListAppend(entry, list);
		        edge->OutNodes = ListAppend(new_list(GraphNode,
						parent), edge->OutNodes);
		  }
                }
	    } else {  /* array outputs */
/* BUG(10) out control edges of iterator graphs are found by out control 
   edges of all write and iteration nodes  				*/
	        for (edge = start; edge != NULL; edge = edge->Next) {
		  if (HasAttribute(edge->Attributes, "lhs") ||
		      HasAttribute(edge->Attributes, "exit")) {
		        entry = new_list(EdgeNode, edge);
		        list = ListAppend(entry, list);
		        edge->OutNodes = ListAppend(new_list(GraphNode,
						parent), edge->OutNodes);
		  }
                }
	    }
        }
    }
    return(list);
}

/**************************************************************************
  ConstructSDef() -- Evaluate the RHS.  If the LHS is an IndexedExp, we
  create an ArrayWritenode, an output control edge, and input data and index
  edges.  If the LHS is an initExp, we create a DELAY node, and have the
  RHS as the first edge to this node, representing the initial value.  We
  then generate and store into the table the input edge as well as the output
  edge.  This will require doing constant arithmetic on the delay value.
  Else, if the RHS is a node, we replace the names of the Tmp edges
  by the names of the LHS, else the RHS must be an edge, we create an
  Assignnode with an output edge being the LHS.
****************************************************************************/
ConstructSDef (SD)
    SingleDefinition SD;
{

    if (SD->LeftHandSide->ExpKind == IndexedExp) {
        DoIndex(SD);
    } 
    else if ((SD->LeftHandSide->ExpKind == InitExp) ||
	     (SD->LeftHandSide->ExpKind == LpInitExp)) {
        DoInit(SD);
    }
    else {
        DoRest(SD);
    }
    return;
}

/**************************************************************************
  DoIndex() -- Process the statement when the LHS is an IndexedExp.
               Assuming there is only one ReturnEdge.
               Assuming there is only one el.
****************************************************************************/
DoIndex(SD)
    SingleDefinition SD;
{
    ListPointer ConstructExp(), ReturnEdge;

    ReturnEdge = ConstructExp(SD->RightHandSide);
    DoIndexLHS(SD->LeftHandSide, ReturnEdge);
}

DoIndexLHS(E, ReturnEdge)
    Expression E;
    ListPointer ReturnEdge;
{
    int level;
    ListPointer el;
    EdgePointer e;
    NodePointer n;
    char *str;

/* LHS is an IndexedExp, requires construction of ArrayWritenode  */
    str = E->ExpDef.SonsForIndexed.ThisArray->SymbolId.IdName;
    n = NewNode(WRITE, "data");
    NodeList = NodeListAppend(n, NodeList);
    n->Attributes = SaveSilage(n->Attributes, E->ExpPos);
    n->Arguments = SetAttribute("array_name", CharNode, Intern(str),
								n->Arguments);
/* Connect data edge to node, if edge is vector, put NULL in n->InEdges */
    e = EP(ReturnEdge);
    SetType(e, Intern(EdgeType(E->ExpDef.SonsForIndexed.ThisArray->SymType)));
    ConnectInEdge(n, e);
    if (IsControlEdge(e)) {
        ConnectInEdge(n, NULL);    /* data edge is NULL */
    }

/* Connect indices as index edges or parameters to node */
    ConnectIndexToNode(E, n);

/* Install output edge, check that it does not already exist */

    if ((el=CheckExistingEdgeToNode(n,&level)) != NULL && level == indexlevel){
/* write control edge already exist, this should only happen in a delay cycle,
   with the input signal an indexed element.  If level is current indexlevel,
   just connect to node, otherwise, must make new edge also */
       e = EP(el);
       ConnectOutEdge(n, e);
       e->Attributes = SetAttribute("lhs", IntNode, (char *)1, e->Attributes);
       ClearList(el);
    }
    else {
/* write control edge needs to be generated, this should be the typical case */
       e = NewEdge(str, "control", EdgeType(E->ExpDef.SonsForIndexed.
		   ThisArray->SymType));
       e->Attributes = SaveSilage(e->Attributes, E->ExpDef.SonsForIndexed.
                   ThisArray->SymbolId.IdPos);
/* adding indexval arguments  */
       InstallIndexToEdge(n, e);
       ControlList = EdgeListAppend(e, ControlList);
       ConnectOutEdge(n,e);
       e->Attributes = SetAttribute("lhs", IntNode, (char *)1, e->Attributes);
    }
} 

/**************************************************************************
  DoInit() -- Process the statement when the LHS is an (Lp)InitExp.
****************************************************************************/
DoInit(SD)
    SingleDefinition SD;
{
    extern int indexlevel;
    NodePointer n;
    EdgePointer e;
    ListPointer ReturnEdge, ConstructExp();
    char *name, *ptr, *SPrintExp(), *IndexName();
    int value, index = FALSE;
    int LpDelay = FALSE;
    char string[STRSIZE], val[STRSIZE], str[STRSIZE], *Type;	
    Expression DelaySig, RHS, DelayVal;

    if (SD->LeftHandSide->ExpKind == LpInitExp)
	LpDelay = TRUE;
    n = (LpDelay == TRUE) ? NewNode(LPDELAY, "data") : NewNode(DELAY, "data") ;
    NodeList = NodeListAppend(n, NodeList);
    n->Attributes = SaveSilage(n->Attributes, SD->LeftHandSide->ExpPos);
    DelaySig = SD->LeftHandSide->ExpDef.SonsForDelay.DelaySigExp;
    DelayVal = SD->LeftHandSide->ExpDef.SonsForDelay.DelayValExp;
    if (DelaySig->ExpKind==IndexedExp){
        name = IndexName(DelaySig);
	index = TRUE;
    } else {
        name = DelaySig->ExpDef.ThisSymbol->SymbolId.IdName;
    }
    RHS = SD->RightHandSide;
    Type = EdgeType(RHS->ExpType);

/* look to see if the signal one delay value earlier exist, if yes, can connect
   delays.  First, construct the name of the edge one delay value earlier,and
   check in Edgetable.							*/
    if (DelayVal->ExpKind == IntegerLeafExp && (value = DelayVal->ExpDef.
				IntegerConstant - 1) > 0) {
       if (LpDelay == TRUE)
           sprintf(string,"%s#%d", name, value);
       else
           sprintf(string,"%s@%d", name, value);
       if ((e = CheckEdgetable(string)) != NULL) {
	  ConnectInEdge(n,e);
	  ConnectInEdge(n, NULL); /* delay value is 1 */
	  n->Arguments = SetAttribute("delay",IntNode,(char *)1,n->Arguments);
	  goto cont;
       }
    }

/* look for signal to be delayed, if does not exist, create new edge, and put
   in Edgetable, but do not add to edgelist, it is temporary to hold node 
   connections.  Later, when the signal is encountered, as a LHS of an
   assignment, we will obtain the edge-node connections from here.  Store
   this new edge in CycleEdge[] incase we do not locate a loop at this
   level of hierarchy, we have to make it an input edge of the next higher
   level.
*/

    if (index == TRUE) {
       	ReturnEdge = ConstructExp(DelaySig);
	e = EP(ReturnEdge);
	LabelDelayControlEdge(e);
    } else {
        if ((e = CheckEdgetable(name)) == NULL) {
            e = NewEdge(name, "data", Type);
            e->Attributes = SaveSilage(e->Attributes, DelaySig->ExpPos);
            AddCycleEdge(e);
	}
    }
    ConnectInEdge(n,e);

/* handle delay-value parameter or edge */

    if (DelayVal->ExpKind == IntegerLeafExp) {
        n->Arguments = SetAttribute("delay", IntNode, (char *)DelayVal->ExpDef.
		        IntegerConstant, n->Arguments);
	ConnectInEdge(n, NULL);
    }
    else {    
	ReturnEdge = ConstructExp(DelayVal);
	ConnectInEdge(n, EP(ReturnEdge));
        if (!HasAttribute(EP(ReturnEdge)->Arguments, "manifest")) {
            printmsg("DoInit:","Non-manifest index edge %s\n", 
						EP(ReturnEdge)->Name);
	    PrintSilage(EP(ReturnEdge)->Attributes);
            EP(ReturnEdge)->Arguments = SetAttribute("manifest", CharNode,
                                Intern("unknown"), EP(ReturnEdge)->Arguments);
        }
    }

/* handle initial-value parameter or edge */

cont:  if (RHS->ExpKind == FixedLeafExp) {
 	   if (RHS->ExpDef.SonsForFixedLeaf.FixedString == NULL) {
		sprintf(str,"%f", 
		(double)RHS->ExpDef.SonsForFixedLeaf.M /
		(double)RHS->ExpDef.SonsForFixedLeaf.S); 
           }
           else {
	       if (RHS->ExpDef.SonsForFixedLeaf.M < 0) {
		    sprintf(str,"-%s",RHS->ExpDef.SonsForFixedLeaf.FixedString);
	       }
	       else {
		    sprintf(str,"%s", RHS->ExpDef.SonsForFixedLeaf.FixedString);
	       }
           }
           n->Arguments = SetAttribute("init", CharNode, Intern(str),
		        				n->Arguments);
	   ConnectInEdge(n, NULL);
	}
	else if (RHS->ExpKind == IntegerLeafExp) {
	    sprintf(str,"%d", RHS->ExpDef.IntegerConstant);
            n->Arguments = SetAttribute("init", CharNode, Intern(str),
		        				n->Arguments);
	    ConnectInEdge(n, NULL);
	}
	else {
            ReturnEdge = ConstructExp(RHS);
	    ConnectInEdge(n, EP(ReturnEdge));
	}

/* Create output edge, and connect to delay node.  */

	val[0] = '\0';
	ptr = SPrintExp(DelayVal,val);
	*ptr = '\0';
	if (LpDelay == TRUE)
	    sprintf(string,"%s#%s", name, val);
        else
	    sprintf(string,"%s@%s", name, val);
	if ((e = CheckEdgetable(string)) == NULL) {
	     strcpy(str, string);
	     e = NewEdge(str, "data", Type);
             e->Attributes = SaveSilage(e->Attributes,SD->LeftHandSide->ExpPos);
 	     EdgeList = EdgeListAppend(e, EdgeList);
	}
	ConnectOutEdge(n, e);
}

/**************************************************************************
  DoRest() -- If RHS is a node, replaces the name of the returning inter-
  mediate edge by the name of the LHS.  If RHS is not a node, create an
  assignnode, and an LHS edge.  Multiple expressions are handled.
****************************************************************************/
DoRest(SD)
    SingleDefinition SD;
{
    ListPointer right;

    right = ConstructExp(SD->RightHandSide);
    DoRestLHS(SD, right);
}

/**************************************************************************
  DoRestLHS() -- If RHS is a node, replaces the name of the returning inter-
  mediate edge by the name of the LHS.  If RHS is not a node, create an
  assignnode, and an LHS edge.  Multiple expressions are handled.
****************************************************************************/
DoRestLHS(SD, r)
    SingleDefinition SD;
    ListPointer r;
{
    extern int indexlevel;
    NodePointer n;
    Expression E, Etmp;
    EdgePointer e, re, le;
    ListPointer l, ConstructExp();
    char *str, *GetFunction();
    extern int Edgenum;

    E = SD->LeftHandSide;
    if (Isnode(SD->RightHandSide)) {
/* RHS is a node, thus we replace the name of the temporary edge of the RHS
   by the name given by the LHS.  This will create scalar data output
   parameters also.  Below, we take care of RHS being TupleExp or SingleExp.
   We cannot decrement Edgenum in TupleExp.  See 7/16/90 notes */
	if (E->ExpKind == TupleExp) {
	    for(;r != NULL; r=r->Next,E=E->ExpDef.SonsForTuple.RestOfTuple){
		Etmp = E->ExpDef.SonsForTuple.FirstExpression;
		if (Etmp->ExpKind == IndexedExp) {
		    DoIndexLHS(Etmp, r);
		} else {
		    str = Etmp->ExpDef.ThisSymbol->SymbolId.IdName;
		    re = EP(r);
	            if ((e = CheckEdgetable(str)) != NULL){
		        n = NP(e->OutNodes);
		        if(!IsLoopNode(n) && strcmp(GetFunction(n), "@") && 
					   strcmp(GetFunction(n),"#")) {
		            printmsg("DoRest()", 
			  "Non-delayed data edge %s already exist\n",e->Name);
			    PrintSilage(e->Attributes);
			}
/* Found the feedback loop, remove edge with this name from CycleEdge[] */
			RemoveCycleEdge(e);
		        ReplaceEdge(e, re);
	            }
	            re->Name = Intern(str);
 		    AddToEdgetable(re);
 		    SetType(re, Intern(EdgeType(Etmp->ExpDef.ThisSymbol->
			SymType)));
                    re->Attributes = SetAttribute("lhs", IntNode, (char *)1,
			re->Attributes);
                    re->Attributes = SaveSilage(re->Attributes, Etmp->ExpDef.
					ThisSymbol->SymbolId.IdPos);
		}
	    }
	} else {
	    str = E->ExpDef.ThisSymbol->SymbolId.IdName;
	    re = EP(r);
	    if ((e = CheckEdgetable(str)) != NULL) { 
		n = NP(e->OutNodes);
		if(!IsLoopNode(n) && strcmp(GetFunction(n), "@") && 
					     strcmp(GetFunction(n),"#")) {
		    printmsg("DoRest()", 
		    "Non-delayed data edge %s already exist\n", e->Name);
		    PrintSilage(e->Attributes);
		}
/* Found the feedback loop, remove edge with this name from CycleEdge[] */
		RemoveCycleEdge(e);
		ReplaceEdge(e, re);      /* e is Dummyedge */
	    }
	    re->Name = Intern(str);
 	    AddToEdgetable(re); /* may overwrite old str */
 	    SetType(re, Intern(EdgeType(E->ExpDef.ThisSymbol->SymType)));
            re->Attributes = SetAttribute("lhs", IntNode, (char *)1,
			re->Attributes);
            re->Attributes = SaveSilage(re->Attributes, E->ExpDef.
					ThisSymbol->SymbolId.IdPos);
	    Edgenum--;
	}
    } else { 
/* RHS is an edge, create an Assignnode, and connect.  Create a new edge
   for the LHS.  The new name is automatically installed into edgetable  
   by the ConstructExp() function    */
	if (E->ExpKind == TupleExp) {
	    for(;r != NULL; r=r->Next,E = E->ExpDef.SonsForTuple.RestOfTuple){
		Etmp = E->ExpDef.SonsForTuple.FirstExpression;
		if (Etmp->ExpKind == IndexedExp) {
		    DoIndexLHS(Etmp, r);
		} else {
		    str = Etmp->ExpDef.ThisSymbol->SymbolId.IdName;
	            if ((le = CheckEdgetable(str)) != NULL){
	                EdgeList = EdgeListAppend(le, EdgeList);
/* Found the feedback loop, remove edge with this name from CycleEdge[] */
		        RemoveCycleEdge(le);
	            } else {
		        l = ConstructExp(Etmp);
		        le = EP(l);
                    }
	            n = NewNode(ASSIGN, "data");
	            re = EP(r);
	            ConnectInEdge(n, re);
	            ConnectOutEdge(n, le);
                    le->Attributes = SetAttribute("lhs", IntNode, (char *)1,
			le->Attributes);
                    le->Attributes = SaveSilage(le->Attributes, Etmp->ExpDef.
					ThisSymbol->SymbolId.IdPos);
	            NodeList = NodeListAppend(n, NodeList);
    		    n->Attributes = SaveSilage(n->Attributes, Etmp->ExpPos);
		}
	    }
        } else {
	    str = E->ExpDef.ThisSymbol->SymbolId.IdName;
	    if ((le = CheckEdgetable(str)) != NULL) {
	        EdgeList = EdgeListAppend(le, EdgeList);
/* Found the feedback loop, remove edge with this name from CycleEdge[] */
		RemoveCycleEdge(le);
	    } else {
		l = ConstructExp(E);
		le = EP(l);
            }
	    n = NewNode(ASSIGN, "data");
    	    n->Attributes = SaveSilage(n->Attributes, E->ExpPos);
	    re = EP(r);
	    ConnectInEdge(n, re);
	    ConnectOutEdge(n, le);
            le->Attributes = SetAttribute("lhs", IntNode, (char *)1,
			le->Attributes);
            le->Attributes = SaveSilage(le->Attributes, E->ExpDef.
					ThisSymbol->SymbolId.IdPos);
	    NodeList = NodeListAppend(n, NodeList);
        }  
   }
}

/**************************************************************************
  ReplaceEdge() -- replaces all connections of an old edge under a name
  with a new edge of the same name.
****************************************************************************/
ReplaceEdge(oldedge, newedge)
    EdgePointer oldedge, newedge;
{
    ListPointer nl, el;

    newedge->OutNodes = oldedge->OutNodes;
    for (nl = oldedge->OutNodes; nl != NULL; nl = nl->Next) {
	for (el = NP(nl)->InEdges;el != NULL; el = el->Next) {
	    if (el->Entry == (pointer) oldedge) 
		el->Entry = (pointer) newedge;
	}
    }
}

/**************************************************************************
  IsNode() --  Checks to see if the Expression is a leaf or an expression.
  If it is a compound expression, then in ConstructSDef(), we just replace
  its temporary name with the name of the LHS.  If it is a leaf, we create
  an Assignnode.  
****************************************************************************/
Isnode(E)
    Expression E;
{
    Expression exp;

	switch(E->ExpKind) {
            case IndexedExp :
	    case DelayExp :
	    case LpDelayExp :
	    case IdentLeafExp :
	    case FixedLeafExp :
	    case IntegerLeafExp :	return(FALSE);
	    case TupleExp :
		for(exp=E;exp!=NULL;exp=exp->ExpDef.SonsForTuple.RestOfTuple) {
		    if (!Isnode(exp->ExpDef.SonsForTuple.FirstExpression))
			return(FALSE);
		}
		return(TRUE);
	    default:			return(TRUE);
	}
}

/**************************************************************************
  ConstructIDef() -- Process a Iterator Statement by creating a Iternode,
  a subgraph representing the loop, IOedges for the graph, determining from
  the IOedges of the graph the IOedges of the Iternode, add index attributes
  to the Iter node.
****************************************************************************/
ConstructIDef(ID)
    IteratedDefinition ID;
{
    extern GraphPointer Graphlist;
    extern st_table *InstanceTable;
    GraphPointer IterGraph;
    NodePointer n, TmpNodeList;
    EdgePointer TmpEdgeList, TmpControlList;
    ListPointer ObtainIOedges();
    char *TmpGraphName();
    int i, DependencyType, LB, UB;

/* save NodeList, EdgeList, ControlList because building a new graph inside
   anther graph.  Instead of also saving Edgetable, we make it a local
   variable, so stacks takes care of it. 				*/

    TmpNodeList = NodeList;	
    TmpEdgeList = EdgeList;
    TmpControlList = ControlList;
    NodeList = (NodePointer) NULL;
    EdgeList = (EdgePointer) NULL;
    ControlList = (EdgePointer) NULL;

    IterGraph = new_graph();
    IterGraph->Name = TmpGraphName();
    IterGraph->Class = Intern("MODULE");
    IterGraph->Model = SetAttribute("model_class", CharNode,
				  Intern("internal"), IterGraph->Model);
    IterGraph->Model = SetAttribute("model_name", CharNode,
				  Intern("iteration"), IterGraph->Model);
    printmsg(NULL, "Begin Graph %s with index %s\n", IterGraph->Name, 
			ID->ThisIterator->SymbolId.IdName);
    DependencyType = ConstructIterDefs(ID);
    IterGraph->EdgeList = EdgeList;
    IterGraph->NodeList = NodeList;
    IterGraph->ControlList = ControlList;
    IterGraph->InEdges = ObtainIOedges(NULL, IterGraph, EDGETYPE, IN);
    IterGraph->InControl = ObtainIOedges(NULL, IterGraph, CONTYPE, IN);
    IterGraph->OutEdges = ObtainIOedges(NULL, IterGraph, EDGETYPE, OUT);
    IterGraph->OutControl = ObtainIOedges(NULL, IterGraph, CONTYPE, OUT);
    st_insert (InstanceTable, IterGraph->Name, IterGraph);
    Graphlist = GraphListAppend(IterGraph, Graphlist);
    printmsg(NULL, "End Graph %s...\n", IterGraph->Name);

    EdgeList = TmpEdgeList;
    NodeList = TmpNodeList;
    ControlList = TmpControlList;
/*
 * n->Master automatically points to IterGraph through NewNode()
 */
    n = NewNode(IterGraph->Name, "Hierarchy");
/*
 * Add Attributes and Arguments to Iteration node.
 */
    if ( DependencyType == PARALLEL)
        n->Attributes = SetAttribute("Dependency", CharNode, Intern("parallel"),
                      n->Attributes);
    else
        n->Attributes = SetAttribute("Dependency", CharNode, Intern("serial"),
                      n->Attributes);

    n->Arguments = SetAttribute("index", CharNode, Intern(ID->ThisIterator->
                      SymbolId.IdName), n->Arguments);
    n->Arguments = SetAttribute("indexlevel", IntNode, indexlevel,
                                n->Arguments);
    LB = EvalManExp(ID->LowerBound);
    n->Arguments = SetAttribute("min", IntNode, LB, n->Arguments);
    UB = EvalManExp(ID->UpperBound);
    n->Arguments = SetAttribute("max", IntNode, UB, n->Arguments);

/*
 * from inedge and outedge lists of IterGraph, determine inedge and outedge
 * of iterator node.
 */
    AddIOEdges(ID,n,IterGraph);

    NodeList = NodeListAppend(n, NodeList);

}

/**************************************************************************
  ConstructIterDefs() --  create the subgraph of the iterator loop, calls
  ConstructIterVar() to install first a Constant node for the loop
  variable, and an edge representing the variable.  Then, go through and
  process statements inside the loop.
****************************************************************************/
ConstructIterDefs(ID)
    IteratedDefinition ID;
{
    extern int indexlevel, maxindexlevel;
    ValueDefinitionList vdl;
    EdgePointer IndexEdge, ConstructIterVar();
    ListPointer ptr;

/* increment indexlevel */
    indexlevel++;
    maxindexlevel++;
    InitManifestEdge();
    InitCycleEdge();
    InitEdgetable();

/* make a Constant node for the loop variable with correct range, and an
   edge for the variable  */

    IndexEdge = ConstructIterVar(ID);

    for( vdl = ID->IterValDefs; vdl != NULL; vdl = vdl->Next) {
        if (vdl->ValDefKind == SingleDef) {
	    ConstructSDef(vdl->ValDef.SDef);
	} 
	else if (vdl->ValDef.IDef->IDKind == ForLoop) {
	    ConstructIDef(vdl->ValDef.IDef);
 	}
	else {
	    printmsg("Sil2FLow", "Unknown Silage construct\n");
	}
    }
/* Process leftover edges in CycleEdge[], and put in EdgeList or ControlList */
    PromoteCycleEdge();

/* decrement indexlevel */
    ClearManifestEdge();
    indexlevel--;

    return(DependencyCheck(IndexEdge));
}

/**************************************************************************
   ConstructIterVar() --  contructs constant node with range for iterator,
   an edge for the loop variable, enters edge into table, and put these 2
   items into node and edge lists.
****************************************************************************/
EdgePointer
ConstructIterVar(ID)
    IteratedDefinition ID;
{
    NodePointer n;
    EdgePointer e;
    char *range, *MakeRange();

    n = NewNode(CONST, "data");
    range = MakeRange(EvalManExp(ID->LowerBound), EvalManExp(ID->UpperBound));
    n->Arguments = SetAttribute("value", CharNode, range, n->Arguments);
    e = NewEdge(ID->ThisIterator->SymbolId.IdName, "Iterator", EdgeType(ID->
	ThisIterator->SymType));
    e->Attributes = SaveSilage(e->Attributes, ID->ThisIterator->SymbolId.IdPos);
    e->Arguments = SetAttribute("manifest", CharNode, range, e->Arguments);
    AddManifestEdge(e);
    ConnectOutEdge(n,e);
    NodeList = NodeListAppend(n, NodeList);
    EdgeList = EdgeListAppend(e, EdgeList);
    return(e);
}
    
/**************************************************************************
  ConstructDDef() -- Process a DoExit Statement by creating a Donode,
  a subgraph representing the loop, IOedges for the graph, determining from
  the IOedges of the graph the IOedges of the Donode.
****************************************************************************/
ConstructDDef(ID)
    IteratedDefinition ID;
{
    GraphPointer DoGraph;
    NodePointer n, TmpNodeList;
    EdgePointer TmpEdgeList, TmpControlList;
    ListPointer ObtainIOedges(), ObtainDoOutedges();
    char *TmpGraphName();

/* save NodeList, EdgeList, ControlList because building a new graph inside
   anther graph.  Instead of also saving Edgetable, we make it a local
   variable, so stacks takes care of it.                                */

    TmpNodeList = NodeList;
    TmpEdgeList = EdgeList;
    TmpControlList = ControlList;
    NodeList = (NodePointer) NULL;
    EdgeList = (EdgePointer) NULL;
    ControlList = (EdgePointer) NULL;

    DoGraph = new_graph();
    DoGraph->Name = TmpGraphName();
    DoGraph->Class = Intern("MODULE");
    DoGraph->Model = SetAttribute("model_class", CharNode,
                                  Intern("internal"), DoGraph->Model);
    DoGraph->Model = SetAttribute("model_name", CharNode,
                                  Intern("do"), DoGraph->Model);
    printmsg(NULL, "Begin DoLoop Graph %s\n", DoGraph->Name);
    ConstructDoDefs(ID);
    DoGraph->EdgeList = EdgeList;
    DoGraph->NodeList = NodeList;
    DoGraph->ControlList = ControlList;
    DoGraph->InEdges = ObtainIOedges(NULL, DoGraph, EDGETYPE, IN);
    DoGraph->InControl = ObtainIOedges(NULL, DoGraph, CONTYPE, IN);
    DoGraph->OutEdges = ObtainDoOutedges(ID->ExitDef->LeftHandSide, DoGraph,
								 EDGETYPE);
    DoGraph->OutControl = ObtainDoOutedges(ID->ExitDef->LeftHandSide, DoGraph,
								 CONTYPE);
    st_insert (InstanceTable, DoGraph->Name, DoGraph);
    Graphlist = GraphListAppend(DoGraph, Graphlist);
    printmsg(NULL, "End Graph %s...\n", DoGraph->Name);

    EdgeList = TmpEdgeList;
    NodeList = TmpNodeList;
    ControlList = TmpControlList;
/*
 * n->Master automatically points to IterGraph through NewNode()
 */
    n = NewNode(DoGraph->Name, "Hierarchy");

/*
 * Add Attributes to Do node.
 */
    n->Attributes = SetAttribute("Dependency", CharNode, Intern("serial"),
                      n->Attributes);

/*
 * from inedge and outedge lists of IterGraph, determine inedge and outedge
 * of iterator node.
 */
    AddIOEdges(ID,n,DoGraph);

    NodeList = NodeListAppend(n, NodeList);
}

/**************************************************************************
  ConstructDoDefs() --  create the subgraph of the Do loop.
****************************************************************************/
ConstructDoDefs(ID)
    IteratedDefinition ID;
{
    GraphPointer DoBodyGraph;
    NodePointer n;

/* increment indexlevel */
    indexlevel++;
    maxindexlevel++;
    InitManifestEdge();
    InitCycleEdge();
    InitEdgetable();

    DoBodyGraph = new_graph();
    DoBodyGraph->Name = TmpGraphName();
    DoBodyGraph->Class = Intern("MODULE");
    DoBodyGraph->Model = SetAttribute("model_class", CharNode,
                                  Intern("internal"), DoBodyGraph->Model);
    DoBodyGraph->Model = SetAttribute("model_name", CharNode,
                                  Intern("dobody"), DoBodyGraph->Model);
    printmsg(NULL, "Begin DoBodyLoop Graph %s\n", DoBodyGraph->Name);
    if (EdgeList != NULL || NodeList != NULL || ControlList != NULL)
	printmsg("ConstructDoDefs:", "EdgeList, NodeList not NULL\n");
    ConstructDoBodyDefs(ID);
    DoBodyGraph->EdgeList = EdgeList;
    DoBodyGraph->NodeList = NodeList;
    DoBodyGraph->ControlList = ControlList;
    DoBodyGraph->InEdges = ObtainIOedges(NULL, DoBodyGraph, EDGETYPE, IN);
    DoBodyGraph->InControl = ObtainIOedges(NULL, DoBodyGraph, CONTYPE, IN);
    DoBodyGraph->OutEdges = ObtainIOedges(NULL, DoBodyGraph, EDGETYPE, OUT);
    DoBodyGraph->OutControl = ObtainIOedges(NULL, DoBodyGraph, CONTYPE, OUT);

    st_insert (InstanceTable, DoBodyGraph->Name, DoBodyGraph);
    Graphlist = GraphListAppend(DoBodyGraph, Graphlist);
    printmsg(NULL, "End Graph %s...\n", DoBodyGraph->Name);

    EdgeList = (EdgePointer) NULL;
    NodeList = (NodePointer) NULL;
    ControlList = (EdgePointer) NULL;

    n = NewNode(DoBodyGraph->Name, "Hierarchy");
    NodeList = NodeListAppend(n, NodeList);
    AddIOEdges(ID,n,DoBodyGraph);
    ConstructExitComputation(ID->ExitDef, n);

/* decrement indexlevel */
    ClearManifestEdge();
    indexlevel--;

    return;
}

/**************************************************************************
  ConstructDoBodyDefs() --  create the subgraph of the DoBody loop.
****************************************************************************/
ConstructDoBodyDefs(ID)
    IteratedDefinition ID;
{
    ValueDefinitionList vdl;
    EdgePointer e, ConstructExitTest();
    NodePointer n;

/* increment indexlevel */
    indexlevel++;
    maxindexlevel++;
    InitManifestEdge();
    InitCycleEdge();
    InitEdgetable();

    for( vdl = ID->IterValDefs; vdl != NULL; vdl = vdl->Next) {
        if (vdl->ValDefKind == SingleDef) {
            ConstructSDef(vdl->ValDef.SDef);
        }
        else if (vdl->ValDef.IDef->IDKind == ForLoop) {
            ConstructIDef(vdl->ValDef.IDef);
        }
        else if (vdl->ValDef.IDef->IDKind == DoLoop) {
	    ConstructDDef(vdl->ValDef.IDef);
	}
	else {
	    printmsg("Sil2Flow", "Unknown Silage construct\n");
	}
    }
    e = ConstructExitTest(ID->ExitDef->RightHandSide, 0);
    n = NewNode(EXIT, "data");
    n->Attributes= SaveSilage(n->Attributes,ID->ExitDef->RightHandSide->ExpPos);
    NodeList = NodeListAppend(n, NodeList);
    ConnectInEdge(n, e);

/* decrement indexlevel */
    ClearManifestEdge();
    indexlevel--;

    return;
}

/**************************************************************************
  ConstructExitTest() --  Add to subgraph the loop test.
****************************************************************************/
EdgePointer
ConstructExitTest(E, val)
    Expression E;
    int val;
{
    ListPointer res;
    EdgePointer e, r = NULL, l, ConstructExitTest();
    NodePointer n;

    res = ConstructExp(E->ExpDef.SonsForCond.IfExp);
    l = EP(res);
    l->Attributes = SetAttribute("exit", IntNode, (char *)val, l->Attributes);
    if (E->ExpDef.SonsForCond.ElseExp != NULL &&
	E->ExpDef.SonsForCond.ElseExp->ExpKind == CondExp)
        r=ConstructExitTest(E->ExpDef.SonsForCond.ElseExp, val+1);
    if (r == NULL)
	return(l);
    else {
	n = NewNode(OR, "data");
	e = NewEdge(NULL, "data", GetType(r));
        e->Attributes = SaveSilage(e->Attributes, E->ExpPos);
	ConnectOutEdge(n, e);
	ConnectInEdge(n, l);
	ConnectInEdge(n, r);
	NodeList = NodeListAppend(n, NodeList);
	EdgeList = EdgeListAppend(e, EdgeList);
	return(e);
    }
}

/**************************************************************************
  ConstructExitComputation() -- Performs final computation aftex exit
****************************************************************************/
ConstructExitComputation(ExitDef, DoBodyNode)
    SingleDefinition ExitDef;
    NodePointer DoBodyNode;
{
    ListPointer res, ConstructExitComp();

    res = ConstructExitComp(ExitDef->RightHandSide, DoBodyNode, 0);
    DoRestLHS(ExitDef, res);
}

/**************************************************************************
  ConstructExitComp() -- Performs final computation aftex exit
****************************************************************************/
ListPointer
ConstructExitComp(E, DoBodyNode, val)
    Expression E;
    NodePointer DoBodyNode;
    int val;
{
    ListPointer l, left, right, entry, ReturnEdge;
    EdgePointer e, boole;
    NodePointer n;

    right = NULL;
    ReturnEdge = NULL;

/* Find exit test boolean edge */
    for (l = DoBodyNode->OutEdges; l != NULL; l = l->Next) {
	if (HasAttribute(EP(l)->Attributes, "exit")) {
	    if (val == (int)GetAttribute(EP(l)->Attributes, "exit")) {
	        boole = EP(l);
	        break;
	    }
        }
    }

    left = ConstructExp(E->ExpDef.SonsForCond.ThenExp);
    if (E->ExpDef.SonsForCond.ElseExp != NULL)
         right = ConstructExitComp(E->ExpDef.SonsForCond.ElseExp, DoBodyNode,
							val+1);
    for (l = left; l != NULL; l = l->Next) {
        n = NewNode(MUX, "data");
        ConnectInEdge(n, boole);
        ConnectInEdge(n, EP(l));
        if (right != NULL) {
            ConnectInEdge(n, EP(right));
            right = right->Next;
        } else
            ConnectInEdge(n, NULL);
        e = NewEdge (NULL,"data",GetType(EP(l)));
        e->Attributes = SaveSilage(e->Attributes, E->ExpPos);
        ConnectOutEdge(n, e);
        NodeList = NodeListAppend(n, NodeList);
        EdgeList = EdgeListAppend(e, EdgeList);
        entry = new_list(EdgeNode, e);
        ReturnEdge = ListAppend(entry, ReturnEdge);
    }
    return(ReturnEdge);
}
/**************************************************************************
  ObtainDoOoutedges() -- obtain output edges of a Dograph using the LHS
  of the ExitDef to find output edges.
***************************************************************************/

ListPointer ObtainDoOutedges(E, parent, type)
    Expression E;
    GraphPointer parent;
    int type;
{
    EdgePointer edge, start;
    ListPointer ptr, entry, list = NULL;
    ListPointer ObtainDoOutedges();

    if (type == EDGETYPE)
	start = EdgeList;
    else
	start = ControlList;

    switch (E->ExpKind) {
	case IdentLeafExp :
	    for(edge = start; edge != NULL; edge = edge->Next) {
               if (!strcmp(E->ExpDef.ThisSymbol->SymbolId.IdName,edge->Name)){
                    entry = new_list(EdgeNode,edge);
                    edge->OutNodes = ListAppend(new_list(GraphNode,
                                                parent), edge->OutNodes);
		    return(entry);
               }
	    }
	    break;
	case IndexedExp :
            for(edge = start; edge != NULL; edge = edge->Next) {
               if (!strcmp(E->ExpDef.SonsForIndexed.ThisArray->SymbolId.IdName,
								edge->Name)){
                    entry = new_list(EdgeNode,edge);
                    edge->OutNodes = ListAppend(new_list(GraphNode,
                                                parent), edge->OutNodes);
                    return(entry);
               }
            }
            break;
	case TupleExp :
	    do {
	        entry = ObtainDoOutedges(E->ExpDef.SonsForTuple.FirstExpression,
					 parent, type);
                list = ListAppend(entry, list);
		E = E->ExpDef.SonsForTuple.RestOfTuple;
	    } while (E != NULL);
	    return(list);
	    break;
	default :
	    printmsg("ObtainDoOutedges:", "Unknown Expression %d\n", 
						E->ExpKind);
	    break;
    }
    return(list);
}

/**************************************************************************
  AddIOEdges() -- Determine the input and output edges of the Iternode from
  the subgraph of the Iternode.  Input edges are easy since they are already
  present in the Edgetable[], so only name matching has to be done.  We have
  to create output edges, and have to insert index arguments on these edges
  to match what is written in the loop.  So far, this routine is not very
  intelligent.
****************************************************************************/
AddIOEdges(ID,n,graph)
    IteratedDefinition ID;
    NodePointer n;
    GraphPointer graph;
{
    extern ListPointer CheckExistingEdgeToEdge();
    EdgePointer e;
    ListPointer el, l, l1;
    char *str, *Type;
    int exitnum, level;


/* perform input data edges.  For edges not yet present, we add it 
 * to EdgeList.  If the edge is a delay cycle edge from lower level hierarchy,
 * We only add to Edgetable and CycleEdge[].  We will finally add it
 * to EdgeList when the cycle is complete.  To test whether an edge is a delay
 * cycle edge, we traverse CycleEdge from current indexlevel to deepest
 * level to look for the edge.
 */
    for(el = graph->InEdges; el != NULL; el = el->Next) {
       str = EP(el)->Name;
       if ((e = CheckEdgetable(str)) == NULL) {
          Type = GetType(EP(el));
	  e = NewEdge(str, EP(el)->Class, Type);
	  if (IsCycleEdge(EP(el)))
              AddCycleEdge(e);
	  else
	      EdgeList = EdgeListAppend(e, EdgeList);
       }	
       ConnectInEdge(n,e);
    }

/* perform input control edges */
    for(el = graph->InControl; el != NULL; el = el->Next) {
       str = EP(el)->Name;
       if ((l = CheckExistingEdgeToEdge(EP(el), &level)) != NULL &&
						level == indexlevel) {
/* got back an edgelist, connect this list to node n  */
           for(l1 = l; l1 != NULL; l1 = l1->Next) {
               ConnectInEdge(n, EP(l1));
           }
	   ClearList(l);
       }
       else {  /* Cycle edge or edge exists from lower indexlevel */
           Type = GetType(EP(el));
	   e = NewEdge(str, EP(el)->Class, Type);
           ControlList = EdgeListAppend(e, ControlList);
           ConnectInEdge(n,e);
           InsertIndex(e, EP(el));
	   if (l != NULL) { /* edge exists from lower indexlevel, copy range */
	       CopyIndexEval(EP(l), e);
	   }
       }	
    }

/* making output data edge, must be a loop delay variable */
    for(el = graph->OutEdges; el != NULL; el = el->Next) {
	str = EP(el)->Name;
	if ((e = CheckEdgetable(str)) != NULL) {
	    printmsg("AddIOEdges:", "output edge %s already exist\n", e->Name);
	    PrintSilage(e->Attributes);
	}
        Type = GetType(EP(el));
	e = NewEdge(str, EP(el)->Class, Type);
	EdgeList = EdgeListAppend(e, EdgeList);
	if (HasAttribute(EP(el)->Attributes, "exit")) {
	  exitnum = (int)GetAttribute(EP(el)->Attributes, "exit");
          e->Attributes = SetAttribute("exit", IntNode, (char *)exitnum,
								e->Attributes);
	} else
          e->Attributes = SetAttribute("lhs", IntNode, (char *)1,e->Attributes);
   	ConnectOutEdge(n,e);
    }

/* making output control edges of iter node, use CheckExistingEdgeToEdge().
   Check range of graph output edge has not yet exist in Edgetable.   */
    for(el = graph->OutControl; el != NULL; el= el->Next) {
        str = EP(el)->Name;
        if ((l = CheckExistingEdgeToEdge(EP(el), &level)) != NULL && 
							level == indexlevel) {
	  printmsg("AddIOEdges():","output edge %s already exist\n",
				EP(el)->Name);
	  PrintSilage(EP(el)->Attributes);
          e = EP(l);
          ConnectOutEdge(n, e);
          e->Attributes = SetAttribute("lhs",IntNode,(char *)1, e->Attributes);
          ClearList(l);
	} else {
          Type = GetType(EP(el));
          e = NewEdge(str, EP(el)->Class, Type);
          InsertIndex(e, EP(el));
          ControlList = EdgeListAppend(e, ControlList);
	  if (HasAttribute(EP(el)->Attributes, "exit")) {
	    exitnum = (int)GetAttribute(EP(el)->Attributes, "exit");
            e->Attributes = SetAttribute("exit", IntNode, (char *)exitnum,
								e->Attributes);
	  } else
            e->Attributes = SetAttribute("lhs",IntNode,(char *)1,e->Attributes);
   	    ConnectOutEdge(n,e);
	}
    }
}

/**************************************************************************
  InsertIndex() -- From the Iternode indices, and output edge of subgraph,
  determine the index arguments of the output edges of the Iternode.  This 
  routine assumes the loop indices is the same order as all the array indices
  and is NOT very intelligent.
****************************************************************************/
InsertIndex(e, sube)
    EdgePointer e;
    EdgePointer sube;
{
    extern int indexlevel;
    char labelEval[STRSIZE], label[STRSIZE], *edgeindex;
    char *range;
    int i=0;

    sprintf(label,"index%d",i);
    while((edgeindex = (char *)GetAttribute(sube->Arguments, label)) != NULL) {
        sprintf(labelEval, "index%dEval", i);
        e->Arguments = SetAttribute (label, CharNode, Intern(edgeindex),
						e->Arguments);
	if (HasAttribute(sube->Arguments, labelEval)) {
	    range = (char *)GetAttribute(sube->Arguments, labelEval);
            if (i == indexlevel)  /* the iteration just completed */
                e->Arguments = SetAttribute (label, CharNode, range,
						e->Arguments);
	    else
                e->Arguments = SetAttribute (labelEval, CharNode, range,
								 e->Arguments);
	}
        sprintf(label,"index%d",++i);
    }	
}

/**************************************************************************
  InheritName() -- The name of arrays of subgraph inherits the name of arrays
  of the Iterator node.
****************************************************************************/
InheritName(NodeEdge, GraphEdge)
    EdgePointer NodeEdge;
    EdgePointer GraphEdge;
{
    char label[STRSIZE], *content;
    int i=0;
    
    sprintf(label,"index%d",i++);
    while((content = (char *)GetAttribute(NodeEdge->Arguments,label)) != NULL) {
        GraphEdge->Arguments = SetAttribute (label, CharNode, Intern(content),
						GraphEdge->Arguments);
        sprintf(label,"index%d",i++);
    }
}

/**************************************************************************
  IterBounds()  -- gives the lower and upper integral bounds of indices.
                   currently DO NOT handle compound expressions with id's.
****************************************************************************/
IterBounds(E, Result)
    Expression E;
    IterExp Result;
{
    ITEREXP L, R;

	switch(E->ExpKind) {
	    case IntegerLeafExp :
                Result->type = Inttype;
                Result->value = E->ExpDef.IntegerConstant;
                return;
            case IdentLeafExp :
                Result->type = IDtype;
                /* Can't seem to find need to return E->..SS.ManValue */
                return;
	    case PlusExp :
		IterBounds(E->ExpDef.SonsForBinOp.LeftExp, &L);
		IterBounds(E->ExpDef.SonsForBinOp.RightExp, &R);
                if (L.type == Inttype && R.type == Inttype) {
		    Result->type = Inttype;
                    Result->value = L.value + R.value;
                }
                else 
		    Result->type = OPtype;
                return;
	    case MinusExp :
		IterBounds(E->ExpDef.SonsForBinOp.LeftExp, &L);
		IterBounds(E->ExpDef.SonsForBinOp.RightExp, &R);
                if (L.type == Inttype && R.type == Inttype) {
		    Result->type = Inttype;
                    Result->value = L.value - R.value;
                }
                else 
		    Result->type = OPtype;
                return;
	    case MultExp :
		IterBounds(E->ExpDef.SonsForBinOp.LeftExp, &L);
		IterBounds(E->ExpDef.SonsForBinOp.RightExp, &R);
                if (L.type == Inttype && R.type == Inttype) {
		    Result->type = Inttype;
                    Result->value = L.value * R.value;
                }
                else 
		    Result->type = OPtype;
                return;
	    case DivExp :
		IterBounds(E->ExpDef.SonsForBinOp.LeftExp, &L);
		IterBounds(E->ExpDef.SonsForBinOp.RightExp, &R);
                if (L.type == Inttype && R.type == Inttype) {
		    Result->type = Inttype;
                    Result->value = L.value / R.value;
                }
                else 
		    Result->type = OPtype;
                return;
	    default :
		printmsg("IterBounds:", "Unknown Expression %d\n", E->ExpKind);
	}
}

/**************************************************************************
  ConstructExp()  --  Creates the necessary nodes and edges to implement
  the expression, insert new nodes and edges into the nodelist, edgelist,
  and Edgetable.  Return an edgelist representing the result of the expression
  just processed.  For compound expressions, the edges returned are  
  intermediate edges, for leaf expressions, they are edges with names.
  Certain expressions may return a list of edges such as function calls,
  tuple expressions, multiplexers.
****************************************************************************/
ListPointer
ConstructExp(E)
    Expression E;
{
    EdgePointer e, le;
    NodePointer n;
    ListPointer DoBin(), entry, ReturnEdge = NULL;
    ListPointer l;

    ListPointer FuncOutParams();
    char str[STRSIZE], *MakeRange();

    if (E == NULL)
 	return (ListPointer)NULL;
    switch(E->ExpKind) {
 	case IdentLeafExp:
	case FixedLeafExp:
	case IntegerLeafExp:
            if ((E->ExpKind == IdentLeafExp)&&(e = CheckEdgetable(E->
		 ExpDef.ThisSymbol->SymbolId.IdName)) != NULL) {
		;
            } else {
                switch (E->ExpKind) {
	            case IdentLeafExp :  /* if not in table, insert */
                        strcpy(str, E->ExpDef.ThisSymbol->SymbolId.IdName);
	                e = NewEdge(str, "data", EdgeType(E->ExpDef.ThisSymbol->
				SymType));
                        e->Attributes = SaveSilage(e->Attributes, E->ExpDef.
				ThisSymbol->SymbolId.IdPos);
			DeriveManifestRange(e);
		        break;
 	            case FixedLeafExp :
		    case IntegerLeafExp:
			e = NewEdge(NULL,"constant",EdgeType(E->ExpType));
                        e->Attributes = SaveSilage(e->Attributes, E->ExpPos);
			n = NewNode(CONST, "data");
			ConnectOutEdge(n,e);
			if (E->ExpKind == FixedLeafExp) {
			    if(E->ExpDef.SonsForFixedLeaf.FixedString == NULL){
				sprintf(str,"%f", 
		        	(double)E->ExpDef.SonsForFixedLeaf.M /
		        	(double)E->ExpDef.SonsForFixedLeaf.S);
			    }
			    else {
				if (E->ExpDef.SonsForFixedLeaf.M < 0) {
				    sprintf(str,"-%s",
				    E->ExpDef.SonsForFixedLeaf.FixedString);
				}
				else {
				    sprintf(str,"%s",
				    E->ExpDef.SonsForFixedLeaf.FixedString);
				}
			    }
			}
			else {
			    sprintf(str,"%d", E->ExpDef.IntegerConstant);
                            e->Arguments = SetAttribute("manifest", CharNode, 
				MakeRange(E->ExpDef.IntegerConstant, E->ExpDef.
				IntegerConstant), e->Arguments);
			}
                        n->Arguments = SetAttribute("value", CharNode, Intern(
				str), n->Arguments);
			NodeList = NodeListAppend(n, NodeList);
                        break;
	            }
	        EdgeList = EdgeListAppend(e, EdgeList);
	        }
            ReturnEdge = new_list(EdgeNode, e);
	    return(ReturnEdge);
        case CastExp :		/* Make a cast node, and an output edge with */
            n = NewNode(CAST, "data");	/* type E->ExpType.  */
            n->Attributes = SaveSilage(n->Attributes, E->ExpPos);
	    n->Arguments = SetAttribute("type", CharNode, Intern(EdgeType(E->
		 	   ExpType)), n->Arguments);
	    l = ConstructExp(E->ExpDef.CastedExp);
            e = NewEdge(NULL,"data",EdgeType(E->ExpType));
            e->Attributes = SaveSilage(e->Attributes, E->ExpPos);
	    ConnectOutEdge(n, e);
            ConnectInEdge(n, EP(l));
            DoUniManifest(CAST, EP(l), e);
	    NodeList = NodeListAppend(n, NodeList);
	    EdgeList = EdgeListAppend(e, EdgeList);
            ReturnEdge = new_list(EdgeNode, e);
	    return(ReturnEdge);
    	case NegateExp :
        case NotExp:
        case CosExp:
        case SinExp:
        case SqrtExp:
	    l = ConstructExp(E->ExpDef.SonsForBinOp.LeftExp);
	    switch(E->ExpKind) {
		case NegateExp : n = NewNode(NEG, "data"); break;
		case NotExp :    n = NewNode(NOT, "data"); break;
		case CosExp :    n = NewNode(COS, "data"); break;
		case SinExp :    n = NewNode(SIN, "data"); break;
		case SqrtExp :   n = NewNode(SQRT, "data"); break;
            }
            n->Attributes = SaveSilage(n->Attributes, E->ExpPos);
	    e = NewEdge(NULL,"data",EdgeType(E->ExpDef.SonsForBinOp.LeftExp->
					ExpType));
            e->Attributes = SaveSilage(e->Attributes, E->ExpPos);
	    ConnectOutEdge(n, e);
	    if (E->ExpKind == NegateExp)
                DoUniManifest(NEG, EP(l), e);
            ConnectInEdge(n, EP(l));
	    NodeList = NodeListAppend(n, NodeList);
	    EdgeList = EdgeListAppend(e, EdgeList);
            ReturnEdge = new_list(EdgeNode, e);
	    return(ReturnEdge);
        case FuncAppExp : {
            GraphPointer subgraph;
/* creates Funcnode  */
            if (st_lookup(InstanceTable, E->ExpDef.SonsForFuncApp.ThisFunction->
			FunctionId.IdName, &subgraph) == 0) {
                printmsg(NULL, "Function %s used but not defined\n",
		E->ExpDef.SonsForFuncApp.ThisFunction->FunctionId.IdName);
            }
	    n = NewNode(E->ExpDef.SonsForFuncApp.ThisFunction->FunctionId.
	                IdName, "Hierarchy");
            n->Attributes = SaveSilage(n->Attributes, E->ExpPos);
/* Process input parameters */
	    E = E->ExpDef.SonsForFuncApp.ActualParameters;
	    NodeList = NodeListAppend(n, NodeList);
	    l = ConstructExp(E->ExpDef.SonsForTuple.FirstExpression);
            le = EP(l);
   	    ConnectInEdge(n,le);
	    for(E=E->ExpDef.SonsForTuple.RestOfTuple;E!=NULL;
                                E=E->ExpDef.SonsForTuple.RestOfTuple) {
               l=ConstructExp(E->ExpDef.SonsForTuple.FirstExpression);
               le = EP(l);
   	       ConnectInEdge(n,le);
            }
/* Process output parameters by generating as many intermediate edges as 
   outputs.  This edge list is then returned.				 */
            ReturnEdge = FuncOutParams(subgraph, n);
	    return(ReturnEdge);
	    }
	case UserFuncExp : {
	    char *FuncId, type[STRSIZE];
            int i, val;
            Expression E1, E2;
	    FuncId = E->ExpDef.SonsForUserFunc.UserFuncId.IdName;
	    n = NewNode(FuncId, "data");
	    n->Attributes = SaveSilage(n->Attributes, E->ExpPos);
	    NodeList = NodeListAppend(n, NodeList);
	    for(E1 = E->ExpDef.SonsForUserFunc.ActualUserParameters,i=0; 
		E1 != NULL; E1 = E1->ExpDef.SonsForTuple.RestOfTuple,i++) {
		E2 = E1->ExpDef.SonsForTuple.FirstExpression;
		if (E2->ExpKind == IntegerLeafExp) {
		   val = E2->ExpDef.IntegerConstant;
		   if (i == 1)   /* position field */
		      n->Arguments = SetAttribute("bit", IntNode, (char *)val,
								n->Arguments);
		   else if (i == 2)
		      n->Arguments = SetAttribute("width", IntNode, (char *)val,
								n->Arguments);
		} else {
		   l=ConstructExp(E2);
                   le = EP(l);
                   ConnectInEdge(n,le);
		}
	    }
	    if (strcmp(FuncId, BIT) == 0)
		val = 1;
	    else if (strcmp(FuncId, BITSELECT) == 0)
		val = GetBitSelectWidth(E->ExpDef.SonsForUserFunc.
						ActualUserParameters);
	    else if (strcmp(FuncId, BITMERGE) == 0)
		val = GetBitMergeWidth(E->ExpDef.SonsForUserFunc.
						ActualUserParameters);
	    sprintf(type, "fix<%d,0>", val);
	    e = NewEdge(NULL,"data",type);
            e->Attributes = SaveSilage(e->Attributes, E->ExpPos);
	    ConnectOutEdge(n, e);
	    EdgeList = EdgeListAppend(e, EdgeList);
            ReturnEdge = new_list(EdgeNode, e);
	    return(ReturnEdge);
	    }
	case PlusExp : 
 	    ReturnEdge = DoBin(E, PLUS);
	    return(ReturnEdge);
	case MultExp :
 	    ReturnEdge = DoBin(E, MULT);
	    return(ReturnEdge);
	case MinusExp :
 	    ReturnEdge = DoBin(E, MINUS);
	    return(ReturnEdge);
	case DivExp :
 	    ReturnEdge = DoBin(E, DIV);
	    return(ReturnEdge);
	case MinExp :
 	    ReturnEdge = DoBin(E, _MIN);
	    return(ReturnEdge);
	case MaxExp :
 	    ReturnEdge = DoBin(E, _MAX);
	    return(ReturnEdge);
	case LeftShiftExp :
 	    ReturnEdge = DoBin(E, LSH);
	    return(ReturnEdge);
	case RightShiftExp :
 	    ReturnEdge = DoBin(E, RSH);
	    return(ReturnEdge);
	case GreaterExp :
 	    ReturnEdge = DoBin(E, GT);
	    return(ReturnEdge);
	case SmallerExp :
 	    ReturnEdge = DoBin(E, LT);
	    return(ReturnEdge);
	case GreaterEqualExp :
 	    ReturnEdge = DoBin(E, GTE);
	    return(ReturnEdge);
	case SmallerEqualExp :
 	    ReturnEdge = DoBin(E, LTE);
	    return(ReturnEdge);
	case EqualExp :
 	    ReturnEdge = DoBin(E, EQ);
	    return(ReturnEdge);
	case NotEqualExp :
 	    ReturnEdge = DoBin(E, NTE);
	    return(ReturnEdge);
	case OrExp :
 	    ReturnEdge = DoBin(E, OR);
	    return(ReturnEdge);
	case ExorExp :
 	    ReturnEdge = DoBin(E, EXOR);
	    return(ReturnEdge);
	case AndExp :
 	    ReturnEdge = DoBin(E, AND);
	    return(ReturnEdge);
  	case DelayExp :
  	case LpDelayExp : {
            EdgePointer BuildDelayLine();

	    e = BuildDelayLine(E);
            ReturnEdge = new_list(EdgeNode, e);
            }
	    return(ReturnEdge);
 	case InitExp :
 	case LpInitExp :
	    printmsg(NULL, "(Lp)InitExp on RHS\n");
	case IndexedExp : {
	    EdgePointer DoReadNode();
            char *IndexName();

	    if ((e = CheckEdgetable(IndexName(E)))==NULL)
		e = DoReadNode(E);
            ReturnEdge = new_list(EdgeNode, e);
	    }
	    return(ReturnEdge);
    	case TupleExp :
	    l = ConstructExp(E->ExpDef.SonsForTuple.FirstExpression);
            entry = new_list(EdgeNode, EP(l));
            ReturnEdge = ListAppend(entry, ReturnEdge);
	    for(E=E->ExpDef.SonsForTuple.RestOfTuple;E!=NULL;
				E=E->ExpDef.SonsForTuple.RestOfTuple) {
	       l=ConstructExp(E->ExpDef.SonsForTuple.FirstExpression);
               entry = new_list(EdgeNode, EP(l));
               ReturnEdge = ListAppend(entry, ReturnEdge);
	    }
	    return(ReturnEdge);
	case CondExp : {
	    ListPointer left, right = NULL;
	    /* construct the boolean test */
	    l = ConstructExp(E->ExpDef.SonsForCond.IfExp);
	    le = EP(l);
	    /* construct the THEN clause */
	    left = ConstructExp(E->ExpDef.SonsForCond.ThenExp);
	    /* construct the ELSE clause */
	    if (E->ExpDef.SonsForCond.ElseExp != NULL) {
		right = ConstructExp(E->ExpDef.SonsForCond.ElseExp);
	    }
	    for (l = left; l != NULL; l = l->Next) {
	        n = NewNode(MUX, "data");
                n->Attributes = SaveSilage(n->Attributes, E->ExpPos);
                ConnectInEdge(n, le);
                ConnectInEdge(n, EP(l));
		if (right != NULL) {
                    ConnectInEdge(n, EP(right));
		    right = right->Next;
		} else
                    ConnectInEdge(n, NULL);
	        e = NewEdge (NULL,"data",GetType(EP(l)));
                e->Attributes = SaveSilage(e->Attributes, E->ExpPos);
	        ConnectOutEdge(n, e);
	        NodeList = NodeListAppend(n, NodeList);
	        EdgeList = EdgeListAppend(e, EdgeList);
		entry = new_list(EdgeNode, e);
                ReturnEdge = ListAppend(entry, ReturnEdge);
	    }
	    }
	    return(ReturnEdge);
	case ReSampExp : {
	    Expression ReSampInput;
	    switch (E->ExpDef.SonsForReSamp.Type) {
	        case UpSampleNode : n = NewNode(UPSAMPLE,"data"); break;
                case DownSampleNode : n = NewNode(DOWNSAMPLE,"data"); break;
                case InterpolateNode : n = NewNode(INTERPOLATE, "data"); break;
                case DecimateNode : n = NewNode(DECIMATE, "data"); break;
                default : printmsg(NULL,"Unknown Sampling Exp %d\n",
			E->ExpDef.SonsForReSamp.Type); break;
            }
            n->Attributes = SaveSilage(n->Attributes, E->ExpPos);
	    NodeList = NodeListAppend(n, NodeList);
	    n->Arguments = SetAttribute("Scale", IntNode, E->ExpDef.
			SonsForReSamp.Scale, n->Arguments);
	    n->Arguments = SetAttribute("Phase", IntNode, E->ExpDef.
			SonsForReSamp.Phase, n->Arguments);
	    ReSampInput = E->ExpDef.SonsForReSamp.ReSampInputs->ExpDef.
			  SonsForTuple.FirstExpression;
	    if (ReSampInput == NULL)
	        printmsg(NULL, "No Sample Input\n");
	    l = ConstructExp(ReSampInput);
	    ConnectInEdge(n, EP(l));
	    e = NewEdge(NULL,"data",EdgeType(ReSampInput));
            e->Attributes = SaveSilage(e->Attributes, ReSampInput->ExpPos);
	    EdgeList = EdgeListAppend(e, EdgeList);
	    ConnectOutEdge(n, e);
	    ReturnEdge = new_list(EdgeNode, e);
	    }
	    return(ReturnEdge);
	case TimeMuxExp : {
	    ListPointer DoTimeMux();
	    ReturnEdge = DoTimeMux(E);
	    }
	    return(ReturnEdge);
	default :
 	    printmsg(NULL, "Unknown Expression %d\n", E->ExpKind);
    }
}

/**************************************************************************
  BuildDelayLine() -- A delay edge is found that was not initialize,
  building a delay line and initialize each edge to 0.
****************************************************************************/
EdgePointer BuildDelayLine(E)
Expression E;
{
    extern int indexlevel;
    ListPointer l;
    EdgePointer e, InEdge, DelayvalEdge, OutEdge, ReturnEdge;
    NodePointer Node;
    Expression InExp, DelayvalExp;
    char string[STRSIZE], val[STRSIZE], BaseName[STRSIZE];
    char *ptr2, *SPrintExp(), *IndexName();
    int delay, index = FALSE, vector = FALSE;
    int LpDelay = FALSE;

    if (E->ExpKind == LpDelayExp)
	LpDelay = TRUE;

    InExp = E->ExpDef.SonsForDelay.DelaySigExp;
    DelayvalExp = E->ExpDef.SonsForDelay.DelayValExp;
    if (InExp->ExpKind == IndexedExp) {
        strcpy(BaseName, IndexName(InExp));
        index = TRUE;
        if (IsVector(InExp)) vector = TRUE;
    } else {
        strcpy(BaseName, InExp->ExpDef.ThisSymbol->SymbolId.IdName);
    }
    val[0] = '\0';
    ptr2 = SPrintExp(DelayvalExp, val);
    *ptr2 = '\0';
    if (LpDelay == TRUE)
        sprintf(string, "%s#%s", BaseName, val);
    else
        sprintf(string, "%s@%s", BaseName, val);
    if ((e = CheckEdgetable(string)) != NULL) 
	return(e);
    if(DelayvalExp->ExpKind != IntegerLeafExp) {
	l = ConstructExp(DelayvalExp);
	DelayvalEdge = EP(l);
        if (!HasAttribute(DelayvalEdge->Arguments, "manifest")) {
            printmsg("BuildDelayLine:","Non-manifest index edge %s\n",
				DelayvalEdge->Name);
	    PrintSilage(DelayvalEdge->Attributes);
            DelayvalEdge->Arguments = SetAttribute("manifest", CharNode,
                                Intern("unknown"), DelayvalEdge->Arguments);
        }
	Node = (LpDelay == TRUE)? NewNode(LPDELAY, "data") :
				  NewNode(DELAY, "data");
        Node->Attributes = SaveSilage(Node->Attributes, E->ExpPos);
	if (index == TRUE) {
	    l = ConstructExp(InExp);
	    InEdge = EP(l);
	    LabelDelayControlEdge(InEdge);
        } else {
	    if (vector)
	        InEdge = NewEdge(BaseName, "control", EdgeType(InExp->ExpType));
	    else
	        InEdge = NewEdge(BaseName, "data", EdgeType(InExp->ExpType));
            InEdge->Attributes = SaveSilage(InEdge->Attributes, InExp->ExpPos);
            AddCycleEdge(InEdge);
        }
	if (vector) {
	    OutEdge = NewEdge(string, "control", EdgeType(InExp->ExpType));
            InsertIndex(OutEdge, InEdge);
            /* InheritName(InEdge, OutEdge);  */
        } else
	    OutEdge = NewEdge(string, "data", EdgeType(InExp->ExpType));
        OutEdge->Attributes = SaveSilage(OutEdge->Attributes, InExp->ExpPos);
	ConnectInEdge(Node, InEdge);
	ConnectInEdge(Node, DelayvalEdge);
	ConnectInEdge(Node, NULL);   /*  for constant init value */
	Node->Arguments = SetAttribute("init", CharNode, Intern("0"),
							Node->Arguments);
	ConnectOutEdge(Node, OutEdge);
	NodeList = NodeListAppend(Node, NodeList);
	if (vector)
	    ControlList = EdgeListAppend(OutEdge, ControlList);
	else
	    EdgeList = EdgeListAppend(OutEdge, EdgeList);
	return(OutEdge);
    }
    else {
	delay = DelayvalExp->ExpDef.IntegerConstant;
	if (LpDelay == TRUE)
	    sprintf(string, "%s#%d", BaseName, delay);
	else
	    sprintf(string, "%s@%d", BaseName, delay);
	if (vector)
	    OutEdge = NewEdge(string, "control", EdgeType(InExp->ExpType));
   	else
	    OutEdge = NewEdge(string, "data", EdgeType(InExp->ExpType));
        OutEdge->Attributes = SaveSilage(OutEdge->Attributes, InExp->ExpPos);
        ReturnEdge = OutEdge;
	while (delay > 0) {
	    if (vector)
	        ControlList = EdgeListAppend(OutEdge, ControlList);
            else
	        EdgeList = EdgeListAppend(OutEdge, EdgeList);
	    Node = (LpDelay == TRUE)? NewNode(LPDELAY, "data") :
				      NewNode(DELAY, "data");
            Node->Attributes = SaveSilage(Node->Attributes, E->ExpPos);
	    Node->Arguments = SetAttribute("init", CharNode, Intern("0"),
							Node->Arguments);
	    Node->Arguments = SetAttribute("delay", IntNode, (char *) 1,
							Node->Arguments);
	    NodeList = NodeListAppend(Node, NodeList);
	    ConnectOutEdge(Node, OutEdge);
	    delay--;
	    if (delay > 0) {
		if (LpDelay == TRUE)
	            sprintf(string, "%s#%d", BaseName, delay);
		else
	            sprintf(string, "%s@%d", BaseName, delay);
	    } else
	        sprintf(string, "%s", BaseName);
            if ((e = CheckEdgetable(string)) != NULL) {
	        ConnectInEdge(Node, e);
	        ConnectInEdge(Node, NULL);   /*  for constant delay value */
	        ConnectInEdge(Node, NULL);   /*  for constant init value */
		break;
	    }
            if (delay > 0) {
		if (vector)
	            InEdge = NewEdge(string,"control",EdgeType(InExp->ExpType));
	 	else
	            InEdge = NewEdge(string, "data", EdgeType(InExp->ExpType)); 
                InEdge->Attributes=SaveSilage(InEdge->Attributes,InExp->ExpPos);
	    } else {
		if (index == TRUE) {
	            l = ConstructExp(InExp);
	            InEdge = EP(l);
                    InsertIndex(OutEdge, InEdge);
		}
		else { 
	            InEdge = NewEdge(string, "data", EdgeType(InExp->ExpType)); 
                    AddCycleEdge(InEdge);
		}
                InEdge->Attributes=SaveSilage(InEdge->Attributes,InExp->ExpPos);
	    }
	    ConnectInEdge(Node, InEdge);
	    ConnectInEdge(Node, NULL);   /*  for constant delay value */
	    ConnectInEdge(Node, NULL);   /*  for constant init value */
            OutEdge = InEdge;
	}
	return(ReturnEdge);
    }
}

/**************************************************************************
  FuncOutParams() -- creates a list of intermediate edges to be returned as
  output parameters to a function call.  They are in the same order as
  the output parameters.
****************************************************************************/
ListPointer FuncOutParams(graph, n)
    GraphPointer graph;
    NodePointer n;
{
    FunctionDefinitionList FDL;
    SymbolTable param;
    EdgePointer e, le;
    ListPointer l, entry, list = NULL;
    char *type;

    for (FDL = SemGraph.OrderedFuncDefs; FDL != NULL; FDL = FDL->OrderedNext) {
	if (strcmp(graph->Name, FDL->FunctionId.IdName) == 0) 
	    break;
    }
    if (FDL == NULL)
	printmsg("FuncOutParams:","Function %s doesn't exist\n", graph->Name);
    for(param = FDL->OutputParameters; param != NULL; param = param->Next) {
        for(l = graph->OutEdges; l!= NULL; l = l->Next) {
            le = EP(l);
	    if (strcmp(param->SymbolId.IdName, le->Name) == 0) {
                type = GetType(le);
                e = NewEdge(NULL, "data", type);
	        EdgeList = EdgeListAppend(e, EdgeList);
        	ConnectOutEdge(n, e);
        	entry = new_list(EdgeNode, e);
        	list = ListAppend(entry, list);
		break;
	    }
	}
        for(l = graph->OutControl; l!= NULL; l = l->Next) {
            le = EP(l);
	    if (strcmp(param->SymbolId.IdName, le->Name) == 0) {
                type = GetType(le);
                e = NewEdge(NULL, "control", type);
                InsertIndex(e, le);
                /* InheritName(le, e);  */
	        ControlList = EdgeListAppend(e, ControlList);
        	ConnectOutEdge(n, e);
        	entry = new_list(EdgeNode, e);
        	list = ListAppend(entry, list);
		break;
	    }
	}
    }
    return(list);
}
        
/**************************************************************************
  DoBin() -- process a binary operation.
****************************************************************************/
ListPointer
DoBin(E, Op)
    Expression E;
    char *Op;
{
    NodePointer n;
    EdgePointer e;
    ListPointer r = NULL, l, ReturnEdge;
    char *ResultantType();

  	n = NewNode(Op, "data");
        n->Attributes = SaveSilage(n->Attributes, E->ExpPos);
        l = ConstructExp(E->ExpDef.SonsForBinOp.LeftExp);
	ConnectInEdge(n, EP(l));
        if ((E->ExpKind == LeftShiftExp || E->ExpKind == RightShiftExp) &&
	    E->ExpDef.SonsForBinOp.RightExp->ExpKind == IntegerLeafExp) {
	    n->Arguments = SetAttribute("shift", IntNode, (char *)E->
	    ExpDef.SonsForBinOp.RightExp->ExpDef.IntegerConstant, n->Arguments);
	    ConnectInEdge(n, NULL);
	} else {
    	    r = ConstructExp(E->ExpDef.SonsForBinOp.RightExp);
	    ConnectInEdge(n, EP(r));
        }
	e = NewEdge(NULL, "data", ResultantType(n, E));
	if (r != NULL)
	    DoBinManifest(Op, EP(l), EP(r), e);
	ConnectOutEdge(n, e);
        NodeList = NodeListAppend(n, NodeList);
        EdgeList = EdgeListAppend(e, EdgeList);
        ReturnEdge = new_list(EdgeNode, e);
        return(ReturnEdge);
}

/****************************************************************************
   DoReadNode() -- Process a read node, with input control edges, indices,
   and output data edge.
****************************************************************************/
EdgePointer
DoReadNode(E)
    Expression E;
{
    EdgePointer oute, e;
    NodePointer n;
    char *name, *TmpEdgeName(), *IndexName(), *IndexEdgesName();
    ListPointer el1, el, CheckOverlapVectorEdge(), CheckExactVectorEdge();
    int level;

    name = E->ExpDef.SonsForIndexed.ThisArray->SymbolId.IdName;

/* If E has a Null index, we might not have to generate a READ, we have to
   check this.  If E has AllNullIndex, we definitely do not generate the READ.
   To check if a READ is necessary, we need to perform an exact range check. */
    if (IsAllNullIndex(E)) {  /* must return edge only */
	if ((el = CheckOverlapVectorEdge(E, &level)) != NULL && 
							level == indexlevel) {
	    return(EP(el));
	} else {
	    e = NewEdge(name, "control", EdgeType(E->ExpDef.SonsForIndexed.
				ThisArray->SymType));
            e->Attributes = SaveSilage(e->Attributes, E->ExpDef.SonsForIndexed.
				ThisArray->SymbolId.IdPos);
	    ControlList = EdgeListAppend(e, ControlList);
	    if (el == NULL)
	        InstallConstIndex(E, e, EDGETYPE);
	    else
		InheritName(EP(el), e);
	    return(e);
	}
    }
    /* See if there is exact match */
    if (IsVector(E) && (el = CheckExactVectorEdge(E, &level)) != NULL) {  
	/* There is exact match */
	if (level == indexlevel) {
	    /* There is exact match at current level, returns edge */
            return(EP(el));
        } else {
	    /* There is exact match previous level, returns new edge */
	    e = NewEdge(name, "control", EdgeType(E->ExpDef.SonsForIndexed.
				ThisArray->SymType));
            e->Attributes = SaveSilage(e->Attributes, E->ExpDef.
			SonsForIndexed.ThisArray->SymbolId.IdPos);
	    ControlList = EdgeListAppend(e, ControlList);
	    InheritName(EP(el), e);
	    return(e);
	}
    }

/* make a new ArrayReadnode  */
    n = NewNode(READ, "data");
    n->Attributes = SaveSilage(n->Attributes, E->ExpPos);
    NodeList = NodeListAppend(n, NodeList);
    n->Arguments = SetAttribute("array_name", CharNode, Intern(name),
	     						     n->Arguments);
/* Now connect indices to ArrayReadnode  */
    ConnectIndexToNode(E, n);

/* connect input control edges if exist, else create new one.  The only way
   the input control edge has not yet exist is if we are in the subgraph
   of an iterator node.  Thus, the index value must be an expression  */
/* BUG(11): it could also be array[i]@1, so we are reading array[i] for the
   first time.  Here we generate the output control edge to a write node 
   feeding to this read node.  */
    if ((el=CheckExistingEdgeToNode(n,&level)) != NULL && level == indexlevel){
/* got back an edgelist, connect this list to node n  */
         for(el1 = el; el1 != NULL; el1 = el1->Next) {
	      ConnectInEdge(n, EP(el1));
         }
	 ClearList(el);
    }
    else {  /* Cycle edge, or else edge exists from lower indexlevel */
	e = NewEdge(name,"control", EdgeType(E->ExpDef.SonsForIndexed.
		    ThisArray->SymType));
        e->Attributes = SaveSilage(e->Attributes, E->ExpDef.SonsForIndexed.
				   ThisArray->SymbolId.IdPos);
	ControlList = EdgeListAppend(e, ControlList);
	ConnectInEdge(n,e);
	InstallIndexToEdge(n, e);
	if (el != NULL) { /* edge exists from lower indexlevel, copy range */
	    CopyIndexEval(EP(el), e);
	}
    }

/* make an output edge with name = array element, installed in edgetable  */
    if (IsVector(E)) {
        oute = NewEdge(name,"control",EdgeType(E->ExpDef.SonsForIndexed.
					ThisArray->SymType));
	InstallIndexToEdge(n, oute);
        ControlList = EdgeListAppend(oute, ControlList);
	ConnectOutEdge(n, oute);
    } else {
        if (n->InEdges == NULL) {
            name = IndexName(E);
        } else {
	    name = IndexEdgesName(n);
        } 
        oute = NewEdge(name,"data",EdgeType(E->ExpDef.SonsForIndexed.ThisArray->
						SymType));
        oute->Attributes = SetAttribute("arrayname", CharNode, Intern(name),
					oute->Attributes);
	oute->Name = TmpEdgeName();
        EdgeList = EdgeListAppend(oute, EdgeList);
        ConnectOutEdge(n,oute);
    }
    oute->Attributes = SaveSilage(oute->Attributes, E->ExpDef.SonsForIndexed.
				  ThisArray->SymbolId.IdPos);
    return(oute);
}

ListPointer
DoTimeMux(E)
Expression E;
{
    Expression TimeMuxInputs, FirstInput;
    NodePointer n;
    EdgePointer e;
    ListPointer p, el;
    ListPointer entry, ReturnEdge = NULL;
    int i;
    char *range, *MakeRange();

    TimeMuxInputs = E->ExpDef.SonsForTimeMux.TimeMuxInputs;
    FirstInput = TimeMuxInputs->ExpDef.SonsForTuple.FirstExpression;

    if (E->ExpDef.SonsForTimeMux.NoPhase == -1) {
    /* Time-Multiplex case */
       n = NewNode(TIMEMUX, "data");
       NodeList = NodeListAppend(n, NodeList);
       n->Attributes = SaveSilage(n->Attributes, E->ExpPos);
       e = NewEdge(NULL, "data", EdgeType(FirstInput));
       e->Attributes = SaveSilage(e->Attributes, FirstInput->ExpPos);
       EdgeList = EdgeListAppend(e, EdgeList);
       ConnectOutEdge(n, e);
       entry = new_list(EdgeNode, e);
       ReturnEdge = ListAppend(entry, ReturnEdge);
       if (E->ExpDef.SonsForTimeMux.NrArrayInputs > 1) {
       /* Time-Multiplex, arrays case.  Presently do NOT use NrArrayInputs */ 
       /* Must build array edge FirstInput[0..NrArrayInputs-1] and check if
	  it already exists.  Otherwise, create it.                        */
	  n->Arguments = SetAttribute("Scale", IntNode, E->ExpDef.
			SonsForTimeMux.NrArrayInputs, n->Arguments);
	  el = ConstructExp(FirstInput);
	  for (p = el; p != NULL; p = p->Next) {
              ConnectInEdge(n, EP(p));
          }
          ClearList(el);
       } else {
       /* Time-Multiplex, tuple case */
          i=0;
          do {
	     el = ConstructExp(TimeMuxInputs->ExpDef.SonsForTuple.
							FirstExpression);
	     ConnectInEdge(n, EP(el));
             TimeMuxInputs = TimeMuxInputs->ExpDef.SonsForTuple.RestOfTuple;
	     i++;
          } while (TimeMuxInputs != NULL);
	  n->Arguments = SetAttribute("Scale", IntNode, i, n->Arguments);
       }
    } else {
    /* Time-Demultiplex case, If NrArrayOutputs > 1, we have array outputs, if
       NrArrayOutputs < 0, say -3, we have 3 data outputs */
       n = NewNode(TIMEDEMUX, "data");
       NodeList = NodeListAppend(n, NodeList);
       n->Attributes = SaveSilage(n->Attributes, E->ExpPos);
       if (E->ExpDef.SonsForTimeMux.NoPhase == 1)
           n->Arguments = SetAttribute("NoPhase", IntNode, (char *)1,
			                                     n->Arguments);
       el = ConstructExp(FirstInput);
       ConnectInEdge(n, EP(el));
       if (E->ExpDef.SonsForTimeMux.NrArrayOutputs > 1) {
       /* Time-Demultiplex, arrays case.  Presently do NOT use NrArrayOutputs.
          We probably need to augment SonsForTimeMux struct to include Scale
          so we would know how many temp output edges to make.  For (A[],2),
          what type of edge do we generate for A[]? we can only generate dummy
          data edges.  */
	   n->Arguments = SetAttribute("Scale", IntNode, E->ExpDef.
			SonsForTimeMux.NrArrayOutputs, n->Arguments);
           e = NewEdge(NULL, "control", EdgeType(FirstInput));
	   range = MakeRange(0,E->ExpDef.SonsForTimeMux.NrArrayOutputs - 1);
           e->Arguments = SetAttribute("index0", CharNode, range, e->Arguments);
           e->Attributes = SaveSilage(e->Attributes, FirstInput->ExpPos);
           ControlList = EdgeListAppend(e, ControlList);
           ConnectOutEdge(n, e);
           entry = new_list(EdgeNode, e);
           ReturnEdge = ListAppend(entry, ReturnEdge);
       } else if (E->ExpDef.SonsForTimeMux.NrArrayOutputs < 0) {
	   n->Arguments = SetAttribute("Scale", IntNode, -(E->ExpDef.
			SonsForTimeMux.NrArrayOutputs), n->Arguments);
	   for(i=0;i<-(E->ExpDef.SonsForTimeMux.NrArrayOutputs);i++) {
              e = NewEdge(NULL, "data", EdgeType(FirstInput));
              e->Attributes = SaveSilage(e->Attributes, FirstInput->ExpPos);
              e->Attributes = SetAttribute("offset", IntNode, (pointer)i,
				e->Attributes);
              EdgeList = EdgeListAppend(e, EdgeList);
              ConnectOutEdge(n, e);
              entry = new_list(EdgeNode, e);
              ReturnEdge = ListAppend(entry, ReturnEdge);
           }
       } else
           printmsg("Time-Demultiplex:","Flowgraph generation not-implemented\n");
    }
    return(ReturnEdge);
}

myMessage(Message)
char *Message;
{
}
