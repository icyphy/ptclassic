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
#ifndef OCT_MASTER_H
#define OCT_MASTER_H

#ifndef ST_H
#include "st.h"
#endif /* ST_H */
#include "ansi.h"

#ifndef OCT_INTERNAL_H
#include "internal.h"
#endif
/*typedef st_table master_table;*/

master_table *oct_make_master_table();

void oct_master_desc_set
	ARGS((struct object_desc *object_desc));

int oct_get_master
	ARGS((struct facet *desc, struct octInstance *inst, struct master **master_p));

int oct_make_master
	ARGS(( struct facet *desc, struct octInstance *inst, struct master **master_p));

octStatus oct_bind_master
	ARGS((struct facet *desc, struct master *master));

octStatus oct_invalidate_all_instance_bbs
	ARGS((struct facet *desc));
octStatus oct_update_all_instance_bbs
	ARGS((struct facet *desc));
octStatus oct_validate_all_instance_bbs
	ARGS((struct facet *facet));
int oct_dump_instances
	ARGS((struct facet *desc));
int oct_restore_instances
	ARGS((struct facet *desc, int *next_type));
#endif /* OCT_MASTER_H */
