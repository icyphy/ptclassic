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
 * VEM Cursor Management
 *
 * David Harrison
 * University of California,  Berkeley
 * 1987, 1989
 *
 * Definitions for using cursors routines defined in cursors.c
 */

#ifndef CURSOR_H
#define CURSOR_H

#include "ansi.h"
#include "general.h"
#include <X11/Xlib.h>

/* VEM cursor definitions */

#define	CONSOLE_CURSOR	0
#define CROSS_CURSOR	1
#define BUSY_CURSOR	2
#define GRAB_CURSOR	3
#define END_CURSORS	4

extern Cursor vemCursor
  ARGS((int curName));
   /* Returns one of the named cursors above */

extern void vemBusyWindow
  ARGS((Window win));
   /* Adds the specified window to the list of windows that may be busy */

extern void vemBusy();
   /* Sets the cursors to busy state */

extern void vemNotBusy();
   /* Sets the cursors to not busy */

#endif /* CURSOR_H */
