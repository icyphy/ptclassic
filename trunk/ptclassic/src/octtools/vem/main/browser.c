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
 * Oct Cell Browser
 *
 * David Harrison
 * University of California, Berkeley
 * 1990
 *
 * This is a simple oct cell browser for use with VEM.  It is not
 * intended to be the ultimate solution.  There are several limitations
 * to this approach.  It uses the simple browsing display package "fb"
 * to show icons of Oct cells and directories in a simple unmoded window.
 * Double clicking on an icon opens it.  This may open a cell in VEM
 * or open another browser on the displayed directory.
 *
 * This is very unix dependent and assumes the original version of
 * Oct is used with it.  It is a hack to some extent to try out
 * this style of interface.
 */

#include "general.h"
#include "defaults.h"
#include "utility.h"
#include "oct.h"
#include "oh.h"
#include "version.h"
#include "dds.h"
#include "vemDM.h"
#include "vemUtil.h"
#include "commands.h"
#include "cursors.h"
#include "da.h"
#include "message.h"
#include "browser.h"
#include "xvals.h"
#include <stdio.h>
#include <sys/types.h>
#ifdef USE_DIRENT_H
#include <dirent.h>
#include <unistd.h>
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

#define NEW_CLASS	"new"
#define DIR_CLASS	"directory"
#define PHY_CLASS	"physical"
#define SYM_CLASS	"symbolic"
#define SCH_CLASS	"schematic"
#define OTH_CLASS	"other"
#define POP_UP_NAME	"vemBrowser"
#define EDITOR_NAME	"editor"

static char *NewClass = NEW_CLASS;
static char *DirClass =	DIR_CLASS;
static char *PhyClass = PHY_CLASS;
static char *SymClass = SYM_CLASS;
static char *SchClass = SCH_CLASS;
static char *OthClass = OTH_CLASS;

static void Open();
#ifdef NEVER
static char *XtClassName = "Browser";
static void XtStart();
static void Dump();
static void Refresh();
#endif

static int scan_dir();
static int oct_cell();

typedef struct refresh_data_defn {
    ddsHandle main;		/* Main browser */
    Widget file_browser;	/* Browser     */
    char *dir;			/* Directory   */
    ddsHandle help;		/* Help dialog */
} refresh_data;

/*
 * Static dialog description (extra data added later)
 */

static int make_fb();
static void free_fb();

static ddsComposite top_data = { "None", DDS_VERTICAL, 10 };
static ddsExtern ext_data = { make_fb, free_fb, (ddsData) 0 };

static void br_dismiss();
static void br_refresh();

static ddsControl cons[] = {
    { "Dismiss\n(Ret)", "<Key>Return|<Key>F1", br_dismiss, (ddsData) 0 },
    { "Refresh\n(F4)", "<Key>F4", br_refresh, (ddsData) 0 }
};
#define NUM_CONS	sizeof(cons)/sizeof(ddsControl)

static char br_help[] =
"This window provides an interface for examining and opening Oct\
 cells in a directory.  The directory is shown in the title at the\
 top of the dialog.  The body of the dialog lists all of the cells\
 in the directory along with any subdirectories.  Clicking on a cell\
 or directory selects it.  Double clicking (clicking quickly twice)\
 will open the cell or directory.  Opening an Oct cell will create\
 a new VEM graphics window looking at the contents of the cell.  Opening\
 a directory will create a new browser looking at the contents of the\
 directory.  At the bottom of the dialog are three control buttons:\n\n\
 Dismiss:  Close the browser window.\n\n\
 Refresh:  The browser does not automatically track changes to the\
 directory it displays.  Clicking on this button will cause the browser\
 to go out and look at the directory again and update the cells and\
 directories shown in the dialog body.\n\n\
 Help: Displays this message";


static vemStatus browserUsage(msg)
char *msg;
{
    vemMsg(MSG_C, "Error: %s\n", msg);
    vemMsg(MSG_C, "format: { \"directory name\" } : browser\n");
    return VEM_ARGRESP;
}

vemStatus browserCmd(spot, cmdList)
vemPoint *spot;			/* Where command was issued */
lsList cmdList;			/* List of arguments        */
/*
 * This command creates a new cell browser.  It takes one
 * optional string argument that specifies the directory
 * to scan.  If not supplied, the current working directory
 * is used.
 */
{
    vemOneArg *oneArg;
    struct stat buf;
    char *dir_name;

    switch (lsLength(cmdList)) {
    case 0:
	vemBrowser((Window) 0, ".");
	break;
    case 1:
	if (lsFirstItem(cmdList, (lsGeneric *) &oneArg, LS_NH) == LS_OK) {
	    if (oneArg && (oneArg->argType == VEM_TEXT_ARG)) {
		dir_name = util_file_expand(oneArg->argData.vemTextArg);
		if ((stat(dir_name, &buf) == 0) &&
		    ((buf.st_mode & S_IFMT) == S_IFDIR)) {
		    vemBrowser((Window) 0, dir_name);
		} else {
		    return browserUsage("Path specified is not a directory");
		}
	    } else {
		return browserUsage("Argument not text");
	    }
	}
	break;
    default:
	return browserUsage("Too many arguments");
    }
    return VEM_OK;
}

void vemBrowser(win, dir)
Window win;
char *dir;
/*
 * Creates and posts a new browser window relative to the window `win'
 * (if non-zero) looking at items in the directory `dir'.
 */
{
    ddsHandle body, cb;
    ddsPosition pos;
    char full_path[MAX_NAME], title_text[MAX_NAME*2];
    refresh_data *rd;
    int i;
#ifndef SYSV
    extern char *getwd();
#endif

    rd = ALLOC(refresh_data, 1);
#ifdef SYSV
    if (strcmp(dir, ".") == 0) dir = getcwd(full_path, MAX_NAME);
#else
    if (strcmp(dir, ".") == 0) dir = getwd(full_path);
#endif
    rd->dir = XtNewString(dir);
    (void) sprintf(title_text, "VEM %s: %s", vemVerString(),
		   util_tilde_compress(dir));
    top_data.title = title_text;
    rd->main = dds_component(0, DDS_TOP, (ddsData) &top_data);
    ext_data.user_data = (ddsData) rd;
    body = dds_component(rd->main, DDS_EXTERN, (ddsData) &ext_data);
    for (i = 0;  i < NUM_CONS;  i++) {
	cons[i].user_data = (ddsData) rd;
	cb = dds_component(rd->main, DDS_CONTROL, (ddsData) (cons+i));
    }
    rd->help = dmHelpButton(rd->main, br_help);
    if (win) {
	pos.pos = DDS_WINDOW; pos.vjust = DDS_BELOW; pos.hjust = DDS_CENTER;
	pos.win = win;
    } else {
	pos.pos = DDS_MOUSE; pos.vjust = DDS_MIDDLE; pos.hjust = DDS_CENTER;
    }
    dds_post(rd->main, &pos, (ddsEvtHandler *) 0);
}

#define CLASS_ORDER(cls) if (class_name == cls) return val; val++

static int class_order(class_name)
char *class_name;
{
    int val = 0;

    CLASS_ORDER(NewClass);
    CLASS_ORDER(DirClass);
    CLASS_ORDER(PhyClass);
    CLASS_ORDER(SymClass);
    CLASS_ORDER(SchClass);
    CLASS_ORDER(OthClass);
    return val;
}

static int comp(a, b)
char *a, *b;
/* The order is: physical, symbolic, schematic, others, directories */
{
    fb_item *one = (fb_item *) a;
    fb_item *two = (fb_item *) b;

    if (one->class == two->class) {
	return strcmp(one->name, two->name);
    } else {
	return class_order(one->class) - class_order(two->class);
    }
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
#ifdef USE_DIRENT_H
    struct dirent *entry;
#else
    struct direct *entry;
#endif
    struct stat buf;
    char full_name[MAX_NAME];
    dary all;
    fb_item one;
    int num_items;

    all = daAlloc(fb_item, EST_DIR_SIZE);
    /* Set up the new cell */
    one.class = NewClass;
    one.name = "New Cell...";
    one.data = (caddr_t) 0;
    *daSetLast(fb_item, all) = one;
    /* Set up the rest of the cells */
    if ( (dp = opendir(dir)) ) {
	while ( (entry = readdir(dp)) ) {
	    if (entry->d_name[0] != '.') {
		(void) sprintf(full_name, "%s/%s", dir, entry->d_name);
		if (stat(full_name, &buf) == 0) {
		    if ((buf.st_mode & S_IFMT) == S_IFDIR) {
			if (!oct_cell(full_name, entry->d_name, all)) {
			    one.class = DirClass;
			    one.name = XtNewString(entry->d_name);
			    one.data = (caddr_t) 0;
			    *daSetLast(fb_item, all) = one;
			}
		    }
		}
	    }
	}
	closedir(dp);
    }
    /* Sort the list by class and name */
    daSort(all, comp);
    num_items = daLen(all);
    *items = daCopyData(fb_item, all);
    daFree(all, (void (*)()) 0);
    return num_items;
}


static int oct_cell(path, short_name, all)
char *path;			/* Directory to examine */
char *short_name;		/* Short form for name  */
dary all;			/* Complete list        */
/*
 * Examines `path' to determine whether it is one or more oct cells.  If so,
 * it sets the class and names for the cells appropriately and adds them
 * into the item list `all'.  Oct cells are determined by looking down
 * two levels in the directory heirarchy and then calling octGetFacetInfo
 * on files at that level.  It also looks one more level down to see if
 * there are any valid cells.  If so, it returns a zero status indicating
 * that this both an oct cell and a directory.  Normally, if it finds 
 * valid oct cells and no other cells beneath it, it returns a non-zero
 * status.
 */
{
    DIR *top, *lv;
#ifdef USE_DIRENT_H
    struct dirent *one, *two;
#else
    struct direct *one, *two;
#endif
    struct stat buf;
    char full_name[MAX_NAME];
    char disp_name[MAX_NAME];
    char fct_name[MAX_NAME];
    char big_name[MAX_NAME];
    fb_item one_item;
    char *p;
    octObject fct;
    struct octFacetInfo info;
    int cell_count = 0;
    int sub_cell_p = 0;

    fct.type = OCT_FACET;
    fct.objectId = oct_null_id;
    fct.contents.facet.cell = path;
    fct.contents.facet.version = OCT_CURRENT_VERSION;
    fct.contents.facet.mode = "r";
    if ( (top = opendir(path)) ) {
	while ( (one = readdir(top)) ) {
	    (void) sprintf(full_name, "%s/%s", path, one->d_name);
	    if ((one->d_name[0] != '.') &&
		(stat(full_name, &buf) == 0) &&
		((buf.st_mode & S_IFMT) == S_IFDIR)) {
		fct.contents.facet.view = one->d_name;
		if ( (lv = opendir(full_name)) ) {
		    while (( two = readdir(lv)) ) {
			(void) strcpy(fct_name, two->d_name);
			if ( (p = strchr(fct_name, ';')) ) {
			    *p = '\0';
			    fct.contents.facet.facet = fct_name;
			    fct.contents.facet.version = p+1;
			    if ((two->d_name[0] != '.') &&
				(octGetFacetInfo(&fct, &info) == OCT_OK)) {
				if (strlen(fct.contents.facet.version)) {
				    (void) sprintf(disp_name, "%s:%s:%s;%s",
						   short_name, one->d_name,
						   fct_name,
						   fct.contents.facet.version);
				} else {
				    (void) sprintf(disp_name, "%s:%s:%s",
						   short_name, one->d_name,
						   fct_name);
				}
				one_item.name = XtNewString(disp_name);
				if (strcmp(one->d_name, "physical") == 0) {
				    one_item.class = PhyClass;
				} else if (strcmp(one->d_name, "symbolic") == 0) {
				    one_item.class = SymClass;
				} else if (strcmp(one->d_name, "schematic") == 0) {
				    one_item.class = SchClass;
				} else {
				    one_item.class = OthClass;
				}
				one_item.data = (caddr_t) 0;
				*daSetLast(fb_item, all) = one_item;
				cell_count++;
			    }
			} else if (two->d_name[0] != '.') {
			    (void) sprintf(big_name, "%s/%s", full_name,
					   two->d_name);
			    if ((stat(big_name, &buf) == 0) &&
				((buf.st_mode & S_IFMT) == S_IFDIR)) {
				sub_cell_p = 1;
			    }
			}
		    }
		    closedir(lv);
		}
	    }
	}
	closedir(top);
    }
    if (cell_count && !sub_cell_p) return 1;
    else return 0;
}


static int make_fb(parent, cmps, vspan, user_data)
ddsData parent;			/* Parent widget          */
ddsData **cmps;			/* Returned widget list   */
int *vspan;			/* Returned vertical span */
ddsData user_data;		/* Actually (refresh_data *) */
/*
 * Makes a new file browser component under `parent' and returns
 * the widget in `cmps'.  Sets up the appropriate callback functions
 * for opening the items shown in the body.
 */
{
    Widget pw = (Widget) parent;
    Widget **wl = (Widget **) cmps;
    refresh_data *info = (refresh_data *) user_data;
    int num_items;
    fb_item *item_list;

    num_items = scan_dir(info->dir, &item_list);
    info->file_browser = fb_create(pw, num_items, item_list, Open,
				   (caddr_t) user_data);
    (*wl) = ALLOC(Widget, 1);
    (*wl)[0] = info->file_browser;
    FREE(item_list);
    *vspan = 1;
    return 1;
}

/*ARGSUSED*/
static void free_fb(nm, list)
int nm;
ddsData *list;
/*
 * Frees up list allocated in make_fb().
 */
{
    FREE(list);
}




static dmTextItem new_cell_fields[] = {
    { "Cell Name", 1, 40, "sample", (Pointer) 0 },
    { "View Name", 1, 40, "", (Pointer) 0 },
    { "Facet Name", 1, 40, "contents", (Pointer) 0 }
};
#define NUM_FIELDS	sizeof(new_cell_fields)/sizeof(dmTextItem)

static void Open(class, label, item_data, global_data)
String class;			/* Class of item */
String label;			/* Label of item */
caddr_t item_data;		/* User data     */
caddr_t global_data;		/* Global data   */
/*
 * This is called when an item should be opened.  For directories,
 * it pops up a new browser.  For oct cells, it opens a new
 * VEM window.  For new cells, it asks the user for the name
 * and then opens the cell.
 */
{
    char full_name[MAX_NAME];
    char cell[MAX_NAME], view[MAX_NAME], facet[MAX_NAME];
    octObject fct;
    char *viewtype;
    Window win;
    refresh_data *info = (refresh_data *) global_data;

    vemBusy();
    (void) sprintf(full_name, "%s/%s", info->dir, label);
    if (strcmp(class, DirClass) == 0) {
	/* Regular directory */
	vemBrowser(dds_window(info->main), XtNewString(full_name));
    } else if (strcmp(class, NewClass) == 0) {
	/* New cell */
	dfGetString("viewtype", &viewtype);
	new_cell_fields[0].value = "sample";
	new_cell_fields[1].value = viewtype;
	new_cell_fields[2].value = "contents";
	if (dmMultiText("New Cell", NUM_FIELDS, new_cell_fields) == VEM_OK) {
	    fct.type = OCT_FACET;
	    strcpy(cell, new_cell_fields[0].value);
	    strcpy(view, new_cell_fields[1].value);
	    strcpy(facet, new_cell_fields[2].value);
	    fct.contents.facet.cell = cell;
	    fct.contents.facet.view = view;
	    fct.contents.facet.facet = facet;
	    fct.contents.facet.version = OCT_CURRENT_VERSION;
	    fct.contents.facet.mode = "a";
	    if (vuOpenWindow(&win, "New VEM Window", &fct, (STR) 0) != VEM_OK) {
    		(void) dmConfirm("Open Failed", octErrorString(), "Dismiss", (STR) 0);
	    }
	}
    } else {
	/* Oct cell */
	fct.type = OCT_FACET;
	fct.contents.facet.version = OCT_CURRENT_VERSION;
	fct.contents.facet.mode = "a";
	if (ohUnpackFacetName(&fct, full_name) == OCT_OK) {
	    if (vuOpenWindow(&win, "New VEM Window", &fct, (STR) 0) != VEM_OK) { 	    
    		(void) dmConfirm("Open Failed", octErrorString(), "Dismiss", (STR) 0);
	    }
	}
    }
}


static void br_dismiss(item)
ddsHandle item;			/* Control button */
/*
 * Called when the user presses the `dismiss' button in the
 * browser.
 */
{
    ddsControl con_data;
    refresh_data *info;

    dds_get(item, (ddsData) &con_data);
    info = (refresh_data *) con_data.user_data;
    FREE(info->dir);
    if (info->help) dds_destroy(info->help);
    dds_destroy(item);
    FREE(info);
}



static void br_refresh(item)
ddsHandle item;			/* Control button */
/*
 * Called when the user presses the `refresh' button in the
 * browser.
 */
{
    ddsControl con_data;
    refresh_data *info;
    int nm;
    fb_item *items;

    dds_get(item, (ddsData) &con_data);
    info = (refresh_data *) con_data.user_data;
    nm = scan_dir(info->dir, &items);
    fb_update(info->file_browser, nm, items);
    FREE(items);
}


#ifdef TEST

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
    XtPopup(vemBrowser(topw, "."), XtGrabNone);
}

char *vemVerString()
/*
 * For testing only
 */
{
    return "VEM 8-2";
}

#endif
