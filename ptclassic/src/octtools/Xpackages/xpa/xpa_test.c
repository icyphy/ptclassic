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
 * Program for demonstrating pull-aside menus
 */

#include "copyright.h"
#include "port.h"
#include "xpa.h"

xpa_entry deep[] = {
    { "Deep option", 'A', 0, 0 },
    { "Another", '\020', 0, 0 },
    { "Even another", 0204, 0, 0 },
    { "Quit", 'q', 0, 0 },
    { 0, 0, 0, 0 }
};

xpa_entry sub1[] = {
    { "Subpoint A", 'x', 0, 0 },
    { "Subpoint B", 'y', 0, 0 },
    { 0, 0, 0, 0 }
};

xpa_entry sub2[] = {
    { "Subvalue", '\003', 0, 0 },
    { "Level 1", 0240, "Final Level", deep },
    { "Many options", '\005', 0, 0 },
    { 0, 0, 0, 0 }
};

xpa_entry menu_spec[] = {
    { "First", 'a', "Submenu", sub1 },
    { "Second",'b', "Foobitz", sub2 },
    { "Third", XPA_NOCHAR, "Submenu", sub1 },
    { "Fourth",'d', "Foobitz", sub2 },
    { "Fifth", '\030', 0, 0 },
    { "Sixth", 'f', 0, 0 },
    { 0, 0, 0, 0 }
};


xpa_entry t2[] = {
    { "quit-selset", XPA_NOCHAR, 0, 0 },
    { 0, 0, 0, 0 }
};

xpa_entry rpc_test[] = {
    { "Selset Test", XPA_NOCHAR, 0, t2 },
    { 0, 0, 0, 0 }
};

static unsigned long res_color(disp, name, color)
Display *disp;			/* X Connection   */
char *name;			/* Color name     */
XColor *color;			/* Returned color */
/*
 * Resolves a color name into a color structure.  If
 * it fails to locate an appropriate cell,  it exits.
 * Handles `black' and `white' specially.  Returns
 * the pixel value and fills in `color' if provided.
 */
{
    XColor clr, exact;
    unsigned long black, white;
    Colormap def_cmap;

    black = BlackPixel(disp, DefaultScreen(disp));
    white = WhitePixel(disp, DefaultScreen(disp));
    def_cmap = DefaultColormap(disp, DefaultScreen(disp));
    if (strcmp(name, "black") == 0) {
	clr.pixel = black;
	XQueryColor(disp, def_cmap, &clr);
    } else if (strcmp(name, "white") == 0) {
	clr.pixel = white;
	XQueryColor(disp, def_cmap, &clr);
    } else {
	/* General color */
	if (!XAllocNamedColor(disp, def_cmap, name, &clr, &exact)) {
	    (void) printf("Cannot allocate color `%s'\n", name);
	    exit(1);
	}
    }
    if (color) *color = clr;
    return clr.pixel;
}

int funny_evt(evt)
XEvent *evt;
{
    (void) printf("Funny event: type is %lx\n", (unsigned long)evt->xany.type);
    return 1;
}

int
main()
/* Posts moded menu */
{
    Display *disp;
    Window simple;
    XEvent evt;
    xpa_menu menu;
    xpa_appearance attr;
    int rtn, i;
    int *result;
    int mask;

    disp = XOpenDisplay("");
/*    (void) XSynchronize(disp, True); */

    /* Set some display attributes */
    if (DefaultDepth(disp, DefaultScreen(disp)) > 1) {
	attr.title_fg = res_color(disp, "yellow", (XColor *) 0);
	attr.title_bg = res_color(disp, "brown", (XColor *) 0);
	attr.item_fg = res_color(disp, "NavyBlue", (XColor *) 0);
	attr.item_bg = res_color(disp, "cyan", (XColor *) 0);
	attr.cur_fg = res_color(disp, "red", (XColor *) 0);
	mask = XPA_T_FG | XPA_T_BG | XPA_I_FG | XPA_I_BG | XPA_C_FG;
    } else {
	mask = 0;
    }

    /* Create menu */
    menu = xpa_create(disp, "Top Level", menu_spec, mask, &attr);

    /* Creates a simple window */
    simple = XCreateSimpleWindow(disp, DefaultRootWindow(disp),
				 100, 100, 200, 200, 2,
				 BlackPixel(disp, DefaultScreen(disp)),
				 WhitePixel(disp, DefaultScreen(disp)));
    XSelectInput(disp, simple, ButtonPressMask);
    XMapWindow(disp, simple);

    /* Event loop */
    for (;;) {
	XNextEvent(disp, &evt);
	if (evt.xany.window == simple) {
	    switch (evt.xany.type) {
	    case ButtonPress:
		/* Post menu */
		rtn = xpa_moded(menu, evt.xbutton.x_root, evt.xbutton.y_root,
				0, (int *) 0, 0, funny_evt, &result);
		if (rtn > 0) {
		    (void) printf("Selection made:");
		    for (i = 0;  i < rtn;  i++) {
			(void) printf(" %d", result[i]);
		    }
		    (void) printf("\n");
		    if ((rtn == 3) && (result[0] == 3) &&
			(result[1] == 1) && (result[2] == 3)) {
			xpa_destroy(menu);
			XSync(disp, False);
			exit(0);
		    }
		} else if (rtn == XPA_NOGRAB) {
		    (void) printf("Can't grab mouse\n");
		} else {
		    (void) printf("No selection made\n");
		}
		break;
	    default:
		(void) printf("Unknown event\n");
		break;
	    }
	} else {
	    (void) printf("Unknown non-simple event\n");
	}
    }
    return 0;
}
