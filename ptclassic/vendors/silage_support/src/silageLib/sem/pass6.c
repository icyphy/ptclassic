/* $Id$ */

/* Author : Chris Scheers */
/* pass6.c -- version 1.11 (IMEC)		last updated: 4/28/89 */
static char *SccsId = "@(#)pass6.c	1.11 (IMEC)	89/04/28";

#include <stdio.h>
#include "sfg.h"

extern SemanticGraph SemGraph;

bool EqualTypes (tp1, tp2)
   SymbolType tp1, tp2;
{
   if (tp1.SymTpKind != tp2.SymTpKind)
      return (false);
   switch (tp1.SymTpKind) {
      case FixedType :
	 if ((tp1.SymTp.FixedTp.w == tp2.SymTp.FixedTp.w) &&
	     (tp1.SymTp.FixedTp.d == tp2.SymTp.FixedTp.d))
            return (true);
         else
	    return (false);
         break;
      case IntType :
	 return (true);
         break;
      case BoolType :
	 return (true);
	 break;
#ifdef DEBUG
      default :
	 Crash ("EqualTypes");
	 break;
#endif
   }
}

SymbolType MultCoercion (tp1, tp2, pos)
   SymbolType tp1, tp2;
   Position pos;
{
   SymbolType ResultTp;
   if (tp1.SymTpKind == FixedType && tp2.SymTpKind == FixedType) {
       ResultTp = tp1;
       ResultTp.SymTp.FixedTp.w = tp1.SymTp.FixedTp.w + tp2.SymTp.FixedTp.w;
       ResultTp.SymTp.FixedTp.d = tp1.SymTp.FixedTp.d + tp2.SymTp.FixedTp.d;
   } else if (tp1.SymTpKind == FixedType && tp2.SymTpKind == IntType) {
	 ResultTp = tp1;
   } else if (tp2.SymTpKind == FixedType && tp1.SymTpKind == IntType) {
	 ResultTp = tp2;
   } else {
	 ResultTp = tp1;
   }
   return (ResultTp);
}

SymbolType DivCoercion (tp1, tp2, pos)
   SymbolType tp1, tp2;
   Position pos;
{
   SymbolType ResultTp;
   if (tp1.SymTpKind == FixedType && tp2.SymTpKind == FixedType) {
       ResultTp = tp1;
/* Rule from the UCB manual
         if (tp1.SymTp.FixedTp.w != tp2.SymTp.FixedTp.w)
            perr (21, pos);
         ResultTp = tp1;
         ResultTp.SymTp.FixedTp.d = tp1.SymTp.FixedTp.d +
            tp1.SymTp.FixedTp.w - tp2.SymTp.FixedTp.d;
         if (ResultTp.SymTp.FixedTp.d < 0)
            perr (24, pos);
*/
   } else if (tp1.SymTpKind == FixedType && tp2.SymTpKind == IntType) {
	 ResultTp = tp1;
   } else if (tp2.SymTpKind == FixedType && tp1.SymTpKind == IntType) {
	 ResultTp = tp2;
   } else {
	 ResultTp = tp1;
   }
   return (ResultTp);
}

void ChangeToExpIntegerLeaf (Exp, IntVal)
   Expression Exp;
   int IntVal;
{
   Exp->ExpKind = IntegerLeafExp;
   Exp->ExpDef.IntegerConstant = IntVal;
}

void ChangeToExpFixedLeaf (Exp, FM, FS, FStr)
   Expression Exp;
   int FM, FS;
   char *FStr;
{
   Exp->ExpKind = FixedLeafExp;
   Exp->ExpDef.SonsForFixedLeaf.M = FM;
   Exp->ExpDef.SonsForFixedLeaf.S = FS;
   Exp->ExpDef.SonsForFixedLeaf.FixedString = FStr;
}

Expression StripFixedDims (Tuple)
   Expression Tuple;
{
   if (Tuple == NULL)
      return (NULL);
   if (Tuple->ExpDef.SonsForTuple.FirstExpression == NULL)
      return (Tuple);
   else
      return
	(StripFixedDims 
	  (Tuple->ExpDef.SonsForTuple.RestOfTuple));
}

int NrFreeDims (Tuple)
   Expression Tuple;
{
   if (Tuple == NULL)
      return (0);
   else
      if (Tuple->ExpDef.SonsForTuple.FirstExpression != NULL)
	 perr (36, Tuple->ExpPos);
      else
	 return
	   (1 + NrFreeDims
             (Tuple->ExpDef.SonsForTuple.RestOfTuple));
}

int SymbolNrDims (ST)
   SymbolTable ST;
{
   if (ST->IsArray == false)
      return (0);
   else
      return (ST->SingleOrArray.AS.NumberOfDimensions);
}

SymbolType DeductTypeOfExp (Exp)
   Expression Exp;
{
   switch (Exp->ExpKind) {
      case IntegerLeafExp :
      case FixedLeafExp :
      case CastExp :
	 return (Exp->ExpType);
	 break;
      case DelayExp :
      case LpDelayExp :
	 return (DeductTypeOfExp (Exp->ExpDef.SonsForDelay.DelaySigExp));
	 break;
      case LeftShiftExp :
      case RightShiftExp :
	 return (DeductTypeOfExp (Exp->ExpDef.SonsForBinOp.LeftExp));
	 break;
      case IdentLeafExp :
	 return (Exp->ExpDef.ThisSymbol->SymType);
	 break;
      case IndexedExp :
	 return (Exp->ExpDef.SonsForIndexed.ThisArray->SymType);
	 break;
      case PlusExp :
      case MinExp :
      case MaxExp :
      case MinusExp : {
	 SymbolType TpOfLHS, TpOfRHS;
	 TpOfLHS = DeductTypeOfExp (Exp->ExpDef.SonsForBinOp.LeftExp);
	 TpOfRHS = DeductTypeOfExp (Exp->ExpDef.SonsForBinOp.RightExp);
	 if (TpOfLHS.SymTpKind == UndefType)
	    return (TpOfRHS);
         else
	    if (TpOfRHS.SymTpKind == UndefType)
	       return (TpOfLHS);
            else
	       /* if (EqualTypes (TpOfLHS, TpOfRHS) == false)
	           perr (21, Exp->ExpPos);
	       else (PH 6/28/92) */
                  return (TpOfLHS);
	 }
	 break;
      case MultExp : {
	 SymbolType TpOfLHS, TpOfRHS;
	 TpOfLHS = DeductTypeOfExp (Exp->ExpDef.SonsForBinOp.LeftExp);
	 TpOfRHS = DeductTypeOfExp (Exp->ExpDef.SonsForBinOp.RightExp);
	 if (TpOfLHS.SymTpKind == UndefType)
	    /* if (TpOfRHS.SymTpKind == UndefType)
	       perr (20, Exp->ExpPos);
            else (PH 6/28/92) */
	       return (MultCoercion (TpOfRHS, TpOfRHS, Exp->ExpPos));
         else
	    if (TpOfRHS.SymTpKind == UndefType)
	       return (MultCoercion (TpOfLHS, TpOfLHS, Exp->ExpPos));
            else
	       return (MultCoercion (TpOfLHS, TpOfRHS, Exp->ExpPos));
	 }
	 break;
      case DivExp : {
	 SymbolType TpOfLHS, TpOfRHS;
	 TpOfLHS = DeductTypeOfExp (Exp->ExpDef.SonsForBinOp.LeftExp);
	 TpOfRHS = DeductTypeOfExp (Exp->ExpDef.SonsForBinOp.RightExp);
	 if (TpOfLHS.SymTpKind == UndefType)
	    /* if (TpOfRHS.SymTpKind == UndefType)
	       perr (20, Exp->ExpPos);
            else (PH 6/28/92) */
	       return (DivCoercion (TpOfRHS, TpOfRHS, Exp->ExpPos));
         else
	    if (TpOfRHS.SymTpKind == UndefType)
	       return (DivCoercion (TpOfLHS, TpOfLHS, Exp->ExpPos));
            else
	       return (DivCoercion (TpOfLHS, TpOfRHS, Exp->ExpPos));
	 }
	 break;
      case CosExp :
	 return (DeductTypeOfExp (Exp->ExpDef.SonsForBinOp.LeftExp));
	 break;
      case SinExp :
	 return (DeductTypeOfExp (Exp->ExpDef.SonsForBinOp.LeftExp));
	 break;
      case SqrtExp :
	 return (DeductTypeOfExp (Exp->ExpDef.SonsForBinOp.LeftExp));
	 break;
      case NegateExp :
	 return (DeductTypeOfExp (Exp->ExpDef.SonsForBinOp.LeftExp));
	 break;
      case FuncAppExp : {
	 SymbolTable OutPars;
         OutPars = Exp->ExpDef.SonsForFuncApp
		   .ThisFunction->OutputParameters;
         if (OutPars->Next != NULL)
            perr (23, Exp->ExpPos);
	 return (OutPars->SymType);
         }
	 break;
      case UserFuncExp : {
         SymbolType UserTp;
         char *FuncId;
         FuncId = Exp->ExpDef.SonsForUserFunc.UserFuncId.IdName;
	 UserTp.SymTpKind = UndefType;
         if (strcmp(FuncId, "bit") == 0) {
	     UserTp.SymTpKind = FixedType;
	     UserTp.SymTp.FixedTp.w = 1;
	     UserTp.SymTp.FixedTp.d = 0;
	     CheckNrBitArgs(Exp->ExpDef.SonsForUserFunc.ActualUserParameters);
	 } else if (strcmp(FuncId, "bitselect") == 0) {
	     UserTp.SymTpKind = FixedType;
	     UserTp.SymTp.FixedTp.w = GetBitSelectWidth(Exp->ExpDef.SonsForUserFunc.
			ActualUserParameters);
	     UserTp.SymTp.FixedTp.d = 0;
         } else if (strcmp(FuncId, "bitmerge") == 0) {
	     UserTp.SymTpKind = FixedType;
	     UserTp.SymTp.FixedTp.w = GetBitMergeWidth(Exp->ExpDef.SonsForUserFunc.
			ActualUserParameters);
	     UserTp.SymTp.FixedTp.d = 0;
         }
	 return (UserTp);
	 }
	 break;
      case CondExp : {
	 SymbolType ThenTp, ElseTp;
	 ThenTp = DeductTypeOfExp (Exp->ExpDef.SonsForCond.ThenExp);
	 if (Exp->ExpDef.SonsForCond.ElseExp == NULL)
	    return (ThenTp);
	 ElseTp = DeductTypeOfExp (Exp->ExpDef.SonsForCond.ElseExp);
	 if (ThenTp.SymTpKind == UndefType)
	    return (ElseTp);
	 if (ElseTp.SymTpKind == UndefType)
	    return (ThenTp);
	 if (EqualTypes (ThenTp, ElseTp) == false)
	    /*perr (21, Exp->ExpPos) (PH 6/28/92) */;
	 return (ThenTp);
	 }
	 break;
      case GreaterExp :
      case SmallerExp :
      case GreaterEqualExp :
      case SmallerEqualExp :
      case EqualExp :
      case NotEqualExp : {
	 SymbolType CompTp;
	 CompTp.SymTpKind = BoolType;
	 CompTp.SymTp.BoolLength = 1;
	 return (CompTp);
         }
	 break;
      case OrExp :
      case ExorExp :
      case AndExp : {
	 SymbolType TpOfLHS, TpOfRHS, ResultTp;
	 TpOfLHS = DeductTypeOfExp (Exp->ExpDef.SonsForBinOp.LeftExp);
	 TpOfRHS = DeductTypeOfExp (Exp->ExpDef.SonsForBinOp.RightExp);
	 if (TpOfLHS.SymTpKind == UndefType)
	    ResultTp = TpOfRHS;
         else
	    if (TpOfRHS.SymTpKind == UndefType)
	       ResultTp = TpOfLHS;
            else
	       /* if (EqualTypes (TpOfLHS, TpOfRHS) == false)
	          perr (21, Exp->ExpPos);
	       else (PH 6/28/92) */
                  ResultTp = TpOfLHS;
	 return (ResultTp);
	 }
	 break;
      case NotExp : {
	 SymbolType ResultTp;
	 ResultTp = DeductTypeOfExp (Exp->ExpDef.SonsForBinOp.LeftExp);
	 return (ResultTp);
	 }
	 break;
      case TimeMuxExp :
      case ReSampExp :
	 perr (46, Exp->ExpPos);
	 break;
      default :
	 perr (16, Exp->ExpPos);
	 break;
   }
}

void CheckExpManifest (Exp, ErrNr)
   Expression Exp;
   int ErrNr;
{
   switch (Exp->ExpKind) {
      case IdentLeafExp :
	 if ((Exp->ExpDef.ThisSymbol->IsArray == true) ||
	     (Exp->ExpDef.ThisSymbol->SymType.SymTpKind != IntType))
	    /* perr (ErrNr, Exp->ExpPos) (PH 6/28/92) */;
      case IntegerLeafExp :
	 break;
      case NegateExp :
	 CheckExpManifest (Exp->ExpDef.SonsForBinOp.LeftExp, ErrNr);
	 break;
      case PlusExp :
      case MinusExp :
      case MinExp :
      case MaxExp :
      case MultExp :
	 CheckExpManifest (Exp->ExpDef.SonsForBinOp.LeftExp, ErrNr);
	 CheckExpManifest (Exp->ExpDef.SonsForBinOp.RightExp, ErrNr);
         break;
#ifdef DEBUG
      default :
	 Crash ("CheckExpManifest");
	 break;
#endif
   }
}

void CheckTypesOfIndexTuple (Tuple)
   Expression Tuple;
{
   if (Tuple == NULL)
      return;
#ifdef DEBUG
   if (Tuple->ExpKind != TupleExp)
      Crash ("CheckTypesOfIndexTuple");
#endif
   if (Tuple->ExpDef.SonsForTuple.FirstExpression != NULL)
      CheckExpManifest
	(Tuple->ExpDef.SonsForTuple.FirstExpression, 36);
   CheckTypesOfIndexTuple
     (Tuple->ExpDef.SonsForTuple.RestOfTuple);
}

void EnforceTypesOfInpParams (ActParams, FormParams, pos)
   Expression ActParams;
   SymbolTable FormParams;
   Position pos;
{
   void EnforceTypeOfExp ();
   if (ActParams == NULL)
      if (FormParams == NULL)
	 return;
      else
	 perr (22, pos);
   else
      if (FormParams == NULL)
	 perr (23, pos);
#ifdef DEBUG
   if (ActParams->ExpKind != TupleExp)
      Crash ("EnforceTypesOfInpParams");
#endif
   EnforceTypeOfExp
     (ActParams->ExpDef.SonsForTuple.FirstExpression,
      FormParams->SymType,
      SymbolNrDims (FormParams),
      false);
   EnforceTypesOfInpParams
     (ActParams->ExpDef.SonsForTuple.RestOfTuple,
      FormParams->Next,
      pos);
}

CheckNrBitArgs(ActPars)
   Expression ActPars;
{
   Expression E;
   int i;

   for (E = ActPars, i=1; E->ExpDef.SonsForTuple.RestOfTuple != NULL;
		     E = E->ExpDef.SonsForTuple.RestOfTuple,i++);
   if (i > 2)
      perr (23, E->ExpDef.SonsForTuple.FirstExpression->ExpPos);
   if (i < 2)
      perr (22, E->ExpDef.SonsForTuple.FirstExpression->ExpPos);
}

GetBitSelectWidth(ActPars)
   Expression ActPars;
{
   Expression E;
   int i;

   for (E = ActPars, i=1; E->ExpDef.SonsForTuple.RestOfTuple != NULL;
		     E = E->ExpDef.SonsForTuple.RestOfTuple, i++);
   if (i > 3)
      perr (23, E->ExpDef.SonsForTuple.FirstExpression->ExpPos);
   if (i < 3)
      perr (22, E->ExpDef.SonsForTuple.FirstExpression->ExpPos);
   return(EvalManExp (E->ExpDef.SonsForTuple.FirstExpression));
}

GetBitMergeWidth(ActPars)
   Expression ActPars;
{
   Expression E;
   SymbolType ResultTp;
   int i, w = 0;

   for (E = ActPars,i=0;E != NULL;E = E->ExpDef.SonsForTuple.RestOfTuple,i++) {
      ResultTp = DeductTypeOfExp(E->ExpDef.SonsForTuple.FirstExpression);  
      if (ResultTp.SymTpKind != FixedType)
	 /* perr (21, E->ExpDef.SonsForTuple.FirstExpression->ExpPos) PH 6/28*/;
      w = w + ResultTp.SymTp.FixedTp.w;
   }
   if (i > 2)
      perr (23, ActPars->ExpDef.SonsForTuple.FirstExpression->ExpPos);
   if (i < 2)
      perr (22, ActPars->ExpDef.SonsForTuple.FirstExpression->ExpPos);
   return(w);
}

void EnforceDeductedTypesOfBitOpParams (Func, ActPars)
   char *Func;
   Expression ActPars;
{
   Expression NextPars;
   void EnforceTypeOfExp ();
   SymbolType ActTp;
   int NFD;
   if (ActPars == NULL)
      return;
#ifdef DEBUG
   if (ActPars->ExpKind != TupleExp)
      Crash ("EnforceDeductedTypesOfBitParams");
#endif
   ActTp = DeductTypeOfExp
     (ActPars->ExpDef.SonsForTuple.FirstExpression);
   if (ActTp.SymTpKind == UndefType)
      /* perr (20, ActPars->ExpPos) PH 6/28*/;
   if ((ActTp.SymTpKind != FixedType) &&
       (ActTp.SymTpKind != IntType))
      /* perr (21, ActPars->ExpPos) PH 6/28*/;
   NFD = 0;
   if (ActPars->ExpDef.SonsForTuple.FirstExpression->ExpKind ==
       IndexedExp)
     NFD = NrFreeDims
	     (StripFixedDims
	        (ActPars->ExpDef.SonsForTuple.FirstExpression
		  ->ExpDef.SonsForIndexed.IndexTupleExp));
   if (((ActPars->ExpDef.SonsForTuple.FirstExpression->ExpKind == DelayExp) ||
     (ActPars->ExpDef.SonsForTuple.FirstExpression->ExpKind == LpDelayExp)) &&
       (ActPars->ExpDef.SonsForTuple.FirstExpression->ExpDef
	.SonsForDelay.DelaySigExp->ExpKind == IndexedExp))
     NFD = NrFreeDims
	     (StripFixedDims
		(ActPars->ExpDef.SonsForTuple.FirstExpression
		  ->ExpDef.SonsForDelay.DelaySigExp->ExpDef
		  .SonsForIndexed.IndexTupleExp));
   EnforceTypeOfExp (ActPars->ExpDef.SonsForTuple.FirstExpression,
		     ActTp, NFD, false);
   /* Now for the 2nd argument */
   NextPars = ActPars->ExpDef.SonsForTuple.RestOfTuple;
   ActTp = DeductTypeOfExp (NextPars->ExpDef.SonsForTuple.FirstExpression);
   if (ActTp.SymTpKind != IntType)
      ActTp.SymTpKind = IntType;
   if (strcmp(Func, "bitselect") == 0) {  /* Do 3rd argument */
      NextPars = NextPars->ExpDef.SonsForTuple.RestOfTuple;
      if (NextPars == NULL)
	 perr (22, ActPars->ExpPos);
      ActTp = DeductTypeOfExp (NextPars->ExpDef.SonsForTuple.FirstExpression);
      if (ActTp.SymTpKind != IntType)
         ActTp.SymTpKind = IntType;
   }
   NextPars = NextPars->ExpDef.SonsForTuple.RestOfTuple;
   if (NextPars != NULL)
      perr (23, ActPars->ExpPos);
}

void EnforceDeductedTypesOfUserParams (ActPars)
   Expression ActPars;
{
   void EnforceTypeOfExp ();
   SymbolType ActTp;
   int NFD;
   if (ActPars == NULL)
      return;
#ifdef DEBUG
   if (ActPars->ExpKind != TupleExp)
      Crash ("EnforceDeductedTypesOfUserParams");
#endif
   ActTp = DeductTypeOfExp
     (ActPars->ExpDef.SonsForTuple.FirstExpression);
   if (ActTp.SymTpKind == UndefType)
      /* perr (20, ActPars->ExpPos) (PH 6/28/92) */;
   if ((ActTp.SymTpKind != FixedType) &&
       (ActTp.SymTpKind != IntType))
      /* perr (21, ActPars->ExpPos) PH 6/28*/;
   NFD = 0;
   if (ActPars->ExpDef.SonsForTuple.FirstExpression->ExpKind ==
       IndexedExp)
     NFD = NrFreeDims
	     (StripFixedDims
	        (ActPars->ExpDef.SonsForTuple.FirstExpression
		  ->ExpDef.SonsForIndexed.IndexTupleExp));
   if (((ActPars->ExpDef.SonsForTuple.FirstExpression->ExpKind == DelayExp) ||
     (ActPars->ExpDef.SonsForTuple.FirstExpression->ExpKind == LpDelayExp)) &&
       (ActPars->ExpDef.SonsForTuple.FirstExpression->ExpDef
	.SonsForDelay.DelaySigExp->ExpKind == IndexedExp))
     NFD = NrFreeDims
	     (StripFixedDims
		(ActPars->ExpDef.SonsForTuple.FirstExpression
		  ->ExpDef.SonsForDelay.DelaySigExp->ExpDef
		  .SonsForIndexed.IndexTupleExp));
   EnforceTypeOfExp (ActPars->ExpDef.SonsForTuple.FirstExpression,
		     ActTp, NFD, false);
   EnforceDeductedTypesOfUserParams
     (ActPars->ExpDef.SonsForTuple.RestOfTuple);
}

void EnforceTypeOfExp (Exp, ForceTp, NFD, IsCast)
   Expression Exp;
   SymbolType ForceTp;
   int NFD;
   bool IsCast;
{
#ifdef DEBUG
   if (ForceTp.SymTpKind == UndefType)
      Crash ("EnforceTypeOfExp 1");
   if ((Exp->ExpType.SymTpKind != UndefType) &&
       (Exp->ExpKind != CastExp))
      Crash ("EnforceTypeOfExp 2");
#endif
   if (NFD > 0)
      switch (Exp->ExpKind) {
         case IndexedExp :
	 case DelayExp :
	 case LpDelayExp :
         case FuncAppExp :
         case CondExp :
	    break;
	 case CastExp :
	    /* perr (24, Exp->ExpPos) PH 6/28*/;
	    break;
         default :
	    /* perr (21, Exp->ExpPos) PH 6/28*/;
	    break;
      }
   switch (Exp->ExpKind) {
      case IntegerLeafExp :
	 Exp->ExpType = ForceTp;
	 if (ForceTp.SymTpKind == FixedType) {
	    ChangeToExpFixedLeaf
	      (Exp, Exp->ExpDef.IntegerConstant, 1, (char *) NULL);
            AddToListOfFixedLeafs (Exp);
	 }
	 break;
      case FixedLeafExp :
	 Exp->ExpType = ForceTp;
	 break;
      case CastExp :
	 if (Exp->ExpType.SymTpKind == UndefType)
	    Exp->ExpType = ForceTp;
	 else if ((IsCast == false) && 
		   EqualTypes (Exp->ExpType, ForceTp) == false)
		  /* perr (21, Exp->ExpPos) PH 6/28*/;
	 EnforceTypeOfExp
	   (Exp->ExpDef.CastedExp, Exp->ExpType, NFD, true);
         break;
      case DelayExp :
      case LpDelayExp : {
	 Expression DlsgExp;
	 SymbolType DlsgTp;
	 CheckExpManifest (Exp->ExpDef.SonsForDelay.DelayValExp, 50);
	 DlsgExp = Exp->ExpDef.SonsForDelay.DelaySigExp;
         DlsgTp = DeductTypeOfExp (DlsgExp);
         if (DlsgTp.SymTpKind == UndefType)
	    /* perr (20, DlsgExp->ExpPos) PH 6/28*/;
	 if ((DlsgTp.SymTpKind == IntType) ||
	     (DlsgTp.SymTpKind == BoolType))
            /* perr (49, DlsgExp->ExpPos) PH 6/28*/;
	 if (DlsgTp.SymTpKind == UndefType)
	     DlsgTp = ForceTp;
	 else if ((IsCast == false) && 
		   EqualTypes (DlsgTp, ForceTp) == false)
		  /* perr (21, Exp->ExpPos) PH 6/28*/;
	 Exp->ExpType = DlsgTp;
	 EnforceTypeOfExp (DlsgExp, DlsgTp, NFD, false);
	 }
	 break;
      case LeftShiftExp :
      case RightShiftExp : {
	 SymbolType DedTp;
         CheckExpManifest (Exp->ExpDef.SonsForBinOp.RightExp, 50);
	 DedTp = DeductTypeOfExp (Exp->ExpDef.SonsForBinOp.LeftExp);
	 if (DedTp.SymTpKind == UndefType)
	    DedTp = ForceTp;
	 if ((DedTp.SymTpKind != FixedType) &&
	     (DedTp.SymTpKind != IntType))
	    /* perr (49, Exp->ExpPos) PH 6/28*/;
	 if (DedTp.SymTpKind == UndefType)
	    DedTp = ForceTp;
	 else if ((IsCast == false) && 
		   EqualTypes (DedTp, ForceTp) == false)
		  /* perr (21, Exp->ExpPos) PH 6/28*/;
	 Exp->ExpType = DedTp;
	 EnforceTypeOfExp
	   (Exp->ExpDef.SonsForBinOp.LeftExp, DedTp, NFD, false);
         }
	 break;
      case IdentLeafExp : {
	 SymbolType IdTp;
	 IdTp = Exp->ExpDef.ThisSymbol->SymType;
	 if (IdTp.SymTpKind == UndefType)
	    IdTp = ForceTp;
	 else if ((IsCast == false) && 
		   EqualTypes (IdTp, ForceTp) == false)
		  /* perr (21, Exp->ExpPos) PH 6/28*/;
	 Exp->ExpDef.ThisSymbol->SymType = IdTp;
	 Exp->ExpType = IdTp;
	 }
         break;
      case IndexedExp : {
	 SymbolType ArrTp;
	 ArrTp = Exp->ExpDef.SonsForIndexed.ThisArray->SymType;
	 if (ArrTp.SymTpKind == UndefType)
	    ArrTp = ForceTp;
	 else if ((IsCast == false) && 
		   EqualTypes (ArrTp, ForceTp) == false)
		  /* perr (21, Exp->ExpPos) PH 6/28*/;
	 if (NrFreeDims
	       (StripFixedDims
	          (Exp->ExpDef.SonsForIndexed.IndexTupleExp))
             != NFD)
            /* perr (21, Exp->ExpPos) PH 6/28*/;
	 CheckTypesOfIndexTuple
	   (Exp->ExpDef.SonsForIndexed.IndexTupleExp);
         Exp->ExpDef.SonsForIndexed.ThisArray->SymType = ArrTp;
	 Exp->ExpType = ArrTp;
	 }
	 break;
      case PlusExp :
      case MinusExp :
      case MinExp :
      case MaxExp :
      case MultExp : 
      case DivExp : {
	 SymbolType TpOfLHS, TpOfRHS, ResultTp;
	 if (IsCast == true)
	    ResultTp = ForceTp;
	 else {
            ResultTp = DeductTypeOfExp (Exp);
	    if (ResultTp.SymTpKind == UndefType)
	       ResultTp = ForceTp;
            else
	       if (EqualTypes (ResultTp, ForceTp) == false)
		  /* perr (21, Exp->ExpPos) PH 6/28*/;
         }
	 Exp->ExpType = ResultTp;
	 if (Exp->ExpType.SymTpKind == BoolType)
	    /* perr (49, Exp->ExpPos) PH 6/28*/;
	 TpOfLHS = DeductTypeOfExp (Exp->ExpDef.SonsForBinOp.LeftExp);
	 TpOfRHS = DeductTypeOfExp (Exp->ExpDef.SonsForBinOp.RightExp);
	 if (TpOfLHS.SymTpKind == UndefType)
	    if (TpOfRHS.SymTpKind == UndefType) {
	       TpOfLHS = ForceTp;
	       TpOfRHS = ForceTp;
            } else
	       TpOfLHS = TpOfRHS;
         else
	    if (TpOfRHS.SymTpKind == UndefType)
	       TpOfRHS = TpOfLHS;
	 if (TpOfLHS.SymTpKind != TpOfRHS.SymTpKind)
	    /* perr (21, Exp->ExpPos) PH 6/28 */;
	 EnforceTypeOfExp
	   (Exp->ExpDef.SonsForBinOp.LeftExp, TpOfLHS, NFD, false);
	 EnforceTypeOfExp
	   (Exp->ExpDef.SonsForBinOp.RightExp, TpOfRHS, NFD, false);
	 }
	 break;
      case SinExp :
      case CosExp :
      case SqrtExp : {
	 Expression RootExp;
	 SymbolType RootTp, ArgTp;
	 RootExp = Exp->ExpDef.SonsForBinOp.LeftExp;
	 if (IsCast == true)
	    RootTp = ForceTp;
         else {
	    RootTp = DeductTypeOfExp (RootExp);
	    if (RootTp.SymTpKind == UndefType)
	       RootTp = ForceTp;
            else
	       if (EqualTypes (RootTp, ForceTp) == false)
                 /* perr (21, Exp->ExpPos) PH 6/28 */;
	 }
         Exp->ExpType = RootTp;
	 ArgTp = DeductTypeOfExp (RootExp);
	 if (ArgTp.SymTpKind == UndefType)
	    ArgTp = ForceTp;
	 if ((ArgTp.SymTpKind != FixedType) ||
	     (RootTp.SymTpKind != FixedType))
	    /* perr (21, Exp->ExpPos) PH 6/28*/;
	 EnforceTypeOfExp (RootExp, ArgTp, NFD, false);
	 }
	 break;
      case NegateExp : {
	 Expression NgtdExp;
	 SymbolType NgtdTp;
	 NgtdExp = Exp->ExpDef.SonsForBinOp.LeftExp;
	 NgtdTp = DeductTypeOfExp (NgtdExp);
	 if (NgtdTp.SymTpKind == UndefType)
	    NgtdTp = ForceTp;
         else
	    if ((IsCast == false) &&
		(EqualTypes (NgtdTp, ForceTp) == false))
               /* perr (21, Exp->ExpPos) PH 6/28*/;
	 Exp->ExpType = NgtdTp;
	 if (Exp->ExpType.SymTpKind == BoolType)
	    /* perr (49, Exp->ExpPos) PH 6/28*/;
	 EnforceTypeOfExp (NgtdExp, NgtdTp, NFD, false);
	 switch (NgtdExp->ExpKind) {
	    case IntegerLeafExp :
	       ChangeToExpIntegerLeaf
		 (Exp, -NgtdExp->ExpDef.IntegerConstant);
	       break;
            case FixedLeafExp :
	       ChangeToExpFixedLeaf
		 (Exp,
		  -NgtdExp->ExpDef.SonsForFixedLeaf.M,
		  NgtdExp->ExpDef.SonsForFixedLeaf.S,
		  NgtdExp->ExpDef.SonsForFixedLeaf.FixedString);
               RmFromListOfFixedLeafs (NgtdExp);
	       AddToListOfFixedLeafs (Exp);
	       break;
            default :
	       break;
         }
	 }
	 break;
      case FuncAppExp : {
	 SymbolType OutTp;
	 SymbolTable OutPar;
	 EnforceTypesOfInpParams
	   (Exp->ExpDef.SonsForFuncApp.ActualParameters,
	    Exp->ExpDef.SonsForFuncApp.ThisFunction->InputParameters,
	    Exp->ExpPos);
	 OutTp = DeductTypeOfExp (Exp);
	 if (IsCast == true)
	    OutTp = ForceTp;
	 else {
	    if (OutTp.SymTpKind == UndefType)
	       OutTp = ForceTp;
            else
	       if (EqualTypes (OutTp, ForceTp) == false)
		  /* perr (21, Exp->ExpPos) PH 6/28*/;
         }
	 OutPar = Exp->ExpDef.SonsForFuncApp.ThisFunction
		    ->OutputParameters;
         if (SymbolNrDims (OutPar) != NFD)
	    /* perr (21, Exp->ExpPos) PH 6/28*/;
	 Exp->ExpType = OutTp;
	 }
	 break;
      case UserFuncExp : {
	 char *FuncId;
         FuncId = Exp->ExpDef.SonsForUserFunc.UserFuncId.IdName;
	 if (strcmp(FuncId, "bit") == 0 || strcmp(FuncId, "bitselect") == 0) {
	     EnforceDeductedTypesOfBitOpParams
	             (FuncId, Exp->ExpDef.SonsForUserFunc.ActualUserParameters);
	     if ((ForceTp.SymTpKind != FixedType) && 
		 (ForceTp.SymTpKind != BoolType) && (ForceTp.SymTpKind != IntType))
	         /* perr (21, Exp->ExpPos) PH 6/28*/;
         } else {
	     EnforceDeductedTypesOfUserParams
	                    (Exp->ExpDef.SonsForUserFunc.ActualUserParameters);
	     if ((ForceTp.SymTpKind != FixedType) &&
	                         (ForceTp.SymTpKind != IntType))
	         /* perr (21, Exp->ExpPos) PH 6/28 */;
         }
         Exp->ExpType = ForceTp;
         }
	 break;
      case CondExp : {
	 SymbolType IfTp, ThenTp, ElseTp, ResultTp;
	 if (IsCast == true)
	    ResultTp = ForceTp;
         else {
	    ResultTp = DeductTypeOfExp (Exp);
	    if (ResultTp.SymTpKind == UndefType)
	       ResultTp = ForceTp;
            else
	       if (EqualTypes (ResultTp, ForceTp) == false)
		  /* perr (21, Exp->ExpPos) PH 6/28 */;
	 }
	 Exp->ExpType = ResultTp;
	 IfTp = DeductTypeOfExp (Exp->ExpDef.SonsForCond.IfExp);
	 if (IfTp.SymTpKind != BoolType)
	    /* perr (21, Exp->ExpDef.SonsForCond.IfExp->ExpPos) PH 6/28*/;
         EnforceTypeOfExp (Exp->ExpDef.SonsForCond.IfExp, IfTp,
			   0, false);
         ThenTp = DeductTypeOfExp (Exp->ExpDef.SonsForCond.ThenExp);
	 if (Exp->ExpDef.SonsForCond.ElseExp == NULL)
	    ElseTp.SymTpKind = UndefType;
         else
            ElseTp = DeductTypeOfExp (Exp->ExpDef.SonsForCond.ElseExp);
	 if (ThenTp.SymTpKind == UndefType)
	    if (ElseTp.SymTpKind == UndefType) {
	       ThenTp = ForceTp;
	       ElseTp = ForceTp;
            } else
	       ThenTp = ElseTp;
         else
	    if (ElseTp.SymTpKind == UndefType)
	       ElseTp = ThenTp;
         EnforceTypeOfExp
	   (Exp->ExpDef.SonsForCond.ThenExp, ThenTp, NFD, false);
	 if (Exp->ExpDef.SonsForCond.ElseExp != NULL)
            EnforceTypeOfExp
	      (Exp->ExpDef.SonsForCond.ElseExp, ElseTp, NFD, false);
	 }
	 break;
      case GreaterExp :
      case SmallerExp :
      case GreaterEqualExp :
      case SmallerEqualExp :
      case EqualExp :
      case NotEqualExp : {
	 SymbolType TpOfLHS, TpOfRHS;
         if (ForceTp.SymTpKind != BoolType)
	    /* perr (21, Exp->ExpPos) PH 6/28*/;
	 Exp->ExpType = ForceTp;
	 TpOfLHS = DeductTypeOfExp (Exp->ExpDef.SonsForBinOp.LeftExp);
	 TpOfRHS = DeductTypeOfExp (Exp->ExpDef.SonsForBinOp.RightExp);
	 if (TpOfLHS.SymTpKind == UndefType)
	    /* if (TpOfRHS.SymTpKind == UndefType)
	       perr (20, Exp->ExpDef.SonsForBinOp.LeftExp->ExpPos);
            else (PH 6/28/92) */
	       TpOfLHS = TpOfRHS;
         else
	    if (TpOfRHS.SymTpKind == UndefType)
	       TpOfRHS = TpOfLHS;
	    /* else
	       if (EqualTypes (TpOfLHS, TpOfRHS) == false)
		   perr (21, Exp->ExpPos) PH 6/28*/;
	 if ((TpOfLHS.SymTpKind == BoolType) ||
	     (TpOfRHS.SymTpKind == BoolType))
            /* perr (49, Exp->ExpPos) PH 6/28*/;
	 EnforceTypeOfExp
	   (Exp->ExpDef.SonsForBinOp.LeftExp, TpOfLHS, NFD, false);
	 EnforceTypeOfExp
	   (Exp->ExpDef.SonsForBinOp.RightExp, TpOfRHS, NFD, false);
	 }
	 break;
      case OrExp :
      case ExorExp :
      case AndExp : {
	 SymbolType ResultTp, TpOfLHS, TpOfRHS;
	 if (IsCast == true)
	    ResultTp = ForceTp;
	 else {
            ResultTp = DeductTypeOfExp (Exp);
	    if (ResultTp.SymTpKind == UndefType)
	       ResultTp = ForceTp;
            else
	       if (EqualTypes (ResultTp, ForceTp) == false)
		  /* perr (21, Exp->ExpPos) PH 6/28*/;
         }
	 Exp->ExpType = ResultTp;
	 TpOfLHS = DeductTypeOfExp (Exp->ExpDef.SonsForBinOp.LeftExp);
	 TpOfRHS = DeductTypeOfExp (Exp->ExpDef.SonsForBinOp.RightExp);
	 if (TpOfLHS.SymTpKind == UndefType)
	    TpOfLHS = ForceTp;
	 if (TpOfRHS.SymTpKind == UndefType)
	    TpOfRHS = ForceTp;
	 if (EqualTypes (TpOfLHS, TpOfRHS) == false)
	    /* perr (21, Exp->ExpPos) PH 6/28*/;
	 if ((TpOfLHS.SymTpKind != BoolType) &&
	     (TpOfLHS.SymTpKind != FixedType) &&
	     (TpOfLHS.SymTpKind != IntType))
            /* perr (21, Exp->ExpPos) PH 6/28*/;
	 EnforceTypeOfExp
	   (Exp->ExpDef.SonsForBinOp.LeftExp, TpOfLHS, NFD, false);
	 EnforceTypeOfExp
	   (Exp->ExpDef.SonsForBinOp.RightExp, TpOfRHS, NFD, false);
	 }
	 break;
      case NotExp : {
	 SymbolType ResultTp, TpOfLHS;
	 if (IsCast == true)
	    ResultTp = ForceTp;
	 else {
            ResultTp = DeductTypeOfExp (Exp);
	    if (ResultTp.SymTpKind == UndefType)
	       ResultTp = ForceTp;
            else
	       if (EqualTypes (ResultTp, ForceTp) == false)
		  /* perr (21, Exp->ExpPos) PH 6/28*/;
         }
	 Exp->ExpType = ResultTp;
	 TpOfLHS = DeductTypeOfExp (Exp->ExpDef.SonsForBinOp.LeftExp);
	 if (TpOfLHS.SymTpKind == UndefType)
	    TpOfLHS = ForceTp;
	 if ((TpOfLHS.SymTpKind != BoolType) &&
	     (TpOfLHS.SymTpKind != FixedType) &&
	     (TpOfLHS.SymTpKind != IntType))
            /* perr (21, Exp->ExpPos) 6/28*/;
	 EnforceTypeOfExp
	   (Exp->ExpDef.SonsForBinOp.LeftExp, TpOfLHS, NFD, false);
	 }
	 break;
      case TimeMuxExp :
      case ReSampExp :
	 perr (46, Exp->ExpPos);
	 break;
      default :
	 perr (16, Exp->ExpPos);
	 break;
   }
}

void DeductTypesOfOutParams (ActParams, FormParams, pos)
   Expression ActParams;
   SymbolTable FormParams;
   Position pos;
{
   Expression ActP;
   if (ActParams == NULL)
      if (FormParams == NULL)
	 return;
      else
	 perr (22, pos);
   else
      if (FormParams == NULL)
	 perr (23, pos);
#ifdef DEBUG
   if (ActParams->ExpKind != TupleExp)
      Crash ("DeductTypesOfOutParams 1");
#endif
   ActP = ActParams->ExpDef.SonsForTuple.FirstExpression;
   switch (ActP->ExpKind) {
      case IdentLeafExp : {
	 SymbolTable SymDef;
	 SymDef = ActP->ExpDef.ThisSymbol;
	 if (SymDef->SymType.SymTpKind == UndefType)
	    SymDef->SymType = FormParams->SymType;
	 }
	 break;
      case IndexedExp : {
	 SymbolTable SymDef;
	 SymDef = ActP->ExpDef.SonsForIndexed.ThisArray;
	 if (SymDef->SymType.SymTpKind == UndefType)
	    SymDef->SymType = FormParams->SymType;
	 }
	 break;
#ifdef DEBUG
      default :
	 Crash ("DeductTypesOfOutParams 2");
	 break;
#endif
   }
   DeductTypesOfOutParams
     (ActParams->ExpDef.SonsForTuple.RestOfTuple,
      FormParams->Next,
      pos);
}

void DeductTypesOfTuple (LHS, RHS)
   Expression LHS, RHS;
{
   void DeductTypeOfSD ();
   if (LHS == NULL)
      if (RHS == NULL)
         return;
      else
	 perr (31, RHS->ExpPos);
   else
      if (RHS == NULL)
	 perr (31, LHS->ExpPos);
   DeductTypeOfSD
     (LHS->ExpDef.SonsForTuple.FirstExpression,
      RHS->ExpDef.SonsForTuple.FirstExpression);
   DeductTypesOfTuple
     (LHS->ExpDef.SonsForTuple.RestOfTuple,
      RHS->ExpDef.SonsForTuple.RestOfTuple);
}

void DeductTypesOfCondTuple (LHS, RHS)
   Expression LHS, RHS;
{
   void DeductTypeOfSD ();
#ifdef DEBUG
   if (LHS->ExpKind != TupleExp)
      Crash ("DeductTypesOfCondTuple");
#endif
   switch (RHS->ExpKind) {
      case TupleExp :
         DeductTypesOfTuple (LHS, RHS);
         break;
      case CondExp :
      case FuncAppExp :
         DeductTypeOfSD (LHS, RHS);
	 break;
      default :
         perr (31, LHS->ExpPos);
         break;
   }
}

SymbolType DeductTypeOfTimeMuxInput (Exp)
   Expression Exp;
{
   SymbolType SwInpTp, DedTp;
   SwInpTp.SymTpKind = UndefType;
   while (Exp != NULL) {
#ifdef DEBUG
      if (Exp->ExpKind != TupleExp)
	 Crash ("DeductTypeOfTimeMuxInput");
#endif
      DedTp = DeductTypeOfExp
                (Exp->ExpDef.SonsForTuple.FirstExpression);
      if (DedTp.SymTpKind != UndefType) 
         if (SwInpTp.SymTpKind == UndefType)
	    SwInpTp = DedTp;
	 else
	    if (EqualTypes (DedTp, SwInpTp) == false)
	       /* perr (45, Exp->ExpPos) PH 6/28*/;
      Exp = Exp->ExpDef.SonsForTuple.RestOfTuple;
   };
   return (SwInpTp);
}

void MakeTypeOfTimeMuxOutputs (LHS, SwTp)
   Expression LHS;
   SymbolType SwTp;
{
   switch (LHS->ExpKind) {
      case IdentLeafExp : {
	 SymbolTable SymDef;
	 SymDef = LHS->ExpDef.ThisSymbol;
	 if (SymDef->SymType.SymTpKind == UndefType)
	    SymDef->SymType = SwTp;
         else
	    if (EqualTypes (SymDef->SymType, SwTp) == false)
	       /* perr (45, LHS->ExpPos) PH 6/28*/;
	 }
	 break;
      case IndexedExp : {
	 SymbolTable SymDef;
	 SymDef = LHS->ExpDef.SonsForIndexed.ThisArray;
	 if (SymDef->SymType.SymTpKind == UndefType)
	    SymDef->SymType = SwTp;
         else
	    if (EqualTypes (SymDef->SymType, SwTp) == false)
	       /* perr (45, LHS->ExpPos) 6/28*/;
         }
	 break;
      case TupleExp :
	 MakeTypeOfTimeMuxOutputs
	   (LHS->ExpDef.SonsForTuple.FirstExpression, SwTp);
         if (LHS->ExpDef.SonsForTuple.RestOfTuple != NULL)
	    MakeTypeOfTimeMuxOutputs
	      (LHS->ExpDef.SonsForTuple.RestOfTuple, SwTp);
	 break;
      default :
         perr (51, LHS->ExpPos);
	 break;
   }
}

void DeductTypeOfSDTimeMux (LHS, RHS)
   Expression LHS, RHS;
{
   SymbolType SwInpTp;
   SwInpTp = DeductTypeOfTimeMuxInput
               (RHS->ExpDef.SonsForTimeMux.TimeMuxInputs);
   if (SwInpTp.SymTpKind == UndefType)
      perr (20, RHS->ExpPos);
   if (SwInpTp.SymTpKind != FixedType)
      /* perr (21, RHS->ExpPos) PH 6/28*/;
   RHS->ExpType = SwInpTp;
   MakeTypeOfTimeMuxOutputs (LHS, SwInpTp);
}

void DeductTypeOfSDReSamp (LHS, RHS)
   Expression LHS, RHS;
{
   SymbolType SwInpTp;
   SwInpTp = DeductTypeOfTimeMuxInput
               (RHS->ExpDef.SonsForReSamp.ReSampInputs);
   if (SwInpTp.SymTpKind == UndefType)
      perr (20, RHS->ExpPos);
   if (SwInpTp.SymTpKind != FixedType)
      /* perr (21, RHS->ExpPos) PH 6/28*/;
   RHS->ExpType = SwInpTp;
   MakeTypeOfTimeMuxOutputs (LHS, SwInpTp);
}

void DeductTypeOfSD (LHS, RHS)
   Expression LHS, RHS;
{
   switch (LHS->ExpKind) {
      case IdentLeafExp : {
	 SymbolTable SymDef;
	 SymDef = LHS->ExpDef.ThisSymbol;
	 if (SymDef->SymType.SymTpKind != UndefType)
	    return;
	 SymDef->SymType =
	    DeductTypeOfExp (RHS);
	 }
	 break;
      case IndexedExp : {
	 SymbolTable SymDef;
	 SymDef = LHS->ExpDef.SonsForIndexed.ThisArray;
	 if (SymDef->SymType.SymTpKind != UndefType)
	    return;
	 SymDef->SymType =
	    DeductTypeOfExp (RHS);
	 }
	 break;
      case TupleExp :
	 switch (RHS->ExpKind) {
	    case FuncAppExp :
	       DeductTypesOfOutParams
		 (LHS,
		  RHS->ExpDef.SonsForFuncApp.ThisFunction
		    ->OutputParameters,
                  RHS->ExpPos);
	       break;
	    case TupleExp :
	       DeductTypesOfTuple (LHS, RHS);
	       break;
	    case CondExp :
	       DeductTypesOfCondTuple
		 (LHS, RHS->ExpDef.SonsForCond.ThenExp);
	       if (RHS->ExpDef.SonsForCond.ElseExp != NULL)
		  DeductTypesOfCondTuple
		    (LHS, RHS->ExpDef.SonsForCond.ElseExp);
	       break;
	    default :
	       perr (16, RHS->ExpPos);
	       break;
         }
	 break;
      case InitExp :
      case LpInitExp : {
	 SymbolTable SymDef;
         Expression DSExp;
	 DSExp = LHS->ExpDef.SonsForDelay.DelaySigExp;
	 switch(DSExp->ExpKind) {
	     case IdentLeafExp : {
		 SymDef = DSExp->ExpDef.ThisSymbol;
		 if (SymDef->SymType.SymTpKind != UndefType)
		     return;
		 SymDef->SymType = DeductTypeOfExp(RHS);
	         }
	         break;
	     case IndexedExp : {
         	 SymDef = LHS->ExpDef.SonsForIndexed.ThisArray;
         	 if (SymDef->SymType.SymTpKind != UndefType)
            	     return;
         	 SymDef->SymType = DeductTypeOfExp (RHS);
         	 }
         	 break;
	     default :
		 break;
	 }
	 break;
         }
      default :
	 perr (12, LHS->ExpPos);
	 break;
   }
}

void EnforceTypesOfOutParams (ActParams, FormParams, pos)
   Expression ActParams;
   SymbolTable FormParams;
   Position pos;
{
   Expression ActP;
   if (ActParams == NULL)
      if (FormParams == NULL)
	 return;
      else
	 perr (22, pos);
   else
      if (FormParams == NULL)
	 perr (23, pos);
#ifdef DEBUG
   if (ActParams->ExpKind != TupleExp)
      Crash ("EnforceTypesOfOutParams 1");
#endif
   ActP = ActParams->ExpDef.SonsForTuple.FirstExpression;
   switch (ActP->ExpKind) {
      case IdentLeafExp :
#ifdef DEBUG
         if (ActP->ExpDef.ThisSymbol->SymType.SymTpKind
	     == UndefType)
            Crash ("EnforceTypesOfOutParams 2");
#endif
	 if (EqualTypes
	       (ActP->ExpDef.ThisSymbol->SymType,
		FormParams->SymType) == false)
            /* perr (21, ActP->ExpPos) PH 6/28*/;
	 if (SymbolNrDims (FormParams) != 0)
	    /* perr (21, ActP->ExpPos) PH 6/28*/;
	 break;
      case IndexedExp :
#ifdef DEBUG
         if (ActP->ExpDef.SonsForIndexed.ThisArray
	       ->SymType.SymTpKind == UndefType)
            Crash ("EnforceTypesOfOutParams 3");
#endif
	 if (EqualTypes
	       (ActP->ExpDef.SonsForIndexed.ThisArray
		  ->SymType,
                FormParams->SymType) == false)
            /* perr (21, ActP->ExpPos) PH 6/28*/;
	 if (SymbolNrDims (FormParams)
	     !=
	     NrFreeDims
	       (StripFixedDims
		  (ActP->ExpDef.SonsForIndexed.IndexTupleExp)))
            /* perr (21, ActP->ExpPos) PH 6/28*/;
	 break;
#ifdef DEBUG
      default :
	 Crash ("EnforceTypesOfOutParams 4");
	 break;
#endif
   }
   EnforceTypesOfOutParams
     (ActParams->ExpDef.SonsForTuple.RestOfTuple,
      FormParams->Next,
      pos);
}

void EnforceTypesOfTuple (LHS, RHS)
   Expression LHS, RHS;
{
   void EnforceTypeOfSD ();
   if (LHS == NULL)
      if (RHS == NULL)
         return;
      else
	 perr (31, RHS->ExpPos);
   else
      if (RHS == NULL)
	 perr (31, LHS->ExpPos);
   EnforceTypeOfSD
     (LHS->ExpDef.SonsForTuple.FirstExpression,
      RHS->ExpDef.SonsForTuple.FirstExpression);
   EnforceTypesOfTuple
     (LHS->ExpDef.SonsForTuple.RestOfTuple,
      RHS->ExpDef.SonsForTuple.RestOfTuple);
}

void EnforceTypesOfCondTuple (LHS, RHS)
   Expression LHS, RHS;
{
   void EnforceTypeOfSD ();
#ifdef DEBUG
   if (LHS->ExpKind != TupleExp)
      Crash ("EnforceTypesOfCondTuple");
#endif
   switch (RHS->ExpKind) {
      case TupleExp :
         EnforceTypesOfTuple (LHS, RHS);
         break;
      case CondExp :
      case FuncAppExp :
         EnforceTypeOfSD (LHS, RHS);
	 break;
      default :
         perr (31, LHS->ExpPos);
         break;
   }
}

void EnforceTypeOfTimeMuxInput (Exp, SwTp, NFD)
   Expression Exp;
   SymbolType SwTp;
   int NFD;
{
   if (Exp == NULL)
      return;
#ifdef DEBUG
   if (Exp->ExpKind != TupleExp)
      Crash ("EnforceTypeOfTimeMuxInput");
#endif
   EnforceTypeOfExp (Exp->ExpDef.SonsForTuple.FirstExpression,
		     SwTp, NFD, false);
   EnforceTypeOfTimeMuxInput
     (Exp->ExpDef.SonsForTuple.RestOfTuple, SwTp, NFD);
}

void EnforceTypeOfSD (LHS, RHS)
   Expression LHS, RHS;
{
   switch (LHS->ExpKind) {
      case IdentLeafExp : {
	 SymbolTable SymDef;
	 SymDef = LHS->ExpDef.ThisSymbol;
	 if (SymDef->SymType.SymTpKind == BoolType)
	    /* perr (49, RHS->ExpPos) PH 6/28*/;
	 if (SymDef->SymType.SymTpKind == UndefType)
	    /* perr (20, SymDef->SymbolId.IdPos) PH 6/28*/;
	 EnforceTypeOfExp (RHS, SymDef->SymType, 0, false);
	 }
	 break;
      case IndexedExp : {
	 SymbolTable SymDef;
	 SymDef = LHS->ExpDef.SonsForIndexed.ThisArray;
	 if (SymDef->SymType.SymTpKind == BoolType)
	    /* perr (49, RHS->ExpPos) PH 6/28*/;
	 if (SymDef->SymType.SymTpKind == UndefType)
	    /* perr (20, SymDef->SymbolId.IdPos) PH 6/28*/;
         EnforceTypeOfExp
	   (RHS,
	    SymDef->SymType,
            NrFreeDims
	      (StripFixedDims
		(LHS->ExpDef.SonsForIndexed.IndexTupleExp)),
	    false);
	 CheckTypesOfIndexTuple
	   (LHS->ExpDef.SonsForIndexed.IndexTupleExp);
	 }
	 break;
      case TupleExp :
	 switch (RHS->ExpKind) {
	    case FuncAppExp :
	       EnforceTypesOfOutParams
		 (LHS,
		  RHS->ExpDef.SonsForFuncApp.ThisFunction
		    ->OutputParameters,
                  RHS->ExpPos);
	       EnforceTypesOfInpParams
	         (RHS->ExpDef.SonsForFuncApp.ActualParameters,
	          RHS->ExpDef.SonsForFuncApp.ThisFunction
		    ->InputParameters,
	          RHS->ExpPos);
	       break;
	    case TupleExp :
	       EnforceTypesOfTuple (LHS, RHS);
	       break;
	    case CondExp : {
	       SymbolType IfTp;
	       IfTp = DeductTypeOfExp
			(RHS->ExpDef.SonsForCond.IfExp);
	       if (IfTp.SymTpKind != BoolType)
		  /* perr (21, RHS->ExpDef.SonsForCond.IfExp->ExpPos) PH 6/28*/;
               EnforceTypeOfExp (RHS->ExpDef.SonsForCond.IfExp,
				 IfTp, 0, false);
	       EnforceTypesOfCondTuple
		 (LHS, RHS->ExpDef.SonsForCond.ThenExp);
               if (RHS->ExpDef.SonsForCond.ElseExp != NULL)
		  EnforceTypesOfCondTuple
		    (LHS, RHS->ExpDef.SonsForCond.ElseExp);
	       }
	       break;
	    default :
	       perr (16, RHS->ExpPos);
	       break;
	 }
	 break;
      case InitExp :
      case LpInitExp : {
	 Expression DSE, DVE;
	 DSE = LHS->ExpDef.SonsForDelay.DelaySigExp;
	 DVE = LHS->ExpDef.SonsForDelay.DelayValExp;
	 CheckExpManifest (DVE, 50);
	 switch (DSE->ExpKind) {
	    case IdentLeafExp : {
	       SymbolTable SymDef;
	       SymDef = DSE->ExpDef.ThisSymbol;
	       if (SymDef->SymType.SymTpKind == UndefType)
	          /* perr (20, SymDef->SymbolId.IdPos) PH 6/28*/;
	       EnforceTypeOfExp (RHS, SymDef->SymType, 0, false);
	       }
	       break;
	    case IndexedExp : {
	       SymbolTable SymDef;
	       int NFD;
	       SymDef = DSE->ExpDef.SonsForIndexed.ThisArray;
	       if (SymDef->SymType.SymTpKind == UndefType)
	          /* perr (20, SymDef->SymbolId.IdPos) PH 6/28*/;
	       NFD = NrFreeDims
	               (StripFixedDims
		         (DSE->ExpDef.SonsForIndexed.IndexTupleExp));
	       if (NFD > 0)
		  perr (16, SymDef->SymbolId.IdPos);
               EnforceTypeOfExp
	         (RHS,
	          SymDef->SymType,
                  NFD,
	          false);
	       CheckTypesOfIndexTuple
	         (DSE->ExpDef.SonsForIndexed.IndexTupleExp);
               }
	       break;
	 }
	 }
	 break;
#ifdef DEBUG
      default :
	 Crash ("EnforceTypeOfSD");
	 break;
#endif
   }
}

void DeductTypesOfVDL (VDL)
   ValueDefinitionList VDL;
{
   if (VDL == NULL)
      return;
   switch (VDL->ValDefKind) {
      case SingleDef :
         if (VDL->ValDef.SDef->RightHandSide->ExpKind == TimeMuxExp)
	    DeductTypeOfSDTimeMux (VDL->ValDef.SDef->LeftHandSide,
			          VDL->ValDef.SDef->RightHandSide);
         else if (VDL->ValDef.SDef->RightHandSide->ExpKind == ReSampExp)
	    DeductTypeOfSDReSamp (VDL->ValDef.SDef->LeftHandSide,
			          VDL->ValDef.SDef->RightHandSide);
	 else
	    DeductTypeOfSD (VDL->ValDef.SDef->LeftHandSide,
			    VDL->ValDef.SDef->RightHandSide);
	 break;
      case IteratedDef :
	 if (VDL->ValDef.IDef->IDKind == DoLoop)
	    DeductTypeOfSD (VDL->ValDef.IDef->ExitDef->LeftHandSide,
			    VDL->ValDef.IDef->ExitDef->RightHandSide);
	 DeductTypesOfVDL (VDL->ValDef.IDef->IterValDefs);
	 break;
#ifdef DEBUG
      default :
	 Crash ("DeductTypesOfVDL");
         break;
#endif
   }
   DeductTypesOfVDL (VDL->Next);
}

void EnforceTypesOfVDL (VDL)
   ValueDefinitionList VDL;
{
   if (VDL == NULL)
      return;
   switch (VDL->ValDefKind) {
      case SingleDef : {
	 Expression RHS;
	 RHS = VDL->ValDef.SDef->RightHandSide;
         if (RHS->ExpKind == TimeMuxExp)
	    if (RHS->ExpDef.SonsForTimeMux.NrArrayInputs > 0)
               EnforceTypeOfTimeMuxInput
                 (RHS->ExpDef.SonsForTimeMux.TimeMuxInputs,
	          RHS->ExpType, 1);
	    else
               EnforceTypeOfTimeMuxInput
                 (RHS->ExpDef.SonsForTimeMux.TimeMuxInputs,
	          RHS->ExpType, 0);
	 else if (RHS->ExpKind == ReSampExp)
	    EnforceTypeOfTimeMuxInput
		 (RHS->ExpDef.SonsForReSamp.ReSampInputs,
		  RHS->ExpType, 0);
	 else
	    EnforceTypeOfSD (VDL->ValDef.SDef->LeftHandSide,
                             RHS);
	 }
	 break;
      case IteratedDef :
	 if (VDL->ValDef.IDef->IDKind == WhileLoop) {
	    SymbolType WhileTp;
	    Expression WExp;
	    WExp = VDL->ValDef.IDef->WhileExp;
	    WhileTp = DeductTypeOfExp (WExp);
	    if (WhileTp.SymTpKind != BoolType)
	       /* perr (21, WExp->ExpPos) PH 6/28*/;
            EnforceTypeOfExp (WExp, WhileTp, 0, false);
         }
	 else if (VDL->ValDef.IDef->IDKind == DoLoop) {
	    EnforceTypeOfSD (VDL->ValDef.IDef->ExitDef->LeftHandSide,
			     VDL->ValDef.IDef->ExitDef->RightHandSide);
	 }
	 EnforceTypesOfVDL (VDL->ValDef.IDef->IterValDefs);
	 break;
#ifdef DEBUG
      default :
	 Crash ("EnforceTypesOfVDL");
	 break;
#endif
   }
   EnforceTypesOfVDL (VDL->Next);
}

void BackwardEnforceTypesOfVDL (VDL)
   ValueDefinitionList VDL;
{
   if (VDL == NULL)
      return;
   BackwardEnforceTypesOfVDL (VDL->Next);
   switch (VDL->ValDefKind) {
      case SingleDef : {
	 Expression RHS;
	 RHS = VDL->ValDef.SDef->RightHandSide;
         if (RHS->ExpKind == TimeMuxExp)
	    if (RHS->ExpDef.SonsForTimeMux.NrArrayInputs > 0)
               EnforceTypeOfTimeMuxInput
                 (RHS->ExpDef.SonsForTimeMux.TimeMuxInputs,
	          RHS->ExpType, 1);
	    else
               EnforceTypeOfTimeMuxInput
                 (RHS->ExpDef.SonsForTimeMux.TimeMuxInputs,
	          RHS->ExpType, 0);
	 else if (RHS->ExpKind == ReSampExp)
	    EnforceTypeOfTimeMuxInput
		 (RHS->ExpDef.SonsForReSamp.ReSampInputs,
		  RHS->ExpType, 0);
	 else
	    EnforceTypeOfSD (VDL->ValDef.SDef->LeftHandSide,
                             RHS);
	 }
	 break;
      case IteratedDef :
	 if (VDL->ValDef.IDef->IDKind == WhileLoop) {
	    SymbolType WhileTp;
	    Expression WExp;
	    WExp = VDL->ValDef.IDef->WhileExp;
	    WhileTp = DeductTypeOfExp (WExp);
	    if (WhileTp.SymTpKind != BoolType)
	       /* perr (21, WExp->ExpPos) PH 6/28*/;
            EnforceTypeOfExp (WExp, WhileTp, 0, false);
         }
	 else if (VDL->ValDef.IDef->IDKind == DoLoop) {
	    EnforceTypeOfSD (VDL->ValDef.IDef->ExitDef->LeftHandSide,
			     VDL->ValDef.IDef->ExitDef->RightHandSide);
	 }
	 BackwardEnforceTypesOfVDL (VDL->ValDef.IDef->IterValDefs);
	 break;
#ifdef DEBUG
      default :
	 Crash ("BackwardEnforceTypesOfVDL");
	 break;
#endif
   }
}

void Iter6FuncDefList (FDL)
   FunctionDefinitionList FDL;
{
   if (FDL == NULL)
      return;
   DeductTypesOfVDL (FDL->ValDefs);  /* Ordered input to output */
   BackwardEnforceTypesOfVDL (FDL->ValDefs); /* Ordered output to input */
   Iter6FuncDefList (FDL->OrderedNext);
}

void pass6 ()
{
   Iter6FuncDefList (SemGraph.OrderedFuncDefs);
}
