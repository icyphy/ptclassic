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
#ifndef OCT_GEO_H
#define OCT_GEO_H

#define GEO_FIELDS GENERIC_FIELDS

struct geo {
    GEO_FIELDS;
};

struct box {
    GEO_FIELDS;
    struct octBox user_box;
};

struct label {
    GEO_FIELDS;
    struct octLabel user_label;
};

struct point {
    GEO_FIELDS;
    struct octPoint user_point;
};

struct edge {
    GEO_FIELDS;
    struct octEdge user_edge;
};

struct circle {
    GEO_FIELDS;
    struct octBox bbox;
    struct octCircle user_circle;
};

#define GEO_POINTS_FIELDS \
    GEO_FIELDS;\
    struct octBox bbox;\
    int32 num_points;\
    struct octPoint *points

struct geo_points {
    GEO_POINTS_FIELDS;
};

struct path {
    GEO_POINTS_FIELDS;
    struct octPath user_path;
};

struct polygon {
    GEO_POINTS_FIELDS;
};

struct change_record {
    GEO_POINTS_FIELDS;
    int object_type;
    struct octChangeRecord user_change_record;
    octObject *old_value;
};

void oct_init_geo_desc();

#endif /* OCT_GEO_H */
