/* Version Identification:
 * @(#)cif.h	1.1	12/15/93
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
#ifndef CIF_H
#define CIF_H

#include "cp.h"

struct cif_link {
    char *link;
    int type;
};

#define CIF_NEXT(ptr) ((struct cif_link *) ptr)->link
#define CIF_TYPE(ptr) ((struct cif_link *) ptr)->type

#define CIF_BOX 1
#define CIF_POLYGON 2
#define CIF_WIRE 3
#define CIF_ROUND_FLASH 4
#define CIF_USER_EXTENSION 5
#define CIF_LAYER 6
#define CIF_INSTANCE 7
#define CIF_CELL 8

struct cif_box {
    char *link;
    int type;
    int line;
    int length, width;
    struct cp_point center, dir;
};

struct cif_queue {
    char *head;
    char *tail;
};

struct cif_chain {
    struct cif_cell *user;
    struct cif_cell *master;
    struct cif_chain *next_user, *last_user;
    struct cif_chain *next_master, *last_master;
};

struct cif_cell {
    char *link;
    int type;
    int line;
    int number;
    int a,b;
    char *name;
    struct {
	unsigned being_processed : 1;
	unsigned already_created : 1;
	unsigned redefined : 1;
	unsigned child_redefined : 1;
    } flags;
    struct cif_queue geos;
    struct cif_queue instances;
    struct cif_chain *masters;
    struct cif_chain *users;
};

struct cif_layer {
    char *link;
    int type;
    int line;
    char *name;
};

struct cif_instance {
    char *link;
    int type;
    int line;
    int cell_num;
    int trans_type;
    int32 trans_x, trans_y;
    double (*matrix)[2];
};

struct cif_wire {
    char *link;
    int type;
    int line;
    int width;
    int number;
    cp_path *path;
};

struct cif_polygon {
    char *link;
    int type;
    int line;
    int number;
    cp_path *path;
};

struct cif_round_flash {
    char *link;
    int type;
    int line;
    int diameter;
    cp_point center;
};

struct cif_user_extension {
    char *link;
    int type;
    int line;
    int number;
    char *text;
};

extern int cif_current_layer_defined;

#endif /* CIF_H */
