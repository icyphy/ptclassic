/* $Id$ */

#include "flowgraph.h"
#include "st.h"

typedef enum {false, true} bool;
#define STRSIZE 50
#define LOWER 0
#define UPPER 1

#define GP(a) ((GraphPointer) a->Entry)
#define NP(a) ((NodePointer) a->Entry)
#define EP(a) ((EdgePointer) a->Entry)

ListPointer new_list(), ListAppend(), SetAttribute();
char *Intern();

/**************************************************************************
  isnumber() -- check if the string is a decimal number.
****************************************************************************/
isnumber(entry)
    pointer entry;
{
    int n, m, k,l;
    float y;

/* check to see if entry is a pointer to a number first  */
    if (entry == NULL) return(FALSE);

    n = sscanf(entry,"%f", &y);
    m = sscanf(entry,"%d..%d", &k, &l);
    if (n == 1 && m != 2)
        return(TRUE);
    else
        return(FALSE);
}

bool IsHierarchy(node)
NodePointer node;
{
    if(node == NULL) return(true);
    return ((strcmp(node->Master->Class, LEAF_NODE) &&
             strcmp(node->Master->Class, "MACRO")) ? true : false);
}

bool IsFuncNode(node)
NodePointer node;
{
    char *Function, *GetFunction();

    if ((Function = GetFunction(node)) == NULL)
        return (false);
    return (!strcmp(Function, "func") ? true : false);
}

int
IsSuccessorNode(Node1, Node2)
NodePointer Node1, Node2;
{
    ListPointer Ptr, Ptr2;
    EdgePointer Edge;
    NodePointer OutNode;

    for (Ptr = Node2->OutEdges; Ptr != NULL; Ptr = Ptr->Next) {
	if (Ptr->Type != EdgeNode) continue;
        Edge = EP(Ptr);
        for (Ptr2 = Edge->OutNodes; Ptr2 != NULL; Ptr2 = Ptr2->Next) {
	    if (Ptr2->Type != NodeNode) continue;
	    OutNode = NP(Ptr2);
	    if (OutNode == Node1) return(TRUE);
	}
    }
    for (Ptr = Node2->OutControl; Ptr != NULL; Ptr = Ptr->Next) {
	if (Ptr->Type != EdgeNode) continue;
        Edge = EP(Ptr);
        for (Ptr2 = Edge->OutNodes; Ptr2 != NULL; Ptr2 = Ptr2->Next) {
	    if (Ptr2->Type != NodeNode) continue;
	    OutNode = NP(Ptr2);
	    if (OutNode == Node1) return(TRUE);
	}
    }
    return(FALSE);
}

bool IsRelationNode (node)
NodePointer node;
{
    char *Function, *GetFunction();

    if ((Function = GetFunction(node)) == NULL)
	return (false);
    return ((!strcmp(Function, "==") ||
	     !strcmp(Function, "!=") ||
	     !strcmp(Function, ">" ) ||
	     !strcmp(Function, ">=") ||
	     !strcmp(Function, "<" ) ||
	     !strcmp(Function, "<=")) ? true : false);
}

bool IsArray(edge)
EdgePointer edge;
{
    if (edge == NULL) return(false);
    return((HasAttribute(edge->Arguments, "index0")) ? true : false);
}

int
IsArrayEdge(Edge)
EdgePointer Edge;
{
    if (Edge == NULL) return(FALSE);
    return((HasAttribute(Edge->Arguments, "index0")) ? TRUE : FALSE);
}

/* (iv) added */
int
HasIndexOnNode(Node)
NodePointer Node;
{
    if (Node == NULL) return(FALSE);
    return((HasAttribute(Node->Arguments, "index0")) ? TRUE : FALSE);
}

bool IsFixedType(edge)
EdgePointer edge;
{
    char *type;

    if (edge == NULL) return(true);
    if ((type = (char *)GetAttribute(edge->Arguments, "type")) == NULL)
	printmsg(NULL, "edge %s has no type argument\n", edge->Name);
    return((strcmp(type, "int") && strcmp(type, "bool")) ? true : false);
}

bool IsIntType(edge)
EdgePointer edge;
{
    char *type;

    if (edge == NULL) return(true);
    if ((type = (char *)GetAttribute(edge->Arguments, "type")) == NULL)
	printmsg(NULL, "edge %s has no type argument\n", edge->Name);
    return((strcmp(type, "int")) ? false : true);
}

bool IsBoolType(edge)
EdgePointer edge;
{
    char *type;

    if (edge == NULL) return(true);
    if ((type = (char *)GetAttribute(edge->Arguments, "type")) == NULL)
	printmsg(NULL, "edge %s has no type argument\n", edge->Name);
    return((strcmp(type, "bool")) ? false : true);
}

/* only used after RemoveAllConstNodes  */
bool IsConstant(edge)
EdgePointer edge;
{
    char *value;

    if (HasAttribute(edge->Attributes, "value")) {
        value = (char *)GetAttribute(edge->Attributes, "value");
        if (isnumber(value))
            return(true);
    }
    return(false);
}

bool IsIndex(edge)
EdgePointer edge;
{
    char *value;

    if (HasAttribute(edge->Attributes, "value")) {
        value = (char *)GetAttribute(edge->Attributes, "value");
        if (!isnumber(value))
            return(true);
    }
    return(false);
}

bool IsReadNode(node)
NodePointer node;
{
    char *Function, *GetFunction();

    if ((Function = GetFunction(node)) == NULL)
        return (false);
    return (!strcmp(Function, "read") ? true : false);
}

bool IsWriteNode(node)
NodePointer node;
{
    char *Function, *GetFunction();

    if ((Function = GetFunction(node)) == NULL)
        return (false);
    return (!strcmp(Function, "write") ? true : false);
}

bool IsDelayInput(edge)
EdgePointer edge;
{
    ListPointer ptr;
    NodePointer node;

    for(ptr = edge->OutNodes; ptr != NULL; ptr = ptr->Next) {
        if (ptr->Type == NodeNode) {
            node = NP(ptr);
            if (!strcmp(GetFunction(node), "@"))
		return(true);
	}
    }
    return(false);
}

bool IsLpDelayNode(node)
NodePointer node;
{
    char *Function, *GetFunction();

    if ((Function = GetFunction(node)) == NULL)
        return (false);
    return (!strcmp(Function, "#") ? true : false);
}

bool IsSampDelayNode(node)
NodePointer node;
{
    char *Function, *GetFunction();

    if ((Function = GetFunction(node)) == NULL)
        return (false);
    return (!strcmp(Function, "@") ? true : false);
}

bool IsIteration(graph)
GraphPointer graph;
{
    char *model;

    model = (char *)GetAttribute(graph->Model, "model_name");
    return(!strcmp(model, "iteration") ? true : false);
}

bool IsLoop(graph)
GraphPointer graph;
{
    char *model;

    model = (char *)GetAttribute(graph->Model, "model_name");
    return((!strcmp(model, "iteration") || !strcmp(model, "do") ||
	    !strcmp(model, "dobody")) ? true : false);
}

bool IsFunc(graph)
GraphPointer graph;
{
    char *model;

    model = (char *)GetAttribute(graph->Model, "model_name");
    return(strcmp(model, "func") ? false : true);
}

bool AllEdgesBool(node)
NodePointer node;
{
    register ListPointer ptr;
    register EdgePointer edge;

    for (ptr = node->InEdges; ptr != NULL; ptr = ptr->Next) {
        edge = EP(ptr);
	if (!IsBoolType(edge)) return(false);
    }
    for (ptr = node->OutEdges; ptr != NULL; ptr = ptr->Next) {
        edge = EP(ptr);
	if (!IsBoolType(edge)) return(false);
    }
    return(true);
}

bool AllEdgesInt(node)
NodePointer node;
{
    register ListPointer ptr;
    register EdgePointer edge;
    bool AllInEdgesInt();

    if(!AllInEdgesInt(node)) return(false);
    for (ptr = node->OutEdges; ptr != NULL; ptr = ptr->Next) {
        edge = EP(ptr);
	if (!IsIntType(edge)) return(false);
    }
    return(true);
}

bool AllInEdgesInt(node)
NodePointer node;
{
    register ListPointer ptr;
    register EdgePointer edge;

    for (ptr = node->InEdges; ptr != NULL; ptr = ptr->Next) {
        edge = EP(ptr);
	if (!IsIntType(edge)) return(false);
    }
    return(true);
}

bool AllInEdgesIntorBool(node)
NodePointer node;
{
    register ListPointer ptr;
    register EdgePointer edge;

    for (ptr = node->InEdges; ptr != NULL; ptr = ptr->Next) {
        edge = EP(ptr);
	if (!IsIntType(edge) && !IsBoolType(edge)) return(false);
    }
    return(true);
}

bool AllEdgesFixed(node)
NodePointer node;
{
    register ListPointer ptr;
    register EdgePointer edge;
    bool AllInEdgesFixed();

    if (!AllInEdgesFixed(node)) return(false);
    for (ptr = node->OutEdges; ptr != NULL; ptr = ptr->Next) {
        edge = EP(ptr);
	if (!IsFixedType(edge)) return(false);
    }
    return(true);
}

bool AllInEdgesFixed(node)
NodePointer node;
{
    register ListPointer ptr;
    register EdgePointer edge;

    for (ptr = node->InEdges; ptr != NULL; ptr = ptr->Next) {
        edge = EP(ptr);
	if (!IsFixedType(edge)) return(false);
    }
    return(true);
}

GetDelayCnt(node, flag)
NodePointer node;
int flag;
{
    int lb, ub;
    EdgePointer DelayValEdge;
    char *range;

    if (node->InEdges->Next == NULL ||
        node->InEdges->Next->Type == NullNode) { /* constant delay */
        if (HasAttribute(node->Arguments, "delay"))
            return((int) GetAttribute(node->Arguments, "delay"));
        else
            return(1);
    } else {        /* non-constant delay values */
        DelayValEdge = EP(node->InEdges->Next);
	if (!HasAttribute(DelayValEdge->Arguments, "manifest"))
	    printmsg("GetDelayCnt:","Non-manifest delay value %s\n",
				DelayValEdge->Name);
	range = (char *)GetAttribute(DelayValEdge->Arguments, "manifest");
        ExtractRange(range, &lb, &ub);
	return((flag == LOWER)? lb : ub);
    }
}

/**************************************************************************
  The following are routines to generate temporary edge, node and graph names.
****************************************************************************/
int Edgenum = 0;
static int Nodenum = 0;

char *TmpEdgeName()
{
    char buffer[STRSIZE];

    sprintf(buffer, "e%d", Edgenum++);
    return(Intern(buffer));
}

char *TmpGraphName()
{
    char buffer[STRSIZE];
    static int num = 1;

    sprintf(buffer, "graph%d", num++);
    return(Intern(buffer));
}

char *
TmpNodeName()
{
    char buffer[STRSIZE];

    sprintf(buffer, "n%d", Nodenum++);
    return(Intern(buffer));
}

itoa(n,s)
    char s[];
    int n;
{
    int i;

    i = 0;
    do {
  	s[i++] = n % 10 + '0';
    } while ((n /= 10) > 0);
    s[i] = '\0';
    reverse(s);
    return(i);
}

reverse(s)
    char s[];
{
    int c, i, j;

    for (i=0, j = strlen(s)-1; i<j; i++, j--) {
    	c = s[i];
	s[i] = s[j];
	s[j] = c;
    }
}

/**************************************************************************
  EdgeName() -- For control edges, i.e. edges representing arrays, append
  the usual edge name with the indices, and finally "_c".  This is so that
  we can differentiate different control edges of the same array.  These
  edges are stored under the same name (the name of the array) in the
  Edgetable and Edgelist. Only their arguments, the indices, are different.
  For strings with strange characters, we add " " around them.  This is
  done by strdup(str) when PrintAtom() is called.
****************************************************************************/
char *EdgeName(e)
    EdgePointer e;
{
    static char name[STRSIZE]; 
    char label[STRSIZE], *index;
    char *ptr;
    int low, high, i = 0;

    sprintf(name, "%s", e->Name);
    ptr = &name[strlen(e->Name)];

    if (!strcmp(e->Class, "control")) {
	sprintf(label, "index%d", i++);
        while((index = (char *)GetAttribute(e->Arguments, label)) != NULL) {
	    sprintf(ptr, "[%s]", index);
            ptr = ptr + strlen(ptr);
	    sprintf(label, "index%d", i++);
	}
        sprintf(ptr, "%s", "_c");
    }
    return(Intern(name));
}

char *
GetArrayName(Edge)
EdgePointer Edge;
{
    char strbase[WORDLENGTH], *strtok();

    strcpy(strbase, Edge->Name);
    return(Intern(strtok(strbase, "[")));
}

static char strbase[STRSIZE];

char *BaseName(edge)
EdgePointer edge;
{
    char *strtok();

    strcpy(strbase, edge->Name);
    return(strtok(strbase, "["));
}


/**************************************************************************
  IndexEdgesName() -- Given a node, returns the full name of the output edge
  expression, i.e. with brackets.
****************************************************************************/
char *IndexEdgesName(n)
    NodePointer n;
{
    static char str[STRSIZE];
    char *name, *value, *p1, *p2, *ConstIndexName();
    ListPointer ptr, IndexEdges;
    EdgePointer edge;
    NodePointer Innode;

    IndexEdges = n->InEdges;	 /* READ node, index edges starts at 1st edge */
    if (strcmp(n->Master->Name, "write") == 0)	  
	IndexEdges = n->InEdges->Next;  /* WRITE node, index edges is 2 */
    if (IndexEdges == NULL)
	return(ConstIndexName(n));
    p1 = str;
    name = (char *)GetAttribute(n->Arguments, "array_name");
    for (p2 = name; *p2 != NULL;)
	*p1++ = *p2++;
    for (ptr = IndexEdges; ptr != NULL; ptr = ptr->Next) {
	edge = EP(ptr);
	name = edge->Name;
        if (edge->InNodes != NULL && edge->InNodes->Type == NodeNode) {
            Innode = NP(edge->InNodes);
/* copy input edge names, except if from a constant node, then copy value */
	    if (!strcmp(Innode->Master->Name, "constant")) {
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
  ConstIndexName() -- Given a node, returns the full name of the output
  edge constant expression, i.e. with brackets.
****************************************************************************/
char *
ConstIndexName(n)
NodePointer n;
{
    static char str[STRSIZE];
    char label[STRSIZE], *range;
    int i = 0;
    char *name, *value, *p1, *p2, *ConstIndexName();
    ListPointer ptr, IndexEdges;
    EdgePointer edge;
    NodePointer Innode;

    p1 = str;
    name = (char *)GetAttribute(n->Arguments, "array_name");
    for (p2 = name; *p2 != NULL;)
	*p1++ = *p2++;
    sprintf(label, "index%d", i);
    while (HasAttribute(n->Arguments, label)) {
        name = (char *)GetAttribute(n->Arguments, label);
        if (!isnumber(name))   /* is a number */
	    printmsg("ConstIndexName:","Range indices %s on node %s\n",
			name, n->Name);
	*p1++ = '[';
	for (p2 = name; *p2 != NULL;)
	    *p1++ = *p2++;
	*p1++ = ']';
	sprintf(label, "index%d", ++i);
    }
    *p1 = '\0';
    return(str);
}

/**************************************************************************
  GetNodeFromName() -- returns node given its name
****************************************************************************/
NodePointer
GetNodeFromName(Graph, Name)
GraphPointer Graph;
char *Name;
{
    NodePointer Node;

    if (Name == NULL) return ((NodePointer)NULL);
    for (Node = Graph->NodeList; Node != NULL; Node = Node->Next) {
	if (strcmp(Node->Name, Name) == 0) break;
    }
    if (Node == NULL) return((NodePointer)NULL);
    return(Node);
}

/**************************************************************************
  MakeRange() -- returns "lb..ub"
****************************************************************************/
char *
MakeRange(lb, ub)
int lb, ub;
{
    char buf[STRSIZE];
    sprintf(buf, "%d..%d", lb, ub);
    return(Intern(buf));
}

HasArrayIndex(Edge, index)
EdgePointer Edge;
int index;
{
    char label[STRSIZE], labelEval[STRSIZE];

    sprintf(labelEval, "index%dEval", index);
    sprintf(label, "index%d", index);
    if (HasAttribute(Edge->Arguments, labelEval) ||
	HasAttribute(Edge->Arguments, label))
	return(TRUE);
    return(FALSE);
}

GetArrayIndexRange(Edge, index, lb, ub)
EdgePointer Edge;
int index;
int *lb, *ub;
{
    char label[STRSIZE], labelEval[STRSIZE], *range;

    sprintf(labelEval, "index%dEval", index);
    sprintf(label, "index%d", index);
    if (HasAttribute(Edge->Arguments, labelEval))
       ExtractRange((char *) GetAttribute(Edge->Arguments, labelEval),
                                                                lb,ub);
    else if (HasAttribute(Edge->Arguments, label)) {
       range = (char *)GetAttribute(Edge->Arguments, label);
       if (isnumber(range))   /* is a number */
           *lb = *ub = atoi(range);
       else         /* is of form lb..ub */
           ExtractRange(range, lb,ub);
    } else
       printmsg("GetArrayIndexRange","Cannot find edge %s index %d",
			Edge->Name, index);
}

GetArrayRange(Graph, Edge, lb, ub)
GraphPointer Graph;
EdgePointer Edge;
int *lb, *ub;
{
    char *Index;

    Index = Intern((char *) GetAttribute(Edge->Arguments, "index0"));

    /* Find the edge */
    for (Edge = Graph->EdgeList; Edge != NULL; Edge = Edge->Next)
        if (Edge->Name == Index)
            break;

    if (Edge == NULL)
        ExtractRange("unknown", lb, ub);
    else
        ExtractRange(GetAttribute(Edge->Arguments, "manifest"), lb, ub);
}

int
AreOverlappingRanges(lb1, ub1, lb2, ub2)
int lb1, ub1, lb2, ub2;
{
    int tmp1, tmp2;

    tmp1 = (lb1 >= lb2 ) ? lb1 : lb2;
    tmp2 = (ub1 < ub2) ? ub1 : ub2;
    return ((tmp1 > tmp2) ? FALSE : TRUE);
}

#define INDEXBOUND 500

/**************************************************************************
  ExtractRange() -- takes "lb..ub" and returns lb, ub.  If manifest range
  is unknown, give it maximum range 0..500.
****************************************************************************/
ExtractRange(range, lb, ub)
char *range;
int *lb, *ub;
{
    char buf[STRSIZE];
    char *strtok();
    int tmp;

    if (range == NULL || strcmp(range, "unknown") == 0) {
        *lb = 0;
        *ub = INDEXBOUND;
    } else {
        strcpy(buf, range);
        *lb = atoi(strtok(buf, ".."));
        *ub = atoi(strtok(NULL, ".."));
    }
    if (*lb > *ub) {
        tmp = *lb;
        *lb = *ub;
        *ub = tmp;
    }
}

/**************************************************************************
  PrintSilage() -- Print Silage src code position from node or edge.
**************************************************************************/
PrintSilage(Attribute)
ListPointer Attribute;
{
    ListPointer Attr, Entry;

    if (!HasAttribute(Attribute, "Silage")) return;
    Attr = (ListPointer) GetAttribute(Attribute, "Silage");
    if (Attr->Type != CharNode)
	printmsg("PrintSilage", "Cannot print Silage position\n");
    printmsg(NULL, "    > FileName: %s ", (char *)Attr->Entry);
    printmsg(NULL, "LineNum: %d ", (int)Attr->Next->Entry);
    printmsg(NULL, "CharNum: %d\n", (int)Attr->Next->Next->Entry);
}

Error(msg)
char *msg;
{
    printmsg(NULL, "Error ** %s\n", msg);
    exit(1);
}

Warning(msg)
char *msg;
{
    extern FILE *LogFile;

    printmsg(NULL, "Warning ** %s\n", msg);
}

