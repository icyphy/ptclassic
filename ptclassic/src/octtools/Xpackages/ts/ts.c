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
/*LINTLIBRARY*/
/*
 * Typescript Library
 *
 * David Harrison
 * University of California, Berkeley
 * 1988
 *
 * This is a library built on top of the AsciiText widget in the
 * Athena widget set that provides typescript capabilities.  It
 * opens a new top-level shell with an AsciiText widget in it.
 * The user supplies a callback function which is called when
 * any key is pressed in the window.  The package exports a function
 * for writing characters onto the end of the buffer of the typescript
 * window.  This function also supports deleting characters from
 * the end of the buffer.  Selection operations similar to those
 * found in xterm are supported.
 *
 * Initially,  this is not a formal subclass of asciiTextWidget.
 * Eventually,  I imagine it could be made into one.
 */

#include "port.h"
#ifdef DEBUG
char *s_prompt();
#endif
#ifndef sequent
#include <memory.h>
#endif
#include <X11/Xatom.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Xaw/AsciiText.h>
#include <X11/Xaw/Text.h>
#include <X11/Xmu/Atoms.h>	/* Pick up XmuInternStrings() */
#include "st.h"
#include "ts.h"

#define MAX_ARGS	50
#define MAX_BUF		120
#define MAX_LF_SEARCH	256

#define NL_CHAR	'\012'
#define BS_CHAR	'\010'

/*
 * Maintains its own context to store buffer size and callback function.
 * The key is (Widget).  The data is (ts_info *).
 */
static st_table *ts_table = (st_table *) 0;

/*
 * The following is stored in the hash table:
 */
typedef struct ts_info_defn {
    Cardinal bufsize;		/* Desired size of buffer */
    Cardinal actsize;		/* Actual size of buffer  */
    String buf;			/* Buffer itself          */
    XtCallbackProc callback;	/* Callback procedure     */
    caddr_t client_data;	/* Client data            */
} ts_info;

/*
 * Forward routines
 */

static void ts_init();
static int ts_args();
static int ts_line_remove();
static int ts_char_remove();
static void ts_del();
static void tsSelectionReceived();
static void SelOffSetInsert();
int ts_merge
	ARGS((int size_one, Cardinal len_one, ArgList args_one,
	      Cardinal len_two, ArgList args_two));



/*
 * Routines for obtaining the current selection.  These are modified
 * versions of routines found in the textWidgetClass.  Unfortunately,
 * this is necessary because the text widget does not export a routine
 * for obtaining selection (just selection position).
 */

typedef struct ts_sel_list_defn {
    String *params;
    Cardinal count;
    Time time;
} ts_sel_list;

typedef struct ts_sel_data_defn {
    ts_info *info;
    ts_sel_list *list;
} ts_sel_data;

static void tsGetSelection(w, time, params, num_params, info)
Widget w;			/* Text widget                    */
Time time;			/* Event time                     */
String *params;			/* selections in precedence order */
Cardinal num_params;		/* Number of parameters           */
ts_info *info;			/* Typescript data                */
/*
 * Obtains current selected set of text (if any).
 */
{
    void tsSelectionReceived();
    Atom selection;
    ts_sel_data *data;
    int buffer;

    XmuInternStrings(XtDisplay(w), params, (Cardinal)1, &selection);
    switch (selection) {
      case XA_CUT_BUFFER0: buffer = 0; break;
      case XA_CUT_BUFFER1: buffer = 1; break;
      case XA_CUT_BUFFER2: buffer = 2; break;
      case XA_CUT_BUFFER3: buffer = 3; break;
      case XA_CUT_BUFFER4: buffer = 4; break;
      case XA_CUT_BUFFER5: buffer = 5; break;
      case XA_CUT_BUFFER6: buffer = 6; break;
      case XA_CUT_BUFFER7: buffer = 7; break;
      default:	       buffer = -1;
    }
    if (buffer >= 0) {
	int nbytes;
	int fmt8 = 8;
	Atom type = XA_STRING;
	char *line = XFetchBuffer(XtDisplay(w), &nbytes, buffer);
	if (nbytes > 0) {
	    data = XtNew(ts_sel_data);
	    data->info = info;
	    data->list = (ts_sel_list *) 0;
	    tsSelectionReceived(w, (caddr_t) data, &selection, &type,
				(caddr_t)line, &nbytes, &fmt8);
	} else if (num_params > 1) {
	    tsGetSelection(w, time, params+1, num_params-1, info);
	}
    } else {
	data = XtNew(ts_sel_data);
	data->info = info;
	if (--num_params) {
	    data->list = XtNew(ts_sel_list);
	    data->list->params = params + 1;
	    data->list->count = num_params;
	    data->list->time = time;
	} else data->list = (ts_sel_list *) 0;
	XtGetSelectionValue(w, selection, XA_STRING, tsSelectionReceived,
			    (caddr_t) data, time);
    }
}


/* ARGSUSED */
static void tsSelectionReceived(w, client_data, selection, type,
				value, length, format)
Widget w;			/* Incoming widget */
caddr_t client_data;		/* Actually ts_sel_data */
Atom *selection, *type;
caddr_t value;			/* Selection value  */
unsigned long *length;		/* Selection length */
int *format;			/* Selection format */
/*
 * Used as a callback for XtGetSelectionValue.
 */
{
    ts_sel_data *seldata = (ts_sel_data *) client_data;
    ts_call_data calldata;
    void SelOffSetInsert();
				  
    if (*type == 0 /*XT_CONVERT_FAIL*/ || *length == 0) {
	if (seldata->list != NULL) {
	    tsGetSelection(w, seldata->list->time, seldata->list->params,
			   seldata->list->count, seldata->info);
	    XtFree(seldata->list);
	}
	return;
    }
    if (*length > 0) {
	calldata.len = *length;
	calldata.buf = (char *) value;
	(*seldata->info->callback)(w, seldata->info->client_data, (caddr_t) &calldata);
	SelOffSetInsert(w, seldata->info);
    }
    XtFree(seldata->list);
    XtFree(seldata);
    XtFree(value);
}


/*
 * Action routines
 */

static Time EventTime(evt)
XEvent *evt;			/* Event to examine */
/*
 * Returns the time field of the event.
 */
{
    Time ret_time = 0;

    switch (evt->type) {
    case ButtonPress:
    case ButtonRelease:
	ret_time = evt->xbutton.time;
	break;
    case KeyPress:
    case KeyRelease:
	ret_time = evt->xkey.time;
	break;
    case MotionNotify:
	ret_time = evt->xmotion.time;
	break;
    case EnterNotify:
    case LeaveNotify:
	ret_time = evt->xcrossing.time;
	break;
    }
    return ret_time;
}

static void SelOffSetInsert(w, info)
Widget w;			/* Incoming text widget */
ts_info *info;			/* Typescript info      */
/*
 * Turns off selection and moves the insertion point to
 * the end (if its not already there).
 */
{
    XawTextPosition left, right, insert;

    insert = XawTextGetInsertionPoint(w);
    if (insert != info->actsize) {
	XawTextSetInsertionPoint(w, (XawTextPosition) info->actsize);
    }
    XawTextGetSelectionPos(w, &left, &right);
    if ((right - left) > 0) {
	XawTextSetSelection(w, info->actsize, info->actsize);
    }
}

/*ARGSUSED*/
static void KeyInput(w, event, params, num_params)
Widget w;			/* Incoming text widget */
XEvent *event;			/* Keypressed event     */
String *params;			/* Parameters           */
Cardinal *num_params;		/* Number of parameters */
/*
 * This routine calls the callback function for the typescript
 * window with the appropriate character.
 */
{
    char buf[MAX_BUF];
    ts_info *info;
    ts_call_data data;
    int len;

    len = XLookupString((XKeyEvent *) event, buf, MAX_BUF, (KeySym *) 0,
			(XComposeStatus *) 0);
    if ((len > 0) && (st_lookup(ts_table, (char *) w, (char **) &info))) {
	/* Call the callback function */
	data.len = len;
	data.buf = buf;
	(*info->callback)(w, info->client_data, (caddr_t) &data);
	SelOffSetInsert(w, info);
    }
}

/*ARGSUSED*/
static void SelInput(w, event, params, num_params)
Widget w;			/* Incoming text widget */
XEvent *event;			/* Causal event         */
String *params;			/* Parameters           */
Cardinal *num_params;		/* Number of parameters */
/*
 * This routine gets the currently selected selection and
 * sends it to the callback function for the widget.
 */
{
    static String default_params[] = { "PRIMARY", "CUT_BUFFER0" };
    Cardinal np;
    ts_info *info;

    if (st_lookup(ts_table, (char *) w, (char **) &info)) {
	if ((np = *num_params) == 0) {
	    params = default_params;
	    np = XtNumber(default_params);
	}
	/*
	 * This routine may be asynchronous and immediately return.
	 */
	tsGetSelection(w, EventTime(event), params, np, info);
    }
}


/*
 * New (or modified) actions
 */

XtActionsRec tsActionTable[] = {
    { "key-input",	KeyInput },
    { "sel-input",	SelInput }
};

/*
 * Translation table
 */

char tsTextTranslations[] =
"\
<Key>:		key-input() \n\
<FocusIn>:	focus-in() \n\
<FocusOut>:	focus-out() \n\
<Btn1Down>:	select-start() \n\
<Btn1Motion>:	extend-adjust() \n\
<Btn1Up>:	extend-end(PRIMARY, CUT_BUFFER0) \n\
<Btn2Down>:	sel-input(PRIMARY, CUT_BUFFER0) \n\
<Btn3Down>:	extend-start() \n\
<Btn3Motion>:	extend-adjust() \n\
<Btn3Up>:	extend-end(PRIMARY, CUT_BUFFER0) \
";

static XtTranslations ts_translations;



Widget tsCreate(app, name, parent, bufsize, callback, client_data, args,num_args)
XtAppContext app;		/* Application context           */
String name;			/* Name of new typescript widget */
Widget parent;			/* Parent widget                 */
Cardinal bufsize;		/* Size of typescript buffer     */
XtCallbackProc callback;	/* Callback on user input        */
caddr_t client_data;		/* Data passed to callback       */
ArgList args;			/* Overriding argument list      */
Cardinal num_args;		/* Number of overriding args     */
/*
 * Creates a new asciiText widget with typescript semantics.  This
 * is done by overriding the translation table for the widget.  The new
 * action table calls the function `callback' whenever a character
 * is typed in the typescript window.  The callback function is
 * called automatically when selection insertion is requested
 * by the user.  The callback function can add characters to
 * the buffer using tsWriteChar or tsWriteStr.  These functions
 * automatically maintain the buffer at a size whose maximum
 * is `bufsize'.  The routine returns NULL if there were problems.
 */
{
    Widget w;
    ts_info *info;
    Arg arg_list[MAX_ARGS];
    Cardinal arg_len;
    String buf;

    if (ts_table == (st_table *) 0) {
	/* Initialize module */
	ts_init(app);
    }
    arg_len = ts_args(arg_list, MAX_ARGS, bufsize, &buf);
    arg_len = ts_merge(MAX_ARGS, arg_len, arg_list, num_args, args);
    w = XtCreateManagedWidget(name, asciiTextWidgetClass,
			      parent, arg_list, arg_len);
    info = XtNew(ts_info);
    info->bufsize = bufsize;
    info->buf = buf;
    info->actsize = 0;
    info->callback = callback;
    info->client_data = client_data;
    (void) st_insert(ts_table, (char *) w, (char *) info);
    return w;
}


static void ts_init(context)
XtAppContext context;
/*
 * Initializes the typescript library.  This builds a new hash table,
 * registers new actions,  and compiles the new translation table.
 */
{
    ts_table = st_init_table(st_ptrcmp, st_ptrhash);
    if (ts_table == (st_table *) 0) {
	XtErrorMsg("allocError", "st_init_table", "tsLibraryError",
		   "Cannot allocate hash table", (String *) NULL,
		   (Cardinal *) NULL);
    }
    XtAppAddActions(context, tsActionTable, XtNumber(tsActionTable));
    ts_translations = XtParseTranslationTable(tsTextTranslations);
}

static int ts_args(arg_list, max_args, bufsize, buf)
Arg arg_list[MAX_ARGS];		/* Argument list to fill in    */
Cardinal max_args;		/* Maximum number of arguments */
int bufsize;			/* String buffer size          */
String *buf;			/* Buffer itself               */
/*
 * This routine sets up all of the arguments to the asciiTextWidget.
 */
{
    static XtCallbackRec ts_del_list[] = {
	{ ts_del, (XtPointer) 0 },
	{ (XtCallbackProc) NULL, (XtPointer) NULL }
    };
    int count = 0;
    

    if (max_args < 9) {
	XtErrorMsg("overflow", "ts_args", "tsLibraryError",
		   "Cannot merge argument lists", (String *) NULL,
		   (Cardinal *) NULL);
    }
    XtSetArg(arg_list[count], XtNeditType, XawtextEdit);	count++;
    XtSetArg(arg_list[count], XtNtype, XawAsciiString);		count++;
    XtSetArg(arg_list[count], XtNuseStringInPlace, True);	count++;
    *buf = memset(XtMalloc(bufsize), 0, bufsize);
    XtSetArg(arg_list[count], XtNstring, *buf);			count++;
    XtSetArg(arg_list[count], XtNlength, bufsize);		count++;
    XtSetArg(arg_list[count], XtNtranslations, ts_translations);count++;
    XtSetArg(arg_list[count], XtNscrollVertical, XawtextScrollAlways);  count++;
    XtSetArg(arg_list[count], XtNwrap, XawtextWrapWord);	count++;
    XtSetArg(arg_list[count], XtNdestroyCallback, ts_del_list); count++;
    return count;
}

int ts_merge(size_one, len_one, args_one, len_two, args_two)
int size_one;			/* Size of list one     */
Cardinal len_one;		/* Length of list one   */
ArgList args_one;		/* First argument list  */
Cardinal len_two;		/* Length of list two   */
ArgList args_two;		/* Second argument list */
/*
 * This routine merges items in `args_two' into `args_one'.  Duplicate
 * entries in `args_two' are ignored.
 */
{
    int i, j;

    for (i = 0;  i < len_two;  i++) {
	for (j = 0;  j < len_one;  j++) {
	    if (strcmp(args_two[i].name, args_one[j].name) == 0) break;
	}
	if (j >= len_one) {
	    if (len_one < size_one) {
		args_one[len_one] = args_two[i];
		len_one++;
	    } else {
		/* Overflow */
		XtErrorMsg("overflow", "ts_merge", "tsLibraryError",
			   "Cannot merge argument lists", (String *) NULL,
			   (Cardinal *) NULL);
	    }
	}
    }
    return len_one;
}


/*ARGSUSED*/
int tsWriteChar(w, ch)
Widget w;			/* Typescript widget to add character to */
unsigned char ch;		/* Character to add to buffer            */
/*
 * This routine adds a character to the typescript widget `w'.  If the
 * passed widget is not a typescript widget,  the routine calls XtErrorMsg.
 * All printable characters are appended onto the buffer.  Backspace
 * (control-H) will delete the last character and move the cursor back
 * one spot.  Tabs will be displayed using the standard asciiTextWidget
 * semantics.
 */
{
    ts_info *info;
    XawTextBlock block;
    char ch_buf[4];
    int ret_value;

    if (st_lookup(ts_table, (char *) w, (char **) &info)) {
	if (ch == BS_CHAR) {
	    /* Backspace processing */
	    block.firstPos = 0;
	    block.length = 0;
	    block.ptr = ch_buf;
	    block.format = FMT8BIT;
	    ret_value = XawTextReplace(w, info->actsize-1, info->actsize, &block);
	    info->actsize -= 1;
	    XawTextSetInsertionPoint(w, info->actsize);
	} else {
	    /* Normal insert character */
	    if (info->actsize >= info->bufsize) {
		/* Must make space */
		if (!ts_line_remove(w, info, 1)) {
		    /* Remove difference */
		    (void) ts_char_remove(w, info, 1);
		}
	    }
	    block.firstPos = 0;
	    block.length = 1;
	    ch_buf[0] = ch;
	    block.ptr = ch_buf;
	    block.format = FMT8BIT;
	    ret_value = XawTextReplace(w, info->actsize, info->actsize, &block);
	    info->actsize += 1;
	    XawTextSetInsertionPoint(w, info->actsize);
	}
    } else {
	/* Lookup error - looks like return values have problems */
	ret_value = 0;
    }
    return ret_value;
}

static int ts_write_buf(w, info, len, str)
Widget w;			/* Typescript widget to add buffer     */
ts_info *info;			/* Local information                   */
int len;			/* Counted string                      */
String str;			/* Null terminated array of characters */
/*
 * Writes the counted string to the typescript widget.  Assumes no special
 * processing is required (e.g. control-H processing).
 */
{
    XawTextBlock block;
    int ret;
#ifdef DEBUG
char buf[1024];
#endif

    if ((info->bufsize - info->actsize) <= len) {
	/* Must make space */
	if (!ts_line_remove(w, info, len)) {
	    /* Remove difference */
	    (void) ts_char_remove(w, info, len);
	}
    }
    block.firstPos = 0;
    block.length = len;
    block.ptr = str;
    block.format = FMT8BIT;
#ifdef DEBUG
sprintf(buf, "Adding %d chars at pos %d giving %d total",
	len, info->actsize, info->actsize + len);
s_prompt(buf);    
#endif
    ret = XawTextReplace(w, info->actsize, info->actsize, &block);
    info->actsize += len;
    XawTextSetInsertionPoint(w, info->actsize);
    return ret;
}

/*ARGSUSED*/
int tsWriteStr(w, str)
Widget w;			/* Typescript widget to add string to  */
String str;			/* Null terminated array of characters */
/*
 * This routine adds all characters in `str' to the typescript widget `w'.
 * The semantics for adding these characters is the same as that for
 * tsWriteChar().  The return value processing for this routine isn't good.
 */
{
    ts_info *info;
    String current;
    int ret_value = 0;

    if (st_lookup(ts_table, (char *) w, (char **) &info)) {
	current = str;
	while (str && *str) {
	    if ( (str = strchr(str, BS_CHAR)) ) {
		ts_write_buf(w, info, str-current, current);
		while (*str && (*str == BS_CHAR)) {
		    ret_value = tsWriteChar(w, *str);
		    str++;
		}
		current = str;
	    } else {
		ret_value = ts_write_buf(w, info, strlen(current), current);
	    }
	}
    } else {
	/* Lookup error */
	ret_value = 0;
    }
    return ret_value;
}



/*
 * Buffer maintenence routines
 */

static int ts_line_remove(w, info, size)
Widget w;			/* asciiStringClass widget */
ts_info *info;			/* Local information       */
int size;			/* Additional characters   */
/*
 * This routine attempts to delete enough lines from the
 * string buffer for `w' to allow `size' characters to
 * be added to it.  The routine fails if the buffer has
 * infrequent line breaks.
 */
{
    char *str = (char *) 0;
    XawTextBlock block;
    int i;
#ifdef DEBUG
char buf[1024];
#endif

    if ((info->bufsize - info->actsize) > size) {
	return 1;
    }
    str = info->buf;
    for (i = 0;  i < info->actsize;  i++) {
	if (str[i] == NL_CHAR) break;
    }
    if ((i < info->actsize) && (i < MAX_LF_SEARCH)) {
	block.firstPos = 0;
	block.length = 0;
	block.ptr = (char *) 0;
	block.format = FMT8BIT;
	/* SHOULD CHECK RESULT */
#ifdef DEBUG
sprintf(buf, "Removing (0,%d) giving %d total", i+1, info->actsize-i-1);
s_prompt(buf);
#endif
	(void) XawTextReplace(w, 0, i+1, &block);
	info->actsize -= (i+1);
	return ts_line_remove(w, info, size);
    } else {
	return 0;
    }
}

static int ts_char_remove(w, info, size)
Widget w;			/* asciiStringClass widget */
ts_info *info;			/* typescript info         */
int size;			/* Additional characters   */
/*
 * This routine removes enough characters from `w' to allow `size'
 * more characters to be added.  Its normally used if no convenient
 * line break can be found.
 */
{
    int num_removed = info->actsize - info->bufsize + size;
    XawTextBlock block;
    int ret_value;

    block.firstPos = 0;
    block.length = 0;
    block.ptr = (char *) 0;
    block.format = FMT8BIT;
    ret_value = XawTextReplace(w, 0, num_removed, &block);
    info->actsize -= 1;
    return ret_value;
}


static void ts_del(w, client, call)
Widget w;
XtPointer client;
XtPointer call;
/*
 * Frees up non-widget specific information associated with
 * the typescript widget.
 */
{
    ts_info *info;
    Widget wptr = w;

    if (st_delete(ts_table, (char **) &wptr, (char **) &info)) {
	XtFree(info->buf);
	XtFree(info);
    }
}

void tsDelete(w)
Widget w;
/*
 * This routine reclaims all resources associated with the
 * typescript widget `w'.  This reclaimation is done
 * automatically if the widget is destroyed as a side effect of
 * calling XtDestroyWidget on any widget above it in the widget
 * hierarchy.
 */
{
    ts_info *info;

    if (st_lookup(ts_table, (char *) w, (char **) &info)) {
	XtDestroyWidget(w);
    } else {
	XtErrorMsg("destroyError", "tsDelete", "tsLibraryError",
		   "Widget is not a typescript", (String *) NULL,
		   (Cardinal *) NULL);
    }
}

#ifdef DEBUG


/* Debugging only */
char *s_prompt(msg)
char *msg;
{
    static char buf[128];

    printf("%s", msg);
    fflush(stdout);
    fgets(buf, 128, stdin);
    return (char *) buf;
}
#endif
