/* $Id$ */

/* $Header$ */

/* Shared definitions for the Silage compiler. */
/* Copyright (C) 1984 by the Regents of the University of California. */

/* $Log$ */

/*----------------------------------------------------------------------*/
/* 									*/
/*	Various utilities						*/
/* 									*/
/*----------------------------------------------------------------------*/

typedef int **word;
	/* Random fullword type to serve as untyped word.
	 */

#define NEW(type) ( (type *) tmalloc(sizeof(type)))

void Punt(/* char *msg */ );

/*----------------------------------------------------------------------*/
/* 									*/
/*	Iliffe vectors							*/
/* 									*/
/*----------------------------------------------------------------------*/

/* An Iliffe vector is either a vector of elements (with bounds) or a
/* vector of Iliffe vectors.  The following routines manipulate them. */

typedef struct ILIFFE { 
	short L,U; 	/* Current lower and upper bounds. */
	short Dims;	/* Dimensionality of this vector */
	short F;	/* Current free space in Elts.
			 */
        word Elts[1];	/* Elements of vector. */
} Iliffe;

extern Iliffe *NewIliffe(/* int S,Dims */);
    /* Create a new Iliffe vector with bounds 0..-1, and dimensionality
    /* Dims.  S>0 is the estimated maximum size in words (defined as 
    /* ILUpper(V)-ILLower(V)+1.)
     */
extern void SetIliffe(/* Iliffe **V, int i, word X */);
    /* Set element i of V to X.  If element i does not exist, adjust V
    /* accordingly.
     */
extern void FreeIliffe(/* Iliffe *V */);
    /* Destroy the vector V.
     */

#define ILIndex(V,n) (&(V->Elts[n - V->L]))
    /* The result of coercing V[n..U] into a word[].
     */
#define ILLower(V) (V->L)
    /* Lower bound of V
     */
#define ILUpper(V) (V->U)
    /* Upper bound of V
     */
#define ILDims(V) (V->Dims)
    /* Dimensionality of V.
     */

/*----------------------------------------------------------------------*/
/* 									*/
/*	Abstract syntax tree definition. 				*/
/*									*/
/*----------------------------------------------------------------------*/

/* See doc/tree.form for details on how particular language constructs
 * are represented.
 */

#define NODENAMES \
    "CommaNode", "DefnNode", "IterateNode", "IteratorNode", "FuncDefnNode",\
    "FormalNode", "CastNode", "NumTypeNode", "ArrayTypeNode",\
    "DelayNode", "LpDelayNode", "OrNode", "AndNode", "ExorNode", "PlusNode", \
    "MinusNode", "MultNode", "DivNode", "IntDivNode", "CompMinusNode", \
    "NegateNode", "CompNode", "NotNode",\
    "FuncAppNode", "SelectNode", "AggregateNode", "CondNode",\
    "ArrayIndexNode", "PragmaNode", "IntegerLeaf", "FixedLeaf","IdentLeaf",\
    "ArrayNameNode", "BoxTypeNode", "BoxNode", "ValuedBoxNode",\
    "ValuedBoxTypeNode", "GreaterNode", "GreaterEqualNode", "SmallerNode",\
    "SmallerEqualNode", "EqualNode", "NotEqualNode", "LeftShiftNode",\
    "RightShiftNode", "InitNode", "LpInitNode", "TupleNode", "IntTypeNode",\
    "WhileNode", "CosNode", "SinNode", "SqrtNode", "MinNode", "MaxNode",\
    "BitNode", "BitSelectNode", "BitMergeNode",\
    "InterpolateNode", "DecimateNode", "TimeMuxNode",\
    "TimeDeMuxNode", "UpSampleNode", "DownSampleNode",\
    "BoolTypeNode", "DoNode", "ExitNode"
 
typedef enum { CommaNode, DefnNode, IterateNode, IteratorNode, FuncDefnNode,
	       FormalNode, CastNode, NumTypeNode, ArrayTypeNode,
	       DelayNode, LpDelayNode, OrNode, AndNode, ExorNode, PlusNode,
               MinusNode, MultNode, DivNode, IntDivNode, CompMinusNode, 
               NegateNode, CompNode, NotNode,
	       FuncAppNode, SelectNode, AggregateNode, CondNode,
	       ArrayIndexNode, PragmaNode, IntegerLeaf, FixedLeaf, IdentLeaf,
	       ArrayNameNode, BoxTypeNode, BoxNode, ValuedBoxNode, 
	       ValuedBoxTypeNode, GreaterNode, GreaterEqualNode, SmallerNode,
	       SmallerEqualNode, EqualNode, NotEqualNode, LeftShiftNode,
	       RightShiftNode, InitNode, LpInitNode, TupleNode, IntTypeNode,
	       WhileNode, CosNode, SinNode, SqrtNode, MinNode, MaxNode, 
	       BitNode, BitSelectNode, BitMergeNode,
	       InterpolateNode, DecimateNode, TimeMuxNode,
	       TimeDeMuxNode, UpSampleNode, DownSampleNode,
	       BoolTypeNode, DoNode, ExitNode }
   NodeKindType;

typedef struct ATTRTYPE {  /* Attribute list:
			   /* see discussion of symbols below. */
    		short AttrId;	/* Attribute's tag */
		short Type;	/* Arbitrary value for keeping run-time
				/* checking of attribute's type */
		int *Value;	/* Attribute's value */
		struct ATTRTYPE *Next;	/* Next element in attribute list */
} AttrType;

typedef struct NODETYPE {	/* Abstract syntax tree nodes. */
		int LineNum,CharNum;	/* Source position of text producing
					/* this node. */
		char *FileName;		/* File from which source came. */
		NodeKindType kind;	/* Type of node. */
		struct NODETYPE *L,*R;  /* Left and Right sons of node. */
		AttrType *Attrs; /* Attributes of node. */
}  NodeType;

#define ERRNODE ((NodeType *) -1)
    /* Used as a token to signal erroneous input to a function. */

    /* In the following macros, node is of type NodeType *.  */
#define Kind(node) ((node)->kind)
#define Left(node) ((node)->L)
#define Right(node) ((node)->R)
#define Line(node) ((node) -> LineNum)
#define CharPos(node) ((node) -> CharNum)
#define FileFrom(node) ((node)  -> FileName)
    
extern NodeType *MakeTree();
extern NodeType *MakeTree2();
    /* If T = MakeTree2(K,L,R,Ln,C,F), then
     * 	Kind(T) = K, Left(T) = L, Right(T) = R, Line(T) = Ln, CharPos(T) = C,
     *  FileFrom(T) = F.
     *
     * MakeTree(K,L,R) = MakeTree2(K,L,R,Line,Char,File), where Line, Char,
     * and File are the line number, character position, and file name
     * associated with R (if non-null) or else with L (if non-null), or else
     * are 0.
     */

extern NodeType *AppendTree();
extern NodeType *AppendValue();
    /* If T1 and T2 are comma lists of items of the form
     *    MakeTree(CommaNode,L1,MakeTree(CommaNode,L2,...,
     *			MakeTree(CommaNode,Ln,NULL)))...))
     * then AppendTree(T1,T2) produces the result of destructively appending
     * T2 to the end of T1.  AppendValue(T1,L) adds a new CommaNode pair to
     * to the end of T1 with NULL as the right son.
     */

extern void PrintTree();
extern void PrintFrac();
extern void PrintCSD();
extern int  EndParseMessage();

extern void Pass2();

extern int ErrorCount;	

/*----------------------------------------------------------------------*/
/*									*/
/*  Routines for handling symbols and attributes.			*/
/*									*/
/*----------------------------------------------------------------------*/

/* The following routine interfaces are for manipulating the symbol table
/* and attaching attributes to expressions.  Any (sub)expression can have any
/* number of attributes, each identified with a unique integer number.  An
/* attribute's value can be any integer or pointer quantity.

/* The symbol table maintains a mapping between identifiers and symbols.  A
/* ``symbol'' is simply an identifier leaf node (generally with attributes).
/* Symbols exist apart from the symbol table, which is only used during
/* semantic analysis to keep track of currently active declarations.

/* Abstractly, a symbol corresponds to a definition of a ``name,'' which is
/* an identifier together with 0 or more indices.  Because functions are
/* handled as macros, a single textual definition can correspond to a
/* number of actual definitions, and hence to a number of
/* symbols, one for each instantiation of the function containing
/* the definition, and within each of those instantiations, one for each value
/* of an attached iterator clause.

/* The semantic analyzer resolves all identifiers in the original abstract
/* syntax tree that refer to the same declaration to the same symbol.

/*----------------------------------------------------------------------*/

	/* Attributes */
extern int * _GetAttr(/* NodeType *exp, int attr, int type */ );
    /* Return value of attribute attr of node exp.   Returns -1 if the 
     * attribute does not exist.  Causes a fatal error if the type value
     * stored with the attribute does not match type.  The value should
     * be coerced as needed.
     */
extern void _SetAttr(/* NodeType *exp, int attr, int *Val, int type */);
    /* Set the value of attribute attr for expression exp to Val, and its
     * type value to type. */
extern int IsAttr(/* NodeType *exp, int attr */);
    /* Returns 1 if attribute attr of node exp exists, and otherwise 0. */

    	/* Some macros for common attribute types */
#define GetNodeAttr(node,attr) ( (NodeType *) _GetAttr(node,attr,1) )
#define GetIntAttr(node,attr) ( (int) _GetAttr(node,attr,2) )
#define GetBoolAttr(node,attr) ( (int) _GetAttr(node,attr,3) )

#define SetNodeAttr(node,attr,val) _SetAttr(node,attr,(int) val, 1)
#define SetIntAttr(node,attr,val) _SetAttr(node,attr,(int) val, 2)
#define SetBoolAttr(node,attr,val) _SetAttr(node,attr,(int) val, 3)


	/* Symbol table */

extern void ClearSym();
    /* Initialize the symbol table. */
extern NodeType * DefineSym( /* NodeType *id */ );
    /* Define a new symbol for the identifier in the leaf node id.
    /* Returns the new symbol and
    /* adds it to the symbol table.  If the symbol is already defined in the
    /* current symbol table block, returns NULL.
     */
extern NodeType * CreateSym();
    /* Like DefineSym, but generates a dummy leaf automatically (which
    /* does not match any other identifier)
    /* and does not enter the symbol in the symbol table.
     */
extern NodeType * Lookup(/* NodeType *id */);
    /* Returns the symbol currently mapped from from the identifier id.
    /* Returns NULL if there is none.
     */
extern void OpenBlock();
    /* Starts a new scope in the symbol table. */
extern void CloseBlock();
    /* Removes the current scope and all declarations in it from the symbol
    /* table.
     */
extern NodeType * NthSymbol(/* int n */);
    /* Return the nth symbol in the current scope, where the first symbol is
    /* numbered 0.  Returns NULL if there is no nth symbol.
     */

/*----------------------------------------------------------------------*/
/*									*/
/*  Routines for handling names.					*/
/*									*/
/*----------------------------------------------------------------------*/

/* A ``name'' is either a symbol or a symbol and a sequence of index values.
/* The latter results from definitions of the form A[i] = ....

/* For any array defined in the program, there is a symbol node corresponding
/* to that array.  In addition, if there are explicitly defined elements of
/* that array, i.e., elements defined A[...][...]... = ..., then the 
/* ElementsAttr attribute of the array's symbol contains an Iliffe vector
/* containing symbols for each defined element.
 */




extern NodeType DefineName( /* NodeType *id, int n,indices[] */ );
    /* Define a new symbol for id[indices[0],...,indices[n-1]], where
    /* n>=0, and id is a simple identifier node.  If this name is already
    /* defined, return NULL. If id is incompatibly defined (e.g. as a
    /* non-array or an array with the wrong dimensionality, return 
    /* ERRNODE and otherwise return the new symbol.
     */

extern NodeType LookupName( /* NodeType *id, int n,indices[] */ );
    /* If id[indices[0],...,indices[n]] is specifically defined (i.e., by
    /* a previous DefineName), return its symbol.  If id is defined
    /* as an array of the appropriate dimensionality, return its symbol.
    /* If id is defined, but not as an array of the appropriate dimensions,
    /* return ERRNODE.  Otherwise, return NULL.
     */

