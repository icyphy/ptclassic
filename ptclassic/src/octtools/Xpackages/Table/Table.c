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
/*LINTLIBRARY*/
/*
 * Table - Forms-based composite widget/geometry manager for the X Toolkit
 *
 * David Harrison
 * University of California, Berkeley
 * 1989
 *
 * This file contains the implementation for the Table widget.
 */

#include "port.h"
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>

#include <X11/Xmu/Xmu.h>

#include "TableP.h"

#define INIT_TBL_SIZE	10
#define TBL_CLASS_NAME	"Table"

static caddr_t def = (caddr_t) 0;

static XtResource resources[] = {
    { XtNlayout, XtCLayout, XtRPointer, sizeof(caddr_t),
	XtOffset(TableWidget, table.init_layout), XtRPointer, (caddr_t) &def },
    { XtNinternalHeight, XtCHeight, XtRDimension, sizeof(Dimension),
	XtOffset(TableWidget, table.int_height), XtRImmediate, (caddr_t) 0 },
    { XtNinternalWidth, XtCWidth, XtRDimension, sizeof(Dimension),
	XtOffset(TableWidget, table.int_width), XtRImmediate, (caddr_t) 0 },
    { XtNcolumnSpacing, XtCSpacing, XtRDimension, sizeof(Dimension),
	XtOffset(TableWidget, table.col_spacing), XtRImmediate, (caddr_t) 0 },
    { XtNrowSpacing, XtCSpacing, XtRDimension, sizeof(Dimension),
	XtOffset(TableWidget, table.row_spacing), XtRImmediate, (caddr_t) 0 },
    { XtNdefaultOptions, XtCOptions, XtROptions, sizeof(XtTblMask),
	XtOffset(TableWidget, table.def_options), XtRImmediate, (caddr_t) 0 },
    { XtNcursor, XtCCursor, XtRCursor, sizeof(Cursor),
	XtOffset(TableWidget, table.cursor), XtRImmediate, None }
};

/* Forward declarations */
static void TblClassInitialize();
static void TblInitialize();
static void TblResize();
static XtGeometryResult TblQueryGeometry();
static void TblPositionChild();
static Boolean TblSetValues();
static XtGeometryResult TblGeometryManager();
static void TblChangeManaged();
static Boolean TblFindChild();
static void TblDestroy();
static XtTblMask ParseOpts();



/*
 * Local structures
 */

typedef struct _TableLoc {
    Position ax, ay;		/* Position in array       */
    Dimension h_span, v_span;	/* Span size in array      */
    XtTblMask options;		/* Widget position options */
} TableLoc, *TableLocPtr;

typedef struct _TableLocEntry {
    Widget w;
    TableLoc loc;
} TableLocEntry, *TableLocEntryPtr;

struct _TableLocTbl {
    Cardinal n_layout;		/* Number of layout widgets */
    Cardinal a_layout;		/* Allocated space          */
    TableLocEntryPtr locs;	/* Widget locations         */
};

struct _TableDefLoc {
    String w_name;		/* Widget name        */
    TableLoc loc;		/* Widget information */
};

typedef unsigned long TableVecMask;
#define	VEC_MINIMIZE	0x01

struct _TableVector {
    TableVecMask mask;		/* Option mask  */
    Cardinal value;		/* Size of item */
};



TableClassRec tableClassRec = {
  { /* core_class fields */
    /* superclass         */    (WidgetClass) &compositeClassRec,
    /* class_name         */    TBL_CLASS_NAME,
    /* widget_size        */    sizeof(TableRec),
    /* class_initialize   */    TblClassInitialize,
    /* class_part_init    */    NULL,
    /* class_inited       */    FALSE,
    /* initialize         */    TblInitialize,
    /* initialize_hook    */    NULL,
    /* realize            */    XtInheritRealize,
    /* actions            */    NULL,
    /* num_actions        */    0,
    /* resources          */    resources,
    /* num_resources      */    XtNumber(resources),
    /* xrm_class          */    NULLQUARK,
    /* compress_motion    */    TRUE,
    /* compress_exposure  */    TRUE,
    /* compress_enterleave*/    TRUE,
    /* visible_interest   */    FALSE,
    /* destroy            */    TblDestroy,
    /* resize             */    TblResize,
    /* expose             */    XtInheritExpose,
    /* set_values         */    TblSetValues,
    /* set_values_hook    */    NULL,
    /* set_values_almost  */    XtInheritSetValuesAlmost,
    /* get_values_hook    */    NULL,
    /* accept_focus       */    NULL,
    /* version            */    XtVersion,
    /* callback_private   */    NULL,
    /* tm_table           */    NULL,
    /* query_geometry     */	TblQueryGeometry,
    /* display_accelerator*/	XtInheritDisplayAccelerator,
    /* extension          */	NULL
  },
  { /* composite_class fields */
    /* geometry_manager   */   TblGeometryManager,
    /* change_managed     */   TblChangeManaged,
    /* insert_child       */   XtInheritInsertChild,
    /* delete_child       */   XtInheritDeleteChild,
    /* extension          */   NULL
  },
  { /* table_class fields */
    /* position_child	  */	TblPositionChild,
    /* find_child	  */	TblFindChild,
  }
};

WidgetClass tableWidgetClass = (WidgetClass) &tableClassRec;



/*ARGSUSED*/
static void cvtStrToDefLoc(args, num_args, from, to)
XrmValue *args;			/* Arguments to converter */
Cardinal *num_args;		/* Number of arguments    */
XrmValue *from;			/* From type              */
XrmValue *to;			/* To type                */
/*
 * Converts a string representation into an array of TableDefLoc
 * structures.
 */
{
    static caddr_t ptr;
    String layout_spec;

    if (*num_args != 0) {
	XtErrorMsg("cvtStrToDefLoc", "wrongParameters", "XtToolkitError",
		   "String to layout takes no additional arguments",
		   (String *) NULL, (Cardinal *) NULL);
    }

    layout_spec = (String) from->addr;
    to->size = sizeof(caddr_t);
    ptr = XtTblParseLayout(layout_spec);
    to->addr = (caddr_t) &ptr;
}

/*ARGSUSED*/
static void cvtStrToOpts(args, num_args, from, to)
XrmValue *args;			/* Arguments to converter */
Cardinal *num_args;		/* Number of arguments    */
XrmValue *from;			/* From type              */
XrmValue *to;			/* To type                */
/*
 * Converts a string representation into a default options
 * mask (XtTblMask).
 */
{
    static XtTblMask mask;
    String opt_spec;

    if (*num_args != 0) {
	XtErrorMsg("cvtStrToOpts", "wrongParameters", "XtToolkitError",
		   "String to options takes no additional arguments",
		   (String *) NULL, (Cardinal *) NULL);
    }

    opt_spec = (String) from->addr;
    to->size = sizeof(int);
    mask = ParseOpts(opt_spec);
    to->addr = (caddr_t) &mask;
}


static void TblClassInitialize()
/*
 * Adds an appropriate string-to-default widget location table
 * converter.
 */
{
    XtAddConverter(XtRString, XtRPointer, cvtStrToDefLoc, NULL, 0);
    XtAddConverter(XtRString, XtROptions, cvtStrToOpts, NULL, 0);
}



/* 
 * Table Management Functions
 *
 * Default table is a linear null terminated array,  the location
 * array is a linear dynamic array.  Both should be replaced
 * with hash tables.
 */

static Cardinal LenDefLoc(ptr)
TableDefLocPtr ptr;
/*
 * Returns the length of a DefLoc list.
 */
{
    Cardinal i;

    for (i = 0;  ptr && ptr[i].w_name;  i++) {
	/* Null body */
    }
    return i;
}

static TableDefLocPtr CopyDefLoc(ptr)
TableDefLocPtr ptr;
/*
 * Makes a dynamically allocated copy of `ptr'.
 */
{
    TableDefLocPtr copy;
    Cardinal i, len;

    len = LenDefLoc(ptr);
    if (len) {
	copy = (TableDefLocPtr) XtCalloc(len+1, sizeof(struct _TableDefLoc));
	for (i = 0;  i < len;  i++) {
	    copy[i] = ptr[i];
	}
	copy[i].w_name = (String) 0;
    } else {
	copy = (TableDefLocPtr) 0;
    }
    return copy;
}

static TableDefLocPtr FindDefLoc(tbl, name)
TableDefLocPtr tbl;		/* Table to examine */
String name;			/* Widget name      */
/*
 * Scans through `tbl' looking for the name `name'.  Returns
 * a pointer to the found value or NULL if not found.
 */
{
    TableDefLocPtr idx;

    for (idx = tbl;  idx && idx->w_name;  idx++) {
	if (strcmp(idx->w_name, name) == 0) return idx;
    }
    return (TableDefLocPtr) 0;
}

static TableDefLocPtr MergeDefLoc(source, dest)
TableDefLocPtr source;		/* Original table     */
TableDefLocPtr dest;		/* Additional entries */
/*
 * Returns a table where the entries in `dest' have been
 * merged with those in `source'.  Similar entries in
 * `dest' override those in `source'.  The returned
 * table is allocated.
 */
{
    TableDefLocPtr result, update;
    Cardinal s_len, d_len;
    Cardinal i, j;

    s_len = LenDefLoc(source);
    d_len = LenDefLoc(dest);
    result = (TableDefLocPtr)
      XtCalloc(s_len + d_len + 1, sizeof(struct _TableDefLoc));
    for (i = 0;  i < s_len;  i++) {
	result[i] = source[i];
    }
    /* Add null termination */
    result[i].w_name = (String) 0;
    /* Now merge the results */
    for (j = 0;  j < d_len;  j++) {
	if ( (update = FindDefLoc(result, dest[j].w_name)) ) {
	    update->loc = dest[j].loc;
	} else {
	    /* Add to end */
	    result[i].w_name = dest[j].w_name;
	    result[i].loc = dest[j].loc;
	    i += 1;
	    result[i].w_name = (String) 0;
	}
    }
    return result;
}



static TableLocTblPtr TblInitLocTbl()
/*
 * Returns a newly allocated location table.  This is implemented
 * at the moment as dynamic array.  Eventually,  a hash table
 * will be used.
 */
{
    TableLocTblPtr rtn;

    rtn = (TableLocTblPtr) XtMalloc(sizeof(struct _TableLocTbl));
    rtn->n_layout = 0;
    rtn->a_layout = INIT_TBL_SIZE;
    rtn->locs = (TableLocEntryPtr)
      XtCalloc(INIT_TBL_SIZE, sizeof(TableLocEntry));
    return rtn;
}

static void TblInsertLoc(tbl, w, locp)
TableLocTblPtr tbl;		/* Table for insertion             */
Widget w;			/* Subwidget to place              */
TableLocPtr locp;		/* Widget location information     */
/*
 * Inserts an item into the location table.  If there is already
 * an entry for the widget,  it is replaced by this one.  If there
 * is no room,  additional room is allocated.
 */
{
    int i;

    for (i = 0;  i < tbl->n_layout;  i++) {
	if (tbl->locs[i].w == w) {
	    tbl->locs[i].loc = *locp;
	    return;
	}
    }
    /* Not in the table */
    if (tbl->n_layout >= tbl->a_layout) {
	/* Make more space */
	tbl->a_layout += tbl->a_layout;
	tbl->locs = (TableLocEntryPtr)
	  XtRealloc((char *)tbl->locs, tbl->a_layout * sizeof(TableLocEntry));
    }
    tbl->locs[tbl->n_layout].w = w;
    tbl->locs[tbl->n_layout].loc = *locp;
    tbl->n_layout += 1;
}

static TableLocPtr TblLocLookup(tbl, w)
TableLocTblPtr tbl;		/* Table for lookup      */
Widget w;			/* What widget to lookup */
/*
 * Looks up widget `w' in the hard widget position table.
 * Returns NULL if it can't find the widget.
 */
{
    int i;

    for (i = 0;  i < tbl->n_layout;  i++) {
	if (tbl->locs[i].w == w) {
	    return &(tbl->locs[i].loc);
	}
    }
    return (TableLocPtr) 0;
}

static void TblFreeLocTbl(tbl)
TableLocTblPtr tbl;		/* Table to free */
/*
 * Frees memory resources of `tbl'.
 */
{
    XtFree((char *) (tbl->locs));
    XtFree((char *) tbl);
}



static void TblInitialize(request, new)
Widget request;			/* Values from resources */
Widget new;			/* Actual widget         */
/*
 * Intializes appropriate fields in instance record.
 */
{
    TableWidget old = (TableWidget) request;
    TableWidget tw = (TableWidget) new;

    tw->table.init_layout = CopyDefLoc(old->table.init_layout);
    tw->table.layout_db = (TableDefLocPtr) 0;
    tw->table.real_layout = TblInitLocTbl();
    tw->table.vec_state = INVALID;
    tw->table.num_rows = tw->table.num_cols = 0;
    tw->table.rows = (TableVecPtr) 0;
    tw->table.cols = (TableVecPtr) 0;
    tw->table.vec_height = 0;
    tw->table.vec_width = 0;
}



static TableLocTblPtr GetManaged(nw, wl)
Cardinal nw;			/* Number of widgets */
WidgetList wl;			/* Widget list       */
/*
 * Returns those widgets in `wl' that are managed and looks
 * up their table postions.  If no table position is found,
 * the widget is placed at 0,0 with a span of 1 with no options.
 */
{
    TableLocTblPtr result;
    Cardinal i;

    result = TblInitLocTbl();
    for (i = 0;  i < nw;  i++) {
	if (XtIsManaged(wl[i])) {
	    if (result->n_layout >= result->a_layout) {
		/* Make more space */
		result->a_layout += result->a_layout;
		result->locs = (TableLocEntryPtr)
		  XtRealloc((char *)result->locs,
			    result->a_layout * sizeof(TableLocEntry));
	    }
	    result->locs[result->n_layout].w = wl[i];
	    if (!TblFindChild(wl[i],
			      &(result->locs[result->n_layout].loc.ax),
			      &(result->locs[result->n_layout].loc.ay),
			      &(result->locs[result->n_layout].loc.h_span),
			      &(result->locs[result->n_layout].loc.v_span),
			      &(result->locs[result->n_layout].loc.options))) {
		/* Can't find location -- make one up */
		result->locs[result->n_layout].loc.ax = 0;
		result->locs[result->n_layout].loc.ay = 0;
		result->locs[result->n_layout].loc.h_span = 1;
		result->locs[result->n_layout].loc.v_span = 1;
		result->locs[result->n_layout].loc.options = 0;
	    }
	    result->n_layout += 1;
	}
    }
    return result;
}


static Cardinal VecSize(mw, val_func)
TableLocTblPtr mw;		/* List of managed widgets           */
void (*val_func)();		/* Returns either row or column info */
/*
 * Determines the vector size by examining locations of all
 * widgets in `mw'.  Basically determines the maximum of loc+span.
 */
{
    Cardinal i, loc, span;
    Cardinal result = 0;
    Boolean small_flag;

    for (i = 0;  i < mw->n_layout;  i++) {
	(*val_func)(&(mw->locs[i]), &loc, &span, &small_flag);
	if (result < loc+span) {
	    result = loc+span;
	}
    }
    return result;
}



static void SetVecOptions(mw, val_func, vec)
TableLocTblPtr mw;		/* Managed widget list */
void (*val_func)();		/* Row or col info     */
TableVecPtr vec;		/* Spacing vector      */
/*
 * Steps through the list of widgets.  If the widget is marked
 * as having the small flag set,  it sets all corresponding
 * options in `vec'.
 */
{
    Cardinal i, j;
    Cardinal loc, span;
    Boolean small_flag;

    for (i = 0;  i < mw->n_layout;  i++) {
	(*val_func)(&(mw->locs[i]), &loc, &span, &small_flag);
	if (small_flag) {
	    for (j = loc;  j < loc+span;  j++) {
		vec[j].mask = VEC_MINIMIZE;
	    }
	}
    }
}


/* Must be set before span_cmp works */
static void (*span_cmp_val_func)();

static int span_cmp(a, b)
char *a, *b;
/*
 * Compares items based on span.
 */
{
    Cardinal loc_a, loc_b;
    Cardinal span_a, span_b;
    Boolean small_flag;

    (*span_cmp_val_func)((TableLocEntryPtr) a, &loc_a, &span_a, &small_flag);
    (*span_cmp_val_func)((TableLocEntryPtr) b, &loc_b, &span_b, &small_flag);
    return span_a - span_b;
}


static Cardinal FindDistrib(loc, span, vec, result)
Cardinal loc, span;		/* Widget loc and span */
TableVecPtr vec;		/* Spacing vector     */
Cardinal *result;		/* Result array       */
/*
 * This routine fills in `result' with a list of indices
 * into the spacing vector suitable for distributing required
 * space.  Normally,  it skips those items marked as
 * VEC_MINIMIZE.  However,  if there aren't any non-VEC_MINIMIZE
 * spaces,  all of them become candidates.
 */
{
    Cardinal i, count;

    count = 0;
    for (i = loc;  i < loc+span;  i++) {
	if (vec[i].mask & VEC_MINIMIZE) continue;
	result[count++] = i;
    }
    if (count == 0) {
	/* Add them all back in */
	for (i = loc;  i < loc+span;  i++) {
	    result[count++] = i;
	}
    }
    return count;
}


static void DoDistrib(n_dist, distrib, loc, span, vec, size)
Cardinal n_dist;		/* Number of distribution points */
Cardinal *distrib;		/* Indicies into `vec'           */
Cardinal loc, span;		/* Widget location and span      */
TableVecPtr vec;		/* Spacing vector                */
Dimension size;			/* Size of widget                */
/*
 * If `size' is larger than the current sum of space in `vec'
 * specified by `loc' and `span',  the difference in space
 * is evenly added to each vector entry given by `distrib'.
 */
{
    Cardinal sum = 0;
    Cardinal i;
    int diff, amt;

    for (i = loc;  i < loc+span;  i++) {
	sum += vec[i].value;
    }
    diff = size - sum;
    if (diff > 0) {
	/* Distribution required */
	amt = diff / n_dist;
	for (i = 0;  i < n_dist-1;  i++) {
	    vec[distrib[i]].value += amt;
	    diff -= amt;
	}
	/* Last one deincremented by remaining space */
	vec[distrib[i]].value += diff;
    }
}




static Cardinal CompVector(mw, val_func, size_func, result)
TableLocTblPtr mw;		/* List of managed widgets with locs */
void (*val_func)();		/* Returns either row or column info */
Dimension (*size_func)();	/* Returns desired size of subwidget */
TableVecPtr *result;		/* Result vector                     */
/*
 * This routine computes the values for either the row or column
 * spacing vector.  The strategy is as follows:
 *   1. Scan mw and determine number of entrys in result and allocate
 *   2. Scan list and set appropriate vector flags.
 *   3. Sort the managed widgets in span order (low to high)
 *   4. For each item in sorted list:
 *      A. Determine distribution locations.
 *      B. Distribute any needed space to locations.
 * There are some inefficiencies here that could be overcome.
 */
{
    Cardinal res_num, i;
    Cardinal n_dist, *distrib;
    Cardinal loc, span;
    Boolean small_flag;

    res_num = VecSize(mw, val_func);
    if (res_num) {
	*result = (TableVecPtr) XtCalloc(res_num, sizeof(struct _TableVector));
	for (i = 0;  i < res_num;  i++) {
	    (*result)[i].mask = 0;
	    (*result)[i].value = 0;
	}
	SetVecOptions(mw, val_func, *result);

	span_cmp_val_func = val_func;
	qsort((char *) mw->locs, (int) mw->n_layout,
	      sizeof(TableLocEntry), span_cmp);

	distrib = (Cardinal *) XtCalloc(res_num, sizeof(Cardinal));
	for (i = 0;  i < mw->n_layout;  i++) {
	    (*val_func)(&(mw->locs[i]), &loc, &span, &small_flag);
	    n_dist = FindDistrib(loc, span, *result, distrib);
	    DoDistrib(n_dist, distrib, loc, span, *result,
		      (*size_func)(mw->locs[i].w));
	}
	return res_num;
    } else {
	*result = (TableVecPtr) 0;
	return 0;
    }
    /*NOTREACHED*/
}


static void ColValues(oneloc, loc, span, small_flag)
TableLocEntryPtr oneloc;	/* Widget data       */
Cardinal *loc;			/* Location in array */
Cardinal *span;			/* Spanning distance */
Boolean *small_flag;		/* Whether locked    */
/*
 * This routine returns column data from `oneloc'.  It is
 * passed to CompVector when computing the column spacing vector.
 */
{
    *loc = oneloc->loc.ax;
    *span = oneloc->loc.h_span;
    *small_flag = oneloc->loc.options & TBL_SM_WIDTH;
}

static void RowValues(oneloc, loc, span, small_flag)
TableLocEntryPtr oneloc;	/* Widget data       */
Cardinal *loc;			/* Location in array */
Cardinal *span;			/* Spanning distance */
Boolean *small_flag;		/* Whether locked    */
/*
 * This routine returns row data from `oneloc'.  It is
 * passed to CompVector when computing the row spacing vector.
 */
{
    *loc = oneloc->loc.ay;
    *span = oneloc->loc.v_span;
    *small_flag = oneloc->loc.options & TBL_SM_HEIGHT;
}

static Dimension ColSize(w)
Widget w;			/* Child widget */
/*
 * This routine returns the desired width of the widget `w'.
 * It is used by CompVector when computing the column vector.
 */
{
    Dimension r_size, r_border;
    XtWidgetGeometry child;

    r_size = w->core.width;
    r_border = w->core.border_width;
    (void) XtQueryGeometry(w, (XtWidgetGeometry *) 0, &child);
    if (child.request_mode & CWWidth) r_size = child.width;
    if (child.request_mode & CWBorderWidth) r_border = child.border_width;
    return r_size + r_border + r_border;
}

static Dimension RowSize(w)
Widget w;			/* Child widget */
/*
 * This routine returns the desired width of the widget `w'.
 * It is used by CompVector when computing the column vector.
 */
{
    Dimension r_size, r_border;
    XtWidgetGeometry child;

    r_size = w->core.height;
    r_border = w->core.border_width;
    (void) XtQueryGeometry(w, (XtWidgetGeometry *) 0, &child);
    if (child.request_mode & CWHeight) r_size = child.height;
    if (child.request_mode & CWBorderWidth) r_border = child.border_width;
    return r_size + r_border + r_border;
}



/*ARGSUSED*/
static void TblRecompVectors(tw)
TableWidget tw;			/* Table widget */
/*
 * Recomputes the size vectors in the table widget by
 * examining the preferred sizes of subwidgets.  The
 * following fields are modified: num_rows, num_cols,
 * rows, cols, vec_height, and vec_width.
 */
{
    TableLocTblPtr managed;
    Cardinal i;

    /* Free existing vectors */
    if (tw->table.cols) XtFree((char *) (tw->table.cols));
    tw->table.num_cols = 0;
    if (tw->table.rows) XtFree((char *) (tw->table.rows));
    tw->table.num_rows = 0;
    tw->table.vec_width = tw->table.vec_height = 0;

    /* Generate list of managed widgets with locations */
    managed = GetManaged(tw->composite.num_children, tw->composite.children);
    
    /* Handle columns */
    tw->table.num_cols = CompVector(managed, ColValues, ColSize,
				    &(tw->table.cols));
    for (i = 0;  i < tw->table.num_cols;  i++) {
	tw->table.vec_width += tw->table.cols[i].value;
    }

    /* Handle rows */
    tw->table.num_rows = CompVector(managed, RowValues, RowSize,
				    &(tw->table.rows));
    for (i = 0;  i < tw->table.num_rows;  i++) {
	tw->table.vec_height += tw->table.rows[i].value;
    }

    TblFreeLocTbl(managed);
}


static void TblRequestResize(tw)
TableWidget tw;			/* Table widget */
/*
 * Asks the parent to become the size given by the row and
 * column vectors.  Precondition: vec_state must be MINIMUM.
 */
{
    XtGeometryResult rtn;
    Dimension act_width, act_height;
    Dimension r_width, r_height;

    act_width = tw->table.vec_width + 2*tw->table.int_width +
      (tw->table.num_cols-1)*tw->table.col_spacing;
    act_height = tw->table.vec_height + 2*tw->table.int_height +
      (tw->table.num_rows-1)*tw->table.row_spacing;
    rtn = XtMakeResizeRequest((Widget) tw, act_width, act_height,
			      &r_width, &r_height);
    switch (rtn) {
    case XtGeometryYes:
    case XtGeometryNo:
	/* Either case -- no action required */
	break;
    case XtGeometryAlmost:
	if ((r_width >= act_width) &&
	    (r_height >= act_height)) {
	    (void) XtMakeResizeRequest((Widget) tw, r_width, r_height,
				       (Dimension *) 0, (Dimension *) 0);
	}
	break;
    case XtGeometryDone:
	/* Added for completeness, should there be an action here? */
	break;
    }
}



static void ExtraSpace(num, vec, size)
Cardinal num;			/* Length of `vec' */
TableVecPtr vec;		/* Spacing vector  */
Dimension size;			/* Overall size    */
/*
 * If `size' is larger than the sum of all widths in `vec',
 * the extra space is distributed evenly among appropriate
 * candidates of `vec'.
 */
{
    Cardinal i, ndist, sum = 0;
    Cardinal *dist;
    int diff, amt;

    for (i = 0;  i < num;  i++) sum += vec[i].value;
    diff = size - sum;
    if (diff > 0) {
	/* Have to distribute space */
	dist = (Cardinal *) XtCalloc(num, sizeof(Cardinal));
	ndist = FindDistrib(0, num, vec, dist);
	amt = diff/ndist;
	for (i = 0;  i < ndist-1;  i++) {
	    vec[dist[i]].value += amt;
	    diff -= amt;
	}
	vec[dist[i]].value += diff;
	XtFree((char *) dist);
    }
}


static Dimension SumVec(loc, span, vec, start, inter, end)
Position loc, span;		/* Start and length      */
TableVecPtr vec;		/* Spacing vector        */
Dimension start;		/* Added before sum      */
Dimension inter;		/* Added between items   */
Dimension end;			/* Added after sum       */
/*
 * Returns the sum of the space in `vec' from `loc' for length `span'.
 * Adds in the appropriate padding given by `start', `inter' and `end'.
 */
{
    Position i;
    Dimension sum = 0;

    for (i = loc;  i < loc+span;  i++) sum += vec[i].value;
    return sum + start + end + ((span >= 0) ? span*inter : 0);
}

static void PlaceWidget(w, x, y, width, height, rw, rh, opt)
Widget w;			/* What widget to place  */
Position x, y;			/* Location of space     */
Dimension width, height;	/* Size of space         */
Dimension rw, rh;		/* Actual size           */
XtTblMask opt;			/* Justification options */
/*
 * This routine moves the widget `w' inside the space given
 * by x, y, width, height.  Its location in this space
 * is determined by looking at the justification options of 
 * `opt'.
 */
{
    Position rx, ry;

    if (opt & TBL_LEFT) rx = x;
    else if (opt & TBL_RIGHT) rx = x + width - rw;
    else rx = x + (width-rw)/2;
    if (opt & TBL_TOP) ry = y;
    else if (opt & TBL_BOTTOM) ry = y + height - rh;
    else ry = y + (height-rh)/2;
#ifdef NOTDEF
    rx += w->core.border_width;
    ry += w->core.border_width;
#endif
    
    XtMoveWidget(w, rx, ry);
}


static void DoPlace(managed, cvec, rvec, vp, hp, rs, cs)
TableLocTblPtr managed;		/* List of managed widgets with locs */
TableVecPtr cvec, rvec;		/* Column and row spacing vector     */
Dimension vp, hp;		/* Vertical and horizontal padding   */
Dimension rs, cs;		/* Row and column interspace         */
/*
 * This routine places each widget in `managed' according to the
 * spacing vectors `cvec' and `rvec' and the widget placement
 * options (justification and resizing).  First,  if allowed,
 * the routine will resize the widget to fit its allocated
 * space.  Then it will place the widget paying attention
 * to the justification.
 */
{
    Cardinal i;
    Position ax, ay;
    Dimension aw, ah;
    Dimension nw, nh;

    for (i = 0;  i < managed->n_layout;  i++) {
	ax = SumVec(0, managed->locs[i].loc.ax, cvec, hp, cs, 0);
	ay = SumVec(0, managed->locs[i].loc.ay, rvec, vp, rs, 0);
	aw = SumVec(managed->locs[i].loc.ax,
		    (Position) managed->locs[i].loc.h_span, cvec, 0, cs, -cs);
	ah = SumVec(managed->locs[i].loc.ay,
		    (Position) managed->locs[i].loc.v_span, rvec, 0, rs, -rs);
	nw = aw - 2*managed->locs[i].w->core.border_width;
	nh = ah - 2*managed->locs[i].w->core.border_width;
	if (managed->locs[i].loc.options & TBL_LK_WIDTH) {
	    nw = managed->locs[i].w->core.width;
	}
	if (managed->locs[i].loc.options & TBL_LK_HEIGHT) {
	    nh = managed->locs[i].w->core.height;
	}
	if (((nw != managed->locs[i].w->core.width) ||
	     (nh != managed->locs[i].w->core.height)) &&
	    (nw > 0) && (nh > 0)) {
	    /* Resize widget */
	    XtResizeWidget(managed->locs[i].w, nw, nh,
			   managed->locs[i].w->core.border_width);
	}
	/* Now place */
	nw = managed->locs[i].w->core.width +
	  2*managed->locs[i].w->core.border_width;;
	nh = managed->locs[i].w->core.height +
	  2*managed->locs[i].w->core.border_width;;
	PlaceWidget(managed->locs[i].w, ax, ay, aw, ah, nw, nh,
		    managed->locs[i].loc.options);
    }
}



/*ARGSUSED*/
static void TblPlacement(tw)
TableWidget tw;			/* Table widget */
/*
 * Places the children of the table widget.  There are several steps:
 *   1.  Distribute any extra space into local copies of spacing vectors.
 *   2.  If the option is set,  any extra space is offered to each widget.
 *   3.  The final placement is done according to the actual size of
 *       each widget and the space allocated for it.
 */
{
    Cardinal i;
    TableVecPtr lrows, lcols;
    TableLocTblPtr managed;
    Dimension real_width, real_height;
    long value;

    if (tw->table.num_rows && tw->table.num_cols) {
	/* Make local copies of vectors */
	lrows = (TableVecPtr)
	  XtCalloc(tw->table.num_rows, sizeof(struct _TableVector));
	for (i = 0;  i < tw->table.num_rows;  i++) {
	    lrows[i] = tw->table.rows[i];
	}
	lcols = (TableVecPtr)
	  XtCalloc(tw->table.num_cols, sizeof(struct _TableVector));
	for (i = 0;  i < tw->table.num_cols;  i++) {
	    lcols[i] = tw->table.cols[i];
	}

	/* Add extra space to vector */
	real_width = real_height = 0;
	if ((value = (tw->core.width - 2*tw->table.int_width -
		      (tw->table.num_cols-1)*tw->table.col_spacing)) >= 0) {
	    real_width = value;
	}
	if ((value = (tw->core.height - 2*tw->table.int_height -
		      (tw->table.num_rows-1)*tw->table.row_spacing)) >= 0) {
	    real_height = value;
	}
	ExtraSpace(tw->table.num_cols, lcols, real_width);
	ExtraSpace(tw->table.num_rows, lrows, real_height);

	/* Get list of managed widgets with locations */
	managed = GetManaged(tw->composite.num_children, tw->composite.children);
	DoPlace(managed, lcols, lrows, tw->table.int_height, tw->table.int_width,
		tw->table.row_spacing, tw->table.col_spacing);

	/* Free up resources */
	XtFree((char *) lcols);
	XtFree((char *) lrows);
	TblFreeLocTbl(managed);
    }
}




static void TblResize(w)
Widget w;			/* Table widget */
/*
 * This routine is called when the table widget itself is
 * resized.  If needed,  the vectors are recomputed and
 * placement is done.
 */
{
    TableWidget tw = (TableWidget) w;
#ifdef WATCH
printf("%s: TblResize new size is %d, %d\n", XtName(w), tw->core.width,
       tw->core.height);
#endif

#ifdef CYCLE
    /* Determined that this could cause infinite loops */
    if ((tw->core.width < tw->table.vec_width) ||
	(tw->core.height < tw->table.vec_height)) {
	TblRequestResize(tw);
    }
#endif
    if (tw->table.vec_state == INVALID) {
	TblRecompVectors(tw);
	tw->table.vec_state = MINIMUM;
    }
    TblPlacement(tw);
}



static XtGeometryResult ExamineRequest(request)
XtWidgetGeometry *request;
/*
 * Examines the bits set in `request' and returns an appropriate
 * geometry manager result.  Pure size changes are accepted
 * (XtGeometryYes),  pure position changes are rejected
 * (XtGeometryNo),  and combinations are conditionally
 * accepted (XtGeometryAlmost).
 */
{
    if (request->request_mode & (CWWidth|CWHeight|CWBorderWidth)) {
	if (request->request_mode & (CWX|CWY|CWSibling|CWStackMode)) {
	    return XtGeometryAlmost;
	} else {
	    return XtGeometryYes;
	}
    } else {
	return XtGeometryNo;
    }
}



static XtGeometryResult TblGeometryManager(w, request, reply)
Widget w;			/* Widget                    */
XtWidgetGeometry *request;	/* Requested geometry change */
XtWidgetGeometry *reply;	/* Actual reply to request   */
/*
 * This routine handles geometry requests from children.  Width
 * and height changes are always accepted.  Position changes
 * are always rejected.  Combinations result in XtGeometryAlmost
 * with the requested widths filled in.  Accepted changes cause
 * an immediate XtResizeWidget followed by a new placement.
 */
{
    Widget parent;
    TableWidget tw = (TableWidget) w->core.parent;
    XtGeometryResult result;
    Dimension width, height, bdr;
    Dimension ow, oh;

#ifdef WATCH
printf("%s: TblGeometryManager %s child wants",
       XtName(tw), XtName(w));
if (request->request_mode & CWWidth) printf(" width = %d", request->width);
if (request->request_mode & CWHeight) printf(" height = %d", request->height);
if (request->request_mode & CWBorderWidth) printf(" border = %d", request->border_width);
printf("\n");
#endif
    parent = w->core.parent;
    if (parent &&
	(strcmp(parent->core.widget_class->core_class.class_name,
		TBL_CLASS_NAME) == 0)) {
	tw = (TableWidget) parent;
	result = ExamineRequest(request);
	switch (result) {
	case XtGeometryYes:
	    if (request->request_mode & XtCWQueryOnly) {
		return XtGeometryYes;
	    }
	    if (request->request_mode & CWWidth) {
		width = request->width;
	    } else {
		width = w->core.width;
	    }
	    if (request->request_mode & CWHeight) {
		height = request->height;
	    } else {
		height = w->core.height;
	    }
	    bdr = w->core.border_width;
#ifdef WATCH
printf("\tRequest granted new size %d, %d, %d\n", width, height, bdr);
#endif
	    if ((width > 0) && (height > 0)) {
		XtResizeWidget(w, width, height, bdr);
	    }
	    ow = tw->table.vec_width;
	    oh = tw->table.vec_height;
	    TblRecompVectors(tw);
	    tw->table.vec_state = MINIMUM;
	    if ((ow != tw->table.vec_width) || (oh != tw->table.vec_height)) {
		TblRequestResize(tw);
	    }
	    TblPlacement(tw);
	    return XtGeometryDone;
	    /*NOTREACHED*/
	case XtGeometryNo:
	    return result;
	case XtGeometryAlmost:
	    *reply = *request;
	    /* Turn off all but the size changes */
	    reply->request_mode &= (CWWidth|CWHeight|CWBorderWidth);
	    return XtGeometryAlmost;
	case XtGeometryDone:
	    /* Added for completeness, should there be an action here? */
	    return result;
	}
	/*NOTREACHED*/
    } else {
	XtErrorMsg("TblGeometryManager", "badParent", "XtToolkitError",
		   "Parent of widget is not a tableClassWidget",
		   (String *) NULL, (Cardinal *) NULL);
    }
    /*NOTREACHED*/
    return XtGeometryNo;
}



/*ARGSUSED*/
static Boolean TblSetValues(current, request, new)
Widget current;			/* Before call to XtSetValues */
Widget request;			/* After call to XtSetValues  */
Widget new;			/* Final version of widget    */
/*
 * Checks for changes to `init_table'.  If so,  a recomputation
 * and replacement occurs.  Always returns false.
 */
{
    TableWidget old = (TableWidget) current;
    TableWidget tw = (TableWidget) new;
    Boolean recomp = False;

    if ((tw->table.init_layout) ||
	(old->table.int_width != tw->table.int_width) ||
	(old->table.int_height != tw->table.int_height) ||
	(old->table.row_spacing != tw->table.row_spacing) ||
	(old->table.col_spacing != tw->table.col_spacing)) {
	recomp = True;
    }
    if (old->table.cursor != tw->table.cursor) {
        XDefineCursor(XtDisplay(tw), XtWindow(tw), tw->table.cursor);
    }
    if (recomp) {
	/* Causes complete recomputation and placement */
	TblRecompVectors(tw);
	tw->table.vec_state = MINIMUM;
	TblRequestResize(tw);
	TblPlacement(tw);
    }
    return FALSE;
}




static void TblChangeManaged(w)
Widget w;			/* Table widget */
/*
 * This routine is called when a change to the managed set of
 * children occurs.  The current implementation simply refigures
 * the widget and repositions all widgets.  Better implementations
 * may be able to examine the change and react accordingly.
 */
{
    TableWidget tw = (TableWidget) w;

#ifdef WATCH
printf("%s: TblChangeManaged\n", XtName(w));
#endif
    TblRecompVectors(tw);
    tw->table.vec_state = MINIMUM;
    TblRequestResize(tw);
    TblPlacement(tw);
}



static XtGeometryResult TblQueryGeometry(w, request, geo_return)
Widget w;			/* Table widget         */
XtWidgetGeometry *request;	/* Parent intended size */
XtWidgetGeometry *geo_return;   /* This widget's size   */
/*
 * This routine is called by a parent that wants a preferred
 * size for the widget.  The `request' is the size (and/or position) 
 * the parent intends to make the widget.  The `geo_return' is the
 * preferred size of the widget.  The preferred size of the
 * table widget is its vector size.
 */
{
    TableWidget tw = (TableWidget) w;

    if (tw->table.vec_state == INVALID) {
	TblRecompVectors(tw);
	tw->table.vec_state = MINIMUM;
    }
    geo_return->request_mode = CWWidth|CWHeight;
    geo_return->width = tw->table.vec_width + 2*tw->table.int_width +
      (tw->table.num_cols-1)*tw->table.col_spacing;
    geo_return->height = tw->table.vec_height + 2*tw->table.int_height +
      (tw->table.num_rows-1)*tw->table.row_spacing;
#ifdef WATCH
printf("%s: TblQueryGeometry returns %d, %d\n",
       XtName(w), geo_return->width, geo_return->height);
#endif
    /* Now determine return code */
    if (((geo_return->request_mode & request->request_mode) !=
	 geo_return->request_mode) ||
	(request->width < geo_return->width) ||
	(request->height < geo_return->height)) {
	return XtGeometryAlmost;
    } else if ((request->width == geo_return->width) &&
	       (request->height == geo_return->height)) {
	return XtGeometryNo;
    } else {
	return XtGeometryYes;
    }
    /*NOTREACHED*/
}



static void TblPositionChild(w, c, r, hspan, vspan, options)
Widget w;			/* Subwidget to place              */
Position c, r;			/* Position in array (column, row) */
Dimension hspan, vspan;		/* Horizontal and vertical span    */
XtTblMask options;		/* Widget placement options        */
/*
 * This routine registers the position of Widget w.  The widget
 * must be a sub-widget of a Table class widget.  The row and
 * column must be non-negative.  The horizontal and vertical
 * span must be positive.  The options are as follows:
 *   TBL_LEFT		Horizontally left justified.
 *   TBL_RIGHT		Horizontally right justified.
 *   TBL_TOP		Vertically top justified.
 *   TBL_BOTTOM 	Vertically bottom justified.
 *   TBL_SM_WIDTH	Force the width to be as small as possible.
 *   TBL_SM_HEIGHT	Force the height to be as small as possible.
 *   TBL_LK_WIDTH	Don't try to expand the widget horizontally.
 *   TBL_LK_HEIGHT	Don't try to expand the widget vertically.
 * If `options' is equal to TBL_DEF_OPT,  it is filled with 
 * the default value for the table widget. The routine adds the 
 * information into a table and recomputes position information.
 */
{
    Widget parent;
    TableWidget tw;
    TableLoc loc;

    if ((c < 0) || (r < 0) || (hspan == 0) || (vspan == 0)) {
	/* Bad arguments */
	XtErrorMsg("TblPositionChild", "wrongParameters", "XtToolkitError",
		   "Bad value for row, column, hspan, or vspan",
		   (String *) NULL, (Cardinal *) NULL);
    }
    parent = w->core.parent;
    if (parent &&
	(strcmp(parent->core.widget_class->core_class.class_name,
		TBL_CLASS_NAME)==0)){
	/* The parent exists and is a TableWidget */
	tw = (TableWidget) parent;
	loc.ax = c;
	loc.ay = r;
	loc.h_span = hspan;
	loc.v_span = vspan;
	if (options == TBL_DEF_OPT) {
	    loc.options = tw->table.def_options;
	} else {
	    loc.options = options;
	}
	TblInsertLoc(tw->table.real_layout, w, &loc);
	tw->table.vec_state = INVALID;
	/* Full recomputation if realized */
#ifdef _IBMR2
#define XtIsRealized(object) (XtWindowOfObject(object) != 0)
#endif
	if (XtIsRealized(parent)) TblResize(parent);
    } else {
	XtErrorMsg("TblPositionChild", "badParent", "XtToolkitError",
		   "Parent of widget is not a tableClassWidget",
		   (String *) NULL, (Cardinal *) NULL);
    }
}



static Boolean TblFindChild(w, c_r, r_r, hspan_r, vspan_r, opt_r)
Widget w;			/* Widget to locate  */
Position *c_r, *r_r;		/* Returned position */
Dimension *hspan_r, *vspan_r;	/* Returned span     */
XtTblMask *opt_r;		/* Returned options  */
/*
 * This routine looks up a child widget's location and span.  The
 * parent must be a table widget.  Only non-zero fields are filled
 * in.  If the widget cannot be found,  the routine returns False
 * and does not modify any of the passed in pointers.  The routine
 * first looks in a table of widget positions defined by the
 * `position_child' class procedure.  If not found there,  it
 * searches the default table using the widget name.  These
 * defaults are set by resources or XtSetValues().
 */
{
    Widget parent;
    TableWidget tw;
    TableLocPtr locp;
    TableDefLocPtr temp;

    parent = w->core.parent;
    if (parent &&
	(strcmp(parent->core.widget_class->core_class.class_name,
		TBL_CLASS_NAME)==0)) {
	tw = (TableWidget) parent;
	if ( (locp = TblLocLookup(tw->table.real_layout, w)) ) {
	    if (c_r) *c_r = locp->ax;
	    if (r_r) *r_r = locp->ay;
	    if (hspan_r) *hspan_r = locp->h_span;
	    if (vspan_r) *vspan_r = locp->v_span;
	    if (opt_r) *opt_r = locp->options;
	    return True;
	} else {
	    if (tw->table.init_layout) {
		temp = MergeDefLoc(tw->table.layout_db, tw->table.init_layout);
		XtFree((char *) (tw->table.init_layout));
		tw->table.init_layout = (TableDefLocPtr) 0;
		XtFree((char *) (tw->table.layout_db));
		tw->table.layout_db = temp;
	    }
	    /* Attempt to look it up */
	    if ( (temp = FindDefLoc(tw->table.layout_db,
				    w->core.name)) ) {
		if (c_r) *c_r = temp->loc.ax;
		if (r_r) *r_r = temp->loc.ay;
		if (hspan_r) *hspan_r = temp->loc.h_span;
		if (vspan_r) *vspan_r = temp->loc.v_span;
		if (opt_r) *opt_r = temp->loc.options;
		return True;
	    } else {
		return False;
	    }
	}
    } else {
	XtErrorMsg("TblFindChild", "badParent", "XtToolkitError",
		   "Parent of widget is not a tableClassWidget",
		   (String *) NULL, (Cardinal *) NULL);
    }
    /*NOTREACHED*/
    return False;
}



static void TblDestroy(w)
Widget w;			/* Widget to destroy */
/*
 * Called to free resources consumed by the widget.
 */
{
    TableWidget tw = (TableWidget) w;

    XtFree((char *) (tw->table.init_layout));
    XtFree((char *) (tw->table.layout_db));
    TblFreeLocTbl(tw->table.real_layout);
    if (tw->table.rows) XtFree((char *) (tw->table.rows));
    if (tw->table.cols) XtFree((char *) (tw->table.cols));
}


static Cardinal DefSpecLen(layout)
String layout;			/* Full layout specification string */
/*
 * Examines `layout' and determines how many statements there are.
 * Basically counts semi-colons and adds one.
 */
{
    String idx = layout;
    Cardinal result = 0;

    while (idx && *idx) {
	idx = strchr(idx, ';');
	if (idx) {
	    result++;
	    idx++;
	}
    }
    return result+1;
}

#define MAX_FIELD	128
#define NUM_FIELDS	6
#define MAX_CHARS	255

static XtTblMask ParseOpts(spec)
String spec;			/* Option letters */
/*
 * Parses the null-terminated string of option characters in `spec'.
 * Returns a mask that is the `or' of all options selected.
 */
{
    static Boolean init = 0;
    static XtTblMask all_chars[MAX_CHARS];
    XtTblMask result = 0;
    String idx;

    if (!init) {
	Cardinal i;

	for (i = 0;  i < MAX_CHARS;  i++) all_chars[i] = 0;
	all_chars['l'] = TBL_LEFT;
	all_chars['r'] = TBL_RIGHT;
	all_chars['t'] = TBL_TOP;
	all_chars['b'] = TBL_BOTTOM;
	all_chars['w'] = TBL_LK_WIDTH;
	all_chars['h'] = TBL_LK_HEIGHT;
	all_chars['W'] = TBL_SM_WIDTH;
	all_chars['H'] = TBL_SM_HEIGHT;
    }
    for (idx = spec;  *idx;  idx++) {
	result |= all_chars[(int)*idx];
    }
    return result;
}

static void DefParse(spec, loc_spec)
String spec;			/* One specification statement */
TableDefLocPtr loc_spec;	/* Result location spec        */
/*
 * Parses a text specification statement into an internal
 * form given  by `loc_spec'.
 */
{
    static String def_name = "No Name";
    char fields[NUM_FIELDS][MAX_FIELD];
    Cardinal num;

    /* First, set up defaults */
    loc_spec->w_name = def_name;
    loc_spec->loc.ax = loc_spec->loc.ay = 0;
    loc_spec->loc.h_span = loc_spec->loc.v_span = 1;
    loc_spec->loc.options = 0;

    /* Now attempt to parse the string */
    num = sscanf(spec, "%s %s %s %s %s %s",
		 fields[0], fields[1], fields[2], fields[3], fields[4], fields[5]);
    if (num >= 1) {
	loc_spec->w_name = XtNewString(fields[0]);
    }
    if (num >= 2) {
	loc_spec->loc.ax = atoi(fields[1]);
    }
    if (num >= 3) {
	loc_spec->loc.ay = atoi(fields[2]);
    }
    if (num >= 4) {
	loc_spec->loc.h_span = atoi(fields[3]);
    }
    if (num >= 5) {
	loc_spec->loc.v_span = atoi(fields[4]);
    }
    if (num >= 6) {
	loc_spec->loc.options = ParseOpts(fields[5]);
    }
}

static String GetSpec(spec_ptr)
String *spec_ptr;		/* Specification pointer */
/*
 * This routine gets the next specification from the string
 * `spec_ptr' and updates the pointer appropriately.
 */
{
    String result;
    String semi;

    if (*spec_ptr && **spec_ptr) {
	semi = strchr(*spec_ptr, ';');
	if (semi) {
	    *semi = '\0';
	    result = *spec_ptr;
	    *spec_ptr = semi+1;
	    return result;
	} else {
	    result = *spec_ptr;
	    *spec_ptr += strlen(*spec_ptr);
	    return result;
	}
    } else {
	return (String) 0;
    }
}



/**********************************************************************
 *
 * Public routines
 *
 **********************************************************************/


/*ARGSUSED*/
caddr_t XtTblParseLayout(layout)
String layout;			/* String layout specification */
/*
 * Parses a string layout specification into an internal form
 * suitable for use in a call to XtSetValues().  The form is
 * a list of statements separated by semicolons.  Each statement
 * has the form:
 *   widget_name column row horizontal_span vertical_span opt_list
 * where the meaning of each field is:
 *   widget_name	Name of the widget as given to XtCreateWidget().
 *   column		Integer >= 0 descibing column in array
 *   row		Row >= 0 describing row in array
 *   horizontal_span	Integer >= 1 describing horizontal widget span
 *   vertical_span	Integer >= 1 describing vertical widget span
 *   opt_list		Series of characters each representing an option:
 *	l:  TBL_LEFT
 *	r:  TBL_RIGHT
 *      t:  TBL_TOP
 *      b:  TBL_BOTTOM
 *      w:  TBL_LK_WIDTH
 *      h:  TBL_LK_HEIGHT
 *      W:  TBL_SM_WIDTH
 *      H:  TBL_SM_HEIGHT
 * The options are as described in TblPostionChild().  The horizontal_span,
 * vertical_span, and opt_list are optional and will default to reasonable
 * values.
 */
{
    TableDefLocPtr result, idx;
    Cardinal len;
    String spec;

    /* Make a copy for safety */
    if (layout && ((len = DefSpecLen(layout)) > 0)) {
	layout = XtNewString(layout);
	result = (TableDefLocPtr) XtCalloc(len+1, sizeof(struct _TableDefLoc));
	idx = result;
	while ( (spec = GetSpec(&layout)) ) {
	    DefParse(spec, idx);
	    idx++;
	}
	/* null terminate */
	idx->w_name = (String) 0;
	XtFree(layout);
	return (caddr_t) result;
    } else {
	return (caddr_t) 0;
    }
    /*NOTREACHED*/
}

void XtTblPosition(w, col, row)
Widget w;			/* Widget to position */
Position col, row;		/* Position in array  */
/*
 * This routine positions a widget that has been created
 * under a widget of class tableWidgetClass.  The widget
 * will be placed at column `col' and row `row'.  If
 * the widget has never been placed before,  it will
 * span only one space in each direction and its
 * options will be the defaults for the table widget.
 */
{
    Position old_row, old_col;
    Dimension old_hspan, old_vspan;
    XtTblMask old_opts;

    if (TblFindChild(w, &old_col, &old_row, &old_hspan, &old_vspan, &old_opts)) {
	TblPositionChild(w, col, row, old_hspan, old_vspan, old_opts);
    } else {
	TblPositionChild(w, col, row, 1, 1, TBL_DEF_OPT);
    }
}

void XtTblResize(w, h_span, v_span)
Widget w;			/* Widget to resize            */
Dimension h_span, v_span;	/* New widget span             */
/*
 * This routine changes the span of widget `w' to (`h_span', `v_span').
 * If the widget has never been placed before,  it will be located
 * at (0,0) and its options will be the defaults for its
 * parent table widget.
 */
{
    Position old_row, old_col;
    Dimension old_hspan, old_vspan;
    XtTblMask old_opts;

    if (TblFindChild(w, &old_col, &old_row, &old_hspan, &old_vspan, &old_opts)) {
	TblPositionChild(w, old_col, old_row, h_span, v_span, old_opts);
    } else {
	TblPositionChild(w, 0, 0, h_span, v_span, TBL_DEF_OPT);
    }
}

void XtTblOptions(w, opt)
Widget w;			/* Widget to change */
XtTblMask opt;			/* New option mask  */
/*
 * This routine changes the options of widget `w' to `opt'.  If
 * the widget has never been placed before,  it will be located
 * and (0,0) with a span of (1,1) and its options will be the
 * default options for its parent table widget.  The option
 * mask is as described for TblPositionChild.
 */
{
    Position old_row, old_col;
    Dimension old_hspan, old_vspan;
    XtTblMask old_opts;

    if (TblFindChild(w, &old_col, &old_row, &old_hspan, &old_vspan, &old_opts)) {
	TblPositionChild(w, old_col, old_row, old_hspan, old_vspan, opt);
    } else {
	TblPositionChild(w, 0, 0, 1, 1, opt);
    }
}

void XtTblConfig(w, col, row, h_span, v_span, opt)
Widget w;			/* Widget to position          */
Position col, row;		/* Position in array           */
Dimension h_span, v_span;	/* Horizonal and vertical span */
XtTblMask opt;			/* Widget placement options    */
/*
 * This routine positions a widget that has been created
 * under a widget of class tableWidgetClass.  The widget
 * will be placed at column `col' and row `row'.  The
 * widget will span the distances given by `h_span' and `v_span'.
 * The options argument is as described for TblPositionChild.
 */
{
    TblPositionChild(w, col, row, h_span, v_span, opt);
}

