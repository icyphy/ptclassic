/*
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the
above copyright notice and the following two paragraphs appear in all
copies of this software.

IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY
FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES
ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF
THE UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF
SUCH DAMAGE.

THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE
PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE UNIVERSITY OF
CALIFORNIA HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
ENHANCEMENTS, OR MODIFICATIONS.

						PT_COPYRIGHT_VERSION_2
						COPYRIGHTENDKEY
*/
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
