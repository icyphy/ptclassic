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
 * Lbl
 *
 * David Harrison
 * University of California, Berkley
 * 1990
 *
 * Lbl is a subclass of the Athena Label widget that uses
 * a slightly different method to compute preferred geometry.
 * This new method returns the actual size of the widget when
 * the resize resource is set.  Everything else about the
 * class is inherited.
 *
 * This is the implementation for the class.
 */

#include "port.h"
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>

#include "LblP.h"

static XtGeometryResult QueryGeometry();
static void ClassInitialize();

LblClassRec lblClassRec = {
  {
/* core_class fields */	
#define superclass		(&labelClassRec)
    /* superclass	  	*/	(WidgetClass) superclass,
    /* class_name	  	*/	"Lbl",
    /* widget_size	  	*/	sizeof(LblRec),
    /* class_initialize   	*/	ClassInitialize,
    /* class_part_initialize	*/	NULL,
    /* class_inited       	*/	FALSE,
    /* initialize	  	*/	NULL,
    /* initialize_hook		*/	NULL,
    /* realize		  	*/	XtInheritRealize,
    /* actions		  	*/	NULL,
    /* num_actions	  	*/	0,
    /* resources	  	*/	NULL,
    /* num_resources	  	*/	0,
    /* xrm_class	  	*/	NULLQUARK,
    /* compress_motion	  	*/	TRUE,
    /* compress_exposure  	*/	TRUE,
    /* compress_enterleave	*/	TRUE,
    /* visible_interest	  	*/	FALSE,
    /* destroy		  	*/	NULL,
    /* resize		  	*/	XtInheritResize,
    /* expose		  	*/	XtInheritExpose,
    /* set_values	  	*/	NULL,
    /* set_values_hook		*/	NULL,
    /* set_values_almost	*/	XtInheritSetValuesAlmost,
    /* get_values_hook		*/	NULL,
    /* accept_focus	 	*/	NULL,
    /* version			*/	XtVersion,
    /* callback_private   	*/	NULL,
    /* tm_table		   	*/	NULL,
    /* query_geometry		*/	QueryGeometry,
    /* display_accelerator	*/	XtInheritDisplayAccelerator,
    /* extension		*/	NULL
  },
/* Simple class fields initialization */
  {
    /* change_sensitive		*/	XtInheritChangeSensitive
  }
};
WidgetClass lblWidgetClass = (WidgetClass)&lblClassRec;

static void ClassInitialize()
/* For some reason, I can't inherit this function */
{
    (*(superclass->core_class.class_initialize))();
}

static XtGeometryResult QueryGeometry(w, intended, preferred)
    Widget w;
    XtWidgetGeometry *intended, *preferred;
{
    register LblWidget lw = (LblWidget)w;

    preferred->request_mode = CWWidth | CWHeight;
    if (lw->label.resize) {
	preferred->width = lw->label.label_width + 2 * lw->label.internal_width;
	preferred->height = lw->label.label_height + 2*lw->label.internal_height;
    } else {
	preferred->width = w->core.width;
	preferred->height = w->core.height;
    }
    if (  ((intended->request_mode & (CWWidth | CWHeight))
	   	== (CWWidth | CWHeight)) &&
	  intended->width == preferred->width &&
	  intended->height == preferred->height)
	return XtGeometryYes;
    else if (preferred->width == w->core.width &&
	     preferred->height == w->core.height)
	return XtGeometryNo;
    else
	return XtGeometryAlmost;
}
