/* $Id$ */

/* 
 * Major include file for FlowGraph
 */

/* dependent include files */
#include <stdio.h>

/* Constant definitions */
#define FALSE 0
#define TRUE 1
#define UNDEFINED -1

#define IN 0
#define OUT 1

#define WORDLENGTH 128

#define CONTROL_EDGE "control"
#define DATA_EDGE "data"
#define ARRAY_EDGE "array"
#define HIERARCHY_NODE "MODULE"
#define LEAF_NODE "LEAF"
#define MACRO_NODE "MACRO"

/* global variables */
extern int fatal;	/* Generic error recovery variable 	*/
extern int debugFlag;	/* Program in debug mode		*/

extern char ProgramName[]; /* Name of Program			*/

/* Definitions of internal data structures */
#include "flowstruct.h"

/* Definitions of useful macros */
#include "flowmacro.h"
