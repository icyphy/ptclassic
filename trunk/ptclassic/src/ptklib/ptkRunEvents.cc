static const char file_id[] = "ptkRunEvents.cc";
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

 Programmer: Alan Kamas
 Created: 1/30/95

This file contains the procedure to process Tcl/Tk events while a
ptolemy Run is taking place.  This uses the Polling code in the
SimControl object.

*/
   
#ifdef PTSVR4
/* ptk.h eventually includes X11/Xlib.h which gets the C++ string.h,
 * and the extern C kills it, so include it first and it protects
 * itself.
 */
#include <X11/Xlib.h>
#endif /* PTSVR4 */

extern "C" {
#include "ptk.h"
}

#include "SimAction.h"

int runEventsOnTimer ()
{
	int sec =0; 
	int usec = 50000;  // every 50,000 micro seconds = 20 times/sec
	// Process all pending events 
        while (Tk_DoOneEvent(TK_DONT_WAIT|TK_ALL_EVENTS));
	// Reset the Timer so that it will fire again
	SimControl::setPollTimer( sec, usec);
	return TRUE;
}

