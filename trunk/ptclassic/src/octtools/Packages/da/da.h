/* Version Identification:
 * $Id$
 */
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
#ifndef DA_H
#define DA_H

/*
 * Definitions for Dynamic Array Package
 *
 * Richard Rudell
 * 1988
 *
 * Modified for export to the world by David Harrison.
 */

#include "ansi.h"
#include "copyright.h"

/*
 * This type should not be manipulated directly.
 */
typedef struct da_struct {
    char *space;		/* space for items */
    int num;			/* number of array elements */
    int n_size;			/* size of 'data' array (in objects) */
    int obj_size;		/* size of each array object */
} da_internal;

typedef struct da_struct *dary;

/*
 * Serious abort errors
 */

#define DA_BAD_JOIN	1	/* attempt to join incompatible arrays   */
#define DA_BAD_APPEND	2	/* attempt to append incompatible arrays */
#define DA_LO_BOUND	3	/* bounds violation: negative offset 	 */
#define DA_HI_BOUND	4	/* bounds violation: access beyond end 	 */
#define DA_NEQ_SIZE	5	/* object size mismatch 		 */
#define DA_UNKNOWN	99	/* unknown error 			 */

#define DA_BOUNDS	10	/* internal error */

extern int da_i;
extern void daAbort
  ARGS((int err, int idx, int limit));
extern dary da_new
  ARGS((int size, int number));
extern int da_resize
  ARGS((dary array, int new_size));
extern char *da_data
  ARGS((dary array));

#define daAlloc(type, number) \
    da_new(sizeof(type), number)
  /* Creates a new dynamic array */

extern void daFree
  ARGS((dary array, void (*func)()));
  /* Destroys an existing dynamic array */

#define daReset(array) \
    (array)->num = 0
  /* Resets an array for reuse */

#define daResize(array, number) \
    (da_i = (number), \
      da_i < 0 ? (daAbort(DA_BOUNDS, da_i, (array)->n_size), 0) : 0, \
      da_i > (array)->n_size ? da_resize(array, da_i) : 0,\
      (array)->num = da_i)
  /* Changes size of array */

#define daLen(array) \
    (array)->num
  /* Returns number of elements in array */

#define daData(type, array) \
    (type *) array->space 

#define daCopyData(type, array) \
    (type *) da_data(array)
  /* Returns a copy of the data from array */

#define daSet(type, a, i) \
    (da_i = (i), \
      da_i < 0 ? (daAbort(DA_BOUNDS, da_i, (a)->n_size), 0) : 0, \
      sizeof(type) != (a)->obj_size ? (daAbort(DA_NEQ_SIZE, 0, 0), 0) : 0,\
      da_i >= (a)->n_size ? da_resize(a, da_i + 1) : 0,\
      da_i >= (a)->num ? (a)->num = da_i + 1 : 0,\
      ((type *) ((a)->space + da_i * (a)->obj_size)))
  /* Returns a pointer to element `i' of `a' suitable for assignment */

#define daSetLast(type, array)	\
    daSet(type, array, (array)->num)
  /* Returns a pointer to the last element of `array' suitable for assignment */

#define daGet(type, a, i) \
    (da_i = (i), \
      (da_i < 0 || da_i >= (a)->num) ? (daAbort(DA_BOUNDS, da_i, (a)->num), 0) : 0,\
      ((type *) ((a)->space + da_i * (a)->obj_size)))

#define daGetLast(type, array)		\
    daGet(type, array, ((array)->num-1))

extern dary daDup
  ARGS((dary old));
  /* Returns a copy of the specified dynamic array */

extern dary daJoin
  ARGS((dary array1, dary array2));
  /* Non-destructively joins two arrays */

extern void daAppend
  ARGS((dary array1, dary array2));
  /* Destructively appends elements onto an array */

extern void daSort
  ARGS((dary array, int (*compare)()));
  /* Sorts elements of an array */

extern void daUniq
  ARGS((dary array, int (*compare)(), void (*free_func)()));
  /* Destructively removes redundant items from array */

#endif /* DA_H */
