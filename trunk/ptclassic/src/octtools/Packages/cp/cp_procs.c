#ifndef lint
static char SccsId[]="$Id$";
#endif /*lint*/
/* Copyright (c) 1990-1994 The Regents of the University of California.
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
#include "copyright.h"
#include "port.h"
#include "tr.h"
#include "errtrap.h"
#include "utility.h"
#include "cp_internal.h"
#include "cp_procs.h"

#define VAL(char) ( (int) ( char - '0'))

extern tr_stack *cp_stack;

int
cp_get_path (number, path)
int *number;
cp_path **path;
{
    cp_path *ptr, *tail, *head;
    int count = 0;

    tail = head = ptr = ALLOC(cp_path, 1);
    
    for(;;) {
	if (!cp_get_point(&ptr->point)) {
	    FREE(ptr);
	    break;
	}
	count++;
	tail = tail->next = ptr;
	ptr = ALLOC(cp_path, 1);
    }
    
    if (count == 0) {
	return 0;
    }

    *path = head;
    *number = count;
    tail->next = NIL(cp_path);

    return 1;
}

int
cp_get_point (point)
cp_point *point;
{
    int temp;

    flush(SEP);

    /* point->x and point->y is a int32, which could be a long, and
       cp_get_sinteger takes a *int arg, so we use a temp variable */
    if (!cp_get_sinteger(&temp))
	return 0;
    point->x=temp;

    if (!cp_get_sinteger(&temp))
	error ("missing second coordinate in point");
    point->y=temp

    return 1;
}

int
cp_get_sinteger(ret_val)
int *ret_val;
{
    int     sign = 1;
    int     val;

    flush (SEP);

    if (!IS (DIGIT, CP_GET_CHAR)) {
	if (current_char == '-') {
	    sign = -1;
	    CP_GET_CHAR;
	}
	else {
	    CP_UNGET_CHAR(current_char, infile);
	    return 0;
	}
    }

    val = VAL (current_char);
    while (IS (DIGIT, CP_GET_CHAR)) {
	if (MAX_INT / 10 - VAL (current_char) < val) {
	    error ("integer out of range");
	    break;
	}

	val = 10 * val + VAL (current_char);
    }

    CP_UNGET_CHAR(current_char, infile);
    *ret_val = val*sign;
    return 1;
}

int
cp_get_integer(ret_val)
int *ret_val;
{
    int     val;

    flush(SEP);

    if (!IS (DIGIT, CP_GET_CHAR)) {
	CP_UNGET_CHAR(current_char, infile);
	return 0;
    }

    val = VAL (current_char);
    while (IS (DIGIT, CP_GET_CHAR)) {
	if (MAX_INT / 10 - VAL (current_char) < val) {
	    error ("integer out of range");
	    break;
	}

	val = 10 * val + VAL (current_char);
    }

    CP_UNGET_CHAR(current_char, infile);
    *ret_val = val;
    return 1;
}
int
cp_get_shortname (ret_ptr)
char **ret_ptr;
{
    int     num = 0;
    char   *name;

    name = malloc(5);

    while (IS(DIGIT|UPPER_CHAR, CP_GET_CHAR)) {
	if (num > 3) {
	    error ("Layer name is too long");
	}
	name[num++] = current_char;
    }

    if (num == 0) {
	return 0;
    }

    name[num] = '\0';

    CP_UNGET_CHAR(current_char, infile);
    *ret_ptr = name;
    return 1;
}

int
cp_get_transterm()
{
    cp_point pnt;

    flush (BLANK);

    switch (CP_GET_CHAR) {

	case 'T': 
	    if (!cp_get_point(&pnt)) {
		error (" point missing after translation ");
	    }
	    tr_translate(cp_stack, (int32) pnt.x, (int32) pnt.y, 1);
	    break;

	case 'R': 
	    if (!cp_get_point(&pnt)) {
		error (" point missing after rotation ");
	    }
	    tr_rotate_dir(cp_stack, (int32) pnt.x, (int32) pnt.y, 1);
	    break;

	case 'M':
	    flush(BLANK);
	    switch (CP_GET_CHAR) {

		case 'X': 
		    tr_do_op(cp_stack, TR_MIRROR_X, 1);
		    break;

		case 'Y': 
		    tr_do_op(cp_stack, TR_MIRROR_Y, 1);
		    break;

		default: 
		    error ("Bad Mirror option in transformation");
		    break;
	    }
	    break;

	default: 
	    CP_UNGET_CHAR(current_char, infile);
	    return 0;
    }

    return 1;
}

void
cp_get_transform(array, x, y, type)
double array[2][2];
int32 *x, *y;
int *type;
{
    while (cp_get_transterm());
    tr_get_transform(cp_stack, array, x, y, type);
}
void
flush (type)
type_type type;
{
    while (IS (type, CP_GET_CHAR)) {
    }

    CP_UNGET_CHAR(current_char, infile);
}

void
error (str)
char   *str;
{
    errRaise(CP_PKG_NAME, -1,
	     "error at line %d : %s",
	     cp_line_number, str);
}
