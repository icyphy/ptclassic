/* $Id$ */

/* Author : Chris Scheers */
/* printsfg.c -- version 1.8 (IMEC)		last updated: 3/1/89 */
static char *SccsId = "@(#)printsfg.c	1.8 (IMEC)	89/03/01";

#include <stdio.h>
#include "sfg.h"

extern SemanticGraph SemGraph;

void PrintType (SType)
SymbolType SType;
{
   switch (SType.SymTpKind) {
      case UndefType :
         printf ("UndefType");
	 break;
      case FixedType :
         printf ("fix<%d,%d>", SType.SymTp.FixedTp.w, SType.SymTp.FixedTp.d);
	 break;
      case IntType :
	 printf ("int");
	 break;
      case BoolType :
	 printf ("bool");
	 break;
   }
}

void PrintExp (E)
   Expression E;
{
   bool IsCast;
   if (E == NULL)
     return;
#ifdef DEBUGTYPES
   if ((E->ExpType.SymTpKind != UndefType) &&
       (E->ExpKind != CastExp)) {
      IsCast = true;
      PrintType (E->ExpType);
      printf (" (");
   } else
      IsCast = false;
#endif
   switch (E->ExpKind) {
      case IdentLeafExp :
	 printf ("%s", E->ExpDef.ThisSymbol->SymbolId.IdName);
	 if (E->ExpDef.ThisSymbol->ProcessId != -1)
            printf ("_%d", E->ExpDef.ThisSymbol->ProcessId);
	 break;
      case FixedLeafExp :
	 if (E->ExpDef.SonsForFixedLeaf.FixedString != NULL) {
	    if (E->ExpDef.SonsForFixedLeaf.M < 0)
	       printf ("-");
	    printf ("%s", E->ExpDef.SonsForFixedLeaf.FixedString);
         } else
	    printf ("%f /* %d.%d */",
		    (double) E->ExpDef.SonsForFixedLeaf.M /
		    (double) E->ExpDef.SonsForFixedLeaf.S,
		    E->ExpDef.SonsForFixedLeaf.M,
		    E->ExpDef.SonsForFixedLeaf.S);
         break;
      case IntegerLeafExp :
	 printf ("%d", E->ExpDef.IntegerConstant);
	 break;
      case CastExp :
	 PrintType (E->ExpType);
	 printf (" (");
	 PrintExp (E->ExpDef.CastedExp);
	 printf (")");
	 break;
      case CosExp :
	 printf ("cos (");
	 PrintExp (E->ExpDef.SonsForBinOp.LeftExp);
	 printf (")");
	 break;
      case SinExp :
	 printf ("sin (");
	 PrintExp (E->ExpDef.SonsForBinOp.LeftExp);
	 printf (")");
	 break;
      case SqrtExp :
	 printf ("sqrt (");
	 PrintExp (E->ExpDef.SonsForBinOp.LeftExp);
	 printf (")");
	 break;
      case MinExp :
	 printf ("min (");
	 PrintExp (E->ExpDef.SonsForBinOp.LeftExp);
	 printf (",");
	 PrintExp (E->ExpDef.SonsForBinOp.RightExp);
	 printf (")");
	 break;
      case MaxExp :
	 printf ("max (");
	 PrintExp (E->ExpDef.SonsForBinOp.LeftExp);
	 printf (",");
	 PrintExp (E->ExpDef.SonsForBinOp.RightExp);
	 printf (")");
	 break;
      case NegateExp :
	 printf ("(");
	 printf ("- ");
	 PrintExp (E->ExpDef.SonsForBinOp.LeftExp);
	 printf (")");
	 break;
      case PlusExp :
	 printf ("(");
	 PrintExp (E->ExpDef.SonsForBinOp.LeftExp);
	 printf (" + ");
	 PrintExp (E->ExpDef.SonsForBinOp.RightExp);
	 printf (")");
	 break;
      case MinusExp :
	 printf ("(");
	 PrintExp (E->ExpDef.SonsForBinOp.LeftExp);
	 printf (" - ");
	 PrintExp (E->ExpDef.SonsForBinOp.RightExp);
	 printf (")");
	 break;
      case MultExp :
	 printf ("(");
	 PrintExp (E->ExpDef.SonsForBinOp.LeftExp);
	 printf (" * ");
	 PrintExp (E->ExpDef.SonsForBinOp.RightExp);
	 printf (")");
	 break;
      case DivExp :
	 printf ("(");
	 PrintExp (E->ExpDef.SonsForBinOp.LeftExp);
	 printf (" / ");
	 PrintExp (E->ExpDef.SonsForBinOp.RightExp);
	 printf (")");
	 break;
      case LeftShiftExp :
	 printf ("(");
	 PrintExp (E->ExpDef.SonsForBinOp.LeftExp);
	 printf (" << ");
	 printf ("(");
	 PrintExp (E->ExpDef.SonsForBinOp.RightExp);
	 printf ("))");
	 break;
      case RightShiftExp :
	 printf ("(");
	 PrintExp (E->ExpDef.SonsForBinOp.LeftExp);
	 printf (" >> ");
	 printf ("(");
	 PrintExp (E->ExpDef.SonsForBinOp.RightExp);
	 printf ("))");
	 break;
      case DelayExp :
      case LpDelayExp :
	 PrintExp (E->ExpDef.SonsForDelay.DelaySigExp);
	 if (E->ExpKind == DelayExp)
	     printf ("@");
	 else
	     printf ("#");
	 if ((E->ExpDef.SonsForDelay.DelayValExp->ExpKind !=
	      IntegerLeafExp) &&
	     (E->ExpDef.SonsForDelay.DelayValExp->ExpKind !=
	      IdentLeafExp))
            printf ("(");
	 PrintExp (E->ExpDef.SonsForDelay.DelayValExp);
	 if ((E->ExpDef.SonsForDelay.DelayValExp->ExpKind !=
	      IntegerLeafExp) &&
	     (E->ExpDef.SonsForDelay.DelayValExp->ExpKind !=
	      IdentLeafExp))
	    printf (")");
         break;
      case InitExp :
      case LpInitExp :
	 PrintExp (E->ExpDef.SonsForDelay.DelaySigExp);
	 if (E->ExpKind == InitExp)
	     printf ("@@");
	 else
	     printf ("##");
	 if ((E->ExpDef.SonsForDelay.DelayValExp->ExpKind !=
	      IntegerLeafExp) &&
	     (E->ExpDef.SonsForDelay.DelayValExp->ExpKind !=
	      IdentLeafExp))
            printf ("(");
	 PrintExp (E->ExpDef.SonsForDelay.DelayValExp);
	 if ((E->ExpDef.SonsForDelay.DelayValExp->ExpKind !=
	      IntegerLeafExp) &&
	     (E->ExpDef.SonsForDelay.DelayValExp->ExpKind !=
	      IdentLeafExp))
	    printf (")");
	 break;
      case FuncAppExp :
	 printf ("%s ", E->ExpDef.SonsForFuncApp.ThisFunction
			 ->FunctionId.IdName);
#ifdef DEBUG
	 if (E->ExpDef.SonsForFuncApp.NrOfApps > 0)
	    printf ("{%d} ", E->ExpDef.SonsForFuncApp.NrOfApps);
#endif
	 printf ("(");
         E = E->ExpDef.SonsForFuncApp.ActualParameters;
         do {
	    PrintExp (E->ExpDef.SonsForTuple.FirstExpression);
	    E = E->ExpDef.SonsForTuple.RestOfTuple;
	    if (E != NULL)
	       printf (", ");
	 } while (E != NULL);
	 printf (")");
	 break;
      case UserFuncExp :
	 printf ("%s (", E->ExpDef.SonsForUserFunc.UserFuncId.IdName);
	 E = E->ExpDef.SonsForUserFunc.ActualUserParameters;
         do {
	    PrintExp (E->ExpDef.SonsForTuple.FirstExpression);
	    E = E->ExpDef.SonsForTuple.RestOfTuple;
	    if (E != NULL)
	       printf (", ");
	 } while (E != NULL);
	 printf (")");
	 break;
      case TimeMuxExp : {
         int NoPhase;
         NoPhase = E->ExpDef.SonsForTimeMux.NoPhase;
	 if (NoPhase == -1)
	     printf ("timemux (");
	 else
	     printf ("timedemux (");
	 E = E->ExpDef.SonsForTimeMux.TimeMuxInputs;
         do {
	    PrintExp (E->ExpDef.SonsForTuple.FirstExpression);
	    E = E->ExpDef.SonsForTuple.RestOfTuple;
	    if (E != NULL)
	       printf (", ");
	 } while (E != NULL);
	 if (NoPhase)
	     printf(", NoPhase");
	 printf (")");
         }
	 break;
      case ReSampExp : {
         int Scale, Phase;
	 switch (E->ExpDef.SonsForReSamp.Type) {
	     case UpSampleNode : printf ("upsample ("); break;
	     case DownSampleNode : printf ("downsample ("); break;
	     case InterpolateNode : printf ("interpolate ("); break;
	     case DecimateNode : printf ("decimate ("); break;
	     default : break;
	 }   
         Scale = E->ExpDef.SonsForReSamp.Scale;
         Phase = E->ExpDef.SonsForReSamp.Phase;
	 E = E->ExpDef.SonsForReSamp.ReSampInputs;
         do {
	    PrintExp (E->ExpDef.SonsForTuple.FirstExpression);
	    E = E->ExpDef.SonsForTuple.RestOfTuple;
	    if (E != NULL)
	       printf (", ");
	 } while (E != NULL);
	 printf(", %d, %d", Scale, Phase);
	 printf (")");
         }
	 break;
      case TupleExp :
	 printf ("( ");
         do {
	    PrintExp (E->ExpDef.SonsForTuple.FirstExpression);
	    E = E->ExpDef.SonsForTuple.RestOfTuple;
	    if (E != NULL)
	       printf (", ");
	 } while (E != NULL);
	 printf (" )");
	 break;
      case IndexedExp : {
	 Expression IndList;
	 printf ("%s", E->ExpDef.SonsForIndexed.ThisArray->SymbolId.IdName);
	 if (E->ExpDef.SonsForIndexed.ThisArray->ProcessId != -1)
            printf ("_%d", E->ExpDef.SonsForIndexed.ThisArray->ProcessId);
	 if (E->ExpDef.SonsForIndexed.ThisArray->SingleOrArray.AS.InPlace == false) 
            for (IndList = E->ExpDef.SonsForIndexed.IndexTupleExp;
	         IndList != NULL;
	         IndList = IndList->ExpDef.SonsForTuple.RestOfTuple) {
	       printf ("[");
	       if (IndList->ExpDef.SonsForTuple.FirstExpression != NULL)
	          PrintExp (IndList->ExpDef.SonsForTuple.FirstExpression);
	       printf ("]");
            }
	 }
	 break;
      case CondExp :
	 printf ("if (");
	 PrintExp (E->ExpDef.SonsForCond.IfExp);
         printf (") -> ");
	 PrintExp (E->ExpDef.SonsForCond.ThenExp);
	 if (E->ExpDef.SonsForCond.ElseExp != NULL) {
	    printf (" || ");
	    PrintExp (E->ExpDef.SonsForCond.ElseExp);
	 }
	 printf (" fi");
	 break;
      case GreaterExp :
	 PrintExp (E->ExpDef.SonsForBinOp.LeftExp);
	 printf (" > ");
	 PrintExp (E->ExpDef.SonsForBinOp.RightExp);
	 break;
      case SmallerExp :
	 PrintExp (E->ExpDef.SonsForBinOp.LeftExp);
	 printf (" < ");
	 PrintExp (E->ExpDef.SonsForBinOp.RightExp);
	 break;
      case GreaterEqualExp :
	 PrintExp (E->ExpDef.SonsForBinOp.LeftExp);
	 printf (" >= ");
	 PrintExp (E->ExpDef.SonsForBinOp.RightExp);
	 break;
      case SmallerEqualExp :
	 PrintExp (E->ExpDef.SonsForBinOp.LeftExp);
	 printf (" <= ");
	 PrintExp (E->ExpDef.SonsForBinOp.RightExp);
	 break;
      case EqualExp :
	 PrintExp (E->ExpDef.SonsForBinOp.LeftExp);
	 printf (" == ");
	 PrintExp (E->ExpDef.SonsForBinOp.RightExp);
	 break;
      case NotEqualExp :
	 PrintExp (E->ExpDef.SonsForBinOp.LeftExp);
	 printf (" != ");
	 PrintExp (E->ExpDef.SonsForBinOp.RightExp);
	 break;
      case OrExp :
	 PrintExp (E->ExpDef.SonsForBinOp.LeftExp);
	 printf (" | ");
	 PrintExp (E->ExpDef.SonsForBinOp.RightExp);
	 break;
      case ExorExp :
	 PrintExp (E->ExpDef.SonsForBinOp.LeftExp);
	 printf (" ^ ");
	 PrintExp (E->ExpDef.SonsForBinOp.RightExp);
	 break;
      case AndExp :
	 PrintExp (E->ExpDef.SonsForBinOp.LeftExp);
	 printf (" & ");
	 PrintExp (E->ExpDef.SonsForBinOp.RightExp);
	 break;
      case NotExp :
	 printf ("! ");
	 PrintExp (E->ExpDef.SonsForBinOp.LeftExp);
	 break;
      default :
	 printf ("Can't print this expression. %d\n", E->ExpKind);
	 break;
   }
#ifdef DEBUGTYPES
   if (IsCast == true)
      printf (")");
#endif
}

void PrintParams (ST)
   SymbolTable ST;
{
   int i;
   if (ST == NULL)
      return;
   printf ("%s", ST->SymbolId.IdName);
   if (ST->ProcessId != -1)
      printf ("_%d", ST->ProcessId);
   printf (" : ");
   PrintType (ST->SymType);
   if (ST->IsArray)
      for (i=0; i<ST->SingleOrArray.AS.NumberOfDimensions; i++) {
         printf ("[");
         if (ST->SingleOrArray.AS.DimensionBounds != NULL)
	    printf ("%d", ST->SingleOrArray.AS.DimensionBounds[i]);
         printf ("]");
      }
   if (ST->Next != NULL)
      printf ("; ");
   PrintParams (ST->Next);
}

void Blancs (n)
   int n;
{
   int i;
   for (i=0; i<n; i++)
      printf (" ");
}

void PrintSDef (SD)
   SingleDefinition SD;
{
   if (SD == NULL)
      printf ("Undefined");
   else {
      PrintExp (SD->LeftHandSide);
      printf (" = ");
      PrintExp (SD->RightHandSide);
   }
}

void PrintLocals (VDL, indent)
   ValueDefinitionList VDL;
   int indent;
{
   if (VDL == NULL)
      return;
   Blancs (indent);
   if (VDL->ValDefKind == SingleDef) {
      PrintSDef (VDL->ValDef.SDef);
   } else {
      if (VDL->ValDef.IDef->IDKind == ForLoop) {
         printf ("(%s : ", VDL->ValDef.IDef->ThisIterator->
			   SymbolId.IdName);
         PrintExp (VDL->ValDef.IDef->LowerBound);
         printf (" .. ");
         PrintExp (VDL->ValDef.IDef->UpperBound);
         printf (") ::");
#ifdef DEBUG
         if (VDL->ValDef.IDef->NestingUp != NULL)
	    printf (" NestingUp : %s", VDL->ValDef.IDef->
		    NestingUp->ThisIterator->SymbolId.IdName);
#endif
         printf ("\n");
         Blancs (indent + 1); printf ("begin\n");
         PrintLocals (VDL->ValDef.IDef->IterValDefs, indent + 3);
         Blancs (indent + 1); printf ("end");
      } else if (VDL->ValDef.IDef->IDKind == WhileLoop) {
	 printf ("while ");
	 PrintExp (VDL->ValDef.IDef->WhileExp);
	 printf (" do\n");
         Blancs (indent); printf ("begin\n");
         PrintLocals (VDL->ValDef.IDef->IterValDefs, indent + 3);
         Blancs (indent); printf ("end");
      } else {
	 printf ("do\n");
         Blancs (indent); printf ("begin\n");
         PrintLocals (VDL->ValDef.IDef->IterValDefs, indent + 3);
         Blancs (indent); printf ("exit condition:\n");
         Blancs (indent); PrintSDef (VDL->ValDef.IDef->ExitDef); printf(";\n");
         Blancs (indent); printf ("end");
      }
   }
   printf (";\n");
   PrintLocals (VDL->Next, indent);
}

void PrintIndexedList (IDL)
   IndexedDefinitionList IDL;
{
   if (IDL == NULL)
      return;
   printf ("     ");
   PrintSDef (IDL->IndexedDef);
   printf (";");
#ifdef DEBUG
   if (IDL->ClosestIteration != NULL)
      printf (" ClosestIteration : %s", IDL->ClosestIteration->
	      ThisIterator->SymbolId.IdName);
#endif
   printf ("\n");
   PrintIndexedList (IDL->Next);
}

void PrintSymTab (ST)
   SymbolTable ST;
{
   if (ST == NULL)
      return;
   if (ST->IsArray == false) {
      printf ("   %s : Single ", ST->SymbolId.IdName);
      if (ST->SymKind != Iterator) {
	 printf ("(");
         PrintType (ST->SymType);
         printf (") ");
      }
      printf (": ");
      if (ST->MaxDelay > 0)
	 printf ("{%d} ", ST->MaxDelay);
      if (ST->SymKind == Iterator)
	 printf ("Iterator");
      else
         PrintSDef (ST->SingleOrArray.SS.SSDef);
      printf (";\n");
   } else {
      int i;
      printf ("   %s : Array", ST->SymbolId.IdName);
      for (i=0; i<ST->SingleOrArray.AS.NumberOfDimensions; i++) {
	 printf ("[");
	 if (ST->SingleOrArray.AS.DimensionBounds != NULL)
	    printf ("%d", ST->SingleOrArray.AS.DimensionBounds[i]);
         printf ("]");
      }
      printf (" (");
      PrintType (ST->SymType);
      printf (") :");
      if (ST->MaxDelay > 0)
	 printf (" {%d}", ST->MaxDelay);
      if (ST->SingleOrArray.AS.ASDef == NULL)
	 printf (" Undefined");
      printf ("\n");
      PrintIndexedList (ST->SingleOrArray.AS.ASDef);
   }
   PrintSymTab (ST->Next);
}

void PrintFuncApps (LOFA)
   Expression LOFA;
{
   if (LOFA == NULL)
      return;
   printf ("%s", LOFA->ExpDef.SonsForFuncApp.ThisFunction
		 ->FunctionId.IdName);
   if (LOFA->ExpDef.SonsForFuncApp.NextFuncApp != NULL)
      printf (", ");
   PrintFuncApps (LOFA->ExpDef.SonsForFuncApp.NextFuncApp);
}

void PrintFuncDefs (FDL)
   FunctionDefinitionList FDL;
{
   if (FDL == NULL)
      return;
   printf ("func %s (", FDL->FunctionId.IdName);
   PrintParams (FDL->InputParameters);
   printf (") ");
   PrintParams (FDL->OutputParameters);
   printf (" =\n");
#ifdef DEBUG
   printf ("Func Apps : ");
   PrintFuncApps (FDL->ListOfFuncApps);
   printf (";\n");
   if (FDL->DelaysExist == true)
      printf ("Delays exist.\n");
   else
      printf ("No Delays.\n");
   printf ("Output parameters :\n");
   PrintSymTab (FDL->OutputParameters);
   printf ("Local symbols :\n");
   PrintSymTab (FDL->LocalSymbols);
#endif
   printf ("begin\n");
   PrintLocals (FDL->ValDefs, 3);
   printf ("end;\n\n");
   PrintFuncDefs (FDL->OrderedNext);
}

void PrintListOfFixedLeafs (LOFL)
   Expression LOFL;
{
   if (LOFL == NULL)
      return;
   PrintExp (LOFL);
   if (LOFL->ExpDef.SonsForFixedLeaf.NextFixed != NULL)
      printf (", ");
   PrintListOfFixedLeafs (LOFL->ExpDef.SonsForFixedLeaf.NextFixed);
}

void PrintSFG ()
{
#ifdef DEBUG
   printf ("Fixed Leafs : ");
   PrintListOfFixedLeafs (SemGraph.ListOfFixedLeafs);
   printf (";\n\n");
#endif
   PrintFuncDefs (SemGraph.OrderedFuncDefs);
}
