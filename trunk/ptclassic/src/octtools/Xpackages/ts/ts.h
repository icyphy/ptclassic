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
 * Typescript Library - Header definitions
 *
 * David Harrison
 * University of California, Berkeley
 * 1988, 1990
 *
 * This is a library built on top of the AsciiText widget in the
 * Athena widget set that provides typescript capabilities.  It
 * opens a new top-level shell with an AsciiText widget in it.
 * The user supplies a callback function which is called when
 * any key is pressed in the window.  The package exports a function
 * for writing characters onto the end of the buffer of the typescript
 * window.  This function also supports deleting characters from
 * the end of the buffer.  Selection operations similar to those
 * found in xterm are supported.
 *
 * Initially,  this is not a formal subclass of asciiTextWidget.
 * Eventually,  I imagine it could be made into one.
 */

#ifndef _TYPESCRIPT_H_
#define _TYPESCRIPT_H_

#include "ansi.h"
#include <X11/Intrinsic.h>
#include <X11/Xaw/AsciiText.h>

typedef struct ts_call_data_defn {
    int len;			/* Length in bytes */
    char *buf;			/* Buffer          */
} ts_call_data;

extern Widget tsCreate
  ARGS((XtAppContext app, String name, Widget parent, Cardinal bufsize,
	XtCallbackProc callback, caddr_t client_data,
	ArgList args, Cardinal num_args));

extern int tsWriteChar
  ARGS((Widget w, int ch));

int tsWriteStr
  ARGS((Widget w, String str));

#endif /* _TYPESCRIPT_H_ */
