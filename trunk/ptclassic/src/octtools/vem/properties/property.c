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
 * VEM Property Editing Commands
 *
 * David Harrison
 * University of California,  Berkeley
 * 1986, 1989
 *
 * This file contains some primitive editing routines for OCT
 * properties.  There are two basic commands defined here:
 *   showProp:		Shows all properties of an object
 *   editProp:		Spawn a dialog based property editor
 */

#include "general.h"		/* General VEM definitions      */
#include "commands.h"		/* Interface for building commands */
#include "message.h"		/* Message handling             */
#include "list.h"		/* List handling package        */
#include "st.h"			/* Hash table package           */
#include "dds.h"		/* Primary dialog interface     */
#include "vemDM.h"		/* High-level dialog interface  */
#include "vemUtil.h"		/* VEM utility functions        */

static char *objTypes[] = {
    "Undefined object",
    "Facet",
    "Terminal",
    "Net",
    "Instance",
    "Polygon",
    "Box",
    "Circle",
    "Path",
    "Label",
    "Property",
    "Bag",
    "Layer",
    "Point",
    "Edge",
    "Formal",
    "Master",
    "Change List",
    "Change Record"
  };

static char *propTypes[] = {
    "null",
    "integer",
    "real",
    "string",
    "id",
    "stranger",
    "aryreal",
    "aryint"
  };

static char valueArea[1024];
#define DEF_ALLOC	100

/*
 * Forward Declarations
 */
static char *getValue();
static vemStatus printProp();
static void prop_edit();
static void new_selection();
static void new_editor();
static int flush_into_prop();
static int32 int_ary_parse();
static int32 real_ary_parse();
static int prop_to_str();
static int prop_diff();
static ddsFlagItem *build_prop_list();
static void free_prop_list();


/*
 * Static Selection Dialog Descriptions
 *
 * More information is added at the actual definition time.
 */

static void sel_edit();
static void sel_new();
static void sel_child();
static void sel_cancel();
static void sel_update();

/* Information for sub-property editors */
typedef struct prop_sel_defn {
    int count;			/* Number of editors running */
    ddsHandle update;		/* Update button             */
} prop_sel;

/* Callback information for buttons */
typedef struct sel_cb_defn {
    ddsHandle list;		/* Item list component  */
    ddsHandle help;		/* Help dialog          */
    prop_sel *ps;		/* Info for sub dialogs */
} sel_cb;

static ddsComposite sel_top_data = { "Which Property?", DDS_VERTICAL, 10 };
static ddsItemList sel_item_data = {
    0, (ddsFlagItem *) 0, DDS_TRUE, -1, DDS_TRUE, 10,
    (void (*)()) 0, (ddsData) 0
};
static ddsControl sel_cons[] = {
    { "Edit", (char *) 0, sel_edit, (ddsData) 0 },
    { "New", (char *) 0, sel_new, (ddsData) 0 },
    { "Edit Children", (char *) 0, sel_child, (ddsData) 0 },
    { "Update", (char *) 0, sel_update, (ddsData) 0 },
    { "Dismiss\n(Del)", "<Key>Delete|<Key>F1", sel_cancel, (ddsData) 0 }
};
static int num_sel_cons = sizeof(sel_cons)/sizeof(ddsControl);
static ddsPosition sel_pos = {
    DDS_MOUSE, DDS_MIDDLE, DDS_CENTER, 0, 0, (Window) 0
};
static char *sel_help =
"There are several possible properties you can examine and edit.  This\
 dialog presents a list of those possible choices.  Choose one of the\
 properties by clicking in the check box next to its name.  Then choose\
 an action from the list of actions at the bottom of the dialog.  Each\
 action is described below:\n\n\
 Edit: spawn a new editor to examine or change the selected property.\n\n\
 New: spawn a new editor to create a new property.  The selected\
 property has no effect on this command.\n\n\
 Dismiss: make this dialog go away without changing anything.\n\n\
 Edit Children: Examine or edit the properties attached to\
 the selected property.";

/*
 * Static Property Editor Descriptions
 *
 * More data is added at dialog creation time.
 */

/* Callback information for buttons */
typedef struct prop_cb_defn {
    ddsHandle name, type, value; /* Real fields in dialog */
    ddsHandle help;		/* Help dialog            */
    prop_sel *ps;		/* Selection info         */
    octId prop_id;		/* Property itself        */
    octId parent_id;		/* Parent object          */
} prop_cb;

static void prop_ok();
static void prop_dismiss();
static void prop_apply();
static void prop_new();
static void prop_child();
static void prop_del();

static ddsComposite prop_top_data = { "Property Editor", DDS_VERTICAL, 3 };
static ddsEditText prop_name_data = { "Name", (char *) 0, 1, 40 };
static char *prop_type_vals[] = {
    "Integer", "Real", "String", "Integer Array", "Real Array"
};
static octPropType prop_type_enums[] = {
    OCT_INTEGER, OCT_REAL, OCT_STRING, OCT_INTEGER_ARRAY, OCT_REAL_ARRAY
};
#define PROP_TYPE_NUM	sizeof(prop_type_vals)/sizeof(char *)
static int prop_type_num = PROP_TYPE_NUM;
static ddsEnumerate prop_type_data = {
    "Type", PROP_TYPE_NUM, prop_type_vals, -1, (void (*)()) 0, (ddsData) 0
};
static ddsEditText prop_val_data = { "Value", (char *) 0, 4, 40 };

static ddsControl prop_cons[] = {
    { "Ok\n(M-Ret)", "Meta<Key>Return|<Key>F1", prop_ok, (ddsData) 0 },
    { "Dismiss\n(M-Del)", "Meta<Key>Delete|<Key>F2", prop_dismiss, (ddsData) 0 },
    { "Apply\n(F4)", "<Key>F4", prop_apply, (ddsData) 0 },
    { "New\n(F5)", "<Key>F5", prop_new, (ddsData) 0 },
    { "Edit Children", (char *) 0, prop_child, (ddsData) 0 },
    { "Delete", (char *) 0, prop_del, (ddsData) 0 },
};
static int num_prop_cons = sizeof(prop_cons)/sizeof(ddsControl);
static ddsPosition prop_pos = {
    DDS_MOUSE, DDS_MIDDLE, DDS_CENTER, 0, 0, (Window) 0
};
static char *prop_help =
"You can edit the name and value of the property using standard line\
 editing commands.  You move between these fields using <Tab>.  Those\
 familiar with Emacs will find that most\
 emacs editing commands also work.  The type of the property is set\
 by choosing one type from the list given to the left of the name\
 and value.  Once you have changed the name, value, and type,  then\
 choose an operation from the command buttons at the bottom of the\
 dialog:\n\n\
 Save: Modify the property with the new values and exit.\n\n\
 New: Create a new property with the specified values and exit.\
 This has no effect on the property you were editing originally.\n\n\
 Dismiss: Exit without updating the property.\n\n\
 Edit Children: Examine or edit the properties attached to the property.\
 This does not work if you are editing a new property.\n\n\
 Delete: Remove the property from the database";


/*
 * First,  various utility routines for dealing with properties
 */

static char *getValue(theProp)
octObject *theProp;		/* The property */
/*
 * Writes out the property value in standard format.  Integers
 * are printed in decimal,  reals are printed using %f,  strings
 * are in quotes,  ids as getting the object and displaying
 * some information about it,  strangers are not displayed,
 * and arrays are simply enclosed in square brackets.
 */
{
    octObject theObj;
    int idx, len;
    char temp[50];

    switch (theProp->contents.prop.type) {
    case OCT_NULL:
	(void) sprintf(valueArea, "<none>");
	break;
    case OCT_INTEGER:
	(void) sprintf(valueArea, "%ld", theProp->contents.prop.value.integer);
	break;
    case OCT_REAL:
	(void) sprintf(valueArea, "%g", theProp->contents.prop.value.real);
	break;
    case OCT_STRING:
	if (strlen(theProp->contents.prop.value.string) > 20) {
	    STRMOVE(valueArea, "`");
	    STRNCONCAT(valueArea, theProp->contents.prop.value.string, 20);
	    STRCONCAT(valueArea, "...'");
	} else {
	    (void) sprintf(valueArea, "`%s'", theProp->contents.prop.value.string);
	}
	break;
    case OCT_ID:
	theObj.objectId = theProp->contents.prop.value.id;
	if (octGetById(&theObj) != OCT_OK) {
	    (void) sprintf(valueArea, "Id %ld", theObj.objectId);
	} else {
	    (void) sprintf(valueArea, "Id %ld <type %s>", theObj.objectId,
			   objTypes[(int) theObj.type]);
	}
	break;
    case OCT_STRANGER:
	(void) sprintf(valueArea, "%ld bytes",
		       (long)theProp->contents.prop.value.stranger.length);
	break;
    case OCT_REAL_ARRAY:
	(void) sprintf(valueArea, "[");
	len = theProp->contents.prop.value.real_array.length;
	if (len > 4) len = 4;
	for (idx = 0; idx < len;  idx++) {
	    if (idx > 0) {
		(void) sprintf(temp, " %g",
			       theProp->contents.prop.value.real_array.array[idx]);
	    } else {
		(void) sprintf(temp, "%g",
			       theProp->contents.prop.value.real_array.array[idx]);
	    }
	    STRCONCAT(valueArea, temp);
	}
	if (theProp->contents.prop.value.real_array.length > len) {
	    STRCONCAT(valueArea, "...");
	}
	STRCONCAT(valueArea, "]");
	break;
    case OCT_INTEGER_ARRAY:
	(void) sprintf(valueArea, "[");
	len = theProp->contents.prop.value.integer_array.length;
	if (len > 4) len = 4;
	for (idx = 0;  idx < len;  idx++) {
	    if (idx > 0) {
		(void) sprintf(temp, " %ld",
			       theProp->contents.prop.value.integer_array.array[idx]);
	    } else {
		(void) sprintf(temp, "%ld",
			       theProp->contents.prop.value.integer_array.array[idx]);
	    }
	    STRCONCAT(valueArea, temp);
	}
	if (theProp->contents.prop.value.integer_array.length > len) {
	    STRCONCAT(valueArea, "...");
	}
	STRCONCAT(valueArea, "]");
	break;
    }
    return((char *) valueArea);
}


static vemStatus printProp(theObj)
octObject *theObj;		/* A Property */
/*
 * This routine prints out the property values of the given object
 * in the standard VEM format:  name (type) = value.
 */
{
    octGenerator theGen;
    octObject propObj;
    int32 xid;
    
    switch (theObj->type) {
    case OCT_FACET:
	(void) sprintf(errMsgArea, "Cell '%s', View '%s', Facet: '%s':\n",
		       theObj->contents.facet.cell,
		       theObj->contents.facet.view,
		       theObj->contents.facet.facet);
	break;
    case OCT_TERM:
	(void) sprintf(errMsgArea, "Terminal named '%s':\n",
		       theObj->contents.term.name);
	break;
    case OCT_NET:
	(void) sprintf(errMsgArea, "Net named '%s':\n",
		       theObj->contents.net.name);
	break;
    case OCT_INSTANCE:
	(void) sprintf(errMsgArea, "Instance named '%s' from master '%s':\n",
		       theObj->contents.instance.name,
		       theObj->contents.instance.master);
	break;
    case OCT_BOX:
	(void) sprintf(errMsgArea, "Box:\n");
	break;
    case OCT_POLYGON:
	(void) sprintf(errMsgArea, "Polygon:\n");
	break;
    case OCT_CIRCLE:
	(void) sprintf(errMsgArea, "Circle:\n");
	break;
    case OCT_PATH:
	(void) sprintf(errMsgArea, "Path:\n");
	break;
    case OCT_LABEL:
	(void) sprintf(errMsgArea, "Label:\n");
	break;
    case OCT_PROP:
	(void) sprintf(errMsgArea, "Property named `%s':\n",
		       theObj->contents.prop.name);
	break;
    case OCT_BAG:
	(void) sprintf(errMsgArea, "Bag named `%s':\n",
		       theObj->contents.bag.name);
	break;
    case OCT_LAYER:
	(void) sprintf(errMsgArea, "Layer named `%s':\n",
		       theObj->contents.layer.name);
	break;
    case OCT_POINT:
	(void) sprintf(errMsgArea, "Point:\n");
	break;
    case OCT_EDGE:
	(void) sprintf(errMsgArea, "Edge:\n");
	break;
    case OCT_CHANGE_LIST:
	(void) sprintf(errMsgArea, "Change list:\n");
	break;
    case OCT_CHANGE_RECORD:
	(void) sprintf(errMsgArea, "Change record:\n");
	break;
    default:
	(void) sprintf(errMsgArea, "Unknown type: %d\n", theObj->type);
	break;
    }
    vemMessage(errMsgArea, MSG_NOLOG|MSG_DISP);
    /* Now generate all of its properties */
    if (octInitGenContents(theObj, OCT_PROP_MASK, &theGen) != OCT_OK) {
	vemMessage("Can't generate properties\n", MSG_NOLOG|MSG_DISP);
	return VEM_CORRUPT;
    }
    while (octGenerate(&theGen, &propObj) == OCT_OK) {
	/* Print out the property information in format */
	octExternalId(&propObj, &xid);
	(void) sprintf(errMsgArea, "  %ld: %s (%s) = %s\n",
		       xid, propObj.contents.prop.name,
		       propTypes[(int) propObj.contents.prop.type],
		       getValue(&propObj));
	vemMsg(MSG_C, "  %ld: %s (%s) = %s\n",
	       xid, propObj.contents.prop.name,
	       propTypes[(int) propObj.contents.prop.type],
	       getValue(&propObj));
    }
    return VEM_OK;
}





vemStatus showProp(spot, cmdList)
vemPoint *spot;			/* Where command was issued */
lsList cmdList;			/* List of arguments        */
/*
 * This command shows all of the properties associated with the
 * object pointed to by 'spot'.  It also shows some information
 * about the object selected.  The command takes no arguments.
 */
{
    vemOneArg *firstArg;
    octObject spotObj, argBag, item;
    octGenerator theGen;
    int len;

    len = lsLength(cmdList);
    if (len > 1) {
	vemMessage("format: [objects] show-prop\n",
		   MSG_NOLOG|MSG_DISP);
	return VEM_CORRUPT;
    }

    if (len == 0) {
	switch (vuFindSpot(spot, &spotObj, OCT_ALL_MASK)) {
	case VEM_OK:
	case VEM_CANTFIND:
	    /* For VEM_CANTFIND, it will be the facet (which is ok) */
	    break;
	case VEM_NOSELECT:
	    vemMessage("No object selected\n", MSG_NOLOG|MSG_DISP);
	    return VEM_ARGRESP;
	}

	/* Ok,  we have the object.  Print out its properties. */
	(void) printProp(&spotObj);
	return VEM_OK;
    }

    lsFirstItem(cmdList, (lsGeneric *) &firstArg, LS_NH);
    if (firstArg->argType != VEM_OBJ_ARG) {
	vemMessage("format: [objects] show-prop\n",
		   MSG_NOLOG|MSG_DISP);
	return VEM_CORRUPT;
    }
    argBag.objectId = firstArg->argData.vemObjArg.theBag;
    if (octGetById(&argBag) != OCT_OK) {
	(void) sprintf(errMsgArea, "Cannot get argument bag:\n  %s\n",
		       octErrorString());
	vemMessage(errMsgArea, MSG_DISP);
	return VEM_CORRUPT;
    }

    if (octInitGenContents(&argBag, OCT_ALL_MASK, &theGen) == OCT_OK) {
	while (octGenerate(&theGen, &item) == OCT_OK) {
	    printProp(&item);
	}
    }
    return VEM_OK;
}    


vemStatus editProp(spot, cmdList)
vemPoint *spot;			/* Where command was issued */
lsList cmdList;			/* List of arguments        */
/*
 * This command starts up the dialog box based property editor.
 * It takes one optional argument:  a selected set.  The command
 * will spawn one selection dialog for each item in the selected
 * set.  If no selected set is specified,  it will try to determine
 * the object from spot.
 */
{
    vemOneArg *firstArg;
    octObject spotObj, argBag, item;
    octGenerator theGen;
    int len;

    len = lsLength(cmdList);
    if (len > 1) {
	vemMessage("format: [objects] edit-prop\n",
		   MSG_NOLOG|MSG_DISP);
	return VEM_CORRUPT;
    }

    if (len == 1) {
	/* He has given us a bag of objects to spawn onto */
	lsFirstItem(cmdList, (lsGeneric *) &firstArg, LS_NH);
	if (firstArg->argType != VEM_OBJ_ARG) {
	    vemMessage("format: [objects] edit-prop\n",
		       MSG_NOLOG|MSG_DISP);
	    return VEM_CORRUPT;
	}
	argBag.objectId = firstArg->argData.vemObjArg.theBag;
	if (octGetById(&argBag) != OCT_OK) {
	    (void) sprintf(errMsgArea, "Cannot get argument bag:\n  %s\n",
			   octErrorString());
	    vemMessage(errMsgArea, MSG_DISP);
	}
	/* Spawn an editor for every object in set (as long as reasonable) */
	if (firstArg->length > 5) {
	    (void) sprintf(errMsgArea,
			   "You have asked to examine properties\non %d objects.  Do you really want\nto spawn that many editors?", firstArg->length);
	    if (dmConfirm("Edit Property", errMsgArea, "Yes", "No") != VEM_OK) {
		vemMessage("Command canceled\n", MSG_NOLOG|MSG_DISP);
		return VEM_ARGRESP;
	    }
	}
	if (octInitGenContents(&argBag, OCT_ALL_MASK, &theGen) == OCT_OK) {
	    while (octGenerate(&theGen, &item) == OCT_OK) {
		prop_edit(&item);
	    }
	} else {
	    vemMsg(MSG_C, "Cant get objects on list:\n  %s\n",
		   octErrorString());
	    return VEM_CORRUPT;
	}
    } else {
	/* Try to find the object using the oct utility package */

	switch (vuFindSpot(spot, &spotObj, OCT_ALL_MASK)) {
	case VEM_OK:
	case VEM_CANTFIND:
	    /* For VEM_CANTFIND, it will be the facet (which is ok) */
	    break;
	case VEM_NOSELECT:
	    vemMessage("No object selected\n", MSG_NOLOG|MSG_DISP);
	    return VEM_ARGRESP;
	}
	/* Ok,  we have the object.  Spawn an editor */
	prop_edit(&spotObj);
    }
    return VEM_OK;
}    



/*
 * New property editor
 *
 * Below are the routines which implement an unmoded dialog based
 * property editor.  Normally,  this interface pops up a dialog
 * to edit the name, value, and type of a property.  If the parent
 * object has more than one property,  a selection dialog will
 * be posted to choose the starting property.
 */

/*
 * Hash table of currently running selection dialogs indexed by
 * Oct Id.  It is used to detect and prevent multiple selection
 * dialogs on the same object.
 */
static st_table *allSels = (st_table *) 0;

/*
 * Hash table fo currently running property editors indexed by
 * Oct Id.  This is used to prevent multiple editors on the same object.
 */
static st_table *allEditors = (st_table *) 0;

typedef struct prop_info_defn {
    octId parentId;		/* Parent of property */
    octId propId;		/* Property id        */
} propInfo;



static void prop_edit(topObj)
octObject *topObj;		/* Top-level object */
/*
 * This routine determines whether a selection dialog
 * box must be posted showing properties directly attached
 * to `topObj'.  If there are less than two properties,
 * the routine directly invokes an editor dialog.  If there
 * are two or more,  a selection dialog is posted.
 */
{
    octObject someProp;
    octGenerator theGen;
    int items;

    /* Count the number of properties on the object */
    (void) octInitGenContents(topObj, OCT_PROP_MASK, &theGen);
    items = 0;
    while (octGenerate(&theGen, &someProp) == OCT_OK) {
	items++;
    }
    if (items == 0) {
	/* Creation editor */
	new_editor(topObj, (octObject *) 0, (prop_sel *) 0);
    } else if (items == 1) {
	/* Edit existing property */
	new_editor(topObj, &someProp, (prop_sel *) 0);
    } else {
	/* More than one property -- selection editor */
	new_selection(topObj, items);
    }
}


/*
 * Due to an ultrix 3.1 cc bug we have to do this
 */
#define FSAME(f1, f2)	(((int (*)()) f1) == ((int (*)()) f2))

static void new_selection(parent, count)
octObject *parent;		/* Parent object   */
int count;			/* Number of props */
/*
 * This routine builds a description for a selection dialog and
 * posts it using the low-level dialog interface supplied by
 * the dm package.  It consists of a list of possible properties
 * for editing and several control buttons:
 *   edit:           Spawn an editor on the selected property.
 *   new:            Spawn an editor for creating a new property.
 *   edit children:  Edit children of selected property.
 *   cancel:         Don't do anything.
 * It makes sure there are no other selection dialogs running
 * on the same object.
 */
{
    ddsHandle top, list, control;
    ddsFlagItem *prop_list;
    sel_cb *info;
    octId *parent_id;
    int i;

    /* Check to see if a selector is already running on this object */
    if (allSels == (st_table *) 0) {
	/* Hash table indexed by octId */
	allSels = st_init_table(st_numcmp, st_numhash);
    }
    if (st_is_member(allSels, (char *) parent->objectId)) {
	/* There is another selector running */
	dmConfirm("Error",
		  "There is another selection\ndialog running for this\nobject",
		  "Continue", (char *) 0);
	return;
    }

    /* Build dialog */
    top = dds_component(0, DDS_TOP, (ddsData) &sel_top_data);
    prop_list = build_prop_list(parent, count);
    parent_id = VEMALLOC(octId);
    *parent_id = parent->objectId;
    sel_item_data.num_items = count;
    sel_item_data.items = prop_list;
    sel_item_data.user_data = (ddsData) parent_id;
    list = dds_component(top, DDS_ITEM_LIST, (ddsData) &sel_item_data);
    free_prop_list(prop_list, count);
    info = VEMALLOC(sel_cb);
    info->list = list;
    info->ps = VEMALLOC(prop_sel);
    info->ps->count = 0;
    for (i = 0;  i < num_sel_cons;  i++) {
	sel_cons[i].user_data = (ddsData) info;
	control = dds_component(top, DDS_CONTROL, (ddsData) (sel_cons+i));
	if (FSAME(sel_cons[i].callback, sel_update)) info->ps->update = control;
    }
    info->help = dmHelpButton(top, sel_help);
    dds_post(top, &sel_pos, (ddsEvtHandler *) 0);

    /* Add it to the list of active selectors */
    (void) st_insert(allSels, (char *) parent->objectId, (char *) 0);
}


static ddsFlagItem *build_prop_list(parent, count)
octObject *parent;		/* Parent object   */
int count;			/* Number of props */
/*
 * This routine builds up a list of properties for use in a
 * property selection dialog.
 */
{
    octGenerator gen;
    octObject prop;
    ddsFlagItem *prop_list;
    char one_descr[1024];
    int32 xid;
    int i;

    prop_list = VEMARRAYALLOC(ddsFlagItem, count);
    VEM_OCTCKRVAL(octInitGenContents(parent, OCT_PROP_MASK, &gen),
		  (ddsFlagItem *)0);
    i = 0;
    while (octGenerate(&gen, &prop) == OCT_OK) {
	octExternalId(&prop, &xid);
	(void) sprintf(one_descr, "%ld: %s (%s) = %s", xid,
		       prop.contents.prop.name,
		       propTypes[(int) prop.contents.prop.type],
		       getValue(&prop));
	prop_list[i].label = VEMSTRCOPY(one_descr);
	prop_list[i].selected_p = DDS_FALSE;
	prop_list[i].user_data = (ddsData) xid;
	i++;
    }
    return prop_list;
}

static void free_prop_list(prop_list, count)
ddsFlagItem *prop_list;		/* List of properties */
int count;			/* Number of props    */
/*
 * This routine recovers the storage used by build_prop_list().
 */
{
    int i;

    for (i = 0;  i < count;  i++) VEMFREE(prop_list[i].label);
    VEMFREE(prop_list);
}



static void sel_edit(item)
ddsHandle item;			/* Control button */
/*
 * Activated when user presses the `Edit' button in a selection
 * dialog.  This spawns an editor on the selected property.
 */
{
    ddsControl con_data;
    ddsItemList item_data;
    sel_cb *info;
    octObject parent, prop;
    octObject parentFacet;	/* Because octGetByExternalId()'s fisrt argh m,ust be a facet. */

    dds_get(item, (ddsData) &con_data);
    info = (sel_cb *) con_data.user_data;
    dds_get(info->list, (ddsData) &item_data);
    if (item_data.selected >= 0) {
	parent.objectId = *((octId *) item_data.user_data);
	if (octGetById(&parent) != OCT_OK) {
	    errRaise("vem", VEM_CORRUPT, "Cannot get parent of property");
	    /*NOTREACHED*/
	}
	octGetFacet( &parent, &parentFacet );
	if (octGetByExternalId(&parentFacet,
			       (int32) item_data.items[item_data.selected].user_data,
			       &prop) != OCT_OK) {
	    errRaise("vem", VEM_CORRUPT, "Cannot get property");
	    /*NOTREACHED*/
	}
	new_editor(&parent, &prop, info->ps);
    } else {
	/* No property selected */
	(void) dmConfirm("Error", "No property was selected\nfor editing.",
			 "Continue", (char *) 0);
    }
}

static void sel_new(item)
ddsHandle item;			/* Control button */
/*
 * Activated when user presses the `New' button in a selection
 * dialog.  This spawns an editor on a newly created property.
 */
{
    ddsControl con_data;
    ddsItemList item_data;
    sel_cb *info;
    octObject parent;

    dds_get(item, (ddsData) &con_data);
    info = (sel_cb *) con_data.user_data;
    dds_get(info->list, (ddsData) &item_data);
    parent.objectId = *((octId *) item_data.user_data);
    if (octGetById(&parent) != OCT_OK) {
	errRaise("vem", VEM_CORRUPT, "Cannot get parent of property");
	/*NOTREACHED*/
    }
    new_editor(&parent, (octObject *) 0, info->ps);
}


static void sel_child(item)
ddsHandle item;			/* Control button */
/*
 * Activated when user presses the `Edit Children' button in a selection
 * dialog.  This edits the children of the selected property.
 */
{
    ddsControl con_data;
    ddsItemList item_data;
    sel_cb *info;
    octObject parent, prop;

    dds_get(item, (ddsData) &con_data);
    info = (sel_cb *) con_data.user_data;
    dds_get(info->list, (ddsData) &item_data);
    if (item_data.selected >= 0) {
	parent.objectId = *((octId *) item_data.user_data);
	if (octGetById(&parent) != OCT_OK) {
	    errRaise("vem", VEM_CORRUPT, "Cannot get parent of property");
	    /*NOTREACHED*/
	}
	if (octGetByExternalId(&parent,
			       (int32) item_data.items[item_data.selected].user_data,
			       &prop) != OCT_OK) {
	    errRaise("vem", VEM_CORRUPT, "Cannot get property");
	    /*NOTREACHED*/
	}
	prop_edit(prop);
    } else {
	/* No property selected */
	(void) dmConfirm("Error", "No property was selected\nas a parent.",
			 "Continue", (char *) 0);
    }
}


static void sel_cancel(item)
ddsHandle item;			/* Control button */
/*
 * Activated when user presses the `Dismiss' button in a selection
 * dialog.  This causes the dialog (and its help dialog) to go
 * away.
 */
{
    ddsControl con_data;
    ddsItemList item_data;
    sel_cb *info;
    octObject parent;
    char *key, *value;

    dds_get(item, (ddsData) &con_data);
    info = (sel_cb *) con_data.user_data;
    dds_get(info->list, (ddsData) &item_data);
    parent.objectId = *((octId *) item_data.user_data);
    if (octGetById(&parent) != OCT_OK) {
	errRaise("vem", VEM_CORRUPT, "Cannot get parent of property");
	/*NOTREACHED*/
    }
    /* Remove from hash table */
    key = (char *) parent.objectId;
    (void) st_delete(allSels, &key, &value);
    /* Turn off backpointer information */
    info->ps->update = (ddsHandle) 0;
    if (info->ps->count == 0) {
	VEMFREE(info->ps);
    }
    /* Free resources */
    if (info->help) dds_destroy(info->help);
    dds_destroy(item);
    VEMFREE(info);
}


static void sel_update(item)
ddsHandle item;
/*
 * This routine is called when the `Update' button in a selection
 * dialog is activated.  It is also called by any editors spawned
 * from the selection dialog when properties are changed.  It
 * updates the list of properties shown in the selection dialog.
 */
{
    octGenerator gen;
    octObject prop, parent;
    ddsControl con_data;
    ddsItemList item_data;
    sel_cb *info;
    int count;

    dds_get(item, (ddsData) &con_data);
    info = (sel_cb *) con_data.user_data;
    dds_get(info->list, (ddsData) &item_data);
    parent.objectId = *((octId *) item_data.user_data);
    if (octGetById(&parent) != OCT_OK) {
	errRaise("vem", VEM_CORRUPT, "Cannot get parent of property");
	/*NOTREACHED*/
    }
    count = 0;
    (void) octInitGenContents(&parent, OCT_PROP_MASK, &gen);
    while (octGenerate(&gen, &prop) == OCT_OK) count++;
    item_data.num_items = count;
    item_data.items = build_prop_list(&parent, count);
    dds_set(info->list, (ddsData) &item_data);
}


static int flush_into_prop(info, prop)
prop_cb *info;			/* Property callback info      */
octObject *prop;		/* Property itself             */
/*
 * This routine flushes the values in the fields described
 * by `info' into `prop'.  It may fail if parsing of the property
 * value doesn't work.  If so, it doesn't modify the passed
 * property.  The strings are directly taken from the dds components.
 * Thus, they will be destroyed when the dialog is destroyed.
 */
{
    ddsEditText ed_vals;
    ddsEnumerate enum_vals;
    struct octProp new_prop;

    /* Name */
    dds_get(info->name, (ddsData) &ed_vals);
    new_prop.name = ed_vals.text;

    /* Type */
    dds_get(info->type, (ddsData) &enum_vals);
    if ((enum_vals.selected < 0) || (enum_vals.selected > prop_type_num)) {
	enum_vals.selected = 2;
    }
    new_prop.type = prop_type_enums[enum_vals.selected];

    /* Value */
    dds_get(info->value, (ddsData) &ed_vals);
    switch (new_prop.type) {
    case OCT_INTEGER:
	if (sscanf(ed_vals.text, "%ld", &new_prop.value.integer) != 1) return 0;
	break;
    case OCT_REAL:
	if (sscanf(ed_vals.text, "%lf", &new_prop.value.real) != 1) return 0;
	break;
    case OCT_STRING:
	new_prop.value.string = ed_vals.text;
	break;
    case OCT_INTEGER_ARRAY:
	new_prop.value.integer_array.length =
	  int_ary_parse(ed_vals.text, &(new_prop.value.integer_array.array));
	if (new_prop.value.integer_array.length < 0) return 0;
	break;
    case OCT_REAL_ARRAY:
	new_prop.value.real_array.length =
	  real_ary_parse(ed_vals.text, &(new_prop.value.real_array.array));
	if (new_prop.value.real_array.length < 0) return 0;
	break;
    default:
	return 0;
    }
    prop->contents.prop = new_prop;
    return 1;
}


#define INIT_ALLOC	20

static int32 int_ary_parse(data, ary)
STR data;			/* String with integers     */
int32 **ary;			/* Integer array (returned) */
/*
 * Parses a string assumed to contain a list of space separated
 * integers.  If successful,  it returns the number of integers
 * and fills the integers into `ary'.  If a parsing error
 * occurs,  it returns a negative result.
 */
{
    int size;
    int loc;
    char *num;

    size = INIT_ALLOC;
    *ary = VEMARRAYALLOC(int32, size);
    loc = 0;
    while (data && *data) {
	/* Eat space */
	while (*data && isspace(*data)) data++;
	num = data;
	/* Move forward to next space */
	while (*data && !isspace(*data)) data++;
	/* Prepare array */
	if (loc >= size) {
	    size *= 2;
	    *ary = VEMREALLOC(int32, *ary, size);
	}
	/* Attempt to read number */
	if (sscanf(num, "%ld", &((*ary)[loc])) != 1) {
	    /* Failed */
	    return -1;
	} else {
	    loc += 1;
	}
    }
    return loc;
}



static int32 real_ary_parse(data, ary)
STR data;			/* String with integers     */
double **ary;			/* Real array (returned)    */
/*
 * Parses a string assumed to contain a list of space separated
 * doubles.  If successful,  it returns the number of reals
 * and fills the reals into `ary'.  If a parsing error
 * occurs,  it returns a negative result.
 */
{
    int size;
    int loc;
    char *num;

    size = INIT_ALLOC;
    *ary = VEMARRAYALLOC(double, size);
    loc = 0;
    while (data && *data) {
	/* Eat space */
	while (*data && isspace(*data)) data++;
	num = data;
	/* Move forward to next space */
	while (*data && !isspace(*data)) data++;
	/* Prepare array */
	if (loc >= size) {
	    size *= 2;
	    *ary = VEMREALLOC(double, *ary, size);
	}
	/* Attempt to read number */
	if (sscanf(num, "%lf", &((*ary)[loc])) != 1) {
	    /* Failed */
	    return -1;
	} else {
	    loc += 1;
	}
    }
    return loc;
}




static void new_editor(parent, prop, ps)
octObject *parent;		/* Parent object  */
octObject *prop;		/* Property       */
prop_sel *ps;			/* Selection info */
/* 
 * Spawns a new unmoded editor for the specified parent/property
 * pair.  The editor consists of an edit text for the property
 * name, an enumerated field for property type, and a edit text
 * for property value.  The following control buttons are available:
 *   Save:   Flush value into property and exit
 *   New:    Flush value into new property and exit
 *   Cancel: Do not change property and exit.
 *   Edit Children: Spawn editor on children of property.
 *   Delete: Remove the indicated property.
 * The parent must be non-zero.  If `prop' is zero, a new property
 * will be created.  The argument `ps' is used to keep the selection
 * dialog up to date.
 */
{
    ddsHandle top, cons;
    prop_cb *info;
    int i;
    STR value_str;

    if (!parent) {
	errRaise("vem", VEM_CORRUPT, "new_editor: parent cannot be null");
	/*NOTREACHED*/
    }
    /* Check to see if there is already an editor running */
    if (allEditors == (st_table *) 0) {
	allEditors = st_init_table(st_numcmp, st_numhash);
    }
    if (prop && st_is_member(allEditors, (char *) prop->objectId)) {
	/* Another editor is running */
	dmConfirm("Error",
		  "There is another property editor\nediting this object.",
		  "Continue", (char *) 0);
	return;
    }
    /* Build description */
    info = VEMALLOC(prop_cb);
    top = dds_component(0, DDS_TOP, (ddsData) &prop_top_data);
    if (prop) prop_name_data.text = prop->contents.prop.name;
    else prop_name_data.text = "";
    info->name = dds_component(top, DDS_EDIT_TEXT, (ddsData) &prop_name_data);
    if (prop) {
	for (i = 0;  i < prop_type_num;  i++)
	  if (prop->contents.prop.type == prop_type_enums[i]) break;
	prop_type_data.selected = i;
    } else {
	/* Default is string */
	prop_type_data.selected = 2;
    }
    info->type = dds_component(top, DDS_ENUMERATE, (ddsData) &prop_type_data);
    value_str = VEMARRAYALLOC(char, DEF_ALLOC);
    value_str[0] = '\0';
    if (prop) {
	(void) prop_to_str(prop, DEF_ALLOC, &value_str);
    }
    prop_val_data.text = value_str;
    info->value = dds_component(top, DDS_EDIT_TEXT, (ddsData) &prop_val_data);
    info->prop_id = (prop ? prop->objectId : oct_null_id);
    info->parent_id = parent->objectId;
    for (i = 0;  i < num_prop_cons;  i++) {
	prop_cons[i].user_data = (ddsData) info;
	cons = dds_component(top, DDS_CONTROL, (ddsData) (prop_cons+i));
    }
    info->help = dmHelpButton(top, prop_help);
    info->ps = ps;
    if (info->ps) info->ps->count += 1;
    dds_post(top, &prop_pos, (ddsEvtHandler *) 0);
    /* Add to list of editors */
    if (prop) {
	(void) st_insert(allEditors, (char *) prop->objectId, (char *) 0);
    }
}


static prop_cb *get_prop_info(item)
ddsHandle item;			/* Control button */
/*
 * Extracts property callback information from the user
 * data field of the control button.
 */
{
    ddsControl data;

    dds_get(item, (ddsData) &data);
    return (prop_cb *) data.user_data;
}

static void prop_apply(item)
ddsHandle item;			/* Control buttona */
/*
 * This routine is called when the `Apply' button in a property
 * editor is activated.  This flushes the value of the property
 * into the database but leaves the dialog posted.
 */
{
    prop_cb *info;
    octObject prop, fct;
    static char *prop_err =
"The selected property doesn't exist\n\
in the database.  You may want to use `New'\n\
to create a new property instead.";
    static char *parse_err =
"The value for the property cannot be parsed\n\
given the property type.  You can\n\
correct the problem by changing either the\n\
property type or the property value.";
    static char *read_only =
"The buffer is marked as read only. No changes\n\
can be made to this buffer.";

    info = get_prop_info(item);
    prop.objectId = info->prop_id;
    if (!octIdIsNull(prop.objectId) && (octGetById(&prop) == OCT_OK)) {
	if (flush_into_prop(info, &prop)) {
	    octGetFacet(&prop, &fct);
	    if (bufWritable(fct.objectId) == VEM_OK) {
		VEM_OCTCKRET(octModify(&prop));
		/* Update selection dialog */
		if (info->ps && info->ps->update) sel_update(info->ps->update);
	    } else {
		(void) dmConfirm("Error", read_only, "Continue", (char *) 0);
	    }
	} else {
	    (void) dmConfirm("Error", parse_err, "Continue", (char *) 0);
	}
    } else {
	(void) dmConfirm("Error", prop_err, "Continue", (char *) 0);
    }
}


static void dump_editor(item, info)
ddsHandle item;			/* Control button */
prop_cb *info;			/* Information    */
/*
 * Frees up the resources consumed by the property editor dialog.
 */
{
    char *key, *value;

    if (info->ps) {
	info->ps->count -= 1;
	if ((info->ps->count <= 0) && (info->ps->update == (ddsHandle) 0)) {
	    VEMFREE(info->ps);
	}
    }
    if (info->help) dds_destroy(info->help);
    dds_destroy(item);
    VEMFREE(info);
    key = (char *) info->prop_id;
    (void) st_delete(allEditors, &key, &value);
}

static void prop_dismiss(item)
ddsHandle item;			/* Control buttona */
/*
 * This routine is called when the `Dismiss' button in a property
 * editor is activated.  It checks to make sure there have been
 * no changes since the editor was invoked.  If there are changes,
 * it asks for confirmation before proceeding.
 */
{
    prop_cb *info;
    octObject prop, new_prop;
#ifdef NEVER
    static char *prop_err =
"The selected property no longer exists\n\
in the database.  You may want to use `New'\n\
to make a new copy.";
#endif
    static char *parse_err =
"The value for the property cannot be parsed\n\
given the property type.  Do you want to exit\n\
the editor without saving the changes you made\n\
to the property value?";
    static char *diff_err =
"You have made changes to the property that\n\
have not been saved.  Do you want to exit\n\
the editor without saving them?";

    info = get_prop_info(item);
    prop.objectId = info->prop_id;
    if (!octIdIsNull(prop.objectId) && (octGetById(&prop) == OCT_OK)) {
	new_prop = prop;
	if (flush_into_prop(info, &new_prop)) {
	    if (prop_diff(&new_prop, &prop)) {
		if (dmConfirm("Warning", diff_err, "Yes", "No") != VEM_OK) {
		    return;
		}
	    }
	    dump_editor(item, info);
	} else {
	    if (dmConfirm("Error", parse_err, "Yes", "No") == VEM_OK) {
		dump_editor(item, info);
	    }
	}
    } else {
	if (flush_into_prop(info, &new_prop)) {
	    prop.contents.prop.name = "";
	    prop.contents.prop.type = OCT_STRING;
	    prop.contents.prop.value.string = "";
	    if (prop_diff(&new_prop, &prop)) {
		if (dmConfirm("Warning", diff_err, "Yes", "No") != VEM_OK) {
		    return;
		}
	    }
	    dump_editor(item, info);
	} else {
	    if (dmConfirm("Error", parse_err, "Yes", "No") == VEM_OK) {
		dump_editor(item, info);
	    }
	}
    }
}

static void prop_ok(item)
ddsHandle item;			/* Control buttona */
/*
 * This routine is called when the `Ok' button in a property
 * editor is activated.  This is just like `Apply' followed
 * by `Dismiss'.
 */
{
    prop_apply(item);
    prop_dismiss(item);
}



static void prop_new(item)
ddsHandle item;			/* Control button */
/*
 * This routine is called when the `New' button in a property
 * editor is activated.  It is like `Apply' except it creates
 * a new property under the parent before flushing the values
 * into the property.  The property need not exist.
 */
{
    prop_cb *info;
    octObject parent, prop, fct;
    static char *ro_err =
"The buffer is read only.  No changes\n\
can be made to this buffer.";
    static char *p_err =
"The value for the property cannot be parsed\n\
given the property type.  You can\n\
correct the problem by changing either the\n\
property type or the property value.";

    info = get_prop_info(item);
    parent.objectId = info->parent_id;
    if (octGetById(&parent) == OCT_OK) {
	prop.type = OCT_PROP;
	if (flush_into_prop(info, &prop)) {
	    octGetFacet(&parent, &fct);
	    if (bufWritable(fct.objectId) == VEM_OK) {
		VEM_OCTCKRET(octCreate(&parent, &prop));
		info->prop_id = prop.objectId;
		/* Update selection dialog */
		if (info->ps && info->ps->update) sel_update(info->ps->update);
	    } else {
		(void) dmConfirm("Error", ro_err, "Continue", (char *) 0);
	    }
	} else {
	    (void) dmConfirm("Error", p_err, "Continue", (char *) 0);
	}
    } else {
	errRaise("vem", VEM_CORRUPT, "Cannot get parent of property");
	/*NOTREACHED*/
    }
}



static void prop_child(item)
ddsHandle item;			/* Control button */
/*
 * This routine is called when the `Edit Children' button is pushed.
 * It edits any children of the selected property.
 */
{
    prop_cb *info;
    octObject prop;
    static char *missing =
"You are not editing a property that exists\n\
in the database.  You may want to create one\n\
using the \"New\" option before attempting\n\
to edit the children of the property.";

    info = get_prop_info(item);
    prop.objectId = info->prop_id;
    if (!octIdIsNull(prop.objectId) && (octGetById(&prop) == OCT_OK)) {
	prop_edit(&prop);
    } else {
	(void) dmConfirm("Error", missing, "Continue", (char *) 0);
    }
}


static void prop_del(item)
ddsHandle item;			/* Control button */
/*
 * This routine is called when the `Delete' button is pushed.
 * It removes the property from the database.
 */
{
    prop_cb *info;
    octObject prop;
    static char *missing =
"You are not editing a property that exists\n\
in the database.  Thus, the property cannot\n\
be deleted.";

    info = get_prop_info(item);
    prop.objectId = info->prop_id;
    if (!octIdIsNull(prop.objectId) && (octGetById(&prop) == OCT_OK)) {
	(void) octDelete(&prop);
	/* update selection dialog */
	if (info->ps && info->ps->update) sel_update(info->ps->update);
    } else {
	(void) dmConfirm("Error", missing, "Continue", (char *) 0);
    }
}



static int prop_to_str(prop, size, area)
octObject *prop;		/* Property to edit */
int size;			/* Size of buffer   */
STR *area;			/* Buffer itself    */
/*
 * Loads the property value into `area'.  Reallocates
 * the array if necessary.  Returns the allocated size
 * of the string.
 */
{
    int str_val_len;
    int cur_len;
    int i;
    char numbuf[128];

    switch (prop->contents.prop.type) {
    case OCT_INTEGER:
	if (size < 64) {
	    size = 64;
	    *area = VEMREALLOC(char, *area, size);
	}
	(void) sprintf(*area, "%ld", prop->contents.prop.value.integer);
	break;
    case OCT_REAL:
	if (size < 128) {
	    size = 128;
	    *area = VEMREALLOC(char, *area, size);
	}
	(void) sprintf(*area, "%f", prop->contents.prop.value.real);
	break;
    case OCT_STRING:
	str_val_len = STRLEN(prop->contents.prop.value.string);
	if (size <= str_val_len-1) {
	    size = str_val_len+1;
	    *area = VEMREALLOC(char, *area, size);
	}
	STRMOVE(*area, prop->contents.prop.value.string);
	break;
    case OCT_INTEGER_ARRAY:
	str_val_len = 0;
	cur_len = 0;
	for (i = 0; i < prop->contents.prop.value.integer_array.length; i++) {
	    (void) sprintf(numbuf, "%ld",
		    prop->contents.prop.value.integer_array.array[i]);
	    str_val_len = STRLEN(numbuf);
	    if (cur_len + str_val_len + 1 >= size) {
		size *= 2;
		*area = VEMREALLOC(char, *area, size);
	    }
	    if (i > 0) {
		STRCONCAT(*area, " ");
	    }
	    STRCONCAT(*area, numbuf);
	}
	break;
    case OCT_REAL_ARRAY:
	str_val_len = 0;
	cur_len = 0;
	for (i = 0; i < prop->contents.prop.value.real_array.length; i++) {
	    (void) sprintf(numbuf, "%f",
		    prop->contents.prop.value.real_array.array[i]);
	    str_val_len = STRLEN(numbuf);
	    if (cur_len + str_val_len + 1 >= size) {
		size *= 2;
		*area = VEMREALLOC(char, *area, size);
	    }
	    if (i > 0) {
		STRCONCAT(*area, " ");
	    }
	    STRCONCAT(*area, numbuf);
	}
	break;
      case OCT_NULL:
      case OCT_ID:
      case OCT_STRANGER:
	/* For completeness of enum handling.  Should we print an
	   error message here? */
	break;
    }
    return size;
}


static int prop_diff(one, two)
octObject *one, *two;		/* Two properties */
/*
 * Returns a non-zero value if the two properties differ.
 */
{
    int i;

    if ((STRCOMP(one->contents.prop.name, two->contents.prop.name) == 0) &&
	(one->contents.prop.type == two->contents.prop.type)) {
	switch (one->contents.prop.type) {
	case OCT_NULL:
	    return 0;
	case OCT_INTEGER:
	    return one->contents.prop.value.integer != two->contents.prop.value.integer;
	case OCT_REAL:
	    return one->contents.prop.value.real != two->contents.prop.value.real;
	case OCT_STRING:
	    return STRCOMP(one->contents.prop.value.string, two->contents.prop.value.string) != 0;
	case OCT_ID:
	    return octIdsEqual(one->contents.prop.value.id, two->contents.prop.value.id) != 0;
	case OCT_STRANGER:
	    return (one->contents.prop.value.stranger.length !=
	      two->contents.prop.value.stranger.length) ||
		(memcmp(one->contents.prop.value.stranger.contents,
			two->contents.prop.value.stranger.contents,
			one->contents.prop.value.stranger.length) != 0);
	case OCT_REAL_ARRAY:
	    if (one->contents.prop.value.real_array.length !=
		two->contents.prop.value.real_array.length) return 1;
	    for (i = 0;  i < one->contents.prop.value.real_array.length;  i++) {
		if (one->contents.prop.value.real_array.array[i] !=
		    two->contents.prop.value.real_array.array[i]) return 1;
	    }
	    return 0;
	case OCT_INTEGER_ARRAY:
	    if (one->contents.prop.value.integer_array.length !=
		two->contents.prop.value.integer_array.length) return 1;
	    for (i = 0;  i < one->contents.prop.value.integer_array.length;  i++) {
		if (one->contents.prop.value.integer_array.array[i] !=
		    two->contents.prop.value.integer_array.array[i]) return 1;
	    }
	    return 0;
	}
    } else {
	return 1;
    }
    return 0;
}
