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
#ifndef INST_H
#define INST_H
#include "ansi.h"

void oct_instance_desc_set
	ARGS((struct object_desc *object_desc));

octStatus oct_add_actual
	ARGS((struct instance *inst, struct term *term));

octStatus oct_add_interface_formal
	ARGS((struct facet *facet, struct term *contents_formal, struct term **new_p));
octStatus oct_recompute_instance_bb
	ARGS((struct instance  *inst, struct octBox *user_box));
int oct_add_to_error_bag
	ARGS((generic *obj));
int oct_only_trivial_attachements
	ARGS((generic *obj, generic *container));
int oct_delete_actual
	ARGS((struct instance *inst, struct term *term));

int oct_update_terminals
	ARGS((struct facet *master, struct instance *inst));

#endif
