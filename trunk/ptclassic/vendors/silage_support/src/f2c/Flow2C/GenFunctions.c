/*   $Id$ */

#include "codegen.h"   /* for RootGraph structure, has dag.h, flowgraph.h  */
#include "st.h"

pointer GetAttribute();
ListPointer SetAttribute(), ListAppend(), new_list();
char *BaseName(), *Intern(), *DelayName(), *DeclName();
extern FILE *CFD, *LogFile;
extern int strcmp();
extern GraphPointer Root;

bool IsArray(), IsFixedType(), IsIntType();
bool IsInput(), IsOutput();
bool IsDelayIO(), IsDelayOut();

extern int indexlevel;
extern st_table *Edgetable[MAXINDEXLEVEL];
extern ListPointer ListOfParams;

GenFunctions(pl_flag)
bool pl_flag;
{
    register GraphPointer Graph;
    register GraphPointer ReverseGraph,tmp;
    bool IsLoop();

    if(!pl_flag)
    {
    for (Graph = Root; Graph != NULL; Graph = Graph->Next) 
	{
        	if (!IsLoop(Graph))
	    	GenFunc(Graph,pl_flag);
	}
    }
/* Need to generate in the reverse order for declaration purposes */
    if(pl_flag)
    {
	tmp = (GraphPointer) malloc (sizeof(Graph));
	tmp = NULL;

	while(tmp != Root) 
	{
		for(Graph = Root; Graph->Next != tmp; Graph = Graph->Next);
		fprintf(stderr,"Graph->name %s \n",Graph->Name);
		tmp = Graph;
        	if (!IsLoop(Graph))
	    	GenFunc(Graph,pl_flag);
	} /* while */
    } /*  pl_flag */
}

GenFunc(Graph,pl_flag)
GraphPointer Graph;
bool pl_flag;
{
   FILE *fp;
   FILE *fq;
   char fInName[100], fOutName[100], inputName[100], outputName[100];
   struct listOfIO {
        char name[100];
   };
   struct listOfIO inputList[10];       /* max num of inputs */
   struct listOfIO outputList[10];      /* max num of outputs */
   int numIn, numOut, numberIn, numberOut, k ;
   bool GenFormalParams(), GenFormalConParams(), comma = false;
   bool GenDeclFormalParams();
   bool topLevel,mycomma;

   numIn = 0; numOut = 0;
   topLevel = false;

if( pl_flag && (Graph == Root) )
{
   topLevel = true;
   strcpy(fInName,Graph->Name); strcat(fInName,".inputs");
   strcpy(fOutName,Graph->Name); strcat(fOutName,".outputs");
   fp = fopen(fInName,"r");
   if (fp == NULL) {fprintf(stderr,"error opening input file\n"); exit(-1); }
   fq = fopen(fOutName,"r");
   if (fq == NULL) {fprintf(stderr,"error opening output file\n"); exit(-1); }

   while( fscanf(fp,"%s",inputName) == 1)
   { strcpy(inputList[numIn].name,inputName); numIn++; }
   numberIn = numIn;

   while( fscanf(fq,"%s",outputName) == 1)
   { strcpy(outputList[numOut].name,outputName); numOut++; }
   numberOut = numOut;
}
    indexlevel++;
    Edgetable[indexlevel] = st_init_table(strcmp, st_strhash);

    if(pl_flag) fprintf(CFD,"void \t");
    fprintf(CFD, "Sim_%s (", Graph->Name);
	if (GE(Graph)->HasDelay) {
	    if (comma == true)
	        fprintf(CFD, ", ");
	    else
	        comma = true;
	    if(!pl_flag)    fprintf(CFD, "pST");
    }
/* this will generate only names of this list for regular case
   while for the .pl file, it should also have the type of signal */
if (!pl_flag)
{
    ListOfParams = (ListPointer) NULL;
    comma = GenFormalParams(Graph->InEdges, comma);
    comma = GenFormalConParams(Graph->InControl, comma);
    comma = GenFormalParams(Graph->OutEdges, comma);
    comma = GenFormalConParams(Graph->OutControl, comma);
    fprintf(CFD, ")\n");
}
/* Generate declarations for the formal params, fixedtype and int  */
    if(!pl_flag)
    {
	if (GE(Graph)->HasDelay) 
	{
	    GenDelayStructName(Graph);
	    fprintf(CFD, " *pST;\n");
    	}
    }

if(!pl_flag)
{
    ClearList(ListOfParams);
    ListOfParams = (ListPointer) NULL;
    mycomma = false;
    GenDeclFormalParams(Graph->InEdges,pl_flag,mycomma);
    GenDeclFormalParams(Graph->InControl,pl_flag,mycomma);
    GenDeclFormalParams(Graph->OutEdges,pl_flag,mycomma);
    GenDeclFormalParams(Graph->OutControl,pl_flag,mycomma);
    ClearList(ListOfParams);
    fprintf(CFD, "{\n");
}

/* now is the special case where we generate the signal type
along with the signalname in the function declaration,
also in case of the pl_flag and the top level, we also need
to pass the data type to the signal */
   mycomma = false;
   if(pl_flag)
   {
	if (GE(Graph)->HasDelay) 
	{
		GenDelayStructName(Graph);
		fprintf(CFD,"* pST");
		mycomma = true;
	}
/* here will be the other formal parameters along with the data types */
	ClearList(ListOfParams);
    	ListOfParams = (ListPointer) NULL;
   mycomma = GenDeclFormalParams(Graph->InEdges,pl_flag,mycomma);
   mycomma = GenDeclFormalParams(Graph->InControl,pl_flag,mycomma);
   mycomma = GenDeclFormalParams(Graph->OutEdges,pl_flag,mycomma);
   mycomma = GenDeclFormalParams(Graph->OutControl,pl_flag,mycomma);
    	ClearList(ListOfParams);
   } /* pl_flag */

   if(topLevel)
   {
		if(mycomma == true) fprintf(CFD,",");
   	for( k=0; k< (numberIn-1) ; k++)
   	{ fprintf(CFD, "float* r_%s_%s, ",Graph->Name,inputList[k].name); }
   	fprintf(CFD, "float* r_%s_%s",Graph->Name,inputList[numberIn-1].name);
	if(numberOut>0) fprintf(CFD,",");
   	for( k=0; k< (numberOut-1) ; k++)
   	{ fprintf(CFD, "float* s_%s_%s,",Graph->Name,outputList[k].name); }
   	fprintf(CFD, "float* s_%s_%s",Graph->Name,outputList[numberOut-1].name);
   } /* topLevel */
if(pl_flag) fprintf(CFD, ")\n{\n");

/* Generate declarations for local delay variables used */
    GenDeclDelayVars(Graph);

/* Generate declarations for local variables used, fixedtype and int */
    GenDeclLocalVars(Graph);

/* Generate initializations for local delay variables */
/* GenInitDelayVars(Graph);   */

/* Remove DelayNodes and TopologicalOrder graph for GenStatements */
    RemoveDelaysInGraph(Graph);
    RemoveLpDelaysInGraph(Graph);
    TopologicalOrder(Graph);

/* Generate the body of the function */
    fprintf(CFD, "\n/* statements of function body */\n");
    GenStatements(Graph,pl_flag);

/* Generate statements to display requested signals */
    GenDisplays(Graph,Graph->EdgeList,pl_flag);
    GenDisplays(Graph,Graph->ControlList,pl_flag);

/* Generate statements to update delay signals */
    GenIncrementDelays (GE(Graph)->ListOfDelays);

    fprintf(CFD, "}\n\n");
    st_free_table(Edgetable[indexlevel]);
    indexlevel--;
}

bool GenFormalParams(List, comma)
ListPointer List;
bool comma;
{
    register ListPointer ptr;
    register EdgePointer edge;

    if (List == NULL) return(comma);
    for (ptr = List; ptr != NULL; ptr = ptr->Next) {
	edge = EP(ptr);
        if (IsMemberOfList(DeclName(edge), CharNode, ListOfParams)) continue;
    	if (comma == true)
	    fprintf(CFD, ", ");
	else
	    comma = true;
  	if (IsFixedType(edge) || (!IsFixedType(edge) && IsOutput(edge)))
	    fprintf(CFD, "p_");
	GenEdgeName(edge);
        ListOfParams = ListAppend(new_list(CharNode, DeclName(edge)),
				ListOfParams);
    }
    return(comma);
}

bool GenFormalConParams(List, comma)
ListPointer List;
bool comma;
{
    register ListPointer ptr;
    register EdgePointer edge;
    ArrayDefinitionList adl, adlNext, AddToControlListDecl(), conlist = NULL;
    int i = 1, j;

    if (List == NULL) return(comma);
    for (ptr = List; ptr != NULL; ptr = ptr->Next) {
	    edge = EP(ptr);
	    conlist = AddToControlListDecl(edge, conlist);
    }

    for (ptr = List; ptr != NULL; ptr = ptr->Next) {
	edge = EP(ptr);
        if (IsMemberOfList(DeclName(edge), CharNode, ListOfParams)) continue;
        if (comma == true)
            fprintf(CFD, ", ");
        else
            comma = true;
        ListOfParams = ListAppend(new_list(CharNode, DeclName(edge)), 
					ListOfParams);
	GenEdgeName(edge);
    }
    for (adl = conlist; adl != NULL; adl = adlNext) {
	adlNext = adl->Next;
	free(adl);
    }
    return(comma);
}

GenEdgeMaxIndex(edge)
EdgePointer edge;
{
    char str[STRSIZE];
    int val, j = 0;

    sprintf(str, "maxindex%d", j);
    while ((val = (int)GetAttribute(edge->Attributes, str)) != NULL) {
	fprintf(CFD, "[%d]", val);
        sprintf(str, "maxindex%d", ++j);
    }
}
          
bool GenDeclFormalParams(List,pl_flag,mycomma)
ListPointer List;
bool pl_flag;
bool mycomma;
{
    register ListPointer ptr;
    register EdgePointer edge;
    int c;
if(List == NULL ) return(mycomma);
if( mycomma == false ) c = 0;
else if (mycomma == true) c = 1;
    for(ptr = List; ptr != NULL; ptr = ptr->Next) {
	if(pl_flag) 
	{ 
		if(c == 1)
		{
			fprintf(CFD, ", ");    
			c = 0;
		}
	}
	edge = EP(ptr);
        if (IsMemberOfList(DeclName(edge), CharNode, ListOfParams)) continue;
	if (IsFixedType(edge)) {
		if(!pl_flag)
		{
			fprintf(CFD, "Sig_Type ");
	    		if (IsArray(edge) == false)
	        	fprintf(CFD, "*p_");
		}
		else if(pl_flag)
		{
			fprintf(CFD, "Sig_Type* ");
	    		if (IsArray(edge) == false)
	        	fprintf(CFD, "p_");
		}
	}
	else {
		if(!pl_flag)
		{
	    		fprintf(CFD, "int ");
	    		if (IsOutput(edge))
	        	fprintf(CFD, "*p_");
		}
		else if(pl_flag)
		{
	    		fprintf(CFD, "int* ");
	    		if (IsOutput(edge))
	        	fprintf(CFD, "p_");
		}
        }
	GenEdgeName(edge);
	GenEdgeMaxIndex(edge);
	if(!pl_flag) fprintf(CFD, ";\n");    
	if(pl_flag) c = 1; 
        st_insert(Edgetable[indexlevel], DeclName(edge), 0);
	ListOfParams = ListAppend(new_list(CharNode, DeclName(edge)), 
					ListOfParams);
    }
	if( c==1) mycomma = true;
	else if ( c==0) mycomma = false;
return(mycomma);
}

GenDeclLocalVars(Graph)
GraphPointer Graph;
{
    fprintf(CFD, "\n/* Declaring variables to hold temporary edges */\n");
    GenDeclLocalInt(Graph);
    GenDeclLocalFixed(Graph);
}

GenDeclLocalInt(Graph)
GraphPointer Graph;
{
    register EdgePointer edge;
    bool IsDeclaredEdge();
    ArrayDefinitionList cl, clNext, AddToControlListDecl(), conlist = NULL;
    register int i;
    NameListDefinitionList el, elNext, AddToNameList(), edgelist = NULL;
    ListPointer Ptr;

    GenIterIndex(Graph);
    for(edge = Graph->EdgeList; edge != NULL; edge = edge->Next) {
	    if (!IsFixedType(edge) && !IsDeclaredEdge(edge)) {
	        edgelist = AddToNameList(edge->Name, edgelist);
	    }
    }
    for(edge = Graph->ControlList; edge != NULL; edge = edge->Next) {
            if (!IsFixedType(edge) && !IsDeclaredEdge(edge)) {
                conlist = AddToControlListDecl(edge, conlist);
            }
    }
    for(el = edgelist; el != NULL; el = elNext) {
	elNext = el->Next;
        fprintf(CFD, ", %s", el->Name);
        st_insert(Edgetable[indexlevel], Intern(el->Name), 0);
        free(el);
    }
    for(cl = conlist; cl != NULL; cl = clNext) {
        clNext = cl->Next;
        fprintf(CFD, ", ");
        GenConEdgeDecl(cl);
        st_insert(Edgetable[indexlevel], DeclName(cl->edge), 0);
	free(cl);
    }
    for (i=0; i< MaxNrDpDim(Graph); i++) {
	    fprintf(CFD, ", DI_%d", i);
    }
    for (Ptr = GE(Graph)->ListOfFuncApps; Ptr != NULL; Ptr = Ptr->Next) {
	if ((int)Ptr->Label > 1 && GE(NP(Ptr)->Master)->HasDelay) {
	    fprintf(CFD, ", FI_");
	    GenDelayInstanceName(NP(Ptr)->Master, NP(Ptr));
            fprintf(CFD, "=0");
	}
    }
    fprintf(CFD, ";\n");
}

GenIterIndex(Graph)
GraphPointer Graph;
{
    register NodePointer node;
    char *index;
    NameListDefinitionList nl, nlNext, AddToNameList(), indexlist = NULL;
    bool IsDeclaredName();

    for(node = Graph->NodeList; node != NULL; node = node->Next) {
        if (IsIterationNode(node)) {
	    index = (char *)GetAttribute(node->Arguments, "index");
	    if (index != NULL && !IsDeclaredName(index))
	        indexlist = AddToNameList(index, indexlist);
	}
    }
    if (indexlist == NULL)
        fprintf(CFD, "    int idum");
    else {
        fprintf(CFD, "    int %s", indexlist->Name);
        st_insert(Edgetable[indexlevel], Intern(indexlist->Name), 0);
        for(nl = indexlist->Next; nl != NULL; nl = nlNext) {
	    nlNext = nl->Next;
	    fprintf(CFD, ", %s", nl->Name);
            st_insert(Edgetable[indexlevel], Intern(nl->Name), 0);
	    free(nl);
	}
    }
}

NameListDefinitionList
AddToNameList(name, list)
char *name;
NameListDefinitionList list;
{
    register NameListDefinitionList ptr, nl;

    for(ptr = list; ptr != NULL; ptr = ptr->Next) {
	if (!strcmp(name, ptr->Name))
	    return(list);
    }
    nl = NEW (NameListDefinition);
    nl->Name = name;
    nl->Next = NULL;
    if (list == NULL)
        return(nl);
    else {
        for(ptr = list; ptr->Next != NULL; ptr = ptr->Next);
	    ptr->Next = nl;
	    return(list);
    }
}
 
MaxNrDpDim(graph)
GraphPointer graph;
{
    return(MaxNrDpDimList(graph->ControlList));
}

MaxNrDpDimList(List)
EdgePointer List;
{
    register EdgePointer edge;
    char label[STRSIZE];
    int i = 0, max = 0;

    for(edge = List; edge != NULL; edge = edge->Next) {
	if (HasAttribute(edge->Attributes, "IsDisplay")) {
	    sprintf(label, "index%d", i);
	    while(1) {
	        if (HasAttribute(edge->Arguments, label))
	       	    sprintf(label, "index%d", ++i);
 	        else
		    break;
	    }
	    if (max < i) max = i;
	}
	i = 0;
    }
    return(max);
}

GenConEdgeDecl(adl)
ArrayDefinitionList adl;
{
    int i = 0;

    fprintf(CFD, "%s", adl->Name);
    while (adl->index[i] >= 0) {
	    fprintf(CFD, "[%d]", adl->index[i] + 1);
	    i++;
    }
}

ArrayDefinitionList
AddToControlListDecl(edge, List)
EdgePointer edge;
ArrayDefinitionList List;
{
    register ArrayDefinitionList adl, ptr;
    ArrayDefinitionList new_controlList();

    for (ptr = List; ptr != NULL; ptr = ptr->Next) {
	if(!strcmp(BaseName(edge), ptr->Name)) {
            UpdateIndex(edge, ptr);
	    return(List);
	}
    }
    adl = new_controlList(edge);
    if (List == NULL)
        return(adl);
    else {
        for (ptr = List; ptr->Next != NULL; ptr = ptr->Next);
        ptr->Next = adl;
        return(List);
    }
}

ArrayDefinitionList
new_controlList(edge)
EdgePointer edge;
{
    ArrayDefinitionList adl;
    char str[STRSIZE];
    int lb, ub;
    int i = 0;

    adl = NEW (ArrayDefinition);
    adl->edge = edge;
    adl->Name = Intern(BaseName(edge));
    adl->index[0] = -1;
    adl->index[1] = -1;
    adl->index[2] = -1;
    adl->index[3] = -1;
    while (HasArrayIndex(edge, i)) {
	GetArrayIndexRange(edge, i, &lb, &ub);
	adl->index[i] = ub;
        sprintf(str, "maxindex%d", i);
	edge->Attributes = SetAttribute(str, IntNode,
                            (char *)(ub+1), edge->Attributes);
        i++;
	if (i > 3)
	    Error("new_controlList() -- exceed array range\n");
    }
    adl->Next = NULL;
    return(adl);
}

UpdateIndex(edge, ptr)
EdgePointer edge;
ArrayDefinitionList ptr;
{
    int i = 0, lb, ub;
    char str[STRSIZE];

    while (HasArrayIndex(edge, i)) {
	GetArrayIndexRange(edge, i, &lb, &ub);
        if (ub > ptr->index[i]) {
	    ptr->index[i] = ub;
            sprintf(str, "maxindex%d", i);
            edge->Attributes = SetAttribute(str, IntNode,
                            (char *)(ub+1), edge->Attributes);
        }
        i++;
	if (i > 3)
	    Error("UpdateIndex() -- exceed array range\n");
    }
}

GenDeclLocalFixed(Graph)
GraphPointer Graph;
{
    register EdgePointer edge;
    bool IsDeclaredEdge();
    ArrayDefinitionList cl, clNext, AddToControlListDecl(), conlist = NULL;
    NameListDefinitionList el, elNext, AddToNameList(), edgelist = NULL;


    fprintf(CFD, "    Sig_Type fdum");
    for(edge = Graph->EdgeList; edge != NULL; edge = edge->Next) {
	    if (IsFixedType(edge) && !IsDeclaredEdge(edge)) {
                edgelist = AddToNameList(edge->Name, edgelist);
	    }
    }
    for(edge = Graph->ControlList; edge != NULL; edge = edge->Next) {
            if (IsFixedType(edge) && !IsDeclaredEdge(edge)) {
                conlist = AddToControlListDecl(edge, conlist);
            }
    }
    for(el = edgelist; el != NULL; el = elNext) {
        elNext = el->Next;
        fprintf(CFD, ", %s", el->Name);
        st_insert(Edgetable[indexlevel], Intern(el->Name), 0);
        free(el);
    }
    for(cl = conlist; cl != NULL; cl = clNext) {
        clNext = cl->Next;
        fprintf(CFD, ", ");
        GenConEdgeDecl(cl);
        st_insert(Edgetable[indexlevel], DeclName(cl->edge), 0);
	free(cl);
    }
    fprintf(CFD, ";\n");
}

GenDisplays(Graph,List,pl_flag)
GraphPointer Graph;
EdgePointer List;
bool pl_flag;
{
    register EdgePointer edge;

    for(edge = List; edge != NULL; edge = edge->Next) {
  	if(!pl_flag)
	{
		if (HasAttribute(edge->Attributes, "IsDisplay")) {
 	    	if (IsArray(edge))
			 GenDisplayArray(edge);
	    	else
			 GenDisplaySingle(edge);
        	}
	}
	if(pl_flag)
	{
		if (HasAttribute(edge->Attributes, "IsDisplay")) {
			fprintf(CFD,"*s_%s_",Graph->Name);
    			GenEdgeName(edge);
			fprintf(CFD," = ");
    			GenEdgeName(edge);
			fprintf(CFD,";\n");
		}
	}
    }
}

GenDisplaySingle(edge)
EdgePointer edge;
{
    GraphPointer Graph;

    if ((Graph = EE(edge)->graph) == NULL) {
	Error("GenDisplaySingle() : No \"graph\" attributes\n");
    }
    Indent();
    if (IsFixedType(edge))
	fprintf(CFD, "Fix");
    else if (IsIntType(edge))
    	fprintf(CFD, "Int");
    else
	fprintf(CFD, "Bool");
    fprintf(CFD, "Display (");
    fprintf(CFD, "dfd_dump");
    fprintf (CFD, ", \"%s ", Graph->Name);
    GenEdgeName(edge);
    fprintf (CFD, "\", ");
    GenSingleEdgeDeref(edge);
    if (IsFixedType(edge))
    GenFixedType(edge);
    fprintf(CFD, ");\n");
}

GenDisplayArray(edge)
EdgePointer edge;
{
    GraphPointer Graph;
    char str[STRSIZE];
    int index, i = 0;

    if ((Graph = EE(edge)->graph) == NULL) {
	Error("GenDisplaySingle() : No \"graph\" attributes\n");
    }
    sprintf(str, "maxindex%d", i);
    while (HasAttribute(edge->Attributes, str)) {
        index = (int)GetAttribute(edge->Attributes, str);
        Indent();
        fprintf (CFD, "for (DI_%d = 0; DI_%d < %d; DI_%d++) ",i,i,index,i);
	    OpenBraces();
        sprintf(str, "maxindex%d", ++i);
    }
    Indent(); fprintf (CFD, "char B[100];\n");
    Indent(); fprintf (CFD, "sprintf (B, \"%s %s", Graph->Name, BaseName(edge));
    for (index = 0; index < i; index++)
	    fprintf(CFD, "[%%d]");
    fprintf(CFD, "\"");
    for (index = 0; index < i; index++)
	    fprintf(CFD, ", DI_%d", index);
    fprintf(CFD, ");\n");

    Indent();
    if (IsFixedType(edge))
        fprintf(CFD, "Fix");
    else if (IsIntType(edge))
        fprintf(CFD, "Int");
    else
        fprintf(CFD, "Bool");
    fprintf(CFD, "Display (");
    fprintf(CFD, "dfd_dump");
    fprintf(CFD, ", B, ");
    GenEdgeName(edge);
    for (index = 0; index < i; index++)
	    fprintf(CFD, "[DI_%d]", index);
    if (IsFixedType(edge))
        GenFixedType(edge);
    fprintf(CFD, ");\n");
    for (index = 0; index < i; index++)
	    CloseBraces();
}

/* some more work has to be done here. */
GenSingleEdgeDeref(edge)
EdgePointer edge;
{
    bool IsFuncInput(), IsFuncOutput();

    GenSingleEdgeRef(edge);
    if ((IsFuncInput(edge) && IsFixedType(edge)) || IsFuncOutput(edge))
	    fprintf(CFD, "[0]");
    GenEdgeDelay(edge);
}

GenSingleEdgeRef(edge)
EdgePointer edge;
{
    bool IsFuncInput(), IsFuncOutput();
    NodePointer Node;

    if ((IsFuncInput(edge) && IsFixedType(edge)) || IsFuncOutput(edge))
	    fprintf(CFD, "p_");
    else if (IsDelayIO(edge)) 
	    fprintf(CFD, "pST->");
    GenEdgeName(edge);
    if (HasAttribute(edge->Attributes, "readout")) {
        Node = (NodePointer)GetAttribute(edge->Attributes, "readout");
        GenEdgeDimFromNode(Node);
    }
}

char *
DeclName(edge)
EdgePointer edge;
{
    char str[STRSIZE];
    int suf;

    if (IsArray(edge) || HasAttribute(edge->Attributes, "readout")) {
        sprintf(str, "%s", BaseName(edge));
    }
    else if (IsDelayOut(edge)) {
        sprintf(str, "%s", DelayName(edge));
    } else {
        sprintf(str, "%s", edge->Name);
    }
    return(Intern(str));
}
