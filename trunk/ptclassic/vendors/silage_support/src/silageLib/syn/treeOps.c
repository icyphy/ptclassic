/* $Id$ */

/* Tree manipulation for the silage compiler. */

#include <stdio.h>
#include "silage.h"

NodeType *MakeTree(kind,left,right)
    NodeKindType kind;
    NodeType *left,*right;
{
    if (right == NULL && left != NULL) 
        return(MakeTree2(kind,left,right,left -> LineNum, left -> CharNum,
			 left -> FileName));
    if (right != NULL)
        return(MakeTree2(kind,left,right,right -> LineNum,right -> CharNum,
			 right -> FileName));
    return (MakeTree2(kind,left,right,0,0,NULL));
}

NodeType *
MakeTree2(kind,left,right,line,Chr,file)
    NodeKindType kind;
    NodeType *left,*right;
    int line,Chr;
    char *file;
{
    NodeType *N;

    N = NEW(NodeType);
    
    N -> L = left; N -> R = right; N -> kind = kind;
    N -> Attrs = NULL;
    N -> LineNum = line; N -> CharNum = Chr; N -> FileName = file;
    return N;
}
	
NodeType *
MakeTree3(kind,left, right)
NodeKindType kind;
NodeType *left,*right;
{
    if (left == NULL && right == NULL)
	return (NULL);
    /* else if (left == NULL)
	return (MakeTree(kind, right, NULL)); */
    else
	return (MakeTree(kind, left, right));
}
 
NodeType *
AppendTree(T1, T2)
     NodeType *T1,*T2;
    /* Given comma lists T1 and T2, return the result of destructively
       appending them. */
{
	NodeType *T;
	
	if (T1 == NULL) return T2;
	for (T = T1; T -> R != NULL; T = T -> R);
	T -> R = T2;
	return T1;
}

NodeType *
AppendValue(T1,V)
    NodeType *T1,*V;
    /* Given comma-list T1 and node V, add V to end of T1. */
{
    NodeType *T;

    if (T1 == NULL) 
        return MakeTree(CommaNode,V,NULL);
    for (T = T1; T -> R != NULL; T = T -> R);
    T -> R = MakeTree(CommaNode,V,NULL);
    return T1;
}

NodeType *
CopyTree(T)
NodeType *T;
{
    NodeType *N, *CopyTree();

    if (T == NULL)
	return (NULL);
    switch (Kind(T)) {
    case IdentLeaf:
    case IntegerLeaf:
    case FixedLeaf :
        N = NEW(NodeType);
        N -> L = T->L; N -> R = T->R; N -> kind = T->kind;
        N -> Attrs = T->Attrs;
        N -> LineNum = T->LineNum; N -> CharNum = T->CharNum;
        N -> FileName = T->FileName;
        return N;
    default :
        N = NEW(NodeType);
        N -> L = CopyTree(Left(T)); N -> R = CopyTree(Right(T));
	N -> kind = T->kind; N -> Attrs = T->Attrs;
        N -> LineNum = T->LineNum; N -> CharNum = T->CharNum;
        N -> FileName = T->FileName;
        return N;
    }
}
	
FreeTree(T)
NodeType *T;
{
    if (T == NULL)
	return;
    switch (Kind(T)) {
    case IntegerLeaf :
    case FixedLeaf :
    case IdentLeaf :
	free (T);
	break;
    default :
	FreeTree(Left(T));
	FreeTree(Right(T));
        free (T);
	break;
    }
}

int
TreeCompare(Tree1, Tree2)
NodeType *Tree1, *Tree2;
{
    if (!Tree1 && !Tree2)
	return (1);
    if (Tree1 && !Tree2 || Tree2 && !Tree1 || Kind(Tree1) != Kind(Tree2))
	return (0);

    switch (Kind(Tree1)) {
	case IntegerLeaf :
	case IdentLeaf :
	    return (Left(Tree1) == Left(Tree2));
        case FixedLeaf :
	    return ((Left(Tree1) == Left(Tree2)) && 
		    (Right(Tree1) == Right(Tree2)));
	default :
	    return (TreeCompare(Left(Tree1), Left(Tree2)) &&
		    TreeCompare(Right(Tree1), Right(Tree2)));
    }
}
