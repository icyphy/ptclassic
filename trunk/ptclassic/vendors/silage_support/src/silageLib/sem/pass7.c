/* $Id$ */

/* Author : Chris Scheers */
/* pass7.c -- version 1.12 (IMEC)		last updated: 3/1/89 */
static char *SccsId = "@(#)pass7.c	1.12 (IMEC)	89/03/01";

#include <stdio.h>
#include "sfg.h"

extern SemanticGraph SemGraph;
extern int NrFreeDims ();
extern SingleDefinition MkSingleDefinition ();
extern Expression MkExpIdentLeaf ();
extern ValueDefinitionList MkValDefSingle ();

int EvalManExp (Exp)
   Expression Exp;
{
   switch (Exp->ExpKind) {
      case IdentLeafExp :
	 if (Exp->ExpDef.ThisSymbol->SymKind != Iterator)
	    perr (50, Exp->ExpPos);
	 return (Exp->ExpDef.ThisSymbol->
		   SingleOrArray.SS.ManValue);
	 break;
      case IntegerLeafExp :
	 return (Exp->ExpDef.IntegerConstant);
	 break;
      case NegateExp :
	 return (- EvalManExp (Exp->ExpDef.
		     SonsForBinOp.LeftExp));
	 break;
      case PlusExp :
	 return (EvalManExp (Exp->ExpDef.
		   SonsForBinOp.LeftExp) +
                 EvalManExp (Exp->ExpDef.
		   SonsForBinOp.RightExp));
	 break;
      case MinusExp :
	 return (EvalManExp (Exp->ExpDef.
		   SonsForBinOp.LeftExp) -
                 EvalManExp (Exp->ExpDef.
		   SonsForBinOp.RightExp));
	 break;
      case MultExp :
	 return (EvalManExp (Exp->ExpDef.
		   SonsForBinOp.LeftExp) *
                 EvalManExp (Exp->ExpDef.
		   SonsForBinOp.RightExp));
	 break;
      default :
	 perr (50, Exp->ExpPos);
	 break;
   }
}

void ChkInitialDimsOfInPars (ST)
   SymbolTable ST;
{
   if (ST == NULL)
      return;
   if ((ST->IsArray == true) &&
       (ST->SingleOrArray.AS.DimensionBounds == NULL))
      perr (32, ST->SymbolId.IdPos);
   ChkInitialDimsOfInPars (ST->Next);
}

void ChkInitialDimsOfOutParsAndLocals (ST)
   SymbolTable ST;
{
   if (ST == NULL)
      return;
   if ((ST->IsArray == true) &&
       (ST->SingleOrArray.AS.DimensionBounds != NULL))
      perr (33, ST->SymbolId.IdPos);
   ChkInitialDimsOfOutParsAndLocals (ST->Next);
}

void ChkIfAllDimsDefined (ST)
   SymbolTable ST;
{
   if (ST == NULL)
      return;
   if ((ST->IsArray == true) &&
       (ST->SingleOrArray.AS.DimensionBounds == NULL))
      perr (37, ST->SymbolId.IdPos);
   ChkIfAllDimsDefined (ST->Next);
}

void InitDimsOfTable (ST)
   SymbolTable ST;
{
   int i;
   if (ST == NULL)
      return;
   if (ST->IsArray == true) {
#ifdef DEBUG
      if (ST->SingleOrArray.AS.DimensionBounds != NULL)
	 Crash ("InitDimsOfTable");
#endif
      ST->SingleOrArray.AS.DimensionBounds =
	 (int *) tmalloc (sizeof (int) *
			  ST->SingleOrArray.AS.NumberOfDimensions);
      for (i=0; i<ST->SingleOrArray.AS.NumberOfDimensions; i++)
	 ST->SingleOrArray.AS.DimensionBounds[i] = 0;
   }
   InitDimsOfTable (ST->Next);
}

void EvalFixedDims (Tuple, ArrayDef, Index)
   Expression Tuple;
   SymbolTable ArrayDef;
   int Index;
{
   int IV;
#ifdef DEBUG
   if (ArrayDef->IsArray == false)
      Crash ("EvalFixedDims");
#endif
   if ((Tuple == NULL) ||
       (Tuple->ExpDef.SonsForTuple.FirstExpression == NULL))
      return;
   IV = EvalManExp (Tuple->ExpDef.SonsForTuple.FirstExpression);
   if (IV < 0)
      perr (34, Tuple->ExpDef.SonsForTuple.FirstExpression->ExpPos);
   if (IV + 1 > ArrayDef->SingleOrArray.AS.DimensionBounds[Index])
      ArrayDef->SingleOrArray.AS.DimensionBounds[Index] = IV + 1;
   EvalFixedDims (Tuple->ExpDef.SonsForTuple.RestOfTuple,
		  ArrayDef,
		  Index + 1);
}

void EvalLHSideExp (Exp)
   Expression Exp;
{
   switch (Exp->ExpKind) {
      case IdentLeafExp :
	 break;
      case IndexedExp :
	 EvalFixedDims (Exp->ExpDef.SonsForIndexed.IndexTupleExp,
			Exp->ExpDef.SonsForIndexed.ThisArray, 0);
	 break;
      case InitExp :
      case LpInitExp : {
         int DV;
	 Expression ExpSig;
	 SymbolTable SymDef;
         ExpSig = Exp->ExpDef.SonsForDelay.DelaySigExp;
	 EvalLHSideExp (ExpSig);
         DV = EvalManExp (Exp->ExpDef.SonsForDelay.DelayValExp);
	 if (DV < 0)
	    perr (17, Exp->ExpDef.SonsForDelay.DelayValExp->ExpPos);
	 switch (ExpSig->ExpKind) {
	    case IdentLeafExp :
	       SymDef = ExpSig->ExpDef.ThisSymbol;
	       break;
	    case IndexedExp :
	       SymDef = ExpSig->ExpDef.SonsForIndexed.ThisArray;
	       break;
	    default :
	       perr (16, ExpSig->ExpPos);
	       break;
         }
	 if (SymDef->MaxDelay < DV)
	    SymDef->MaxDelay = DV;
	 }
	 break;
      case TupleExp :
	 EvalLHSideExp (Exp->ExpDef.SonsForTuple.FirstExpression);
	 if (Exp->ExpDef.SonsForTuple.RestOfTuple != NULL)
	    EvalLHSideExp (Exp->ExpDef.SonsForTuple.RestOfTuple);
         break;
#ifdef DEBUG
      default :
	 Crash ("EvalLHSideExp");
	 break;
#endif
   }
}

void EvalRHSideExp (Exp)
   Expression Exp;
{
   switch (Exp->ExpKind) {
      case DelayExp :
      case LpDelayExp : {
         int DV;
	 Expression ExpSig;
	 SymbolTable SymDef;
         DV = EvalManExp (Exp->ExpDef.SonsForDelay.DelayValExp);
	 if (DV < 0)
	    perr (17, Exp->ExpDef.SonsForDelay.DelayValExp->ExpPos);
         ExpSig = Exp->ExpDef.SonsForDelay.DelaySigExp;
	 switch (ExpSig->ExpKind) {
	    case IdentLeafExp :
	       SymDef = ExpSig->ExpDef.ThisSymbol;
	       break;
	    case IndexedExp :
	       SymDef = ExpSig->ExpDef.SonsForIndexed.ThisArray;
	       break;
	    default :
	       perr (16, ExpSig->ExpPos);
	       break;
         }
	 if (SymDef->MaxDelay < DV)
	    SymDef->MaxDelay = DV;
	 }
	 break;
      case FuncAppExp :
	 Exp->ExpDef.SonsForFuncApp.NrOfApps++;
	 EvalRHSideExp (Exp->ExpDef.SonsForFuncApp.ActualParameters);
	 break;
      case UserFuncExp :
	 EvalRHSideExp (Exp->ExpDef.SonsForUserFunc.ActualUserParameters);
	 break;
      case TimeMuxExp :
	 EvalRHSideExp (Exp->ExpDef.SonsForTimeMux.TimeMuxInputs);
	 break;
      case ReSampExp :
	 EvalRHSideExp (Exp->ExpDef.SonsForReSamp.ReSampInputs);
	 break;
      case TupleExp :
	 EvalRHSideExp (Exp->ExpDef.SonsForTuple.FirstExpression);
	 if (Exp->ExpDef.SonsForTuple.RestOfTuple != NULL)
	    EvalRHSideExp (Exp->ExpDef.SonsForTuple.RestOfTuple);
	 break;
      case CastExp :
	 EvalRHSideExp (Exp->ExpDef.CastedExp);
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
	 EvalRHSideExp (Exp->ExpDef.SonsForBinOp.LeftExp);
	 EvalRHSideExp (Exp->ExpDef.SonsForBinOp.RightExp);
	 break;
      case CosExp :
      case SinExp :
      case SqrtExp :
      case NegateExp :
      case LeftShiftExp :
      case RightShiftExp :
      case NotExp :
	 EvalRHSideExp (Exp->ExpDef.SonsForBinOp.LeftExp);
	 break;
      case IdentLeafExp :
      case IndexedExp :
      case FixedLeafExp :
      case IntegerLeafExp :
	 break;
      case CondExp :
	 EvalRHSideExp (Exp->ExpDef.SonsForCond.IfExp);
	 EvalRHSideExp (Exp->ExpDef.SonsForCond.ThenExp);
	 if (Exp->ExpDef.SonsForCond.ElseExp != NULL)
	    EvalRHSideExp (Exp->ExpDef.SonsForCond.ElseExp);
	 break;
      default :
	 perr (7, Exp->ExpPos);
	 break;
   }
}

void EvalManExpsOfVDL (VDL)
   ValueDefinitionList VDL;
{
   if (VDL == NULL)
      return;
   switch (VDL->ValDefKind) {
      case SingleDef : {
	 Expression LHS, RHS;
	 LHS = VDL->ValDef.SDef->LeftHandSide;
	 RHS = VDL->ValDef.SDef->RightHandSide;
	 if ((RHS->ExpKind == TimeMuxExp) &&
	     (RHS->ExpDef.SonsForTimeMux.NrArrayOutputs > 0)) {
            SymbolTable ArrSymDef;
#ifdef DEBUG
            if (LHS->ExpKind != IndexedExp)
	       Crash ("EvalManExpsOfVDL 1");
#endif
            ArrSymDef = LHS->ExpDef.SonsForIndexed.ThisArray;
#ifdef DEBUG
            if (ArrSymDef->SingleOrArray.AS.NumberOfDimensions != 1)
	       Crash ("EvalManExpsOfVDL 2");
#endif
	    if (ArrSymDef->SingleOrArray.AS.DimensionBounds[0] <
	        RHS->ExpDef.SonsForTimeMux.NrArrayOutputs)
	       ArrSymDef->SingleOrArray.AS.DimensionBounds[0] =
	          RHS->ExpDef.SonsForTimeMux.NrArrayOutputs;
	 }
	 EvalLHSideExp (LHS);
	 EvalRHSideExp (RHS);
	 }
	 break;
      case IteratedDef : {
	 int LB, UB;
	 IteratedDefinition ID;
	 ID = VDL->ValDef.IDef;
	 if (ID->IDKind != ForLoop) return;
         LB = EvalManExp (ID->LowerBound);
	 UB = EvalManExp (ID->UpperBound);
	 if ((LB < 0) || (UB < 0))
	    perr (30, ID->LowerBound->ExpPos);
	 for (ID->ThisIterator->SingleOrArray.SS.ManValue = LB;
	      ID->ThisIterator->SingleOrArray.SS.ManValue <= UB;
	      ID->ThisIterator->SingleOrArray.SS.ManValue++) {
	    EvalManExpsOfVDL (ID->IterValDefs);
         }
	 }
	 break;
#ifdef DEBUG
      default :
         Crash ("EvalManExpsOfVDL 3");
	 break;
#endif
   }
   EvalManExpsOfVDL (VDL->Next);
}

void CopyDims (BoundsTo, ND, NFD, BoundsFrom)
   int BoundsTo[], ND, NFD, BoundsFrom[];
{
   int i;
   for (i=ND-NFD; i<ND; i++)
      if (BoundsFrom[i-ND+NFD] > BoundsTo[i])
         BoundsTo[i] = BoundsFrom[i-ND+NFD];
}

void CompDims (BoundsTo, ND, NFD, BoundsFrom, pos)
   int BoundsTo[], ND, NFD, BoundsFrom[];
   Position pos;
{
   int i;
   for (i=ND-NFD; i<ND; i++)
      if (BoundsFrom[i-ND+NFD] != BoundsTo[i])
	 /* perr (21, pos) PH 6/28*/;
}

void ChkIfEqualDims (BoundsFrom, MaxDFrom, NFD,
		     BoundsTo, MaxDTo, pos)
   int BoundsFrom[], MaxDFrom, NFD, BoundsTo[], MaxDTo;
   Position pos;
{
   int i, j;
   i = MaxDFrom-NFD;
   j = MaxDTo-NFD;
   while (i<MaxDFrom) {
      if (BoundsTo[j] != BoundsFrom[i])
	 /* perr (21, pos) PH 6/28*/;
      i++; j++;
   }
#ifdef DEBUG
   if (j != MaxDTo)
      Crash ("ChkIfEqualDims");
#endif
}

void ChkDimsOfOutParams (ActParams, FormParams, IsProcedure, pos)
   Expression ActParams;
   SymbolTable FormParams;
   bool IsProcedure;
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
      Crash ("ChkDimsOfOutParams 1");
#endif
   ActP = ActParams->ExpDef.SonsForTuple.FirstExpression;
   switch (ActP->ExpKind) {
      case IdentLeafExp :
	 break;
      case IndexedExp : {
	 int NFD;
         NFD = NrFreeDims
		 (StripFixedDims
		    (ActP->ExpDef.SonsForIndexed.IndexTupleExp));
	 if (NFD != 0) {
#ifdef DEBUG
            if ((ActP->ExpDef.SonsForIndexed.ThisArray->IsArray == false) ||
                (FormParams->IsArray == false))
               Crash ("ChkDimsOfOutParams 2");
#endif
	    if (IsProcedure == true)
	       CompDims (ActP->ExpDef.SonsForIndexed.ThisArray
			   ->SingleOrArray.AS.DimensionBounds,
		         ActP->ExpDef.SonsForIndexed.ThisArray
			   ->SingleOrArray.AS.NumberOfDimensions,
                         NFD,
		         FormParams->SingleOrArray.AS.DimensionBounds,
			 ActP->ExpPos);
            else
	       CopyDims (ActP->ExpDef.SonsForIndexed.ThisArray
			   ->SingleOrArray.AS.DimensionBounds,
		         ActP->ExpDef.SonsForIndexed.ThisArray
			   ->SingleOrArray.AS.NumberOfDimensions,
                         NFD,
		         FormParams->SingleOrArray.AS.DimensionBounds);
         }
	 }
	 break;
#ifdef DEBUG
      default :
	 Crash ("ChkDimsOfOutParams 3");
	 break;
#endif
   }
   ChkDimsOfOutParams
     (ActParams->ExpDef.SonsForTuple.RestOfTuple,
      FormParams->Next,
      IsProcedure,
      pos);
}

void ChkDimsOfTuple (LHS, RHS, IsProcedure)
   Expression LHS, RHS;
   bool IsProcedure;
{
   void ChkDimsOfSD ();
   if (LHS == NULL)
      if (RHS == NULL)
	 return;
      else
	 perr (31, RHS->ExpPos);
   else
      if (RHS == NULL)
	 perr (31, LHS->ExpPos);
   ChkDimsOfSD
     (LHS->ExpDef.SonsForTuple.FirstExpression,
      RHS->ExpDef.SonsForTuple.FirstExpression,
      IsProcedure);
   ChkDimsOfTuple
     (LHS->ExpDef.SonsForTuple.RestOfTuple,
      RHS->ExpDef.SonsForTuple.RestOfTuple,
      IsProcedure);
}

void ChkDimsOfCondTuple (LHS, RHS, IsProcedure)
   Expression LHS, RHS;
   bool IsProcedure;
{
   void ChkDimsOfSD ();
#ifdef DEBUG
   if (LHS->ExpKind != TupleExp)
      Crash ("ChkDimsOfCondTuple");
#endif
   switch (RHS->ExpKind) {
      case TupleExp :
	 ChkDimsOfTuple (LHS, RHS, IsProcedure);
	 break;
      case CondExp :
      case FuncAppExp :
	 ChkDimsOfSD (LHS, RHS, IsProcedure);
	 break;
      default :
	 perr (31, LHS->ExpPos);
	 break;
   }
}

void ChkDimsOfSD (LHS, RHS, IsProcedure)
   Expression LHS, RHS;
   bool IsProcedure;
{
   switch (LHS->ExpKind) {
      case IdentLeafExp :
	 break;
      case IndexedExp : {
	 int NFD;
         NFD = NrFreeDims
		 (StripFixedDims
		    (LHS->ExpDef.SonsForIndexed.IndexTupleExp));
	 if (NFD != 0) {
	    switch (RHS->ExpKind) {
	       case FuncAppExp :
#ifdef DEBUG
                  if ((LHS->ExpDef.SonsForIndexed.ThisArray
			 ->IsArray == false) ||
                      (RHS->ExpDef.SonsForFuncApp.ThisFunction
			 ->OutputParameters->IsArray == false))
                     Crash ("ChkDimsOfSD 1");
#endif
		  if (IsProcedure == true)
	             CompDims (LHS->ExpDef.SonsForIndexed.ThisArray
			         ->SingleOrArray.AS.DimensionBounds,
			       LHS->ExpDef.SonsForIndexed.ThisArray
			         ->SingleOrArray.AS.NumberOfDimensions,
                               NFD,
		               RHS->ExpDef.SonsForFuncApp.ThisFunction
			         ->OutputParameters
			         ->SingleOrArray.AS.DimensionBounds,
			       LHS->ExpPos);
		  else
	             CopyDims (LHS->ExpDef.SonsForIndexed.ThisArray
			         ->SingleOrArray.AS.DimensionBounds,
			       LHS->ExpDef.SonsForIndexed.ThisArray
			         ->SingleOrArray.AS.NumberOfDimensions,
                               NFD,
		               RHS->ExpDef.SonsForFuncApp.ThisFunction
			         ->OutputParameters
			         ->SingleOrArray.AS.DimensionBounds);
		  break;
	       case CondExp :
                  ChkDimsOfSD
		    (LHS, RHS->ExpDef.SonsForCond.ThenExp, IsProcedure);
		  if (RHS->ExpDef.SonsForCond.ElseExp != NULL)
		     ChkDimsOfSD
		       (LHS, RHS->ExpDef.SonsForCond.ElseExp, IsProcedure);
		  break;
	       case IndexedExp :
		  if (IsProcedure == false)
		     perr (16, RHS->ExpPos);
                  ChkIfEqualDims (RHS->ExpDef.SonsForIndexed.ThisArray
		                    ->SingleOrArray.AS.DimensionBounds,
                                  RHS->ExpDef.SonsForIndexed.ThisArray
		                    ->SingleOrArray.AS.NumberOfDimensions,
                                  NrFreeDims
		                    (StripFixedDims
		                      (RHS->ExpDef.SonsForIndexed.IndexTupleExp)),
                                  LHS->ExpDef.SonsForIndexed.ThisArray
		                    ->SingleOrArray.AS.DimensionBounds,
                                  LHS->ExpDef.SonsForIndexed.ThisArray
		                    ->SingleOrArray.AS.NumberOfDimensions,
		                  RHS->ExpPos);
		  break;
	       case UserFuncExp :
               case TimeMuxExp :
               case ReSampExp :
		  break;
	       case DelayExp :
		  ChkDimsOfSD
		    (LHS, RHS->ExpDef.SonsForDelay.DelaySigExp, IsProcedure);
		  break;
               default :
		  perr (16, RHS->ExpPos);
		  break;
	    }
         }
	 }
	 break;
      case TupleExp :
	 switch (RHS->ExpKind) {
	    case FuncAppExp :
	       ChkDimsOfOutParams
		 (LHS,
		  RHS->ExpDef.SonsForFuncApp.ThisFunction
		    ->OutputParameters,
		  IsProcedure,
		  RHS->ExpPos);
	       break;
	    case TimeMuxExp :
	    case ReSampExp :
	       break;
            case TupleExp :
	       ChkDimsOfTuple (LHS, RHS, IsProcedure);
	       break;
	    case CondExp :
	       ChkDimsOfCondTuple
		 (LHS, RHS->ExpDef.SonsForCond.ThenExp, IsProcedure);
               if (RHS->ExpDef.SonsForCond.ElseExp != NULL)
		  ChkDimsOfCondTuple
		    (LHS, RHS->ExpDef.SonsForCond.ElseExp, IsProcedure);
	       break;
            default :
	       perr (16, RHS->ExpPos);
	       break;
         }
	 break;
      case InitExp :
      case LpInitExp :
	 /* do nothing */
	 break;
#ifdef DEBUG
      default :
         Crash ("ChkDimsOfSD 2");
         break;
#endif
   }
}

void ChkDimsOfVDL (VDL, IsProcedure)
   ValueDefinitionList VDL;
   bool IsProcedure;
{
   if (VDL == NULL)
      return;
   switch (VDL->ValDefKind) {
      case SingleDef :
	 ChkDimsOfSD (VDL->ValDef.SDef->LeftHandSide,
		      VDL->ValDef.SDef->RightHandSide,
		      IsProcedure);
	 break;
      case IteratedDef :
	 if (VDL->ValDef.IDef->IDKind == DoLoop)
             ChkDimsOfSD (VDL->ValDef.IDef->ExitDef->LeftHandSide,
                            VDL->ValDef.IDef->ExitDef->RightHandSide);
	 ChkDimsOfVDL (VDL->ValDef.IDef->IterValDefs,
		       IsProcedure);
	 break;
#ifdef DEBUG
      default :
         Crash ("ChkDimsOfVDL");
	 break;
#endif
   }
   ChkDimsOfVDL (VDL->Next, IsProcedure);
}

void ChkDimsOfIndexedPar (AP, FP)
   Expression AP;
   SymbolTable FP;
{
   ChkIfEqualDims (AP->ExpDef.SonsForIndexed.ThisArray
		     ->SingleOrArray.AS.DimensionBounds,
                   AP->ExpDef.SonsForIndexed.ThisArray
		     ->SingleOrArray.AS.NumberOfDimensions,
                   NrFreeDims
		     (StripFixedDims
		       (AP->ExpDef.SonsForIndexed.IndexTupleExp)),
                   FP->SingleOrArray.AS.DimensionBounds,
		   FP->SingleOrArray.AS.NumberOfDimensions,
		   AP->ExpPos);
}

void ChkDimsOfArrayPar (ActP, FormP)
   Expression ActP;
   SymbolTable FormP;
{
#ifdef DEBUG
   if (FormP->IsArray == false)
      Crash ("ChkDimsOfArrayPar 1");
#endif
   switch (ActP->ExpKind) {
      case IndexedExp :
         ChkDimsOfIndexedPar (ActP, FormP);
         break;
      case FuncAppExp : {
         SymbolTable OutPar;
         OutPar = ActP->ExpDef.SonsForFuncApp.ThisFunction
		    ->OutputParameters;
#ifdef DEBUG
         if (OutPar->IsArray == false)
	    Crash ("ChkDimsOfArrayPar 2");
#endif
	 ChkIfEqualDims (OutPar->SingleOrArray.AS.DimensionBounds,
			 OutPar->SingleOrArray.AS.NumberOfDimensions,
			 OutPar->SingleOrArray.AS.NumberOfDimensions,
			 FormP->SingleOrArray.AS.DimensionBounds,
			 FormP->SingleOrArray.AS.NumberOfDimensions,
			 ActP->ExpPos);
	 }
	 break;
      case DelayExp :
      case LpDelayExp :
#ifdef DEBUG
         if (ActP->ExpDef.SonsForDelay.DelaySigExp->ExpKind != IndexedExp)
	    Crash ("ChkDimsOfArrayPar 3");
#endif
	 ChkDimsOfIndexedPar (ActP->ExpDef.SonsForDelay.DelaySigExp,
			      FormP);
	 break;
      case CondExp :
	 ChkDimsOfArrayPar (ActP->ExpDef.SonsForCond.ThenExp, FormP);
	 if (ActP->ExpDef.SonsForCond.ElseExp != NULL)
	    ChkDimsOfArrayPar (ActP->ExpDef.SonsForCond.ElseExp, FormP);
	 break;
      default :
         perr (16, ActP->ExpPos);
         break;
   }
}

void ChkDimsOfInpParams (ActParams, FormParams, pos)
   Expression ActParams;
   SymbolTable FormParams;
   Position pos;
{
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
      Crash ("ChkDimsOfInpParams 1");
#endif
   if (FormParams->IsArray == true)
      ChkDimsOfArrayPar
	(ActParams->ExpDef.SonsForTuple.FirstExpression,
	 FormParams);
   ChkDimsOfInpParams
     (ActParams->ExpDef.SonsForTuple.RestOfTuple,
      FormParams->Next,
      pos);
}

void ChkDimsOfLOFA (LOFA)
   Expression LOFA;
{
   if (LOFA == NULL)
      return;
#ifdef DEBUG
   if (LOFA->ExpKind != FuncAppExp)
      Crash ("ChkDimsOfLOFA");
#endif
   ChkDimsOfInpParams
     (LOFA->ExpDef.SonsForFuncApp.ActualParameters,
      LOFA->ExpDef.SonsForFuncApp.ThisFunction->InputParameters,
      LOFA->ExpPos);
   ChkDimsOfLOFA (LOFA->ExpDef.SonsForFuncApp.NextFuncApp);
}

/*
 * Omit this addition of IDel because Flow2C will take care of it.
 * Phu Hoang 7/22/91
 */
void GenInputVar (RefSymDef, FDL)
   SymbolTable *RefSymDef;
   FunctionDefinitionList FDL;
{
   SymbolTable OldST, NewST;
   ValueDefinitionList VDL;
   /* OldST = *RefSymDef;
   NewST = NEW (SymbolTableNode);
   *RefSymDef = NewST;
   *NewST = *OldST;
   NewST->MaxDelay = -1;
   NewST->IsCyclic = false;
   OldST->SymbolId.IdName = "IDel";
   OldST->SymKind = LocalSym;
   OldST->SingleOrArray.SS.SSDef =
      MkSingleDefinition
	(MkExpIdentLeaf (OldST->SymbolId.IdPos, OldST),
	 MkExpIdentLeaf (OldST->SymbolId.IdPos, NewST));
   OldST->SingleOrArray.SS.SSDef->RightHandSide->ExpType =
      OldST->SymType;
   OldST->Next = FDL->LocalSymbols;
   FDL->LocalSymbols = OldST;
   VDL = MkValDefSingle (OldST->SingleOrArray.SS.SSDef);
   VDL->Next = FDL->ValDefs;
   FDL->ValDefs = VDL; */
}

void AppendToValDefs (VDL, RefValDefs)
   ValueDefinitionList VDL, *RefValDefs;
{
   if (*RefValDefs == NULL)
      *RefValDefs = VDL;
   else
      AppendToValDefs (VDL, &((*RefValDefs)->Next));
}

/*
 * Omit this addition of ODel because Flow2C will take care of it.
 * Phu Hoang 7/22/91
 */
void GenOutputVar (RefSymDef, FDL)
   SymbolTable *RefSymDef;
   FunctionDefinitionList FDL;
{
   SymbolTable OldST, NewST;
   /* OldST = *RefSymDef;
   NewST = NEW (SymbolTableNode);
   *RefSymDef = NewST;
   *NewST = *OldST;
   NewST->MaxDelay = -1;
   NewST->IsCyclic = false;
   NewST->SymbolId.IdName = "ODel";
   NewST->SingleOrArray.SS.SSDef =
      MkSingleDefinition
	(MkExpIdentLeaf (OldST->SymbolId.IdPos, NewST),
	 MkExpIdentLeaf (OldST->SymbolId.IdPos, OldST));
   NewST->SingleOrArray.SS.SSDef->RightHandSide->ExpType =
      OldST->SymType;
   OldST->SymKind = LocalSym;
   OldST->Next = FDL->LocalSymbols;
   FDL->LocalSymbols = OldST;
   AppendToValDefs (MkValDefSingle (NewST->SingleOrArray.SS.SSDef),
		    &(FDL->ValDefs));  */
}

void SymTabGenInputVar (RefST, FDL)
   SymbolTable *RefST;
   FunctionDefinitionList FDL;
{
   if (*RefST == NULL)
      return;
   if ((*RefST)->MaxDelay > -1) {
      if ((*RefST)->IsArray == true)
         perr (25, (*RefST)->SymbolId.IdPos);
      GenInputVar (RefST, FDL);
   }
   SymTabGenInputVar (&((*RefST)->Next), FDL);
}

void SymTabGenOutputVar (RefST, FDL)
   SymbolTable *RefST;
   FunctionDefinitionList FDL;
{
   if (*RefST == NULL)
      return;
   if ((*RefST)->MaxDelay > -1) {
      if ((*RefST)->IsArray == true)
         perr (25, (*RefST)->SymbolId.IdPos);
      GenOutputVar (RefST, FDL);
   }
   SymTabGenOutputVar (&((*RefST)->Next), FDL);
}

bool SymTabDelaysExist (ST)
   SymbolTable ST;
{
   if (ST == NULL)
      return (false);
   if (ST->MaxDelay > -1)
      return (true);
   else
      return (SymTabDelaysExist (ST->Next));
}

bool FuncAppsDelaysExist (LOFA)
   Expression LOFA;
{
   if (LOFA == NULL)
      return (false);
#ifdef DEBUG
   if (LOFA->ExpKind != FuncAppExp)
      Crash ("FuncAppDelaysExist");
#endif
   if (LOFA->ExpDef.SonsForFuncApp.ThisFunction->DelaysExist == true)
      return (true);
   else
      return (FuncAppsDelaysExist
		(LOFA->ExpDef.SonsForFuncApp.NextFuncApp));
}

bool FuncDefDelaysExist (FDL)
   FunctionDefinitionList FDL;
{
   bool rtv;
   rtv = false;
   if (SymTabDelaysExist (FDL->InputParameters) == true) {
      rtv = true;
      SymTabGenInputVar (&(FDL->InputParameters), FDL);
   }
   if (SymTabDelaysExist (FDL->OutputParameters) == true) {
      rtv = true;
      SymTabGenOutputVar (&(FDL->OutputParameters), FDL);
   }
   if (SymTabDelaysExist (FDL->LocalSymbols) == true)
      rtv = true;
   if (rtv == true)
      return (true);
   else
      return (FuncAppsDelaysExist (FDL->ListOfFuncApps));
}

bool NoIntsInTable (ST)
   SymbolTable ST;
{
   if (ST == NULL)
      return (true);
   if ((ST->SymType.SymTpKind == IntType) &&
       (ST->SymKind != Iterator))
      return (false);
   else
      return (NoIntsInTable (ST->Next));
}

bool NoIntsInFDL (FDL)
   FunctionDefinitionList FDL;
{
   if (NoIntsInTable (FDL->InputParameters) == false)
      return (false);
   if (NoIntsInTable (FDL->OutputParameters) == false)
      return (false);
   if (NoIntsInTable (FDL->LocalSymbols) == false)
      return (false);
   else
      return (true);
}

void Iter7FuncDefList (FDL)
   FunctionDefinitionList FDL;
{
   if (FDL == NULL)
      return;
   if ((NoIntsInFDL (FDL) == true) &&
       (FDL->IsProcedure == false)) {
      FDL->IsProcedure = false;
      ChkInitialDimsOfInPars (FDL->InputParameters);
      ChkInitialDimsOfOutParsAndLocals (FDL->OutputParameters);
      ChkInitialDimsOfOutParsAndLocals (FDL->LocalSymbols);
      InitDimsOfTable (FDL->OutputParameters);
      InitDimsOfTable (FDL->LocalSymbols);
      EvalManExpsOfVDL (FDL->ValDefs);
   } else {
      FDL->IsProcedure = true;
      ChkIfAllDimsDefined (FDL->InputParameters);
      ChkIfAllDimsDefined (FDL->OutputParameters);
      ChkIfAllDimsDefined (FDL->LocalSymbols);
   }
   ChkDimsOfVDL (FDL->ValDefs, FDL->IsProcedure);
   ChkDimsOfLOFA (FDL->ListOfFuncApps);
   FDL->DelaysExist = FuncDefDelaysExist (FDL);
/*   if ((FDL->DelaysExist == true) &&
       (FDL->IsProcedure == true))
      perr (38, FDL->FunctionId.IdPos); */
   Iter7FuncDefList (FDL->OrderedNext);
}

void pass7 ()
{
   Iter7FuncDefList (SemGraph.OrderedFuncDefs);
}
