/* $Id$ */

/* Author : Chris Scheers */
/* dpr1.c -- version 1.8 (IMEC)		last updated: 3/1/89 */
static char *SccsId = "@(#)dpr1.c	1.8 (IMEC)	89/03/01";

#include <stdio.h>
#include "sfg.h"

extern SemanticGraph SemGraph;

#define MAXDIMS 100

int OffsetInArray (DimBounds, NrDims, Indices)
   int *DimBounds, NrDims, *Indices;
{
   int i, n;
   n = Indices[0];
   for (i=1; i<NrDims; i++)
      n = n * DimBounds[i] + Indices[i];
   return (n);
}

int NrOfElems (DimBounds, NrDims)
   int *DimBounds, NrDims;
{
   int i, n;
   n = 1;
   for (i=0; i<NrDims; i++)
      n *= DimBounds[i];
   return (n);
}

void AllocArrayInST (ST)
   SymbolTable ST;
{
   int NElems;
#ifdef DEBUG
   if (ST->IsArray == false)
      Crash ("AllocArrayInST");
#endif
   NElems = NrOfElems
	      (ST->SingleOrArray.AS.DimensionBounds,
	       ST->SingleOrArray.AS.NumberOfDimensions);
   ST->SingleOrArray.AS.NumberOfElements = NElems;
   ST->SingleOrArray.AS.IsProduced = (bool *)
      tmalloc (ST->SingleOrArray.AS.NumberOfElements * sizeof (bool));
}

int CopyTupleIndices (Tuple, Indices, i)
   Expression Tuple;
   int *Indices;
   int i;
{
   if (Tuple == NULL)
      return (i);
   if (Tuple->ExpDef.SonsForTuple.FirstExpression == NULL)
      return (i);
   Indices[i] = EvalManExp (Tuple->ExpDef.SonsForTuple.FirstExpression);
   if (Indices[i] < 0)
      perr (42, Tuple->ExpPos);
   return
     (CopyTupleIndices
	(Tuple->ExpDef.SonsForTuple.RestOfTuple, Indices, i+1));
}

void CheckIndexedRHS (Exp, SymDef, Indices)
   Expression Exp;
   SymbolTable SymDef;
   int *Indices;
{
   int Offset;
   Offset = OffsetInArray (SymDef->SingleOrArray.AS.DimensionBounds,
		           SymDef->SingleOrArray.AS.NumberOfDimensions,
			   Indices);
   if (Offset >= SymDef->SingleOrArray.AS.NumberOfElements)
      perr (42, Exp->ExpPos);
   if (SymDef->SingleOrArray.AS.IsProduced[Offset] == false) {
      int i;
      fprintf (stderr, "Value consumed before produced : %s",
	       SymDef->SymbolId.IdName);
      for (i=0; i<SymDef->SingleOrArray.AS.NumberOfDimensions; i++)
         fprintf (stderr, "[%d]", Indices[i]);
      fprintf (stderr, ".\n");
      if (Exp->ExpPos.FileName != NULL) {
	 fprintf (stderr, "File %s, Line %d, (Character %d)\n",
			  Exp->ExpPos.FileName,
			  Exp->ExpPos.LineNum,
			  Exp->ExpPos.CharNum);
      }
      exit (1);
/* error message too short
      perr (41, Exp->ExpPos);
*/
   }
}

void IterateFreeDims (StartOfFreeDims, Exp, SymDef, Indices, CheckFunc)
   int StartOfFreeDims;
   Expression Exp;
   SymbolTable SymDef;
   int *Indices;
   void (*CheckFunc) ();
{
   int i, NrOfDims;
   NrOfDims = SymDef->SingleOrArray.AS.NumberOfDimensions;
   for (i=StartOfFreeDims; i<NrOfDims; i++)
      Indices[i] = 0;
   while (1) {
      CheckFunc (Exp, SymDef, Indices);
      for (i=StartOfFreeDims;
	   Indices[i]+1 == SymDef->SingleOrArray.AS.DimensionBounds[i];
	   i++)
         Indices[i] = 0;
      if (i == NrOfDims)
	 break;
      else
         Indices[i]++;
   }
}

void SimTupleRHS (Exp)
   Expression Exp;
{
   void SimRHS ();
   if (Exp == NULL)
      return;
#ifdef DEBUG
   if (Exp->ExpKind != TupleExp)
      Crash ("SimTupleRHS");
#endif
   SimRHS (Exp->ExpDef.SonsForTuple.FirstExpression);
   SimTupleRHS (Exp->ExpDef.SonsForTuple.RestOfTuple);
}

void SimRHS (Exp)
   Expression Exp;
{
   switch (Exp->ExpKind) {
      case IdentLeafExp :
	 if ((Exp->ExpDef.ThisSymbol->SymKind != Iterator) &&
	     (Exp->ExpDef.ThisSymbol->SingleOrArray.SS.IsProduced == false))
	    perr (41, Exp->ExpPos);
	 break;
      case IndexedExp : {
	 SymbolTable SymDef;
	 int Indices[MAXDIMS], StartOfFreeDims;
	 StartOfFreeDims =
	    CopyTupleIndices
	      (Exp->ExpDef.SonsForIndexed.IndexTupleExp, Indices, 0);
	 SymDef = Exp->ExpDef.SonsForIndexed.ThisArray;
         if (StartOfFreeDims == SymDef->SingleOrArray.AS.NumberOfDimensions)
	    CheckIndexedRHS (Exp, SymDef, Indices);
         else
            IterateFreeDims (StartOfFreeDims, Exp, SymDef, Indices, CheckIndexedRHS);
	 }
	 break;
      case FixedLeafExp :
      case IntegerLeafExp :
      case DelayExp :
      case LpDelayExp :
	 break;
      case CastExp :
	 SimRHS (Exp->ExpDef.CastedExp);
	 break;
      case PlusExp :
      case MinusExp :
      case MultExp :
      case DivExp :
      case MinExp :
      case MaxExp :
      case GreaterExp :
      case SmallerExp :
      case GreaterEqualExp :
      case SmallerEqualExp :
      case EqualExp :
      case NotEqualExp :
      case OrExp :
      case ExorExp :
      case AndExp :
	 SimRHS (Exp->ExpDef.SonsForBinOp.LeftExp);
	 SimRHS (Exp->ExpDef.SonsForBinOp.RightExp);
	 break;
      case CosExp :
      case SinExp :
      case SqrtExp :
      case NotExp :
      case NegateExp :
      case LeftShiftExp :
      case RightShiftExp :
	 SimRHS (Exp->ExpDef.SonsForBinOp.LeftExp);
	 break;
      case FuncAppExp :
         SimTupleRHS (Exp->ExpDef.SonsForFuncApp.ActualParameters);
	 break;
      case UserFuncExp :
         SimTupleRHS (Exp->ExpDef.SonsForUserFunc.ActualUserParameters);
	 break;
      case TimeMuxExp :
	 SimTupleRHS (Exp->ExpDef.SonsForTimeMux.TimeMuxInputs);
	 break;
      case ReSampExp :
	 SimTupleRHS (Exp->ExpDef.SonsForReSamp.ReSampInputs);
	 break;
      case CondExp :
	 SimRHS (Exp->ExpDef.SonsForCond.IfExp);
	 SimRHS (Exp->ExpDef.SonsForCond.ThenExp);
	 if (Exp->ExpDef.SonsForCond.ElseExp != NULL)
	    SimRHS (Exp->ExpDef.SonsForCond.ElseExp);
	 break;
      case TupleExp :
	 SimTupleRHS (Exp);
	 break;
      default :
	 perr (7, Exp->ExpPos);
	 break;
   }
}

void CheckIndexedLHS (Exp, SymDef, Indices)
   Expression Exp;
   SymbolTable SymDef;
   int *Indices;
{
   int Offset;
   Offset = OffsetInArray (SymDef->SingleOrArray.AS.DimensionBounds,
			   SymDef->SingleOrArray.AS.NumberOfDimensions,
			   Indices);
   if (Offset >= SymDef->SingleOrArray.AS.NumberOfElements)
      perr (42, Exp->ExpPos);
   if (SymDef->SingleOrArray.AS.IsProduced[Offset] == true) {
      int i;
      fprintf (stderr, "Single assignment violation : %s",
	       SymDef->SymbolId.IdName);
      for (i=0; i<SymDef->SingleOrArray.AS.NumberOfDimensions; i++)
         fprintf (stderr, "[%d]", Indices[i]);
      fprintf (stderr, ".\n");
      if (Exp->ExpPos.FileName != NULL) {
	 fprintf (stderr, "File %s, Line %d, (Character %d)\n",
			  Exp->ExpPos.FileName,
			  Exp->ExpPos.LineNum,
			  Exp->ExpPos.CharNum);
      }
      exit (1);
   }
/* error message too short
      perr (43, Exp->ExpPos);
*/
   SymDef->SingleOrArray.AS.IsProduced[Offset] = true;
}

void SimTupleLHS (Exp)
   Expression Exp;
{
   void SimLHS ();
   if (Exp == NULL)
      return;
#ifdef DEBUG
   if (Exp->ExpKind != TupleExp)
      Crash ("SimTupleLHS");
#endif
   SimLHS (Exp->ExpDef.SonsForTuple.FirstExpression);
   SimTupleLHS (Exp->ExpDef.SonsForTuple.RestOfTuple);
}

void SimLHS (Exp)
   Expression Exp;
{
   switch (Exp->ExpKind) {
      case IdentLeafExp :
	 if (Exp->ExpDef.ThisSymbol->SingleOrArray.SS.IsProduced == true)
	    perr (43, Exp->ExpPos);
	 Exp->ExpDef.ThisSymbol->SingleOrArray.SS.IsProduced = true;
	 break;
      case IndexedExp : {
	 SymbolTable SymDef;
	 int Indices[MAXDIMS], StartOfFreeDims;
	 StartOfFreeDims =
	    CopyTupleIndices
	      (Exp->ExpDef.SonsForIndexed.IndexTupleExp, Indices, 0);
	 SymDef = Exp->ExpDef.SonsForIndexed.ThisArray;
         if (StartOfFreeDims == SymDef->SingleOrArray.AS.NumberOfDimensions)
	    CheckIndexedLHS (Exp, SymDef, Indices);
         else
            IterateFreeDims (StartOfFreeDims, Exp, SymDef, Indices, CheckIndexedLHS);
	 }
	 break;
      case InitExp :
      case LpInitExp :
	 break;
      case TupleExp :
	 SimTupleLHS (Exp);
	 break;
      default :
	 perr (7, Exp->ExpPos);
	 break;
   }
}

void SimulateEvaluation (VDL)
   ValueDefinitionList VDL;
{
   if (VDL == NULL)
      return;
   switch (VDL->ValDefKind) {
      case SingleDef : {
	 Expression RHS;
	 RHS = VDL->ValDef.SDef->RightHandSide;
	 if ((RHS->ExpKind == TimeMuxExp) &&
	     (RHS->ExpDef.SonsForTimeMux.NrArrayInputs > 0)) {
	    SymbolTable ArrSymDef;
	    Expression TMInputs;
	    TMInputs = RHS->ExpDef.SonsForTimeMux.TimeMuxInputs;
#ifdef DEBUG
            if ((TMInputs->ExpKind != TupleExp) ||
		(TMInputs->ExpDef.SonsForTuple.FirstExpression
		  ->ExpKind != IndexedExp))
               Crash ("SimulateEvaluation 1");
#endif
            ArrSymDef = TMInputs->ExpDef.SonsForTuple.FirstExpression
			  ->ExpDef.SonsForIndexed.ThisArray;
#ifdef DEBUG
	    if (ArrSymDef->SingleOrArray.AS.NumberOfDimensions != 1)
	       Crash ("SimulateEvaluation 2");
#endif
	    if (ArrSymDef->SingleOrArray.AS.DimensionBounds[0] <
		RHS->ExpDef.SonsForTimeMux.NrArrayInputs)
               perr (41, RHS->ExpPos);
         }
	 SimRHS (VDL->ValDef.SDef->RightHandSide);
	 SimLHS (VDL->ValDef.SDef->LeftHandSide);
	 }
	 break;
      case IteratedDef : {
	 IteratedDefinition ID;
	 int LB, UB;
	 ID = VDL->ValDef.IDef;
	 if (ID->IDKind == WhileLoop)
	    perr (7, ID->WhileExp->ExpPos);
	 LB = EvalManExp (ID->LowerBound);
	 UB = EvalManExp (ID->UpperBound);
	 for (ID->ThisIterator->SingleOrArray.SS.ManValue = LB;
	      ID->ThisIterator->SingleOrArray.SS.ManValue <= UB;
	      ID->ThisIterator->SingleOrArray.SS.ManValue++) {
	    SimulateEvaluation (ID->IterValDefs);
         }
	 }
	 break;
#ifdef DEBUG
      default :
	 Crash ("SimulateEvaluation 3");
#endif
   }
   SimulateEvaluation (VDL->Next);
}

void IterSymTab (ST)
   SymbolTable ST;
{
   if (ST == NULL)
      return;
   if (ST->IsArray == true)
      AllocArrayInST (ST);
   IterSymTab (ST->Next);
}

void InitIsProduced (ST, Value)
   SymbolTable ST;
   bool Value;
{
   if (ST == NULL)
      return;
   if (ST->IsArray == true) {
      int i;
      for (i=0; i<ST->SingleOrArray.AS.NumberOfElements; i++)
	 ST->SingleOrArray.AS.IsProduced[i] = Value;
   } else
      ST->SingleOrArray.SS.IsProduced = Value;
   InitIsProduced (ST->Next, Value);
}

void IterDpr1FuncDefList (FDL)
   FunctionDefinitionList FDL;
{
   if (FDL == NULL)
      return;
   if (FDL->IsProcedure == false) {
      IterSymTab (FDL->InputParameters);
      IterSymTab (FDL->OutputParameters);
      IterSymTab (FDL->LocalSymbols);
      InitIsProduced (FDL->InputParameters, true);
      InitIsProduced (FDL->OutputParameters, false);
      InitIsProduced (FDL->LocalSymbols, false);
      SimulateEvaluation (FDL->ValDefs);
   }
   IterDpr1FuncDefList (FDL->OrderedNext);
}

void dpr1 ()
{
   IterDpr1FuncDefList (SemGraph.OrderedFuncDefs);
}
