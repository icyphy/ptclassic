/* $Id$ */
/* Author : Chris Scheers */
/* pass3.c -- version 1.8 (IMEC)		last updated: 3/1/89 */
static char *SccsId = "@(#)pass3.c	1.8 (IMEC)	89/03/01";

#include <stdio.h>
#include "sfg.h"

extern NodeType *TheTree;
extern SemanticGraph SemGraph;

extern Position MkPosition ();
extern SymbolTable FindSymDef (), MkSymTab ();
extern FunctionDefinitionList FindFuncDef ();
extern Expression MkExp (), MkExpTuple (), MkExpIdentLeaf ();
extern int SynEvalConstExp ();

Expression MkExp (pos, EK)
   Position pos;
   ExpressionKind EK;
{
   Expression Exp;
   Exp = NEW (ExpressionNode);
   Exp->ExpKind = EK;
   Exp->ExpPos = pos;
   Exp->ExpType.SymTpKind = UndefType;
   Exp->UniqueId = 0;
   return (Exp);
}

Expression MkExpIdentLeaf (pos, RefSym)
   Position pos;
   SymbolTable RefSym;
{
   Expression Exp;
   Exp = MkExp (pos, IdentLeafExp);
   Exp->ExpDef.ThisSymbol = RefSym;
   return (Exp);
}

Expression MkExpIntegerLeaf (pos, IntValue)
   Position pos;
   int IntValue;
{
   Expression Exp;
   Exp = MkExp (pos, IntegerLeafExp);
   Exp->ExpDef.IntegerConstant = IntValue;
   return (Exp);
}

Expression MkExpBinOp (pos, OpCode, L, R)
   Position pos;
   ExpressionKind OpCode;
   Expression L, R;
{
   Expression Exp;
   Exp = MkExp (pos, OpCode);
   Exp->ExpDef.SonsForBinOp.LeftExp = L;
   Exp->ExpDef.SonsForBinOp.RightExp = R;
   return (Exp);
}

Expression MkExpTuple (pos, First, Rest)
   Position pos;
   Expression First, Rest;
{
   Expression Exp;
   Exp = MkExp (pos, TupleExp);
   Exp->ExpDef.SonsForTuple.FirstExpression = First;
   Exp->ExpDef.SonsForTuple.RestOfTuple = Rest;
   return (Exp);
}

Expression MkExpIndexedExp (pos, RefArray, IndexTuple)
   Position pos;
   SymbolTable RefArray;
   Expression IndexTuple;
{
   Expression Exp;
   Exp = MkExp (pos, IndexedExp);
   Exp->ExpDef.SonsForIndexed.ThisArray = RefArray;
   Exp->ExpDef.SonsForIndexed.IndexTupleExp = IndexTuple;
   return (Exp);
}

SingleDefinition MkSingleDefinition (LHS, RHS)
   Expression LHS, RHS;
{
   SingleDefinition SD;
   SD = NEW (SingleDefinitionNode);
   SD->LeftHandSide = LHS;
   SD->RightHandSide = RHS;
   return (SD);
}

ValueDefinitionList MkValDef ()
{
   ValueDefinitionList VDL;
   VDL = NEW (ValueDefinition);
   VDL->Next = NULL;
   VDL->OrderedNext = NULL;
   return (VDL);
}

ValueDefinitionList MkValDefSingle (SD)
   SingleDefinition SD;
{
   ValueDefinitionList VDL;
   VDL = MkValDef ();
   VDL->ValDefKind = SingleDef;
   VDL->ValDef.SDef = SD;
   return (VDL);
}

ValueDefinitionList MkValDefIterated (ID)
   IteratedDefinition ID;
{
   ValueDefinitionList VDL;
   VDL = MkValDef ();
   VDL->ValDefKind = IteratedDef;
   VDL->ValDef.IDef = ID;
   return (VDL);
}

ValueDefinitionList MkValDefList (FirstOfVDL, RestOfVDL)
   ValueDefinitionList FirstOfVDL, RestOfVDL;
{
   if (FirstOfVDL == NULL)
      return (RestOfVDL);
   FirstOfVDL->Next = RestOfVDL;
   return (FirstOfVDL);
}

IteratedDefinition MkIteratedDefinition (RefIterator, LB, UB, VDL, UpID)
   SymbolTable RefIterator;
   Expression LB, UB;
   ValueDefinitionList VDL;
   IteratedDefinition UpID;
{
   IteratedDefinition ID;
   ID = NEW (IteratedDefinitionNode);
   ID->IDKind = ForLoop;
   ID->ThisIterator = RefIterator;
   ID->LowerBound = LB;
   ID->UpperBound = UB;
   ID->IterValDefs = VDL;
   ID->NestingUp = UpID;
   return (ID);
}

IteratedDefinition MkWhileDefinition (WE, VDL, UpID)
   Expression WE;
   ValueDefinitionList VDL;
   IteratedDefinition UpID;
{
   IteratedDefinition ID;
   ID = NEW (IteratedDefinitionNode);
   ID->IDKind = WhileLoop;
   ID->WhileExp = WE;
   ID->IterValDefs = VDL;
   ID->NestingUp = UpID;
   return (ID);
}

IteratedDefinition MkDoDefinition (ED, VDL, UpID)
   SingleDefinition ED;
   ValueDefinitionList VDL;
   IteratedDefinition UpID;
{
   IteratedDefinition ID;
   ID = NEW (IteratedDefinitionNode);
   ID->IDKind = DoLoop;
   ID->ExitDef = ED;
   ID->IterValDefs = VDL;
   ID->NestingUp = UpID;
   return (ID);
}

IndexedDefinitionList MkIndexedDefinitionList (SD, ID, RestOfIDL)
   SingleDefinition SD;
   IteratedDefinition ID;
   IndexedDefinitionList RestOfIDL;
{
   IndexedDefinitionList IDL;
   IDL = NEW (IndexedDefinition);
   IDL->IndexedDef = SD;
   IDL->ClosestIteration = ID;
   IDL->Next = RestOfIDL;
   return (IDL);
}

SymbolTable FindIdLHS (SynId, SemFDL, IsInit)
   NodeType *SynId;
   FunctionDefinitionList SemFDL;
   bool IsInit;
{
   SymbolTable SymDef;
   FunctionDefinitionList SemFuncDef;
   SymDef = FindSymDef (SemFDL->LocalSymbols, Left (SynId));
   if (SymDef != NULL)
      return (SymDef);
   SemFuncDef = FindFuncDef (SemGraph.FuncDefs, Left (SynId));
   if (SemFuncDef != NULL)
      perr (14, MkPosition (SynId));
   SymDef = FindSymDef (SemFDL->InputParameters, Left (SynId));
   if (SymDef != NULL) {
      if (IsInit == true)
	 return (SymDef);
      else
         perr (9, MkPosition (SynId));
   }
   return (NULL);
}

SymbolTable FindIdManifest (SynId, SemFDL)
   NodeType *SynId;
   FunctionDefinitionList SemFDL;
{
   SymbolTable SymDef;
   SymDef = FindSymDef (SemFDL->LocalSymbols, Left (SynId));
   if (SymDef == NULL)
      SymDef = FindSymDef (SemFDL->InputParameters, Left (SynId));
   if (SymDef == NULL)
      SymDef = FindSymDef (SemFDL->OutputParameters, Left (SynId));
   if (SymDef == NULL)
      perr (13, MkPosition (SynId));
   if ((SymDef->SymKind == Iterator) &&
       (SymDef->SingleOrArray.SS.IsVisible == false))
      perr (16, MkPosition (SynId));
   return (SymDef);
}

bool HasDimension (SymDef, NrDims)
   SymbolTable SymDef;
   int NrDims;
{
   if (SymDef->IsArray == true)
      if (NrDims == SymDef->SingleOrArray.AS.NumberOfDimensions)
	 return (true);
      else
	 return (false);
   else
      if (NrDims == 0)
	 return (true);
      else
	 return (false);
}

SymbolTable DeclareSignal (SynId, NrDims, SemFDL, IsInit)
   NodeType *SynId;
   int NrDims;
   FunctionDefinitionList SemFDL;
   bool IsInit;
{
   SymbolTable SymDef;
   SymDef = FindSymDef (SemFDL->OutputParameters, Left (SynId));
   if (SymDef != NULL)
      SymDef->SymKind = OutPar;
   else
      SymDef = FindIdLHS (SynId, SemFDL, IsInit);
   if (SymDef != NULL) {
      if (SymDef->SymKind == Iterator)
	 perr (28, SymDef->SymbolId.IdPos);
      if (HasDimension (SymDef, NrDims) == false)
	 perr (21, MkPosition (SynId));
      return (SymDef);
   } else {
      SemFDL->LocalSymbols = MkSymTab (SynId, NULL, NrDims, SemFDL->LocalSymbols);
      SemFDL->LocalSymbols->SymKind = LocalSym;
      return (SemFDL->LocalSymbols);
   }
}

SymbolTable DeclareIterator (SynId, SemFDL)
   NodeType *SynId;
   FunctionDefinitionList SemFDL;
{
   SymbolTable SymDef;
   SymDef = FindSymDef (SemFDL->OutputParameters, Left (SynId));
   if (SymDef != NULL)
      perr (28, MkPosition (SynId));
   SymDef = FindIdLHS (SynId, SemFDL, false);
   if (SymDef != NULL) {
      if (SymDef->SymKind != Iterator)
	 perr (28, MkPosition (SynId));
      if (SymDef->SingleOrArray.SS.IsVisible == true)
	 perr (29, MkPosition (SynId));
      return (SymDef);
   } else {
      SemFDL->LocalSymbols = MkSymTab (SynId, NULL, 0, SemFDL->LocalSymbols);
      SemFDL->LocalSymbols->SymKind = Iterator;
      SemFDL->LocalSymbols->SymType.SymTpKind = IntType;
      return (SemFDL->LocalSymbols);
   }
}

Expression MkExpManifest (SynExp, SemFDL)
   NodeType *SynExp;
   FunctionDefinitionList SemFDL;
{
   Position pos;
   pos = MkPosition (SynExp);
   switch (Kind (SynExp)) {
      case IdentLeaf :
	 return (MkExpIdentLeaf
		   (pos, FindIdManifest (SynExp, SemFDL)));
         break;
      case IntegerLeaf :
	 return (MkExpIntegerLeaf (pos, (int) Left (SynExp)));
	 break;
      case NegateNode :
	 return (MkExpBinOp
		   (pos, NegateExp,
		    MkExpManifest (Left (SynExp), SemFDL),
		    (Expression) NULL));
         break;
      case PlusNode :
	 return (MkExpBinOp
		   (pos,
		    PlusExp,
		    MkExpManifest (Left (SynExp), SemFDL),
		    MkExpManifest (Right (SynExp), SemFDL)));
         break;
      case MinusNode :
	 return (MkExpBinOp
		   (pos,
		    MinusExp,
		    MkExpManifest (Left (SynExp), SemFDL),
		    MkExpManifest (Right (SynExp), SemFDL)));
         break;
      case MultNode :
	 return (MkExpBinOp
		   (pos,
		    MultExp,
		    MkExpManifest (Left (SynExp), SemFDL),
		    MkExpManifest (Right (SynExp), SemFDL)));
         break;
      case IntDivNode :
	 return (MkExpIntegerLeaf
		   (pos, SynEvalConstExp (SynExp)));
         break;
      default :
	 perr (16, pos);
	 break;
   }
}

Expression MkIndexTuple (ArrayPos, SynIndexTree, SemFDL)
   Position ArrayPos;
   NodeType *SynIndexTree;
   FunctionDefinitionList SemFDL;
{
   Expression SemIndexExp;
   Position pos;
   if (SynIndexTree == NULL)
      return (NULL);
   if (Left (SynIndexTree) == NULL) {
      SemIndexExp = NULL;
      pos = ArrayPos;
   } else {
      SemIndexExp =
         MkExpManifest (Left (SynIndexTree), SemFDL);
      pos = MkPosition (Left (SynIndexTree));
   }
   return
     (MkExpTuple
	(pos,
         SemIndexExp,
         MkIndexTuple (pos, Right (SynIndexTree), SemFDL)));
}

Expression MkExpLHS (SynExp, SemFDL, IsInit)
   NodeType *SynExp;
   FunctionDefinitionList SemFDL;
   bool IsInit;
{
   if (SynExp == NULL)
      return (NULL);
   switch (Kind (SynExp)) {
      case IntegerLeaf :
	 return (MkExpIntegerLeaf (MkPosition (SynExp),
	                           (int) Left (SynExp)));
	 break;
      case IdentLeaf : {
	 SymbolTable SymDef;
	 SymDef = DeclareSignal (SynExp, 0, SemFDL, IsInit);
	 return (MkExpIdentLeaf (SymDef->SymbolId.IdPos, SymDef));
	 }
	 break;
      case ArrayNameNode : {
	 SymbolTable SymDef;
	 SymDef = DeclareSignal 
		    (Left (SynExp), NrOfDims (Right (SynExp)), SemFDL, IsInit);
	 return
	   (MkExpIndexedExp
	      (MkPosition (Left (SynExp)),
	       SymDef,
	       MkIndexTuple
	         (MkPosition (Left (SynExp)),
		  Right (SynExp), SemFDL)));
         }
	 break;
      case CommaNode :
	 return
	   (MkExpTuple
	      (MkPosition (Left (SynExp)),
	       MkExpLHS (Left (SynExp), SemFDL, IsInit),
	       MkExpLHS (Right (SynExp), SemFDL, IsInit)));
         break;
      default :
	 perr (12, MkPosition (SynExp));
	 break;
   }
}

bool IsinIndexedDef (IDL, SD)
   IndexedDefinitionList IDL;
   SingleDefinition SD;
{
   if (IDL == NULL)
      return (false);
   if (IDL->IndexedDef == SD)
      return (true);
   else
      return (IsinIndexedDef (IDL->Next, SD));
}

void FillInTupleDefs (Tuple, SD, UpID)
   Expression Tuple;
   SingleDefinition SD;
   IteratedDefinition UpID;
{
   Expression FirstExp;
   if (Tuple == NULL)
      return;
   FirstExp = Tuple->ExpDef.SonsForTuple.FirstExpression;
   switch (FirstExp->ExpKind) {
      case IntegerLeafExp :
	 /* only for multi-rate */
	 break;
      case IdentLeafExp :
	 FirstExp->ExpDef.ThisSymbol->SingleOrArray.SS.SSDef = SD;
	 break;
      case IndexedExp : {
	 SymbolTable SymDef;
         SymDef = FirstExp->ExpDef.SonsForIndexed.ThisArray;
	 if (IsinIndexedDef (SymDef->SingleOrArray.AS.ASDef, SD)
	     == false)
	    SymDef->SingleOrArray.AS.ASDef =
	       MkIndexedDefinitionList
	         (SD, UpID, SymDef->SingleOrArray.AS.ASDef);
	 }
         break;
#ifdef DEBUG
      default :
	 Crash ("FillInTupleDefs");
	 break;
#endif
   }
   FillInTupleDefs (Tuple->ExpDef.SonsForTuple.RestOfTuple, SD, UpID);
}

SingleDefinition MkSingleDefLHS (SynLHS, UpID, SemFDL)
   NodeType *SynLHS;
   IteratedDefinition UpID;
   FunctionDefinitionList SemFDL;
{
   switch (Kind (SynLHS)) {
      case IdentLeaf : {
	 Expression SemExp;
	 SymbolTable SymDef;
	 SemExp = MkExpLHS (SynLHS, SemFDL, false);
	 SymDef = SemExp->ExpDef.ThisSymbol;
	 SymDef->SingleOrArray.SS.SSDef =
	    MkSingleDefinition
	      (SemExp, (Expression) NULL);
	 return (SymDef->SingleOrArray.SS.SSDef);
         }
	 break;
      case ArrayNameNode : {
	 Expression SemExp;
	 SymbolTable SymDef;
	 SemExp = MkExpLHS (SynLHS, SemFDL, false);
	 SymDef = SemExp->ExpDef.SonsForIndexed.ThisArray;
	 SymDef->SingleOrArray.AS.ASDef =
	    MkIndexedDefinitionList
	      (MkSingleDefinition
		 (SemExp , (Expression) NULL),
	       UpID,
               SymDef->SingleOrArray.AS.ASDef);
	 return (SymDef->SingleOrArray.AS.ASDef->IndexedDef);
         }
	 break;
      case TupleNode : {
	 Expression SemExp;
	 SingleDefinition SD;
	 SemExp = MkExpLHS (Left (SynLHS), SemFDL, false);
	 SD = MkSingleDefinition (SemExp, (Expression) NULL);
         FillInTupleDefs (SemExp, SD, UpID);
	 return (SD);
	 }
         break;
      case InitNode :
      case LpInitNode :
         if ((Kind (Left (SynLHS)) != IdentLeaf) &&
	     (Kind (Left (SynLHS)) != ArrayNameNode))
	    perr (16, MkPosition (Left (SynLHS)));
	 if (Kind (Left (SynLHS)) == IdentLeaf)
	    return (MkSingleDefinition
	              (MkExpLHS (Left (SynLHS), SemFDL, true),
		       (Expression) NULL));
         return (MkSingleDefLHS (Left (SynLHS), UpID, SemFDL));
	 break;
      default :
         perr (12, MkPosition (SynLHS));
         break;
   }
}

ValueDefinitionList MkLocalDef (SynVDN, UpID, SemFDL)
   NodeType *SynVDN;
   IteratedDefinition UpID;
   FunctionDefinitionList SemFDL;
{
   switch (Kind (SynVDN)) {
      case DefnNode :
	 return
	   (MkValDefSingle
	      (MkSingleDefLHS (Left (SynVDN), UpID, SemFDL)));
	 break;
      case PragmaNode :
	 return (NULL);
	 break;  /* pragma nodes are ignored */
      case IterateNode : {
	 NodeType *SynIteratedDefs, *SynIterator;
	 ValueDefinitionList IterVDL, MkLocalDefinitions ();
	 IteratedDefinition NewID;
	 SymbolTable SymDef;
	 SynIterator = Left (SynVDN);
	 SynIteratedDefs = Right (SynVDN);
	 SymDef = DeclareIterator (Left (SynIterator), SemFDL);
	 NewID =
	   (MkIteratedDefinition
	      (SymDef,
	       MkExpManifest (Left (Right (SynIterator)), SemFDL),
	       MkExpManifest (Right (Right (SynIterator)), SemFDL),
	       (ValueDefinitionList) NULL,
	       UpID));
	 SymDef->SingleOrArray.SS.IsVisible = true;
         if (Kind (SynIteratedDefs) == CommaNode)
	    IterVDL = MkLocalDefinitions (SynIteratedDefs, NewID, SemFDL);
         else
	    IterVDL = MkLocalDef (SynIteratedDefs, NewID, SemFDL);
	 SymDef->SingleOrArray.SS.IsVisible = false;
	 NewID->IterValDefs = IterVDL;
	 return (MkValDefIterated (NewID));
	 }
	 break;
      case WhileNode : {
	 NodeType *SynIteratedDefs;
	 ValueDefinitionList IterVDL;
	 IteratedDefinition NewID;
	 SemFDL->IsProcedure = true;
	 SynIteratedDefs = Right (SynVDN);
	 NewID =
	   (MkWhileDefinition
	      ((Expression) NULL, (ValueDefinitionList) NULL, UpID));
	 if (Kind (SynIteratedDefs) == CommaNode)
	    IterVDL = MkLocalDefinitions (SynIteratedDefs, NewID, SemFDL);
         else
	    IterVDL = MkLocalDef (SynIteratedDefs, NewID, SemFDL);
	 NewID->IterValDefs = IterVDL;
	 return (MkValDefIterated (NewID));
	 }
	 break;
      case DoNode : {
	 NodeType *SynIteratedDefs;
         SingleDefinition ExitDef;
	 ValueDefinitionList IterVDL;
	 IteratedDefinition NewID;
	 SynIteratedDefs = Left (SynVDN);
	 NewID =
	   (MkDoDefinition
	      ((SingleDefinition) NULL, (ValueDefinitionList) NULL, UpID));
/* ExitDef is Right(SynVDN),  LHS of ExitDef is Left(Right(SynVDN))  */
         ExitDef = MkSingleDefLHS (Left(Right(SynVDN)), UpID, SemFDL);
	 NewID->ExitDef = ExitDef;
	 if (Kind (SynIteratedDefs) == CommaNode)
	    IterVDL = MkLocalDefinitions (SynIteratedDefs, NewID, SemFDL);
         else
	    IterVDL = MkLocalDef (SynIteratedDefs, NewID, SemFDL);
	 NewID->IterValDefs = IterVDL;
	 return (MkValDefIterated (NewID));
	 }
#ifdef DEBUG
      default :
	 Crash ("MkLocalDef");
	 break;
#endif
   }
}

ValueDefinitionList MkLocalDefinitions (SynValDefs, UpID, SemFDL)
   NodeType *SynValDefs;
   IteratedDefinition UpID;
   FunctionDefinitionList SemFDL;
{
   ValueDefinitionList VDL;
   if (SynValDefs == NULL)
      return(NULL);
#ifdef DEBUG
   if (Kind (SynValDefs) != CommaNode)
      Crash ("MkLocalDefinitions");
#endif
   VDL = MkLocalDef (Left (SynValDefs), UpID, SemFDL);
   return
     (MkValDefList
	(VDL,
	 MkLocalDefinitions (Right (SynValDefs), UpID, SemFDL)));
}

void ChkIfOutParsDefined (OutPars)
   SymbolTable OutPars;
{
   if (OutPars == NULL)
      return;
   if (OutPars->SymKind == UndefSym)
      perr (11, OutPars->SymbolId.IdPos);
   else
      ChkIfOutParsDefined (OutPars->Next);
}

void MkLocalSymbols (SynFDN, SemFDL)
   NodeType *SynFDN;
   FunctionDefinitionList SemFDL;
{
   NodeType *SynValDefs;
   SynValDefs = Right (Right (Right (SynFDN)));
   SemFDL->ValDefs = MkLocalDefinitions
		       (SynValDefs, (IteratedDefinition) NULL, SemFDL);
   ChkIfOutParsDefined (SemFDL->OutputParameters);
}

void Iter3FuncDefList (SynFDL, SemFDL)
   NodeType *SynFDL;
   FunctionDefinitionList SemFDL;
{
   if (SemFDL == NULL)
      return;
   if (Kind (Left (SynFDL)) == PragmaNode)
      Iter3FuncDefList (Right (SynFDL), SemFDL);
   else {
      MkLocalSymbols (Left (SynFDL), SemFDL);
      Iter3FuncDefList (Right (SynFDL), SemFDL->Next);
   }
}

void pass3 ()
{
   Iter3FuncDefList (TheTree, SemGraph.FuncDefs);
}
