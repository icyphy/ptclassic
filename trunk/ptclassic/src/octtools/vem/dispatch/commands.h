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
 * VEM Command Header File
 *
 * David Harrison
 * University of California, Berkeley
 * 1986
 *
 * This file contains definitions a CAD programmer should include
 * when defining new tightly bound VEM commands.  It contains definitions
 * for interpreting the argument list constructed by VEM.  Files
 * which should be included before this one:
 *    <X/Xlib.h>    C language interface to X
 *    "general.h"   VEM general definitions
 *    "buffer.h"    Buffer management routines
 *    "oct.h"	    The OCT database definitions
 */

#ifndef COMMANDS
#define COMMANDS

#include "ansi.h"
#include "port.h"
#include <X11/Xlib.h>
#include "general.h"
#include "buffer.h"
#include "oct.h"

/*
 * Argument types:
 */

#define	VEM_POINT_ARG	0x0	/* One or more points   */
#define VEM_BOX_ARG	0x1	/* One or more boxes    */
#define VEM_LINE_ARG	0x2	/* One or more lines    */
#define VEM_TEXT_ARG	0x4	/* A single string      */
#define VEM_OBJ_ARG	0x8	/* A bag of OCT objects */

/*
 * Argument types VEM_POINT_ARG, VEM_BOX_ARG, and VEM_LINE_ARG consist of
 * an array of pointers to the following structure.  For boxes,
 * the array should be interpreted as pairs of points specifing
 * the lower left and upper right of the box.  For lines,  the
 * array should be interpreted as a ordered set of points specifing
 * a path.
 */

typedef struct vemPntArg {
    Window theWin;		/* X Window containing mouse        */
    int x, y;			/* Physical location in window      */
    octId theFct;		/* Facet point lies in              */
    struct octPoint thePoint;	/* Virtual location snapped to grid */
} vemPoint;

/*
 * Arguments of type VEM_OBJ_ARG are pointers to the structure
 * given below:
 */

typedef struct vemObjArgType {
    octId theFct;		/* Facet the objects belong to    */
    octId theBag;		/* OCT_BAG of main objects        */
    struct octTransform tran;	/* Transformation of objects      */
    atrHandle attr;		/* Attributes for display         */
    octId selBag;		/* OCT_BAG of highlighted objects */
} vemObjects;

/*
 * The basic element of a command argument list is given below.
 * It is a union of the possible values VEM_POINT_ARG, VEM_BOX_ARG,
 * VEM_LINE_ARG, VEM_STRING_ARG, and VEM_OBJ_ARG.  The 'length'
 * field is defined differently for each of the types above.
 * For points, lines,  and boxes,  it specifies the number of points in the 
 * array of vemPoint structures.  For strings, it gives the string 
 * length (in bytes).  For collections of objects,  it gives the 
 * number of objects.
 */

typedef struct vem_onearg {
    int argType;		/* Argument type (given above) */
    int length;			/* Number of elements          */
    int alloc_size;		/* Allocated size of `points'  */
    union args {
	vemPoint *points;	/* VEM_POINT_ARG, VEM_BOX_ARG,
				 * VEM_LINE_ARG                */
	STR vemTextArg;		/* VEM_TEXT_ARG                */
	vemObjects vemObjArg;	/* VEM_OBJ_ARG                 */
    } argData;
} vemOneArg;
	
/*
 * Function declarations exported by command table
 */

extern int vemNumFuncs();
   /* Returns number of functions in command array */

extern vemStatus vemGetFunc
  ARGS((int idx, STR name, STR dispName, STR helpMsg, vemStatus (**func)()));
   /* Returns the function located at position `idx' */

extern vemStatus vemLookUpFunc
  ARGS((STR name, vemStatus (**funcPointer)()));
  /* Looks up unique id `name' in the table and returns pointer */

#endif
