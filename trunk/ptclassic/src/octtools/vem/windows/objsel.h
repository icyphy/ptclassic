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
 * Object Selection Set Header
 *
 * David Harrison
 * University of California,  Berkeley
 * 1987
 *
 * This file contains header definitions for mulitple object selection
 * set highlighting.  The implementation of the module is in the
 * file objsel.c.  The module exports routines for highlighting
 * any number of objects using any number of colors in all
 * appropriate windows.
 *
 * The philosophy used in this module is based on the vemObjects
 * structure defined in commands.h.  The module supplies routines
 * for modifying this structure that also have the side-effect
 * of causing the appropriate highlighting to occur.  This is
 * a low-level interface used by the selection and highlighting
 * modules.
 */

#ifndef OBJSEL_HEADER
#define OBJSEL_HEADER

#include "commands.h"		/* Dispatch module */

extern void objOn
  ARGS((vemObjects *objset));
  /* Turns on the highlighting of objects in a set */

extern void objOff
  ARGS((vemObjects *objset));
  /* Turns off the highlighting of objects in a set */

extern void objAdd
  ARGS((vemObjects *objset, octObject *obj));
  /* Adds a new object to an existing set   */

extern void objSub
  ARGS((vemObjects *objset, octObject *obj));
  /* Removes an object from an existing set */

extern void objRepl
  ARGS((vemObjects *objset, octObject *oldobj, octObject *newobj));
  /* Special routine to replace an object   */

extern void objTran
  ARGS((vemObjects *objset, struct octTransform *newtran));
  /* Changes the transformation of a set   */

extern vemStatus objMove
  ARGS((vemObjects *objset, octCoord xoff, octCoord yoff));
  /* Attempts to move object set as a unit */

extern vemStatus objBB
  ARGS((vemObjects *objset, struct octBox *bb));
  /* Returns the bounding box of a set     */

extern vemStatus objTranBB
  ARGS((vemObjects *objset, struct octBox *bb));
  /* Returns transformed bounding box      */

extern void objRedraw();
  /* Redraws selected objects in a region */

#endif
