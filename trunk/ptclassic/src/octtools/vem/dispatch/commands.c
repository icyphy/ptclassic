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
/*LINTLIBRARY*/
/*
 * VEM Command Table
 *
 * David Harrison
 * University of California, Berkeley
 * 1986
 *
 * This file contains a table of command name/function pointer
 * mappings used by the bindings module.  VEM provides two
 * mechanisms for extension:  the addition of tighly bound
 * commands and the remote process communication protocol.
 * This file contains definitions for the former.  The latter
 * is discussed elsewhere.
 *
 * Tightly bound VEM commands are commands which run in the VEM
 * address space.  All of these functions have the following
 * form:
 *
 * vemStatus command(spot, cmdList)
 * vemPoint spot;	
 * vemList cmdList;	
 *
 * 'spot' is where the mouse was located when the command was
 * selected.  If the command was selected using a menu,  it is
 * where the mouse was located BEFORE the menu was popped up.
 * 'cmdList' is a standard VEM list of pointers to the command
 * argument structure.  Both 'Point' and the command argument
 * structure are defined in commands.h.
 *
 * To add a new tightly bound command,  do the following:
 *   1.  Compile and link (and preferably test) the function
 *       you wish to add.
 *   2.  Add the function to the list of external functions
 *       listed under "VEM Commands" below.
 *   3.  Determine a command name for the function and add
 *       the name/function pointer pair to the table
 *       labeled "VEM Function Pointer Mappings" below.
 *   4.  Decide upon appropriate key and menu bindings and
 *       add them to a local copy of the standard VEM bindings
 *       file.
 *   5.  Make sure VEM reads the newly defined bindings by
 *       adding the name of your bindings file to the vem .Xdefaults
 *       options under the name "vem.bindings".
 *   6.  Link your functions against the provided VEM object
 *       files to produce a new VEM executable.  This VEM
 *       should execute your newly defined commands properly.
 */

#include "commands.h"		/* Argument list definitions */

/* For the function pointer mapping: */

typedef struct func_mapping_defn {
    STR name;			/* Command name     */
    STR dispName;		/* Display name     */
    vemStatus (*func)();	/* Function pointer */
    STR helpStr;		/* Help string      */
} func_mapping;



/*
 * VEM Commands
 */

extern vemStatus vemVersionCmd();/* Displays the VEM version string */
extern vemStatus browserCmd();	/* Start a cell browser (browser.c) */
extern vemStatus redrawCmd();	/* Redraws a window (vem.c)       */
extern vemStatus pushCmd();	/* Pushes context (vem.c)         */
extern vemStatus popCmd();	/* Pops the context (vem.c)       */
extern vemStatus vemBindCmd();	/* Shows bindings (vem.c)         */
extern vemStatus bdOutBindings();/* Shows all bindings (bindings.c) */
extern vemStatus delOneItemCmd(); /* Delete argument item (basic.c) */
extern vemStatus delOneArgCmd();/* Delete whole argument (basic.c) */
extern vemStatus intrCmd();	/* Deactivate window (basic.c)    */
extern vemStatus zoomInCmd();	/* Zoom in  (basic.c)             */
extern vemStatus zoomOutCmd();	/* Zoom out (basic.c)             */
extern vemStatus eqWinCmd();	/* Make windows same scale (basic.c) */
extern vemStatus fullCmd();	/* Zoom out to include everything */
extern vemStatus panCmd();	/* Pan      (basic.c)             */
extern vemStatus winMenuCmd();  /* Display palatte of layers (basic.c) */
extern vemStatus listPalettes();/* List possible palettes (basic.c) */
extern vemStatus newWinCmd();	/* Create new window (basic.c)    */
extern vemStatus delWinCmd();	/* Delete window or exit (basic.c)*/
extern vemStatus cancelCmd();	/* Wipes argument list (basic.c)  */
extern vemStatus whereCmd();	/* Input point info (basic.c)     */
extern vemStatus expandCmd();	/* Toggle expand mode (basic.c)   */
extern vemStatus saveCmd();	/* Save buffer (basic.c)          */
extern vemStatus writeCmd();	/* Write the buffer (basic.c)     */
extern vemStatus saveAllCmd();	/* Save all buffers (basic.c)     */
extern vemStatus revertCmd();	/* Revert the buffer (basic.c)    */
extern vemStatus recoverCmd();	/* Recover alternate version (basic.c) */
extern vemStatus revertNoConf(); /* Revert no confirm (basic.c)   */
extern vemStatus deepRRCmd();	/* Recursive re-read (basic.c)    */
extern vemStatus replWinCmd();	/* Replace a buffer (basic.c)     */
extern vemStatus killAppCmd();	/* Kill an application (vemRPC.c) */
extern vemStatus killBufCmd();	/* Kill a buffer (basic.c)        */
extern vemStatus optWinCmd();   /* Window options (opts.c)        */
extern vemStatus optLECmd();	/* Edit displayed layers (layers.c)*/
extern vemStatus optToggleGridCmd(); /* Toggle grid (util.c)       */
extern vemStatus phyCreateCmd();/* New physical object (physical.c)  */
extern vemStatus phyReplaceCmd(); /* Replace physical object (physical.c) */
extern vemStatus phyCircleCmd(); /* Creates a new circle (physical.c) */
extern vemStatus phyLabelCmd();	/* Creates or edits labels (physical.c) */
extern vemStatus phyInstance();  /* Creates a new instance in physical */
extern vemStatus pathWidthCmd();/* Sets the physical path width   */
extern vemStatus phyTermCmd();	/* New formal term (physical.c)   */
extern vemStatus phyCngLayer(); /* Changes the layer of selected objects */
extern vemStatus phyDelCmd();	/* Delete obj (physical.c)        */
extern vemStatus phyMoveCmd();	/* Move a selected set (physical.c) */
extern vemStatus phyCopyCmd();  /* Copy a selected set (physical.c) */
extern vemStatus seCreateCmd(); /* Symbolic create (create.c)     */
extern vemStatus sePromote();	/* Promote a set of terminals     */
extern vemStatus seChangeSegs();/* Symbolic change segment (cseg.c) */
extern vemStatus seReplace();   /* Symbolic replace instance      */
extern vemStatus seDelete();	/* Symbolic deletion (symbolic.c) */
extern vemStatus seSelectNet();/* Select major net */
extern vemStatus seCopy();	/* Copy objwcts */
extern vemStatus seMove();	/* Copy and delete objects */
extern vemStatus seReconnect();	/* Reconnect instances (se_reconn.c) */
extern vemStatus showProp();	/* List properties   (property.c) */
extern vemStatus editProp();	/* Edit properties (property.c)   */
extern vemStatus bagSelect();	/* Select items in a bag (bags.c) */
extern vemStatus bagSelectCnts(); /* Select contents of bag (bags.c) */
extern vemStatus bagNew();	/* Create a new bag (bags.c)      */
extern vemStatus bagAttach();	/* Attach stuff to a bag (bags.c)  */
extern vemStatus bagDetach();   /* Detach stuff from a bag (bags.c) */
extern vemStatus selectCmd();	/* Add to selected set (selection.c) */
extern vemStatus selTermCmd();  /* Selects terminals (selection.c)  */
extern vemStatus selLayerCmd();	/* Selects by layer (selection.c)  */
extern vemStatus unSelectCmd();	/* Subtract from set (selection.c) */
extern vemStatus nameObjCmd();	/* Name an object (basic.c)       */
extern vemStatus selTransCmd(); /* Transform selected objects in place */
extern vemStatus altCmd();	/* Alternate buffer command (basic.c) */
extern vemStatus swapCmd();	/* Swap buffer command (basic.c)      */
extern vemStatus pushInstanceCmd();	/* Push into a instances' master */
extern vemStatus rpcAnyApplication();	/* Run any RPC application */
extern vemStatus rpcReset();	/* Reset the RPC running flag for a window */
extern vemStatus rpcSignalCmd(); /* Send signal to RPC application. */
extern vemStatus undoCmd();	/* Undo last operation */
extern vemStatus interfaceCmd(); /* Update interface  */
  

/* New schematic interface */
extern vemStatus schemCreateCmd();	/* schemCreate.c  */
extern vemStatus schemMove();	/* schemMvCp.c */
extern vemStatus schemCopy();	/* schemMvCp.c */


/* 
 * VEM Function Pointer Mappings
 *
 * The convention used here is the unique name for the function
 * is the same as its real function name while the display name
 * is an emacs like command name.
 */

func_mapping all_mappings[] = {
    { "vemVersionCmd", 		"version", 		vemVersionCmd,
      "Displays the current VEM version" 				},
    { "redrawCmd",		"redraw-window",	redrawCmd,
      "Redraws the window containing the cursor" 			},
    { "browserCmd",		"browser",		browserCmd,
      "Opens a new cell browser"					},
    { "pushCmd",		"push-context",		pushCmd,
      "Pushes the current argument list allowing you to enter new ones" },
    { "popCmd",			"pop-context",		popCmd,
      "Pops the current argument list replacing it with the pushed one" },
    { "vemBindCmd",		"bindings",		vemBindCmd,
      "Displays the key, menu, and type-in bindings for a command"      },
    { "bdOutBindings",		"log-bindings",		bdOutBindings,
      "Outputs all alias, menu, and key bindings to log file"		},
    { "delOneItemCmd",		"",			delOneItemCmd,
      "Deletes an item from the argument list"				},
    { "delOneArgCmd",		"",			delOneArgCmd,
      "Deletes a grouping of items from the argument list"		},
    { "intrCmd",		"interrupt",		intrCmd,
      "Interrupts a graphics window preventing redraw"			},
    { "zoomInCmd",		"zoom-in",		zoomInCmd,
      "Zooms in by a factor of two or to the provided box"		},
    { "zoomOutCmd",		"zoom-out",		zoomOutCmd,
      "Zooms out by a factor of two or in proportion to the provided box" },
    { "eqWinCmd",		"same-scale",		eqWinCmd,
      "Makes the window containing the mouse the same size as the one with a point" },
    { "panCmd",			"pan",			panCmd,
      "Pans the window so that the mouse (or point) is the new center"	},
    { "winMenuCmd",		"palette",		winMenuCmd,
      "Opens a window containing a menu of layers or instances"		},
    { "listPalettes",		"list-palettes",	listPalettes,
      "Displays list of available palettes"				},
    { "newWinCmd",		"open-window",		newWinCmd,
      "Creates a new graphics window from an old one or a cell specification" },
    { "delWinCmd",		"close-window",		delWinCmd,
      "Closes the window containing the mouse cursor"			},
    { "cancelCmd",		"",			cancelCmd,
      "Deletes the current argument list"				},
    { "expandCmd",		"toggle-expansion",	expandCmd,
      "Toggles between the `contents' and `interface' facets of instances" },
    { "fullCmd",		"show-all",		fullCmd,
      "Zooms the window to show all of the cell's contents"		},
    { "whereCmd",		"where",		whereCmd,
      "Displays information about the current mouse position"		},
    { "saveCmd",		"save-window",		saveCmd,
      "Saves the contents of the window containing the mouse on disk"	},
    { "writeCmd",		"write-window",		writeCmd,
      "Saves the contents of the window containing the mouse to an alternate location on disk"	},
    { "saveAllCmd",		"save-all",		saveAllCmd,
      "Allows you to choose which buffers of all buffers to save on disk" },
    { "revertCmd",		"re-read",		revertCmd,
      "Rereads the contents of the cell from disk"			},
    { "revertNoConf",		"rr-nc",		revertNoConf,
      "Rereads the contents of the cell without confirmation (dangerous)" },
    { "deepRRCmd",		"deep-reread",		deepRRCmd,
      "Rereads the contents of the cell recursively"			},
    { "recoverCmd",		"recover-facet",	recoverCmd,
      "Replaces current cell with an alternate version saved earlier." },
    { "replWinCmd",		"switch-facet",		replWinCmd,
      "Replaces the cell in  this window with one you specify",		},
    { "killBufCmd",		"kill-buffer",		killBufCmd,
      "Removes all traces of the cell in the window containing the mouse" },
    { "killAppCmd",		"kill-application",	killAppCmd,
      "Stops any RPC application running in the window containing the mouse" },
    { "nameObjCmd",		"name-objects",		nameObjCmd,
      "Changes the name of instances, nets, bags, properties, and terminals" },
    { "optWinCmd",		"window-options",	optWinCmd,
      "Configure window specific display options"                          },
    { "optLECmd",		"layer-display",	optLECmd,
      "Displays a dialog allowing you to change which layers are displayed" },
    { "optToggleGridCmd",	"toggle-grid",		optToggleGridCmd,
      "Toggle the visibility of the grid in the specified window"       },
    { "phyCreateCmd",		"create-geometry",	phyCreateCmd,
      "Creates new boxes, paths, polygons, and labels"			},
    { "phyReplaceCmd",		"alter-geometry",	phyReplaceCmd,
      "Replaces existing geometry with one you specify"			},
    { "phyCircleCmd",		"create-circle",	phyCircleCmd,
      "Creates new circles"						},
    { "phyLabelCmd",		"edit-label",		phyLabelCmd,
      "Creates or edits labels"						},
    { "phyInstance",		"create-instance",	phyInstance,
      "Places instances of other physical cells at a specified spot"    },
    { "pathWidthCmd",		"set-path-width",	pathWidthCmd,
      "Sets the default path width on a layer-by-layer basis"		},
    { "phyTermCmd",		"create-terminal",	phyTermCmd,
      "Creates a new formal terminal"					},
    { "phyCngLayer",		"change-layer",		phyCngLayer,
      "Changes the layer of existing geometry to one you specify"	},
    { "phyDelCmd",		"delete-physical",	phyDelCmd,
      "Deletes existing geometry"					},
    { "phyMoveCmd",		"move-physical",		phyMoveCmd,
      "Moves selected geometry to a new location or orientation"	},
    { "phyCopyCmd",		"copy-physical",		phyCopyCmd,
      "Copies selected geometry to a new location or orientation"	},
    { "seCreateCmd",		"create",		seCreateCmd,
      "Creates new instances, segments, or formal terminals"		},
    { "sePromote",		"promote",		sePromote,
      "Promotes a set of actual terminals selected using select-term"   },
    { "seChangeSegs",		"change-segment",	seChangeSegs,
      "Changes the layer and/or the width of selected segments"		},
    { "seReplace",		"replace-instance",	seReplace,
      "Replaces instances with other instances in symbolic"		},
    { "seReconnect",		"reconnect-instance",	seReconnect,
      "Checks and corrects instance connection problems"		},
    { "seDelete",		"delete-objects",	seDelete,
      "Deletes selected objects"					},
    { "seSelectNet",		"select-net",		seSelectNet,
      "Highlights the net containing the object under the cursor"	},
    { "seCopy",			"copy-objects",		seCopy,
      "Copies selected objects to a new location"			},
    { "seMove",			"move-objects",		seMove,
      "Moves selected objects to a new location"			},
    { "selectCmd",		"select-objects",	selectCmd,
      "Selects items inside boxes, under points, and that intersect lines"},
    { "selTermCmd",		"select-terms",		selTermCmd,
      "Selects formal terminals"					},
    { "selLayerCmd", 		"select-layer",		selLayerCmd,
      "Line select-objects but conditionalizes on a layer"		},
    { "unSelectCmd",		"unselect-objects",	unSelectCmd,
      "Unselects items that were previously selected (use control-W to unselect all)" },
    { "selTransCmd",		"transform",		selTransCmd,
      "Transforms selected objects in place according to supplied specification" },
    { "showProp",		"show-property",	showProp,
      "Displays the properties attached to object under cursor"		},
    { "editProp",		"edit-property",	editProp,
      "Edits properties attached to object under cursor"		},
    { "bagSelect",		"select-bag",		bagSelect,
      "Selects the bag containing the object under the cursor"		},
    { "bagSelectCnts",		"sel-bag-contents",	bagSelectCnts,
      "Selects the items in the bag containing the object under the cursor" },
    { "bagNew",			"create-bag",		bagNew,
      "Creates a new empty bag"						},
    { "bagAttach",		"attach-to-bag",	bagAttach,
      "Attaches selected items to a previously created bag"		},
    { "bagDetach",		"detach-from-bag",	bagDetach,
      "Detaches selected items from a previously created bag"		},
    { "updateInterface",        "update-interface",     interfaceCmd,
      "Updates the interface facet" 					},
    { "altCmd",			"alt-buffer",		altCmd,
      "Places another facet in the background of the window containing the cursor" },
    { "swapCmd",		"swap-bufs",		swapCmd,
      "Swaps the background and foreground facets of a window"		},
    { "pushInstanceCmd",	"push-master",		pushInstanceCmd,
      "Opens a new window looking at the master of instance under the cursor" },
    { "rpcAnyApplication",	"rpc-any",		rpcAnyApplication,
      "Starts an rpc application given host and path"			},
    { "rpcSignalCmd",		"rpc-signal",		rpcSignalCmd,
      "Send a signal to the RPC application" },
    { "rpcReset",		"rpc-reset",		rpcReset,
      "Resets the RPC state of a window if the application dies unexpectedly" },
    { "schemCreateCmd",		"create",		schemCreateCmd,
      "Creates new primitives, wires, and formal terminals"		},
    { "schemMove",		"move-objects",		schemMove,
      "Moves selected objects to a new location"			},
    { "schemCopy",		"copy-objects",		schemCopy,
      "Copys selected objects to a new location"			},
    { "undoCmd",		"undo",			undoCmd,
      "Undo the last operation"						}
};

#define	NUMFUNCS	(sizeof(all_mappings) / sizeof(func_mapping))



/*
 * Routine for accessing the table of functions directly (used
 * by the bindings module to register the functions).
 */

int vemNumFuncs()
/*
 * This routine returns the number of functions defined in the
 * function array.
 */
{
    return NUMFUNCS;
}

vemStatus vemGetFunc(idx, name, dispName, helpMsg, func)
int idx;			/* Between 0 and vemNumFuncs()-1   */
STR name;			/* Copies the name into this space */
STR dispName;			/* Copies display name here        */
STR helpMsg;			/* Copies help message here        */
vemStatus (**func)();		/* Returns the function pointer    */
{
    if ((idx >= 0) && (idx < NUMFUNCS)) {
	STRMOVE(name, all_mappings[idx].name);
	STRMOVE(dispName, all_mappings[idx].dispName);
	STRMOVE(helpMsg, all_mappings[idx].helpStr);
	*func = all_mappings[idx].func;
	return VEM_OK;
    } else return VEM_CORRUPT;
}

/*
 * General Purpose Lookup function 
 */

vemStatus vemLookUpFunc(name, funcPointer)
STR name;			/* Name of command             */
vemStatus (**funcPointer)();	/* Pointer to function pointer */
/*
 * Looks up 'name' in the table defined above.  If it is found,
 * it returns VEM_OK and sets 'funcPointer' to the proper value.
 * If it is not found,  it returns VEM_CANTFIND and 'funcPointer'
 * is set to NULL.  NOTE: I don't think anyone uses this.
 */
{
    int idx;

    for (idx = 0;  idx < NUMFUNCS;  idx++) {
	if (STRCOMP(name, all_mappings[idx].name) == 0) break;
    }
    if (idx < NUMFUNCS) {
	/* Found it */
	*funcPointer = all_mappings[idx].func;
	return VEM_OK;
    } else {
	/* Didn't find it */
	*funcPointer = 0;
	return VEM_CANTFIND;
    }
}
    
