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
/* paramStructs.c  edg
Version identification:
$Id$
This file contains functions that manage param data structures.
ParamList and param string conversion functions.
Caveats: Need to be able to free allocated memory in ParamListType.
    This module should use an object oriented approach instead of
    having user worry about param structs.
Updates: 4/14/89 to PStrToPList()
*/


/* Includes */
#include "local.h"
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>
#include "util.h"
#include "err.h"
#include "paramStructs.h"

/* FreeFlatPList 10/26/95 BLE */
void
FreeFlatPList(pListPtr)
ParamListType *pListPtr;
{
    if (pListPtr) {
        if (pListPtr->array) {
	    /* Note that we can't necessarily free the array[i].name
               array[i].type, array[i].value strings, since
	       routines in compile.c and octIfc.c do not create
	       new copies of the name and value fields, but instead
	       are indices into a single string.
	       See also the comment at the top of  POct::DeletePList().
	     */
	    free(pListPtr->array);
	    pListPtr->length = 0;
	    pListPtr->array = 0;
        }
	if (pListPtr->dynamic_memory) {
	    free(pListPtr->dynamic_memory);
	    pListPtr->dynamic_memory = 0;
	}
    }
}


/* PStrToPList  11/14/88 4/18/88
Converts a param str to a ParamList.
Use FreeFlatList to deallocate the ParamList allocated by PStrToPList.
Inputs:
    pStr = param string (can be empty, ex: "0").  pStr is not changed.
    pListPtr = the address of a ParamListType node
Outputs:
    return = TRUE if ok, else FALSE and pListPtr points to an empty list.
	Returns FALSE for the "" case.
    pListPtr = if ok, points to a ParamList where params are stored.
	The ParamList array is dynamically allocated.  Routines can
	free() memory in ParamList array when no longer needed.  The
	length of the returned pList can be 0 (empty list).
Updates:
    4/14/89 = returns a ParamList which has 1 extra empty slot at
	the end for EditFormalParams().  (Kludgy fix, but it works.)
*/
#define CTLA 1			/* control-A */

boolean
PStrToPList(pStr, pListPtr)
const char *pStr;
ParamListType *pListPtr;
{
    int i, param_n;
    char *from, *to, *copy;
    ParamType *place;
    int sep, err = FALSE;

    /* return a pList length of 0 by default */
    pListPtr->length = 0;
    pListPtr->array = 0;
    pListPtr->dynamic_memory = 0;
    pListPtr->flat_plist_flag = TRUE;

    if (*pStr == '\0') {
	/* invalid param string: star is not a member of knownstars */
	ErrAdd("PStrToPList#1: empty PStr (star not in knownstars?)");
	return(FALSE);
    }

    param_n = atoi(pStr);

    if (param_n == 0) {
	/* assume there are no params */
	/* make an extra slot */
    	pListPtr->array = (ParamType *) calloc(1, sizeof(ParamType));
	return(TRUE);
    }

/* We support old-style parameters (with '|' as the separator and no
 * type information) and new-style (with ^A as the separator).
 */

    sep = CTLA;
    from = strchr(pStr, sep);
    if (from == NULL) {
	sep = '|';
	if ((from = strchr(pStr, sep)) == NULL) {
	    /* can't find separator between length and first param! */
	    return(FALSE);
        }
    }

    /* Duplicate Lisp param string, skiping the # of params, so we can chop 
       it up into smaller strings.
    */
    copy = DupString(++from);	/* copy all the params in pStr */

    /* Allocate space for pList array, plus extra slot 4/14/89 */
    pListPtr->array = (ParamType *) calloc(param_n + 1, sizeof(ParamType));

    from = copy;		/* from points to first param name of copy */
    place = pListPtr->array;	/* points to first pList param slot */

    for (i = 0; i < param_n; i++) {
	/* set param name at current place */
	if ((to = strchr(from, sep)) == NULL) {
	    err = TRUE;
	    break;
	}
	*to = '\0';
	place->name = from;
	from = ++to;
	if (sep == '|') place->type = 0;
	else {
	    /* set param type at current place */
	    if ((to = strchr(from, sep)) == NULL) {
		err = TRUE;
		break;
	    }
	    *to = '\0';
	    place->type = from;
	    from = ++to;
	}
	/* set param value at current place */
	if ((to = strchr(from, sep)) == NULL) {
	    err = TRUE;
	    break;
	}
	*to = '\0';
	place->value = from;
	from = ++to;

	place++;
    }
    if (err) {
	ErrAdd("PStrToPList: format error in param from facet");
	free(copy);
	FreeFlatPList(pListPtr);
	return(FALSE);
    }
    pListPtr->length = param_n;
    pListPtr->dynamic_memory = copy;

    return(TRUE);
}


/* PListToPStr  4/18/88
Converts a ParamList to a param str.
Inputs:
    pListPtr = the address of a ParamList.
Outputs:
    return = a formatted param string.  Can free() when no longer needed.
*/
char *
PListToPStr(pListPtr)
ParamListType *pListPtr;
{
    char *strBuf, *paramBuf;
    char tempBuf[32];
    int i, index, maxparambuflen, parambuflen, strbuflen;
    ParamType *place;

    /* Compute how large of a parameter string we need */
    sprintf(tempBuf, "%d%c", pListPtr->length, CTLA);
    strbuflen = strlen(tempBuf);
    maxparambuflen = strbuflen;
    place = pListPtr->array;
    for (i = 0; i < pListPtr->length; i++) {
	if (!place->type) place->type = "";
	parambuflen = strlen(place->name) + strlen(place->type) +
		      strlen(place->value) + 3;
	if ( parambuflen > maxparambuflen ) maxparambuflen = parambuflen;
	strbuflen += parambuflen;
	place++;
    }

    /* Allocate buffers: add one to length for the end-of-string marker */
    strBuf = (char *) malloc(strbuflen + 1);
    paramBuf = (char *) malloc(maxparambuflen + 1);
    if ( paramBuf == 0 || strBuf == 0 ) {
	if ( paramBuf ) free(paramBuf);
	if ( strBuf ) free(strBuf);
	ErrAdd("PListToPStr: malloc failed.");
	return(0);
    }

    /* Create the parameter string */
    sprintf(strBuf, "%d%c", pListPtr->length, CTLA);
    index = strlen(strBuf);
    place = pListPtr->array;
    for (i = 0; i < pListPtr->length; i++) {
	if (!place->type) place->type = "";
	sprintf(paramBuf, "%s%c%s%c%s%c", place->name, CTLA,
		place->type, CTLA, place->value, CTLA);
	strcpy(&strBuf[index], paramBuf);
	index += strlen(paramBuf);
	place++;
    }

    free(paramBuf);
    return(strBuf);
}


/* CopyFlatPList 11/10/95 BLE
Makes a dynamic copy of a flat parameter list.
If the source parameter list pointer is equal to the destination
pointer, then the source parameter list is freed before it is overwritten.
*/
boolean
CopyFlatPList(srcPtr, destPtr)
ParamListType* srcPtr;
ParamListType* destPtr;
{
    char *parameterString;
    boolean retval;

    parameterString = PListToPStr(srcPtr);
    if (srcPtr == destPtr) FreeFlatPList(srcPtr);
    retval = PStrToPList(parameterString, destPtr);
    free(parameterString);

    return(retval);
}
