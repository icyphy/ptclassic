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
 * VEM Selection Set Header File
 *
 * David Harrison
 * University of California,  Berkeley
 * 1986
 *
 * This file contains headers for the routines implemented in
 * selset.c.  The following files should be included before
 * this one:
 *    oct.h
 */

#ifndef SELSET
#define SELSET

#include "oct.h"
#include "commands.h"

extern vemOneArg *vemAttrSelSet();
   /* Creates a new selected set with predefined attributes */
extern vemOneArg *vemNewSelSet();
   /* Creates a new selected set with some specified attributes */
extern int vemFreeAttrSet();
   /* Frees the selected set and returns predefined attributes */
extern int vemFreeSelSet();
   /* Frees the selected set AND any predefined attributes     */

extern int vemClearSelSet();
   /* Deselects all items in a specified selected set           */
extern int vemAddSelSet();
   /* Adds a specified OCT object to a selected set             */
extern int vemDelSelSet();
   /* Removes a specified OCT object from a selected set        */
extern int vemRCSelSet();
   /* Special routine which replaces an object in a set         */
extern int vemZoomSelSet();
   /* Zooms a specified window to contain only selected items    */

#endif
