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
#include "scan.h"

/*LINTLIBRARY*/

    /* insert the point after 'pointer' */

void fa_insert_point(pointer, x, y, delta)
scan_point *pointer;
fa_coord x;
fa_coord y;
int delta;
{
    register scan_point *ptr;

    ptr = ALLOC(scan_point, 1);

    pointer->next->last = ptr;
    ptr->next = pointer->next;
    ptr->last = pointer;
    pointer->next = ptr;

    ptr->x = x;
    ptr->y = y;
    ptr->delta = delta;

}


void fa_init_line(line)
scan_line *line;
{
    line->head = ALLOC(scan_point, 1);
    line->tail = ALLOC(scan_point, 1);
    line->head->x = -FA_INFINITY;
    line->tail->x = FA_INFINITY;
    line->head->y = line->tail->y = -FA_INFINITY;
    line->head->delta = line->tail->delta = 0;
    line->head->r_depth = line->tail->r_depth = 0;

    line->tail->next = NIL(scan_point);
    line->tail->last = line->head;
    line->head->next = line->tail;
    line->head->last = NIL(scan_point);
}

void fa_free_line(line)
scan_line line;
{
    FREE(line.head);
    FREE(line.tail);
}

void fa_remove_point(pointer)
scan_point *pointer;
{
    pointer->last->next = pointer->next;
    pointer->next->last = pointer->last;
    FREE(pointer);
}
