/*
 * Text File Scanning Package Header File
 *
 * David Harrison
 * University of California, 1985
 *
 * This file contains definitions required for use of the scanning routines
 * implemented in scan.c.  Other include files which should be included
 * with this one:
 *   general.h
 */

#ifndef SCAN
#define SCAN

#include "general.h"

#define SCANEOF		-1	/* Returned by scanItem at end of file */
#define SCANERROR	-2	/* Bad lexical unit read at this pos.  */

/* Lexical unit structure */

#define	LEFTPAREN	1
#define RIGHTPAREN	2
#define IDENTIFIER	3
#define NUMBERCONST	4
#define STRINGCONST	5

typedef struct lex_unit {
    int lexType;
    union {
	STR strData;		/* IDENTIFIER, STRINGCONST */
	int numData;		/* NUMBERCONST */
    } lexUnion;
} lexUnit;

extern int scanItem();
  /* Reads the next lexical unit */
extern int scanToOpen();
  /* Scans to next open parenthesis */
extern int scanOverList();
  /* Skips remaining portion of lisp like list */

#endif
