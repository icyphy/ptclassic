/* $Id$ */

/* Author : Chris Scheers */
/* pass5.c -- version 1.7 (IMEC)		last updated: 3/1/89 */
static char *SccsId = "@(#)pass5.c	1.7 (IMEC)	89/03/01";

#include <stdio.h>
#include "sfg.h"
#include "st.h"

extern SemanticGraph SemGraph;
extern FunctionDefinitionList MainInFront ();
extern FunctionDefinitionList FindFuncDef ();
extern FunctionDefinitionList RmFuncDef ();

FunctionDefinitionList ReverseOrderedFuncDefList (FDL)
   FunctionDefinitionList FDL;
{
   FunctionDefinitionList p, q, r;
   p = NULL;
   q = FDL;
   while (q->OrderedNext != q) {
      r = q->OrderedNext;
      q->OrderedNext = p;
      p = q;
      q = r;
   }
   q->OrderedNext = p;
   return (q);
}

void AddToOrderedFuncDefList (FuncDef)
   FunctionDefinitionList FuncDef;
{
   if (SemGraph.OrderedFuncDefs == NULL)
      FuncDef->OrderedNext = FuncDef;
   else
      FuncDef->OrderedNext = SemGraph.OrderedFuncDefs;
   SemGraph.OrderedFuncDefs = FuncDef;
}

void TheseFunctionsFirst (ExpFA)
   Expression ExpFA;
{
   void OrderThisFunction ();
   if (ExpFA == NULL)
      return;
   OrderThisFunction (ExpFA->ExpDef.SonsForFuncApp.ThisFunction);
   TheseFunctionsFirst (ExpFA->ExpDef.SonsForFuncApp.NextFuncApp);
}

void OrderThisFunction (FDL)
   FunctionDefinitionList FDL;
{
   if (FDL->OrderedNext != NULL)
      return;
   if (FDL->IsRecursive == true)
      perr (18, FDL->FunctionId.IdPos);
   FDL->IsRecursive = true;
   TheseFunctionsFirst (FDL->ListOfFuncApps);
   FDL->IsRecursive = false;
   AddToOrderedFuncDefList (FDL);
}

FunctionDefinitionList
MainInFront (FDL, DesignName)
FunctionDefinitionList FDL;
char *DesignName;
{
    FunctionDefinitionList MainFunc;
    Position pos;
    char *RootFunc, *FindRootFunc();

    /* Check first for the "main" function */
    MainFunc = FindFuncDef (FDL, "main");
    if (MainFunc != NULL) {
        FDL = RmFuncDef (FDL, MainFunc);
        MainFunc->Next = FDL;
        return (MainFunc);
    }
    /*
     * If no "main" function is present, look for a function with the
     * same name as the design file 
     */
    MainFunc = FindFuncDef (FDL, DesignName);
    if (MainFunc != NULL) {
        FDL = RmFuncDef (FDL, MainFunc);
        MainFunc->Next = FDL;
        return (MainFunc);
    }

    /*
     * Check if any single function is defined as a root function 
     */
    RootFunc = FindRootFunc(FDL);
    if (RootFunc != NULL) {
	MainFunc =  FindFuncDef (FDL, RootFunc);
        FDL = RmFuncDef (FDL, MainFunc);
        MainFunc->Next = FDL;
        return (MainFunc);
    }

    pos.FileName = NULL;
    perr (3, pos);
}

static st_table *FunctionTable;

char *
FindRootFunc(FDL)
FunctionDefinitionList FDL;
{
    FunctionDefinitionList Ptr;
    int Mark;
    char *Root, *Name, *strcmp();
    st_generator *func_gen;

    FunctionTable = st_init_table(strcmp, st_strhash);

    for (Ptr = FDL; Ptr != NULL; Ptr = Ptr->Next) {
	if (st_lookup(FunctionTable, Ptr->FunctionId.IdName, &Mark) == false) {
	    st_insert(FunctionTable, Ptr->FunctionId.IdName, false);
	    MarkAllFunctions(Ptr);
	}
    }

    /* Check if there is more than one function with only one mark */
    Root = NULL;
    st_foreach_item(FunctionTable, func_gen, &Name, &Mark) {
	if (Mark == true)
	    continue;
	if (Root != NULL) 
	    return (NULL);
	Root = Name;
    }
    st_free_table(FunctionTable);
    return (Root);

}

MarkAllFunctions(FDL)
FunctionDefinitionList FDL;
{
    FunctionDefinitionList Func;
    Expression Ptr;
    int Mark;

    /* Run over all sub-functions and put a mark on them */
    for (Ptr = FDL->ListOfFuncApps; Ptr != NULL; 
	 Ptr = Ptr->ExpDef.SonsForFuncApp.NextFuncApp) {
	Func = Ptr->ExpDef.SonsForFuncApp.ThisFunction;
	if (st_lookup(FunctionTable, Func->FunctionId.IdName, &Mark) == false){
	    st_insert(FunctionTable, Func->FunctionId.IdName, true);
	    MarkAllFunctions(Func);
	}
	else
	    st_insert(FunctionTable, Func->FunctionId, true);
    }
}

void pass5 (DesignName)
char *DesignName;
{
   SemGraph.FuncDefs = MainInFront (SemGraph.FuncDefs, DesignName);
   OrderThisFunction (SemGraph.FuncDefs);
   SemGraph.OrderedFuncDefs =
      ReverseOrderedFuncDefList (SemGraph.OrderedFuncDefs);
}
