/* Version Identification:
 * $Id$
 */
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
static scan_point *boxer_scan(pointer, x)
register scan_point *pointer;
register fa_coord x;
{

    while ( pointer->next->x <= x ) {
	    pointer = pointer->next;
    }

    return(pointer);
}

static void boxer_check_emit(outbox, pointer, vertex)
fa_box **outbox;
scan_point *pointer;
fa_vertex *vertex;
{

    switch (vertex->type) {

	case UPPER_LEFT_NEG : 
	    if (pointer->y != vertex->y) {
		put_box( outbox, pointer->last->x,
				 pointer->y,
				 vertex->x,
				 vertex->y );
		
		pointer->last->y = pointer->y = vertex->y;
	    }
	    break;

	case UPPER_RIGHT_NEG : 
	case UPPER_LEFT_POS :
	case LEFT_DIAGONAL :
	    if (pointer->y != vertex->y) {
		put_box( outbox, vertex->x,
				 pointer->y,
				 pointer->next->x,
				 vertex->y );
		
		pointer->next->y = pointer->y = vertex->y;
	    }
	    break;

	case LOWER_LEFT_NEG : 	
	    if (pointer->next->y != vertex->y) {
		put_box( outbox, pointer->last->x,
				pointer->next->y,
				pointer->next->x,
				vertex->y );
		
		pointer->next->y = 
		    pointer->last->y = pointer->y = vertex->y;
	    }
	    break;
	  case NO_VERTEX:
	  case LOWER_RIGHT_POS:
	  case UPPER_RIGHT_POS:
	  case LOWER_LEFT_POS:
	  case RIGHT_DIAGONAL:
	    break;
	  default:
	    break;

    }
}

static void put_box( outbox, left, bottom, right, top)
fa_box **outbox;
fa_coord left;
fa_coord bottom;
fa_coord right;
fa_coord top;
{
    fa_box *ptr;

    if (left == FA_INFINITY || bottom == FA_INFINITY ||
        left == -FA_INFINITY || bottom == -FA_INFINITY ) {
	fa_debug_func(left,bottom);
    }

    if (right == FA_INFINITY || top == FA_INFINITY ||
        right == -FA_INFINITY || top == -FA_INFINITY ) {
	fa_debug_func(right,top);
    }

    ptr = ALLOC(fa_box, 1);

    ptr->left = left;
    ptr->bottom = bottom;
    ptr->right = right;
    ptr->top = top;

    ptr->next = *outbox;
    *outbox = ptr;
}

