/* $Id$ */

/* Copyright (C) 1990 Regents of the University of California
 * All rights reserved.
 */

/*
 * Library of generic utility routines for the flowgraph database
 */

#include "flowgraph.h"
#include "st.h"

/* global functions */
extern ListPointer new_list();
extern char *Intern();

int fatal = FALSE;

pointer
GetAttribute(Attributes, Attribute)
ListPointer Attributes;
char *Attribute;
{
    for (; Attributes != NULL; Attributes = Attributes->Next) {
#if 0
	if (Attributes->Label == NULL) continue;
#endif /* 0 */
	if (!strcmp(Attributes->Label, Attribute))
	    return (Attributes->Entry);
    }
    return (NULL);
}

EntryType
GetAttributeType(Attributes, Attribute)
ListPointer Attributes;
char *Attribute;
{
    for (; Attributes != NULL; Attributes = Attributes->Next)
	if (!strcmp(Attributes->Label, Attribute))
	    return (Attributes->Type);
    return (NullNode);
}

int
GetNumAttribute(Attributes, Attribute, Value)
ListPointer Attributes;
char *Attribute;
float *Value;
{
    for (; Attributes != NULL; Attributes = Attributes->Next) {
	if (Attributes->Label && !strcmp(Attributes->Label, Attribute)) {
	    switch (Attributes->Type) {
	    case (IntNode) :
		*Value = (float) ( (int) Attributes->Entry);
		return (TRUE);
	    case (RealNode) :
		*Value = *((float *) Attributes->Entry);
		return (TRUE);
	    case (CharNode) :
		sscanf((char *) Attributes->Entry, "%f", Value);
		return (TRUE);
	    default :
	        *Value = 0.0;
		return (FALSE);
	    }
	}
    }
    return (FALSE);
}

ListPointer 
SetAttribute(Attribute, Type, Value, Attributes)
char *Attribute;
EntryType Type;
pointer Value;
ListPointer Attributes;
{
    ListPointer Entry, ListAppend();

    for (Entry = Attributes; Entry != NULL; Entry = Entry->Next) {
	if (!strcmp(Entry->Label, Attribute)) {
	    Entry->Type = Type;
	    Entry->Entry = Value;
	    return (Attributes);
	}
    }
    Entry = new_list();
    Entry->Label = Intern(Attribute);
    Entry->Type = Type;
    Entry->Entry = Value;
    return (ListAppend(Entry, Attributes));
}

ListPointer
SetAttributeAlways(Attribute, Type, Value, Attributes)
char *Attribute;
EntryType Type;
pointer Value;
ListPointer Attributes;
{
    ListPointer Entry, ListAppend();

    Entry = new_list();
    Entry->Label = Intern(Attribute);
    Entry->Type = Type;
    Entry->Entry = Value;
    return (ListAppend(Entry, Attributes));
}
    
ListPointer
InheritAttribute(SrcAttr, DestAttr, Attribute)
ListPointer SrcAttr, DestAttr;
char *Attribute;
{
    pointer value;
    EntryType type, GetAttributeType();

    if (HasAttribute(SrcAttr, Attribute)) {
         value = GetAttribute(SrcAttr, Attribute);
         type = GetAttributeType(SrcAttr, Attribute);
         DestAttr = SetAttribute(Attribute, type, value,
                                                DestAttr);
    }
    return(DestAttr);
}

int
HasAttribute(Attributes, Attribute)
ListPointer Attributes;
char *Attribute;
{
    for (; Attributes != NULL && strcmp(Attributes->Label, Attribute);
	 Attributes = Attributes->Next);
    return ((Attributes == NULL) ? FALSE : TRUE);
}

ListPointer
RemoveAttribute(Attributes, Attribute)
ListPointer Attributes;
char *Attribute;
{
    ListPointer Ptr, Last, val;

    for (Ptr = Attributes,Last = NULL;Ptr != NULL;Last = Ptr,Ptr = Ptr->Next)
	if (!strcmp(Ptr->Label, Attribute))
	    break;
    if (Ptr == NULL)
	return(Attributes);
    else if (Last == NULL)
        val = Ptr->Next;
    else {
        Last->Next = Ptr->Next;
        val = Attributes;   
    }
    free_list(Ptr);
    return(val);
}

char *
GetFunction(Node)
NodePointer Node;
{
    if (Node == NULL || Node->Master == NULL || Node->Master->Model == NULL)
	return (NULL);
    return ( (char *) GetAttribute(Node->Master->Model, "model_name"));
}

SetFunction(Node, Function)
NodePointer Node;
char *Function;
{
    if (Node == NULL || Node->Master == NULL || Node->Master->Model == NULL)
	return;
    Node->Master->Model = SetAttribute("model_name", CharNode, Intern(Function),
	Node->Master->Model);
}

int
CheckFunction(Node, Function)
NodePointer Node;
char *Function;
{
    char *Tmp, *GetFunction();

    Tmp = GetFunction(Node);
    if (Tmp != NULL) 
        return (strcmp(Function, Tmp) ? FALSE : TRUE);
    else
        return (FALSE);
}

int
IsDelayNode(Node)
NodePointer Node;
{
    char *Function, *GetFunction();

    if ((Function = GetFunction(Node)) == NULL)
	return (FALSE);
    return (!strcmp(Function, "@") || !strcmp(Function, "#") ? TRUE : FALSE);
}

int
IsLoopNode(node)
NodePointer node;
{
    char *Function, *GetFunction();

    if ((Function = GetFunction(node)) == NULL)
	return (FALSE);
    return ((!strcmp(Function, "do") || !strcmp(Function, "dobody") ||
	     !strcmp(Function, "iteration")) ? TRUE : FALSE);
}

int
IsProcessNode(Node)
NodePointer Node;
{
    return((strcmp(Node->Class, "Process")) ? FALSE : TRUE);
}

int
IsIterationNode(Node)
NodePointer Node;
{
    char *Function, *GetFunction();

    if ((Function = GetFunction(Node)) == NULL)
	return (FALSE);
    return ((!strcmp(Function, "iteration")) ? TRUE : FALSE);
}

int
NumberOfIterations(Node)
NodePointer Node;
{
    char *Function, *GetFunction();
    int Min, Max , Step, Number;

    if ((Function = GetFunction(Node)) == NULL)
	return (0);
    if (!strcmp(Function, "iteration")) {
	Min = (int) GetAttribute(Node->Arguments, "min");
	Max = (int) GetAttribute(Node->Arguments, "max");
	Step = (int) GetAttribute(Node->Arguments, "step");

	if (Step < 0)
	    Step = - Step;
	else if (Step == 0)
	    Step = 1;
	return ( (Max - Min) / Step + 1);
    }
    else if (!strcmp(Function, "do")) {
	Number = (int) GetAttribute(Node->Arguments, "avg");
	return ((Number == 0) ? 1 : Number);
    }
    else {
	return (1);
    }
}

int
IsLeafGraph(Graph)
GraphPointer Graph;
{
    /* Checks is a graph is at the bottom of the hierarchy */
    /* This routine does not require NodeClustering, but is slow */
    NodePointer Node;

    for (Node = Graph->NodeList; Node != NULL; Node = Node->Next)
	if (IsHierarchyNode(Node))
	    return(FALSE);
    return (TRUE);
}

int
IsHierarchicalGraph(Graph)
GraphPointer Graph;
{
    /* Checks if a graph is at the bottom of the hierarchy */
    /* This is a faster version than IsLeafGraph, but requires
     * that NodeClustering was done beforehand */

    NodePointer Node;

    for (Node = Graph->NodeList; Node != NULL; Node = Node->Next) {
	if (IsHierarchyNode(Node))
	    return (TRUE);
	else if (!IsDelayNode(Node))
	    return (FALSE);
    }
    return (FALSE);

}

int
IsOutputNode(Node)
NodePointer Node;
{
    char *Function, *GetFunction();

    if ((Function = GetFunction(Node)) == NULL)
	return (FALSE);
    return ((!strcmp(Function, "output")) ? TRUE : FALSE);
}

int
IsInputNode(Node)
NodePointer Node;
{
    char *Function, *GetFunction();

    if ((Function = GetFunction(Node)) == NULL)
	return (FALSE);
    return ((!strcmp(Function, "input")) ? TRUE : FALSE);
}

int
IsIONode(Node)
NodePointer Node;
{
    char *Function, *GetFunction();

    if ((Function = GetFunction(Node)) == NULL)
	return (FALSE);
    return ((!strcmp(Function, "output") || !strcmp(Function, "input")) ?
	TRUE : FALSE);
}

int 
IsMemoryNode(Node)
NodePointer Node;
{
    char *Function, *GetFunction();

    if ((Function = GetFunction(Node)) == NULL)
	return (FALSE);
    return ((!strcmp(Function, "read") || !strcmp(Function, "write")) ?
	TRUE : FALSE);
}

int
HasMemoryNodes(Graph)
GraphPointer Graph;
{
    NodePointer Node;

    for (Node = Graph->NodeList; Node != NULL; Node = Node->Next) {
	if (IsHierarchyNode(Node)) {
	    if (HasMemoryNodes(Node->Master))
		return (TRUE);
	}
	else {
	    if (IsMemoryNode(Node))
		return (TRUE);
	}
    }
    return (FALSE);
}

SetUserName(Edge)
EdgePointer Edge;
{
    Edge->Attributes = SetAttribute("UserName", CharNode, "true",
	Edge->Attributes);
}

UnsetUserName(Edge)
EdgePointer Edge;
{
    Edge->Attributes = RemoveAttribute(Edge->Attributes, "UserName"); 
}

int
IsUserName(Edge)
EdgePointer Edge;
{
    char *Value;

    Value = (char *) GetAttribute(Edge->Attributes, "UserName");
    return ((Value == NULL || strcmp(Value, "true")) ? FALSE : TRUE);
}

char *
GetType(Edge)
EdgePointer Edge;
{
    return ( (char*) GetAttribute(Edge->Arguments, "type"));
}

SetType(Edge, Type)
EdgePointer Edge;
char *Type;
{
    ListPointer SetAttribute();

    Edge->Arguments = SetAttribute("type", CharNode, Intern(Type),
                                   Edge->Arguments); 
}

int
IsHierarchyNode(Node)
NodePointer Node;
{
    if (Node == NULL)
	return (FALSE);

    return ((strcmp(Node->Master->Class, LEAF_NODE) &&
	     strcmp(Node->Master->Class, "MACRO")) ? TRUE : FALSE);
}

int
IsLeafNode(Node)
NodePointer Node;
{
    if ((!Node) || (Node->Master == NULL))    /* NULL checking -- sh 09/28/92 */
	return (FALSE);
    return ( (strcmp(Node->Master->Class, LEAF_NODE)) ? FALSE : TRUE);
}

int
IsControlEdge(Edge)
EdgePointer Edge;
{
    if (!Edge) return FALSE;    /* Edge might be NULL -- sh 09/28/92 */
    return ( (strcmp(Edge->Class, "control")) ? FALSE : TRUE);
}

int
IsControlClass(Class)
char *Class;
{
    return ((strcmp(Class, "control")) ? FALSE : TRUE);
}


int
IsConstantEdge(Edge)
EdgePointer Edge;
{
    NodePointer Source;

    if (!Edge) return FALSE;
    if (!strcmp(Edge->Class, "constant"))
	return (TRUE);
    if (Edge->InNodes != NULL && Edge->InNodes->Type == NodeNode) {
	Source = (NodePointer) Edge->InNodes->Entry;
	if (!strcmp(GetFunction(Source), "constant"))
	    return (TRUE);
    }
    return (FALSE);
}

int
IsInputEdge(Edge)
EdgePointer Edge;
{
    if (!Edge) return FALSE;
    if (Edge->InNodes == NULL)
	return (FALSE);
    if (Edge->InNodes->Type == GraphNode ||
	IsInputNode((NodePointer) Edge->InNodes->Entry))
	return (TRUE);
    return (FALSE);
}

int 
IsOutputEdge(Edge)
EdgePointer Edge;
{
    ListPointer Ptr;

    if (!Edge) return FALSE;
    for (Ptr = Edge->OutNodes; Ptr != NULL; Ptr = Ptr->Next) {
	if (Ptr->Type == GraphNode ||
	    IsOutputNode((NodePointer) Ptr->Entry))
		return (TRUE);
    }
    return (FALSE);
}

FlowVersion(Graph)
GraphPointer Graph;
{
    char NewName[WORDLENGTH], Version[8];
    char *Ptr, *rindex();

    strcpy(NewName, Graph->Name);
    Ptr = rindex(NewName, '#');
    if (Ptr == NULL) {
	strcat(NewName, "#0");
    }
    else {
	*(Ptr++) = '\0';
	sprintf(Version, "#%d", atoi(Ptr) + 1);
	strcat(NewName, Version);
    }
    Graph->Name = Intern(NewName);
}

/* Routines to set and check the current synthesis status of a graph */

static char *GraphModes[] = {
    "scheduled",
    "allocated",
    "annotated",
    "parsed",
};

static int nr_of_modes  = (sizeof(GraphModes) / sizeof(GraphModes[0]));

SetMode(Graph, Mode)
GraphPointer Graph;
char *Mode;
{
    Graph->Attributes = SetAttribute("mode", CharNode, Intern(Mode),
	Graph->Attributes);
}

int
CheckMode(Graph, Mode)
GraphPointer Graph;
char *Mode;
{
    int i,j;
    char *CurrentMode;

    CurrentMode = (char *) GetAttribute(Graph->Attributes, "mode");

    if (CurrentMode == NULL)
	CurrentMode = Intern("parsed");

    for (i = 0; i < nr_of_modes; i++) 
	if (!strcmp(CurrentMode, GraphModes[i]))
	    break;
    for (j = 0; j < nr_of_modes; j++) 
	if (!strcmp(Mode, GraphModes[j]))
	    break;

    if (i > j) {
	printmsg(NULL, "\nCannot apply demanded operation on graph %s !\n",
	    Graph->Name);
	printmsg(NULL, "    (current synthesis status : \"%s\")\n",
	    CurrentMode);
	return (FALSE);
    }
    else {
	return (TRUE);
    }
}


SetAddressMode(Graph, Mode)
GraphPointer Graph;
char *Mode;
{
    Graph->Attributes = SetAttribute("address_mode", CharNode, Intern(Mode),
	Graph->Attributes);
}

/******************************************************************************/

/*
 * Removes the quotes from a character string
 */

char *
StripQuotes(Identifier)
char *Identifier;
{
    static char Stripped[WORDLENGTH];
    char *Ptr1, *Ptr2;

    for (Ptr1 = Identifier, Ptr2 = Stripped; *Ptr1 != '\0'; Ptr1++) 
	if (*Ptr1 != '"')
	    *(Ptr2++) = *Ptr1;
    *Ptr2 = '\0';
    return (Stripped);
}

char *
AddQuotes(string)
char *string;
{
    /* string output routine - if the string contains weird characters quotes
       will be added */

    static char NewString[128];
    char *ptr;

    if (*string == '\0')
	return (string);

    /* check if non-ascii's */
    if (!(*string >= 'a' && *string <= 'z') &&
        !(*string >= 'A' && *string <= 'Z')) {
        sprintf(NewString, "\"%s\"", string);
        return (NewString);
    }
    for (ptr = string + 1; *ptr != '\0'; ptr++) {
	if (!(*ptr >= 'a' && *ptr <= 'z') &&
	    !(*ptr >= 'A' && *ptr <= 'Z') &&
	    !(*ptr >= '0' && *ptr <= '9') &&
	     (*ptr != '[') &&
	     (*ptr != ']') &&
	     (*ptr != '-') &&
	     (*ptr != '_') &&
	     (*ptr != '#') &&
	     (*ptr != '<') &&
	     (*ptr != '>') &&
	     (*ptr != ',') &&
	     (*ptr != '%')) {
            sprintf(NewString, "\"%s\"", string);
            return (NewString);
	}
    }
    return (string);

}

/* Remove the directorie tree from a file name */

char *
RemoveDirectoryTree(FileName)
char *FileName;
{
    char *Ptr;

    Ptr = rindex(FileName, '/');
    if (Ptr == NULL)
	return (FileName);
    else
	return (++Ptr);
}

/* Upper to lower case transformation */

char *
UpperToLower(Identifier)
char *Identifier;
{
    static char Lower[WORDLENGTH];
    char *Ptr1, *Ptr2;

    for (Ptr1 = Identifier, Ptr2 = Lower; *Ptr1 != '\0'; Ptr1++, Ptr2++) 
	if (*Ptr1 >= 'A' && *Ptr1 <= 'Z')
	    *Ptr2 = *Ptr1 - 'A' + 'a';
	else
	    *Ptr2 = *Ptr1;
    *Ptr2 = '\0';
    return (Intern(Lower));
}

int
IsGraphArgument(Name)
char *Name;
{
    char *Ptr;

    Ptr =  Name + strlen(Name) - 1;

    return ( *Ptr-- == 'h' &&
             *Ptr-- == 'p' &&
             *Ptr-- == 'a' &&
             *Ptr-- == 'r' &&
             *Ptr-- == 'g') ? TRUE : FALSE;
}

/******************************************************************************/

#include <varargs.h>

printmsg(va_alist)
va_dcl
{
    /* Generic error display channel for flowgraph */

    va_list args;
    char *Format, *ErrorHeader, Message[128];

    va_start(args);

    /* print out name of function causing error */
    ErrorHeader = va_arg(args, char *); 
    if (ErrorHeader != NULL) 
        (void) sprintf(Message, "%s : ", ErrorHeader);
    else
	(void) sprintf(Message, "");

    fprintf(stdout, "%s", Message);

    Format = va_arg(args, char *);

    /* print out remainder of message */
    (void) vsprintf(Message, Format, args);

    fprintf(stdout, "%s", Message);

    va_end(args);
    fflush(stdout);
}

SilageReference(SilageAttribute)
ListPointer SilageAttribute;
{
    if (SilageAttribute == NULL)
	return;

    fprintf(stdout, 
	"\tCorresponding Silage Statement: File %s, Line %d, Char %d\n",
	(char *) SilageAttribute->Entry,
	(int) SilageAttribute->Next->Entry,
	(int) SilageAttribute->Next->Next->Entry);
}

/******************************************************************************/

/* 
 * String Hash Routines
 */

static st_table *SymbolTable = NULL;

InitSymbolTable()
{
    int strcmp();

    SymbolTable = st_init_table(strcmp, st_strhash);
    KeyInit();
}


char *
Intern(str)
char *str;
{
    char *value;

    if (!str) return(0);
    if (st_lookup(SymbolTable, str, &value) == 0) {
        value = (char *) malloc(strlen(str)+1);
	strcpy(value, str);
	st_add_direct(SymbolTable, value, value);
    }
    return (value);
}

/*****************************************************************************/
/* 
 * Refer hash Routines
 */

#define REFERSIZE 1024
static LIST ReferTable[REFERSIZE + 1];
int ReferCount = 0;

InitializeReferHash()
{
    register ListPointer p;

    ReferCount = 0;

    for (p = ReferTable; p != ReferTable + REFERSIZE; p++) {
	p->Label = NULL;
	p->Entry = NULL;
	p->Type = NullNode;
    }
}

AddToReferHash(Name, Pointer)
char *Name;
pointer Pointer;
{
    register ListPointer p;

    for (p = ReferTable + ReferHash(Name); 
         p->Label != NULL && strcmp(p->Label, Name) != 0; p++);
    if (p == ReferTable + REFERSIZE)
        for (p = ReferTable; 
             p->Label != NULL && strcmp(p->Label,Name) != 0; p++);
    if (p->Label == NULL) {

        if (++ReferCount == REFERSIZE) {
            printmsg("Parser","Node/Edge table overflow. Please Increase\n");
            exit (-1);
        }

	p->Label = Name;
	p->Entry = Pointer;
    }
}

pointer
CheckFromReferHash(Name)
char *Name;
{
    register ListPointer p;

    for (p = ReferTable + ReferHash(Name); 
         p->Label != NULL && strcmp(p->Label, Name) != 0; p++);
    if (p == ReferTable + REFERSIZE)
        for (p = ReferTable; 
             p->Label != NULL && strcmp(p->Label,Name) != 0; p++);
    if (p->Label == NULL)
	return (NULL);
    else 
	return (p->Entry);
}

int
ReferHash(s)
char *s;

/* Return hashed value for s in range 0 .. StringTableSize-1.  
   StringTableSize must be power of 2.
*/
{
    int Sum = 0;
    for (; *s != NULL;) Sum += *s++;
    return (Sum & (REFERSIZE-1));
}

/*
 * Routines to parse a list structure stored in a character string 
 */

static char LispString[WORDLENGTH*2];
static char *LispList;

InitListEntry(List)
char *List;
{
    strcpy(LispString, List);
    LispList = LispString;
}

int
GetNextListEntry(Entry1, Entry2)
char *Entry1, *Entry2;
{
    char *Ptr, *index();

    Ptr = LispList;

    LispList = index(Ptr, ')');

    if (LispList == NULL)
	return (FALSE);

    *(LispList++) = '\0';

    Ptr = index(Ptr, '(');
    if (Ptr == NULL)
	return (FALSE);

    return ((sscanf(++Ptr, "%s %s", Entry1, Entry2) == 2) ? TRUE : FALSE);
}

char *
UniqueName(String)
char *String;
{
    char Tmp[256];
    int counter = 0;

    while (TRUE) {
        sprintf(Tmp, "%s_%d", String, counter++);
        if (st_lookup(SymbolTable, Tmp, NULL) == 0) 
	    break;
    }
    return (Intern(Tmp));
}

char *
UniqueEdgeName(Edge, List, Prefix)
EdgePointer Edge;
EdgePointer List;
char *Prefix;
{
    EdgePointer E;
    char NewName[WORDLENGTH];

    for (E = List; E != NULL; E = E->Next) {
	if (!strcmp(Edge->Name, E->Name)) {
	    sprintf(NewName, "%s_%s", Prefix, Edge->Name);
	    return (Intern(NewName));
	}
    }
    return (Edge->Name);
}

char *
UniqueNodeName(Node, List, Prefix)
NodePointer Node;
NodePointer List;
char *Prefix;
{
    NodePointer N;
    char NewName[WORDLENGTH];

    for (N = List; N != NULL; N = N->Next) {
	if (!strcmp(Node->Name, N->Name)) {
	    sprintf(NewName, "%s_%s", Prefix, Node->Name);
	    return (Intern(NewName));
	}
    }
    return (Node->Name);
}

char *
UniqueControlName(Control, List, Prefix)
EdgePointer Control;
EdgePointer List;
char *Prefix;
{
    EdgePointer C;
    char NewName[WORDLENGTH];

    for (C = List; C != NULL; C = C->Next) {
	if (!strcmp(Control->Name, C->Name)) {
	    sprintf(NewName, "%s_%s", Prefix, Control->Name);
	    return (Intern(NewName));
	}
    }
    return (Control->Name);
}
