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
 * VEM Header File for the Bindings Package
 *
 * David Harrison
 * University of California,  Berkeley
 * 1986
 *
 * This file contains the definitions required for use of the
 * bindings package.
 */

#ifndef BINDINGS
#define BINDINGS

#include "ansi.h"
#include "general.h"
#include <X11/Xlib.h>
#include "xpa.h"

/* This can be typedef'ed to void if supported */
typedef struct bd_dummy_defn {
    int dummy;			/* Not used */
} bd_dummy;

typedef bd_dummy *bdTable;	/* Binding table handle */

extern vemStatus bdRegFunc
  ARGS((STR id, STR dispName, STR helpStr, vemStatus (*func)()));
   /* Registers a new tightly bound function suitable for binding     */

extern vemStatus bdRegApp
  ARGS((STR id, STR dispName, STR hostname, STR pathname));
   /* Registers a new remote application suitable for binding         */

extern vemStatus bdRegAppFunc
  ARGS((STR id, STR dispName, int appNumber, int appIndex));
   /* Registers a new remote application command suitable for binding */

extern vemStatus bdCreate
  ARGS((STR tableName, bdTable *theTable));
   /* Creates a new bindings table */

extern vemStatus bdInit
  ARGS((STR fileName));
   /* Reads the bindings in a bindings file */

extern vemStatus bdGetTable
  ARGS((STR tableName, bdTable *theTable));
   /* Gets a binding table by name */

extern vemStatus bdBindKey
  ARGS((bdTable theTable, STR id, int commandKey));
   /* Adds a new key binding to a specified binding table */

extern vemStatus bdBindMenu
  ARGS((bdTable, STR id, STR paneName, int select));
   /* Adds a new menu binding to a specified binding table */

extern vemStatus bdBindAlias
  ARGS((bdTable theTable, STR id, STR name));
   /* Adds a new alias to a specified binding table */

/*
 * The following definitions are for routines which request the
 * binding of a function given a key, menu, or alias.
 */

/* Binding options */

#define NCK	0x01		/* Don't type check command */

/* Binding types */

#define TIGHT_COMMAND	0x01	/* Tightly bound command      */
#define INVOKE_REMOTE	0x02	/* Start a remote application */
#define REMOTE_COMMAND	0x03	/* Remote application command */

/* For new remote applications */

typedef struct bd_newApp {
    STR hostName;		/* Name of host                */
    STR pathName;		/* Absolute path to executable */
} bdRemoteApplication;

/* For remote application commands */

typedef struct bd_appFun {
    int appNum;			/* Application number          */
    int appIndex;		/* Index of function           */
} bdRemoteCommand;

/* Basic binding */

typedef struct bd_bind {
    STR id;			/* Unique string identifier    */
    STR dispName;		/* Display name for command    */
    int bindType;		/* One of the types above      */
    union {
	vemStatus (*assocFunc)(); 	/* TIGHT_COMMAND   */
	bdRemoteApplication theApp;	/* INVOKE_REMOTE   */
	bdRemoteCommand theFun;		/* REMOTE_COMMAND  */
    } bindValue;
    bdTable bd_tbl;		/* Binding table for trace back */
    STR help_str;		/* One line help message        */
} bdBinding;

extern vemStatus bdKey
  ARGS((bdTable theTable, int keyChar, bdBinding *binding));
   /* Returns a binding for the given key */

extern vemStatus bdName
  ARGS((bdTable Tbl1, bdTable Tbl2, STR commandName, int *number,
	STR completion, bdBinding *binding));
   /* Returns a binding for a given name */

extern vemStatus bdMenu
  ARGS((bdTable theTable, int depth, int *select,
	STR *commandName, bdBinding *binding));
   /* Returns a binding for a given menu pane and selection item */

extern vemStatus bdGetMenu
  ARGS((Display *disp, bdTable theTable, xpa_menu *theMenu,
	int **last_num, int ***last_sel));
   /* Returns an X menu for a previously created binding table */

extern vemStatus bdQuery
  ARGS((bdBinding *cmd, STR *keys, STR *panes, STR *aliases));
   /* Returns the current key, menu, and alias bindings for a command */

extern vemStatus bdDestroy
  ARGS((bdTable theTable));
   /* Deletes a specified binding table */

#endif
