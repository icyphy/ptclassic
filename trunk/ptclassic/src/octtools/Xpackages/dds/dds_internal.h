/* Version Identification:
 * $Id$
 */
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
 * Internal definitions for dialog description system
 *
 * David Harrison
 * University of California, Berkeley
 * 1990
 */

#ifndef _DDS_INTERNAL_H_
#define _DDS_INTERNAL_H_

#include "port.h"
#include "utility.h"

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include "dds.h"

#define MAX_ARGS	10

#define FOCUS
#define ACCELERATORS

/*
 * Below is the class definition for all dds components.
 * A table of these classes based on type is in dds.c.
 * Each component exports only the class definition for
 * the item.
 */

typedef union dds_internal_defn ddsInternal;
typedef struct dds_class_defn ddsClass;

struct dds_class_defn {
    ddsType type;		/* Type of class  */
    ddsHandle (*_dds_new)	/* Make a new one */
      ARGS((ddsInternal *parent, ddsClass *class, ddsData data));
    void (*_dds_get)		/* Get info on existing one */
      ARGS((ddsInternal *item, ddsData data));
    void (*_dds_set)		/* Change existing one */
      ARGS((ddsInternal *item, ddsData data));
    void (*_dds_del)		/* Delete exising one  */
      ARGS((ddsInternal *item));
    Widget (*_dds_parent_widget) /* Widget for creation */
      ARGS((ddsInternal *parent, ddsInternal *child));
    void (*_dds_place)		/* Places subwidgets */
      ARGS((ddsInternal *parent, ddsInternal *item,
	    int num_widgets, WidgetList widgets, int vspan));
};

/*
 * All structures have the same two fields so that type
 * and parent information can be extracted without
 * knowing the specifics about the component.
 */

typedef struct dds_shared_defn {
    ddsClass *class;		/* Type and class routines */
    ddsInternal *parent;	/* Parent component        */
} ddsShared;

typedef struct dds_full_initialize_defn {
    ddsShared base;		/* Shared information         */
    XtAppContext context;	/* Application context        */
    Widget app_shell;		/* Application shell          */
} ddsFullInitialize;

typedef struct dds_focus_item_defn {
    Widget w;			/* Widget itself */
    XtCallbackProc cb;		/* Callback      */
    XtPointer client;		/* Closure       */
} ddsFocusItem;

typedef struct dds_focus_defn {
    int alloc;			/* Allocated size        */
    int count;			/* Current number        */
    int current;		/* Current focus         */
    ddsFocusItem *list;		/* List of focus widgets */
    Widget def_focus;		/* Widget for default focus */
} ddsFocus;

typedef struct dds_top_defn {
    Widget top_shell;		/* Top level pop-up shell */
#ifdef FOCUS
    ddsFocus *focus;		/* Focus info             */
#ifdef ACCELERATORS
    ddsBoolean installed_p;	/* Accelerators installed */
#endif /* ACCELERATORS */
#endif /* FOCUS */
} ddsTop;

typedef struct dds_full_composite_defn {
    ddsShared base;		/* Shared information         */
    ddsTop *top_info;		/* Zero for underlings        */
    Widget overallTable;	/* Overall table              */
    Widget title;		/* Title of composite         */
    Widget bodyTable;		/* Table of body widgets      */
    Widget controlTable;	/* Table of control buttons   */
    int cur_row, cur_col;	/* Current spot for placement */
    int max_vspan;		/* Maximum vertical span      */
    int last_control;		/* Last control button column */
    ddsComposite user_spec;	/* Copy of user spec         */
} ddsFullComposite;

typedef struct dds_full_item_list_defn {
    ddsShared base;		/* Shared information        */
    Widget optVport;		/* Optional viewport         */
    Widget bodyTable;		/* Table of items            */
    Widget leader;		/* Leader for exclusive      */
    WidgetList checkBoxes;	/* Toggle widget children    */
    WidgetList checkLabels;	/* Label children            */
    ddsItemList user_spec;	/* Copy of user spec         */
} ddsFullItemList;

typedef struct dds_full_edit_text_defn {
    ddsShared base;		/* Shared information        */
    Widget label;		/* Label on left             */
    Widget text;		/* Editable text on right    */
    ddsEditText user_spec;	/* Copy of user spec         */
} ddsFullEditText;

typedef struct dds_full_enumerate_defn {
    ddsShared base;		/* Shared information        */
    Widget label;		/* Label on left             */
    Widget value;		/* Basic value               */
    Widget simpleMenu;		/* Pop up menu               */
    WidgetList sub_items;	/* Menu items themselves     */
    ddsEnumerate user_spec;	/* Copy of user spec         */
} ddsFullEnumerate;

typedef struct dds_full_show_text_defn {
    ddsShared base;		/* Shared information        */
    Widget text;		/* Displayed text            */
    int real_rows, real_cols;	/* Actual size               */
    char *real_buf;		/* Actual text displayed     */
    ddsShowText user_spec;	/* User specification        */
} ddsFullShowText;

typedef struct dds_full_number_defn {
    ddsShared base;		/* Shared information          */
    Widget label;		/* Label on left               */
    Widget number;		/* Value in middle             */
    Widget bar;			/* Scroll bar for manipulation */
    ddsNumber user_spec;	/* User specification          */
} ddsFullNumber;

typedef struct dds_full_control_defn {
    ddsShared base;		/* Shared information          */
    Widget button;		/* Control button itself     */
    ddsControl user_spec;	/* User specification        */
} ddsFullControl;

typedef struct dds_full_extern_defn {
    ddsShared base;		/* Shared information */
    ddsExtern user_spec;	/* User specification */
} ddsFullExtern;

union dds_internal_defn {
    ddsShared base;		/* Shared information */
    ddsFullInitialize initialize; /* DDS_INITIALIZE */
    ddsFullComposite composite;	/* DDS_COMPOSITE, DM_TOP */
    ddsFullItemList item_list;	/* DDS_ITEM_LIST */
    ddsFullEditText edit_text;	/* DDS_EDIT_TEXT */
    ddsFullEnumerate enumerate;	/* DDS_ENUMERATE */
    ddsFullShowText show_text;	/* DDS_SHOW_TEXT */
    ddsFullNumber number;	/* DDS_NUMBER    */
    ddsFullControl control;	/* DDS_CONTROL   */
    ddsFullExtern ext;		/* DDS_EXTERN    */
};

/*
 * ----------------------------------------------------------------------
 * Exported classes
 * ----------------------------------------------------------------------
 */

extern ddsClass _dds_initialize;	/* dds_initialize.c */
extern ddsClass _dds_top_composite; 	/* dds_composite.c */
extern ddsClass _dds_norm_composite; 	/* dds_composite.c */
extern ddsClass _dds_item_list; 	/* dds_item_list.c */
extern ddsClass _dds_edit_text; 	/* dds_edit_text.c */
extern ddsClass _dds_enumerate; 	/* dds_enumerate.c */
extern ddsClass _dds_show_text; 	/* dds_show_text.c */
extern ddsClass _dds_number;		/* dds_number.c    */
extern ddsClass _dds_control;		/* dds_control.c   */
extern ddsClass _dds_extern;		/* dds_extern.c    */

/*
 * ----------------------------------------------------------------------
 * Global routines
 * ----------------------------------------------------------------------
 */

extern void _dds_destroy_callback();
  /* Used for destroy callbacks on widgets */

extern void _dds_set_init
  ARGS((ddsInternal *init));
  /* Sets the initial item */

extern ddsInternal *_dds_get_init
  ARGS((ddsBoolean must_exist_p));
  /* Gets the current initializer */

extern ddsInternal *_dds_find_top
  ARGS((ddsInternal *item));
  /* Returns the top of the dialog */

/*
 * ----------------------------------------------------------------------
 * Focus management -- see dds_focus.c
 * ----------------------------------------------------------------------
 */

#ifdef FOCUS

extern void _dds_focus_init
  ARGS((ddsInternal *item));
  /* Initializes focus management */

extern void _dds_default_focus
  ARGS((ddsInternal *item, Widget widget));
  /* Sets the default focus widget if there are no real focus widgets */

extern void _dds_focus_widget
  ARGS((ddsInternal *item, Widget widget, XtCallbackProc cb, XtPointer client));
  /* Adds a widget to the focus list */

extern void _dds_reset_focus
  ARGS((ddsInternal *item));
  /* Resets the state of the focus */

extern void _dds_advance_focus
  ARGS((ddsInternal *item));
  /* Advances focus by one field */

extern void _dds_retreat_focus
  ARGS((ddsInternal *item));
  /* Moves focus back one field */

extern void _dds_set_focus
  ARGS((ddsInternal *item, Widget widget));
  /* Sets focus to given widget */

extern void _dds_free_focus
  ARGS((ddsInternal *item));
  /* Frees focus information */

#ifdef ACCELERATORS

extern void _dds_install_accelerators
  ARGS((ddsInternal *item));
  /* Installs accelerators */

#endif /* ACCELERATORS */

#endif /* FOCUS */

void dds_add_cursor_widget
  ARGS((Widget w, String res));
  /* Registers a new widget/cursor-resource pair */
void dds_remove_cursor_widget
  ARGS((Widget w, String res));
  /* Unregisters a widget/cursor-resource pair */

#endif /* _DDS_INTERNAL_H_ */

