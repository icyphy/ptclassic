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

#include "port.h"
#include "st.h"
#include "general.h"
#include "message.h"
#include "oct.h"
#include "vemUtil.h"

/*
  -------------------------------
  $Header$

  $Id$
 
  $Log$
 * Revision 3.0  90/04/03  16:55:24  greer
 * murphy undo stuff ( 4/3/90 )
 * 
 * Revision 1.2  90/03/16  14:53:17  murf
 * The undo updates, and an ID header for everyone
 * 
 * Revision 1.1  90/03/07  12:15:43  murf
 * Initial revision
 * 
  -------------------------------
  */
static st_table *facet_changelistid_table = 0;/* keys are facets, data is changelist ids */

st_table *get_facet_changelistid_table_addr()
{
    return facet_changelistid_table;
}

void
drsInitChangeList(facet)
octObject *facet;
{
    octObject cl;
    /* don't create two changelists on the same facet (just in case) */
    /* this check was made necc. when a port is added new to a cell */
    if( facet_changelistid_table ) {
	  if( st_lookup(facet_changelistid_table, (char *) facet->objectId, 0) )
	    return;
      }
    cl.type = OCT_CHANGE_LIST;
    cl.contents.changeList.objectMask =
      OCT_FACET_MASK | OCT_NET_MASK | OCT_INSTANCE_MASK | OCT_PROP_MASK |
	OCT_BAG_MASK | OCT_POLYGON_MASK | OCT_BOX_MASK | OCT_CIRCLE_MASK | 
	  OCT_PATH_MASK | OCT_LABEL_MASK | OCT_LAYER_MASK |
	    OCT_POINT_MASK | OCT_EDGE_MASK | OCT_FORMAL_MASK | OCT_TERM_MASK;
    cl.contents.changeList.functionMask =
      OCT_ALL_FUNCTION_MASK & ~OCT_ATTACH_CONTAINER_MASK & ~OCT_DETACH_CONTAINER_MASK;
    if (vuTemporary(facet, &cl) != OCT_OK) {
	vemMsg(MSG_C, "Could not create change list.  Undo will not work in this facet.\n");
    } else {
	if( !facet_changelistid_table ) {
	    facet_changelistid_table = st_init_table(st_numcmp,st_numhash);
	}
	st_insert(facet_changelistid_table, (char *) facet->objectId, (char *) cl.objectId);
    }
}

