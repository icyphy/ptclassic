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
/*
 * Dialog Definition System
 * Initialize package
 *
 * David Harrison
 * University of California, Berkeley
 * 1990
 */

#include "dds_internal.h"

#include <X11/Shell.h>

static ddsHandle no_new();
static void no_get_set();
static void no_del();

/*static ddsHandle do_full_init();
 static ddsHandle do_partial_init();
*/
static Widget init_parent();
static void init_placement();

ddsClass _dds_initialize = {
    DDS_INITIALIZE, no_new, no_get_set, no_get_set, no_del,
    init_parent, init_placement
};

static void xt_dump_core();



ddsBoolean dds_initialize(argc_ptr, argv)
int *argc_ptr;			/* Pointer to number of arguments */
char *argv[];			/* Argument list itself           */
/*
 * Creates a new top level component from which all dialogs
 * constructed using dds are derived.  This need be done
 * only once.  You cannot get, set, or delete this component.
 * Its only purpose is to create dialog components.
 */
{
    ddsFullInitialize *item;
    Display *disp;

    item = ALLOC(ddsFullInitialize, 1);
    item->base.class = &_dds_initialize;
    item->base.parent = (ddsInternal *) 0;
    _dds_set_init((ddsInternal *) item);

    XtSetErrorHandler(xt_dump_core);
    XtToolkitInitialize();
    item->context = XtCreateApplicationContext();
    disp = XtOpenDisplay(item->context, "", argv[0],
			 DDS_DEFAULT_CLASS, (XrmOptionDescRec *) 0,
			 (Cardinal) 0, argc_ptr, argv);
    if ( disp == 0 ) {
      /* Cannot open the display. */
      return DDS_FALSE;
    }
    item->app_shell =
      XtVaAppCreateShell(argv[0], DDS_DEFAULT_CLASS,
			 applicationShellWidgetClass, disp,
			 XtNallowShellResize, True,
			 XtNinput, True,
			 NULL);
    return DDS_TRUE;		/* Initialization successful. */
}



void dds_partial_init(context, app_shell)
XtAppContext context;		/* Application context */
Widget app_shell;		/* Application shell   */
/*
 * This routine does initialization assuming the user has already
 * established a connection to X, started the toolkit, parsed
 * the argument list, created an application context, and
 * made a top level shell.  The routine simply assigns the
 * necessary information and returns.  Applications that
 * want to be independent of the toolkit intrinsics should
 * use dds_initialize() instead.
 */
{
    ddsFullInitialize *item;

    item = ALLOC(ddsFullInitialize, 1);
    _dds_set_init((ddsInternal *) item);
    item->base.class = &_dds_initialize;
    item->base.parent = (ddsInternal *) 0;
    item->context = context;
    item->app_shell = app_shell;
}


static ddsHandle no_new(parent, class, data)
ddsInternal *parent;		/* Not used */
ddsClass *class;		/* Not used */
ddsData data;			/* Not used */
/*
 * Components of type DDS_INITIALIZE are created by calling
 * dds_initialize() or dds_partial_init().  They cannot
 * be created using dm_component.
 */
{
    errRaise(ddsPackageName, DDS_NOT_SUPPORTED,
	     "Items of type DDS_INITIALIZE are created using dm_initialize()");
    /*NOTREACHED*/
    return (ddsHandle)NULL;
}


static void no_get_set(item, data)
ddsInternal *item;		/* DDS_INITIALIZE             */
ddsData data;			/* Should be (dmInitialize *) */
/*
 * Get and set operations on the initial top level component is
 * not supported.
 */
{
    errRaise(ddsPackageName, DDS_NOT_SUPPORTED,
	     "dds_get() and dds_set() are not supported on components of type DDS_INITIALIZE");
    /*NOTREACHED*/
}

static void no_del(item)
ddsInternal *item;		/* DDS_INITIALIZE             */
/*
 * Delete is not supported for the top level component.
 */
{
    errRaise(ddsPackageName, DDS_NOT_SUPPORTED,
	     "Components of type DDS_INITIALIZE cannot be destroyed");
    /*NOTREACHED*/
}



/*ARGSUSED*/
static Widget init_parent(parent, child)
ddsInternal *parent;		/* DDS_INITIALIZE */
ddsInternal *child;		/* New component  */
/*
 * Returns a widget suitable for use as a parent widget for
 * creating sub-widgets under the initializer widget.
 */
{
    return parent->initialize.app_shell;
}

/*ARGSUSED*/
static void init_placement(parent, child, nm, widgets, vspan)
ddsInternal *parent;		/* DDS_INITIALIZE    */
ddsInternal *child;		/* New component     */
int nm;				/* Number of widgets */
WidgetList widgets;		/* Sub-widgets       */
int vspan;
/*
 * Since items under an initializer are pop-ups, no placement
 * is required.
 */
{
    /* Empty body */
}

static void xt_dump_core(msg)
String msg;
{
    (void) fprintf(stderr, "Fatal toolkit error: %s\n", msg);
    abort();
}
