/* $Id$ */

/* Author : Chris Scheers */
/* pass4.c -- version 1.8 (IMEC)		last updated: 3/1/89 */
static char *SccsId = "@(#)pass4.c	1.8 (IMEC)	89/03/01";

#include <stdio.h>
#include "sfg.h"

extern NodeType *TheTree;
extern SemanticGraph SemGraph;
extern Expression MkExp (), MkExpIdentLeaf (), MkExpIntegerLeaf (),
		  MkExpBinOp (), MkExpTuple (), MkIndexTuple (),
		  MkExpIndexedExp (), MkExpManifest ();
extern int SynEvalConstExp ();
extern Position MkPosition ();
extern SymbolTable FindSymDef ();
extern SymbolType MkSymType ();
extern FunctionDefinitionList FindFuncDef ();
extern bool HasDimension ();

Expression MkExpFixedLeaf (pos, FM, FS, FStr)
   Position pos;
   int FM, FS;
   char *FStr;
{
   Expression Exp;
   Exp = MkExp (pos, FixedLeafExp);
   Exp->ExpDef.SonsForFixedLeaf.M = FM;
   Exp->ExpDef.SonsForFixedLeaf.S = FS;
   Exp->ExpDef.SonsForFixedLeaf.FixedString = FStr;
   return (Exp);
}

Expression MkExpCast (pos, CExp, CType)
   Position pos;
   Expression CExp;
   SymbolType CType;
{
   Expression Exp;
   Exp = MkExp (pos, CastExp);
   Exp->ExpDef.CastedExp = CExp;
   Exp->ExpType = CType;
   return (Exp);
}

Expression MkExpDelay (pos, DelSig, DelVal)
   Position pos;
   Expression DelSig, DelVal;
{
   Expression Exp;
   Exp = MkExp (pos, DelayExp);
   Exp->ExpDef.SonsForDelay.DelaySigExp = DelSig;
   Exp->ExpDef.SonsForDelay.DelayValExp = DelVal;
   return (Exp);
}

Expression MkExpLpDelay (pos, DelSig, DelVal)
   Position pos;
   Expression DelSig, DelVal;
{
   Expression Exp;
   Exp = MkExp (pos, LpDelayExp);
   Exp->ExpDef.SonsForDelay.DelaySigExp = DelSig;
   Exp->ExpDef.SonsForDelay.DelayValExp = DelVal;
   return (Exp);
}

Expression MkExpInit (pos, DelSig, DelVal)
   Position pos;
   Expression DelSig, DelVal;
{
   Expression Exp;
   Exp = MkExp (pos, InitExp);
   Exp->ExpDef.SonsForDelay.DelaySigExp = DelSig;
   Exp->ExpDef.SonsForDelay.DelayValExp = DelVal;
   return (Exp);
}

Expression MkExpLpInit (pos, DelSig, DelVal)
   Position pos;
   Expression DelSig, DelVal;
{
   Expression Exp;
   Exp = MkExp (pos, LpInitExp);
   Exp->ExpDef.SonsForDelay.DelaySigExp = DelSig;
   Exp->ExpDef.SonsForDelay.DelayValExp = DelVal;
   return (Exp);
}

Expression MkExpFuncApp (pos, RefFunc, ActPars)
   Position pos;
   FunctionDefinitionList RefFunc;
   Expression ActPars;
{
   Expression Exp;
   Exp = MkExp (pos, FuncAppExp);
   Exp->ExpDef.SonsForFuncApp.ThisFunction = RefFunc;
   Exp->ExpDef.SonsForFuncApp.ActualParameters = ActPars;
   Exp->ExpDef.SonsForFuncApp.NrOfApps = 0;
   return (Exp);
}

Expression MkExpUserFunc (pos, UFName, ActPars)
   Position pos;
   char *UFName;
   Expression ActPars;
{
   Expression Exp;
   Exp = MkExp (pos, UserFuncExp);
   Exp->ExpDef.SonsForUserFunc.UserFuncId.IdName = UFName;
   Exp->ExpDef.SonsForUserFunc.UserFuncId.IdPos = pos;
   Exp->ExpDef.SonsForUserFunc.ActualUserParameters = ActPars;
   return (Exp);
}

Expression MkExpTimeMux (pos, Inputs)
   Position pos;
   Expression Inputs;
{
   Expression Exp;
   Exp = MkExp (pos, TimeMuxExp);
   Exp->ExpDef.SonsForTimeMux.TimeMuxInputs = Inputs;
   Exp->ExpDef.SonsForTimeMux.NrArrayInputs = 0;
   Exp->ExpDef.SonsForTimeMux.NrArrayOutputs = 0;
   Exp->ExpDef.SonsForTimeMux.NoPhase = -1;
   return (Exp);
}

Expression MkExpReSamp (pos, Inputs)
   Position pos;
   Expression Inputs;
{
   Expression Exp;
   Exp = MkExp (pos, ReSampExp);
   Exp->ExpDef.SonsForReSamp.ReSampInputs = Inputs;
   Exp->ExpDef.SonsForReSamp.Scale = 0;
   Exp->ExpDef.SonsForReSamp.Phase = 0;
   return (Exp);
}

Expression MkExpCond (pos, IExp, TExp, EExp)
   Position pos;
   Expression IExp, TExp, EExp;
{
   Expression Exp;
   Exp = MkExp (pos, CondExp);
   Exp->ExpDef.SonsForCond.IfExp = IExp;
   Exp->ExpDef.SonsForCond.ThenExp = TExp;
   Exp->ExpDef.SonsForCond.ElseExp = EExp;
   return (Exp);
}

SymbolTable FindIdRHS (SynId, SemFDL)
   NodeType *SynId;
   FunctionDefinitionList SemFDL;
{
   SymbolTable SymDef;
   SymDef = FindSymDef (SemFDL->LocalSymbols, Left (SynId));
   if (SymDef != NULL)
      return (SymDef);
   SymDef = FindSymDef (SemFDL->InputParameters, Left (SynId));
   if (SymDef != NULL)
      return (SymDef);
   SymDef = FindSymDef (SemFDL->OutputParameters, Left (SynId));
   if (SymDef != NULL)
      return (SymDef);
   else
      perr (13, MkPosition (SynId));
}

void AddToListOfFuncApps (SemFuncAppExp, SemFDL)
   Expression SemFuncAppExp;
   FunctionDefinitionList SemFDL;
{
#ifdef DEBUG
   if (SemFuncAppExp->ExpKind != FuncAppExp)
      Crash ("AddToListOfFuncApps");
#endif DEBUG
   SemFuncAppExp->ExpDef.SonsForFuncApp.NextFuncApp =
      SemFDL->ListOfFuncApps;
   SemFDL->ListOfFuncApps = SemFuncAppExp;
}

void AddToListOfFixedLeafs (SemFixedExp)
   Expression SemFixedExp;
{
#ifdef DEBUG
   if (SemFixedExp->ExpKind != FixedLeafExp)
      Crash ("AddToListOfFixedLeafs");
#endif
   SemFixedExp->ExpDef.SonsForFixedLeaf.NextFixed =
      SemGraph.ListOfFixedLeafs;
   SemGraph.ListOfFixedLeafs = SemFixedExp;
}

void RmFromListOfFixedLeafs (SemFixedExp)
   Expression SemFixedExp;
{
#ifdef DEBUG
   if (SemFixedExp->ExpKind != FixedLeafExp)
      Crash ("RmFromListOfFixedLeafs");
#endif
   if (SemGraph.ListOfFixedLeafs == SemFixedExp)
      SemGraph.ListOfFixedLeafs =
	 SemGraph.ListOfFixedLeafs->ExpDef.SonsForFixedLeaf.NextFixed;
   else {
      Expression FLIter;
      for (FLIter = SemGraph.ListOfFixedLeafs;
	   FLIter->ExpDef.SonsForFixedLeaf.NextFixed != SemFixedExp;
	   FLIter = FLIter->ExpDef.SonsForFixedLeaf.NextFixed);
      FLIter->ExpDef.SonsForFixedLeaf.NextFixed =
	 SemFixedExp->ExpDef.SonsForFixedLeaf.NextFixed;
   }
}

Expression MkTupleRHS (SynExprSeq, SemFDL)
   NodeType *SynExprSeq;
   FunctionDefinitionList SemFDL;
{
   Expression MkExpRHS ();
   if (SynExprSeq == NULL)
      return (NULL);
#ifdef DEBUG
   if (Kind (SynExprSeq) != CommaNode)
      Crash ("MkTupleRHS");
#endif
   if (Kind (Left (SynExprSeq)) == TupleNode)
      perr (16, MkPosition (Left (SynExprSeq)));
   return (MkExpTuple
	     (MkPosition (Left (SynExprSeq)),
	      MkExpRHS (Left (SynExprSeq), SemFDL),
	      MkTupleRHS (Right (SynExprSeq), SemFDL)));
}

Expression MkExpRHS (SynExp, SemFDL)
   NodeType *SynExp;
   FunctionDefinitionList SemFDL;
{
   Position pos;
   pos = MkPosition (SynExp);
   switch (Kind (SynExp)) {
      case IdentLeaf : {
	 Expression Exp;
	 Exp = MkExpIdentLeaf (pos, FindIdRHS (SynExp, SemFDL));
	 if (HasDimension (Exp->ExpDef.ThisSymbol, 0) == false) 
	    perr (21, pos);
	 return (Exp);
	 }
	 break;
      case IntegerLeaf :
	 return (MkExpIntegerLeaf (pos, (int) Left (SynExp)));
	 break;
      case FixedLeaf : {
	 Expression Exp;
	 Exp = MkExpFixedLeaf (pos, (int) Left (SynExp),
				    (int) Right (SynExp),
				    (char *) SynExp->Attrs);
         AddToListOfFixedLeafs (Exp);
	 return (Exp);
	 }
	 break;
      case CastNode :
	 return (MkExpCast (MkPosition (Left (SynExp)),
		 	    MkExpRHS (Right (SynExp), SemFDL),
			    MkSymType (Left (SynExp),
				       MkPosition (Right (SynExp)))));
	 break;
      case CosNode :
	 return (MkExpBinOp (pos, CosExp,
			     MkExpRHS (Left (SynExp), SemFDL),
			     (Expression) NULL));
         break;
      case SinNode :
	 return (MkExpBinOp (pos, SinExp,
			     MkExpRHS (Left (SynExp), SemFDL),
			     (Expression) NULL));
         break;
      case SqrtNode :
	 return (MkExpBinOp (pos, SqrtExp,
			     MkExpRHS (Left (SynExp), SemFDL),
			     (Expression) NULL));
         break;
      case MinNode :
	 return (MkExpBinOp
		   (pos,
		    MinExp,
		    MkExpRHS (Left (SynExp), SemFDL),
		    MkExpRHS (Right (SynExp), SemFDL)));
         break;
      case MaxNode :
	 return (MkExpBinOp
		   (pos,
		    MaxExp,
		    MkExpRHS (Left (SynExp), SemFDL),
		    MkExpRHS (Right (SynExp), SemFDL)));
         break;
      case NegateNode :
	 return (MkExpBinOp (pos, NegateExp,
			     MkExpRHS (Left (SynExp), SemFDL),
			     (Expression) NULL));
         break;
      case PlusNode :
	 return (MkExpBinOp
		   (pos,
		    PlusExp,
		    MkExpRHS (Left (SynExp), SemFDL),
		    MkExpRHS (Right (SynExp), SemFDL)));
         break;
      case MinusNode :
	 return (MkExpBinOp
		   (pos,
		    MinusExp,
		    MkExpRHS (Left (SynExp), SemFDL),
		    MkExpRHS (Right (SynExp), SemFDL)));
         break;
      case MultNode :
	 return (MkExpBinOp
		   (pos,
		    MultExp,
		    MkExpRHS (Left (SynExp), SemFDL),
		    MkExpRHS (Right (SynExp), SemFDL)));
         break;
      case DivNode :
	 return (MkExpBinOp
		   (pos,
		    DivExp,
		    MkExpRHS (Left (SynExp), SemFDL),
		    MkExpRHS (Right (SynExp), SemFDL)));
	 break;
      case IntDivNode :
	 return (MkExpIntegerLeaf
		   (pos, SynEvalConstExp (SynExp)));
         break;
      case DelayNode :
      case LpDelayNode : {
	 Expression ExpSig, ExpVal;
	 SymbolTable SymDef;
         ExpSig = MkExpRHS (Left (SynExp), SemFDL);
	 switch (ExpSig->ExpKind) {
	    case IdentLeafExp :
	       SymDef = ExpSig->ExpDef.ThisSymbol;
	       break;
	    case IndexedExp :
	       SymDef = ExpSig->ExpDef.SonsForIndexed.ThisArray;
	       break;
	    default :
	       perr (16, MkPosition (Left (SynExp)));
	       break;
         }
	 SymDef->MaxDelay = 0;
         ExpVal = MkExpManifest (Right (SynExp), SemFDL);
	 if (Kind(SynExp) == DelayNode)
	     return (MkExpDelay (pos, ExpSig, ExpVal));
	 else
	     return (MkExpLpDelay (pos, ExpSig, ExpVal));
	 }
	 break;
      case LeftShiftNode :
	 return (MkExpBinOp
	   	   (pos,
		    LeftShiftExp,
		    MkExpRHS (Left (SynExp), SemFDL),
		    MkExpManifest (Right (SynExp), SemFDL)));
         break;
      case RightShiftNode :
	 return (MkExpBinOp
		   (pos,
		    RightShiftExp,
		    MkExpRHS (Left (SynExp), SemFDL),
		    MkExpManifest (Right (SynExp), SemFDL)));
         break;
      case FuncAppNode : {
	 FunctionDefinitionList SemFuncDef;
	 Expression Exp;
	 SemFuncDef =
	    FindFuncDef (SemGraph.FuncDefs, Left (Left (SynExp)));
	 if (SemFuncDef == NULL) {
/*	    perr (15, MkPosition (Left (SynExp)));  User Defined Function */
	    Exp = MkExpUserFunc
		    (MkPosition (Left (SynExp)),
		     (char *) Left (Left (SynExp)),
		     MkTupleRHS (Right (SynExp), SemFDL));
	    return (Exp);
         }
	 Exp = MkExpFuncApp
		 (MkPosition (Left (SynExp)),
		  SemFuncDef,
		  MkTupleRHS (Right (SynExp), SemFDL));
         AddToListOfFuncApps (Exp, SemFDL);
	 return (Exp);
	 }
         break;
      case BitNode : {
         Expression Exp;
	 Exp = MkExpUserFunc (MkPosition (Left (SynExp)), "bit", 
			MkTupleRHS (Left (SynExp), SemFDL));
	 return (Exp);
         }
	 break;
      case BitSelectNode : {
         Expression Exp;
	 Exp = MkExpUserFunc (MkPosition (Left (SynExp)), "bitselect", 
			MkTupleRHS (Left (SynExp), SemFDL));
	 return (Exp);
         }
	 break;
      case BitMergeNode : {
         Expression Exp;
	 Exp = MkExpUserFunc (MkPosition (Left (SynExp)), "bitmerge", 
			MkTupleRHS (Left (SynExp), SemFDL));
	 return (Exp);
         }
	 break;
      case TimeMuxNode :
      case TimeDeMuxNode : {
	 Expression Exp;
	 NodeType *LLS;
	 Position pos;
	 LLS = Left (Left (SynExp));
	 if (Kind (LLS) == ArrayNameNode)
            LLS = Left (LLS);
	 pos = MkPosition (LLS);
#ifdef DEBUG
         if (pos.FileName == NULL)
	    Crash ("MkExpRHS 1");
#endif
/*	 if (strcmp ("main", SemFDL->FunctionId.IdName))
	    perr (55, pos);  */
         Exp = MkExpTimeMux
		 (pos, MkTupleRHS (Left (SynExp), SemFDL));
         return (Exp);
	 }
	 break;
      case UpSampleNode :
      case DownSampleNode :
      case InterpolateNode :
      case DecimateNode : {
         Expression Exp;
         NodeType *LLS;
         Position pos;
         LLS = Left (Left (SynExp));
         if (Kind (LLS) == ArrayNameNode)
            LLS = Left (LLS);
         pos = MkPosition (LLS);
#ifdef DEBUG
         if (pos.FileName == NULL)
            Crash ("MkExpRHS 1");
#endif
/*       if (strcmp ("main", SemFDL->FunctionId.IdName))
            perr (55, pos);  */
         Exp = MkExpReSamp
                 (pos, MkTupleRHS (Left (SynExp), SemFDL));
         return (Exp);
         }
         break;
      case ArrayNameNode : {
	 Expression Exp;
	 Exp =
	   (MkExpIndexedExp
	      (MkPosition (Left (SynExp)),
	       FindIdRHS (Left (SynExp), SemFDL),
	       MkIndexTuple
		 (MkPosition (Left (SynExp)),
		  Right (SynExp), SemFDL)));
         if (HasDimension (Exp->ExpDef.SonsForIndexed.ThisArray,
			   NrOfDims (Right (SynExp))) == false)
	    perr (21, MkPosition (Left (SynExp)));
	 return (Exp);
	 }
	 break;
      case TupleNode :
	 return (MkTupleRHS (Left (SynExp), SemFDL));
	 break;
      case CondNode : {
         Expression EExp;
	 if (Right (SynExp) == NULL)
	    EExp = NULL;
         else
	    EExp = MkExpRHS (Right (SynExp), SemFDL);
         return (MkExpCond
		   (pos,
		    MkExpRHS (Left (Left (SynExp)), SemFDL),
		    MkExpRHS (Right (Left (SynExp)), SemFDL),
		    EExp));
	 }
	 break;
      case GreaterNode :
	 return (MkExpBinOp
		   (pos,
		    GreaterExp,
		    MkExpRHS (Left (SynExp), SemFDL),
		    MkExpRHS (Right (SynExp), SemFDL)));
         break;
      case SmallerNode :
	 return (MkExpBinOp
		   (pos,
		    SmallerExp,
		    MkExpRHS (Left (SynExp), SemFDL),
		    MkExpRHS (Right (SynExp), SemFDL)));
         break;
      case GreaterEqualNode :
	 return (MkExpBinOp
		   (pos,
		    GreaterEqualExp,
		    MkExpRHS (Left (SynExp), SemFDL),
		    MkExpRHS (Right (SynExp), SemFDL)));
         break;
      case SmallerEqualNode :
	 return (MkExpBinOp
		   (pos,
		    SmallerEqualExp,
		    MkExpRHS (Left (SynExp), SemFDL),
		    MkExpRHS (Right (SynExp), SemFDL)));
         break;
      case EqualNode :
	 return (MkExpBinOp
		   (pos,
		    EqualExp,
		    MkExpRHS (Left (SynExp), SemFDL),
		    MkExpRHS (Right (SynExp), SemFDL)));
         break;
      case NotEqualNode :
	 return (MkExpBinOp
		   (pos,
		    NotEqualExp,
		    MkExpRHS (Left (SynExp), SemFDL),
		    MkExpRHS (Right (SynExp), SemFDL)));
         break;
      case OrNode :
	 return (MkExpBinOp
		   (pos,
		    OrExp,
		    MkExpRHS (Left (SynExp), SemFDL),
		    MkExpRHS (Right (SynExp), SemFDL)));
         break;
      case ExorNode :
	 return (MkExpBinOp
		   (pos,
		    ExorExp,
		    MkExpRHS (Left (SynExp), SemFDL),
		    MkExpRHS (Right (SynExp), SemFDL)));
         break;
      case AndNode :
	 return (MkExpBinOp
		   (pos,
		    AndExp,
		    MkExpRHS (Left (SynExp), SemFDL),
		    MkExpRHS (Right (SynExp), SemFDL)));
         break;
      case NotNode :
	 return (MkExpBinOp
		   (pos,
		    NotExp,
		    MkExpRHS (Left (SynExp), SemFDL),
		    (Expression) NULL));
	 break;
      default :
	 perr (7, pos);
	 break;
   }
}

int NrOfExpressions (Exp)
   Expression Exp;
{
   if (Exp == NULL)
      return (0);
   switch (Exp->ExpKind) {
      case TupleExp :
	 return (1 + NrOfExpressions
		       (Exp->ExpDef.SonsForTuple.RestOfTuple));
         break;
      default :
	 return (1);
	 break;
   }
}

int ArrayTimeMux (Exp)
   Expression Exp;
{
   Expression IndExp, NrPortsExp;
   SymbolTable ArrSymDef;
   int NFD, NrPorts;
   if (Exp->ExpKind != TupleExp)
      return (0);
   IndExp = Exp->ExpDef.SonsForTuple.FirstExpression;
   if (IndExp->ExpKind != IndexedExp)
      return (0);
   ArrSymDef = IndExp->ExpDef.SonsForIndexed.ThisArray;
   if (ArrSymDef->SingleOrArray.AS.NumberOfDimensions != 1)
      perr (51, IndExp->ExpPos);
   NFD = NrFreeDims
	   (StripFixedDims
	      (IndExp->ExpDef.SonsForIndexed.IndexTupleExp));
   if (NFD > 1)
      perr (51, IndExp->ExpPos);
   if (NFD == 0)
      return (0);
   if (Exp->ExpDef.SonsForTuple.RestOfTuple == NULL)
      perr (54, Exp->ExpPos);
   NrPortsExp = Exp->ExpDef.SonsForTuple.RestOfTuple
		  ->ExpDef.SonsForTuple.FirstExpression;
   if (Exp->ExpDef.SonsForTuple.RestOfTuple
	 ->ExpDef.SonsForTuple.RestOfTuple != NULL)
      perr (51, Exp->ExpDef.SonsForTuple.RestOfTuple
		->ExpDef.SonsForTuple.FirstExpression->ExpPos);
   if (NrPortsExp->ExpKind != IntegerLeafExp)
      perr (51, NrPortsExp->ExpPos);
   NrPorts = NrPortsExp->ExpDef.IntegerConstant;
   if (NrPorts < 1)
      perr (51, NrPortsExp->ExpPos);
   return (NrPorts);
}

void MkLocalExpRHS (SynVDN, SemVDL, SemFDL)
   NodeType *SynVDN;
   ValueDefinitionList SemVDL;
   FunctionDefinitionList SemFDL;
{
   switch (Kind (SynVDN)) {
      case DefnNode :
#ifdef DEBUG
	 if (SemVDL->ValDefKind != SingleDef)
	    Crash ("MkLocalExpRHS 1");
#endif
	 if (Kind (Left (SynVDN)) == InitNode) {
            SemVDL->ValDef.SDef->LeftHandSide =
	       MkExpInit (MkPosition (Left (SynVDN)),
			  SemVDL->ValDef.SDef->LeftHandSide,
			  MkExpManifest (Right (Left (SynVDN)),
					 SemFDL));
            SemVDL->ValDef.SDef->RightHandSide =
	       MkExpRHS (Right (SynVDN), SemFDL);
	 } else if (Kind (Left (SynVDN)) == LpInitNode) {
            SemVDL->ValDef.SDef->LeftHandSide =
	       MkExpLpInit (MkPosition (Left (SynVDN)),
			  SemVDL->ValDef.SDef->LeftHandSide,
			  MkExpManifest (Right (Left (SynVDN)),
					 SemFDL));
            SemVDL->ValDef.SDef->RightHandSide =
	       MkExpRHS (Right (SynVDN), SemFDL);
	 } else {
	    SemVDL->ValDef.SDef->RightHandSide =
	       MkExpRHS (Right (SynVDN), SemFDL);
            switch (Kind (Right (SynVDN))) {
               case TimeMuxNode :
               case TimeDeMuxNode : {
		  Expression TMInputs;
		  Expression TMOutputs;
		  int NrPorts;
		  TMInputs =
	            SemVDL->ValDef.SDef->RightHandSide
		     ->ExpDef.SonsForTimeMux.TimeMuxInputs;
		  TMOutputs = SemVDL->ValDef.SDef->LeftHandSide;

		  TMInputs->ExpPos =
		     SemVDL->ValDef.SDef->RightHandSide->ExpPos;
		  TMOutputs->ExpPos =
		     SemVDL->ValDef.SDef->RightHandSide->ExpPos;

		  NrPorts = ArrayTimeMux (TMInputs);
                  if (NrPorts > 0) {
		  /* Having extract NrPorts, clears 2nd component of Tuple */
		     SemVDL->ValDef.SDef->RightHandSide
		       ->ExpDef.SonsForTimeMux.TimeMuxInputs
		       ->ExpDef.SonsForTuple.RestOfTuple = NULL;
		     SemVDL->ValDef.SDef->RightHandSide
		       ->ExpDef.SonsForTimeMux.NrArrayInputs = NrPorts;
		  }
		  NrPorts = ArrayTimeMux (TMOutputs);
                  if (NrPorts > 0) {
		     SemVDL->ValDef.SDef->LeftHandSide =
			TMOutputs->ExpDef.SonsForTuple.FirstExpression;
		     SemVDL->ValDef.SDef->RightHandSide
		       ->ExpDef.SonsForTimeMux.NrArrayOutputs = NrPorts;
		  } else {
                    SemVDL->ValDef.SDef->RightHandSide ->
			ExpDef.SonsForTimeMux.NrArrayOutputs = 
                        - NrOfExpressions (SemVDL->ValDef.SDef->LeftHandSide);
                  }
		  }
		  break;
	       default :
		  break;
	    }
            switch (Kind (Right (SynVDN))) {
	       case TimeMuxNode : {
		  Expression TMOutputs;
		  TMOutputs = SemVDL->ValDef.SDef->LeftHandSide;
                  if ((TMOutputs->ExpKind == TupleExp) ||
		      (SemVDL->ValDef.SDef->RightHandSide
			 ->ExpDef.SonsForTimeMux.NrArrayOutputs > 1))
		     perr (48, TMOutputs->ExpPos);
		  }
		  break;
               case TimeDeMuxNode : {
		  Expression TDMInputs;
		  Expression NoPhaseExp;
		  TDMInputs =
	            SemVDL->ValDef.SDef->RightHandSide
		     ->ExpDef.SonsForTimeMux.TimeMuxInputs;
		  /* Extract NoPhase as 2nd component of Tuple */
                  if (TDMInputs->ExpDef.SonsForTuple.RestOfTuple == NULL)
                     perr (47, TDMInputs->ExpPos);
                  NoPhaseExp = TDMInputs->ExpDef.SonsForTuple.RestOfTuple
			       ->ExpDef.SonsForTuple.FirstExpression;
                  if ((TDMInputs->ExpDef.SonsForTuple.RestOfTuple->ExpDef.
			SonsForTuple.RestOfTuple != NULL) ||
		      (SemVDL->ValDef.SDef->RightHandSide
			 ->ExpDef.SonsForTimeMux.NrArrayInputs > 1))
                     perr (47, TDMInputs->ExpPos);
		  if (NoPhaseExp->ExpKind != IntegerLeafExp)
                     perr (51, NoPhaseExp->ExpPos);
                  SemVDL->ValDef.SDef->RightHandSide->ExpDef.SonsForTimeMux.
		     NoPhase = NoPhaseExp->ExpDef.IntegerConstant;
		  /* Clears 2nd component of Tuple */
		  TDMInputs->ExpDef.SonsForTuple.RestOfTuple = NULL;
		  }
		  break;
	       case InterpolateNode :
	       case DecimateNode :
	       case UpSampleNode :
	       case DownSampleNode : {
		  Expression RSInputs;
		  Expression ScaleExp, PhaseExp;
		  SemVDL->ValDef.SDef->RightHandSide->ExpDef.SonsForReSamp.
			Type = Kind (Right (SynVDN));
		  RSInputs =
	            SemVDL->ValDef.SDef->RightHandSide
		                     ->ExpDef.SonsForReSamp.ReSampInputs;
		  /* Extract Scale as 2nd component of Tuple */
                  if (RSInputs->ExpDef.SonsForTuple.RestOfTuple == NULL)
                     perr (47, RSInputs->ExpPos);
		  ScaleExp = RSInputs->ExpDef.SonsForTuple.RestOfTuple
			  ->ExpDef.SonsForTuple.FirstExpression;
		  /* Extract Phase as 3rd component of Tuple */
                  if (RSInputs->ExpDef.SonsForTuple.RestOfTuple
		              ->ExpDef.SonsForTuple.RestOfTuple == NULL)
                     perr (47, RSInputs->ExpPos);
		  PhaseExp = RSInputs->ExpDef.SonsForTuple.RestOfTuple
			  ->ExpDef.SonsForTuple.RestOfTuple
			  ->ExpDef.SonsForTuple.FirstExpression;
		  if (ScaleExp->ExpKind != IntegerLeafExp)
                     perr (51, ScaleExp->ExpPos);
                  SemVDL->ValDef.SDef->RightHandSide->ExpDef.SonsForReSamp.
		        Scale = ScaleExp->ExpDef.IntegerConstant;
		  if (PhaseExp->ExpKind != IntegerLeafExp)
                     perr (51, PhaseExp->ExpPos);
                  SemVDL->ValDef.SDef->RightHandSide->ExpDef.SonsForReSamp.
		        Phase = PhaseExp->ExpDef.IntegerConstant;
		  /* Clears 2nd component of Tuple */
		  RSInputs->ExpDef.SonsForTuple.RestOfTuple = NULL;
	          }
		  break;
	       default :
		  break;
            }
	 }
	 break;
      case PragmaNode :
#ifdef DEBUG
	 if (SemVDL != NULL)
	    Crash ("MkLocalExpRHS 2");
#endif
	 break;
      case IterateNode : {
	 void MkLocalExpressionsRHS ();
	 NodeType *SynIteratedDefs;
#ifdef DEBUG
	 if (SemVDL->ValDefKind != IteratedDef)
	    Crash ("MkLocalExpRHS 3");
#endif
	 SynIteratedDefs = Right (SynVDN);
	 SemVDL->ValDef.IDef->ThisIterator
	   ->SingleOrArray.SS.IsVisible = true;
	 if (Kind (SynIteratedDefs) == CommaNode)
	    MkLocalExpressionsRHS
	      (SynIteratedDefs,
	       SemVDL->ValDef.IDef->IterValDefs,
	       SemFDL);
         else
	    MkLocalExpRHS
	      (SynIteratedDefs,
	       SemVDL->ValDef.IDef->IterValDefs,
	       SemFDL);
	 SemVDL->ValDef.IDef->ThisIterator
	   ->SingleOrArray.SS.IsVisible = false;
	 }
	 break;
      case WhileNode : {
	 void MkLocalExpressionsRHS ();
	 NodeType *SynIteratedDefs;
#ifdef DEBUG
	 if (SemVDL->ValDefKind != IteratedDef)
	    Crash ("MkLocalExpRHS 4");
#endif
	 SemVDL->ValDef.IDef->WhileExp =
	    MkExpRHS (Left (SynVDN), SemFDL);
	 SynIteratedDefs = Right (SynVDN);
	 if (Kind (SynIteratedDefs) == CommaNode)
	    MkLocalExpressionsRHS
	      (SynIteratedDefs,
	       SemVDL->ValDef.IDef->IterValDefs,
	       SemFDL);
         else
	    MkLocalExpRHS
	      (SynIteratedDefs,
	       SemVDL->ValDef.IDef->IterValDefs,
	       SemFDL);
	 }
	 break;
      case DoNode : {
	void MkLocalExpressionsRHS() ;
	NodeType *SynIteratedDefs;
#ifdef DEBUG
         if (SemVDL->ValDefKind != IteratedDef)
            Crash ("MkLocalExpRHS 5");
#endif
/* ExitNode is Right(SynVDN),  if_body is Right(Right(SynVDN))   */
         SemVDL->ValDef.IDef->ExitDef->RightHandSide = 
				MkExpRHS (Right(Right(SynVDN)), SemFDL);
         SynIteratedDefs = Left (SynVDN);
         if (Kind (SynIteratedDefs) == CommaNode)
            MkLocalExpressionsRHS
              (SynIteratedDefs,
               SemVDL->ValDef.IDef->IterValDefs,
               SemFDL);
         else
            MkLocalExpRHS
              (SynIteratedDefs,
               SemVDL->ValDef.IDef->IterValDefs,
               SemFDL);
         }
         break;
#ifdef DEBUG
      default :
	 Crash ("MkLocalExpRHS 6");
	 break;
#endif
   }
}

void MkLocalExpressionsRHS (SynValDefs, SemVDL, SemFDL)
   NodeType *SynValDefs;
   ValueDefinitionList SemVDL;
   FunctionDefinitionList SemFDL;
{
   if (SynValDefs == NULL)
      return;
#ifdef DEBUG
   if (Kind (SynValDefs) != CommaNode)
      Crash ("MkLocalExpressionsRHS");
#endif
   if (Kind (Left (SynValDefs)) == PragmaNode)
      MkLocalExpressionsRHS
	(Right (SynValDefs), SemVDL, SemFDL);
   else {
      MkLocalExpRHS (Left (SynValDefs), SemVDL, SemFDL);
      MkLocalExpressionsRHS
	(Right (SynValDefs), SemVDL->Next, SemFDL);
   }
}

void MkLocalRHSides (SynFDN, SemFDL)
   NodeType *SynFDN;
   FunctionDefinitionList SemFDL;
{
   NodeType *SynValDefs;
   SynValDefs = Right (Right (Right (SynFDN)));
   MkLocalExpressionsRHS (SynValDefs, SemFDL->ValDefs, SemFDL);
}

void Iter4FuncDefList (SynFDL, SemFDL)
   NodeType *SynFDL;
   FunctionDefinitionList SemFDL;
{
   if (SemFDL == NULL)
      return;
   if (Kind (Left (SynFDL)) == PragmaNode)
      Iter4FuncDefList (Right (SynFDL), SemFDL);
   else {
      MkLocalRHSides (Left (SynFDL), SemFDL);
      Iter4FuncDefList (Right (SynFDL), SemFDL->Next);
   }
}

void pass4 ()
{
   Iter4FuncDefList (TheTree, SemGraph.FuncDefs);
}
