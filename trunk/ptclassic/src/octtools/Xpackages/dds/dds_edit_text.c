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
 * Edit Text Component
 *
 * David Harrison
 * University of California, Berkeley
 * 1990
 */

#include "dds_internal.h"
#include "st.h"

#include <X11/Xaw/Label.h>
#include <X11/Xaw/AsciiText.h>

#define WIDTH_EST(w) ((w)*5/8)
#define AVE_CHAR_STR	"n"
#define CARET_SIZE	2

static ddsHandle new_edtext();
static void get_edtext();
static void set_edtext();
static void del_edtext();
static Widget parent_edtext();
static void place_edtext();

ddsClass _dds_edit_text = {
    DDS_EDIT_TEXT, new_edtext, get_edtext, set_edtext, del_edtext,
    parent_edtext, place_edtext
};


#ifdef FOCUS
/*
 * Focus related definitions
 */

#define gray_width 8
#define gray_height 8
static char gray_bits[] = {
   0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa};

#define solid_width 8
#define solid_height 8
static char solid_bits[] = {
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

static void do_focus();
static void edtext_focus();
static void ddsSetField();
static void put();
static ddsInternal *get();
static void rel();
static void turn_on();
static void turn_off();

static int focus_init = 0;
static XtTranslations focus_translations;

XtActionsRec newActions[] = {
    { "ddsSetField",	ddsSetField }
};
Cardinal newActionsCount = XtNumber(newActions);

static char new_translations[] =
  "Mod1<Key>Tab:		ddsSetField(Previous)\n\
   Ctrl<Key>q,<Key>Tab:	insert-char()\n\
   <Btn1Down>:			select-start() ddsSetField(Here)\n\
   <Key>Next:			ddsSetField(Next)\n\
   <Key>Prior:			ddsSetField(Previous)\n\
   <Key>Tab:			ddsSetField(Next)";

#endif /* FOCUS */


static ddsHandle new_edtext(parent, class, data)
ddsInternal *parent;		/* Should be DDS_TOP or DDS_COMPOSITE */
ddsClass *class;		/* Should be DDS_EDIT_TEXT            */
ddsData data;			/* Should be (ddsEditText *)          */
/*
 * Makes a new editable text component.  This consists of two widgets:
 * a label to the left and a editable text field on the right.  The
 * text field is sized using a rough estimator of font size.
 */
{
    ddsFullEditText *item;
    ddsInternal *same;
    XFontStruct *font;
    int top, bottom, right, left, char_width, char_height, wc;
    Widget pw, both[2];


    if (parent && ((parent->base.class->type == DDS_TOP) ||
		   (parent->base.class->type == DDS_COMPOSITE))) {
	item = ALLOC(ddsFullEditText, 1);
	same = (ddsInternal *) item;
	item->base.class = class;
	item->base.parent = parent;
	item->user_spec = *((ddsEditText *) data);
/* XXX hack to get single line text input fields to work - RLS */
#if !defined(vax) && !defined(mips)
	if (item->user_spec.rows <= 1) {
	    item->user_spec.rows = 2;
	}
#endif
/* XXX */
	pw = (*parent->base.class->_dds_parent_widget)(parent, same);
	wc = 0;
	if (item->user_spec.label) {
	    item->user_spec.label = util_strsav(item->user_spec.label);
	    item->label =
	      XtVaCreateManagedWidget(DDS_EDLBL_NAME,
				      labelWidgetClass,
				      pw, XtNlabel, item->user_spec.label,
				      NULL);
	    both[wc++] = item->label;
	} else {
	    item->label = (Widget) 0;
	}
	if (!item->user_spec.text) item->user_spec.text = "";
	item->user_spec.text = util_strsav(item->user_spec.text);
	item->text =
	  XtVaCreateManagedWidget(DDS_EDTEXT_NAME, asciiTextWidgetClass, pw,
				  XtNeditType, XawtextEdit,
				  XtNscrollVertical,
				  (item->user_spec.rows > 1 ?
				   XawtextScrollAlways :
				   XawtextScrollWhenNeeded),
				  XtNstring, item->user_spec.text,
				  XtNtype, XawAsciiString,
				  NULL);
	dds_add_cursor_widget(item->text, XtNcursor);
	both[wc++] = item->text;
	XtAddCallback(item->text, XtNdestroyCallback, _dds_destroy_callback,
		      (XtPointer) item);
	(*parent->base.class->_dds_place)(parent, same, wc, both,
					  item->user_spec.rows);
	do_focus(item);

	/* Here is yet another text widget size hack */
	top = bottom = left = right = 0;
	XtVaGetValues(item->text, XtNfont, &font, 
		      XtNtopMargin, &top, XtNbottomMargin, &bottom,
		      XtNrightMargin, &right, XtNleftMargin, &left,
		      NULL);
	char_height = font->ascent + font->descent;
	char_width = XTextWidth(font, AVE_CHAR_STR, strlen(AVE_CHAR_STR));
	XtVaSetValues(item->text,
		      XtNheight, (char_height*item->user_spec.rows)+top+bottom+CARET_SIZE,
		      XtNwidth, (char_width*item->user_spec.cols)+right+left,
		      NULL);
	return (ddsHandle) item;
    } else {
	errRaise(ddsPackageName, DDS_BAD_PARENT,
		 "The parent of DDS_EDIT_TEXT components must be of type DDS_TOP or DDS_COMPOSITE"); 
	/*NOTREACHED*/
    }
    return(ddsHandle)NULL;
}



static void get_edtext(item, data)
ddsInternal *item;		/* DDS_EDIT_TEXT   */
ddsData data;			/* (ddsEditText *) */
/*
 * Copies out the current state of the editable text item.  Note
 * that both the label and the text are internally owned strings
 * and should not be modified by the application.
 */
{
    ddsEditText *actual = (ddsEditText *) data;
    char *str;

    XtVaGetValues(item->edit_text.text, XtNstring, &str, NULL);
    FREE(item->edit_text.user_spec.text);
    item->edit_text.user_spec.text = util_strsav(str);
    *actual = item->edit_text.user_spec;
}

static void set_edtext(item, data)
ddsInternal *item;		/* DDS_EDIT_TEXT   */
ddsData data;			/* (ddsEditText *) */
/*
 * Sets the state of the editable text item.  If the state
 * differs, the appropriate widget is updated.
 */
{
    XFontStruct *font;
    ddsEditText *actual = (ddsEditText *) data;
    Arg arg_list[MAX_ARGS];
    int arg_len;
    int top, bottom, left, right, char_height, char_width;

    if (item->edit_text.user_spec.label &&
	(actual->label != item->edit_text.user_spec.label)) {
	FREE(item->edit_text.user_spec.label);
	if (!actual->label) actual->label = DDS_EDTEXT_NAME;
	item->edit_text.user_spec.label = util_strsav(actual->label);
	XtVaSetValues(item->edit_text.label,
		      XtNlabel, item->edit_text.user_spec.label, NULL);
    }
    arg_len = 0;
    if (actual->text != item->edit_text.user_spec.text) {
	FREE(item->edit_text.user_spec.text);
	if (!actual->text) actual->text = "";
	item->edit_text.user_spec.text = util_strsav(actual->text);
	XtSetArg(arg_list[arg_len], XtNstring, item->edit_text.user_spec.text);
	arg_len++;
    }
    top = bottom = left = right = 0;
    XtVaGetValues(item->edit_text.text, XtNfont, &font,
		  XtNtopMargin, &top, XtNbottomMargin, &bottom,
		  XtNrightMargin, &left, XtNleftMargin, &left, NULL);
    char_height = font->ascent + font->descent;
    char_width = XTextWidth(font, AVE_CHAR_STR, strlen(AVE_CHAR_STR));
    if (actual->rows != item->edit_text.user_spec.rows) {
	XtSetArg(arg_list[arg_len], XtNheight,
		   char_height*actual->rows+top+bottom+CARET_SIZE);
	arg_len++;
	item->edit_text.user_spec.rows = actual->rows;
    }
    if (actual->cols != item->edit_text.user_spec.cols) {
	XtSetArg(arg_list[arg_len], XtNwidth,
		   char_width*actual->cols+right+left);
	arg_len++;
	item->edit_text.user_spec.cols = actual->cols;
    }
    if (arg_len > 0) {
	XtSetValues(item->edit_text.text, arg_list, arg_len);
    }
}



static void del_edtext(item)
ddsInternal *item;		/* DDS_EDIT_TEXT */
/*
 * Releases the non-widget related data associated with an edit
 * text component.
 */
{
#ifdef FOCUS
    rel(item->edit_text.text);
#endif /* FOCUS */
    dds_remove_cursor_widget(item->edit_text.text, XtNcursor);
    if (item->edit_text.user_spec.label) FREE(item->edit_text.user_spec.label);
    FREE(item->edit_text.user_spec.text);
    FREE(item);
}



/*ARGSUSED*/
static Widget parent_edtext(parent, child)
ddsInternal *parent;		/* DDS_CONTROL */
ddsInternal *child;		/* New child   */
/*
 * The control component cannot have children.
 */
{
    errRaise(ddsPackageName, DDS_BAD_PARENT,
	     "DDS_EDIT_TEXT components may not have child components");
    /*NOTREACHED*/
    return (Widget)NULL;
}

/*ARGSUSED*/
static void place_edtext(parent, child, nw, wlist, vspan)
ddsInternal *parent;		/* DDS_CONTROL              */
ddsInternal *child;		/* New component            */
int nw;				/* Number of widgets        */
WidgetList wlist;		/* Widgets themselves       */
int vspan;
/*
 * This is an error.  The control component cannot have children.
 */
{
    errRaise(ddsPackageName, DDS_BAD_PARENT,
	     "DDS_EDIT_TEXT components may not have child components");
    /*NOTREACHED*/
}


#ifdef FOCUS

static void do_focus(item)
ddsFullEditText *item;
/*
 * Sets up focus management for the editable text component.  Right
 * now, this just adds the editable text field to the focus
 * widget list for the dialog.  Later, additional translations
 * will be required.
 */
{
    if (!focus_init) {
	XtAppAddActions(XtWidgetToApplicationContext(item->text),
			newActions, newActionsCount);
	focus_translations = XtParseTranslationTable(new_translations);
	focus_init = 1;
    }
    XtOverrideTranslations(item->text, focus_translations);
    put(item, item->text);
    _dds_focus_widget((ddsInternal *) item, item->text, edtext_focus, (XtPointer) item);
}



static void edtext_focus(w, client, call)
Widget w;			/* Widget itself       */
XtPointer client;		/* (ddsFullEditText *) */
XtPointer call;			/* focus_in_p          */
/*
 * Called when the focus changes between DDS_EDIT_TEXT components.
 */
{
    ddsFullEditText *item = (ddsFullEditText *) client;
    int focus_in_p = (int) call;

    if (focus_in_p) {
	turn_on(item->text);
    } else {
	turn_off(item->text);
    }
}


static void turn_on(w)
Widget w;
/*
 * Updates the widget so that visually it looks "on".
 */
{
    Pixel bdr = (Pixel) 0, bg = (Pixel) 0;
    int depth = 0;
    Pixmap map;

    XtVaGetValues(w, XtNborderColor, &bdr, XtNdepth, &depth,
		  XtNbackground, &bg, NULL);
    map = XCreatePixmapFromBitmapData(XtDisplay(w), XtWindow(w),
				      solid_bits, solid_width, solid_height,
				      bdr, bg, depth);
    XtVaSetValues(w, XtNborderPixmap, map, NULL);
    XFreePixmap(XtDisplay(w), map);
}

static void turn_off(w)
Widget w;
/*
 * Updates the widget to that visually it looks "off".
 */
{
    Pixel bdr = (Pixel) 0;
    Pixel bg = (Pixel) 0;
    int depth = 0;
    Pixmap map;

    XtVaGetValues(w, XtNborderColor, &bdr,
		  XtNbackground, &bg, XtNdepth, &depth, NULL);
    map =
      XCreatePixmapFromBitmapData(XtDisplay(w), XtWindow(w),
				  gray_bits, gray_width, gray_height,
				  bdr, bg, depth);
    XtVaSetValues(w, XtNborderPixmap, map, NULL);
    XFreePixmap(XtDisplay(w), map);
}



/*
 * Small table mapping widgets to ddsInternal for use by ddsSetField
 * translation function.
 */

static st_table *item_table = (st_table *) 0;

static void put(item, w)
ddsInternal *item;		/* Internal item structure */
Widget w;			/* Widget                  */
/*
 * Associates the widget `w' with the item structure `item'.
 */
{
    if (!item_table) {
	item_table = st_init_table(st_ptrcmp, st_ptrhash);
    }
    (void) st_insert(item_table, (char *) w, (char *) item);
}

static ddsInternal *get(w)
Widget w;			/* Widget */
/*
 * Retrieves the item structure from the table for widget `w'.
 */
{
    ddsInternal *result;

    if (item_table && st_lookup(item_table, (char *) w, (char **) &result)) {
	return result;
    } else {
	errRaise(ddsPackageName, DDS_INTERNAL,
		 "Failed to locate edit text component from widget");
	/*NOTREACHED*/
    }
    return (ddsInternal*)NULL;
}

static void rel(w)
Widget w;			/* Widget */
/*
 * Removes the widget from the table.
 */
{
    ddsInternal *val;

    (void) st_delete(item_table, (char **) &w, (char **) &val);
}



static void ddsSetField(w, evt, params, np)
Widget w;			/* Widget for event      */
XEvent *evt;			/* Event itself          */
String *params;			/* Params in translation */
Cardinal *np;			/* Number of parameters  */
/*
 * New action routine for dealing with focus.  It has one string
 * parameter which can be one of the following:
 *   Here	Set the focus to this widget
 *   Next	Set the focus to the next widget
 *   Previous	Set the focus to the previous widget
 */
{
    if (*np != 1) {
	errRaise(ddsPackageName, DDS_INTERNAL,
		 "ddsSetField action must have one argument");
	/*NOTREACHED*/
    }
    switch (params[0][0]) {
    case 'H':
    case 'h':
	_dds_set_focus(get(w), w);
	break;
    case 'N':
    case 'n':
	_dds_advance_focus(get(w));
	break;
    case 'P':
    case 'p':
	_dds_retreat_focus(get(w));
	break;
    default:
	errRaise(ddsPackageName, DDS_INTERNAL,
		 "ddsSetField parameter must be Here, Next, or Previous");
	/*NOTREACHED*/
    }
}

#endif /* FOCUS */
