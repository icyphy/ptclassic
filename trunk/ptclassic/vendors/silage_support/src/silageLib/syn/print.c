/* $Id$ */


/* Tree printout routines */

#include <stdio.h>
#include "silage.h"

#define sPrintFrac PrintFrac

static char *NodeNames[] = { NODENAMES };
extern NodeType *TheTree;

static void INDENT(n)
    int n;
{
    for (; n > 7; n -= 8) printf("        ");
    for (; n > 0; n--) printf(" ");
}

void PrintTotalTree()
{
    PrintTree (TheTree, 0);
}

void PrintTree(T,indent)
    NodeType *T;
    int indent;
{
	int moreIndent=6;
    if (T == NULL) {
	INDENT(indent); printf("-*-\n"); return;
    }
    switch (Kind(T)) {
	case IntegerLeaf:
	    INDENT(indent); printf("%d\n",Left(T));
	    break;
	case FixedLeaf:
	    INDENT(indent);
	    fPrintFrac(Left(T), Right(T), stdout);
	    printf("\n");
	    /* printf(" (");
	    PrintCSD(Left(T), Right(T), stdout);
	    printf(")\n"); */
	    break;
	case IdentLeaf:
	    INDENT(indent); printf("%s\n",Left(T));
	    break;
	case CommaNode:
	    PrintTree(Right(T),
			    (Right(T) == NULL 
				? indent+moreIndent: indent+moreIndent));
	    INDENT(indent); printf("CommaNode\n");
	    PrintTree(Left(T),indent+moreIndent);
	    break;
	default:
	    PrintTree(Right(T),indent+moreIndent);
	    INDENT(indent); printf("%s\n",NodeNames[(int) Kind(T)]);
	    PrintTree(Left(T),indent+moreIndent);
	    break;
    }
}

#define ORIGINALNODENAMES \
    ",", "=", "IterateNode", "IteratorNode", "FuncDefnNode",\
    "FormalNode", "CastNode", "NumTypeNode", "ArrayTypeNode",\
    "@", "#", " | ", " & ", " ^ ", "+", "-", "*",\
    "/", " div ", "~", "-", \
    "~", " ! ",\
    "FuncAppNode", "->", "AggregateNode", "CondNode",\
    "", "pragma(", "IntegerLeaf", "FixedLeaf","IdentLeaf",\
	"", "BoxTypeNode", "BoxNode", "ValuedBoxNode",\
	"ValuedBoxTypeNode", ">", ">=", "<",\
	"=<", "==", "<>","<<",">>"

static	char	*OriginalNodeNames[]={ ORIGINALNODENAMES	};


PrintTreeOriginal(T,String)
NodeType *T;
char *String;
{
    char String1[256], String2[256];

    if (T == NULL) {
	sprintf(String,"");
	return;
    }
    switch (Kind(T)) {
	case IntegerLeaf:
	    sprintf(String,"%d",(int) Left(T));
	    break;
	case FixedLeaf:
	    sPrintFrac(Left(T), Right(T), String);
	    break;
	case IdentLeaf:
	    sprintf(String, "%s", (char *) Left(T));
	    break;
        case ArrayNameNode:
            PrintTreeOriginal(Left(T),String1);
            PrintTreeOriginal(Right(T),String2);
            sprintf(String, "[%s %s]", String1, String2);
            break;
        case ArrayIndexNode:
            PrintTreeOriginal(Left(T), String1);
            PrintTreeOriginal(Right(T),String2);
            sprintf(String,",%s %s", String1, String2);
            break;
	case NumTypeNode :
	    PrintTreeOriginal(Left(T), String1);
	    PrintTreeOriginal(Right(T), String2);
	    sprintf(String,"num<%s,%s>", String1, String2);
	    break;
	case BoxTypeNode :
	case ValuedBoxTypeNode :
	    break;
	case CastNode :
	    PrintTreeOriginal(Right(T), String2);
	    sprintf(String,"%s", String2);
	    break;
	case FuncAppNode :
	    PrintTreeOriginal(Left(T), String1);
	    PrintTreeOriginal(Right(T), String2);
	    sprintf(String,"%s(%s)", String1, String2);
	    break;
	case CondNode :
	    PrintTreeOriginal(Left(Left(T)), String1);
	    PrintTreeOriginal(Right(Left(T)), String2);
	    sprintf(String,"if (%s) -> %s", String1, String2);
	    PrintTreeOriginal(Right(T), String2);
	    sprintf(String,"%s %s fi", String, String2);
	    break;
	case CommaNode:
	    PrintTreeOriginal(Left(T), String1);
	    PrintTreeOriginal(Right(T), String2);
	    if (Right(T) == NULL)
		sprintf(String,"%s", String1);
	    else
	        sprintf(String,"%s,%s", String1, String2);
	    break;
	case NegateNode:
	case CompNode:
	case NotNode:
	    PrintTreeOriginal(Left(T), String1);
	    sprintf(String,"%s%s", OriginalNodeNames[ (int) Kind(T)],
		    String1);
	    break;
	default:
	    PrintTreeOriginal(Left(T), String1);
	    PrintTreeOriginal(Right(T), String2);
	    sprintf(String,"%s%s%s", String1, OriginalNodeNames[ (int) Kind(T)],
				     String2);
	    break;
    }
}

/************************************************************************/

# define MAX_FRACTIONAL 8	/* Maximal length of fractional decimal part */

void PrintFrac(M,S, String)
int M,S;
char *String;
{
    int i = 0;

    if (M < 0) {
	sprintf(String,"-"); M = -M;
    }
    else
	*String = '\0';
    if (S == 1) { /* Integer Number */
	sprintf(String,"%s%d", String, M);
    }
    else {
        sprintf(String,"%s%d.", String, M/S);
        M = M%S;
        while (M != 0 && i++ < MAX_FRACTIONAL) {
	    sprintf(String,"%s%1d", String, (M*10)/S);
	    M = (M*10)%S;
	}
    }
}

fPrintFrac(M,S, Fptr)
int M, S;
FILE *Fptr;
{
    char String[256];

    PrintFrac(M,S, String);
    fprintf(Fptr, "%s", String);
}

#define WL 32

void PrintCSD(M,S,Fptr)
int M,S;
FILE *Fptr;
    /* Print M*2**S in canonical signed digit form. */
{
    char Buffer[WL+4];
    int i,j,M2;
    
    M2 = M; M = abs(M);
    i = WL+1; Buffer[WL+2] = '\''; Buffer[WL+3] = NULL;
    if (S > 0) {
	Buffer[i--] = '.';
	for (j = S; j>0; j--) Buffer[i--] = '0';
    }
    while (M != 0 || i >= S+WL+1) {
	if (i == S+WL+1) {
	    Buffer[i--] = '.';
	    if (M == 0) break;
	}
	if ((M&7) == 7) {
	    Buffer[i] = '-';
	    M++;
	}
	else if ((M&1) == 1) Buffer[i] = '1';
	else Buffer[i] = '0';
	M >>= 1;
	i--;
    }
    if (M2 < 0) fprintf(Fptr,"-");
    fprintf(Fptr,"%s",Buffer+i+1);
}
