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
 * External definitions for dialog description package
 *
 * David Harrison
 * University of California, Berkeley
 * 1990
 *
 * The package assumes at a minimum that you are using X11.  The
 * actual implementation uses the X toolkit and the athena widget
 * set also.
 */

#ifndef _DDS_H_
#define _DDS_H_

#include <X11/Xlib.h>

#include "ansi.h"
#include "port.h"
#include "utility.h"
#include "errtrap.h"

extern char *ddsPackageName;

/*
 * Names used for creating widgets
 */

#define DDS_DEFAULT_CLASS	"Dds"
#define DDS_POPUP_NAME		"dialog"
#define DDS_OVERALL_NAME	"overall"
#define DDS_TITLE_NAME		"title"
#define DDS_BODY_NAME		"body"
#define DDS_CONTROL_NAME	"control"
#define DDS_CONBTN_NAME		"button"
#define DDS_EDLBL_NAME		"description"
#define DDS_EDTEXT_NAME		"edit"
#define DDS_ILVPORT_NAME	"vport"
#define DDS_ILTBL_NAME		"ilbody"
#define DDS_ILEXBOX_NAME	"exclbox"
#define DDS_ILMULBOX_NAME	"mulbox"
#define DDS_ILEXLBL_NAME	"description"
#define DDS_ENLBL_NAME		"description"
#define DDS_ENVAL_NAME		"value"
#define DDS_ENMENU_NAME		"choices"
#define DDS_ENCHOICE_NAME	"choice"
#define DDS_SHOWTEXT_NAME	"showtext"
#define DDS_NUMLBL_NAME		"description"
#define DDS_NUMVAL_NAME		"numvalue"
#define DDS_NUMSCR_NAME		"valscroll"

/*
 * Control buttons added to composite items are named based on the
 * initial label defined for the button.
 */

/* Opaque type for dds components */
typedef struct dds_dummy_defn {
    int dummy;			/* Not used */
} *ddsHandle, *ddsData;

typedef enum dds_boolean_defn {
    DDS_FALSE, DDS_TRUE
} ddsBoolean;

typedef enum dds_type_defn {
    DDS_INITIALIZE,		/* (None)        */
    DDS_TOP,			/* ddsComposite  */
    DDS_COMPOSITE,		/* ddsComposite  */
    DDS_ITEM_LIST,		/* ddsItemList   */
    DDS_EDIT_TEXT,		/* ddsEditText   */
    DDS_ENUMERATE,		/* ddsEnumerate  */
    DDS_SHOW_TEXT,		/* ddsShowText   */
    DDS_NUMBER,			/* ddsNumber     */
    DDS_CONTROL,		/* ddsControl    */

    DDS_EXTERN,			/* ddsExtern     */
    DDS_LAST_TYPE		/* Internal use only */
} ddsType;

typedef enum dds_comp_just_defn {
    DDS_HORIZONTAL,		/* Layout items across */
    DDS_VERTICAL		/* Layout items down   */
} ddsCompJust; 

typedef struct dds_composite_defn {
    char *title;		/* Optional title           */
    ddsCompJust just;		/* Item layout direction    */
    int max_items;		/* Maximum number of items  */
} ddsComposite;

typedef struct dds_flag_item {
    char *label;		/* Label next to item      */
    ddsBoolean selected_p;	/* Whether or not selected */
    ddsData user_data;		/* User data for each item */
} ddsFlagItem;

typedef struct dds_item_list_defn {
    int num_items;		/* Number of items             */
    ddsFlagItem *items;		/* Item list 		       */
    ddsBoolean exclusive_p;	/* Flags operate exclusively   */
    int selected;		/* If exclusive, one chosen    */
    ddsBoolean scroll_p;	/* Scroll on overflow          */
    int max_items;		/* Maximum before scrolling    */
    void (*callback)		/* Function called on toggle   */
      ARGS((ddsHandle item_list, int toggled));
    ddsData user_data;		/* User data for callback */
} ddsItemList;

typedef struct dds_edit_text_defn {
    char *label;		/* Label text to field */
    char *text;			/* Editable text       */
    int rows, cols;		/* Size of text field  */
} ddsEditText;

typedef struct dds_enum_item_defn {
    char *label;		/* Label next to item    */
    int num_items;		/* Number of items       */
    char **items;		/* Array of possible values */
    int selected;		/* Selected value        */
    void (*callback)		/* Function called on change */
      ARGS((ddsHandle item, int previous));
    ddsData user_data;		/* User data for callback */
} ddsEnumerate;

typedef enum dds_st_disposition_defn {
    DDS_FROM_STRING,		/* Display text in string        */
    DDS_FROM_FILE		/* Interpret string as file name */
} ddsStDisposition;
			      
typedef struct dds_show_text_defn {
    ddsStDisposition where;	/* Where is text to display */
    int rows, cols;		/* Expected size of field   */
    char *text;			/* Text to display          */
    ddsBoolean word_wrap_p;	/* Turn on word wrap        */
    ddsBoolean compute_size_p;	/* Compute size             */
} ddsShowText;

typedef struct dds_number_defn {
    char *label;		/* Label for value */
    double minimum;		/* Minimum value */
    double maximum;		/* Maximum value */
    double increment;		/* Increment     */
    double value;		/* Actual value  */
    char *format;		/* Number display format */
    void (*callback)		/* Function called on change */
      ARGS((ddsHandle number));
    ddsData user_data;		/* User data for callback */
} ddsNumber;

typedef enum dds_end_type {
    DDS_CONTINUE,		/* Allow dialog to continue */
    DDS_DONE			/* Return control to caller */
} ddsEndType;

typedef struct dds_control_defn {
    char *name;			/* Button name               */
    char *accelerator;		/* Accelerator specification */
    void (*callback)		/* Function called on push   */
      ARGS((ddsHandle item));
    ddsData user_data;		/* User data for callback  */
} ddsControl;

typedef struct dds_extern_defn {
    int (*new_comps)		/* Creation callback */
      ARGS((ddsData parent, ddsData **cmps, int *vspan, ddsData user_data));
    void (*free_comps)		/* Callback for freeing list */
      ARGS((int num_comps, ddsData *cmps));
    ddsData user_data;		/* Uuser data for callback */
} ddsExtern;

typedef enum dds_pos_type_defn {
    DDS_ABSOLUTE,		/* Position at given location  */
    DDS_MOUSE,			/* Position relative to mouse  */
    DDS_WINDOW			/* Position relative to window */
} ddsPosType;

typedef enum dds_pos_vjust_defn {
    DDS_ABOVE, DDS_MIDDLE, DDS_BELOW
} ddsPosVJust;

typedef enum dds_pos_hjust_defn {
    DDS_LEFT, DDS_CENTER, DDS_RIGHT
} ddsPosHJust;

typedef struct dds_position_defn {
    ddsPosType pos;		/* How to position dialog   */
    ddsPosVJust vjust;		/* Vertical justification   */
    ddsPosHJust hjust;		/* Horizontal justification */
    int x, y;			/* Position for DDS_ABSOLUTE */
    Window win;			/* Window for DDS_WINDOW     */
} ddsPosition;

typedef struct dds_evt_handler_defn {
    int (*other_handler)	/* Routine to call       */
      ARGS((XEvent *evt, ddsData data));
    ddsData user_data;		/* User data to function */
} ddsEvtHandler;

/*
 * -------------------------------------------------------------------------
 * Procedural interface
 * -------------------------------------------------------------------------
 */

extern ddsBoolean dds_initialize
  ARGS((int *argc_ptr, char *argv[]));
  /* Normal initialization of package */

extern void dds_partial_init();
  /* Initialize assuming user has started up toolkit */

extern ddsHandle dds_component
  ARGS((ddsHandle parent, ddsType type, ddsData data));
  /* Creates a new dialog component */

extern ddsType dds_type
  ARGS((ddsHandle item));
  /* Returns type of component created using dds_component */

extern void dds_get
  ARGS((ddsHandle item, ddsData data));
  /* Returns data structure given handle */

extern void dds_set
  ARGS((ddsHandle item, ddsData data));
  /* Sets data structure given handle */

extern void dds_post
  ARGS((ddsHandle dialog, ddsPosition *pos, ddsEvtHandler *other));
  /* Posts dialog on screen at specified position */

extern void dds_unpost
  ARGS((ddsHandle item));
  /* Removes dialog from screen but it isn't destroyed */

extern void dds_destroy
  ARGS((ddsHandle item));
  /* Removes dialog from screen if necessary and destroys dialog */

extern int dds_loop
  ARGS((ddsEvtHandler *other));
  /* Loops handling all events */

extern int dds_outstanding
  ARGS((ddsEvtHandler *other));
  /* Handles all outstanding events then returns */

extern Window dds_window
  ARGS((ddsHandle component));

extern void dds_override_cursor
  ARGS((Cursor cursor));
  /* Sets the cursor temporarily for all dds dialogs */

/*
 * Fatal errors
 */

#define DDS_NOT_TOP		1	/* Top level not DM_TOP    */
#define DDS_BAD_TYPE		2	/* Unknown type            */
#define DDS_BAD_PARENT		3	/* Bad parent component    */
#define DDS_NOT_SUPPORTED	4	/* Unsupported operation   */
#define DDS_NOT_INIT		5	/* Component type not DM_INIT */
#define DDS_MULTI_INIT		6	/* Attempt to initialize twice */
#define DDS_INTERNAL		7	/* Internal error          */
#define DDS_BAD_VALUE		8	/* Bad enumerated value    */

#endif /* _DDS_H_ */
