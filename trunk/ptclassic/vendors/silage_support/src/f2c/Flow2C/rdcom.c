/* $Id$ */
/* rdcom.c  */

#define STRINGLENGTH 1000
#define INFINITY 1000000

#include "codegen.h"

extern FILE *ComFD;
extern GraphPointer Root;
extern char *DumpFile;

/* Control Flags */
extern int FFTFlag;
extern int MagnitudeFlag;
extern int HighLevelFlag;
extern int LinearFlag;

extern ListPointer ListOfInputs;
extern ListPointer ListOfDisplays;

char *Intern();
ListPointer SetAttribute(), new_list(), ListAppend();

FILE *OpenFile (FileName)
   char *FileName;
{
   FILE *FD;
   FD = fopen (FileName, "r");
   if (FD == NULL) {
      printmsg(NULL, "Can't open file : %s\n", FileName);
      exit (1);
   }
   return (FD);
}

void ComErr (Lnr)
   int Lnr;
{
   printmsg(NULL, "Error in line %d of .com-file.\n", Lnr);
   exit (1);
}

void ReadInputFile (Lnr)
   int Lnr;
{
   char InpName[STRINGLENGTH];
   char InpFile[STRINGLENGTH];
   int p;
   bool IsConst = false;
   EdgePointer Inedge;
   EdgePointer FindEntry();

   if (fscanf (ComFD, "%s", InpName) != 1)
      ComErr (Lnr);
   if (strcmp (InpName, "FILE"))
      ComErr (Lnr);
   if (fscanf (ComFD, "%s", InpName) != 1)
      ComErr (Lnr);
   if (!strcmp (InpName, "CONSTANT")) {
      IsConst = true;
      if (fscanf (ComFD, "%s", InpName) != 1)
         ComErr (Lnr);
   }
   if (fscanf (ComFD, "%s", InpFile) != 1)
      ComErr (Lnr);
   p = strlen (InpFile) - 1;
   if (InpFile[p] != ';')
      ComErr (Lnr);
   InpFile[p] = '\0';
   Inedge = FindEntry(Root, InpName);
   if (Inedge == NULL)
       ComErr (Lnr);
   if (IsConst)
       Inedge->Attributes = SetAttribute("IsConstant", IntNode, (char *)IsConst,
			Inedge->Attributes);
   Inedge->Attributes = SetAttribute("InFile", CharNode, Intern(InpFile),
			Inedge->Attributes);
   ListOfInputs = ListAppend(new_list(EdgeNode, Inedge), ListOfInputs);
}

void ReadDisplay (Lnr)
   int Lnr;
{
   char FuncName[STRINGLENGTH];
   char SymName[STRINGLENGTH];
   int p;
   EdgePointer Edge;
   EdgePointer FindEntry();

   if (fscanf (ComFD, "%s", FuncName) != 1)
      ComErr (Lnr);
   if (fscanf (ComFD, "%s", SymName) != 1)
      ComErr (Lnr);
   p = strlen (SymName) - 1;
   if (SymName[p] != ';')
      ComErr (Lnr);
   else
      SymName[p] = '\0';


   Edge = FindEntry(Root, SymName);
   if (Edge == NULL)
       ComErr (Lnr);
   Edge->Attributes = SetAttribute("IsDisplay", IntNode, (char *)true,
			Edge->Attributes);
   ListOfDisplays = ListAppend(new_list(EdgeNode, Edge), ListOfDisplays);
}	

void
ReadTimeMode(Lnr)
int Lnr;
{
    char Mode[STRINGLENGTH];
    int p;

    if (fscanf (ComFD, "%s", Mode) != 1)
       ComErr (Lnr);
    p = strlen (Mode) - 1;
    if (Mode[p] != ';')
        ComErr (Lnr);
    else
	Mode[p] = '\0';

    if (!strcmp(Mode, "HIGH_LEVEL"))
	HighLevelFlag = 0;
    else if(!strcmp(Mode, "BIT_TRUE"))
	HighLevelFlag = 1;
    else if(!strcmp(Mode, "ALL"))
	HighLevelFlag = 2;
    else
        ComErr (Lnr);
}	

void
ReadFreqMode(Lnr)
int Lnr;
{
    char Mode[STRINGLENGTH];
    int p, end;

    while (end == 0) {
        if (fscanf (ComFD, "%s", Mode) != 1)
            ComErr (Lnr);
        p = strlen(Mode) - 1;
        if (Mode[p] == ';') {
	    end = 1; 
	    Mode[p] = '\0';
	}
	if (!strcmp(Mode, "LINEAR"))
	    LinearFlag = 1;
	else if (!strcmp(Mode, "LOG"))
	    LinearFlag = 0;
	else if (!strcmp(Mode, "AMPLITUDE"))
	    MagnitudeFlag = 0;
	else if (!strcmp(Mode, "PHASE"))
	    MagnitudeFlag = 1;
	else if (!strcmp(Mode, "AMPL/PHASE"))
	    MagnitudeFlag = 2;
	else
            ComErr (Lnr);
    }
}

ChkIfAllInpFilesDefined (Graph)
   GraphPointer Graph;
{
   NodePointer node;
   EdgePointer edge;
   GraphPointer SubGraph;
   
   for (node = Graph->NodeList; node != NULL; node = node->Next) {
       if (IsHierarchy(node)) {
	    SubGraph = (GraphPointer)node->Master;
	    ChkIfAllInpFilesDefined(SubGraph);
       }

       if (IsInputNode(node)) {
	   edge = (node->OutEdges == NULL) ? EP(node->OutControl) :
                                                EP(node->OutEdges);

           if (GetAttribute(edge->Attributes, "InFile") == NULL) {
               printmsg(NULL, "Input file name missing in .com file for");
               printmsg(NULL, " parameter : %s\n", edge->Name);
               exit (1);
	   }
       }
   }
}

int ReadComFile ()
{
   ListPointer Inputs;
   char token[STRINGLENGTH];
   int StepValue = INFINITY;
   int Lnr = 0;

   while (fscanf (ComFD, "%s", token) == 1) {
      Lnr++;
      if (!strcmp (token, "INPUT"))
	      ReadInputFile (Lnr);
      else if (!strcmp (token, "DISPLAY"))
          ReadDisplay (Lnr);
      else if (!strcmp (token, "TIME")) {
	  ReadTimeMode(Lnr);
      }
      else if (!strcmp (token, "FREQ")) {
	  FFTFlag = 1; 
	  ReadFreqMode(Lnr);
      }
      else if (!strcmp (token, "FREQ;")) {
	  FFTFlag = 1; 
      }
      else if (!strcmp (token, "STEP")) {
	      if (fscanf (ComFD, "%d;", &StepValue) != 1)
              ComErr (Lnr);
          if (StepValue < 1)
	          ComErr (Lnr);
      } 
      else if (!strcmp (token, "GO;"))
	  break;
      else if (token[0] == '#')
          fgets (token, STRINGLENGTH, ComFD);
      else
	  ComErr (Lnr);
   }
   ChkIfAllInpFilesDefined (Root);
   return (StepValue);
}

EdgePointer FindEntry(Graph, Name)
    GraphPointer Graph;
    char *Name;
{
    register EdgePointer edge;
    register NodePointer node;
    char *edgename, *GetArrayName();
    GraphPointer SubGraph;


    for (edge = Graph->EdgeList; edge != NULL; edge = edge->Next) {
	edgename = edge->Name;
/* commented this out because it gives wrong results if
two inputs are say x1 and x. 
Asawaree, 3/6/93 */
        /*if (!strncmp(edgename,Name,strlen(Name))) */
        if (!strcmp(edgename,Name))
	    return(edge);
    }
    for (edge = Graph->ControlList; edge != NULL; edge = edge->Next) {
	edgename = GetArrayName(edge);
        /*if (!strncmp(edgename,Name,strlen(Name))) */
        if (!strncmp(edgename,Name))
	    return(edge);
    }
    for (node = Graph->NodeList; node != NULL; node = node->Next) {
        if (IsHierarchy(node)) {
	    SubGraph = (GraphPointer)node->Master;
	    return(FindEntry(SubGraph, Name));
	}
    }
    return(NULL);
}
