/* $Id$ */

/* Author : Chris Scheers */
/* dpr3.c -- version 1.5 (IMEC)		last updated: 3/1/89 */
static char *SccsId = "@(#)dpr3.c	1.5 (IMEC)	89/03/01";

#include <stdio.h>
#include "sfg.h"

extern SemanticGraph SemGraph;

void EnforceProcessOfSymbol (SymDef, Process, pos)
   SymbolTable SymDef;
   int Process;
   Position pos;
{
   if (SymDef->ProcessId == -1)
      SymDef->ProcessId = Process;
   else
      if (SymDef->ProcessId != Process)
         perr (52, pos);
}

void EnforceProcessOfExp (Exp, Process)
   Expression Exp;
   int Process;
{
   switch (Exp->ExpKind) {
      case FixedLeafExp :
      case IntegerLeafExp :
	 break;
      case InitExp :
      case LpInitExp :
	 /* must be present */
	 break;
      case IdentLeafExp : 
	 EnforceProcessOfSymbol
	   (Exp->ExpDef.ThisSymbol, Process, Exp->ExpPos);
	 break;
      case IndexedExp :
	 EnforceProcessOfSymbol
	   (Exp->ExpDef.SonsForIndexed.ThisArray, Process, Exp->ExpPos);
	 break;
      case DelayExp :
      case LpDelayExp :
	 EnforceProcessOfExp (Exp->ExpDef.SonsForDelay.DelaySigExp, Process);
	 break;
      case CastExp :
	 EnforceProcessOfExp (Exp->ExpDef.CastedExp, Process);
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
	 EnforceProcessOfExp (Exp->ExpDef.SonsForBinOp.LeftExp, Process);
	 EnforceProcessOfExp (Exp->ExpDef.SonsForBinOp.RightExp, Process);
	 break;
      case SinExp :
      case CosExp :
      case SqrtExp :
      case NotExp :
      case NegateExp :
      case LeftShiftExp :
      case RightShiftExp :
	 EnforceProcessOfExp (Exp->ExpDef.SonsForBinOp.LeftExp, Process);
	 break;
      case FuncAppExp :
         EnforceProcessOfExp
	   (Exp->ExpDef.SonsForFuncApp.ActualParameters, Process);
	 break;
      case UserFuncExp :
	 EnforceProcessOfExp
	   (Exp->ExpDef.SonsForUserFunc.ActualUserParameters, Process);
	 break;
      case TimeMuxExp :
	 EnforceProcessOfExp
	   (Exp->ExpDef.SonsForTimeMux.TimeMuxInputs, Process);
	 break;
      case ReSampExp :
	 EnforceProcessOfExp
	   (Exp->ExpDef.SonsForReSamp.ReSampInputs, Process);
	 break;
      case CondExp :
	 EnforceProcessOfExp (Exp->ExpDef.SonsForCond.IfExp, Process);
	 EnforceProcessOfExp (Exp->ExpDef.SonsForCond.ThenExp, Process);
	 if (Exp->ExpDef.SonsForCond.ElseExp != NULL)
	    EnforceProcessOfExp (Exp->ExpDef.SonsForCond.ElseExp, Process);
	 break;
      case TupleExp :
	 EnforceProcessOfExp
	   (Exp->ExpDef.SonsForTuple.FirstExpression, Process);
         if (Exp->ExpDef.SonsForTuple.RestOfTuple != NULL)
	    EnforceProcessOfExp
	      (Exp->ExpDef.SonsForTuple.RestOfTuple, Process);
	 break;
      default :
	 perr (7, Exp->ExpPos);
	 break;
   }
}

/* blabla
void DeductProcessOfSD (LHS, RHS, CurrentProcess)
   Expression LHS, RHS;
   int CurrentProcess;
{
   switch (LHS->ExpKind) {
      case IdentLeafExp :
	 EnforceProcessOfExp (RHS, CurrentProcess);
	 break;
      case IndexedExp :
	 break;
      case TupleExp :
	 break;
      case InitExp :
      case LpInitExp :
	 break;
#ifdef DEBUG
      default :
	 Crash ("DeductProcessOfSD");
	 break;
#endif
   }
}
*/

int ProcessPartitioning (VDL, CurrentProcess, InLoop)
   ValueDefinitionList VDL;
   int CurrentProcess;
   bool InLoop;
{
   if (VDL == NULL)
      return (CurrentProcess);
   switch (VDL->ValDefKind) {
      case SingleDef : {
	 Expression RHS, LHS;
	 LHS = VDL->ValDef.SDef->LeftHandSide;
	 RHS = VDL->ValDef.SDef->RightHandSide;
	 EnforceProcessOfExp (RHS, CurrentProcess);
	 if (RHS->ExpKind == TimeMuxExp || RHS->ExpKind == ReSampExp)
	    if (InLoop == true)
	       perr (46, RHS->ExpPos);
            else
	       EnforceProcessOfExp
	         (LHS, ++CurrentProcess);
	 else
	    EnforceProcessOfExp
	      (LHS, CurrentProcess);
	 }
	 break;
      case IteratedDef :
	 if (VDL->ValDef.IDef->IDKind == WhileLoop)
	    EnforceProcessOfExp
	      (VDL->ValDef.IDef->WhileExp, CurrentProcess);
	 CurrentProcess =
	   ProcessPartitioning
	     (VDL->ValDef.IDef->IterValDefs, CurrentProcess, true);
	 break;
#ifdef DEBUG
      default :
	 Crash ("ProcessPartioning");
#endif
   }
   return
     (ProcessPartitioning (VDL->Next, CurrentProcess, InLoop));
}

void dpr3 ()
{
   SemGraph.FuncDefs->NumberOfProcesses =
     ProcessPartitioning (SemGraph.FuncDefs->ValDefs, 1, false);
}
