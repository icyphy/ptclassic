/* $Id$ */

/* Author : Chris Scheers */
/* pass1.c -- version 1.7 (IMEC)		last updated: 3/1/89 */
static char *SccsId = "@(#)pass1.c	1.7 (IMEC)	89/03/01";

#include <stdio.h>
#include "sfg.h"

extern NodeType *TheTree;
extern SemanticGraph SemGraph;

FunctionDefinitionList FindFuncDef (FDL, FName)
   FunctionDefinitionList FDL;
   char *FName;
{
   if (FDL == NULL)
      return (NULL);
   if (!strcmp (FName, FDL->FunctionId.IdName))
      return (FDL);
   else
      return (FindFuncDef (FDL->Next, FName));
}

FunctionDefinitionList RmFuncDef (FDL, FDef)
   FunctionDefinitionList FDL, FDef;
{
   if (FDL == FDef)
      return (FDL->Next);
   else {
      FunctionDefinitionList FDIter;
      for (FDIter = FDL;
	   FDIter->Next != FDef;
	   FDIter = FDIter->Next);
      FDIter->Next = FDef->Next;
      return (FDL);
   }
}

Position MkPosition (T)
   NodeType *T;
{
   Position Pos;
   Pos.LineNum = Line (T);
   Pos.CharNum = CharPos (T);
   Pos.FileName = FileFrom (T);
   Pos.kind = T->kind;
   Pos.L = T->L;
   Pos.R = T->R;
   return (Pos);
}

FunctionDefinitionList MkFuncDef (T, NextFDL)
   NodeType *T;
   FunctionDefinitionList NextFDL;
{
   FunctionDefinitionList FDL;
#ifdef DEBUG
   if (Kind (T) != IdentLeaf)
      Crash ("MkFuncDef");
#endif
   FDL = NEW (FunctionDefinition);
   FDL->FunctionId.IdName = (char *) Left (T);
   FDL->FunctionId.IdPos = MkPosition (T);
   FDL->OrderedValDefs = NULL;
   FDL->ListOfFuncApps = NULL;
   FDL->IsRecursive = false;
   FDL->IsProcedure = false;
   FDL->OrderedNext = NULL;
   FDL->Next = NextFDL;
   return (FDL);
}

FunctionDefinitionList MkFuncDefList (T)
   NodeType *T;
{
   NodeType *OuterDefn;
   FunctionDefinitionList FDL;
   if (T == NULL)
      return (NULL);
#ifdef DEBUG
   if (Kind (T) != CommaNode)
      Crash ("MkFuncDefList");
#endif
   OuterDefn = Left (T);
   switch (Kind (OuterDefn)) {
      case DefnNode :
	 perr (1, MkPosition (OuterDefn));
	 break;
      case PragmaNode :
	 return (MkFuncDefList (Right (T)));
	 break;  /* pragma nodes are ignored */
      case FuncDefnNode :
         FDL = MkFuncDef (Left (OuterDefn), MkFuncDefList (Right (T)));
         if (FindFuncDef (FDL->Next, FDL->FunctionId.IdName) != NULL)
            perr (2, FDL->FunctionId.IdPos);
         else
            return (FDL);
         break;
#ifdef DEBUG
      default :
	 Crash ("MkFuncDefList 2");
	 break;
#endif
   }
}

void pass1 ()
{
   SemGraph.FuncDefs = MkFuncDefList (TheTree);
   SemGraph.OrderedFuncDefs = NULL;
   SemGraph.ListOfFixedLeafs = NULL;
}
