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
 * Header file for VEM Dialog Interface Routines
 *
 * David Harrison
 * University of California,  Berkeley
 * 1986, 1989, 1990
 *
 * This file contains definitions for using the dialog interface
 * routines found in vemDM.c.  Other files which should be
 * included before this one:
 *    general.h		VEM general declarations
 *
 * Now implemented on top of the even more powerful dds interface.
 * This is kept for compatibility.
 */

#ifndef VEMDM
#define VEMDM

#include "ansi.h"
#include "general.h"
#include "dds.h"

extern char *vemdm_pkg_name;

typedef struct dm_which_item {
    STR itemName;		/* Displayed name               */
    Pointer userData;		/* Associated user data         */
    int flag;			/* Non-zero if user selected it */
} dmWhichItem;


typedef struct dm_which_text {
    STR itemPrompt;		/* Type-in prompt                */
    int rows, cols;		/* Size of type-in               */
    STR value;			/* Optional Default value        */
    Pointer userData;		/* Optional Associated user data */
} dmTextItem;

typedef struct dm_enum_item {
    STR itemPrompt;		/* Enumerated prompt      */
    int ni;			/* Number of values       */
    STR *enum_vals;		/* Enumerated values      */
    int selected;		/* Index to selected item */
    Pointer userData;		/* Optional data          */
} dmEnumItem;

typedef struct dm_num_item {
    STR itemPrompt;		/* Prompt for value    */
    double min, max;		/* Minimum and maximum */
    double step;		/* Increment           */
    double value;		/* Value displayed     */
    char *format;		/* Format for number (like printf) */
    Pointer userData;		/* Optional data       */
} dmNumItem;

extern void dmCompatInit
  ARGS((int (*evt_handler)()));
   /* Initializes dm compatibility package */

extern vemStatus dmMultiWhich
  ARGS((STR title, int count, dmWhichItem *items, int (*item_func)(), STR help_buf));
   /* Select zero or more from list of items */

extern vemStatus dmWhichOne
  ARGS((STR title, int count, dmWhichItem *items, int *itemSelect,
	int (*item_func)(), STR help_buf));
   /* Select exactly one from a list of items */

extern vemStatus dmMultiText
  ARGS((STR title, int count, dmTextItem *items));
   /* Multiple text type-in fields */

extern vemStatus dmMultiEnum
  ARGS((STR title, int count, dmEnumItem *items, STR help_buf));
   /* Asks for enumerated values */

extern vemStatus dmMultiNumber
  ARGS((STR title, int count, dmNumItem *items, STR help_buf));
   /* Asks for numerical values in a range */

extern vemStatus dmConfirm
  ARGS((STR title, STR message, STR yesButton, STR noButton));
   /* Present a confirmation dialog */

extern void dmMessage
  ARGS((STR title, STR message, int auto_format));
   /* Show text in an unmoded dialog */

extern ddsHandle dmHelpButton
  ARGS((ddsHandle parent, char *help_buf));
   /* Add help control button to unmoded dialog */

#endif
