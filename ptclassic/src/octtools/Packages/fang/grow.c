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
#include "port.h"
#include "utility.h"
#include "fang.h"
#include "fa_internal.h"

/*LINTLIBRARY*/

extern fa_vertex_type fa_inverse_type[];

#define use_type(type) \
    ( shrink ? fa_inverse_type[(int)(type)] : (type) )

#define output(geo, x, y, type) fa_put_vertex(geo, x, y, use_type(type))

void fa_grow(geo, amount, outgeo)
fa_geometry *geo;
int amount;
fa_geometry *outgeo;
{
    int shrink = 0;
    fa_vertex_type type;
    fa_vertex *ptr;

    fa_init(outgeo);

    if (amount < 0) {
	amount = -amount;
	shrink = 1;
	fa_merge1(geo);
    }

    for (ptr = geo->head; ptr != NIL(fa_vertex); ptr = ptr->next) {

	type = use_type(ptr->type);

	switch (type) {

	    case UPPER_LEFT_POS : 
		output(outgeo, ptr->x - amount,
		    ptr->y + amount, type);
		break;

	    case LOWER_RIGHT_POS : 
		output(outgeo, ptr->x + amount,
		    ptr->y - amount, type);
		break;

	    case UPPER_RIGHT_POS : 
		output(outgeo, ptr->x + amount,
		    ptr->y + amount, type);
		break;

	    case LOWER_LEFT_POS : 
		output(outgeo, ptr->x - amount,
		    ptr->y - amount, type);
		break;

	    case UPPER_RIGHT_NEG : 
		output(outgeo, ptr->x, ptr->y, UPPER_RIGHT_POS);
		output(outgeo, ptr->x - amount, ptr->y,
		    UPPER_LEFT_POS);
		output(outgeo, ptr->x, ptr->y - amount,
		    LOWER_RIGHT_POS);
		break;

	    case LOWER_LEFT_NEG : 
		output(outgeo, ptr->x, ptr->y, LOWER_LEFT_POS);
		output(outgeo, ptr->x + amount, ptr->y,
		    LOWER_RIGHT_POS);
		output(outgeo, ptr->x, ptr->y + amount,
		    UPPER_LEFT_POS);
		break;

	    case UPPER_LEFT_NEG : 
		output(outgeo, ptr->x, ptr->y, UPPER_LEFT_POS);
		output(outgeo, ptr->x + amount, ptr->y,
		    UPPER_RIGHT_POS);
		output(outgeo, ptr->x, ptr->y - amount,
		    LOWER_LEFT_POS);
		break;

	    case LOWER_RIGHT_NEG : 
		output(outgeo, ptr->x, ptr->y, LOWER_RIGHT_POS);
		output(outgeo, ptr->x - amount, ptr->y,
		    LOWER_LEFT_POS);
		output(outgeo, ptr->x, ptr->y + amount,
		    UPPER_RIGHT_POS);
		break;


	    case NO_VERTEX : 
	    case LEFT_DIAGONAL : 
	    case RIGHT_DIAGONAL : 
	    default :
		fa_error("Illegal edge type found in fa_grow");
	}
    }

    if (shrink) {
	fa_merge1(outgeo);
    }
}

void fa_grow_xy(geo, amount, direction, outgeo)
fa_geometry *geo;
int amount;
int direction;               /*   FA_VERTICAL or FA_HORIZONTAL  */
fa_geometry *outgeo;
/*
 *  This function is similar to fa_grow, except that it grows geos
 *  only in one direction, the one specified by the <direction>
 *  parameter.
 *  Enrico Malavasi  -  August 1991
 */
{
    int shrink = 0;
    int shrink_x, shrink_y;
    int amount_x, amount_y;

    fa_vertex_type type;
    fa_vertex *ptr;    

    if((direction != FA_VERTICAL) && (direction != FA_HORIZONTAL))
        fa_error("Illegal direction for fa_grow_xy");

    fa_init(outgeo);

    if (amount < 0) {
	amount = -amount;
	shrink = 1;
	fa_merge1(geo);
    }
    amount_x = (direction == FA_VERTICAL) ? 0 : amount;
    amount_y = (direction == FA_VERTICAL) ? amount : 0;

    shrink_x = (direction == FA_VERTICAL) ? 0 : shrink;
    shrink_y = (direction == FA_VERTICAL) ? shrink : 0;

    for (ptr = geo->head; ptr != NIL(fa_vertex); ptr = ptr->next) {
	
	type = use_type(ptr->type);

	switch (type) {
	    
	  case UPPER_LEFT_POS : 
	    output(outgeo, ptr->x - amount_x, ptr->y + amount_y, type);
	    break;

	  case LOWER_RIGHT_POS : 
	    output(outgeo, ptr->x + amount_x, ptr->y - amount_y, type);
	    break;

	  case UPPER_RIGHT_POS : 
	    output(outgeo, ptr->x + amount_x, ptr->y + amount_y, type);
	    break;
	    
	  case LOWER_LEFT_POS : 
	    output(outgeo, ptr->x - amount_x, ptr->y - amount_y, type);
	    break;
	    
	  case UPPER_RIGHT_NEG : 
	    output(outgeo, ptr->x, ptr->y, UPPER_RIGHT_POS);
	    output(outgeo, ptr->x - amount_x, ptr->y, UPPER_LEFT_POS);
	    output(outgeo, ptr->x, ptr->y - amount_y, LOWER_RIGHT_POS);
	    break;

	  case LOWER_LEFT_NEG : 
	    output(outgeo, ptr->x, ptr->y, LOWER_LEFT_POS);
	    output(outgeo, ptr->x + amount_x, ptr->y, LOWER_RIGHT_POS);
	    output(outgeo, ptr->x, ptr->y + amount_y, UPPER_LEFT_POS);
	    break;

	  case UPPER_LEFT_NEG : 
	    output(outgeo, ptr->x, ptr->y, UPPER_LEFT_POS);
	    output(outgeo, ptr->x + amount_x, ptr->y, UPPER_RIGHT_POS);
	    output(outgeo, ptr->x, ptr->y - amount_y, LOWER_LEFT_POS);
	    break;

	  case LOWER_RIGHT_NEG : 
	    output(outgeo, ptr->x, ptr->y, LOWER_RIGHT_POS);
	    output(outgeo, ptr->x - amount_x, ptr->y, LOWER_LEFT_POS);
	    output(outgeo, ptr->x, ptr->y + amount_y, UPPER_RIGHT_POS);
	    break;


	  case NO_VERTEX : 
	  case LEFT_DIAGONAL : 
	  case RIGHT_DIAGONAL : 
	    default :
	      fa_error("Illegal edge type found in fa_grow");
	}
    }

    if (shrink) {
	fa_merge1(outgeo);
    }
}
