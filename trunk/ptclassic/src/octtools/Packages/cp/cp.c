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
#include "utility.h"
#include "cp_internal.h"
#include "cp_procs.h"
#include "tr.h"

#define TEXT_BUFFER_INIT_SIZE 120        /* (power of two) - SLOP */
#define SLOP 8

static boolean in_definition = FALSE;
tr_stack *cp_stack;

/* Who knows what this function does, it has lots of undefined calls
 * in it, which we declare below
 */ 
extern void cp_call
	ARGS((int cell_num, double (*array)[2], int 32 x, int32 y, int type));
extern void cp_defdelete
	ARGS((int cell_num));
extern void cp_deffinish();
extern void cp_defstart
        ARGS((int cell_num, int a, int b));
extern void cp_layer
	ARGS((char *name));
extern void cp_userextension
	ARGS((int user_num, char *result));
extern void cp_end();
extern void cp_polygon
	ARGS((int number, struct cp_path *path));
extern void cp_box
	ARGS((int length, int width, cp_point center, cp_point dir));
extern void cp_roundflash
	ARGS((int diameter, cp_point center));
extern void cp_wire
	ARGS((int width, cp_path number, int path));

void
cp_parse(input_file)
FILE * input_file;
{
    char   *text_string;
    int     buffer_size;

    buffer_size = TEXT_BUFFER_INIT_SIZE;
    text_string = ALLOC(char, TEXT_BUFFER_INIT_SIZE);
    infile = input_file;

    cp_stack = tr_create();

    for(;;) {

	flush(BLANK);

	if(IS(DIGIT, CP_GET_CHAR)) {

	    int     user_num;
	    int     char_count;
	    char    *result;

	    CP_UNGET_CHAR(current_char, infile);

	    if (!cp_get_integer(&user_num)) {
		error("Cannot find extension number is user_extension command");
	    }

	    char_count = 0;

	    while(CP_GET_CHAR != ';') {
		if(char_count > buffer_size - 1) {
		    buffer_size = 2*(buffer_size + SLOP) - SLOP;
		    text_string = realloc(text_string, (unsigned) buffer_size);
		}
		text_string[char_count++] = current_char;
	    }

	    text_string[char_count++] = '\0';
	    result = util_strsav(text_string);
	    
	    CP_UNGET_CHAR(current_char, infile);
	    cp_userextension(user_num, result);

	} else {

	    switch(current_char) {

	    case 'E': 
		{
		    int     c;

		    while((c = getc(infile)) != EOF && IS(BLANK, (char) c));

		    if(c != EOF) {
			error("Extra characters after End statement");
		    }

		    cp_end();
		    tr_free(cp_stack);
		    FREE(text_string);

		    return;
		}

	    case 'P': 
		{
		    struct cp_path *path;
		    int number;

		    if(!cp_get_path(&number, &path)) {
			error("Cannot find path in Polygon command");
		    }
		    cp_polygon(number, path);
		}
		break;

	    case 'B': 
		{
		    int     width,
		            length;
		    cp_point center;
		    cp_point dir;

		    if(!cp_get_integer(&length)) {
			error("Cannot find length in Box command");
		    }

		    if(!cp_get_integer(&width)) {
			error("Cannot find width in Box command");
		    }

		    if(!cp_get_point(&center)) {
			error("Cannot find center in Box command");
		    }

		    if (!cp_get_point(&dir)) {
			dir.x = 1;
			dir.y = 0;
		    }

		    cp_box(length, width, center, dir);

		}
		break;

	    case 'R': 
		{
		    int     diameter;
		    cp_point center;

		    if(!cp_get_integer(&diameter)) {
			error("Cannot find diameter in RoundFlash command");
		    }

		    if(!cp_get_point(&center)) {
			error("Cannot find center in RoundFlash command");
		    }

		    cp_roundflash(diameter, center);
		}
		break;

	    case 'W': 
		{
		    int     width;
		    cp_path *path;
		    int number;

		    if(!cp_get_integer(&width)) {
			error("Cannot find width in Wire command");
		    }

		    if(!cp_get_path(&number, &path)) {
			error("Cannot find path in Polygon command");
		    }

		    cp_wire(width, number, path);
		}
		break;

	    case 'L': 
		{
		    char   *name;

		    flush(BLANK);

		    if(!cp_get_shortname(&name)) {
			error("Cannot find name in Layer command");
		    }

		    cp_layer(name);
		}
		break;

	    case 'D': 
		flush(BLANK);

		switch(CP_GET_CHAR) {

		case 'S': 
		    {
			int     cell_num;
			int     a,
			        b;

			if(in_definition) {
			    error("Nested cell definitions not allowed");
			}

			in_definition = TRUE;

			if(!cp_get_integer(&cell_num)) {
			    error("Cannot find cell number in Definition Start command");
			}

			if(!cp_get_integer(&a)) {
			    a = b = 1;
			}
			else {
			    if(!cp_get_integer(&b)) {
				error("Cannot find both scale factors in Definition Start command");
			    }
			}

			cp_defstart(cell_num, a, b);
		    }
		    break;

		case 'F': 

		    {
			if(!in_definition) {
			    error(" Definition Finish found outside of cell definition");
			}

			cp_deffinish();
		    }

		    in_definition = FALSE;
		    break;

		case 'D': 
		    {
			int     cell_num;

			if(in_definition) {
			    error(" Definition Delete found inside of cell definition");
			}

			if(!cp_get_integer(&cell_num)) {
			    error("Cannot find cell_num in Definition Delete command");
			}

			cp_defdelete(cell_num);
		    }
		    break;

		default: 
		    {
			error("Unrecogizable command");
		    }
		}
		break;

	    case 'C': 
		{
		    int     cell_num;
		    double   (*array)[2];
		    int32 x, y;
		    int type;

		    array = (double (*)[2]) malloc(sizeof(double [2][2]));

		    if(!cp_get_integer(&cell_num)) {
			error("Cannot find cell number in Call command");
		    }

		    tr_identity(cp_stack);
		    cp_get_transform(array, &x, &y, &type);

		    cp_call(cell_num, array, x, y, type);
		}
		break;

	    case '(': 
		{
		    int     level = 1;

		    while(level > 0) {
			CP_GET_CHAR;
			if(current_char == '(')
			    level++;
			else
			    if(current_char == ')')
				level--;
		    }

		}
		break;

	    default: 
		{
		    error("Unrecognizable command");
		}

	    }			/* switch */
	}

	flush(BLANK);

	if(CP_GET_CHAR != ';') {
	    error("Bad end of command found");
	}

    }				/* for */
}				/* parse */
