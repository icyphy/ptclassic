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
 * Text File Scanning Routines
 *
 * David Harrison
 * University of California, 1985
 *
 * This file contains the implementation of routines useful for scanning
 * the lisp-like text files read by the technology package.
 */

#include "port.h"
#include "scan.h"

#define MAXBUF 1024

static char bufAry[MAXBUF];


int scanItem(filePtr, item)
FILE *filePtr;			/* File to scan          */
lexUnit *item;			/* Returned lexical item */
/*
 * Reads the next lexical unit.  The lexical units supported are defined
 * in the header file for the module.  The routine normally
 * returns zero.  When EOF is reached,  it returns SCANEOF.  NOTE: the
 * strings returned are owned by this package and should be copied for
 * personal use.  Blank space: space, tab, \n.  Implementation is messy.
 */
{
    register short c;
    register int i;

    /* Skip over blank space */
    while (((c = getc(filePtr)) != EOF) &&
	   ((c == ' ') || (c == '\t') || (c == '\n')))
      /* nothing */ ;
    if (c == EOF) return(SCANEOF);
    if (c == '(') {
	item->lexType = LEFTPAREN;
	return(0);
    }
    if (c == ')') {
	item->lexType = RIGHTPAREN;
	return(0);
    }
    if (c == '"') {
	i = 0;
	while (((c = getc(filePtr)) != EOF) && (c != '"'))
	  bufAry[i++] = c;
	if (c == EOF) return(SCANERROR);
	bufAry[i] = '\0';
	item->lexType = STRINGCONST;
	item->lexUnion.strData = bufAry;
	return(0);
    }
    if ((c >= '0') && (c <= '9')) {
	item->lexType = NUMBERCONST;
	item->lexUnion.numData = c - '0';
	while (((c = getc(filePtr)) != EOF) &&
	       ((c >= '0') && (c <= '9')))
	  item->lexUnion.numData = item->lexUnion.numData * 10 + (c - '0');
	ungetc(c, filePtr);
	return(0);
    }
    if (c == '-') {
	/* Could be a negative number or an identifier (peek to see which) */
	c = getc(filePtr);  ungetc(c, filePtr);
	if ((c >= '0') && (c <= '9')) {
	    /* Its a negative number - read using scanItem */
	    scanItem(filePtr, item);
	    item->lexUnion.numData = -item->lexUnion.numData;
	    return(0);
	}
	/* Else let it fall into identifier code (not pretty!) */
	c = '-';
    }
    /* All that's left is identifier.  Read to blank space character */
    i = 0;
    bufAry[i++] = c;
    while (((c = getc(filePtr)) != EOF) &&
	   (c != ' ') && (c != '\t') && (c != '\n'))
      bufAry[i++] = c;
    bufAry[i] = '\0';
    item->lexType = IDENTIFIER;
    item->lexUnion.strData = bufAry;
    return(0);
}



int scanToOpen(filePtr)
FILE *filePtr;			/* Input file */
/*
 * This routine scans to the next open parenthesis.  If it finds one,
 * it returns zero.  Otherwise,  it returns SCANEOF.
 */
{
    register short c;

    while (((c = getc(filePtr)) != EOF) && (c != '('))
      /* nothing */ ;
    if (c == EOF) return(SCANEOF);
    else return(0);
}

int scanOverList(filePtr, depth)
FILE *filePtr;			/* Input file    */
int depth;			/* Depth in list */
/*
 * This routine scans over the remaining portion of lisp-like list
 * structure.  The depth indicates how deeply nested in the list
 * the scanner already is.  A depth of one is the normal case.
 * Returns zero if everything when well or SCANEOF if it did
 * not find the necessary closing parenthesis.
 */
{
    register short c;

    while (depth > 0) {
	while (((c = getc(filePtr)) != EOF) &&
	       (c != '(') && (c != ')'))
	  /* nothing */ ;
	if (c == EOF) return(SCANEOF);
	if (c == '(') depth++;
	if (c == ')') depth--;
    }
    return(0);
}
