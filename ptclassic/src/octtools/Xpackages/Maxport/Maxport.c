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
 * Maxport
 *
 * David Harrison
 * University of California, Berkley
 * 1990
 *
 * Maxport is a subclass of the Athena Viewport widget that allows
 * the caller to specify a maximum width and height.  This is useful
 * if you have a child widget that starts out very large and you
 * would like to force the viewport to scroll initially.
 *
 * This is the implementation of the class.
 */

#include "port.h"
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>

#include <X11/Xaw/XawInit.h>
#include <X11/Xmu/Misc.h>
#include <X11/Xaw/Scrollbar.h>
#include <X11/Xaw/Viewport.h>
#include "MaxportP.h"

#define offset(field) XtOffset(MaxportWidget,maxport.field)
static XtResource resources[] = {
    {XtNmaxWidth, XtCWidth, XtRDimension, sizeof(Dimension),
       offset(max_width), XtRImmediate, (caddr_t) 0 },
    {XtNmaxHeight, XtCHeight, XtRDimension, sizeof(Dimension),
       offset(max_height), XtRImmediate, (caddr_t) 0 }
};
#undef offset

static XtGeometryResult PreferredGeometry();
static void ClassInit();
static void Oops();

#define XtInheritConstraintInitialize	((XtInitProc) _XtInherit)
#define XtRealInheritLayout		((Boolean (*)()) Oops)

#define superclass	(&viewportClassRec)
MaxportClassRec maxportClassRec = {
  { /* core_class fields */
    /* superclass	  */	(WidgetClass) superclass,
    /* class_name	  */	"Maxport",
    /* widget_size	  */	sizeof(MaxportRec),
    /* class_initialize	  */	XawInitializeWidgetSet,
    /* class_part_init    */    ClassInit,
    /* class_inited	  */	FALSE,
    /* initialize	  */	NULL,
    /* initialize_hook    */    NULL,
    /* realize		  */	XtInheritRealize,
    /* actions		  */	NULL,
    /* num_actions	  */	0,
    /* resources	  */	resources,
    /* num_resources	  */	XtNumber(resources),
    /* xrm_class	  */	NULLQUARK,
    /* compress_motion	  */	TRUE,
    /* compress_exposure  */	TRUE,
    /* compress_enterleave*/    TRUE,
    /* visible_interest	  */	FALSE,
    /* destroy		  */	NULL,
    /* resize		  */	XtInheritResize,
    /* expose		  */	XtInheritExpose,
    /* set_values	  */	NULL,
    /* set_values_hook    */    NULL,
    /* set_values_almost  */    XtInheritSetValuesAlmost,
    /* get_values_hook    */	NULL,
    /* accept_focus	  */	NULL,
    /* version            */    XtVersion,
    /* callback_private	  */	NULL,
    /* tm_table    	  */	NULL,
    /* query_geometry     */    PreferredGeometry,
    /* display_accelerator*/	XtInheritDisplayAccelerator,
    /* extension          */	NULL
  },
  { /* composite_class fields */
    /* geometry_manager	  */	XtInheritGeometryManager,
    /* change_managed	  */	XtInheritChangeManaged,
    /* insert_child	  */	XtInheritInsertChild,
    /* delete_child	  */	XtInheritDeleteChild,
    /* extension          */	NULL
  },
  { /* constraint_class fields */
    /* subresourses	  */	NULL,
    /* subresource_count  */	0,
    /* constraint_size	  */	sizeof(ViewportConstraintsRec),
    /* initialize	  */	XtInheritConstraintInitialize,
    /* destroy		  */	NULL,
    /* set_values	  */	NULL,
    /* extension          */	NULL
  },
  { /* form_class fields */
    /* layout		  */	XtRealInheritLayout
  },
  { /* viewport_class fields */
    /* empty		  */	0
  },
  { /* maxport_class fields */
    /* empty              */    0
  }
};
#undef superclass

WidgetClass maxportWidgetClass = (WidgetClass)&maxportClassRec;

static void ClassInit(class)
WidgetClass class;
{
    MaxportClassRec *mp = (MaxportClassRec *) class;
    ViewportClassRec *super = (ViewportClassRec *) mp->core_class.superclass;

    if (mp->constraint_class.initialize == XtInheritConstraintInitialize) {
	mp->constraint_class.initialize = super->constraint_class.initialize;
    }
    if (mp->form_class.layout == XtRealInheritLayout) {
	mp->form_class.layout = super->form_class.layout;
    }
}

static void Oops()
{
    _XtInherit();
}


static XtGeometryResult PreferredGeometry(w, constraints, reply)
    Widget w;
    XtWidgetGeometry *constraints, *reply;
{
    MaxportWidget mw = (MaxportWidget) w;
    XtGeometryResult result;

    if (mw->viewport.child != NULL) {
	result = XtQueryGeometry( ((ViewportWidget)w)->viewport.child,
				 constraints, reply );
	if (reply->request_mode & CWWidth) {
	    if (mw->viewport.vert_bar) {
		reply->width += mw->viewport.vert_bar->core.width +
		  2*mw->viewport.vert_bar->core.border_width;
	    }
	}
	if ((mw->maxport.max_width > 0) &&
	    (reply->width > mw->maxport.max_width)) {
	    if (!(reply->request_mode & CWWidth)) {
		reply->request_mode |= CWWidth;
	    }
	    reply->width = mw->maxport.max_width;
	}
	if (reply->request_mode & CWHeight) {
	    if (mw->viewport.horiz_bar) {
		reply->height += mw->viewport.horiz_bar->core.width +
		  2*mw->viewport.horiz_bar->core.border_width;
	    }
	}
	if ((mw->maxport.max_height > 0) &&
	    (reply->height > mw->maxport.max_height)) {
	    if (!(reply->request_mode & CWHeight)) {
		reply->request_mode |= CWHeight;
	    }
	    reply->height = mw->maxport.max_height;
	}
#ifdef NOTDEF
printf("%s: Maxport preferred geometry:", XtName(w));
if (reply->request_mode & CWWidth) printf(" width = %d", reply->width);
if (reply->request_mode & CWHeight) printf(" height = %d", reply->height);
printf("\n");
#endif
	if (((reply->request_mode & constraints->request_mode) !=
	     reply->request_mode) ||
	    (constraints->width < reply->width) ||
	    (constraints->height < reply->height)) {
	    return XtGeometryAlmost;
	} else if ((constraints->width == reply->width) &&
		   (constraints->height == reply->height)) {
	    return XtGeometryNo;
	} else {
	    return XtGeometryYes;
	}
    } else {
	return XtGeometryYes;
    }
}
