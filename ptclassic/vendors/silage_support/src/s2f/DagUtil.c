/* $Id$ */

#include <ctype.h>
#include "sfg.h"
#include "dag.h"
#include "stringswitch.h"
#include "st.h"

char *Intern();
pointer GetAttribute();
GraphPointer new_graph();
ListPointer new_list(), ListAppend();
ListPointer SetAttribute();
EdgePointer new_edge(), EdgeListAppend();
NodePointer new_node(), NodeListAppend();

/**************************************************************************
  NewEdge() -- creates a new edge.
****************************************************************************/
EdgePointer NewEdge(name, class, type)
    char *name, *class, *type;
{
    EdgePointer e;
    char *TmpEdgeName();

    e = new_edge();
    e->Class = Intern(class);
    if (name == NULL) {
        e->Name = TmpEdgeName();
    }
    else {
        e->Name = Intern(name);
        AddToEdgetable(e);
    }
    SetType(e, Intern(type));
    return(e);
}

/**************************************************************************
   NewNode() -- creates a new node.
****************************************************************************/
NodePointer
NewNode(func, class)
    char *func, *class;
{
    pointer CheckFromHash();
    NodePointer n;
    GraphPointer Subgraph, FlowGraph, ParseOctSubgraph(), GraphListAppend();
    extern GraphPointer Graphlist;
    extern st_table *InstanceTable;
    char *Path, *GetOctPath(), *TmpNodeName();

    n = new_node();
    n->Name = TmpNodeName();
    n->Class = Intern(class);
    /* Why OCT ? */
    if (st_lookup(InstanceTable, func, &Subgraph) == 0) {
        Path = GetOctPath("flowgraph.oct", func, FLOWVIEW, "contents");
        FlowGraph = ParseOctSubgraph(Path);
	Graphlist = GraphListAppend(FlowGraph, Graphlist);
        n->Master = FlowGraph;
    }
    else
	n->Master = Subgraph;
    return(n);
}

GraphPointer
NewGraph(func)
char *func;
{
    char *Path, *GetOctPath();
    extern st_table *InstanceTable;
    GraphPointer FlowGraph, ParseOctSubgraph();

    if (st_lookup(InstanceTable, func, &FlowGraph) == 0) {
        Path = GetOctPath("flowgraph.oct", func, FLOWVIEW, "contents");
        FlowGraph = ParseOctSubgraph(Path);
    }
    return(FlowGraph);
}

/**************************************************************************
   ConnectOutEdge() -- Connect output edge oute out of node n.
   Same as ConnectNodeToEdge(), but handles Null Edge.
****************************************************************************/
 ConnectOutEdge(n, outedge)
     NodePointer n;
     EdgePointer outedge;
 {
       register ListPointer Entry;

       if (outedge == NULL)
             Entry = new_list(NullNode, (pointer) outedge);
       else
             Entry = new_list(EdgeNode, (pointer) outedge);
       if (outedge != NULL && IsControlEdge(outedge))
           n->OutControl = ListAppend(Entry, n->OutControl);
       else
           n->OutEdges = ListAppend(Entry, n->OutEdges);

         Entry = new_list(NodeNode, (pointer) n);
       if (outedge != NULL)
           outedge->InNodes = ListAppend(Entry, outedge->InNodes);
 }

 /**************************************************************************
   ConnectInEdge() -- Connect input edge ine into node n.
   Same as ConnectEdgeToNode(), but handles Null Edge.
 ****************************************************************************/
 ConnectInEdge(n, inedge)
     NodePointer n;
     EdgePointer inedge;
 {
       register ListPointer Entry;

       if (inedge == NULL)
             Entry = new_list(NullNode, (pointer) inedge);
       else
             Entry = new_list(EdgeNode, (pointer) inedge);
       if (inedge != NULL && IsControlEdge(inedge))
           n->InControl = ListAppend(Entry, n->InControl);
       else
           n->InEdges = ListAppend(Entry, n->InEdges);

       Entry = new_list(NodeNode, (pointer) n);
       if (inedge != NULL)
           inedge->OutNodes = ListAppend(Entry, inedge->OutNodes);
 }

/**************************************************************************
  EdgeType() -- returns a character string of the type of the edge.
****************************************************************************/
char *
EdgeType(type)
    SymbolType type;
{
    static char name[15];

    switch (type.SymTpKind) {
	case UndefType :
	    sprintf(name,"%s","int");
	    break;
	case FixedType :
	    sprintf(name,"fix<%d,%d>",type.SymTp.FixedTp.w,
						type.SymTp.FixedTp.d);
	    break;
	case IntType :
	    sprintf(name, "%s", "int");
	    break;
	case BoolType :
	    sprintf(name, "%s", "bool");
	    break;
    }
    return(name);
}

/****************************************************************************
   ExtractType() --  Convert from a string into the SymbolType structure.
****************************************************************************/
ExtractType(type, Type)
    char *type;
    SymbolType *Type;
{
    char c;

    c = *type;
    if (c == 'i' || c == 'I') {
        Type->SymTpKind = IntType;
    }
    else if (c == 'b' || c == 'B') {
        Type->SymTpKind = BoolType;
    } 
    else if (c == 'f') {
       Type->SymTpKind = FixedType;
       sscanf(type, "fix<%d,%d>",&Type->SymTp.FixedTp.w,&Type->SymTp.FixedTp.d);
    } 
    else if (c == 'F') {
       Type->SymTpKind = FixedType;
       sscanf(type, "FIX<%d,%d>",&Type->SymTp.FixedTp.w,&Type->SymTp.FixedTp.d);
    }
    else {
        printmsg("ExtractType:","unknown type %s\n", type);
    }
}

/**************************************************************************
   ResultantType() -- determines the data type of the output intermediate
   edge given the two input edges and the operation that is performed.
   This routine is NOT intelligent.
****************************************************************************/
char *
ResultantType(n, E)
    NodePointer n;
    Expression E;
{
    char *EdgeType();

    EdgePointer ledge, redge;
    char *ltype, *rtype, *GetType();
    SymbolType Ltype, Rtype, Restype;

    ledge = EP(n->InEdges);
    ltype = GetType(ledge);
    ExtractType(ltype, &Ltype);
    if (n->InEdges->Next == NULL || n->InEdges->Next->Type == NullNode) {
	return(ltype);
    } else {
        redge = EP(n->InEdges->Next);
        rtype = GetType(redge);
        ExtractType(rtype, &Rtype);
    }
    switch(E->ExpKind) {
        case PlusExp:
	case MinusExp:
	case DelayExp:
	case LpDelayExp:
	case InitExp:
	case LpInitExp:
	case MultExp:
	case DivExp:
	case MinExp:
	case MaxExp:
	    if (Ltype.SymTpKind==FixedType && Rtype.SymTpKind==FixedType){
		Restype.SymTpKind = FixedType;
		Restype.SymTp.FixedTp.w = Ltype.SymTp.FixedTp.w;
		Restype.SymTp.FixedTp.d = Ltype.SymTp.FixedTp.d;
		if (E->ExpKind == MultExp) {
		    Restype.SymTp.FixedTp.w = Ltype.SymTp.FixedTp.w +
			     Rtype.SymTp.FixedTp.w;
		    Restype.SymTp.FixedTp.d = Ltype.SymTp.FixedTp.d +
			     Rtype.SymTp.FixedTp.d;
		}
		return(EdgeType(Restype));
	    } else if (Ltype.SymTpKind == IntType &&
		Rtype.SymTpKind == FixedType) {
		    return(rtype);
	    } else if (Ltype.SymTpKind == FixedType &&
		Rtype.SymTpKind == IntType) {
		    return(ltype);
	    } else if (Ltype.SymTpKind == IntType &&
			Rtype.SymTpKind == IntType) {
		    return(ltype);
	    } else {
		 printmsg(NULL, "Resultanttype mismatch %d %d\n",
			Ltype.SymTpKind, Rtype.SymTpKind);
	    }
	case GreaterExp :
	case SmallerExp :
	case GreaterEqualExp :
	case SmallerEqualExp :
	case EqualExp :
 	case NotEqualExp :
		  return("bool");
 	case LeftShiftExp:
 	case RightShiftExp:
	case OrExp:
	case AndExp:
	case ExorExp:
		  return(ltype);
	    default:
		printmsg("ResultantType", "Unknown Expression %d\n", E->ExpKind);
    }
}

/**************************************************************************
  SPrintExp() -- used for printing arithmetic expressions occuring as indices
  in an array element.  This routine is NOT intelligent.
****************************************************************************/
char *SPrintExp(E,p1)
    Expression E;
    char *p1;
{
    char *p2, *SPrintExp();
    if (E==NULL)
	return(p1);
    switch (E->ExpKind) {
	case IdentLeafExp :
	    p2 = E->ExpDef.ThisSymbol->SymbolId.IdName;
	    for(; *p2 != NULL;)
		*p1++ = *p2++;
	    return(p1);
	case IntegerLeafExp :
	    p1 = p1 + itoa(E->ExpDef.IntegerConstant,p1);
	    return(p1);
	case PlusExp :
	    *p1++ = '(';
	    p1 = SPrintExp(E->ExpDef.SonsForBinOp.LeftExp, p1);
	    *p1++ = ' '; *p1++ = '+'; *p1++ = ' ';
	    p1 = SPrintExp(E->ExpDef.SonsForBinOp.RightExp, p1);
	    *p1++ = ')';
	    return(p1);
	case MinusExp :
	    *p1++ = '(';
	    p1 = SPrintExp(E->ExpDef.SonsForBinOp.LeftExp, p1);
	    *p1++ = ' '; *p1++ = '-'; *p1++ = ' ';
	    p1 = SPrintExp(E->ExpDef.SonsForBinOp.RightExp, p1);
	    *p1++ = ')';
	    return(p1);
	case MultExp :
	    *p1++ = '(';
	    p1 = SPrintExp(E->ExpDef.SonsForBinOp.LeftExp, p1);
	    *p1++ = ' '; *p1++ = '*'; *p1++ = ' ';
	    p1 = SPrintExp(E->ExpDef.SonsForBinOp.RightExp, p1);
	    *p1++ = ')';
	    return(p1);
	default :
	    printmsg(NULL, "Unknown array subscript expression %d\n", E->ExpKind);
    }
}

/**************************************************************************
  IndexName() -- Given an indexedExp, returns the full name of the expression,
  i.e. with brackets.
****************************************************************************/
char *IndexName(E)
    Expression E;
{

	Expression E1, E2;
	char *SPrintExp(), *p1, *p2;
        ITEREXP Result;
        static char name[STRSIZE];

	p1 = name;
	p2 = E->ExpDef.SonsForIndexed.ThisArray->SymbolId.IdName;
	for(; *p2 != NULL;)
	    *p1++ = *p2++;
	if (E->ExpDef.SonsForIndexed.ThisArray->SingleOrArray.AS.InPlace
								== FALSE) {
	    for (E1 = E->ExpDef.SonsForIndexed.IndexTupleExp;
		E1 != NULL;
		E1 = E1->ExpDef.SonsForTuple.RestOfTuple) {
		E2 = E1->ExpDef.SonsForTuple.FirstExpression;
		*p1++ = '[';
		if (E2 != NULL) {
		    IterBounds(E2, &Result);
		    if (Result.type == Inttype) {
			sprintf(p1, "%d", Result.value);
			p1 = p1 + strlen(p1);
		    } else {
		        p1 = SPrintExp(E2, p1);
		    }
                }
		*p1++ = ']';
	    }
	}
	*p1 = '\0'; 
        return(name);
}

/**************************************************************************
  SaveSilage() -- Stores Silage src code position onto node or edge.
**************************************************************************/
ListPointer
SaveSilage(Attribute, Pos)
ListPointer Attribute;
Position Pos;
{
    ListPointer Attr, Entry;

    if (Pos.FileName == NULL)
	return(Attribute);
    if (HasAttribute(Attribute, "Silage")) {
	Attr = (ListPointer)GetAttribute(Attribute, "Silage");
        if (Attr->Type != CharNode)
	    printmsg("SaveSilage", "Incorrect existing Silage Attribute\n");
	Attr->Entry = (pointer)Intern(StripQuotes(Pos.FileName));
	Attr->Next->Entry = (pointer)Pos.LineNum;
	Attr->Next->Next->Entry = (pointer)Pos.CharNum;
        return(Attribute);
    }
    Attr = (ListPointer)NULL;
    Entry = new_list(CharNode, (pointer)Intern(StripQuotes(Pos.FileName)));
    Attr = ListAppend(Entry, Attr);
    Entry = new_list(IntNode, (pointer)Pos.LineNum);
    Attr = ListAppend(Entry, Attr);
    Entry = new_list(IntNode, (pointer)Pos.CharNum);
    Attr = ListAppend(Entry, Attr);
    return(SetAttribute("Silage", ListNode, (pointer)Attr, Attribute));
}

/**************************************************************************
  The following are hash routines used in hashing edge names in Edgetable.
**************************************************************************/


/**************************************************************************
  InitializeHash() -- Initializes hashtable
**************************************************************************/
InitializeHash(table)
    EntryPointer table[];
{
    register EntryPointer *p;

    for(p = table; p != table + TBLSIZE; p++) {
        *p = (EntryPointer) NULL;
    }
}

/**************************************************************************
*  hashfunc(s) --  Given a string s, this function returns the value of a *
*                  hash function used in Aho & Ullman, p. 436.            *
**************************************************************************/
hashfunc(s)
	char *s;
{
	char *p;
	unsigned h=0,g;
	for (p=s; *p != EOS; p++) {
		h = (h<<4) + (*p);
		if (g = h&0xf0000000) {
			h = h^(g>>24);
			h = h^g;
		}
	}
	return (h % TBLSIZE);
}

/**************************************************************************
  CheckFromHash() -- returns the entry in table whose name matches 
  string s.
****************************************************************************/
pointer CheckFromHash(s,table)
    char *s;
    EntryPointer table[];
{
    EntryPointer ptr;

    for (ptr = table[hashfunc(s)]; ptr != NULL; ptr = ptr->Next) {
	if (strcmp(s, ptr->Name) == 0) return(ptr->Entry);
    }
    return((pointer)NULL);
}

/**************************************************************************
   AddToHash() -- install the string and corresponding entry into table.
****************************************************************************/
AddToHash(s,entry,table)
    char *s;
    pointer entry;
    EntryPointer table[];
{
    EntryPointer ptr, new_entry();
    int index;

    ptr = new_entry();
    ptr->Name = Intern(s);
    ptr->Entry = entry;
    index = hashfunc(s);
    ptr->Next = table[index];
    table[index] = ptr;
}

/**************************************************************************
   Memory Allocation for edge entries.
****************************************************************************/
#define CHUNK 100

static EntryPointer freeentry = NULL;

get_free_entry()
/* allocates a set of free space equal to CHUNK structures GRAPH */
{
    register int i;

    freeentry = (EntryPointer) malloc (CHUNK * sizeof (ENTRY));
    if (freeentry == NULL) {
        printmsg("malloc","can't malloc for ENTRY\n");
        exit (-1);
    }
    for (i = 0; i < CHUNK-1; i++)
        (freeentry + i)->Next = freeentry + i + 1;
    (freeentry + i)->Next = NULL;
}

EntryPointer
new_entry()
{
    register EntryPointer entry;

    if (!freeentry)
        get_free_entry();
    entry = freeentry;
    freeentry = freeentry->Next;
    entry->Name = NULL;
    entry->Entry = NULL;
    entry->Next = NULL;
    return(entry);
}

free_entry(entry)
register EntryPointer entry;
{
    if (!entry)
        return;
    entry->Next = freeentry;
    freeentry = entry;
}

extern NodePointer NodeList;
extern EdgePointer EdgeList, ControlList;
static st_table *ReferTable;

ClearTokens(Nodelist, Edgelist, Controllist)
NodePointer Nodelist;
EdgePointer Edgelist;
EdgePointer Controllist;
{
    EdgePointer Edge;
    NodePointer Node;
    GraphPointer Graph;

    for (Node = Nodelist; Node != NULL; Node = Node->Next) {
        if (IsHierarchy(Node)) {
	    Graph = (GraphPointer)Node->Master;
            if (st_lookup(ReferTable, Graph->Name, NULL) == NULL) {
		Graph->Token = CLEAR;
                st_insert(ReferTable, Graph->Name, 0);
	        ClearTokens(Graph->NodeList,Graph->EdgeList,Graph->ControlList);
	    }
	} else
	    Node->Token = CLEAR;
    }
    for (Edge = Edgelist; Edge != NULL; Edge = Edge->Next)
	Edge->Token = CLEAR;
    for (Edge = Controllist; Edge != NULL; Edge = Edge->Next)
	Edge->Token = CLEAR;
}

DependencyCheck(IndexEdge)
EdgePointer IndexEdge;
{
    ListPointer Ptr1, Ptr2;
    ListPointer WriteList = NULL;
    ListPointer ReadList = NULL;
    NodePointer Node, WriteNode, ReadNode;
    char *WriteArray, *ReadArray;
    int DependencyType = PARALLEL;
    
    /* Check for Loop delays */
    for (Node = NodeList; Node != NULL; Node = Node->Next)
	if (IsLpDelayNode(Node))
	    return(SERIAL);

    /* Clear Tokens */
    ReferTable = st_init_table(strcmp, st_strhash);
    ClearTokens(NodeList, EdgeList, ControlList);
    st_free_table(ReferTable);

    /* run over outnodes */
    ProcessOutnodes(IndexEdge, &WriteList, &ReadList);
    
    for (Ptr1 = WriteList; Ptr1 != NULL; Ptr1 = Ptr1->Next) {
        WriteNode = NP(Ptr1);
        WriteArray = (char *)GetAttribute(WriteNode->Arguments, "array_name");
        for (Ptr2 = ReadList; Ptr2 != NULL; Ptr2 = Ptr2->Next) {
	    ReadNode = NP(Ptr2);
            ReadArray = (char *)GetAttribute(ReadNode->Arguments, "array_name");
	    if ((strcmp(WriteArray, ReadArray) == 0) && 
	       (ReadNode->Token == SET || WriteNode->Token == SET)) {
		DependencyType = SERIAL;
		break;
	    }
   	}
    }
    ClearList(WriteList);
    ClearList(ReadList);
    return(DependencyType);
}

ProcessOutnodes(InEdge, WriteList, ReadList)
EdgePointer InEdge;
ListPointer *WriteList;
ListPointer *ReadList;
{
    ListPointer Ptr1;
    NodePointer Node;

    for (Ptr1 = InEdge->OutNodes; Ptr1 != NULL; Ptr1 = Ptr1->Next) {
	if (Ptr1->Type == NodeNode) {
	    Node = NP(Ptr1);
 	    if (Node->Token == SET) continue;
	    AddToDepenList(InEdge, Node, WriteList, ReadList);
        }
    }
}

/* The SET flag signifies that the index edge has gone through some sort
 * of computation (i-1) or (i+1), and so must have SERIAL dependency.
 * Not a very good test.
 */
AddToDepenList(IndexEdge, Node, WriteList, ReadList)
EdgePointer IndexEdge;
NodePointer Node;
ListPointer *WriteList;
ListPointer *ReadList;
{
    ListPointer Ptr1;
    EdgePointer OutEdge, InEdge;
    GraphPointer Subgraph;

    if (IsIteration(Node->Master)) {
	Subgraph = Node->Master;
	for (Ptr1 = Subgraph->InEdges; Ptr1 != NULL; Ptr1 = Ptr1->Next) {
	    if (Ptr1->Type == EdgeNode) {
		InEdge = EP(Ptr1);
	        if (strcmp(IndexEdge, InEdge) == 0)
		    break;
	    }
	}
	ProcessOutnodes(InEdge,WriteList, ReadList);
    } else {
 	STRINGSWITCH(Node->Master->Name)
	    FIRSTCASE(READ) { *ReadList = ListAppend(new_list(NodeNode, 
			      (pointer)Node), *ReadList);
			      if (IndexEdge->Token == SET)
				  Node->Token = SET;
			    }
	    CASE(WRITE) { *WriteList = ListAppend(new_list(NodeNode, (pointer)
			      Node), *WriteList); 
			  if (IndexEdge->Token == SET)
			      Node->Token = SET;
			}
	    CASE(DELAY) {;  /* index going into delay nodes, do nothing */
			}
	    CASE(LPDELAY) {;
			}
            else {  /* should be arithmethic operation on index */
		for (Ptr1 = Node->OutEdges;Ptr1 != NULL;Ptr1 = Ptr1->Next) {
		    if (Ptr1->Type == EdgeNode) {
		        OutEdge = EP(Ptr1);
			OutEdge->Token = SET;
			Node->Token = SET;
			ProcessOutnodes(OutEdge, WriteList, ReadList);
		    }
		}
	    }
	ENDSWITCH
    }
}

