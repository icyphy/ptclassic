/* $Id$ */

/* Author : Chris Scheers */
/* dpr2.c -- version 1.5 (IMEC)		last updated: 3/1/89 */
static char *SccsId = "@(#)dpr2.c	1.5 (IMEC)	89/03/01";

#include <stdio.h>
#include "sfg.h"

extern SemanticGraph SemGraph;

void CheckCDFInTuple (Tuple)
   Expression Tuple;
{
   void CheckCDFInExp ();
   if (Tuple == NULL)
      return;
#ifdef DEBUG
   if (Tuple->ExpKind != TupleExp)
      Crash ("CheckCDFInTuple");
#endif
   CheckCDFInExp
     (Tuple->ExpDef.SonsForTuple.FirstExpression);
   CheckCDFInTuple
     (Tuple->ExpDef.SonsForTuple.RestOfTuple);
}

void CheckCDFInExp (Exp)
   Expression Exp;
{
   switch (Exp->ExpKind) {
      case IdentLeafExp :
      case IndexedExp :
      case FixedLeafExp :
      case IntegerLeafExp :
      case DelayExp :
      case LpDelayExp :
      case CastExp :
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
	 CheckCDFInExp (Exp->ExpDef.SonsForBinOp.LeftExp);
	 CheckCDFInExp (Exp->ExpDef.SonsForBinOp.RightExp);
	 break;
      case SinExp :
      case CosExp :
      case SqrtExp :
      case NotExp :
      case NegateExp :
      case LeftShiftExp :
      case RightShiftExp :
	 CheckCDFInExp (Exp->ExpDef.SonsForBinOp.LeftExp);
	 break;
      case FuncAppExp :
	 if (Exp->ExpDef.SonsForFuncApp.ThisFunction->DelaysExist == true)
	    perr (44, Exp->ExpPos);
         CheckCDFInTuple (Exp->ExpDef.SonsForFuncApp.ActualParameters);
	 break;
      case UserFuncExp :
         CheckCDFInTuple (Exp->ExpDef.SonsForUserFunc.ActualUserParameters);
	 break;
      case TimeMuxExp :
         CheckCDFInTuple (Exp->ExpDef.SonsForTimeMux.TimeMuxInputs);
	 break;
      case ReSampExp :
         CheckCDFInTuple (Exp->ExpDef.SonsForReSamp.ReSampInputs);
	 break;
      case CondExp :
	 CheckCDFInExp (Exp->ExpDef.SonsForCond.IfExp);
	 CheckCDFInExp (Exp->ExpDef.SonsForCond.ThenExp);
	 if (Exp->ExpDef.SonsForCond.ElseExp != NULL)
	    CheckCDFInExp (Exp->ExpDef.SonsForCond.ElseExp);
	 break;
      case TupleExp :
	 CheckCDFInTuple (Exp);
	 break;
      default :
	 perr (7, Exp->ExpPos);
	 break;
   }
}

void NoCondDelFuncsInTuple (Tuple)
   Expression Tuple;
{
   void NoCondDelFuncsInExp ();
   if (Tuple == NULL)
      return;
#ifdef DEBUG
   if (Tuple->ExpKind != TupleExp)
      Crash ("NoCondDelFuncsInTuple");
#endif
   NoCondDelFuncsInExp
     (Tuple->ExpDef.SonsForTuple.FirstExpression);
   NoCondDelFuncsInTuple
     (Tuple->ExpDef.SonsForTuple.RestOfTuple);
}

void NoCondDelFuncsInExp (Exp)
   Expression Exp;
{
   switch (Exp->ExpKind) {
      case IdentLeafExp :
      case IndexedExp :
      case FixedLeafExp :
      case IntegerLeafExp :
      case DelayExp :
      case LpDelayExp :
      case CastExp :
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
	 NoCondDelFuncsInExp (Exp->ExpDef.SonsForBinOp.LeftExp);
	 NoCondDelFuncsInExp (Exp->ExpDef.SonsForBinOp.RightExp);
	 break;
      case SinExp :
      case CosExp :
      case SqrtExp :
      case NotExp :
      case NegateExp :
      case LeftShiftExp :
      case RightShiftExp :
	 NoCondDelFuncsInExp (Exp->ExpDef.SonsForBinOp.LeftExp);
	 break;
      case FuncAppExp :
         NoCondDelFuncsInTuple (Exp->ExpDef.SonsForFuncApp.ActualParameters);
	 break;
      case UserFuncExp :
         NoCondDelFuncsInTuple (Exp->ExpDef.SonsForUserFunc.ActualUserParameters);
	 break;
      case TimeMuxExp :
	 NoCondDelFuncsInTuple (Exp->ExpDef.SonsForTimeMux.TimeMuxInputs);
	 break;
      case ReSampExp :
	 NoCondDelFuncsInTuple (Exp->ExpDef.SonsForReSamp.ReSampInputs);
	 break;
      case CondExp :
	 NoCondDelFuncsInExp (Exp->ExpDef.SonsForCond.IfExp);
	 CheckCDFInExp (Exp->ExpDef.SonsForCond.ThenExp);
	 if (Exp->ExpDef.SonsForCond.ElseExp != NULL)
	    CheckCDFInExp (Exp->ExpDef.SonsForCond.ElseExp);
	 break;
      case TupleExp :
	 NoCondDelFuncsInTuple (Exp);
	 break;
      default :
	 perr (7, Exp->ExpPos);
	 break;
   }
}

void NoCondDelFuncs (VDL)
   ValueDefinitionList VDL;
{
   if (VDL == NULL)
      return;
   switch (VDL->ValDefKind) {
      case SingleDef :
	 NoCondDelFuncsInExp (VDL->ValDef.SDef->RightHandSide);
	 break;
      case IteratedDef :
	 NoCondDelFuncs (VDL->ValDef.IDef->IterValDefs);
	 break;
#ifdef DEBUG
      default :
	 Crash ("NoCondDelFuncs");
#endif
   }
   NoCondDelFuncs (VDL->Next);
}

void IterDpr2FuncDefList (FDL)
   FunctionDefinitionList FDL;
{
   if (FDL == NULL)
      return;
   if (FDL->IsProcedure == false)
      NoCondDelFuncs (FDL->ValDefs);
   IterDpr2FuncDefList (FDL->OrderedNext);
}

void dpr2 ()
{
   IterDpr2FuncDefList (SemGraph.OrderedFuncDefs);
}
