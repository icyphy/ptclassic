/* $Id$ */

/* Author : Chris Scheers */
/* pass8.c -- version 1.9 (IMEC)		last updated: 3/1/89 */
static char *SccsId = "@(#)pass8.c	1.9 (IMEC)	89/03/01";

#include <stdio.h>
#include "sfg.h"

extern SemanticGraph SemGraph;
extern bool NoIntsInFDL ();

bool NoIterations (VDL)
   ValueDefinitionList VDL;
{
   if (VDL == NULL)
      return (true);
   if (VDL->ValDefKind == IteratedDef)
      return (false);
   else
      return (NoIterations (VDL->Next));
}

bool NoArraysInTable (ST)
   SymbolTable ST;
{
   if (ST == NULL)
      return (true);
   if (ST->IsArray == true)
      return (false);
   else
      return (NoArraysInTable (ST->Next));
}

bool NoArraysInFDL (FDL)
   FunctionDefinitionList FDL;
{
   if (NoArraysInTable (FDL->InputParameters) == false)
      return (false);
   if (NoArraysInTable (FDL->OutputParameters) == false)
      return (false);
   if (NoArraysInTable (FDL->LocalSymbols) == false)
      return (false);
   else
      return (true);
}

ValueDefinitionList ReverseOrderedValDefList (VDL)
   ValueDefinitionList VDL;
{
   ValueDefinitionList p, q, r;
   p = NULL;
   q = VDL;
   while (q->OrderedNext != q) {
      r = q->OrderedNext;
      q->OrderedNext = p;
      p = q;
      q = r;
   }
   q->OrderedNext = p;
   return (q);
}

void AddToOrderedValDefList (ValDef, FDL)
   ValueDefinitionList ValDef;
   FunctionDefinitionList FDL;
{
   if (FDL->OrderedValDefs == NULL)
      ValDef->OrderedNext = ValDef;
   else
      ValDef->OrderedNext = FDL->OrderedValDefs;
   FDL->OrderedValDefs = ValDef;
}

bool IsDefinedInTuple (SymDef, TupleLHS)
   SymbolTable SymDef;
   Expression TupleLHS;
{
   Expression FirstExp, RestExp;
   if (TupleLHS == NULL)
      return (false);
#ifdef DEBUG
   if (TupleLHS->ExpKind != TupleExp)
      Crash ("IsDefinedInTuple 1");
#endif
   FirstExp = TupleLHS->ExpDef.SonsForTuple.FirstExpression;
   RestExp = TupleLHS->ExpDef.SonsForTuple.RestOfTuple;
#ifdef DEBUG
   if (FirstExp->ExpKind != IdentLeafExp)
      Crash ("IsDefinedInTuple 2");
#endif
   if (FirstExp->ExpDef.ThisSymbol == SymDef)
      return (true);
   else
      return (IsDefinedInTuple (SymDef, RestExp));
}

bool IsDefinedInVDL (SymDef, VDL)
   SymbolTable SymDef;
   ValueDefinitionList VDL;
{
   Expression LHS;
   if (VDL == NULL)
      return (false);
#ifdef DEBUG
   if (VDL->ValDefKind != SingleDef)
      Crash ("IsDefinedInVDL");
#endif
   LHS = VDL->ValDef.SDef->LeftHandSide;
   switch (LHS->ExpKind) {
      case IdentLeafExp :
         if (LHS->ExpDef.ThisSymbol == SymDef)
            return (true);
         else
            return (IsDefinedInVDL (SymDef, VDL->Next));
	 break;
      case InitExp :
      case LpInitExp :
         return (IsDefinedInVDL (SymDef, VDL->Next));
	 break;
      case TupleExp :
	 if (IsDefinedInTuple (SymDef, LHS) == true)
	    return (true);
         else
            return (IsDefinedInVDL (SymDef, VDL->Next));
	 break;
   }
}

ValueDefinitionList FindValDef (SymDef, VDL)
   SymbolTable SymDef;
   ValueDefinitionList VDL;
{
   Expression LHS;
#ifdef DEBUG
   if ((VDL == NULL) ||
       (VDL->ValDefKind == IteratedDef))
      Crash ("FindValDef");
#endif
   LHS = VDL->ValDef.SDef->LeftHandSide;
   switch (LHS->ExpKind) {
      case IdentLeafExp :
         if (LHS->ExpDef.ThisSymbol == SymDef)
            return (VDL);
         else
            return (FindValDef (SymDef, VDL->Next));
	 break;
      case InitExp :
      case LpInitExp :
         return (FindValDef (SymDef, VDL->Next));
	 break;
      case TupleExp :
	 if (IsDefinedInTuple (SymDef, LHS) == true)
	    return (VDL);
         else
            return (FindValDef (SymDef, VDL->Next));
	 break;
   }
}

void ThisExpressionFirst (RHS, FDL)
   Expression RHS;
   FunctionDefinitionList FDL;
{
   void OrderThisDefinition ();
   switch (RHS->ExpKind) {
      case IdentLeafExp :
	 if (RHS->ExpDef.ThisSymbol->SymKind != InPar)
	    OrderThisDefinition
	      (FindValDef
	        (RHS->ExpDef.ThisSymbol, FDL->ValDefs),
               FDL);
	 break;
      case IntegerLeafExp :
      case FixedLeafExp :
      case DelayExp :
      case LpDelayExp :
	 break;
      case CastExp :
	 ThisExpressionFirst (RHS->ExpDef.CastedExp, FDL);
	 break;
      case NegateExp :
      case CosExp :
      case SinExp :
      case SqrtExp :
      case LeftShiftExp :
      case RightShiftExp :
      case NotExp :
	 ThisExpressionFirst (RHS->ExpDef.SonsForBinOp.LeftExp, FDL);
	 break;
      case PlusExp :
      case MinusExp :
      case MultExp :
      case DivExp :
      case MinExp :
      case MaxExp :
      case OrExp :
      case ExorExp :
      case AndExp :
      case GreaterExp :
      case SmallerExp :
      case GreaterEqualExp :
      case SmallerEqualExp :
      case EqualExp :
      case NotEqualExp :
	 ThisExpressionFirst (RHS->ExpDef.SonsForBinOp.LeftExp, FDL);
	 ThisExpressionFirst (RHS->ExpDef.SonsForBinOp.RightExp, FDL);
	 break;
      case FuncAppExp :
	 ThisExpressionFirst (RHS->ExpDef.SonsForFuncApp.ActualParameters, FDL);
	 break;
      case UserFuncExp :
	 ThisExpressionFirst (RHS->ExpDef.SonsForUserFunc.ActualUserParameters, FDL);
	 break;
      case TimeMuxExp :
	 ThisExpressionFirst (RHS->ExpDef.SonsForTimeMux.TimeMuxInputs, FDL);
	 break;
      case ReSampExp :
	 ThisExpressionFirst (RHS->ExpDef.SonsForReSamp.ReSampInputs, FDL);
	 break;
      case TupleExp :
	 ThisExpressionFirst
	   (RHS->ExpDef.SonsForTuple.FirstExpression, FDL);
         if (RHS->ExpDef.SonsForTuple.RestOfTuple != NULL)
	    ThisExpressionFirst
	      (RHS->ExpDef.SonsForTuple.RestOfTuple, FDL);
	 break;
      case CondExp :
	 ThisExpressionFirst (RHS->ExpDef.SonsForCond.IfExp, FDL);
	 ThisExpressionFirst (RHS->ExpDef.SonsForCond.ThenExp, FDL);
	 if (RHS->ExpDef.SonsForCond.ElseExp != NULL)
	    ThisExpressionFirst (RHS->ExpDef.SonsForCond.ElseExp, FDL);
	 break;
      default :
	 break;
   }
}

void CheckTupleCyclic (TupleLHS)
   Expression TupleLHS;
{
   Expression FirstExp, RestExp;
   if (TupleLHS == NULL)
      return;
#ifdef DEBUG
   if (TupleLHS->ExpKind != TupleExp)
      Crash ("CheckTupleCyclic 1");
#endif
   FirstExp = TupleLHS->ExpDef.SonsForTuple.FirstExpression;
   RestExp = TupleLHS->ExpDef.SonsForTuple.RestOfTuple;
#ifdef DEBUG
   if (FirstExp->ExpKind != IdentLeafExp)
      Crash ("CheckTupleCyclic 2");
#endif
   if (FirstExp->ExpDef.ThisSymbol->IsCyclic == true)
      perr (19, FirstExp->ExpDef.ThisSymbol->SymbolId.IdPos);
   CheckTupleCyclic (RestExp);
}

void MakeTupleCyclic (TupleLHS)
   Expression TupleLHS;
{
   if (TupleLHS == NULL)
      return;
   TupleLHS->ExpDef.SonsForTuple.FirstExpression
     ->ExpDef.ThisSymbol->IsCyclic = true;
   MakeTupleCyclic (TupleLHS->ExpDef.SonsForTuple.RestOfTuple);
}

void RemoveTupleCyclic (TupleLHS)
   Expression TupleLHS;
{
   if (TupleLHS == NULL)
      return;
   TupleLHS->ExpDef.SonsForTuple.FirstExpression
     ->ExpDef.ThisSymbol->IsCyclic = false;
   RemoveTupleCyclic (TupleLHS->ExpDef.SonsForTuple.RestOfTuple);
}

void OrderThisDefinition (VDL, FDL)
   ValueDefinitionList VDL;
   FunctionDefinitionList FDL;
{
   Expression LHS;
   if (VDL->OrderedNext != NULL)
      return;
   LHS = VDL->ValDef.SDef->LeftHandSide;
   switch (LHS->ExpKind) {
      case IdentLeafExp :
	 if (LHS->ExpDef.ThisSymbol->IsCyclic == true)
            perr (19, LHS->ExpDef.ThisSymbol
			 ->SymbolId.IdPos);
         LHS->ExpDef.ThisSymbol->IsCyclic = true;
	 ThisExpressionFirst (VDL->ValDef.SDef->RightHandSide, FDL);
         LHS->ExpDef.ThisSymbol->IsCyclic = false;
	 AddToOrderedValDefList (VDL, FDL);
	 break;
      case InitExp :
      case LpInitExp :
	 AddToOrderedValDefList (VDL, FDL);
	 break;
      case TupleExp :
	 CheckTupleCyclic (LHS);
	 MakeTupleCyclic (LHS);
	 ThisExpressionFirst (VDL->ValDef.SDef->RightHandSide, FDL);
	 RemoveTupleCyclic (LHS);
	 AddToOrderedValDefList (VDL, FDL);
         break;
#ifdef DEBUG
      default :
	 Crash ("OrderThisDefinition");
         break;
#endif
   }
}

void TupleSingleAssignment (TupleLHS, VDL)
   Expression TupleLHS;
   ValueDefinitionList VDL;
{
   Expression FirstExp, RestExp;
   if (TupleLHS == NULL)
      return;
#ifdef DEBUG
   if (TupleLHS->ExpKind != TupleExp)
      Crash ("TupleSingleAssignment 1");
#endif
   FirstExp = TupleLHS->ExpDef.SonsForTuple.FirstExpression;
   RestExp = TupleLHS->ExpDef.SonsForTuple.RestOfTuple;
#ifdef DEBUG
   if (FirstExp->ExpKind != IdentLeafExp)
      Crash ("TupleSingleAssignment 2");
#endif
   if ((IsDefinedInTuple
	  (FirstExp->ExpDef.ThisSymbol, RestExp) == true) ||
       (IsDefinedInVDL
	  (FirstExp->ExpDef.ThisSymbol, VDL->Next) == true))
      perr (10, FirstExp->ExpDef.ThisSymbol->SymbolId.IdPos);
   TupleSingleAssignment (RestExp, VDL);
}

void OrderDefinitions (VDL, FDL)
   ValueDefinitionList VDL;
   FunctionDefinitionList FDL;
{
   Expression LHS;
   if (VDL == NULL)
      return;
#ifdef DEBUG
   if (VDL->ValDefKind != SingleDef)
      Crash ("OrderDefinitions");
#endif
   LHS = VDL->ValDef.SDef->LeftHandSide;
   switch (LHS->ExpKind) {
      case IdentLeafExp :
         if (IsDefinedInVDL
	       (LHS->ExpDef.ThisSymbol,
	        VDL->Next) == true)
            perr (10, LHS->ExpDef.ThisSymbol->SymbolId.IdPos);
	 break;
      case InitExp :
      case LpInitExp :
	 break;
      case TupleExp :
	 TupleSingleAssignment (LHS, VDL);
	 break;
   }
   OrderThisDefinition (VDL, FDL);
   OrderDefinitions (VDL->Next, FDL);
}

void CopyOrderedNextToNext (VDL)
   ValueDefinitionList VDL;
{
   ValueDefinitionList VD;
   if (VDL == NULL)
      return;
   VD = VDL->Next;
   VDL->Next = VDL->OrderedNext;
   VDL->OrderedNext = VD;
   CopyOrderedNextToNext (VD);
}

void Iter8FuncDefList (FDL)
   FunctionDefinitionList FDL;
{
   ValueDefinitionList VDL;
   if (FDL == NULL)
      return;
   if ((NoIterations (FDL->ValDefs) == true) &&
       (NoIntsInFDL (FDL) == true) &&
       (NoArraysInFDL (FDL) == true)) {
      OrderDefinitions (FDL->ValDefs, FDL);
      FDL->OrderedValDefs =
	 ReverseOrderedValDefList (FDL->OrderedValDefs);
      VDL = FDL->ValDefs;
      FDL->ValDefs = FDL->OrderedValDefs;
      FDL->OrderedValDefs = VDL;
      CopyOrderedNextToNext (VDL);
   }
   Iter8FuncDefList (FDL->OrderedNext);
}

void pass8 ()
{
   Iter8FuncDefList (SemGraph.OrderedFuncDefs);
}
