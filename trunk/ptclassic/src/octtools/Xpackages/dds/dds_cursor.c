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
 * Dialog Definition System
 * Cursor Management Routines
 *
 * David Harrison
 * University of California, Berkeley
 * 1990
 *
 * Unfortunately, this is a hack.  The goal is to provide a facility
 * for briefly displaying an alternate cursor in a dds dialog to
 * show that it is "busy" and then have the cursor revert to what
 * it was before.  
 *
 * This is difficult in dds due to a number of problems.  First, the
 * toolkit has no concept of cursors.  Some widgets have cursor resources
 * and some don't.  This means you must set the cursor explicitly for each
 * widget that supports cursors.  Second, dds does not keep linkages
 * down from parent to child in the component heirarchy.  This makes
 * it hard to enumerate the widgets that must change.  Third,  if widgets
 * change, there must be a mechanism for updating their cursor status.
 * It is definitely possible to find elegant solutions to these problems
 * and normally I try to do so.  However,  time prevents me from doing
 * so.
 *
 * What is done instead is as follows: routines that create widgets
 * that have settable cursors call the function dds_add_cursor_widget()
 * to register the widget here.  The destroy function for that widget
 * must also call dds_remove_cursor_widget().  The routine
 * dds_override_cursor() then steps through the list of cursor widgets
 * saves their current cursor then overrides it.  Passing zero
 * to this routine causes the cursors to revert back to the default.
 */
 
#include "dds_internal.h"
#include "st.h"

#define MAX_CURSORS_PER_WIDGET	10

static st_table *cursor_widgets = (st_table *) 0;

typedef struct dds_cursor_key_defn {
    Widget w;			/* Widget itself        */
    String res_name;		/* Cursor resource name */
} dds_cursor_key;

typedef struct dds_cursor_item_defn {
    Cursor def;			/* Default cursor (if any) */
} dds_cursor_item;



static int cursor_cmp(key1, key2)
char *key1, *key2;
/*
 * Compares the keys for equality.  These are actually pointers
 * to (dds_cursor_key *).
 */
{
    dds_cursor_key *one = (dds_cursor_key *) key1;
    dds_cursor_key *two = (dds_cursor_key *) key2;

    if (one->w == two->w) {
	return strcmp(one->res_name, two->res_name);
    } else {
	return -1;
    }
}

static int cursor_hash(key, modulus)
char *key;
int modulus;
/*
 * This is the hashing function for (dds_cursor_key *).  It
 * uses st_strhash to compute a hash for the string then
 * uses st_ptrhash to determine the hash for the pointer
 * then adds them together and takes the modulus.
 */
{
    dds_cursor_key *cursor_key = (dds_cursor_key *) key;

    return (st_ptrhash((char *) cursor_key->w, modulus) +
	    st_strhash(cursor_key->res_name, modulus))%modulus;
}


void dds_add_cursor_widget(w, res)
Widget w;			/* Cursor widget to add     */
String res;			/* Name for cursor resource */
/*
 * Adds a new widget to the list of widgets that have a settable
 * cursor.  The cursor resource is named `res'.  A widget may be
 * entered more than once but each cursor resource name must be
 * different.
 */
{
    dds_cursor_key *key;
    dds_cursor_item *value;

    if (!cursor_widgets) {
	cursor_widgets = st_init_table(cursor_cmp, cursor_hash);
    }
    key = ALLOC(dds_cursor_key, 1);
    key->w = w;
    key->res_name = res;
    value = ALLOC(dds_cursor_item, 1);
    value->def = (Cursor) 0;
    (void) st_insert(cursor_widgets, (char *) key, (char *) value);
}



void dds_remove_cursor_widget(w, res)
Widget w;			/* Cursor widget to remove  */
String res;			/* Name for cursor resource */
/*
 * Removes a cursor from the list of cursor widgets.
 */
{
    dds_cursor_key *key;
    dds_cursor_key key_template;
    char *value;
    
    key_template.w = w;
    key_template.res_name = res;
    key = &key_template;
    if (st_delete(cursor_widgets, (char **) &key, &value)) {
	FREE(key);
	FREE(value);
    }
}



static enum st_retval set_cursor(key, value, arg)
char *key;
char *value;
char *arg;
/*
 * Sets the cursor in a specified widget to the given
 * value.  Preserves the old cursor before doing the
 * change.  If the given value is zero, restores the
 * cursor to the default value.
 */
{
    Cursor new_cursor = (Cursor) arg;
    dds_cursor_key *one = (dds_cursor_key *) key;
    dds_cursor_item *item = (dds_cursor_item *) value;
    Arg arg_list[MAX_ARGS];
    int arg_len;

    if (XtIsRealized(one->w)) {
	if (new_cursor) {
	    if (!item->def) {
		arg_len = 0;
		XtSetArg(arg_list[arg_len], one->res_name, &(item->def)); arg_len++;
		XtGetValues(one->w, arg_list, arg_len);
	    }
	    arg_len = 0;
	    XtSetArg(arg_list[arg_len], one->res_name, new_cursor); arg_len++;
	    XtSetValues(one->w, arg_list, arg_len);
	} else {
	    arg_len = 0;
	    XtSetArg(arg_list[arg_len], one->res_name, item->def);  arg_len++;
	    XtSetValues(one->w, arg_list, arg_len);
	    item->def = (Cursor) 0;
	}
    }
    return ST_CONTINUE;
}

void dds_override_cursor(cur)
Cursor cur;			/* Overridding cursor */
/*
 * This routine temporarily overrides the cursors in all dds dialogs.
 * The new cursor is `cur'.  This is used most often to signal that
 * the system is "busy".  The cursor can be set back to the default
 * by specifying the cursor as zero.
 */
{
    if (cursor_widgets) st_foreach(cursor_widgets, set_cursor, (char *) cur);
}
