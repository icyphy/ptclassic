/* $Id$ */

/*
 * Revision Control Information
 *
 * $Source$
 * $Author$
 * $Revision$
 * $Date$
 *
 */
/* LINTLIBRARY */

#include <stdio.h>
#include "util.h"

int array_i;		/* global for macro's */

#define INIT_SIZE	3


array_t *
array_do_alloc(size, number)
int size;
int number;
{
    array_t *array;

    array = ALLOC(array_t, 1);
    array->num = 0;
    array->n_size = MAX(number, INIT_SIZE);
    array->obj_size = size;
    array->space = ALLOC(char, array->n_size * array->obj_size);
    (void) memset(array->space, 0, array->n_size * array->obj_size);
    return array;
}


void
array_free(array)
array_t *array;
{
    FREE(array->space);
    FREE(array);
}


array_t *
array_dup(old)
array_t *old;
{
    array_t *new;

    new = ALLOC(array_t, 1);
    new->num = old->num;
    new->n_size = old->num;
    new->obj_size = old->obj_size;
    new->space = ALLOC(char, new->n_size * new->obj_size);
    (void) memcpy(new->space, old->space, old->num * old->obj_size);
    return new;
}

/* append the elements of array2 to the end of array1 */
void
array_append(array1, array2)
array_t *array1;
array_t *array2;
{
    char *pos;

    if (array1->obj_size != array2->obj_size) {
	(void) array_abort(2);
	/* NOTREACHED */
    }

    /* make sure array1 has enough room */
    if (array1->n_size < array1->num + array2->num) {
	(void) array_resize(array1, array1->num + array2->num);
    }

    pos = array1->space + array1->num * array1->obj_size;
    (void) memcpy(pos, array2->space, array2->num * array2->obj_size);
    array1->num += array2->num;
}


/* join array1 and array2, returning a new array */
array_t *
array_join(array1, array2)
array_t *array1;
array_t *array2;
{
    array_t *array;
    char *pos;

    if (array1->obj_size != array2->obj_size) {
	(void) array_abort(3);
	fail("array: join not defined for arrays of different sizes\n");
	/* NOTREACHED */
    }

    array = ALLOC(array_t, 1);
    array->num = array1->num + array2->num;
    array->n_size = array->num;
    array->obj_size = array1->obj_size;
    array->space = ALLOC(char, array->n_size * array->obj_size);
    (void) memcpy(array->space, array1->space, array1->num * array1->obj_size);
    pos = array->space + array1->num * array1->obj_size;
    (void) memcpy(pos, array2->space, array2->num * array2->obj_size);
    return array;
}

char *
array_do_data(array)
array_t *array;
{
    char *data;

    data = ALLOC(char, array->num * array->obj_size);
    (void) memcpy(data, array->space, array->num * array->obj_size);
    return data;
}


int			/* would like to be void, except for macro's */
array_resize(array, new_size)
array_t *array;
int new_size;
{
    int old_size;
    char *pos;

    old_size = array->n_size;
    array->n_size = MAX(array->n_size * 2, new_size);
    array->space = REALLOC(char, array->space, array->n_size * array->obj_size);
    pos = array->space + old_size * array->obj_size;
    (void) memset(pos, 0, (array->n_size - old_size)*array->obj_size);
}

void
array_sort(array, compare)
array_t *array;
int (*compare)();
{
    qsort(array->space, array->num, array->obj_size, compare);
}


void
array_uniq(array, compare, free_func)
array_t *array;
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
	if ((*compare)((char **) obj1, (char **) obj2) != 0) {
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

int			/* would like to be void, except for macro's */
array_abort(i)
int i;
{
    switch (i) {
	case 0: 
	    fail("array: array access out of bounds\n");
	    /* NOTREACHED */
	case 1:
	    fail("array: object size mismatch\n");
	    /* NOTREACHED */
	case 2:
	    fail("array: append not defined for arrays of different sizes\n");
	    /* NOTREACHED */
	case 3:
	    fail("array: join not defined for arrays of different sizes\n");
	    /* NOTREACHED */
	default:
	    fail("array: unknown error\n");
	    /* NOTREACHED */
    }
}
