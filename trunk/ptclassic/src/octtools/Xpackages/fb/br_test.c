#ifndef lint
static char SccsId[]="$Id$";
#endif /*lint*/
/* Copyright (c) 1990-1994 The Regents of the University of California.
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
 * Sample program showing how to use the browser
 */

#include <stdio.h>
#include <sys/types.h>
#include "port.h"
#include "ansi.h"
#include "errtrap.h"
#ifdef SYSV
#include <dirent.h>
#else
#include <sys/dir.h>
#endif
#include <sys/stat.h>

#include "fb.h"

#include <X11/StringDefs.h>
#include <X11/Shell.h>
#include <X11/Xaw/AsciiText.h>
#include <X11/Xaw/Paned.h>
#include <X11/Xaw/Box.h>
#include <X11/Xaw/Command.h>

#define MAX_ARGS	10
#define MAX_NAME	1024
#define EST_DIR_SIZE	40

#define DIR_CLASS	"directory"
#define FILE_CLASS	"file"
#define POP_UP_NAME	"main"
#define EDITOR_NAME	"editor"

static char *DirClass =	DIR_CLASS;
static char *FileClass = FILE_CLASS;

static char *XtClassName = "Browser";
static int scan_dir
	ARGS((char *dir, fb_item **items));

static void XtStart();
static Widget Top;
static Widget Browser();
static void Open();
static void Dump();
static void Refresh();

static int PopupCount = 0;

typedef struct refresh_data_defn {
    Widget file_browser;	/* Browser   */
    char *dir;			/* Directory */
} refresh_data;

void dump_core(msg)
String msg;
{
    (void) fprintf(stderr, "Fatal toolkit error: %s\n", msg);
    abort();
}

int xlib_core(disp, evt)
Display *disp;
XErrorEvent *evt;
{
    char err_buf[1024], prot_msg[1024];
    char proto[1024];

    XGetErrorText(disp, evt->error_code, err_buf, 1024);
    (void) sprintf(proto, "XRequest.%d", (int) (evt->request_code));
    XGetErrorDatabaseText(disp, "", proto, "unknown", prot_msg, 1024);
    (void) fprintf(stderr, "Fatal X error: %s (%s)", err_buf, prot_msg);
    abort();
}

int
main(argc, argv)
int argc;
char *argv[];
{
    Widget top_level_widget;
    XtAppContext context;
    Display *disp;
    int arg_len;
    Arg arg_list[MAX_ARGS];

    errCore(1);
    XtSetErrorHandler(dump_core);
    (void) XSetErrorHandler(xlib_core);
    XtToolkitInitialize();
    context = XtCreateApplicationContext();
    disp = XtOpenDisplay(context, "", argv[0], XtClassName,
			 (XrmOptionDescRec *) 0, 0,
			 &argc, argv);
#ifdef NOTDEF
    (void) XSynchronize(disp, True);
#endif
    arg_len = 0;
    XtSetArg(arg_list[arg_len], XtNallowShellResize, True);	arg_len++;
    XtSetArg(arg_list[arg_len], XtNinput, True);		arg_len++;
    top_level_widget = XtAppCreateShell(argv[0], XtClassName,
					applicationShellWidgetClass,
					disp, arg_list, arg_len);
    XtStart(argc, argv, top_level_widget);
    XtAppMainLoop(context);
    return 0;
}

static void XtStart(argc, argv, topw)
int argc;
char *argv[];
Widget topw;
{
    Top = topw;
    XtPopup(Browser(topw, "."), XtGrabNone);
    PopupCount++;
}

static int comp(a, b)
char *a, *b;
/* Directories are ahead of files, then sorted by name */
{
    fb_item *one = (fb_item *) a;
    fb_item *two = (fb_item *) b;

    if (one->class == two->class) {
	return strcmp(one->name, two->name);
    } else if (one->class == DirClass) {
	return -1;
    } else {
	return 1;
    }
}

static Widget Browser(parent, dir)
Widget parent;
char *dir;
{
    Widget pop_up, pane, box, close, refresh, files;
    int num_items;
    fb_item *items;
    refresh_data *rd;

    num_items = scan_dir(dir, &items);
    pop_up = XtCreatePopupShell(POP_UP_NAME, topLevelShellWidgetClass,
				parent, (ArgList) 0, 0);
    pane = XtCreateManagedWidget("pane", panedWidgetClass,
				 pop_up, (ArgList) 0, 0);
    box = XtCreateManagedWidget("box", boxWidgetClass,
				pane, (ArgList) 0, 0);
    close = XtCreateManagedWidget("close", commandWidgetClass,
				  box, (ArgList) 0, 0);
    XtAddCallback(close, XtNcallback, Dump, (caddr_t) pop_up);
    refresh = XtCreateManagedWidget("refresh", commandWidgetClass,
				    box, (ArgList) 0, 0);
    files = fb_create(pane, num_items, items, Open, (caddr_t) dir);
    rd = (refresh_data *) XtMalloc(sizeof(refresh_data));
    rd->file_browser = files;
    rd->dir = dir;
    XtAddCallback(refresh, XtNcallback, Refresh, (caddr_t) rd);
    XtFree((char *) items);
    return pop_up;
}


static int scan_dir(dir, items)
char *dir;			/* Directory to search    */
fb_item **items;		/* Returned list of items */
/*
 * Scans the specified directory and returns a list of items
 * for that directory.
 */
{
    DIR *dp;
#ifdef SYSV
    struct dirent *entry;
#else
    struct direct *entry;
#endif
    struct stat buf;
    char full_name[MAX_NAME];
    int num_alloc, num_items;

    num_alloc = EST_DIR_SIZE;
    num_items = 0;
    (*items) = (fb_item *) XtCalloc(num_alloc, sizeof(fb_item));
    if ( (dp = opendir(dir)) ) {
	while ( (entry = readdir(dp)) ) {
	    if (entry->d_name[0] != '.') {
		(void) sprintf(full_name, "%s/%s", dir,
			       (char *)(entry->d_name));
		if (stat(full_name, &buf) == 0) {
		    if (num_items >= num_alloc) {
			num_alloc *= 2;
			(*items) = (fb_item *)
			  XtRealloc((char *)(*items),
				    num_alloc*sizeof(fb_item));
		    }
		    if ((buf.st_mode & S_IFMT) == S_IFDIR) {
			(*items)[num_items].class = DirClass;
		    } else {
			(*items)[num_items].class = FileClass;
		    }
		    (*items)[num_items].name = XtNewString(entry->d_name);
		    (*items)[num_items].data = (caddr_t) 0;
		    num_items++;
		}
	    }
	}
	closedir(dp);
    }
    /* Sort the list by file name */
    qsort((*items), num_items, sizeof(fb_item), comp);

    return num_items;
}



static void Open(class, label, item_data, global_data)
String class;			/* Class of item */
String label;			/* Label of item */
caddr_t item_data;		/* User data     */
caddr_t global_data;		/* Global data   */
/*
 * This is called when an item should be opened.  For directories,
 * it pops up a new browser.  For files, it spawns off an ascii text
 * widget to look at the file.
 */
{
    int arg_len;
    Arg arg_list[MAX_ARGS];
    Widget pop_up, vpane, sub_box, close;
    char full_name[MAX_NAME];

    (void) sprintf(full_name, "%s/%s", global_data, label);
    if (strcmp(class, DirClass) == 0) {
	XtPopup(Browser(Top, XtNewString(full_name)), XtGrabNone);
	PopupCount++;
    } else {
	arg_len = 0;
	XtSetArg(arg_list[arg_len], XtNtype, XawAsciiFile);  arg_len++;
	XtSetArg(arg_list[arg_len], XtNstring, full_name);   arg_len++;
	pop_up = XtCreateManagedWidget(full_name, topLevelShellWidgetClass,
				       Top, arg_list, arg_len);
	vpane = XtCreateManagedWidget("pane", panedWidgetClass,
				      pop_up, arg_list, arg_len);
	sub_box = XtCreateManagedWidget("box", boxWidgetClass,
					vpane, arg_list, arg_len);
	close = XtCreateManagedWidget("close", commandWidgetClass,
				      sub_box, arg_list, arg_len);
	XtAddCallback(close, XtNcallback, Dump, (caddr_t) pop_up);
	(void) XtCreateManagedWidget(EDITOR_NAME, asciiTextWidgetClass,
				     vpane, arg_list, arg_len);
	XtPopup(pop_up, XtGrabNone);
	PopupCount++;
    }
}


static void Dump(w, client_data, call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
    XtDestroyWidget((Widget) client_data);
    PopupCount--;
    if (PopupCount <= 0) exit(0);
}


static void Refresh(w, client_data, call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
    int nm;
    fb_item *items;
    refresh_data *rd = (refresh_data *) client_data;

    nm = scan_dir(rd->dir, &items);
    fb_update(rd->file_browser, nm, items);
    XtFree((char *) items);
}
