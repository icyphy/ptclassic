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
 * VEM Utility Routine Header File
 *
 * David Harrison
 * University of California,  Berkeley
 * 1986
 *
 * This file contains definitions of various utility routines
 * defined in vemUtil.c.  Include files which should be
 * included before this one:
 *    "general.h"	General VEM definitions
 */

#ifndef VEMUTIL
#define VEMUTIL

#include "ansi.h"
#include "general.h"
#include <X11/Xlib.h>
#include "st.h"
#include "oct.h"
#include "commands.h"
#include "list.h"

#define VEMTEMPBAG	";;; VEM Temporary Bag ;;;"

/*
 * openwin.c
 * Window opening assistance
 */

extern vemStatus vuOpenWindow
  ARGS((Window *newXWindow, STR prompt, octObject *newFacet, STR geoSpec));
   /* Opens a new window using the standard XCreate routine */
extern vemStatus vuMasterWindow
  ARGS((Window *newXWindow,octObject *newFacet,octObject *oldInst,STR geoSpec));
   /* Opens a new window looking at master of `oldInst' */
extern vemStatus vuCopyWindow
  ARGS((Window *newXWindow, STR prompt, Window win, struct octBox *ext));
   /* Copies a previously opened window again using XCreate */
extern vemStatus vuRPCWindow
  ARGS((Window *newXWindow, STR prompt, octObject *newFacet,
	STR rpcSpec, STR geoSpec));
   /* Opens a window and starts and RPC application in it   */

/*
 * parse.c
 * Parsing assistance routines
 */

extern vemStatus vuParseFacet
  ARGS((STR facetSpec, octObject *theFacet));
   /* Parses a string facet specification */
extern vemStatus vuParseInst
  ARGS((STR masterSpec, octObject *inst, STR *rtn));
   /* Parses an instance master specification */
extern vemStatus vuCkStyle
  ARGS((vemPoint *spot, lsList cmdList));
   /* Checks for edit style inconsistencies */
extern vemStatus vuCkBuffer
  ARGS((lsList cmdList, octId *fctId));
   /* Checks to make sure everything is in same buffer */
extern vemStatus vuParseLayer
  ARGS((vemPoint *spot, lsList cmdList, octObject *sptFacet, octObject *lyr));
   /* Parses a layer specification from a command string */

/*
 * octassist.c
 * Oct assistance routines
 */

extern octStatus vuCreateTemporary
  ARGS((octObject *container, octObject *object));
   /* Convenience routine for creating temporary objects */
extern octStatus vuTemporary
  ARGS((octObject *fct, octObject *object));
   /* Creates objects connected to standard bag */
extern vemStatus vuFindLayer
  ARGS((octObject *theFacet, struct octPoint *spot, char *layerName));
   /* Finds the layer of an object under a location */
extern vemStatus vuAddArgToBag
  ARGS((octObject *fct, octObject *theBag, vemOneArg *arg, octObject *set_bag,
	st_table *off_layers));
   /* Adds an argument list item to a bag */
extern vemStatus vuAddTermToBag
  ARGS((octObject *fct, octObject *theBag, vemOneArg *arg, octObject *set_bag,
	Pointer extra));
   /* Adds terminals of items to a bag */
extern vemStatus vuAddLyrToBag
  ARGS((octObject *fct, octObject *theBag, vemOneArg *arg, octObject *set_bag,
	char *lyr_name));
   /* Adds an argument list item to a bag conditionalized on a layer */
extern vemStatus vuSubArgFromBag
  ARGS((octObject *fct, octObject *theBag, vemOneArg *arg, octObject *set_bag,
	st_table *off_layers));
   /* Similar to vuAddArgToBag but conditionalizes on things already added */
extern vemStatus vuFindSpot
  ARGS((vemPoint *spot, octObject *rtnObj, octObjectMask mask));
   /* Finds geometry at a location subject to a mask */
extern vemStatus vuFindBag
  ARGS((vemPoint *spot, octObject *rtnBag));
   /* Interactively queries for a destination bag */
extern vemStatus vuAddBag
  ARGS((octObject *bag, octObject *addBag));
   /* Adds items to a bag */
extern vemStatus vuSubBag
  ARGS((octObject *bag, octObject *subBag));
   /* Subtracts items from a bag */
extern vemStatus vuMinusLayer
  ARGS((struct octObject *bag, struct octObject *layer));
   /* Subtracts items on a layer from a bag */
extern vemStatus vuObjArgParse
  ARGS((vemPoint *spot, lsList cmdList, octId *theFctId, octObject *finalBag,
	int layerflag));
   /* Parses a complex VEM argument list */
extern vemStatus vuOffSelect
  ARGS((vemOneArg *arg));
   /* Turns off selection of an object set on the arg list */
extern vemStatus vuWipeList
  ARGS((lsList someList));
   /* Wipes out all information on the argument list */
extern char *vuDispObject
  ARGS((octObject *obj));
   /* Returns textual representation of an object */

/*
 * other.c
 * General purpose routines
 */

extern int uniq
  ARGS((char *base, int nel, int width, int (*compar)()));
   /* Takes a sorted list and removes the duplicate entries */
extern char *vuMakeName
  ARGS((octObject *theObj));
   /* Allocates and returns a simple text representation of an object */
extern STR vuCellName
  ARGS((octObject *cell));
   /* Returns a nicely formatted name for a cell */

/*
 * Color utilities
 */
extern unsigned long vuWhite
  ARGS((XColor *clr));
   /* Returns white pixel */
extern unsigned long vuBlack
  ARGS((XColor *clr));
   /* Returns black pixel */

#endif
