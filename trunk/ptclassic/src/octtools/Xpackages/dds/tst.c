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
 * Simple test program for dds
 */

#include "dds.h"
#include "utility.h"

#include <stdio.h>
#include <sys/types.h>
#ifdef SYSV
#include <dirent.h>
#else
#include <sys/dir.h>
#endif
#include <sys/stat.h>

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include "../fb/LImage.h"

#define MAX_NAME	1024
#define EST_DIR_SIZE	20

static char *DirClass = "Dir";
static char *FileClass = "File";

typedef struct fb_item_defn {
    char *class;		/* Defines the image          */
    char *name;			/* Name displayed under image */
    caddr_t data;		/* User specific data         */
} fb_item;

static void quit();
static void press();
static ddsHandle build_files();
static int scan_dir();

static ddsComposite top_data = { "Sample Dialog", DDS_VERTICAL, 10 };
static ddsEditText first = { "Name", "<name>", 1, 20 };
static ddsEditText second = { (char *) 0, "<value>", 5, 20 };
static ddsEditText third = { "Final", "", 3, 25 };
static ddsControl control = { "Ok", "Meta<Key>g|<Key>F1|<Key>KP_F1|Meta<Key>.", quit, (ddsData) 0 };
static ddsPosition pos = { DDS_ABSOLUTE, DDS_MIDDLE, DDS_CENTER, 100, 100, 0 };

static ddsFlagItem items[] = {
    { "One", DDS_FALSE } , { "Two", DDS_FALSE }, { "Three", DDS_FALSE },
    { "Four", DDS_FALSE }, { "Five", DDS_FALSE }
};
#define NUM_ITEMS sizeof(items)/sizeof(ddsFlagItem)

static ddsItemList fourth = {
    NUM_ITEMS, items, DDS_FALSE, -1, DDS_FALSE, 2, press, (ddsData) 0
};

static char *prime_list[] = {
    "One", "Two", "Three", "Five", "Seven\nand a half", "Eleven", "Thirteen",
    "Seventeen", "Ninteen", "Twenty-Three"
};

static ddsEnumerate pr_data = {
    "Prime Number", sizeof(prime_list)/sizeof(char *), prime_list,
    0, 0, 0
};

static ddsShowText stxt_data = {
    DDS_FROM_FILE, 15, 40, "sample.doc", DDS_TRUE, DDS_TRUE
};
    
static ddsNumber hours_data = {
    "Hours", 1.0, 24.0, 1.0, 8.0, "%2.0lf", 0, 0
};

static ddsNumber minutes_data = {
    "Minutes", 0.0, 60.0, 0.1, 5.0, "%2.1lf", 0, 0
};

static int make_limage();
static void free_limage();
typedef struct limage_data_defn {
    char *class;
    char *name;
} limage_data;
static limage_data li_data = { "file", "Foobar" };
static ddsExtern ext_data = { make_limage, free_limage, (ddsData) &li_data };

static ddsHandle top, name, value, final, multi, files, prime, done, motd;
static ddsHandle hrs, mins, image;

static void do_post();

#define MM 1024

static int x_errors(disp, evt)
Display *disp;
XErrorEvent *evt;
{
    char message[MM];
    char number[MM];
    char request[MM];

    XGetErrorText(disp, evt->error_code, message, MM);
    (void) sprintf(number, "%d", evt->request_code);
    XGetErrorDatabaseText(disp, "XRequest", number, number, request, MM);

    (void) fprintf(stderr, "Fatal X Error: %s\n", message);
    (void) fprintf(stderr, "               Request %s (minor code %d)\n",
		   request, evt->minor_code);
    abort();
    return -1;
}

int
main(argc, argv)
int argc;
char *argv[];
{
    errCore(1);
    XSetErrorHandler(x_errors);
    dds_initialize(&argc, argv);
    top = dds_component(0, DDS_TOP, (ddsData) &top_data);
    name = dds_component(top, DDS_EDIT_TEXT, (ddsData) &first);
    value = dds_component(top, DDS_EDIT_TEXT, (ddsData) &second);
    final = dds_component(top, DDS_EDIT_TEXT, (ddsData) &third);
    multi = dds_component(top, DDS_ITEM_LIST, (ddsData) &fourth);
    done = dds_component(top, DDS_CONTROL, (ddsData) &control);
    files = build_files(top, ".");
    prime = dds_component(top, DDS_ENUMERATE, (ddsData) &pr_data);
    hrs = dds_component(top, DDS_NUMBER, (ddsData) &hours_data);
    mins = dds_component(top, DDS_NUMBER, (ddsData) &minutes_data);
    motd = dds_component(top, DDS_SHOW_TEXT, (ddsData) &stxt_data);
    image = dds_component(top, DDS_EXTERN, (ddsData) &ext_data);
    do_post(top);
    dds_loop((ddsEvtHandler *) 0);
    return 0;
}

struct pos_data {
    ddsHandle post;
    ddsPosition pos;
    ddsHandle q_xloc;		/* X Location          */
    ddsHandle q_yloc;		/* Y Location          */
};
typedef struct pos_data PosData;

static void change_pos(item, previous)
ddsHandle item;
int previous;
{
    ddsEnumerate val;
    PosData *data;

    dds_get(item, (ddsData) &val);
    data = (PosData *) val.user_data;
    switch (val.selected) {
    case 0: data->pos.pos = DDS_ABSOLUTE; break;
    case 1: data->pos.pos = DDS_MOUSE; break;
    case 2: data->pos.pos = DDS_WINDOW; break;
    }
}

static void change_vjust(item, previous)
ddsHandle item;
int previous;
{
    ddsEnumerate val;
    PosData *data;

    dds_get(item, (ddsData) &val);
    data = (PosData *) val.user_data;
    switch (val.selected) {
    case 0: data->pos.vjust = DDS_ABOVE; break;
    case 1: data->pos.vjust = DDS_MIDDLE; break;
    case 2: data->pos.vjust = DDS_BELOW; break;
    }
}

static void change_hjust(item, previous)
ddsHandle item;
int previous;
{
    ddsEnumerate val;
    PosData *data;

    dds_get(item, (ddsData) &val);
    data = (PosData *) val.user_data;
    switch (val.selected) {
    case 0: data->pos.hjust = DDS_LEFT; break;
    case 1: data->pos.hjust = DDS_CENTER; break;
    case 2: data->pos.hjust = DDS_RIGHT; break;
    }
}

static int cant_handle(evt, data)
XEvent *evt;
ddsData data;
{
    printf("Bad event of type %d\n", evt->type);
    return 0;
}

static void post_it(item)
ddsHandle item;
{
    static ddsEvtHandler null_handler = { cant_handle, 0 };
    ddsControl con;
    ddsEditText val;
    PosData *data;

    dds_get(item, (ddsData) &con);
    data = (PosData *) con.user_data;
    dds_get(data->q_xloc, (ddsData) &val);
    data->pos.x = atol(val.text);
    dds_get(data->q_yloc, (ddsData) &val);
    data->pos.y = atol(val.text);
    dds_post(data->post, &data->pos, &null_handler);
}

static void all_done(item)
ddsHandle item;
{
    exit(0);
}

static void do_post(comp)
ddsHandle comp;
/*
 * Builds and posts an unmoded dialog asking how to post `comp'.
 */
{
    static PosData data;
    static ddsComposite td = { "Position for Dialog", DDS_VERTICAL, 10 };
    static char *types[] = { "DDS_ABSOLUTE", "DDS_MOUSE", "DDS_WINDOW" };
    static ddsEnumerate pd = { "Type", sizeof(types)/sizeof(char *),
				 types, 1, change_pos, (ddsData) &data };
    static char *vjs[] = { "DDS_ABOVE", "DDS_MIDDLE", "DDS_BELOW" };
    static ddsEnumerate vj = { "Vertical Justification",
				 sizeof(vjs)/sizeof(char *),
				 vjs, 1, change_vjust, (ddsData) &data };
    static char *hjs[] = { "DDS_LEFT", "DDS_CENTER", "DDS_RIGHT" };
    static ddsEnumerate hj = { "Vertical Justification",
				 sizeof(hjs)/sizeof(char *),
				 hjs, 1, change_hjust, (ddsData) &data };
    static ddsEditText xv = { "X Location", "0", 1, 20 };
    static ddsEditText yv = { "Y Location", "0", 1, 20 };
    static ddsControl con = { "Post Dialog", "Meta<Key>g|<Key>F1|<Key>KP_F1|Meta<Key>.", post_it, (ddsData) &data };
    static ddsControl con2 = { "Done", (char *) 0, all_done, (ddsData) &data };
    static ddsPosition pos = { DDS_MOUSE, DDS_MIDDLE, DDS_CENTER, 0, 0, 0 };

    ddsHandle top, item;

    data.post = comp;
    data.pos.pos = DDS_MOUSE;
    data.pos.vjust = DDS_MIDDLE;
    data.pos.hjust = DDS_CENTER;
    top = dds_component(0, DDS_TOP, (ddsData) &td);
    item = dds_component(top, DDS_ENUMERATE, (ddsData) &pd);
    item = dds_component(top, DDS_ENUMERATE, (ddsData) &vj);
    item = dds_component(top, DDS_ENUMERATE, (ddsData) &hj);
    data.q_xloc = dds_component(top, DDS_EDIT_TEXT, (ddsData) &xv);
    data.q_yloc = dds_component(top, DDS_EDIT_TEXT, (ddsData) &yv);
    item = dds_component(top, DDS_CONTROL, (ddsData) &con);
    item = dds_component(top, DDS_CONTROL, (ddsData) &con2);
    dds_post(top, &pos, 0);
    data.pos.win = dds_window(top);
}


static ddsHandle build_files(top, dir)
ddsHandle top;			/* Top component */
char *dir;			/* Directory     */
{
    int nf;
    fb_item *fl;
    ddsItemList list;
    int i;
    ddsHandle result;

    nf = scan_dir(dir, &fl);
    list.num_items = nf;
    list.items = ALLOC(ddsFlagItem, nf);
    for (i = 0;  i < nf;  i++) {
	list.items[i].label = fl[i].name;
	list.items[i].selected_p = DDS_FALSE;
    }
    list.exclusive_p = DDS_TRUE;
    list.selected = -1;
    list.scroll_p = DDS_TRUE;
    list.max_items = 4;
    list.callback = 0;
    list.user_data = 0;
    result = dds_component(top, DDS_ITEM_LIST, (ddsData) &list);
    FREE(list.items);
    FREE(fl);
    return result;
}

static void quit(item)
ddsHandle item;
{
    ddsEditText data;
    ddsItemList md;
    ddsEnumerate en;
    int i;

    dds_get(name, (ddsData) &data);
    printf("Name is `%s'\n", data.text);
    dds_get(value, (ddsData) &data);
    printf("Value is `%s'\n", data.text);
    dds_get(final, (ddsData) &data);
    printf("Final is `%s'\n", data.text);
    dds_get(multi, (ddsData) &md);
    for (i = 0;  i < md.num_items;  i++) {
	printf("%s is %s\n", md.items[i].label,
	       ((md.items[i].selected_p == DDS_TRUE) ? "DDS_TRUE" : "DDS_FALSE"));
    }
    dds_get(prime, (ddsData) &en);
    printf("Enumerated value is `%s'\n", en.items[en.selected]);
    fflush(stdout);
    dds_unpost(item);
}

static void press(item, thing)
ddsHandle item;
int thing;
{
    char buf[1024];
    ddsEditText data;
    ddsItemList idata;

    dds_get(item, (ddsData) &idata);
    sprintf(buf, "State of `%s' has changed to `%s'\n",
	    idata.items[thing].label, (idata.items[thing].selected_p == DDS_TRUE ?
			   "DDS_TRUE" : "DDS_FALSE"));
    dds_get(final, (ddsData) &data);
    data.text = buf;
    dds_set(final, (ddsData) &data);
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
		(void) sprintf(full_name, "%s/%s", dir, entry->d_name);
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

static int make_limage(parent, cmps, vspan, user_data)
ddsData parent;			/* Actually (Widget)      */
ddsData **cmps;			/* Actually (Widget **)   */
int *vspan;			/* Returned vertical span */
ddsData user_data;		/* Actually (limage_data *) */
/*
 * This routine creates an limage widget to show that foreign
 * widgets can be encorporated using the DDS_EXTERN component.
 */
{
    Widget pw = (Widget) parent;
    Widget **wl = (Widget **) cmps;
    limage_data *info = (limage_data *) user_data;

    (*wl) = ALLOC(Widget, 1);
    (*wl)[0] = XtVaCreateManagedWidget(info->class, lImageWidgetClass, pw,
				       XtNlabel, info->name, NULL);
    *vspan = 2;
    return 1;
}

static void free_limage(num, cmps)
int num;
ddsData *cmps;
/*
 * Frees the list.
 */
{
    FREE(cmps);
}
