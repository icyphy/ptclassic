#ifndef lint
static char SccsId[]="$Id$";
#endif /*lint*/
/* Copyright (c) 1990-1993 The Regents of the University of California.
 * All rights reserved.
 * 
 * Permission is hereby granted, without written agreement and without
 * license or royalty fees, to use, copy, modify, and distribute this
 * software and its documentation for any purpose, provided that the above
 * copyright notice and the following two paragraphs appear in all copies
 * of this software.
 * 
 * IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY 
 * FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES 
 * ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF 
 * THE UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF 
 * SUCH DAMAGE.
 * 
 * THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE
 * PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE UNIVERSITY OF
 * CALIFORNIA HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
 * ENHANCEMENTS, OR MODIFICATIONS.
 * 
 */
/* LINTLIBRARY */
/*
 * Dynamic Array Package
 *
 * Richard Rudell
 * Exported by David Harrison
 *
 * This package provides routines for dealing with dynamically
 * sized arrays.  See the documentation in da.doc for details.
 */

#include "copyright.h"
#include "port.h"
#include "utility.h"
#include "da.h"
#include "errtrap.h"

char *da_pkg_name = "da";

/* Standard error strings */
static char da_bad_join[] =
  "cannot join arrays with different sized objects";
static char da_bad_append[] =
  "cannot append arrays with different sized objects";
static char da_low_bound_err[] =
  "array bounds violation: negative offset: %d";
static char da_hi_bound_err[] =
  "array bounds violation: attempt to access object %d (only %d objects in array)";
static char da_neq_size[] =
  "object size mismatch";
static char da_unknown_error[] =
  "unknown array error";

int da_i;		/* global for macro's */

char *da_memptr;		/* Memory allocation return */

#define INIT_SIZE	3



dary da_new(size, number)
int size;			/* Size of each object */
int number;			/* Number of objects   */
/*
 * Returns a newly allocated array of `number' objects each
 * `size' units wide.
 */
{
    dary array;

    array = ALLOC(da_internal, 1);
    array->num = 0;
    array->n_size = MAX(number, INIT_SIZE);
    array->obj_size = size;
    array->space = ALLOC(char, array->n_size * array->obj_size);
    (void) memset(array->space, 0, array->n_size * array->obj_size);
    return array;
}


void daFree(array, func)
dary array;			/* Array to free      */
void (*func)();			/* User free function */
/*
 * Frees an array allocated with daAlloc().  If `func' is provided,
 * it will be called once for each item in the array to allow
 * user allocated items in the array to be freed.
 */
{
    char *ptr;

    if (func) {
	for (ptr = array->space;
	     ptr < (array->space + (array->num * array->obj_size));
	     ptr += array->obj_size)
	  (*func)(ptr);
    }
    FREE(array->space);
    FREE(array);
}



char *da_data(array)
dary array;
/*
 * Returns a locally allocated copy of the data in `array'.
 */
{
    char *data;

    data = ALLOC(char, array->num * array->obj_size);
    (void) memcpy(data, array->space, array->num * array->obj_size);
    return data;
}



dary daDup(old)
dary old;			/* Array to copy */
/*
 * Returns a copy of the array `old'.
 */
{
    da_internal *new;

    new = ALLOC(da_internal, 1);
    new->num = old->num;
    new->n_size = old->num;
    new->obj_size = old->obj_size;
    new->space = ALLOC(char, new->n_size * new->obj_size);
    (void) memcpy(new->space, old->space, old->num * old->obj_size);
    return (dary) new;
}


dary daJoin(array1, array2)
dary array1;		
dary array2;
/*
 * Returns a new array containing the members of array1 followed
 * by the members of array2.
 */
{
    dary array;
    char *pos;

    if (array1->obj_size != array2->obj_size) {
	errRaise(da_pkg_name, DA_BAD_JOIN, da_bad_join);
	/* NOTREACHED */
    }

    array = ALLOC(da_internal, 1);
    array->num = array1->num + array2->num;
    array->n_size = array->num;
    array->obj_size = array1->obj_size;
    array->space = ALLOC(char, array->n_size * array->obj_size);
    (void) memcpy(array->space, array1->space, array1->num * array1->obj_size);
    pos = array->space + array1->num * array1->obj_size;
    (void) memcpy(pos, array2->space, array2->num * array2->obj_size);
    return array;
}



void daAppend(array1, array2)
dary array1;
dary array2;
/*
 * Destructively appends the items of array2 onto array1
 */
{
    char *pos;

    if (array1->obj_size != array2->obj_size) {
	errRaise(da_pkg_name, DA_BAD_APPEND, da_bad_append);
	/* NOTREACHED */
    }

    /* make sure array1 has enough room */
    if (array1->n_size < array1->num + array2->num) {
	(void) da_resize(array1, array1->num + array2->num);
    }

    pos = array1->space + array1->num * array1->obj_size;
    (void) memcpy(pos, array2->space, array2->num * array2->obj_size);
    array1->num += array2->num;

}



int da_resize(array, new_size)
dary array;
int new_size;
/*
 * Resizes the array to `new_size'.
 */
{
    int old_size;
    char *pos;

    old_size = array->n_size;
    array->n_size = MAX(array->n_size * 2, new_size);
    array->space = REALLOC(char, array->space, array->n_size * array->obj_size);
    pos = array->space + old_size * array->obj_size;
    (void) memset(pos, 0, (array->n_size - old_size)*array->obj_size);
    /* Is this right? formerly, this function did not return anything */
    return array->n_size;
}


void daSort(array, compare)
dary array;
int (*compare)();
{
    qsort(array->space, array->num, array->obj_size, compare);
}


void daUniq(array, compare, free_func)
dary array;
int (*compare)();
void (*free_func)();
{
    int i, last;
    char *dest, *obj1, *obj2;

    dest = array->space;
    obj1 = array->space;
    obj2 = array->space + array->obj_size;
    last = array->num;

    for(i = 1; i < last; i++) {
	if ((*compare)((char *) obj1, (char *) obj2) != 0) {
	    if (dest != obj1) {
		(void) memcpy(dest, obj1, array->obj_size);
	    }
	    dest += array->obj_size;
	} else {
	    if (free_func != 0) (*free_func)(obj1);
	    array->num--;
	}
	obj1 += array->obj_size;
	obj2 += array->obj_size;
    }
    if (dest != obj1) {
	(void) memcpy(dest, obj1, array->obj_size);
    }
}



void daAbort(err, idx, limit)
int err;			/* What error          */
int idx;			/* Item being accessed */
int limit;			/* Array size          */
/*
 * This function is called when a serious error has occurred in
 * the dynamic array package.  If the user has set a routine
 * using daError(),  it will be called.  Otherwise,  the 
 * routine prints an error message and aborts the program.
 */
{
    switch (err) {
    case DA_BOUNDS: 
	if (idx < 0) {
	    errRaise(da_pkg_name, DA_LO_BOUND, da_low_bound_err, idx);
	    /* NOTREACHED */
	} else if (idx >= limit) {
	    errRaise(da_pkg_name, DA_HI_BOUND, da_hi_bound_err, idx, limit);
	    /* NOTREACHED */
	} else {
	    errRaise(da_pkg_name, DA_UNKNOWN, da_unknown_error);
	    /* NOTREACHED */
	}
	break;
    case DA_NEQ_SIZE:
	errRaise(da_pkg_name, DA_NEQ_SIZE, da_neq_size);
	/* NOTREACHED */
	break;
    default:
	errRaise(da_pkg_name, DA_UNKNOWN, da_unknown_error);
	/* NOTREACHED */
	break;
    }
    (void) fprintf(stderr,
		   "array: warning -- continuing after serious error.\n");
}
