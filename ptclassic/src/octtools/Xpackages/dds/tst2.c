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
#include "dds.h"

ddsHandle init, value;
void done();
void cancel();
int evt();

ddsComposite top_data = { "Dialog Box", DDS_VERTICAL, 10 };
ddsEditText ed_data = { "String", "<default>", 3, 40 };
ddsControl done_data = { "Done", done, 0 };
ddsControl cancel_data = { "Cancel", cancel, 0 };
ddsPosition where = { DDS_MOUSE, DDS_MIDDLE, DDS_CENTER, 0, 0, 0 };
ddsEvtHandler ignore = { evt, 0 };

main(argc, argv)
int argc, argv[];
{
    ddsHandle top, btn;

    init = dds_initialize(&argc, argv);
    top = dds_component(init, DDS_TOP, (ddsData) &top_data);
    value = dds_component(top, DDS_EDIT_TEXT, (ddsData) &ed_data);
    btn = dds_component(top, DDS_CONTROL, &done_data);
    btn = dds_component(top, DDS_CONTROL, &cancel_data);
    dds_post(init, top, &where, &ignore);
    dds_loop(init, &ignore);
}

void done(item)
ddsHandle item;
{
    ddsEditText data;

    dds_get(value, (ddsData) &data);
    printf("String is `%s'\n", data.text);
    exit(0);
}

void cancel(item)
ddsHandle item;
{
    static char *sample[] = { "One", "Two", "Three", "Four", 0 };
    
    printf("Selected is %d\n", which_one(sample));
    exit(0);
}

int evt(xevt, data)
XEvent *evt;
ddsData data;
{
    return 0;
}

/* ------- */

void finish(item)
ddsHandle item;
{
    dds_unpost(item);
}				

int other(evt, data)
XEvent *evt;
ddsData data;
{
    return 0;
}

int which_one(items)
char **items;			/* Null terminated array */
{
    static ddsComposite m_attr = { "Which One?", DDS_VERTICAL, 3 };
    static ddsEvtHandler h_evt = { other, 0 };
    ddsFlagItem *flag_items;
    ddsHandle top, it, btn;
    ddsItemList flag;
    ddsControl control;
    ddsPosition pos;
    char **idx;
    int i;

    top = dds_component(init, DDS_TOP, (ddsData) &m_attr);
    for (idx = items;  *idx;  idx++) {
	/* Null body -- counts items */
    }
    flag_items = ALLOC(ddsFlagItem, (idx - items)+1);
    for (i = 0;  i < (idx-items);  i++) {
	flag_items[i].label = items[i];
	flag_items[i].selected_p = DDS_FALSE;
    }
    flag.num_items = (idx - items);
    flag.items = flag_items;
    flag.exclusive_p = DDS_TRUE;
    flag.selected = -1;		/* None selected */
    flag.scroll_p = DDS_TRUE;
    flag.max_items = 10;
    flag.callback = 0; flag.user_data = 0;
    it = dds_component(top, DDS_ITEM_LIST, (ddsData) &flag);
    control.name = "Ok";  control.callback = finish; control.user_data = 0;
    btn = dds_component(top, DDS_CONTROL, (ddsData) &control);
    pos.pos = DDS_MOUSE;  pos.vjust = DDS_MIDDLE;  pos.hjust = DDS_CENTER;
    dds_post(init, top, &pos, other);
    dds_get(it, (ddsData) &flag);
    dds_destroy(top);
    FREE(flag_items);
    return flag.selected;
}
