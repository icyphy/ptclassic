/* $Id$ */

/* Author : Chris Scheers */
/* perr.c -- version 1.11 (IMEC)		last updated: 3/1/89 */
static char *SccsId = "@(#)perr.c	1.11 (IMEC)	89/03/01";

#include <stdio.h>
#include "sfg.h"

char *Nodename[] = { NODENAMES };

void perr (nr, pos)
int nr;
Position pos;
{
   switch (nr) {
      case 1 :
	 printmsg (NULL, "Only Function definitions allowed at top level.");
	 break;
      case 2 :
	 printmsg (NULL, "Function defined more than once.");
	 break;
      case 3 :
	 printmsg (NULL, "Could not locate root (main) function.");
	 break;
      case 4 :
	 printmsg (NULL, "Input parameter declared more than once.");
	 break;
      case 5 :
	 printmsg (NULL, "Type must be specified.");
	 break;
      case 6 :
	 printmsg (NULL, "Illegal type specification.");
	 break;
      case 7 :
	 printmsg (NULL, "Not implemented.");
	 break;
      case 8 :
	 printmsg (NULL, "Output parameter declared more than once.");
	 break;
      case 9 :
	 printmsg (NULL, "Input parameter can not be redefined.");
	 break;
      case 10 :
	 printmsg (NULL, "Value defined more than once.");
	 break;
      case 11 :
	 printmsg (NULL, "Return value not defined in function body.");
	 break;
      case 12 :
	 printmsg (NULL, "Illegal left hand side of definition.");
	 break;
      case 13 :
	 printmsg (NULL, "Undefined value.");
	 break;
      case 14 :
	 printmsg (NULL, "Function name can not be used as value name.");
	 break;
      case 15 :
	 printmsg (NULL, "Undefined function.");
	 break;
      case 16 :
	 printmsg (NULL, "Illegal expression in this context.");
	 break;
      case 17 :
	 printmsg (NULL, "Delay value must be >= zero.");
	 break;
      case 18 :
	 printmsg (NULL, "Recursive function application not allowed.");
	 break;
      case 19 :
	 printmsg (NULL, "Cyclic value definition not allowed.");
	 break;
      case 20 :
	 printmsg (NULL, "Type of symbol or expression unknown.");
	 break;
      case 21 :
	 printmsg (NULL, "Type conflict.");
	 break;
      case 22 :
	 printmsg (NULL, "Too few input/output parameters.");
	 break;
      case 23 :
	 printmsg (NULL, "Too many input/output parameters.");
	 break;
      case 24 :
	 printmsg (NULL, "Illegal coercion.");
	 break;
      case 25 :
	 printmsg (NULL, "No delay of indexed input/output parameter allowed.");
	 break;
      case 26 :
	 printmsg (NULL, "Array has too many dimensions.");
	 break;
      case 27 :
	 printmsg (NULL, "Illegal bound for array.");
	 break;
      case 28 :
	 printmsg (NULL, "Illegal name for iterator.");
	 break;
      case 29 :
	 printmsg (NULL, "Nested iterators must have different name.");
	 break;
      case 30 :
	 printmsg (NULL, "Illegal bounds for iteration.");
	 break;
      case 31 :
	 printmsg (NULL, "Tuple's don't match");
	 break;
      case 32 :
	 printmsg (NULL, "Dimensions of input parameter missing.");
	 break;
      case 33 :
	 printmsg (NULL, "Dimensions of array not needed.");
	 break;
      case 34 :
	 printmsg (NULL, "Negative index not allowed.");
	 break;
      case 35 :
	 printmsg (NULL, "Local symbol declared more than once.");
	 break;
      case 36 :
	 printmsg (NULL, "Illegal array indexing.");
	 break;
      case 37 :
	 printmsg (NULL, "Dimensions of array needed.");
	 break;
      case 38 :
	 printmsg (NULL, "No delays allowed in sequential process.");
	 break;
      case 39 :
	 printmsg (NULL, "Illegal type for parameters of 'main'.");
	 break;
      case 40 :
	 printmsg (NULL, "Expression too complex.");
	 break;
      case 41 :
	 printmsg (NULL, "Value consumed before produced.");
	 break;
      case 42 :
	 printmsg (NULL, "Array index out of bounds.");
	 break;
      case 43 :
	 printmsg (NULL, "Single assignment violation.");
	 break;
      case 44 :
	 printmsg (NULL, "No Delay allowed in this context.");
	 break;
      case 45 :
	 printmsg (NULL, "Multi-rate inputs/outputs must have the same type.");
	 break;
      case 46 :
	 printmsg (NULL, "Illegal usage of multi-rate operation.");
	 break;
      case 47 :
	 printmsg (NULL, "Multi-rate operation has incorrect number of inputs.");
	 break;
      case 48 :
	 printmsg (NULL, "Multi-Rate operation has incorrect number of outputs.");
	 break;
      case 49 :
	 printmsg (NULL, "Illegal type in this context.");
	 break;
      case 50 :
	 printmsg (NULL, "Manifest expression expected.");
	 break;
      case 51 :
	 printmsg (NULL, "Illegal operand of multi-rate operation.");
	 break;
      case 52 :
	 printmsg (NULL, "No communication channel between these two processes.");
	 break;
      case 53 :
	 printmsg (NULL, "Cut operation must have equal number of inputs and outputs.");
	 break;
      case 54 :
	 printmsg (NULL, "Operand of multi-rate operation missing.");
	 break;
      case 55 :
	 printmsg (NULL, "Multi-rate function only allowed in the main function.");
	 break;
      case 56 :
	 printmsg (NULL, "Values global to all processes must be constant.");
	 break;
      default :
	 printmsg (NULL, "Unknown Error.");
   }
   printmsg (NULL, "\n");
   if (pos.FileName != NULL)
      printmsg(NULL, "File %s, Line %d, (Character %d)\n",
		      pos.FileName, pos.LineNum, pos.CharNum);
/* printmsg(NULL, "NodeKind: %s", Nodename[pos.kind]);
   if (pos.L != NULL)
      printmsg(NULL, ", LeftSon: %s", Nodename[pos.L->kind]);
   if (pos.R != NULL)
      printmsg(NULL, ", RightSon: %s", Nodename[pos.R->kind]);
   printmsg(NULL, "\n"); */
   exit (1);
}

void Crash (c)
char *c;
{
   printmsg (NULL, "Inconsistency in : %s.\n", c);
   exit (1);
}
