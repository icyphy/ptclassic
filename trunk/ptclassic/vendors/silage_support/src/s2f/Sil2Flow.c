/* $Id$ */

/* sc.c -- version 1.6 (IMEC)		last updated: 8/28/88 */
static char *SccsId = "@(#)sc.c	1.6 (IMEC)	88/08/28";

#include "sfg.h"
#include "dag.h"

SemanticGraph SemGraph;

#define WORDLENGTH 128
#define WRONG_ARG {printmsg(NULL,"usage : Sil2Flow [-dsfaH] designname\n"); \
	 printmsg(NULL, " -d : Prints result after Silage frontend\n"); \
	 printmsg(NULL, " -s : Performs symbolic evaluation from Silage\n"); \
         printmsg(NULL, " -f : Include leaf cells when dumping graph\n"); \
	 printmsg(NULL, " -a : Dumps AFL flowgraph; default is OCT.\n"); \
	 printmsg(NULL, " -m : Don't Performs multirate transformation\n"); \
	 printmsg(NULL, " -H : Performs flowgraph optimizations for HYPER\n"); \
	 printmsg(NULL, " -M : Performs flowgraph optimizations for McDAS\n"); \
         exit(1);}  

char ProgramName[WORDLENGTH];
int debugFlag = false;          /* debug flag  */
int ForcedFlag = false;    	/* force print out of leaf cells */
int AflFlag = false;
int OctFlag = true;
int RpcFlag = false;
int HYPERflag = false;
int McDASflag = false;
int Multirateflag = true;
int verboseFlag = false;
int castFlag = false;
FILE *LogFile;

main(argc, argv)
int argc;
char *argv[];
{
   char silageFile[WORDLENGTH], *s;
   char DesignName[WORDLENGTH];
   GraphPointer Root, ConstructDAG();
   bool s_flag = false;   /* Symbolic evaluation */

   LogFile = stdout;
   strcpy(ProgramName, argv[0]);

   while (--argc > 0 && (*++argv)[0] == '-') {
      for (s = argv[0]+1; *s !='\0'; s++)
         switch (*s) {
	    case 'd' :
	       debugFlag = true;
	       break;
	    case 'f' :
	       ForcedFlag = true;
	       break;
	    case 's' :
	       s_flag = true;
	       break;
	    case 'a' :
	       AflFlag = true;
	       OctFlag = false;
	       break;
            case 'H' :
               HYPERflag = true;
               break;
            case 'M' :
               McDASflag = true;
               break;
            case 'm' :
               Multirateflag = false;
               break;
            case 'c' :
               castFlag = true;
               break;
	    default:
	       WRONG_ARG;
	 }
   }
   if (argc != 1)
      WRONG_ARG;

/*
 * Set up directory search path
 */
   ReadPath(NULL);
   InitSymbolTable();
   GraphInputInit();
   ParseOctInit();

   sprintf(silageFile, "%s.sil", *argv);
   strcpy(DesignName, *argv);
   SilageParser(silageFile);

   pass1 ();
   pass2 ();
   pass3 ();
   pass4 ();
   pass5 (DesignName);  /* The design can help to determine the main program */
   pass8 ();
   pass6 ();
   pass7 ();

   if (debugFlag == true)
      PrintSFG ();

   dpr2 ();
   if (s_flag == true)
       dpr1 ();
/*   dpr3 ();   /*


/*
 * Construct DAG from Parse tree, print .afl and OCT format
 */
   Root = ConstructDAG();
   Root->Name = *argv;
   if (Multirateflag == true) {
       RemoveAllConstNodes(Root);
       RemoveAllConstCastNodes(Root);
       ClusterProcess(Root);
   }

/* Resolve undefined edges and types before proceeding */
/* First of all, define types on iterators edges       */
   ResolveIteratorTypes(Root);
   if (FlowTypes(Root, TRUE) == TRUE) {
       printmsg(NULL, "\nDumping flowgraph ...\n");
       DumpFlowGraph(Root, FALSE);
       exit (-1);
   }

   if (McDASflag == true) {
      printmsg(NULL, "\nPerforming compiler optimizations ...\n");
/*      RemoveIdentities(Root);
      ManifestExpressions(Root);
      CommonSubExpression(Root);
      DeadCodeElimination(Root);
      PerformGraphCleanUp(Root); */
   }

   if (HYPERflag == true) {
       printmsg(NULL, "translating flowgraph ...\n");
       if (TransformGraph(Root) == FALSE)
           exit (-1);
   }

   if (AflFlag == true) {
       DumpFlowGraph(Root, ForcedFlag);
   }	 
   else {
       if (WriteOctGraph(Root, ForcedFlag) == 0) {
        printmsg("OctWrite", "Errors in generating Oct database for Graph %s\n",
                Root->Name);
        exit (-1);
       }
   }

   ParseOctFree();
   exit (0);
}
