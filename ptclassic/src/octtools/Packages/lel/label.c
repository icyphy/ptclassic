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
/*LINTLIBRARY*/
/*
 * lelEvalLabel
 *
 * David Harrison
 * University of California, Berkeley
 * 1989
 *
 * This file contains an implementation of a routine for evaluating
 * labels with embedded expressions.
 *
 *
 *  MODIFICATIONS BY MCC:
 *  Copyright (C) 1989 MCC CAD Program
 *
 *  Kevin Gourley
 *
 *  9/14/89  Added lbl argument to pl_eval.
 *
 */

#include "port.h"
#include "oct.h"
#include "oh.h"
#include "utility.h"
#include "lel.h"

#define INTRO_CHAR	'%'

/*
 * Major function exported by lang.c
 */
extern char *pl_eval();

/*
 * Static string for returning results
 */
#define INIT_SIZE	256
static char *lbl_result = (char *) 0;
static long lbl_size = INIT_SIZE;
static long lbl_len;

#define LBL_INIT \
    if (!lbl_result) {				\
	lbl_result = ALLOC(char, lbl_size);	\
    }						\
    lbl_len = 0;				

#define LBL_CAT(nstr, len) \
    if (lbl_len + (len) > lbl_size) {				\
	lbl_size += lbl_size + (len);				\
	lbl_result = REALLOC(char, lbl_result, lbl_size);	\
    }								\
    (void) strncpy(lbl_result + lbl_len, (nstr), (len));	\
    lbl_len += (len);						\
    lbl_result[lbl_len] = '\0'

/* Like LBL_CAT but doesn't modify length and makes `ptr' point to copy */
#define LBL_TMP(nstr, len, ptr) \
    if (lbl_len + (len) > lbl_size) {				\
	lbl_size += lbl_size + (len);				\
	lbl_result = REALLOC(char, lbl_result, lbl_size);	\
    }								\
    (ptr) = strncpy(lbl_result + lbl_len, (nstr), (len));	\
    lbl_result[lbl_len+(len)] = '\0'

#define LBL_RESULT	lbl_result

octStatus lelEvalLabel(inst, lbl, facet)
octObject *inst;		/* Req'd for evaluating props in hierarchy */
octObject *lbl;			/* Label to examine and evaluate           */
char *facet;			/* Master facet for `inst'                 */
/*
 * This routine evaluates the label field of the `label' object
 * as a format string.  Substitutions are performed as specified
 * in substitution specifications embedded in this format string.
 * These specifications are enclosed by percent characters (%).
 * Characters that are not part of a conversion specification are
 * copied into the result string unchanged.
 *
 * The substitution specification consists of an expression in postfix
 * form.  The expression is evaluated and the result is substituted
 * into the original string in place of the expression.  The expression
 * itself consiss of constants and operators.  Constants are pushed
 * onto a stack.  Operators remove items form the stack as arguments
 * and push results back onto the stack.  Three constant specifications
 * are allowed: integer, real, and string.  Strings must be enclosed
 * double quote characters.  A short description of supported operators
 * is given below:
 *   [int] name => [str]	Finds name of oct object given XId
 *   [int] value => [item]	Finds value of oct object given XId
 *   [item] [item] + => [item]	Adds items
 *   [item] [item] - => [item]	Subtracts items
 *   [item] [item] * => [item]	Multiplies items
 *   [item] [item] / => [item]	Divides items
 *   [real] [int] [int] sci => [str] Converts number into scientific notation
 *   [real] [int] [int] eng => [str] Converts number into engineering notation
 *   lambda => [int]		Returns number of Oct units per lambda
 *   coord => [real]		Returns number of meters per oct unit
 *
 * The object used as the source for a substitution is obtained
 * using octGetByExternalId.  The facet containing `label' is
 * used as the facet argument and the external identifier given
 * in the format specification is used as the external identifier
 * parameter.  If the result object is a property,  lelFindProp is
 * used to resolve the value of the property and `facet' is
 * passed through as the master facet specification parameter.
 * The resulting string is placed in the label.  However,  this
 * string is internal to the package and should be copied before
 * another call to lelEvalLabel is made.
 */
{
    register char *orig = lbl->contents.label.label;
    register char *idx;
    octObject fct;
    char *expr, *result;
    int len;

    /*
     * The first loop is unraveled for efficiency and initialization
     */

    if ((!orig) || (!(idx = strchr(orig, INTRO_CHAR)))) return OCT_OK;
    LBL_INIT;
    len = idx - orig;
    LBL_CAT(orig, len);
    /* Move over percent sign */
    orig = idx+1;
    /* 
     * Find opposing percent sign -- this has problems if percent
     * signs are embedded in the expression.
     */
    idx = strchr(orig, INTRO_CHAR);
    if (idx) {
	/* Found closing percent - evaluate expression */
	len = idx - orig;
	LBL_TMP(orig, len, expr);
	if (expr[0]) {
	    octGetFacet(lbl, &fct);
	    /* MCC added lbl arg to pl_eval */
	    result = pl_eval(&fct, inst, lbl, facet, expr);
	    len = strlen(result);
	    LBL_CAT(result, len);
	} else {
	    LBL_CAT("%", 1);
	}
	orig = idx+1;
    } else {
	/* No closing percent - assume regular percent */
	LBL_CAT("%", 1);
	len = strlen(orig);
	LBL_CAT(orig, len);
	orig = (char *) 0;
    }
    /* Now the loop (assumes its outside an expression) */
    while (orig) {
	idx = strchr(orig, INTRO_CHAR);
	if (idx) {
	    /* Expression */
	    len = idx - orig;
	    LBL_CAT(orig, len);
	    orig = idx+1;
	    /* Opposite */
	    idx = strchr(orig, INTRO_CHAR);
	    if (idx) {
		len = idx - orig;
		LBL_TMP(orig, len, expr);
		if (expr[0]) {
		    /* MCC added lbl arg to pl_eval */
		    result = pl_eval(&fct, inst, lbl, facet, expr);
		    len = strlen(result);
		    LBL_CAT(result, len);
		} else {
		    LBL_CAT("%", 1);
		}
		orig = idx+1;
	    } else {
		LBL_CAT("%", 1);
		len = strlen(orig);
		LBL_CAT(orig, len);
		orig = (char *) 0;
	    }
	} else {
	    /* No expression - copy and break */
	    len = strlen(orig);
	    LBL_CAT(orig, len);
	    orig = (char *) 0;
	}
    }
    lbl->contents.label.label = LBL_RESULT;
    return OCT_OK;
}
