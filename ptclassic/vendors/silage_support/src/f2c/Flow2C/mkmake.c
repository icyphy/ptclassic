/* $Id$ */
/* Author : Chris Scheers */
/* mkmake.c -- version 1.11 (IMEC)		last updated: 4/19/89 */
static char *SccsId = "@(#)mkmake.c	1.11 (IMEC)	89/04/19";

#include <stdio.h>
#include "params.h"

extern FILE *CFD;
extern int HighLevelFlag;
extern char FlowGraphName[];

#define makefile "makefile"

FILE *CreateFile (FileName)
   char *FileName;
{
   FILE *FD;
/* Too robust
   FD = fopen (FileName, "r");
   if (FD != NULL) {
      printmsg(NULL, "File exists, not removed : %s\n", FileName);
      exit (1);
   }
*/
   FD = fopen (FileName, "w");
   if (FD == NULL) {
      printmsg(NULL, "Can't open file : %s\n", FileName);
      exit (1);
   }
   return (FD);
}

void CloseFile (FD)
FILE *FD;
{
   fclose (FD);
}

extern char *FlowInclude;
extern char *FlowLib;

void MkMake (BaseName, MFD)
   char *BaseName;
   FILE *MFD;
{
   fprintf (MFD, "BASE = %s\n\n", BaseName); 
   fprintf (MFD, "VERSION = %d\n\n", GetVersion(FlowGraphName)); 
   fprintf (MFD, "FPU = %s\n\n", FPU_OPTION);
   fprintf (MFD, "INC = %s\n\n", FlowInclude);
   fprintf (MFD, "LIB = %s\n\n", FlowLib);
   if (HighLevelFlag == 0)
       fprintf (MFD, "all : clean high\n\n");
   else if (HighLevelFlag == 1) 
       fprintf (MFD, "all : clean bit\n\n");
   else
       fprintf (MFD, "all : clean high bit\n\n");
   fprintf (MFD, "high : $(BASE)H\n");
   fprintf (MFD, "\t@echo high level simulation ...\n");
   fprintf (MFD, "\t@$(BASE)H\n\n");
   fprintf (MFD, "$(BASE)H : $(BASE).c\n");
   fprintf (MFD, "\t@echo compiling ...\n");
   fprintf (MFD, "\t@cc -g -DHIGHLEVEL $(FPU) -I$(INC) $(BASE).c ");
   fprintf (MFD, "$(LIB)/f2cpost.a -lm -o $(BASE)H\n\n");

   fprintf (MFD, "bit : $(BASE)B\n");
   fprintf (MFD, "\t@echo bit true simulation ...\n");
   fprintf (MFD, "\t@$(BASE)B\n\n");
   fprintf (MFD, "$(BASE)B : $(BASE).c\n");
   fprintf (MFD, "\t@echo compiling ...\n");
   fprintf (MFD, "\t@cc -g -DBITTRUE -DDETOV $(FPU) -I$(INC) $(BASE).c ");
   fprintf (MFD, "$(LIB)/f2cpost.a $(LIB)/BIT.a -lm -o $(BASE)B\n\n");
/*
   fprintf (MFD, "bitfast : $(BASE).c\n	");
   fprintf (MFD, "cc -g -DBITFAST $(FPU) -I$(INC) $(BASE).c ");
   fprintf (MFD, "$(LIB)/f2cpost.a $(LIB)/BIT.a -lm -o $(BASE)F\n\n");
*/
   fprintf (MFD, "clean :\n	");

   if (GetVersion(FlowGraphName) == -1)
       fprintf (MFD, "@rm -f $(BASE).time $(BASE).freq $(BASE).phase\n");
    else
       fprintf (MFD,"@rm -f $(BASE)#$(VERSION).time $(BASE)#$(VERSION).freq $(BASE)#$(VERSION).phase\n");
}

void GenMakefile (BaseName)
   char *BaseName;
{
   FILE *MFD;
   MFD = CreateFile (makefile);
   MkMake (BaseName, MFD);
   CloseFile (MFD);
}
