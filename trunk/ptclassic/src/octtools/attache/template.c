#ifndef lint
static char SccsId[]="$Id$";
#endif /*lint*/
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
#include "copyright.h"
#include "port.h"
#include "utility.h"
#include "options.h"
#include "oct.h"
#include "oh.h"
#include "st.h"
#include "errtrap.h"
#include "io.h"
#include "internal.h"
#include "obj.h"
#include "command.h"
#include "update.h"

#include "template.h"

#define TEMP_STOPS	"\r\n\t\016\020\04\033"

static int tField, tFieldCount;
static int tCurY, tCurX;
static int (*tCurGetFn)();
static char *tCurDatumPtr;
static struct enumTemplate *tCurTemplatePtr;
static char tCurString[1024];
static octObject *tCurObjPtr;
static struct fieldTemplate *recursionTemplate;
static int matchString();

void printTemplate(templatePtr, objPtr)
struct fieldTemplate *templatePtr;
octObject *objPtr;
{
    char *datumPtr;
    char *currentString = (char *)NULL;

    while (templatePtr->text != NIL(char)) {
	IOputs(templatePtr->text);
	datumPtr = ((char *) objPtr) + templatePtr->offset;
	recursionTemplate = NIL(struct fieldTemplate);
	if (templatePtr->printFn != NILPF) {
	    currentString = (*templatePtr->printFn)
				    (datumPtr, templatePtr->enumList, objPtr);
	    IOputsSee(currentString);
	}
	if (templatePtr->getFn != NILGF) {
	    if (--tFieldCount == 0) {
		tCurDatumPtr = datumPtr;
		(void) strcpy(tCurString, currentString);
		IOgetyx(&tCurY, &tCurX);
		tCurGetFn = templatePtr->getFn;
		tCurTemplatePtr = templatePtr->enumList;
		tCurObjPtr = objPtr;
	    }
	}
	if (recursionTemplate) printTemplate(recursionTemplate, objPtr);
	templatePtr++;
    }
}

void initGetTemplate(reset)
int reset;
{
    if (reset == RESET_FIELD) tField = 1;
    tFieldCount = tField;
    tCurGetFn = NILGF;
}

int
getTemplate()
{
    int stopCh;

    if (tCurGetFn == NILGF) {
	if (tField == 1) return(0);
	tField = 1;
    } else {
	IOmove(tCurY, tCurX);
	IOrefresh();
	stopCh = (*tCurGetFn) (tCurDatumPtr, tCurTemplatePtr);
	switch (stopCh) {
	    case '\n':
	    case '\r':
	    case '\t':
	    case CNTRL('N'):
		tField++;
		break;
	    case CNTRL('P'):
		tField--;
		break;
	    case CNTRL('D'):
		return(0);
	}
    }
    IObotHome();
    IOclearLine();
    return(1);
}

/*ARGSUSED*/
char *tPrintString(cptr, tmpPtr, objPtr)
char *cptr;
struct enumTemplate *tmpPtr;
octObject *objPtr;
{
    char *strPtr = * ((char **) cptr);

    if (strPtr == NIL(char)) {
	return("");
    } else {
	return(strPtr);
    }
}

/*
 *	tPrintName
 *
 *	Like tPrintString, except uses fixed-width display.
 */
/*ARGSUSED*/
char *tPrintName(cptr, tmpPtr, objPtr)
char *cptr;
struct enumTemplate *tmpPtr;
octObject *objPtr;
{
    char *strPtr = * ((char **) cptr);
    static char string[30];

    if (strPtr == NIL(char)) {
	strPtr = "";
    }
    (void) sprintf(string, "%-15.25s", strPtr);
    return(string);
}

/*ARGSUSED*/
char *tPrintInteger(cptr, tmpPtr, objPtr)
char *cptr;
struct enumTemplate *tmpPtr;
octObject *objPtr;
{
    int32 number = * (int32 *) cptr;
    static char string[20];

    (void) sprintf(string, "%ld", (long) number);
    return(string);
}

/*
 *	tPrintCoord
 *
 *	Like tPrintInteger, except uses octCoord and holds a minimum
 *  width of 6 digits.
 */
/*ARGSUSED*/
char *tPrintCoord(cptr, tmpPtr, objPtr)
char *cptr;
struct enumTemplate *tmpPtr;
octObject *objPtr;
{
    octCoord number = * (octCoord *) cptr;
    static char string[20];

    (void) sprintf(string, "%6ld", (long) number);
    return(string);
}

/*ARGSUSED*/
char *tPrintReal(cptr, tmpPtr, objPtr)
char *cptr;
struct enumTemplate *tmpPtr;
octObject *objPtr;
{
    double number = * (double *) cptr;
    static char string[40];

    (void) sprintf(string, "%g", number);
    return(string);
}

/*ARGSUSED*/
char *tPrintID(cptr, tmpPtr, objPtr)
char *cptr;
struct enumTemplate *tmpPtr;
octObject *objPtr;
{
    octObject inst;

    inst.objectId = * (octId *) cptr;
    if (octIdIsNull(inst.objectId)) {
	return("oct_null_id");
    } else {
	OCT_ASSERT(octGetById(&inst), "%s");
	printObjectSummary(&inst);
	return("");		/* XXX */
    }
}

/*ARGSUSED*/
char *tPrintXid(cptr, tmpPtr, objPtr)
char *cptr;
struct enumTemplate *tmpPtr;
octObject *objPtr;
{
    static char string[20];
    int32 xid;

    OCT_VPROTECT(octExternalId(objPtr, &xid));
    if (DID_OCT_FAIL()) {
	return("???  ");
    } else {
	(void) sprintf(string, "%5ld", (long) xid);
	return(string);
    }
}

/*	Special functions for the label justification fields		*/
/*		(bit fields have no addresses)				*/

#define JUST_PRINT(NAME, FIELD)	\
char *NAME(cptr, tmpPtr, objPtr)					\
char *cptr;								\
struct enumTemplate *tmpPtr;						\
octObject *objPtr;							\
{									\
    octObject *labelPtr = (octObject *) cptr;				\
    int just = labelPtr->contents.label.FIELD;				\
    char *tPrintEnum();							\
									\
    return(tPrintEnum((char *) &just, tmpPtr, objPtr));			\
}

JUST_PRINT(tPrintVJust, vertJust)

JUST_PRINT(tPrintHJust, horizJust)

JUST_PRINT(tPrintLJust, lineJust)

/*ARGSUSED*/
char *tPrintEnum(cptr, tmpPtr, objPtr)
char *cptr;
struct enumTemplate *tmpPtr;
octObject *objPtr;
{
    int enumVal = * (int *) cptr;;

    if (tmpPtr == NIL(struct enumTemplate)) {
	errRaise(optProgName, 0, "internal error -- bad enum template");
    }

    while (tmpPtr->name && tmpPtr->value != enumVal) tmpPtr++;

    if (tmpPtr->name == NIL(char)) return("");

    recursionTemplate = tmpPtr->subFields;
    return(tmpPtr->name);
}

/*ARGSUSED*/
char *tPrintLayer(cptr, tmpPtr, objPtr)
char *cptr;
struct enumTemplate *tmpPtr;
octObject *objPtr;
{
    octObject *geoPtr = (octObject *) cptr;
    octObject layer;
    octGenerator gen;

    OCT_PROTECT(octInitGenContainers(geoPtr, OCT_LAYER_MASK, &gen));
    if (DID_OCT_FAIL() || octGenerate(&gen, &layer) != OCT_OK) return("");

    if (octGenerate(&gen, &layer) == OCT_GEN_DONE) {
	return(layer.contents.layer.name);
    } else {
	OCT_ASSERT(octFreeGenerator(&gen), "%s");
	return(" >1 ");
    }
}

/*ARGSUSED*/
char *tPrintBB(cptr, tmpPtr, objPtr)
char *cptr;
struct enumTemplate *tmpPtr;
octObject *objPtr;
{
    octObject *geoPtr = (octObject *) cptr;
    static char string[50];
    struct octBox box;

    OCT_PROTECT(octBB(geoPtr, &box));
    if (DID_OCT_FAIL()) {
	(void) sprintf(string, "%24s", "");
    } else {
	(void) sprintf(string, "%5ld,%-5ldBB%5ld,%-5ld",
		    (long) box.lowerLeft.x, (long) box.lowerLeft.y,
		    (long) box.upperRight.x, (long) box.upperRight.y);
    }
    return(string);
}

/*
 *	tPrintConnWidth
 *
 *	Prints the width of a connectivity object (net or term) in the
 *  same size space as used by tPrintBB (24 characters)
 */
/*ARGSUSED*/
char *tPrintConnWidth(cptr, tmpPtr, objPtr)
char *cptr;
struct enumTemplate *tmpPtr;
octObject *objPtr;
{
    int32 number = * (int32 *) cptr;
    static char string[50];

    if (number == 1) {
	(void) sprintf(string, "%24s", "");
    } else {
	(void) sprintf(string, "%10s<%ld>%11s", "", (long) number, "");
    }
    string[24] = '\0';
    return(string);
}

/*ARGSUSED*/
char *tPrintNPoints(cptr, tmpPtr, objPtr)
char *cptr;
struct enumTemplate *tmpPtr;
octObject *objPtr;
{
    octObject *geoPtr = (octObject *) cptr;
    int32 npoints = 0;
    static char string[50];

    /*
     * XXX
     * should check return code, but then newly created paths and polygons
     * must be entered into oct before they are edited (but instances
     * better not be)
     */
    OCT_PROTECT(octGetPoints(geoPtr, &npoints, NIL(struct octPoint)));
    (void) sprintf(string, "%ld", (long)npoints);
    return(string);
}

/*ARGSUSED*/
char *tPrintNGeos(cptr, tmpPtr, objPtr)
char *cptr;
struct enumTemplate *tmpPtr;
octObject *objPtr;
{
    octObject *lyrPtr = (octObject *) cptr;
    int ngeos = 0;
    static char string[50];
    octGenerator gen;
    octStatus status;
    octObject dummyObject;

    OCT_PROTECT(octInitGenContents(lyrPtr, OCT_GEO_MASK, &gen));
    if (DID_OCT_FAIL()) return("");

    while ((status = octGenerate(&gen, &dummyObject)) == OCT_OK) {
	ngeos++;
    }
    OCT_ASSERT(status, "%s");
    (void) sprintf(string, "%d", ngeos);
    return(string);
}

/*ARGSUSED*/
char *tPrintNObjs(cptr, tmpPtr, objPtr)
char *cptr;
struct enumTemplate *tmpPtr;
octObject *objPtr;
{
    octObject *bagPtr = (octObject *) cptr;
    int nobjs = 0;
    static char string[50];
    octGenerator gen;
    octStatus status;
    octObject dummyObject;

    OCT_PROTECT(octInitGenContents(bagPtr, OCT_ALL_MASK, &gen));
    if (DID_OCT_FAIL()) return("?");

    while ((status = octGenerate(&gen, &dummyObject)) == OCT_OK) {
	nobjs++;
    }
    OCT_ASSERT(status, "%s");
    (void) sprintf(string, "%d", nobjs);
    return(string);
}

/*ARGSUSED*/
int tGetString(cptr, tmpPtr)
char *cptr;
struct enumTemplate *tmpPtr;
{
    char *strPtr = * (char **) cptr;
    int stop;

    stop = getString(tCurString, tCurString + strlen(tCurString), TEMP_STOPS);
    if (strcmp(tCurString, strPtr)) {
	* (char **) cptr = strPtr = ALLOC(char, strlen(tCurString)+1);
	(void) strcpy(strPtr, tCurString);
    }
    return(stop);
}

/*ARGSUSED*/
int tGetInteger(cptr, tmpPtr)
char *cptr;
struct enumTemplate *tmpPtr;
{
    int32 *numberPtr = (int32 *) cptr;
    int stop;
    long atol();

    stop = getString(tCurString, tCurString + strlen(tCurString), TEMP_STOPS);
    *numberPtr = atol(tCurString);
    return(stop);
}

/*ARGSUSED*/
int tGetCoord(cptr, tmpPtr)
char *cptr;
struct enumTemplate *tmpPtr;
{
    octCoord *numberPtr = (octCoord *) cptr;
    int stop;
    long atol();

    stop = getString(tCurString, tCurString + strlen(tCurString), TEMP_STOPS);
    *numberPtr = atol(tCurString);
    return(stop);
}

/*ARGSUSED*/
int tGetReal(cptr, tmpPtr)
char *cptr;
struct enumTemplate *tmpPtr;
{
    double *numberPtr = (double *) cptr;
    int stop;
    double atof();

    stop = getString(tCurString, tCurString + strlen(tCurString), TEMP_STOPS);
    *numberPtr = atof(tCurString);
    return(stop);
}

/*ARGSUSED*/
int tGetID(cptr, tmpPtr)
char *cptr;
struct enumTemplate *tmpPtr;
{
    octId *idPtr = (octId *) cptr;
    int stop;
    char *dummy;

    stop = getString(tCurString, tCurString + strlen(tCurString), TEMP_STOPS);

    if (strcmp(tCurString, ".") == 0) {
	*idPtr = currentState->currentObject.objectId;
    } else if (strcmp(tCurString, "/") == 0) {
	*idPtr = currentState->currentFacet.objectId;
    } else if (		strcmp(tCurString, "oct_null_id") == 0 ||
			strcmp(tCurString, "null_id") == 0 ||
			strcmp(tCurString, "0") == 0) {
	*idPtr = oct_null_id;
    } else {
	if (st_lookup(nameTable, tCurString, &dummy) == 0) {
	    errRaise(optProgName, 0, "unknown name");
	}
	*idPtr = (octId) dummy;
    }
    return(stop);
}

/*	Special functions for the label justification fields		*/
/*		(bit fields have no addresses)				*/

#define JUST_GET(NAME, FIELD)	\
int NAME(cptr, tmpPtr)							\
char *cptr;								\
struct enumTemplate *tmpPtr;						\
{									\
    octObject *labelPtr = (octObject *) cptr;				\
    int just = labelPtr->contents.label.FIELD;				\
    int stop;								\
    int tGetEnum();							\
									\
    stop = tGetEnum((char *) &just, tmpPtr);				\
    labelPtr->contents.label.FIELD = just;				\
    return(stop);							\
}

JUST_GET(tGetVJust, vertJust)

JUST_GET(tGetHJust, horizJust)

JUST_GET(tGetLJust, lineJust)

#define NO_MATCH	0
#define PART_MATCH	1
#define EXACT_MATCH	2

int tGetEnum(cptr, tmpPtr)
char *cptr;
struct enumTemplate *tmpPtr;
{
    int *enumPtr = (int *) cptr;
    int stop;
    char oldString[100];
    char *newString;
    int nMatch = 0;
    struct enumTemplate *matchTmpPtr = (struct enumTemplate *)NULL;

    *oldString = '\0';
    (void) strncat(oldString, tCurString, 99);
    stop = getString(tCurString, tCurString + strlen(tCurString), TEMP_STOPS);
    strUpcase(tCurString);
    if (strncmp(oldString, tCurString, strlen(oldString)) == 0 &&
			    strlen(tCurString) > strlen(oldString)) {
	newString = tCurString + strlen(oldString);
    } else {
	newString = tCurString;
    }
    while (tmpPtr->name) {
	switch (MAX(matchString(tmpPtr->name, tCurString),
		    matchString(tmpPtr->name, newString))) {
	    case EXACT_MATCH:
		matchTmpPtr = tmpPtr;
		goto match;
	    case PART_MATCH:
		nMatch++;
		matchTmpPtr = tmpPtr;
	}
	tmpPtr++;
    }

    if (nMatch < 1) errRaise(optProgName, 0, "Unknown named value");
    if (nMatch > 1) errRaise(optProgName, 0, "Ambiguous named value");

match:
    if (*enumPtr != matchTmpPtr->value) {
	if (matchTmpPtr->initFn) (*matchTmpPtr->initFn)(tCurObjPtr);
	*enumPtr = matchTmpPtr->value;
    }
    return(stop);
}

static int matchString(str, testStr)
register char *str;
register char *testStr;
{
    while (*testStr) {
	if (*str++ != *testStr++) return(NO_MATCH);
    }
    return(*str ? PART_MATCH : EXACT_MATCH);
}
