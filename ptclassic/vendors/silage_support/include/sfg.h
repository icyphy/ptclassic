/* $Id$ */

/* sfg.h -- version 1.9 (IMEC)		last updated: 3/8/89 */
/* C. Scheers */

#include "silage.h"

#define INFINITY 1000000
typedef enum {false, true} bool;

typedef struct Position {
   int LineNum, CharNum;
   char *FileName;
   NodeKindType kind;
   struct NODETYPE *L, *R;
} Position;

typedef struct Identifier {
   char *IdName;
   Position IdPos;
   int UniqueId;
} Identifier;

typedef struct FunctionDefinition *FunctionDefinitionList;
typedef struct ValueDefinition *ValueDefinitionList;
typedef struct SingleDefinitionNode *SingleDefinition;
typedef struct IteratedDefinitionNode *IteratedDefinition;
typedef struct IndexedDefinition *IndexedDefinitionList;
typedef struct SymbolTableNode *SymbolTable;
typedef struct ExpressionNode *Expression;

typedef struct SemanticGraph {
   FunctionDefinitionList FuncDefs;
   FunctionDefinitionList OrderedFuncDefs;
   Expression ListOfFixedLeafs;
} SemanticGraph;

typedef struct FunctionDefinition {
   Identifier FunctionId;
   SymbolTable InputParameters;
   SymbolTable OutputParameters;
   SymbolTable LocalSymbols;
   ValueDefinitionList ValDefs;
   ValueDefinitionList OrderedValDefs;
   Expression ListOfFuncApps;
   bool DelaysExist;
   bool IsProcedure;
   bool IsRecursive;
   int NumberOfProcesses;
   FunctionDefinitionList Next;
   FunctionDefinitionList OrderedNext;
} FunctionDefinition;

typedef struct SingleDefinitionNode {
   Expression LeftHandSide;
   Expression RightHandSide;
} SingleDefinitionNode;

typedef enum {ForLoop, WhileLoop, DoLoop} IterationKind;
typedef struct IteratedDefinitionNode {
   IterationKind IDKind;
   SymbolTable ThisIterator;
   Expression LowerBound;
   Expression UpperBound;
   Expression WhileExp;
   SingleDefinition ExitDef;
   ValueDefinitionList IterValDefs;
   IteratedDefinition NestingUp;
} IteratedDefinitionNode;

typedef enum {SingleDef, IteratedDef} ValueDefinitionKind;
typedef struct ValueDefinition {
   ValueDefinitionKind ValDefKind;
   union {
     SingleDefinition SDef;
     IteratedDefinition IDef;
   } ValDef;
   ValueDefinitionList Next;
   ValueDefinitionList OrderedNext;
} ValueDefinition;

typedef struct IndexedDefinition {
   SingleDefinition IndexedDef;
   IteratedDefinition ClosestIteration;
   IndexedDefinitionList Next;
} IndexedDefinition;

typedef enum {IntType, FixedType, BoolType, UndefType} SymbolTypeKind;
typedef struct SymbolType {
   SymbolTypeKind SymTpKind;
   union {
      struct {int w, d;} FixedTp;
      int BoolLength;
   } SymTp;
} SymbolType;

typedef struct SingleSymbol {
   SingleDefinition SSDef;
   int ManValue;
   bool IsVisible;
   bool IsProduced;
} SingleSymbol;

typedef struct ArraySymbol {
   IndexedDefinitionList ASDef;
   bool InPlace;
   int NumberOfDimensions;
   int *DimensionBounds;   /* Array of length NumberOfDimensions */
   int NumberOfElements;
   bool *IsProduced;   /* Array of length NumberOfElements */
} ArraySymbol;

typedef enum {InPar, OutPar, LocalSym, Iterator, UndefSym} SymbolKind;
typedef struct SymbolTableNode {
   Identifier SymbolId;
   SymbolKind SymKind;
   SymbolType SymType;
   bool IsArray;
   union {
      SingleSymbol SS;
      ArraySymbol AS;
   } SingleOrArray;
   int MaxDelay;
   bool ToDisplay;
   char *DisplayFile;
   bool IsConstant;
   bool IsCyclic;
   int ProcessId;
   SymbolTable Next;
} SymbolTableNode;

typedef enum {OrExp, AndExp, ExorExp, PlusExp, MinusExp,
	      MultExp, DivExp, IntDivExp, CompMinusExp,
	      LeftShiftExp, RightShiftExp, NegateExp, CosExp, SinExp, SqrtExp,
	      MinExp, MaxExp,
	      CompExp, NotExp, GreaterExp, SmallerExp,
	      GreaterEqualExp, SmallerEqualExp, EqualExp,
	      NotEqualExp, CastExp, DelayExp, InitExp, LpDelayExp,
	      LpInitExp, FuncAppExp, IndexedExp, CondExp, IdentLeafExp,
 	      FixedLeafExp, IntegerLeafExp, TupleExp, UserFuncExp, TimeMuxExp,
	      ReSampExp
	     } ExpressionKind;
typedef struct ExpressionNode {
   ExpressionKind ExpKind;
   SymbolType ExpType;
   Position ExpPos;
   int UniqueId;
   union {
      struct {
	 Expression LeftExp;
	 Expression RightExp;
      } SonsForBinOp;
      Expression CastedExp;
      struct {
	 Expression DelaySigExp;
         Expression DelayValExp;
      } SonsForDelay;
      struct {
         FunctionDefinitionList ThisFunction;
	 Expression ActualParameters;
	 int NrOfApps;
	 Expression NextFuncApp;
      } SonsForFuncApp;
      struct {
	 Identifier UserFuncId;
	 Expression ActualUserParameters;
      } SonsForUserFunc;
      struct {
	 Expression TimeMuxInputs;
	 int NrArrayInputs, NrArrayOutputs;
	 int NoPhase;
      } SonsForTimeMux;
      struct {
	 Expression ReSampInputs;
         NodeKindType Type;
	 int Scale;
	 int Phase;
      } SonsForReSamp;
      struct {
	 SymbolTable ThisArray;
	 Expression IndexTupleExp;
      } SonsForIndexed;
      struct {
	 Expression IfExp;
	 Expression ThenExp;
	 Expression ElseExp;
      } SonsForCond;
      SymbolTable ThisSymbol;  /* IdentLeaf */
      struct {
	 int M, S;
	 char *FixedString;
	 Expression NextFixed;
      } SonsForFixedLeaf;
      int IntegerConstant;  /* IntegerLeaf */
      struct {
	 Expression FirstExpression;
	 Expression RestOfTuple;
      } SonsForTuple;
   } ExpDef;
} ExpressionNode;
