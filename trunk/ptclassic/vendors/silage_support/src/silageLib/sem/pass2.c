/* $Id$ */
/* Author : Chris Scheers */
/* pass2.c -- version 1.10 (IMEC)		last updated: 4/21/89 */
static char *SccsId = "@(#)pass2.c	1.10 (IMEC)	89/04/21";

#include <stdio.h>
#include "sfg.h"

extern NodeType *TheTree;
extern SemanticGraph SemGraph;

extern Position MkPosition ();
extern SymbolTable FindIdLHS ();

SymbolTable FindSymDef (ST, SName)
   SymbolTable ST;
   char *SName;
{
   if (ST == NULL)
      return (NULL);
   if (!strcmp (SName, ST->SymbolId.IdName))
      return (ST);
   else
      return (FindSymDef (ST->Next, SName));
}

int SynEvalConstExp (SynExp)
   NodeType *SynExp;
{
   switch (Kind (SynExp)) {
      case IntegerLeaf :
	 return ((int) Left (SynExp));
	 break;
      case NegateNode :
	 return (- SynEvalConstExp (Left (SynExp)));
	 break;
      case PlusNode :
	 return (SynEvalConstExp (Left (SynExp)) +
	 	 SynEvalConstExp (Right (SynExp)));
         break;
      case MinusNode :
	 return (SynEvalConstExp (Left (SynExp)) -
		 SynEvalConstExp (Right (SynExp)));
         break;
      case MultNode :
	 return (SynEvalConstExp (Left (SynExp)) *
		 SynEvalConstExp (Right (SynExp)));
         break;
      case IntDivNode :
	 return (SynEvalConstExp (Left (SynExp)) /
		 SynEvalConstExp (Right (SynExp)));
         break;
      default :
	 perr (16, MkPosition (SynExp));
	 break;
   }
}

int NrOfDims (SynBoxList)
   NodeType *SynBoxList;  /* can be of type :
			     BoxNode, ValuedBoxNode,
			     ArrayIndexNode */
{
   if (SynBoxList == NULL)
      return (0);
   else
      return (1 + NrOfDims (Right (SynBoxList)));
}

void CalcBounds (Bounds, SynBoxList, index)
   int Bounds[];
   NodeType *SynBoxList;
   int index;
{
   if (SynBoxList == NULL)
      return;
   Bounds[index] = SynEvalConstExp (Left (SynBoxList));
   if (Bounds[index] < 1)
      perr (27, MkPosition (SynBoxList));
   CalcBounds (Bounds, Right (SynBoxList), index + 1);
}

SymbolType MkSymType (SynSType, pos)
   NodeType *SynSType;
   Position pos;
{
   SymbolType SemSType;
#ifdef DEBUG
   if ((Kind (SynSType) != NumTypeNode) &&
       (Kind (SynSType) != IntTypeNode) &&
       (Kind (SynSType) != BoolTypeNode))
      Crash ("MkSymType");
#endif
   if (Kind (SynSType) == IntTypeNode && Left (SynSType) == NULL)
      SemSType.SymTpKind = IntType;
   else
   if (Kind (SynSType) == BoolTypeNode)
      SemSType.SymTpKind = BoolType;
   else {
      if (Left (SynSType) == NULL)
         perr (5, pos);
      SemSType.SymTpKind = FixedType;
      SemSType.SymTp.FixedTp.w = SynEvalConstExp (Left (SynSType));
      if (Right (SynSType) == NULL) {
	 if (Kind (SynSType) == IntTypeNode)
	    SemSType.SymTp.FixedTp.d = 0;
         else
	    SemSType.SymTp.FixedTp.d = SemSType.SymTp.FixedTp.w - 1;
      }	  
      else
         SemSType.SymTp.FixedTp.d = SynEvalConstExp (Right (SynSType));
      if ((SemSType.SymTp.FixedTp.w < SemSType.SymTp.FixedTp.d) ||
          (SemSType.SymTp.FixedTp.w <= 0))
         perr (6, pos);
   }
   return (SemSType);
}

SymbolTable MkSymTab (SynId, SynSType, NrDims, SemSymTabNext)
   NodeType *SynId, *SynSType;
   int NrDims;
   SymbolTable SemSymTabNext;
{
   SymbolTable SemST;
   SemST = NEW (SymbolTableNode);
   SemST->SymbolId.IdName = (char *) Left (SynId);
   SemST->SymbolId.IdPos = MkPosition (SynId);
   if (SynSType == NULL) {
      SemST->SymType.SymTpKind = UndefType;
      if (NrDims > 0) {
         SemST->IsArray = true;
	 SemST->SingleOrArray.AS.NumberOfDimensions = NrDims;
         SemST->SingleOrArray.AS.DimensionBounds = NULL;
      } else {
	 SemST->IsArray = false;
      }
   } else {
      if ((Kind (SynSType) == NumTypeNode) ||
	  (Kind (SynSType) == IntTypeNode) ||
	  (Kind (SynSType) == BoolTypeNode)) {
	 SemST->IsArray = false;
         SemST->SymType = MkSymType (SynSType, SemST->SymbolId.IdPos);
      } else {
	 SemST->IsArray = true;
	 SemST->SymType = MkSymType (Left (SynSType), SemST->SymbolId.IdPos);
	 SemST->SingleOrArray.AS.NumberOfDimensions = NrOfDims (Right (SynSType));
	 if (Kind (SynSType) == ValuedBoxTypeNode) {
	    SemST->SingleOrArray.AS.DimensionBounds =
	       (int *) tmalloc (sizeof (int) *
				SemST->SingleOrArray.AS.NumberOfDimensions);
	    CalcBounds (SemST->SingleOrArray.AS.DimensionBounds,
			Right (SynSType), 0);
	 } else
	    SemST->SingleOrArray.AS.DimensionBounds = NULL;
      }
   }
   if (SemST->IsArray == true) {
      SemST->SingleOrArray.AS.ASDef = NULL;
      SemST->SingleOrArray.AS.InPlace = false;
   } else
      SemST->SingleOrArray.SS.SSDef = NULL;
   SemST->ToDisplay = false;
   SemST->DisplayFile = NULL;
   SemST->IsConstant = false;
   SemST->IsCyclic = false;
   SemST->SymKind = UndefSym;
   SemST->MaxDelay = -1;
   SemST->ProcessId = -1;
   SemST->Next = SemSymTabNext;
   return (SemST);
}

void DeclareTypedSignal (SynId, SynSType, SemFDL)
   NodeType *SynId, *SynSType;
   FunctionDefinitionList SemFDL;
{
   SymbolTable SymDef;
   SymDef = FindIdLHS (SynId, SemFDL);
   if (SymDef != NULL)
      perr (35, MkPosition (SynId));
   SymDef = FindSymDef (SemFDL->OutputParameters, (char *) Left (SynId));
   if (SymDef != NULL)
      perr (35, MkPosition (SynId));
   SemFDL->LocalSymbols =
     MkSymTab (SynId, SynSType, -1, SemFDL->LocalSymbols);
   SemFDL->LocalSymbols->SymKind = LocalSym;
}

SymbolTable MkParamsTab (SynParams, ErrNr)
   NodeType *SynParams;
   int ErrNr;
{
   NodeType *SynFoDN;
   SymbolTable SemParamsTab;
   if (SynParams == NULL)
      return (NULL);
#ifdef DEBUG
   if (Kind (SynParams) != CommaNode)
      Crash ("MkParamsTab");
#endif
   SynFoDN = Left (SynParams);
   SemParamsTab = MkSymTab (Left (SynFoDN), Right (SynFoDN), -1,
		            MkParamsTab (Right (SynParams), ErrNr));
   if (FindSymDef (SemParamsTab->Next,
		   SemParamsTab->SymbolId.IdName) != NULL)
      perr (ErrNr, SemParamsTab->SymbolId.IdPos);
   return (SemParamsTab);
}

void MkParamsInPar (ST)
   SymbolTable ST;
{
   if (ST == NULL)
      return;
   ST->SymKind = InPar;
   MkParamsInPar (ST->Next);
}

void MkLocalDecls (SynLocalDecls, SemFDL)
   NodeType *SynLocalDecls;
   FunctionDefinitionList SemFDL;
{
   NodeType *SynFoDN;
   if (SynLocalDecls == NULL)
      return;
#ifdef DEBUG
   if (Kind (SynLocalDecls) != CommaNode)
      Crash ("MkLocalDecls");
#endif
   SynFoDN = Left (SynLocalDecls);
   DeclareTypedSignal (Left (SynFoDN), Right (SynFoDN), SemFDL);
   MkLocalDecls (Right (SynLocalDecls), SemFDL);
}

void MkDeclarations (SynFDN, SemFDL)
   NodeType *SynFDN;
   FunctionDefinitionList SemFDL;
{
   NodeType *SynInParams, *SynOutParams, *SynLocalDecls;
   SynInParams = Left (Left (Right (SynFDN)));
   SemFDL->InputParameters = MkParamsTab (SynInParams, 4);
   MkParamsInPar (SemFDL->InputParameters);
   SynOutParams = Right (Left (Right (SynFDN)));
   if (Kind (SynOutParams) != CommaNode) {
      NodeType *SynReturnId;
      SynReturnId = MakeTree2 (IdentLeaf, "return", NULL,
			       SemFDL->FunctionId.IdPos.LineNum,
			       SemFDL->FunctionId.IdPos.CharNum,
			       SemFDL->FunctionId.IdPos.FileName);
      SemFDL->OutputParameters =
	 MkSymTab (SynReturnId, SynOutParams, -1, (SymbolTable) NULL);
   } else
      SemFDL->OutputParameters = MkParamsTab (SynOutParams, 8);
   SynLocalDecls = Left (Right (Right (SynFDN)));
   SemFDL->LocalSymbols = NULL;
   MkLocalDecls (SynLocalDecls, SemFDL);
}

void Iter2FuncDefList (SynFDL, SemFDL)
   NodeType *SynFDL;
   FunctionDefinitionList SemFDL;
{
   if (SemFDL == NULL)
      return;
   if (Kind (Left (SynFDL)) == PragmaNode)
      Iter2FuncDefList (Right (SynFDL), SemFDL);
   else {
      MkDeclarations (Left (SynFDL), SemFDL);
      Iter2FuncDefList (Right (SynFDL), SemFDL->Next);
   }
}

void pass2 ()
{
   Iter2FuncDefList (TheTree, SemGraph.FuncDefs);
}
