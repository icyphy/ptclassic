/* $Id$ */

/* Copyright (C) 1990 Regents of the University of California
 * All rights reserved.
 */

/*
 * Routines to dump the FlowGraph in the standard format
 */

#include "flowgraph.h"
#include "st.h"

/* global functions */
extern int strcmp();
extern char *Intern();
extern char *GetPath();

/* local variables */
static FILE *dumpFile;
static int DumpLeafCells;
static st_table *GraphTable;

DumpFlowGraph(Root, ForcedFlag)
GraphPointer Root;
int ForcedFlag;
{
    char *Path, FileName[WORDLENGTH];

    /* Main Dump Routine */
    /* check if a file with the requested name already exists */

    sprintf(FileName, "%s.afl", Root->Name); 
    Path = GetPath("flowgraph.afl", FileName);
    if (Path != NULL) {  /* Increase the version number */
	FlowVersion(Root);
	sprintf(FileName, "%s.afl", Root->Name);
    }
    dumpFile = fopen(FileName, "w");
    if (dumpFile == NULL) {		/* [erc/8-3-92] so that these
					 * routines will not dump core
					 * because of bad permissions
					 */
      printf("DumpFlowGraph: Could not open file `%s' for writing!\n",
	FileName);
      exit(-1);
    }

    DumpLeafCells = ForcedFlag;

    GraphTable = st_init_table(strcmp, st_strhash);

    DumpGraph(Root);
    fclose(dumpFile);			/* [erc/6-22-92] */

    st_free_table(GraphTable);
}

UpdateFlowGraph(Root, ForcedFlag)
GraphPointer Root;
int ForcedFlag;
{
    /* Identical to DumpFlowGraph but the version-number is not increased */

    char FileName[WORDLENGTH];

    sprintf(FileName, "%s.afl", Root->Name); 
    dumpFile = fopen(FileName, "w");
    if (dumpFile == NULL) {		/* [erc/8-3-92] */
      printf("UpdateFlowGraph: Could not open file `%s' for writing!\n",
	FileName);
      exit(-1);
    }

    DumpLeafCells = ForcedFlag;

    GraphTable = st_init_table(strcmp, st_strhash);

    DumpGraph(Root);
    fclose(dumpFile);			/* [erc/6-22-92] */

    st_free_table(GraphTable);
}


DumpGraph(Graph)
GraphPointer Graph;
{
    NodePointer Node;
    EdgePointer Edge;

    char *Path, *GetPath(), FileName[WORDLENGTH];

    /* Dump each graph at most one time */

    if (Graph == NULL || st_lookup(GraphTable, Graph->Name, NULL) != 0)
	return;
    else
	st_insert(GraphTable, Graph->Name, 0);

    /* Check if we have to include the library leafcells */	
    if (DumpLeafCells == FALSE && !strcmp(Graph->Class, LEAF_NODE)) {
	sprintf(FileName, "%s.afl", Graph->Name);
	Path = GetPath("flowgraph.afl", FileName);
	if (Path != NULL)
	    return;
    }

    OpenList("GRAPH");

    PrintAtom("NAME", CharNode, (pointer) Graph->Name, TRUE);

    PrintAtom("CLASS", CharNode, (pointer) Graph->Class, TRUE);

    if (Graph->Arguments != NULL) {
        OpenList("ARGUMENTS");
	PrintAtom(NULL, ListNode, (pointer) Graph->Arguments, TRUE);
	NewLine();
        CloseList();  /* ARGUMENTS */
    }
    if (Graph->Attributes != NULL) {
        OpenList("ATTRIBUTES");
	PrintAtom(NULL, ListNode, (pointer) Graph->Attributes, TRUE);
	NewLine();
        CloseList();  /* ATTRIBUTES */
    }
    if (Graph->Model != NULL) {
        OpenList("MODEL");
	PrintAtom(NULL, ListNode, (pointer) Graph->Model, TRUE);
	NewLine();
        CloseList();  /* MODEL */
    }

    PrintAtom("IN_EDGES", ListNode, (pointer) Graph->InEdges, TRUE);
    PrintAtom("OUT_EDGES", ListNode, (pointer) Graph->OutEdges, TRUE);
    PrintAtom("IN_CONTROL", ListNode, (pointer) Graph->InControl, TRUE);
    PrintAtom("OUT_CONTROL", ListNode, (pointer) Graph->OutControl, TRUE);

    if (Graph->NodeList != NULL) {
        OpenList("NODELIST");
        for (Node = Graph->NodeList; Node; Node = Node->Next) {
	    OpenList ("NODE");
	    DumpNode(Node);
	    NewLine();
	    CloseList();  /* NODE */
        }
	NewLine();
        CloseList(); /* NODELIST */
    }

    if (Graph->EdgeList != NULL) {
        OpenList("EDGELIST");
        for (Edge = Graph->EdgeList; Edge; Edge = Edge->Next) {
	    OpenList ("EDGE");
	    DumpEdge(Edge);
	    NewLine();
	    CloseList(); /* EDGE */
        }
	NewLine();
        CloseList(); /* EDGELIST */
    }

    if (Graph->ControlList != NULL) {
        OpenList("CONTROLLIST");
        for (Edge = Graph->ControlList; Edge; Edge = Edge->Next) {
	    OpenList ("EDGE");
	    DumpEdge(Edge);
	    NewLine();
	    CloseList(); /* EDGE */
        }
	NewLine();
        CloseList(); /* CONTROLLIST */
    }

    NewLine();
    CloseList(); /* GRAPH */

    /* Dump the SubGraphs */
    for (Node = Graph->NodeList; Node != NULL; Node = Node->Next) 
	DumpGraph(Node->Master);

    NewLine();
}

DumpNode(Node)
NodePointer Node;
{

    PrintAtom("NAME", CharNode, (pointer) Node->Name, TRUE);
    PrintAtom("CLASS", CharNode, (pointer) Node->Class, TRUE);
    PrintAtom("MASTER", CharNode, (pointer) Node->Master->Name, TRUE);

    if (Node->Arguments != NULL) {
        OpenList("ARGUMENTS");
	NewLine();
	PrintIndent();
	PrintAtom(NULL, ListNode, (pointer) Node->Arguments, FALSE);
	NewLine();
        CloseList();  /* ARGUMENTS */
    }
    if (Node->Attributes != NULL) {
        OpenList("ATTRIBUTES");
	NewLine();
	PrintIndent();
	PrintAtom(NULL, ListNode, (pointer) Node->Attributes, FALSE);
	NewLine();
        CloseList();  /* ARGUMENTS */
    }

    PrintAtom("IN_EDGES", ListNode, (pointer) Node->InEdges, TRUE);
    PrintAtom("OUT_EDGES", ListNode, (pointer) Node->OutEdges, TRUE);
    PrintAtom("IN_CONTROL", ListNode, (pointer) Node->InControl, TRUE);
    PrintAtom("OUT_CONTROL", ListNode, (pointer) Node->OutControl, TRUE);

}

DumpEdge(Edge)
EdgePointer Edge;
{

    PrintAtom("NAME", CharNode, (pointer) Edge->Name, TRUE);
    PrintAtom("CLASS", CharNode, (pointer) Edge->Class, TRUE);

    if (Edge->Arguments != NULL) {
        OpenList("ARGUMENTS");
	NewLine();
	PrintIndent();
	PrintAtom(NULL, ListNode, (pointer) Edge->Arguments, FALSE);
	NewLine();
        CloseList();  /* ARGUMENTS */
    }

    if (Edge->Attributes != NULL) {
        OpenList("ATTRIBUTES");
	NewLine();
	PrintIndent();
	PrintAtom(NULL, ListNode, (pointer) Edge->Attributes, FALSE);
	NewLine();
        CloseList();  /* ATTRIBUTES */
    }

    PrintAtom("IN_NODES", ListNode, (pointer) Edge->InNodes, TRUE);
    PrintAtom("OUT_NODES", ListNode, (pointer) Edge->OutNodes, TRUE);
}

/*
 * Print Routines
 */

static int IndexCount = 0;
static char Indent[] = "	";

NewLine()
{
    fprintf(dumpFile, "\n");
}

OpenList(String)
char *String;
{
    NewLine();
    PrintIndent();
    fprintf(dumpFile, "(%s ", String);
    IndexCount++;
}

CloseList()
{
    IndexCount--;
    PrintIndent();
    fprintf(dumpFile, ")");
}

PrintAtom(Label, Type, Pointer, NewLineFlag)
char *Label;
EntryType Type;
pointer Pointer;
int NewLineFlag;
{
    ListPointer Ptr;
    char *AddQuotes();

    if (Pointer == NULL && Type != IntNode && Type != NullNode)
	return;

    if (Label != NULL && *Label != '\0') {
	Label = Intern(AddQuotes(Label));

	if (NewLineFlag) {
	    NewLine();
	    PrintIndent();
	}
	switch (Type) {
        case (IntNode) :
	    fprintf(dumpFile, "(%s %d)", Label, (int) Pointer);
	    break;
	case (RealNode) :
	    fprintf(dumpFile, "(%s %f)", Label, *((double *) Pointer));
	    break;
        case (CharNode) :
	    fprintf(dumpFile, "(%s %s)", Label, AddQuotes((char *) Pointer));
	    break;
        case (ListNode) :
	    fprintf(dumpFile, "(%s (", Label);
	    IndexCount++;
	    for (Ptr = (ListPointer) Pointer; Ptr; Ptr = Ptr->Next)
	        PrintAtom(Ptr->Label, Ptr->Type, Ptr->Entry, NewLineFlag);
	    IndexCount--;
	    fprintf(dumpFile, "))");
	    break;
        case (EdgeNode) :
	    fprintf(dumpFile, "(%s %s)", Label, 
		AddQuotes(((EdgePointer) Pointer)->Name));
	    break;
        case (NodeNode) :
	    fprintf(dumpFile, "(%s %s)", Label, 
		AddQuotes(((NodePointer) Pointer)->Name));
	    break;
	case (GraphNode) :
	    fprintf(dumpFile, "(%s %s)", Label, 
		AddQuotes(((GraphPointer) Pointer)->Name));
	    break;
	case (NullNode) :
	    fprintf(dumpFile, "(%s nil)", Label);
	    break;
        default:
	    break;
	}
    }
    else {
        switch (Type) {
        case (IntNode) :
	    fprintf(dumpFile, "%d ", (int) Pointer);
	    break;
        case (CharNode) :
	    fprintf(dumpFile, "%s ", AddQuotes((char *) Pointer));
	    break;
        case (ListNode) :
	    fprintf(dumpFile, "(");
	    for (Ptr = (ListPointer) Pointer; Ptr; Ptr = Ptr->Next)
	        PrintAtom(Ptr->Label, Ptr->Type, Ptr->Entry, NewLineFlag);
	    fprintf(dumpFile, ")");
	    break;
        case (EdgeNode) :
	    fprintf(dumpFile, "%s ", AddQuotes(((EdgePointer) Pointer)->Name)); 
	    break;
        case (NodeNode) :
	    fprintf(dumpFile, "%s ", AddQuotes(((NodePointer) Pointer)->Name)); 
    	    break;
	case (GraphNode) :
	    fprintf(dumpFile, "parent ");
	    break;
	case (NullNode) :
	    fprintf(dumpFile, "nil ");
        default:
	    break;
	}
    }
}

PrintIndent()
{
    register int i;

    for (i = 0; i < IndexCount; i++)
	fprintf(dumpFile, "%s", Indent);
}

PrintAflHeader(Graph)
GraphPointer Graph;
{
    fprintf(dumpFile, "/* FlowGraph dump for Graph \"%s \"*/\n", Graph->Name);
    fprintf(dumpFile, "/* Generated by program %s 	*/\n", ProgramName);
}

/* 
 * The following are by CMC for debugging purpose, should not affect original
 * program.
 */

CMCDumpFlowGraph(Root, ForcedFlag) /* don't dump master */
GraphPointer Root;
{
    char *Path, FileName[WORDLENGTH];

    sprintf(FileName, "%s.afl", Root->Name); 
    Path = GetPath("flowgraph.afl", FileName);
    if (Path != NULL) {  /* Increase the version number */
        FlowVersion(Root);
        sprintf(FileName, "%s.afl", Root->Name);
    }
    if((dumpFile = fopen(FileName, "w")) == (FILE *) NULL)
      printf("can't open file %s\n", FileName);

    DumpLeafCells = ForcedFlag;

    GraphTable = st_init_table(strcmp, st_strhash);

    CMCDumpGraph(Root);

    st_free_table(GraphTable);
}

CMCDumpGraph(Graph)
GraphPointer Graph;
{
    NodePointer Node;
    EdgePointer Edge;

    char *Path, *GetPath(), FileName[WORDLENGTH];

    if (Graph == NULL || st_lookup(GraphTable, Graph->Name, NULL) != 0)
        return;
    else
        st_insert(GraphTable, Graph->Name, 0);

    NewLine();

    OpenList("GRAPH");

    PrintAtom("NAME", CharNode, (pointer) Graph->Name, TRUE);

    PrintAtom("CLASS", CharNode, (pointer) Graph->Class, TRUE);

    if (Graph->Arguments != NULL) {
        OpenList("ARGUMENTS");
        PrintIndent();
        PrintAtom(NULL, ListNode, (pointer) Graph->Arguments, FALSE);
        NewLine();
        CloseList();  /* ARGUMENTS */
    }
    if (Graph->Attributes != NULL) {
        OpenList("ATTRIBUTES");
        PrintIndent();
        PrintAtom(NULL, ListNode, (pointer) Graph->Attributes, FALSE);
        NewLine();
        CloseList();  /* ATTRIBUTES */
    }
    if (Graph->Model != NULL) {
        OpenList("MODEL");
        PrintIndent();
        PrintAtom(NULL, ListNode, (pointer) Graph->Model, FALSE);
        NewLine();
        CloseList();  /* MODEL */
    }
    PrintAtom("IN_EDGES", ListNode, (pointer) Graph->InEdges, TRUE);
    PrintAtom("OUT_EDGES", ListNode, (pointer) Graph->OutEdges, TRUE);
    PrintAtom("IN_CONTROL", ListNode, (pointer) Graph->InControl, TRUE);
    PrintAtom("OUT_CONTROL", ListNode, (pointer) Graph->OutControl, TRUE);

    if (Graph->NodeList != NULL) {
        OpenList("NODELIST");
        for (Node = Graph->NodeList; Node; Node = Node->Next) {
            OpenList ("NODE");
            DumpNode(Node);
            CloseList();  /* NODE */
        }
        CloseList(); /* NODELIST */
    }

    if (Graph->EdgeList != NULL) {
        OpenList("EDGELIST");
        for (Edge = Graph->EdgeList; Edge; Edge = Edge->Next) {
            OpenList ("EDGE");
            DumpEdge(Edge);
            CloseList(); /* EDGE */
        }
        CloseList(); /* EDGELIST */
    }

    CloseList(); /* GRAPH */
}
