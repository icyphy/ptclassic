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
 * Dialog Definition System
 * Externally Defined Items
 *
 * David Harrison
 * University of California, Berkeley
 * 1990
 */

#include "dds_internal.h"

static ddsHandle new_ext();
static void get_ext();
static void set_ext();
static void del_ext();
static Widget parent_ext();
static void place_ext();

ddsClass _dds_extern = {
    DDS_EXTERN, new_ext, get_ext, set_ext, del_ext, parent_ext, place_ext
};



static ddsHandle new_ext(parent, class, data)
ddsInternal *parent;		/* Should be DDS_TOP or DDS_COMPOSITE */
ddsClass *class;		/* Should be DDS_EXTERN               */
ddsData data;			/* Should be (ddsEnumerate *)         */
/*
 * Makes a new external component.  This component is special.  It
 * allows the user to incorporate locally developed components into
 * dds dialogs.  The behaviour of these units are the responsibility
 * of the caller.  Dds simply handles the placement of the items.
 * 
 * Dds calls the routine `new_comps' with the arguments specified.
 * The `parent' argument will be a specification for the parent
 * of the new components (if any).  The routine should create
 * the new components under that parent, and return them in
 * `cmps'.  The placement algorithm used by dds places items into
 * an array.  Items are arrayed horizontally based on the number
 * of components returned (this is the return value for the function).  
 * They are arrayed vertically based on the vertical span, `vspan' 
 * returned by the routine.  This should be set based on how many
 * primitive components you expect vertically.  If you don't know,
 * specify 1.  The list of components is freed by calling the
 * user specified function `free_comps'.
 * 
 * For the Xt/Xaw implementation, the components are considered
 * to be widgets.  A destroy callback will be placed on the first
 * of these to remove dds stuff from it.
 */
{
    ddsFullExtern *item;
    ddsInternal *same;
    Widget pw, *all_widgets;
    int num_widgets, vspan;

    if (parent) {
	item = ALLOC(ddsFullExtern, 1);
	same = (ddsInternal *) item;
	item->base.class = class;
	item->base.parent = parent;
	item->user_spec = *((ddsExtern *) data);
	pw = (*parent->base.class->_dds_parent_widget)(parent, same);
	num_widgets = (*item->user_spec.new_comps)
	  ((ddsData) pw, (ddsData **) &all_widgets, &vspan,
	   item->user_spec.user_data);
	if (num_widgets >= 0) {
	    XtAddCallback(all_widgets[0], XtNdestroyCallback,
			  _dds_destroy_callback, (XtPointer) item);
	    (*parent->base.class->_dds_place)(parent, same, num_widgets,
					      all_widgets, vspan);
	    (*item->user_spec.free_comps)
	      (num_widgets, (ddsData *) all_widgets);
	    return (ddsHandle) item;
	} else {
	    errRaise(ddsPackageName, DDS_BAD_VALUE,
		     "DDS_EXTERN components must have at least one widget");
	    /*NOTREACHED*/
	}
    } else {
	errRaise(ddsPackageName, DDS_BAD_PARENT,
		 "Items of type DDS_EXTERN must have a parent");
	/*NOTREACHED*/
    }
    return (ddsHandle)NULL;
}



static void get_ext(item, data)
ddsInternal *item;		/* DDS_EXTERN    */
ddsData data;			/* (ddsExtern *) */
/*
 * Copies out the current state of the extern component.
 */
{
    ddsExtern *actual = (ddsExtern *) data;

    *actual = item->ext.user_spec;
}



static void set_ext(item, data)
ddsInternal *item;		/* DDS_EXTERN    */
ddsData data;			/* (ddsExtern *) */
/*
 * Changes the state of the external component.  This has no
 * effect on the component.  Real changes must be done by the
 * caller.
 */
{
    ddsExtern *actual = (ddsExtern *) data;

    item->ext.user_spec = *actual;
}



static void del_ext(item)
ddsInternal *item;		/* DDS_EXTERN */
/*
 * Frees all non-widget resources consumed by extern component.
 */
{
    FREE(item);
}



/*ARGSUSED*/
static Widget parent_ext(parent, child)
ddsInternal *parent;		/* DDS_EXTERNAL  */
ddsInternal *child;		/* New child     */
{
    errRaise(ddsPackageName, DDS_BAD_PARENT,
	     "DDS_EXTERNAL components may not have child components");
    /*NOTREACHED*/
    return (Widget)NULL;
}

/*ARGSUSED*/
static void place_ext(parent, child, nw, wlist, vspan)
ddsInternal *parent;		/* DDS_EXTERNAL  */
ddsInternal *child;		/* New child     */
int nw;				/* Number of widgets */
WidgetList wlist;		/* New children  */
int vspan;
{
    errRaise(ddsPackageName, DDS_BAD_PARENT,
	     "DDS_EXTERNAL components may not have child components");
    /*NOTREACHED*/
}
