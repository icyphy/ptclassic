/*
 * Copyright (C) 1992 Regents of the University of California
 * All rights reserved.
 * $Id$
 */

#include "flowgraph.h"
#define STRINGLENGTH 1000
typedef enum {false, true} bool;

GraphPointer Root = NULL;

FILE *CFD;  /* File descriptor for C output */
FILE *ComFD;  /* File descriptor for .com file */
FILE *LogFile; /* for outputing current status */

char *DumpFile;

/* global and static variables */

char ProgramName[STRINGLENGTH];
char FlowGraphName[STRINGLENGTH];

char *FlowBin;	/* Path to hyper binaries */
char *FlowLib;	/* Path to hyper libraries */
char *FlowInclude;  /* Path to hyper include libraries */

int debugFlag = FALSE;  /* needed by GetPath.c in HyperUtil.a */
int ForcedFlag = FALSE;
int RpcFlag = FALSE;
int verboseFlag = FALSE;
int interactiveFlag = FALSE;
int castFlag = TRUE;

static bool a_flag = false;  /*  write out AFL format */
static bool l_flag = false;  /*  optimize local variable usage */
static bool pl_flag = false;  /*  for generation of .pl file for Ptolemy*/
static bool highlevel = false; /* include highlevel.h and stuff */
static bool bittrue = false; /* include bittrue.h and stuff */

/*
 * global functions 
 */

extern char *getenv();

/*
 * Definitions
 */

#define WRONG_ARG {printmsg(NULL,"usage : Flow2C [-aldmfvphb] file\n"); \
           printmsg(NULL," -a : Work in AFL format\n"); \
           printmsg(NULL," -l : Minimize usage of local variables\n"); \
           printmsg(NULL," -d : Dumps library paths\n"); \
           printmsg(NULL," -m : Interactively requests inputs and outputs\n");\
           printmsg(NULL," -f : Include leaf cells when dumping graph\n"); \
           printmsg(NULL," -v : Verbose mode with -A flag\n"); \
           printmsg(NULL," -p : Generate .pl file for Ptolemy \n"); \
           printmsg(NULL," -h : highlevel simulation for Ptolemy \n"); \
           printmsg(NULL," -b : bittrue simulation for Ptolemy \n"); \
                   exit(1);}  

main(argc, argv)
int argc;
char *argv[];
{
   char CFile[STRINGLENGTH];
   char ComFile[STRINGLENGTH];
   char Dump[STRINGLENGTH];
   char *GraphName, *RemoveVersion(), *Intern();
   FILE *CreateFile (), *OpenFile ();
   GraphPointer ConstructDAG(), ParseOctSubgraph(), GraphInput();

   strcpy(ProgramName, argv[0]);

   /* Get environment variables first */
   if (GetEnvironmentVariables() == FALSE)
      exit (-1);

   /* Check if the program has to be run in interactive format */
   RunInInteractiveMode(&argc, argv);

   ParseArguments(argc, argv); 

/*
 * Set up directory search path
 */
   ReadPath(NULL);
   InitSymbolTable();
   GraphInputInit();
   ParseOctInit();

   GraphName = RemoveVersion(FlowGraphName);
   if(pl_flag) sprintf (CFile,"Silage%s.pl", GraphName);
   else sprintf (CFile,"%s.c", GraphName);
   sprintf (ComFile, "%s.com", GraphName);
   sprintf (Dump, "%s.dmp", FlowGraphName);
   DumpFile = Intern(Dump);

   if (interactiveFlag == FALSE)
       ComFD = OpenFile (ComFile);

   if (debugFlag)
       CFD = stdout;
   else
       CFD = CreateFile (CFile);
   LogFile = stdout;

   if (a_flag == true)
       Root = GraphInput(FlowGraphName, verboseFlag);
   else
       Root = ParseOctSubgraph(FlowGraphName);

   if (Root == NULL)
       WRONG_ARG;

   Root->Name = GraphName; 

/*
 * Translate to C
 */
   FlowToC(l_flag,pl_flag,highlevel,bittrue);
   GenMakefile (GraphName);

/*
 * Dump Graph if asked
 */
 /*  if (a_flag == true)
       DumpFlowGraph(Root, ForcedFlag); */

   ParseOctFree();

   CloseFile (ComFD);
   CloseFile (CFD);
   exit (0);
}

ParseArguments(argc, argv)
int argc;
char **argv;
{
   char *s;

   if(argc > 1) {
       s = argv[argc-1];
       if ( *s != '-') 
	   strcpy(FlowGraphName, argv[argc-1]);
       else
	   WRONG_ARG;
   }
   while (--argc > 0 && (*++argv)[0] == '-') {
      for (s = argv[0]+1; *s !='\0'; s++)
         switch (*s) {
	    case 'v' :
	       verboseFlag = true;
	       break;
	    case 'f' :
	       ForcedFlag = true;
	       break;
	    case 'l' :
	       l_flag = true;
	       break;
	    case 'd' :
	       debugFlag = true;
	       break;
	    case 'm' :
	       interactiveFlag = true;
	       break;
            case 'a' :
	       a_flag = true;
	       break;
            case 'p' :
	       pl_flag = true;
	       fprintf(stderr,"setting pl_flag \n");
	       break;
            case 'h' :
	       highlevel = true;
	       fprintf(stderr,"highlevel simulation \n");
	       break;
            case 'b' :
	       bittrue = true;
	       fprintf(stderr,"bittrue simulation \n");
	       break;
	    default:
	       WRONG_ARG;
	 }
   }

   if (argc != 1)
      WRONG_ARG;
}

myMessage(msg)
char *msg;
{
}

/*****************************************************************************
    GetEnvironmentVariables() -- Routine to find the pathnames to both the
    hyper binary and library directories
 *****************************************************************************/
 
GetEnvironmentVariables()
{
    FlowBin = getenv("FLOWBIN");
    FlowLib = getenv("FLOWLIB");
    FlowInclude = getenv("FLOWINCLUDE");
 
    if (FlowLib == NULL || FlowBin == NULL) {
        fprintf(stderr,
      "xhyper> The environment variables FLOWBIN or FLOWLIB are not defined\n");
        fprintf(stderr,
            "\tPlease run ~hyper/hyperscript first!\n");
        return (FALSE);
    }
    else
        return (TRUE);
}
