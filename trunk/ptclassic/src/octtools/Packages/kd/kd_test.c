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
/*
 * K-d tree package testing program
 *
 * David Harrison
 * University of California, Berkeley
 * 1988
 *
 * This program puts k-d through its paces.  It builds a tree
 * of random boxes and performs a number of queries on it
 * comparing the results with those obtained from a linear
 * search of the same space.  It then deletes every item
 * from the tree and makes sure none are left.  If everything
 * is ok,  it returns zero.
 */

#include "copyright.h"
#include "port.h"
#include "kd.h"
#include "errtrap.h"
#include <sys/time.h>

/* for errtrap */
char *optProgName = "";

#define KD_BOXES	10000
#define KD_REGIONS      1000

#define MIN_RANGE	-10000
#define MAX_RANGE	10000
#define RANGE_SPAN	(MAX_RANGE - MIN_RANGE + 1)
#define BOX_RANGE	1000

static kd_box boxes[KD_BOXES];
static void gen_boxes();
static int gen_box();
static void rand_box();

#define BOXINTERSECT(b1, b2) \
  (((b1)[KD_RIGHT] >= (b2)[KD_LEFT]) && \
   ((b2)[KD_RIGHT] >= (b1)[KD_LEFT]) && \
   ((b1)[KD_TOP] >= (b2)[KD_BOTTOM]) && \
   ((b2)[KD_TOP] >= (b1)[KD_BOTTOM]))

/*ARGSUSED*/
int
main(argc, argv)
int argc;
char *argv[];
{
    kd_tree tree;
    kd_box region, size;
    kd_gen gen;
    int local[KD_BOXES];
    int idx, i, j, k, n, item;
    
    optProgName = argv[0];
    gen_boxes();
    idx = 0;
    tree = kd_build(gen_box, (kd_generic) &idx);
    
    /* Phase one: random generations of boxes */
    for (i = 0;  i < KD_REGIONS;  i++) {
	rand_box(region);
	gen = kd_start(tree, region);
	n = 0;
	while (kd_next(gen, (kd_generic *) &(local[n]), size) == KD_OK) {
	    n++;
	}
	kd_finish(gen);
	/* Now the test of truth: is it right? */
	for (j = 0;  j < KD_BOXES;  j++) {
	    if (BOXINTERSECT(region, boxes[j])) {
		for (k = 0;  k < n;  k++) {
		    if (local[k] == j+1) {
			local[k] = -1;
			break;
		    }
		}
		if (k >= n) errRaise("kd_test", 0, "missing item");
	    }
	}
	for (k = 0;  k < n;  k++) {
	    if (local[k] >= 0) errRaise("kd_test", 0, "extra item");
	}
    }

    /* Phase two:  delete each item in reverse order... */
    for (i = KD_BOXES-1;  i >= 0;  i--) {
	if (kd_delete(tree, (kd_generic) (i+1), boxes[i]) != KD_OK) {
	    errRaise("kd_test", 0, "failed to delete item");
	}
    }
    /* and ask for everything in the tree - shouldn't be any */
    region[KD_LEFT] = MIN_RANGE-1;
    region[KD_BOTTOM] = MIN_RANGE-1;
    region[KD_RIGHT] = MAX_RANGE+1;
    region[KD_TOP] = MAX_RANGE+1;
    gen = kd_start(tree, region);
    while (kd_next(gen, (kd_generic *) &item, size) == KD_OK) {
	errRaise("kd_test", 0, "incomplete delete");
    }
    return 0;
}

static void gen_boxes()
/* Generates a whole bunch of random boxes */
{
    int i;

    for (i = 0;  i < KD_BOXES;  i++) {
	rand_box(boxes[i]);
    }
}

static int gen_box(arg, val, size)
kd_generic arg;
kd_generic *val;
kd_box size;
/*
 * Generates next box for insertion into tree
 */
{
    int *offset = (int *) arg;

    if (*offset < KD_BOXES) {
	*val = (kd_generic) (*offset)+1;
	size[KD_LEFT] = boxes[*offset][KD_LEFT];
	size[KD_BOTTOM] = boxes[*offset][KD_BOTTOM];
	size[KD_RIGHT] = boxes[*offset][KD_RIGHT];
	size[KD_TOP] = boxes[*offset][KD_TOP];
	*offset += 1;
	return 1;
    } else {
	return 0;
    }
}

static void rand_box(box)
kd_box box;
/* Generates a random box in `box' */
{
    static int init = 0;
#ifdef SYSV
    long lrand48();
#else
    long random();
#endif

    if (!init) {
	struct timeval tp;
	struct timezone whocares;

	/* Randomize generator */
	(void) gettimeofday(&tp, &whocares);
#ifdef SYSV
	(void) srand48((int) (tp.tv_sec + tp.tv_usec));	
#else
	(void) srandom((int) (tp.tv_sec + tp.tv_usec));
#endif
	init = 1;
    }

#ifdef SYSV
    box[KD_LEFT] = (lrand48() % RANGE_SPAN) + MIN_RANGE;
    box[KD_BOTTOM] = (lrand48() % RANGE_SPAN) + MIN_RANGE;
    box[KD_RIGHT] = box[KD_LEFT] + (lrand48() % BOX_RANGE);
    box[KD_TOP] = box[KD_BOTTOM] + (lrand48() % BOX_RANGE);
#else
    box[KD_LEFT] = (random() % RANGE_SPAN) + MIN_RANGE;
    box[KD_BOTTOM] = (random() % RANGE_SPAN) + MIN_RANGE;
    box[KD_RIGHT] = box[KD_LEFT] + (random() % BOX_RANGE);
    box[KD_TOP] = box[KD_BOTTOM] + (random() % BOX_RANGE);
#endif
}
