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
#include "copyright.h"
#include "port.h"
#include "oct.h"
#include "tr.h"

#define PRINTF	(void) printf

#define HELP \
"p - push stack, ^ - pop stack, 1 - identity, t - translate, o - do operation,\n\
d - rotate dir, a - rotate angle, m - rotate matrix, x - transform things,\n\
i - is_{manhattan,mirrored}, s - scale, g - print top of stack as rotation\n\
. - print top of stack, h, ? - this message.\n\
Capitals cause left-multiply\n"

int
main()
{
    int32 x, y, tx, ty, scaler;
    int op, mirrored;
    double angle, dm[2][2], scale;
    char c[2];
    tr_stack *stack;
    int add_after = 0;
    long tlx,tly;		/* Read in as longs, because int32
				   could be long or int */

    PRINTF(HELP);

    stack = tr_create();
    
    if (stack == (tr_stack *) 0) {
	PRINTF("Could not initialize the package\n");
	exit(-1);
    }

    for(;;) {

	PRINTF("> ");
	if (scanf("%1s", c) != 1) {
	    tr_free(stack);
	    exit(0);
	}

	if (isupper(c[0])) {
	    c[0] += 'a' - 'A';
	    add_after = 1;
	} else {
	    add_after = 0;
	}

	switch (c[0]) {
	case 'p' :
	    tr_push(stack);
	    break;

	case '^' :
	    tr_pop(stack);
	    break;
	    
	case '1' :
	    tr_identity(stack);
	    break;

	case 't' :
	    PRINTF("    translate by: ");
	    (void) scanf("%ld %ld", &tlx, &tly);
	    x = tlx; y = tly;
	    tr_translate(stack, x, y, add_after);
	    break;

	case 's' :
	    PRINTF("    scale by: ");
	    (void) scanf("%lg", &scale);
	    tr_scale(stack, scale, add_after);
	    break;

	case 'o' :
	again:
	    op = -1;
	    PRINTF("    do operation #: ");
	    (void) scanf("%1s", c);
	    op = c[0] - '0';
	    if (op < 0 || op > 7) {
		PRINTF("No=0, MX=1, MY=2, R90=3, R180=4, R270=5, MXR90=6, MYR90=7\n");
		goto again;
	    }
	    tr_do_op(stack, op, add_after);
	    break;

	case 'd' :
	    PRINTF("    direction vector: ");
	    (void) scanf("%ld %ld", &tlx, &tly);
	    x = tlx; y = tly;
	    tr_rotate_dir(stack, x, y, add_after);
	    break;

	case 'a' :
	    PRINTF("    angle: ");
	    (void) scanf("%lf", &angle);
	    tr_rotate_angle(stack, angle, add_after);
	    break;

	case 'm' :
	    PRINTF("    matrix: ");
	    (void) scanf("%lf %lf %lf %lf", &dm[0][0], &dm[0][1], &dm[1][0],
		  &dm[1][1]);
	    tr_rotate_matrix(stack, dm, add_after);
	    break;

	case 'x' :
        xagain:
	    PRINTF("    transform what: ");
	    (void) scanf("%1s", c);
	    switch (c[0]) {
	    case 'v' :
		PRINTF("        xform the vector: ");
		(void) scanf("%ld %ld", &tlx, &tly);
		x = tlx; y = tly;
		tr_transform(stack, &x, &y);
		PRINTF("yielding (%ld,%ld)\n", (long)x, (long)y);
		break;
	    case 'a' :
		PRINTF("        xform the angle: ");
		(void) scanf("%lf", &angle);
	        tr_angle_transform(stack, &angle);
		PRINTF("yielding (%g)\n", angle);
		break;
	    case 's' :
		PRINTF("        xform the scaler: ");
		(void) scanf("%ld", &tlx);
		scaler = tlx;
		tr_scaler_transform(stack, &scaler);
		PRINTF("yielding (%ld)\n", (long)scaler);
		break;
	    case 'd':
		PRINTF("        xform the direction: ");
		(void) scanf("%ld %ld", &tlx, &tly);
		x = tlx; y = tly;
		tr_itransform(stack, &x, &y);
		PRINTF("yielding (%ld,%ld)\n", (long)x, (long)y);
		break;
	    case 'i' :
		PRINTF("    inverse transform the vector: ");
		(void) scanf("%ld %ld", &x, &y);
		tr_inverse_transform(stack, &x, &y);
		PRINTF("yielding (%ld,%ld)\n", (long)x, (long)y);
		break;
	    default:
		PRINTF("one of v(ector), a(ngle), s(caler), d(irection) i(nverse)\n");
		goto xagain;
	    }
	    break;
	case 'i' :
	iagain :
	    PRINTF("    is what: ");
	    (void) scanf("%1s", c);
	    switch (c[0]) {
	    case 'm' :
		PRINTF("tr_is_manhattan returns %d\n",
					       tr_is_manhattan(stack));
		break;
	    case 'x' :
		PRINTF("tr_is_mirrored returns %d\n",
					      tr_is_mirrored(stack));
		break;
	    default:
		PRINTF("one of m (is_manhattan), x (is_mirrored)\n");
		goto iagain;
	    }
	    break;
	case 'g' :
	    tr_get_angle(stack, &angle, &mirrored, &scale);
	    PRINTF("yields %g rotation %smirrored scaled by %g\n",
		   angle, (mirrored ? "" : "not "), scale);
	    break;
	case '.' :
	    tr_get_transform(stack, dm, &tx, &ty, &op);
	    PRINTF("yields [%g,%g][%g,%g] (%s) by (%ld,%ld)\n",
		   dm[0][0], dm[0][1], dm[1][0], dm[1][1], tr_op_name[op],
		   (long)tx, (long)ty);
	    break;
	case '?' :
	case 'h' :
	    PRINTF(HELP);
	}
    }
    return 0;
}
